//
// Created by liangdaxin on 23-7-1.
//

#include "../include/Project.h"
#include "../include/utils.h"
#include <fstream>
#include <vector>

void Project::load_params() {
    std::cout << "\n=========projector params loading===========\n" << std::endl;

    std::fstream file;
    file.open(project_param_file_,std::ios::in);
    if(!file.is_open()){
        std::cout<< project_param_file_+  "open failed"<<std::endl;
    }

    std::string title;
    std::string data;
    std::string line;
    std::vector<double> numbers;

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
    std::cout<<"============load projector params succeeded=========="<<std::endl;
}

void Project::printInfo() {
    std::cout << "\n==========print projector params=================\n";
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

