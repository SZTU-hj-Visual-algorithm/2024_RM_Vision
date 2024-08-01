#include <opencv2/opencv.hpp>
#include <iostream>
#include "robot_status.h"
// ROS
#include <ros/package.h>

class DNN_detect{
    std::string net_path;   // onnx模型路径
    int input_width;        // 输入网络宽度
    int input_height;       // 输入网络高度
    cv::dnn::Net net;       // DNN网络
public:
    DNN_detect();           // 初始化
    void img_processing(cv::Mat ori_img, std::vector<cv::Mat>& numROIs);    // 将ROI图像二值化
    cv::Mat net_forward(const std::vector<cv::Mat>& numROIs);       // 对容器内ROI进行推理
};

