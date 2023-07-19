//
// Created by liangdaxin on 23-6-28.
//

#ifndef BINOCULAR_STRUCTURED_LIGHT_SYSTEM_CAMERA_H
#define BINOCULAR_STRUCTURED_LIGHT_SYSTEM_CAMERA_H

#include <iostream>
#include <opencv2/opencv.hpp>

class Camera {
public:
    std::string camera_param_file_; // 参数文件

    cv::Mat M_;     // 内参矩阵
    cv::Mat D_;      // 畸变系数
    cv::Mat Rotation_;  // 旋转矩阵
    cv::Mat Translation_; // 平移矩阵

public:
    Camera() = default;
    ~Camera() = default;
    //打印标定信息
    void printInfo();
//    //保存参数
//    void save_params();
    //加载相机参数
    void load_params();
};


#endif //BINOCULAR_STRUCTURED_LIGHT_SYSTEM_CAMERA_H
