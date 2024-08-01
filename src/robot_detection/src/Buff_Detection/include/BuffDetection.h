//
// Created by Glb on 2023/9/15.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <Eigen/Eigen>
#include "AngleSolve.h"
#include <opencv2/dnn.hpp>
#include <cmath>

#include "robot_status.h"
#include "robot_struct.h"
#include <ros/package.h>

#define POINT_DIST(p1, p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))


//存储所有矩形的信息结构体
struct candidateRect_Buff {
    float area{};
    cv::RotatedRect rect;
    cv::Point2f Vertex[4];
    float color_sum ;

    candidateRect_Buff(float area_, cv::RotatedRect rect_, cv::Point2f Vertex_[4]) {
        area = area_;
        rect = rect_;
        for (int i = 0; i < 4; i++) {
            Vertex[i] = Vertex_[i];
        }
    }

    candidateRect_Buff() = default;
};


//符叶结构体
struct leaf {
    float angle{};
    float last_angle{};
    cv::Point2f leaf_target[4];
    cv::RotatedRect leaf_RotatedRect;
    cv::Point2f leaf_bottom;
    cv::Point2f leaf_top;

    leaf() = default;
};


//大符 类
class BuffDetection {
public:
    //构造函数
    BuffDetection();

    std::vector<BuffTarget> Detection(cv::Mat &frame);

    /*   公有定义      */
    AngleSolve AS;                   //类对象
    Eigen::Vector3d R_world_pos;     // 世界坐标
    cv::Point2f R_pixel;            // 像素坐标
    int our_color;
    enum BuffColor  { RED = 1 , BLUE = 2 }; 
    int buff_mode;
    void Detection_Init();
    // Logger logger;


    int br_blue_threshold;
    int br_red_threshold;
    int rb_red_threshold;
    int rb_blue_threshold;
    int r_max_area;
    int r_min_area;
    int leaf_max_area;
    int leaf_min_area;
    int leaf_r_max_distance;
    int leaf_r_min_distance;
    float r_min_hb_ratio;
    float r_max_hb_ratio;
    float r_min_hw_ratio;
    float r_max_hw_ratio;

    float leaf_min_hw_ratio;
    float leaf_max_hw_ratio;

    float r_min_are_ratio;
    float r_max_are_ratio;




private:
    //数据输出
    void show(cv::Mat &show_src);

    //图像处理
    void SetImage(const cv::Mat &frame);

    //找轮廓，初级筛选 //然后分流操作
    void findCandidateRect( cv::Mat &images);

    //求旋转角度
    float getAngle(cv::Point2f pt1, cv::Point2f pt2, cv::Point2f pt0, std::string mode);

    //找R标，将初级的矩形进一步筛选
    void findR(candidateRect_Buff &R_rect);

    //确认是否是R标,传入R的结构体
    bool is_R(candidateRect_Buff &R_rect) const;

    
    //寻找符叶
    void findLeaf(candidateRect_Buff &Leaf);

    //判断是否
    bool isLeaf(candidateRect_Buff &Leaf) const;

    //深度学习分类
    bool classifier(cv::Mat &src , size_t id , std::string&ModePath);

    // 放射变换
    cv::Mat warpAndConvertToGray(const cv::Mat& src, const std::vector<cv::Point2f>& Points);
 

    //获取最终目标大符信息（解决两个目标问题）
    void Get_final_buff();
  
 
    //保存照片到本地
    void ImWrite_frame(cv::Mat &Write_image);



    //结构体定义
    candidateRect_Buff R_candidateRect_;
    candidateRect_Buff leaf_candidateRect_;
    BuffTarget buff_target_;
    leaf leaf_;
    





    // R标重新识别
    bool download = false;
    bool sameLeaf = true;
    bool First_R_Inited = true;  // R的第一次识别开关
    bool GET_FIRST_R_POS = false;  // 获取R正确第一次正确识别的R坐标
    bool RInit = false;
    size_t R_ReInited_flag;  // 重新识别的标志
    size_t R_ReInited = 0;
    bool RUpdate = false;

    Eigen::Vector3d FIRST_R_WORLD_POS;     // 世界坐标
    Eigen::Vector3d last_R_world_pos;     // 世界坐标
    

    cv::Point2f R_Point;
    cv::Point2f FIRST_R_PIXEL;
    cv::Point2f now_Point;
    cv::Point2f last_Point;

    cv::Mat binary;
    cv::Mat src_;
 
 

    size_t leaf_classifier_confidence;
    size_t frameCount = 0;

    
    enum LeafModelClass  { FalseLeafClass = 0 , OtherLeafClass = 1 , TrueLeafClass = 2};
    enum HandleModelClass  { FalseHandle = 0 , TrueHandleClass = 1};

 


    //容器定义

    std::vector<BuffTarget> first_buff_target;
    std::vector<BuffTarget> final_buff_target;


    //路径

    std::string LeafModelPath;
    std::string HandleModelPath;
    std::string RModelPath;
    std::string image_write_PATH;


};




