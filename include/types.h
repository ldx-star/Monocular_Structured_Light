//
// Created by liangdaxin on 23-6-28.
//

#ifndef BINOCULAR_STRUCTURED_LIGHT_SYSTEM_TYPES_H
#define BINOCULAR_STRUCTURED_LIGHT_SYSTEM_TYPES_H
#include <iostream>

#define m_MAX_FLOAT 99999999 //统一windows和Linux
struct Config{
public:
    std::string camera_param_file ; // 相机参数文件
    std::string project_param_file ; // 投影仪参数文件

    int num_shift; // 相移步数
    int num_gray_code;//格雷码个数
    std::string Project_imgs_path;//投影图片路径
    double grayCode_threshold; // 格雷码阈值
    double B_min; //调制度阈值
    int window_size; // 中值滤波窗口
    int img_cols;
    int img_rows;
    int project_width;

    std::string save_path;

};

struct Circle{
    cv::Point2f Center;
    double Radius;
};

#endif //BINOCULAR_STRUCTURED_LIGHT_SYSTEM_TYPES_H
