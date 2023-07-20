//
// Created by liangdaxin on 23-6-29.
//
#include "../include/utils.h"
#include "../include/Phase.h"

void Phase::cal_absolute_phase() {
    int height = img_rows_;
    int width = img_cols_;
    auto &Pha = Pha_;
    auto &B = B_;
    const auto &Order1 = Order1_;
    const auto &Order2 = Order2_;
    auto &V1_K1 = V1_K1_;
    auto &V2_K2 = V2_K2_;


    //获得投影图片
    std::vector<std::string> img_fileNames;

    utils::glob(Project_imgs_path_, img_fileNames);


    std::vector<cv::Mat> imgs;
    utils::im_read(img_fileNames, imgs, 0);
    //获取相移条纹投影图案和格雷码投影图案
    // 17-24 格雷码 25 互补格雷码 26-33 相移条纹 34 纯黑 35 纯白
    std::vector<cv::Mat> shift_imgs, gray_code_imgs;
    for (int i = 0; i < 36; i++) {
        if (i >= 26 && i <= 33) {
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
    //解包裹相位
    cal_wrapped_phase(shift_imgs, shift_imgs.size());
    //解格雷码
    cal_gray_code(gray_code_imgs, gray_code_imgs.size());
    //解绝对相位
    /*              pha(x,y) + 2*pi*k2(x,y)         if pha(x,y) <= -pi/2
     *  Pha(x,y) =  pha(x,y) + 2*pi*k1(x,y)         if pha(x,y) < pi/2
     *              pha(x,y) + 2*pi*k2(x,y)-2*pi    if pha(x,y) >= pi/2
     *
     */
    int max_order = 0;
    int count = 0;
    int order = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double &pha = Pha.at<double>(i, j);
            if(pha <= 0){
                continue;
            }
            if (pha <= M_PI / 2) {
                order = Order2.at<int>(i, j);
                int v2_k2 = V2_K2[order];
                Pha.at<double>(i, j) = pha + 2.0 * M_PI * V2_K2[order];
                max_order = max_order > V2_K2[order]?max_order:V2_K2[order];
                if(V2_K2[order] > 152){
                    count++;
                }
            } else if (pha >= 3 * M_PI / 2) {
                order = Order2.at<int>(i, j);
                int v2_k2 = V2_K2[order]-1;
                Pha.at<double>(i, j) = pha + 2.0 * M_PI * (V2_K2[order] - 1);
                max_order = max_order > V2_K2[order]?max_order:V2_K2[order];
                if(V2_K2[order] > 152){
                    count++;
                }

            } else {
                order = Order1.at<int>(i, j);
                int v1_k1 = V1_K1[order];
                Pha.at<double>(i, j) = pha + 2.0 * M_PI * (V1_K1[order]);
                max_order = max_order > V1_K1[order]?max_order:V1_K1[order];
                if(V1_K1[order] > 152){
                    count++;
                }

            }
        }
        int a = 10;

    }


}

void Phase::cal_gray_code(std::vector<cv::Mat> imgs, unsigned int N) {
    assert(!imgs.empty());

    int height = imgs[0].rows;
    int width = imgs[0].cols;

    cv::Mat &Order1 = Order1_;
    cv::Mat &Order2 = Order2_;
    Order1 = cv::Mat::zeros(height, width, CV_32S);
    Order2 = cv::Mat::zeros(height, width, CV_32S);
    //二值化
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            double max = 0, min = m_MAX_FLOAT;
            //找最大值和最小值
            for (int k = 0; k < N; k++) {
                double value = imgs[k].at<double>(i, j);
                max = max > value ? max : value;
                min = min < value ? min : value;
            }
            //通过最大值最小值做二值化
            // (value - value_min) / (value_max -value_min)
            for (int k = 0; k < N; k++) {
                double value = imgs[k].at<double>(i, j);
                double std = (value - min) / (max - min);
                imgs[k].at<double>(i, j) = std > grayCode_threshold_ ? 1 : 0;
            }

            //制作阶次图
            //头两张为纯黑、纯白图，不要
            //不含互补格雷码

            //尾两张时纯黑纯白图，不要
            int order = 0;
            for (int k = 0; k < N - 3; k++) {
                double value = imgs[k].at<double>(i, j);
                order += int(value * pow(2, N - 3 - 1 - k));
            }
            Order1.at<int>(i, j) = order;
            //含互补格雷码
            order = 0;
            for (int k = 0; k < N - 2; k++) {
                double value = imgs[k].at<double>(i, j);
                order += int(value * pow(2, N - 2 - 1 - k));
            }
            Order2.at<int>(i, j) = order;
        }
    }

    //建立V1->K1映射
    //不含互补格雷码
    std::vector<int> codes1 = gray_code(num_gray_code_);
    std::unordered_map<int, int> map1;
    for (int i = 0; i < codes1.size(); i++) {
        std::pair<int, int> pair{codes1[i], i};
        V1_K1_.insert(pair);
    }
    //建立V2->K2映射
    //含互补格雷码
    std::vector<int> codes2 = gray_code(num_gray_code_+1);
    //0->0  1,2->1 3,4->2...
    int count = 0;
    std::pair<int, int> pair{codes2[0], count++};
    V2_K2_.insert(pair);
    for (int i = 1; i < codes2.size(); i++) {
        pair = {codes2[i], count};
        if (i  % 2  == 0) {
            count++;
        }
        V2_K2_.insert(pair);
    }

}

std::vector<int> Phase::gray_code(int n) {
    int num = std::pow(2, n);
    std::vector<int> codes(num);
    codes[0] = 0;
    int times = 1;
    while (times < num) {
        int code = codes[times - 1];
        //改变最右位的值
        if ((code & 1) == 1) {
            //最右位为1，将它变成0
            code -= 1;
        } else {
            code += 1;
        }
        codes[times++] = code;
        //改变从右起第一个为1的左边那一位
        code = codes[times - 1];
        int a = 1;
        while ((code & a) == 0) {
            a <<= 1;
        }
        a <<= 1;
        if ((code & a) != 0) {
            //为1，将它变成0
            code -= a;
        } else {
            code += a;
        }
        codes[times++] = code;
    }
    return codes;
}

void Phase::cal_wrapped_phase(std::vector<cv::Mat> imgs, unsigned int N) {
    cv::Mat sin_sum = cv::Mat::zeros(imgs[1].size(), CV_64F);
    cv::Mat cos_sum = cv::Mat::zeros(imgs[1].size(), CV_64F);
    for (int i = 0; i < N; i++) {
        cv::Mat img = imgs[i];
        cv::GaussianBlur(img, img, cv::Size(3, 3), 1);
        sin_sum = sin_sum + img * std::sin(2 * i * M_PI / N);
        cos_sum = cos_sum + img * std::cos(2 * i * M_PI / N);
    }

////    cv::imwrite("../sin_sum.tiff",sin_sum);
////    cv::imwrite("../cos_sum.tiff",cos_sum);
//
//    sin_sum = cv::imread("../n_9.tiff",cv::IMREAD_UNCHANGED);
//    cos_sum = cv::imread("../d_9.tiff",cv::IMREAD_UNCHANGED);
//    cv::Mat ground = cv::imread("../ground.tiff",cv::IMREAD_UNCHANGED);
//    sin_sum = cv::imread("../numerator.tiff",cv::IMREAD_UNCHANGED);
//    cos_sum = cv::imread("../denominator.tiff",cv::IMREAD_UNCHANGED);
////
////    sin_sum.convertTo(sin_sum,CV_64F);
////    cos_sum.convertTo(cos_sum,CV_64F);



    Pha_ = cv::Mat::zeros(imgs[1].size(), CV_64F);
    B_ = cv::Mat::zeros(imgs[1].size(), CV_64F);
    for (int i = 0; i < Pha_.rows; i++) {
        for (int j = 0; j < Pha_.cols; j++) {
            double pha_value = -1 * atan2(sin_sum.at<double>(i, j), cos_sum.at<double>(i, j));

            double b_value = sqrt(sin_sum.at<double>(i, j) * sin_sum.at<double>(i, j) +
                                  cos_sum.at<double>(i, j) * cos_sum.at<double>(i, j)) * 2 / N;
            //过滤异常值
            B_.at<double>(i,j) = b_value;
            if(b_value < B_min_){
                Pha_.at<double>(i, j) = 0;
                continue;
            }
            if (pha_value < 0) {
                Pha_.at<double>(i, j) = pha_value + 2 * M_PI;
            } else {
                Pha_.at<double>(i, j) = pha_value;
            }
        }
    }
}