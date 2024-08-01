#ifndef ARMOR_DETECTION_H
#define ARMOR_DETECTION_H

#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"
#include "Number_DNN.h"


//灯条结构体
//struct Light : public cv::RotatedRect     //灯条结构体
//{
//    Light() = default;
//    explicit Light(cv::RotatedRect &box) : cv::RotatedRect(box) //列表初始化
//    {
//        cv::Point2f p[4];
//        box.points(p);
//        std::sort(p,p+4,[](const cv::Point2f &a, const cv::Point2f &b) {return a.y<b.y;});
//        top = (p[0] + p[1]) / 2;
//        bottom = (p[2] + p[3]) / 2;
//        height = POINT_DIST(top, bottom);
//        width = POINT_DIST(p[0], p[1]);
//        angle = top.x < bottom.x ? box.angle : 90 + box.angle;
//        if(fabs(bottom.x - top.x) <= 0.01) angle = 90;
//    }
//    int lightColor;                     // 灯条颜色
//    cv::Point2f top;                    // 灯条上边中点
//    cv::Point2f bottom;                 // 灯条下边中点
//    double angle;                       // 灯条倾斜角度
//    double height;                      // 灯条高度
//    double width;                       // 灯条宽度
//};
//
////装甲板结构体
//    struct Armor : public cv::RotatedRect    //装甲板结构体
//    {
//        Armor() = default;
//        explicit Armor(cv::RotatedRect &box) : cv::RotatedRect(box)
//        {
//            confidence = 0;
//            id = 0;
//            type = SMALL;
//            grade = 0;
//        }
//        cv::Point2f armor_pt4[4];           // 左下角开始逆时针
//        float confidence;                   // 装甲板置信度
//        int id;                             // 装甲板类别
//        int grade;                          // 装甲板分数
//        int type;                           // 装甲板类型
//        Eigen::Vector3d world_position;     // 当前的真实坐标
//        Eigen::Vector3d camera_position;    // 当前的相机坐标
//        Eigen::Matrix<double, 3, 1> R;      // 旋转向量
//
//    };

//主类
class ArmorDetector : public robot_state {

    public:
        ArmorDetector(); //构造函数初始化

        std::vector<Armor> Detection(const cv::Mat &src); //识别流程入口函数
    private:

        int binThresh;                      // 二值化阈值
        int categories;                     // 装甲板种类 (1:英雄,2:工程,345:步兵,6:哨兵,7:前哨站,8:基地)

        /** 灯条识别参数 */
        double light_max_angle;             // 灯条最大倾斜角度
        double light_min_hw_ratio;          // 灯条最小高宽比
        double light_max_hw_ratio;          // 灯条最大高宽比
        double light_min_area_ratio;        // 灯条最小面积比 (RotatedRect / Rect)
        double light_max_area_ratio;        // 灯条最大面积比 (RotatedRect / Rect)
        double light_max_area;              // 灯条最大面积

        /** 装甲板识别参数 */
        double armor_big_max_wh_ratio;      // 大装甲板最大宽高比
        double armor_big_min_wh_ratio;      // 大装甲板最小宽高比
        double armor_small_max_wh_ratio;    // 小装甲板最大宽高比
        double armor_small_min_wh_ratio;    // 小装甲板最小宽高比
        double armor_max_angle;             // 装甲板最大倾斜角度
        double armor_height_offset;         // 装甲板左右亮灯条中心点高度差
        double armor_ij_min_ratio;          // 装甲板左右灯条的最小高度比
        double armor_ij_max_ratio;          // 装甲板左右灯条的最大高度比
        double armor_max_offset_angle;

        /** 装甲板打分参数 */
        double near_standard;               // 靠近图像中心打分
        int grade_standard;                 // 分数阈值
        int height_standard;                // 固定距离得到的装甲板标准高度

        /** 装甲板打分比例参数 */
        double id_grade_ratio;              // id得分系数比例
        double height_grade_ratio;          // 高度得分系数比例
        double near_grade_ratio;            // 靠近图像中心得分系数

        float thresh_confidence;            // 数字识别置信度阈值

        cv::Mat _src;                       // src的复制体
        cv::Mat _binary;                    // 二值化图片

        /** 存放识别数据的容器 */
        std::vector<Light> candidateLights; // 筛选的灯条容器
        std::vector<Armor> candidateArmors; // 筛选的装甲板容器
        std::vector<Armor> finalArmors;     // 最终装甲板容器
        std::vector<cv::Mat> numROIs;       // 数字ROI容器
        Armor finalArmor;                   // 最终装甲板

        DNN_detect Dnn_Detect;               //数字识别对象


        // ======================图像处理======================
        void setImage(const cv::Mat &src); // 对图像进行二值化

        // ======================灯条匹配======================
        void findLights(); // 找灯条获取候选匹配的灯条
        bool isLight(Light& light, std::vector<cv::Point> &cnt); //判断是否为灯条

        // =====================装甲板匹配=====================
        void matchLights(); // 匹配灯条获取候选装甲板
        bool conTain(cv::RotatedRect &match_rect,std::vector<Light> &Lights, size_t &i, size_t &j); // 判断两个灯条是否包含第三个灯条
        void chooseTarget(); //找出优先级最高的装甲板
        bool get_max(const float *data, float &confidence, int &id);    //

        // =====================装甲板分数=====================
        int armorGrade(const Armor& checkArmor);

        // ======================数字图像=====================
        void preImplement(Armor& armor);        //获取装甲板数字图像ROI

    };

#endif //ARMOR_DETECTION_H
