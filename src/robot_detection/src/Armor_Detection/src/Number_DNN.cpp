#include "../include/Number_DNN.h"

using namespace cv;
using namespace std;

/**
 *  函数名: DNN_detect
 *  传入: 无
 *  传出: 无
 *  功能: 初始化,读取文件参数
 */
DNN_detect::DNN_detect()
{
    //============================= 参数文件路径 =============================
    std::string package_path = ros::package::getPath("robot_detection");
    FileStorage fs(package_path + "/SetParam_File/Dnn_data.yaml", FileStorage::READ);

    net_path = (string)fs["net_path"];
    input_width = (int)fs["input_width"];
    input_height = (int)fs["input_height"];
    net = dnn::readNetFromONNX(net_path);

    // GPU加速推理(英伟达 NVIDIA)
//    net.setPreferableTarget(dnn::dnn4_v20211004::DNN_TARGET_CUDA_FP16);
//    net.setPreferableBackend(dnn::dnn4_v20211004::DNN_BACKEND_CUDA);
    fs.release();
}

/**
 *  函数名: img_processing
 *  传入: Mat ori_img, std::vector<cv::Mat>& numROIs (ROI图像,存放ROI的容器)
 *  传出: 无
 *  功能: 将传入的ROI图像进行二值化,放入传入的numROIs容器
 */
void DNN_detect::img_processing(Mat ori_img, std::vector<cv::Mat>& numROIs) {
    Mat out_blob;
//    Mat ggg;
//    resize(ori_img,ggg,Size (330,450));
//    imshow("ori_img",ggg);
    cvtColor(ori_img, ori_img, cv::COLOR_RGB2GRAY);
    threshold(ori_img, ori_img, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    numROIs.push_back(ori_img);
}

/**
 *  函数名: net_forward
 *  传入: const std::vector<cv::Mat>& numROIs (存放ROI图像的容器)
 *  传出: Mat outputs
 *  功能: 对numROIs容器内的所有ROI图像推理,返回推理结果,使用 Mat类型存放结果
 */
Mat DNN_detect::net_forward(const std::vector<cv::Mat>& numROIs) {
    //!< opencv dnn supports dynamic batch_size, later modify
    cv::Mat blob;
    dnn::blobFromImages(numROIs, blob, 1.0f/255.0f, Size(input_width, input_height));
    net.setInput(blob);
    Mat outputs = net.forward();
    return outputs;
}

