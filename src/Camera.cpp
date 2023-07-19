//
// Created by liangdaxin on 23-6-28.
//

#include "../include/Camera.h"
#include "../include/utils.h"
#include "../include/types.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>



//void Camera::save_params() {
//    std::cout << "=========保存参数===========" << std::endl;
//
//    utils::makedir(camera_param_path_);
//
//    std::fstream file;
//    file.open(camera_param_path_ + "/" + camera_param_file_,std::ios::out);
//
//    if(file.is_open()){
//        file<<"M_L:\n";
//        file<<M_L_<<std::endl;
//
//        file<<"M_R:\n";
//        file<<M_R_<<std::endl;
//
//        file<<"D_L:\n";
//        file<<D_L_<<std::endl;
//
//        file<<"D_R:\n";
//        file<<D_R_<<std::endl;
//
//        file<<"R:\n";
//        file<<R_<<std::endl;
//
//        file<<"T:\n";
//        file<<T_<<std::endl;
//
//        file<<"E:\n";
//        file<<E_<<std::endl;
//
//        file<<"F:\n";
//        file<<F_<<std::endl;
//
//        file<<"error_L:\n";
//        file<<error_L_<<std::endl;
//
//        file<<"error_R:\n";
//        file<<error_R_<<std::endl;
//
//        file<<"error_LR:\n";
//        file<<error_LR_<<std::endl;
//    }
//
//
//    file.close();
//
//    std::cout << "save params success" << std::endl;
//
//
//}

void Camera::load_params() {
    std::cout << "\n=========camera params loading===========\n" << std::endl;

    std::fstream file;
    file.open(camera_param_file_,std::ios::in);
    if(!file.is_open()){
        std::cout<< camera_param_file_ +  "open failed"<<std::endl;
    }

    std::string title;
    std::string data;
    std::string line;
    std::vector<double>numbers;

    while(!file.eof()){
        std::getline(file,line);
        //linux 下会读回车（\r）而windows却不会
        //为了实现跨平台编译，需统一格式
        utils::delete_escape(line);
        if(utils::is_includeNum(line)){
            data = line;
        }else{
            if(!numbers.empty()){
                if(title == "m_k"){
                    cv::Mat mat(3,3,CV_64F,numbers.data());
                    M_ = mat.clone();
                }else if(title == "m_discoeff"){
                    cv::Mat mat(1,5,CV_64F,numbers.data());
                    D_ = mat.clone();
                }else if(title == "m_rotMatrix"){
                    cv::Mat mat(3,3,CV_64F,numbers.data());
                    Rotation_ = mat.clone();
                }else if(title == "m_transMatrix"){
                    cv::Mat mat(3,1,CV_64F,numbers.data());
                    Translation_ = mat.clone();
                }
            }
            title = line;
            numbers.clear();
            continue;
        }
        std::istringstream iss(data); // 使用字符串流读取输入
        std::string word;
        while (iss >> word) {
            // 尝试将每个字符串转换为数值
            std::istringstream wordIss(word);

            std::string num = utils::extract_num(word);
            double number = std::stod(num);
            numbers.emplace_back(number);
        }
    }
    file.close();
    std::cout<<"============load camera params succeeded==========="<<std::endl;
}

void Camera::printInfo() {
    std::cout << "\n==========print camera params=================\n";
    std::cout<< std::endl;

    std::cout<< "M_:" << std::endl;
    std::cout<< M_<< std::endl;
    std::cout<< std::endl;

    std::cout<< "D_:" << std::endl;
    std::cout<< D_ << std::endl;
    std::cout<< std::endl;

    std::cout<< "Rotation_:" << std::endl;
    std::cout<< Rotation_ << std::endl;
    std::cout<< std::endl;

    std::cout<< "Translation_:" << std::endl;
    std::cout<< Translation_ << std::endl;
    std::cout<< std::endl;

}

