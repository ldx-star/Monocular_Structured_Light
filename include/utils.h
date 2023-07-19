//
// Created by liangdaxin on 23-6-28.
//

#ifndef BINOCULAR_STRUCTURED_LIGHT_SYSTEM_UTILS_H
#define BINOCULAR_STRUCTURED_LIGHT_SYSTEM_UTILS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <type_traits>

#include "types.h"


class utils {
public:
    static void im_read(const std::vector<std::string> &img_fileNames, std::vector<cv::Mat> &imgs, int color_model = 1);

    static bool
    detectCenters(const cv::Mat &img, std::vector<cv::Point2f> &centers_xy, const cv::Size &patSize, bool show = true);

    static bool makedir(const std::string &path);

    static std::string extract_num(const std::string &str);

    static bool is_includeNum(const std::string &line);

    static cv::Mat MedianFilter(const cv::Mat &img, const int &window_size);

    static void plot_wrapped_pha(cv::Mat pha);

    //去掉转义字符
    static void delete_escape(std::string &line);

    static std::string path_join(const std::string &path1, const int &path2);
    static std::string path_join(const std::string &path1, const std::string &path2);

    static void glob(const std::string& filePath,std::vector<std::string>& img_fileNames);

};


#endif //BINOCULAR_STRUCTURED_LIGHT_SYSTEM_UTILS_H
