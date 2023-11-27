//
// Created by steins_xin on 23-11-1.
//
#pragma once

//! 整个项目的结构体
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
// 外部头文件
#include "robot_status.h"

using namespace Robot;
/////////////////////////////  Armor_Detection结构体  /////////////////////////////
//灯条结构体
struct Light : public cv::RotatedRect     //灯条结构体
{
    Light() = default;
    explicit Light(cv::RotatedRect &box) : cv::RotatedRect(box) //列表初始化
    {
        cv::Point2f p[4];
        box.points(p);
        std::sort(p,p+4,[](const cv::Point2f &a, const cv::Point2f &b) {return a.y<b.y;});
        top = (p[0] + p[1]) / 2;
        bottom = (p[2] + p[3]) / 2;
        height = POINT_DIST(top, bottom);
        width = POINT_DIST(p[0], p[1]);
        angle = top.x < bottom.x ? box.angle : 90 + box.angle;
        if(fabs(bottom.x - top.x) <= 0.01) angle = 90;
    }
    int lightColor{};                     // 灯条颜色
    cv::Point2f top;                    // 灯条上边中点
    cv::Point2f bottom;                 // 灯条下边中点
    double angle{};                       // 灯条倾斜角度
    double height{};                      // 灯条高度
    double width{};                       // 灯条宽度
};

//装甲板结构体
struct Armor : public cv::RotatedRect    //装甲板结构体
{
    Armor() = default;
    explicit Armor(cv::RotatedRect &box) : cv::RotatedRect(box)
    {
        confidence = 0;
        id = 0;
        type = SMALL;
        grade = 0;
    }
    cv::Point2f armor_pt4[4];           // 左下角开始逆时针
    float confidence;                   // 装甲板置信度
    int id;                             // 装甲板类别
    int grade;                          // 装甲板分数
    int type;                           // 装甲板类型
    Eigen::Vector3d world_position;     // 当前的真实坐标
    Eigen::Vector3d camera_position;    // 当前的相机坐标
    Eigen::Matrix<double, 3, 1> R;      // 旋转向量

};
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////  Serial_Device结构体  //////////////////////////////
typedef struct form
{
    int mode;                           // 模式
    int dat_is_get;                     // 数据是否获取
    float data[3];                      // pitch yaw shoot
    float quat[4];                      // 四元数
    std::vector<Armor> armors;          // 装甲板
    chrono_time tim;                    // 当前时间戳
}form;//线程之间的数据交换结构体
//////////////////////////////////////////////////////////////////////////////////
