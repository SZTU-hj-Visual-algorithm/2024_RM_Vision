//
// Created by steins_xin on 23-10-1.
//

#ifndef ARMOR_TRACK_KALMAN_H
#define ARMOR_TRACK_KALMAN_H

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
// 外部头文件
#include "robot_status.h"
// ROS
#include <ros/package.h>

class KalmanFilter {
public:
    void Initial();                                             // 卡尔曼初始化
    //===========================API接口================================
    void setF(double t);                                        // 设置状态转移矩阵，基于时间差进行转移
    void setPosAndSpeed(Eigen::Vector3d position, Eigen::Vector3d speed);   //设置点和速度
    Eigen::Matrix<double, 6, 1> predict();                      // 更新步
    Eigen::Matrix<double, 6, 1> update(Eigen::Vector3d z_k);    // 预测步

private:
    /** 卡尔曼运算矩阵 */
    Eigen::Matrix<double, 6, 3> K;                              // Kalman增益
    Eigen::Matrix<double, 6, 6> F;                              // 转移矩阵
    Eigen::Matrix<double, 3, 6> H;                              // 观测矩阵
    Eigen::Matrix<double, 6, 6> Q;                              // 预测过程噪声偏差的方差
    Eigen::Matrix<double, 3, 3> R;                              // 测量噪声偏差，(系统搭建好以后，通过测量统计实验获得)
    Eigen::Matrix<double, 6, 6> P;                              // 估计误差协方差
    /** 卡尔曼结果矩阵 */
    Eigen::VectorXd x_pre;                                      // 预测矩阵
    Eigen::Matrix<double, 6, 1> x_post;                         // 校正矩阵
};


#endif //ARMOR_TRACK_KALMAN_H
