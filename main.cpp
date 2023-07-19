//
// Created by liangdaxin on 23-6-28.
//
#include "include/Camera.h"
#include "include/types.h"
#include <opencv2/opencv.hpp>
#include <chrono>
#include "include/Phase.h"
#include "include/Project.h"
#include "include/Build.h"
#include "include/utils.h"

void phase_shifting_and_gray_code(const Config &config) {
    /// 加载相机参数
    Camera camera = Camera();
    camera.camera_param_file_ = config.camera_param_file;
    camera.load_params();
    camera.printInfo();

    /// 加载投影仪参数
    Project project = Project();
    project.project_width_ = config.project_width;
    project.project_param_file_ = config.project_param_file;
    project.load_params();
    project.printInfo();


    ///解相位
    Phase phase = Phase();
    phase.img_cols_ = config.img_cols;
    phase.img_rows_ = config.img_rows;

    phase.window_size_ = config.window_size;
    phase.Project_imgs_path_ = config.Project_imgs_path;
    phase.grayCode_threshold_ = config.grayCode_threshold;
    phase.B_min_ = config.B_min;
    phase.num_shift_ = config.num_shift;
    phase.num_gray_code_ = config.num_gray_code;

    //计算绝对相位
    phase.cal_absolute_phase();
    ///重建
    std::cout << "===========start rebuild=================" << std::endl;
    Build build(camera, project, phase);
    build.save_path_ = config.save_path;
    build.reconstruction();
    build.save_cloud_points();
    std::cout << "===========start rebuild=================" << std::endl;

}

void get(std::string img_path, Phase phase, std::vector<cv::Mat> &origin, std::vector<cv::Mat> &shifts, cv::Mat &ground,
         cv::Mat &numerator,
         cv::Mat &denominator,
         cv::Mat &wrapped_phase, cv::Mat &absolute_phase) {
    origin.clear();shifts.clear();
    int height = phase.img_rows_;
    int width = phase.img_cols_;
    auto &Pha = phase.Pha_;
    const auto &Order1 = phase.Order1_;
    const auto &Order2 = phase.Order2_;
    auto &V1_K1 = phase.V1_K1_;
    auto &V2_K2 = phase.V2_K2_;

    std::vector<std::string> img_fileNames;
    utils::glob(img_path, img_fileNames);
    std::vector<cv::Mat> imgs;
    utils::im_read(img_fileNames, imgs, 0);
    //获取相移条纹投影图案和格雷码投影图案
    // 17-24 格雷码 25 互补格雷码 26-33 相移条纹 34 纯黑 35 纯白
    std::vector<cv::Mat> shift_imgs, gray_code_imgs;
    for (int i = 0; i < 36; i++) {
        origin.emplace_back(imgs[i].clone());
        if (i >= 26 && i <= 33) {
            cv::Mat img = imgs[i].clone();
            //相移条纹
            shift_imgs.emplace_back(imgs[i]);
        }
        if (i >= 17 && i <= 25) {
            gray_code_imgs.emplace_back(imgs[i]);
        }
        if (i == 34 || i == 35) {
            gray_code_imgs.emplace_back(imgs[i]);
        }
    }
    //shifts
    shifts = shift_imgs;

    //numerator denominator
    cv::Mat sin_sum = cv::Mat::zeros(shift_imgs[1].size(), CV_64F);
    cv::Mat cos_sum = cv::Mat::zeros(shift_imgs[1].size(), CV_64F);
    cv::Mat ground_img = cv::Mat::zeros(shift_imgs[1].size(), CV_64F);
    for (int i = 0; i < 8; i++) {
        cv::Mat img = shift_imgs[i].clone();
        ground_img += img;
        cv::GaussianBlur(img, img, cv::Size(3, 3), 1);
        sin_sum = sin_sum + img * std::sin(2 * i * M_PI / 8);
        cos_sum = cos_sum + img * std::cos(2 * i * M_PI / 8);
    }
    numerator = sin_sum;
    denominator = cos_sum;

    //ground
    ground = ground_img / 8;

    //解包裹相位
    phase.cal_wrapped_phase(shift_imgs, shift_imgs.size());
    wrapped_phase = Pha.clone();
    //解格雷码
    phase.cal_gray_code(gray_code_imgs, gray_code_imgs.size());
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double pha = Pha.at<double>(i, j);
            if(pha <= 0){
                continue;
            }
            if (pha <= M_PI / 2) {
                Pha.at<double>(i, j) = pha + 2.0 * M_PI * V2_K2[Order2.at<int>(i, j)];
            } else if (pha >= 3 * M_PI / 2) {
                Pha.at<double>(i, j) = pha + 2.0 * M_PI * (V2_K2[Order2.at<int>(i, j)] - 1);
            } else {
                Pha.at<double>(i, j) = pha + 2.0 * M_PI * (V1_K1[Order1.at<int>(i, j)]);
            }
        }
    }
    absolute_phase = cv::Mat::zeros(height, width, CV_64F);
    absolute_phase = Pha.clone();
}

void makeDatasets(Config config, int num_files) {
    Phase phase;
    phase.img_cols_ = config.img_cols;
    phase.img_rows_ = config.img_rows;

    phase.window_size_ = config.window_size;
    phase.Project_imgs_path_ = config.Project_imgs_path;
    phase.grayCode_threshold_ = config.grayCode_threshold;
    phase.B_min_ = config.B_min;
    phase.num_shift_ = config.num_shift;
    phase.num_gray_code_ = config.num_gray_code;

    int count = 0;
    std::string src = "/home/ldx/temp_folder/datasets/cal_wrapped_phase";
    std::string des = "/home/ldx/temp_folder/datasets/cal_wrapped_phase/datasets";
    for (int i = 0; i < num_files; i++) {
        //左
        std::string path = utils::path_join(src, i);
        std::string imgPath = utils::path_join(path, "Left");
        std::string des_path = utils::path_join(des, count++);
        cv::Mat ground, numerator, denominator, wrapped_phase, absolute_phase;
        std::vector<cv::Mat> shifts, origin;
        std::string shifts_folder = utils::path_join(des_path, "shifts");
        std::string ground_folder = utils::path_join(des_path, "ground");
        std::string numerator_folder = utils::path_join(des_path, "numerator");
        std::string denominator_folder = utils::path_join(des_path, "denominator");
        std::string wrapped_folder = utils::path_join(des_path, "wrapped");
        std::string absolute_folder = utils::path_join(des_path, "absolute");
        std::string origin_path = utils::path_join(des_path, "Left");
        utils::makedir(shifts_folder);
        utils::makedir(ground_folder);
        utils::makedir(numerator_folder);
        utils::makedir(denominator_folder);
        utils::makedir(wrapped_folder);
        utils::makedir(absolute_folder);
        utils::makedir(origin_path);
        get(imgPath, phase, origin, shifts, ground, numerator, denominator, wrapped_phase, absolute_phase);
        for (int j = 0; j < 36; j++) {
            if(j < 8){
                cv::imwrite(utils::path_join(shifts_folder, j) + ".bmp", shifts[j]);
            }
            cv::imwrite(utils::path_join(origin_path, j) + ".bmp", origin[j]);

        }
        cv::imwrite(utils::path_join(ground_folder, "ground.tiff"), ground);
        cv::imwrite(utils::path_join(numerator_folder, "numerator.tiff"), numerator);
        cv::imwrite(utils::path_join(denominator_folder, "denominator.tiff"), denominator);
        cv::imwrite(utils::path_join(wrapped_folder, "wrapped.tiff"), wrapped_phase);
        cv::imwrite(utils::path_join(absolute_folder, "absolute.tiff"), absolute_phase);

        // 右
        path = utils::path_join(src, i);
        imgPath = utils::path_join(path, "Right");
        des_path = utils::path_join(des, count++);

        shifts_folder = utils::path_join(des_path, "shifts");
        ground_folder = utils::path_join(des_path, "ground");
        numerator_folder = utils::path_join(des_path, "numerator");
        denominator_folder = utils::path_join(des_path, "denominator");
        wrapped_folder = utils::path_join(des_path, "wrapped");
        absolute_folder = utils::path_join(des_path, "absolute");
        origin_path = utils::path_join(des_path, "Right");
        utils::makedir(shifts_folder);
        utils::makedir(ground_folder);
        utils::makedir(numerator_folder);
        utils::makedir(denominator_folder);
        utils::makedir(wrapped_folder);
        utils::makedir(absolute_folder);
        utils::makedir(origin_path);
        get(imgPath, phase, origin,shifts, ground, numerator, denominator, wrapped_phase, absolute_phase);
        for (int j = 0; j < 36; j++) {
            if(j < 8){
                cv::imwrite(utils::path_join(shifts_folder, j) + ".bmp", shifts[j]);
            }
            cv::imwrite(utils::path_join(origin_path, j) + ".bmp", origin[j]);
            cv::Mat des_img = cv::imread(utils::path_join(origin_path, j) + ".bmp",0);
            des_img.convertTo(des_img,CV_64F);
        }
        cv::imwrite(utils::path_join(ground_folder, "ground.tiff"), ground);
        cv::imwrite(utils::path_join(numerator_folder, "numerator.tiff"), numerator);
        cv::imwrite(utils::path_join(denominator_folder, "denominator.tiff"), denominator);
        cv::imwrite(utils::path_join(wrapped_folder, "wrapped.tiff"), wrapped_phase);
        cv::imwrite(utils::path_join(absolute_folder, "absolute.tiff"), absolute_phase);

    }

}



int main() {
    Config config;
    config.img_rows = 2048;
    config.img_cols = 2448;

    //相机标定参数
    config.camera_param_file = "../calibrated_parameters/rightCam.ini";
    //投影仪标定参数
    config.project_param_file = "../calibrated_parameters/rightProj.ini";

    ///解相位参数
    //相移条纹
    config.num_shift = 8; // 相移步数
    config.Project_imgs_path = "../project_imgs";//投影图片路径
    config.grayCode_threshold = 0.5;
    config.B_min = 4;
    config.window_size = 7;
    config.project_width = 912;
    //格雷码
    config.num_gray_code = 8;//不算纯白和纯黑的图


    makeDatasets(config, 72);

//
//    Config config;
//
//    config.img_rows = 2048;
//    config.img_cols = 2448;
//
//    //相机标定参数
//    config.camera_param_file  = "../calibrated_parameters/rightCam.ini";
//    //投影仪标定参数
//    config.project_param_file  = "../calibrated_parameters/rightProj.ini";
//
//    ///解相位参数
//    //相移条纹
//    config.num_shift = 8; // 相移步数
//    config.Project_imgs_path = "../project_imgs";//投影图片路径
//    config.grayCode_threshold = 0.5;
//    config.B_min = 6;
//    config.window_size = 7;
//    config.project_width =912;
//    //格雷码
//    config.num_gray_code = 8;//不算纯白和纯黑的图
//
//    config.save_path = "../out";
//
//    phase_shifting_and_gray_code(config);
//
    return 0;
}