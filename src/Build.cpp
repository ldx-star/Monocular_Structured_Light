//
// Created by liangdaxin on 23-7-2.
//

#include "../include/Build.h"
#include "../include/utils.h"
#include <opencv2/opencv.hpp>
#include <fstream>

Build::Build(const Camera &camera, const Project &project, const Phase &phase) : camera_(camera), project_(project),
                                                                                 phase_(phase) {}

void Build::reconstruction() {
    int height = phase_.img_rows_;
    int width = phase_.img_cols_;
    cv::Mat RT;
    cv::hconcat(camera_.Rotation_, camera_.Translation_, RT);
    cv::Mat Pc = camera_.M_ * RT;
    cv::hconcat(project_.Rotation_, project_.Translation_, RT);
    cv::Mat Pp = project_.M_ * RT;
    const cv::Mat &Pha = phase_.Pha_;
    std::vector<cv::Point3f> &cloud_points = cloud_points_;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (Pha.at<double>(i, j) <= 0) {
                continue;
            }
            int u = j;
            int v = i;
            int p = Pha.at<double>(i, j) * project_.project_width_ / (2 * M_PI * 151) - 1;
            cv::Point3f point;
            double A_data[] = {
                    Pc.at<double>(0, 0) - u * Pc.at<double>(2, 0), Pc.at<double>(0, 1) - u * Pc.at<double>(2, 1),
                    Pc.at<double>(0, 2) - u * Pc.at<double>(2, 2), Pc.at<double>(1, 0) - v * Pc.at<double>(2, 0),
                    Pc.at<double>(1, 1) - v * Pc.at<double>(2, 1), Pc.at<double>(1, 2) - v * Pc.at<double>(2, 2),
                    Pp.at<double>(0, 0) - p * Pc.at<double>(2, 0), Pp.at<double>(0, 1) - p * Pc.at<double>(2, 1),
                    Pp.at<double>(0, 2) - p * Pc.at<double>(2, 2)
            };
            cv::Mat A = cv::Mat(3, 3, CV_64F, A_data);
            double B_data[] = {
                    u * Pc.at<double>(2, 3) - Pc.at<double>(0, 3),
                    v * Pc.at<double>(2, 3) - Pc.at<double>(1, 3),
                    p * Pp.at<double>(2, 3) - Pp.at<double>(0, 3)
            };
            cv::Mat B = cv::Mat(3, 1, CV_64F, B_data);
            cv::Mat inverse_A;
            cv::invert(A, inverse_A);
            cv::Mat output;

            cv::gemm(inverse_A, B, 1.0, cv::Mat(), 0.0, output);
            point.x = output.at<double>(0, 0);
            point.y = output.at<double>(0, 1);
            point.z = output.at<double>(0, 2);
            cloud_points.emplace_back(point);
        }
    }
}

void Build::save_cloud_points() {
    std::fstream file;
    std::string path = save_path_ + "/cloud_points";
    auto &cloud_points = cloud_points_;
    utils::makedir(path);
    file.open(path + "/cloud_points.txt", std::ios::out);
    for (int i = 0; i < cloud_points.size(); i++) {

        auto x = cloud_points[i].x;
        auto y = cloud_points[i].y;
        auto z = cloud_points[i].z;
        file << x << "\t" << y << "\t" << z << "\t" << std::endl;
    }
}