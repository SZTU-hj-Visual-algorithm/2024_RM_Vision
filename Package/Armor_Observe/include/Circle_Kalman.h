//
// Created by steins_xin on 23-10-12.
//

#ifndef ARMOR_OBSERVE_CIRCLE_KALMAN_H
#define ARMOR_OBSERVE_CIRCLE_KALMAN_H

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

class Circle_Kalman {
public:
    void Initial();                                             // 卡尔曼初始化
    bool Set_init;                                              // 卡尔曼设置初始化标志
    bool Track_OK;                                              // 卡尔曼进入跟踪状态
    //===========================API接口================================
    void setF(double dt);                                        // 设置状态转移矩阵，基于时间差进行转移
    void setPosAndSpeed(Eigen::Vector3d position, Eigen::Vector2d speed);   //设置点和速度
    Eigen::Matrix<double, 4, 1> predict();                      // 更新步
    Eigen::Matrix<double, 4, 1> update(Eigen::Vector2d z_k);    // 预测步
private:
    /** 卡尔曼运算矩阵 */
    Eigen::Matrix<double, 4, 2> K;                              // Kalman增益
    Eigen::Matrix<double, 4, 4> F;                              // 转移矩阵
    Eigen::Matrix<double, 2, 4> H;                              // 观测矩阵
    Eigen::Matrix<double, 4, 4> Q;                              // 预测过程噪声偏差的方差
    Eigen::Matrix<double, 2, 2> R;                              // 测量噪声偏差，(系统搭建好以后，通过测量统计实验获得)
    Eigen::Matrix<double, 4, 4> P;                              // 估计误差协方差
    /** 卡尔曼结果矩阵 */
    Eigen::VectorXd x_pre;                                      // 预测矩阵
    Eigen::Matrix<double, 4, 1> x_post;                         // 校正矩阵
    /** 其他参数 */
    int num = 4;                                                // 卡尔曼迭代次数阈值
    int Track_num;                                              // 卡尔曼迭代次数
};


#endif //ARMOR_OBSERVE_CIRCLE_KALMAN_H
