//
// Created by liangdaxin on 23-7-1.
//

#ifndef TRIANGULAR_STEREO_MODEL_PROJECT_H
#define TRIANGULAR_STEREO_MODEL_PROJECT_H
#include <iostream>
#include <opencv2/opencv.hpp>

class Project {
public:
    std::string project_param_file_ ; // 投影仪参数文件

    cv::Mat M_;     // 内参矩阵
    cv::Mat D_;      // 畸变系数
    cv::Mat Rotation_;  // 旋转矩阵
    cv::Mat Translation_; // 平移矩阵

    int project_width_;

public:
    void load_params();// 加载投影仪参数
    void printInfo();//打印标定信息
};


#endif //TRIANGULAR_STEREO_MODEL_PROJECT_H
