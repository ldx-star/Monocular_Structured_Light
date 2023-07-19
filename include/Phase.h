//
// Created by liangdaxin on 23-6-29.
//

#ifndef BINOCULAR_STRUCTURED_LIGHT_SYSTEM_PHASE_H
#define BINOCULAR_STRUCTURED_LIGHT_SYSTEM_PHASE_H
#include <iostream>
#include <opencv2/opencv.hpp>

class Phase {
public:
    int img_rows_;
    int img_cols_;

    cv::Mat Pha_;//绝对相位
    cv::Mat B_;//调制度
    int num_shift_; // 相移步数
    int num_gray_code_;//格雷码个数

    std::string Project_imgs_path_;//投影图片路径
    double grayCode_threshold_; // 格雷码阈值
    double B_min_; //调制度阈值
    int window_size_; // 中滤波窗口
public:
    void cal_absolute_phase();
    //获取格雷码
    std::vector<int> gray_code(int n);
public:
    void cal_wrapped_phase(std::vector<cv::Mat> imgs, unsigned int N);
    void cal_gray_code(std::vector<cv::Mat> imgs, unsigned int N);
public:
    std::unordered_map<int,int> V1_K1_;//V1->K1的映射，不含互补格雷码
    std::unordered_map<int,int> V2_K2_;//V2->K2的映射，含互补格雷码
    cv::Mat Order1_; //不含互补格雷码
    cv::Mat Order2_; //含互补格雷码
};


#endif //BINOCULAR_STRUCTURED_LIGHT_SYSTEM_PHASE_H
