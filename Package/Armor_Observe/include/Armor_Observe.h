//
// Created by steins_xin on 23-10-11.
//

#ifndef ARMOR_TRACK_ARMOR_OBSERVE_H
#define ARMOR_TRACK_ARMOR_OBSERVE_H

#include <opencv2/opencv.hpp>
#include "Circle_Kalman.h"
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"
#include "AngleSolve.h"
#include "Armor_Track.h"

/** 平滑滤波结构体 */
struct Smooth{
public:
    Smooth() = default;
    bool fit = false;                                               // 是否可以拟合
    //========================API接口=======================
    bool update(Eigen::Vector3d position,Eigen::Vector3d &Smooth);  // 更新数据
private:
    int max_size = 10;                                               // 队列最大长度(隔帧拟合)
    std::deque<Eigen::Vector3d> Data_info;                          // 数据队列
};


class ArmorObserve {
public:
    ArmorObserve();
    cv::Mat _src;
    bool Fit_OK;                                                                // 圆心是否拟合成功标志
    Eigen::Vector3d Smooth_position;                                            // 平滑之后的圆心世界坐标
    //========================API接口=======================
    void Center_fitting(Armor &armor,double axes_length,double z,Observe OB,SpinTracker OB_Track);                       // 拟合圆心位置
    void World_projection_show();                                               // 绘制重投影装甲板
    /** 通过重投影绘制整车观测 */
    void ArmorObserve_show(Eigen::Matrix<double, 3, 1> center,Observe OB,SpinTracker OB_Track);
    AngleSolve AS;
    Eigen::Vector3d pre_pos;          // 预测圆心坐标
    Eigen::Vector3d pre_Armor;        // 预测装甲板坐标
    double Angle_Speed;                             // 角速度
    Eigen::Vector3d spin_Aromor;                    // 陀螺装甲板
    double spin_angle;
    double Left_Armor_angle;
    double Right_Armor_angle;
    cv::Point2f Left_Armor_cir,Right_Armor_cir,O_Armor_cir,F_Armor_cir;
    Eigen::Vector3d Left_Armor,Right_Armor,O_Armor,F_Armor;
    Eigen::Vector3d left_target;
    Eigen::Vector3d right_target;
private:
    //========================参数部分=======================
    /** 装甲板实际参数 */
    double small_w;
    double small_h;
    double big_w;
    double big_h;

    /** 重投影坐标 */
    cv::Point2f lu,ld,ru,rd;                            // 定义4点坐标
    Eigen::Vector3d imuPoint_lu;                        // 世界坐标系 左上
    Eigen::Vector3d imuPoint_ld;                        // 世界坐标系 左下
    Eigen::Vector3d imuPoint_ru;                        // 世界坐标系 右上
    Eigen::Vector3d imuPoint_rd;                        // 世界坐标系 右下

    /** 圆心参数 */
    double r = 0.23;                                    // 圆心半径(需要计算得到,这里先固定)
    cv::Point2f cir;                                    // 圆心像素坐标
    Eigen::Vector3d center_position;                    // 圆心世界坐标
    Smooth Smooth_Filter;                               // 圆心平滑结构体

    /** 计算参数 */
    double yaw;                                         // 装甲板朝向角(左为正,右为负)
    double Armor_distance;                              // 装甲板4点距离差之和

    /** 外部库对象 */
    Circle_Kalman CKF;
    Eigen::Matrix<double, 4, 1> pre;                    // 卡尔曼预测矩阵

    //========================函数部分=======================

};


#endif //ARMOR_TRACK_ARMOR_OBSERVE_H
