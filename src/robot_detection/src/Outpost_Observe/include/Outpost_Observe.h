//
// Created by steins_xin on 24-4-15.
//

#ifndef RM_DETECTION_OUTPOST_OBSERVE_H
#define RM_DETECTION_OUTPOST_OBSERVE_H

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>

//外部头文件
#include "robot_status.h"
#include "robot_struct.h"
#include "AngleSolve.h"
#include "Outpost_Kalman.h"

// ROS
#include <ros/package.h>

struct Spin_Direction_Tarker
{
    bool Init = false;                      // 初始化标志
    Armor last_armor;                       // 上次装甲板
    chrono_time last_timestamp;             // 上次装甲板时间戳

    // 陀螺分数
    double spin_score = 0;
};

/** 平滑滤波结构体 */
struct Smooth_post{
public:
    Smooth_post() = default;
    bool fit = false;                                               // 是否可以拟合
    //========================API接口=======================
    bool update(Eigen::Vector3d position,Eigen::Vector3d &Smooth);  // 更新数据
    int max_size = 100;                                             // 队列最大长度(隔帧拟合)
    std::deque<Eigen::Vector3d> Data_info;                          // 数据队列
};

class Outpost_Observe
{
public:
    Outpost_Observe();      
    double spin_angle;
    double Left_Armor_angle;
    double Right_Armor_angle;
    AngleSolve AS;                                                      // 坐标类

    void Outpost_Track(Armor armor,const chrono_time &time);            // 前哨站|主函数
    int Spin_State();                                                   // 返回前哨站陀螺状态

    /** 击打预测 */
    Eigen::Vector3d Center_Armor,O_Armor,R_Armor,L_Armor;               // 圆心 | 当前装甲板 | 右侧装甲板 | 左侧装甲板
    Eigen::Vector3d Smooth_position;                                            // 平滑之后的圆心世界坐标
    cv::Point2f O_Armor_cir,R_Armor_cir,L_Armor_cir;                    // 装甲板中心 | 当前装甲板 | 右侧装甲板 | 左侧装甲板
    cv::Point2f cir;                                                    // 圆心像素坐标
    double O_Angle,R_Angle,L_Angle;                                     // 当前角度 | 右侧角度 | 左侧角度
    void Center_fitting(Armor armor);                                   // 全观测函数
    void Reset();                                                   // 重置初始化
private:
    /** 陀螺观测 */
    Robot::SpinHeading Outpost_Spin_state = UNKNOWN;                    // 陀螺状态 | CLOCKWISE | COUNTER_CLOCKWISE
    Spin_Direction_Tarker Spin_tarker;                                  // 陀螺观测器
    void Spin_direction(Armor armor, const chrono_time &time);          // 陀螺方向判定
    
    /** 前哨站参数 */
    double Outpost_Angle_speed;                                         // 0.144°/ms 
    double Outpost_R;                                                   // 0.553m|最外围

    /** 圆心参数 */
    Eigen::Vector3d center_position;                                    // 圆心世界坐标
    double Armor_yaw;                                                   // 装甲板朝向角(左为正,右为负)
    Smooth_post Smooth_Filter;                                          // 圆心平滑结构体
    
    /** 外部库 */
    bool Init = false;                                                  // 初始化标志
    chrono_time t;                                                      // 当前时间
    Outpust_KalmanFilter KF;                                            // 滤波类
};



#endif //RM_DETECTION_OUTPOST_OBSERVE_H