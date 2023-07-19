//
// Created by liangdaxin on 23-7-2.
//

#ifndef TRIANGULAR_STEREO_MODEL_BUILD_H
#define TRIANGULAR_STEREO_MODEL_BUILD_H
#include "Camera.h"
#include "Project.h"
#include "Phase.h"
#include "types.h"
class Build {
public:
    Camera camera_;
    Project project_;
    Phase phase_;
    std::vector<cv::Point3f> cloud_points_;
    std::string save_path_;
public:
    Build(const Camera& camera, const Project& project, const Phase& phase);
    void reconstruction();
    void save_cloud_points();
};


#endif //TRIANGULAR_STEREO_MODEL_BUILD_H
