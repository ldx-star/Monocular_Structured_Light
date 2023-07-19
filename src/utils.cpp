//
// Created by liangdaxin on 23-6-28.
//

#include "../include/utils.h"
#include <filesystem>


void utils::im_read(const std::vector<std::string> &img_fileNames, std::vector<cv::Mat> &imgs, int color_model) {
    cv::Mat img;

    for (int i = 0; i < img_fileNames.size(); i++) {
        const std::string fileName = img_fileNames[i];
        img = cv::imread(fileName, color_model);
        if (img.data == nullptr) {
            std::cout << "棋盘格读取错误" << std::endl;
            exit(1);
        }
        img.convertTo(img, CV_64F);
        imgs.emplace_back(img);
    }
}

void utils::plot_wrapped_pha(cv::Mat pha) {

    auto row_ptr = pha.ptr<double>(1);
    auto image = cv::Mat::zeros(2500, 2500, CV_64F);
    std::vector<cv::Point2f> points;
    for (int i = 0; i < pha.cols; i++) {
        points[i].x = i;
        points[i].y = row_ptr[i];
    }

}

void utils::delete_escape(std::string &line) {
    std::string str;
    for (auto e: line) {
        if (e != '\r' && e != '\n') {
            str += e;
        } else {
            break;
        }
    }
    line = str;
}

bool utils::makedir(const std::string &path) {
    if (std::filesystem::is_directory(path)) {
        std::cout << "file exited" << std::endl;
    } else {
        std::filesystem::create_directories(path);
        if (std::filesystem::is_directory(path)) {
            std::cout << "file create succeed" << std::endl;
        } else {
            std::cout << "file create succeed" << std::endl;
            return false;
        }
    }
    return true;
}

std::string utils::extract_num(const std::string &str) {
    std::string data;
    for (auto e: str) {
        if (e >= '0' && e <= '9' || e == '.' || e == '-' || e == 'e') {
            data += e;
        }
    }
    return data;
}

bool utils::is_includeNum(const std::string &line) {
    for (const auto &e: line)
        if (e >= '0' && e <= '9') {
            return true;
        }
    return false;
}

cv::Mat utils::MedianFilter(const cv::Mat &img, const int &window_size) {
    int radius = window_size / 2;
    unsigned int size = window_size * window_size;
    int height = img.cols;
    int width = img.rows;
    cv::Mat out = cv::Mat::zeros(img.size(), CV_8U);

    std::vector<int> window(size);
    for (int i = 0; i < img.cols; i++) {
        for (int j = 0; j < img.rows; j++) {
            window.clear();
            for (int r = -radius; r <= radius; r++) {
                for (int c = -radius; c <= radius; c++) {
                    int row = i + r;
                    int col = j + c;
                    if (row >= 0 && row < height && col >= 0 && col < width) {
                        window.emplace_back(img.at<uint8_t>(row, col));
                    }
                }
            }
            //排序
            std::sort(window.begin(), window.end());
            //取中值
            unsigned int mid = size / 2;
            out.at<uint8_t>(i, j) = window[mid];
        }
    }
    return out;
}

std::string utils::path_join(const std::string &path1, const int &path2)  {
    std::string ret;
    ret = path1 + "/" + std::to_string(path2);
    return ret;
}
std::string utils::path_join(const std::string &path1, const std::string &path2) {
    std::string ret;
    return path1 + "/" + path2;
}

void utils::glob(const std::string &filePath, std::vector<std::string> &img_fileNames) {
    for(int i = 0; i < 36; i++){
        std::string fileName = filePath + "/" + std::to_string(i) + ".bmp";
        img_fileNames.emplace_back(fileName);
    }
}