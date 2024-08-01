
#ifndef NEURALARMORDETECTOR_NEURAL_ARMOR_DETECTION_H
#define NEURALARMORDETECTOR_NEURAL_ARMOR_DETECTION_H

#include <string>
#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
// use as sam the old Detector head
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"
// openvino
#include <openvino/openvino.hpp>
// ROS
#include <ros/package.h>

#define armor_big_max_wh_ratio 5.0
#define armor_big_min_wh_ratio 3.0
#define armor_small_max_wh_ratio 3.0
#define armor_small_min_wh_ratio 0.8
#define near_standard 500
#define height_standard 25
#define grade_standard 60
//装甲板打分系数比例
#define id_grade_ratio 0.6
#define near_grade_ratio 0.2
#define height_grade_ratio 0.2

// 配置文件
struct Config {
	float confThreshold;
	float nmsThreshold;
	float scoreThreshold;
	int inpWidth;
	int inpHeight;
	std::string onnx_path;
};

// 图像切割大小 
struct Resize
{
	cv::Mat resized_image;
	int dw;
	int dh;
};

// 识别部分
struct Detection {
	int class_id;
	float confidence;
	cv::Rect box;
    std::vector<cv::Point2f> keyPoint;
};

// 神经网络
class NeuralArmorDetector {
public:
	NeuralArmorDetector();
	~NeuralArmorDetector();
    std::vector<Armor> detect(cv::Mat& frame);
	int enemy_color;				// 选择击打颜色 | 1:红 2:蓝
private:
	/** 模型置信度 */
	float confThreshold;			// 筛选的置信度
	float nmsThreshold;				// 进行NMS时使用的阈值
	float scoreThreshold;			// NMS时使用的置信度

	/** 输入图像大小 */
	int inpWidth;					// 图像大小(640x640)
	int inpHeight;					// 图像大小(640x640)

	/** 模型识别class范围 */
	int detectClassStartIndex; 	    // 模型筛选颜色的起点
    int detectRange;				// 模型识别的class范围
    int detectPointIndex;			// 模型输出解析kpt的索引,它的值 = 模型的输出 + 4, 旧版模型是36,新版模型是27,所以值要么是40,要么是31
	float rx;   					// 原始图像和调整大小的图像的宽度比
	float ry;   					// 原始图像和调整大小的图像的高度比

    //
    int dx;
    int dy;

    // frame size
    int frame_w;
    int frame_h;

	/** 模型路径 */
	std::string onnx_path;			// onnx模型路径
    std::string model_cache;		// 编译模型缓存路径
	Resize resize;

	// openvino
	ov::Tensor input_tensor;
	ov::InferRequest infer_request;
	ov::CompiledModel compiled_model;


	// 模型初始化
	void initial_model();							
	// 图像预处理
    void preprocess_img_letterBox(cv::Mat& frame);
	// 图像处理
    std::vector<Armor> postprocess_img(cv::Mat& frame, float * detections, ov::Shape & output_shape);
	// 装甲板打分
    std::vector<Armor> neuralArmorGrade(const std::vector<Detection>& candidateArmors);


	// 模型的检测结果可视化
    void drawDetections(cv::Mat& frame, const std::vector<Detection>& detections);
	// 绘制装甲板
    void drawArmors(cv::Mat &frame, const std::vector<Armor> &armors);	
};



#endif //NEURALARMORDETECTOR_NEURAL_ARMOR_DETECTION_H
