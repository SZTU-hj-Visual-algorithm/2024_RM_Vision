//
// Created by steins_xin on 23-9-29.
//

#ifndef RM_DETECTION_ANGLESOLVE_H
#define RM_DETECTION_ANGLESOLVE_H


#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
//外部头文件
#include "robot_status.h"
#include "robot_struct.h"

class AngleSolve : public robot_state {
public:
    AngleSolve();
    cv::Mat _src;
    robot_state Robot_msg;                                      // 串口数据
    //========================API接口========================
    /** 坐标系转换 */
    cv::Point2f imu2pixel(Eigen::Vector3d imu_pos);                     // 世界坐标转换为像素坐标
    Eigen::Vector3d pixel2imu(Armor &armor);                            // 像素坐标转换为世界坐标
    Eigen::Vector3d pixel2imu_buff(cv::Point2f *p, int type);           // 像素坐标转换为世界坐标(BUFF)
    /** 四元数旋转矩阵 */
    Eigen::Matrix3d quaternionToRotationMatrix(float quaternion[4]);    // 四元数转换为旋转矩阵
    Eigen::Matrix3d quaternionToRotationMatrix_buff();                  // 四元数转换为旋转矩阵(BUFF)
    Eigen::Matrix3d EulerAngle2RotationMatrix(double pitch,double roll,double yaw); // 解算旋转矩阵
    /** 重投影 */
    void AngleSolve_show(Armor &armor);                                 // 绘制装甲板朝向角
    double Armor_Angle(Armor &armor);                                   // 计算装甲板朝向角度
    double World_projection(Armor &armor,double Angle);                 // 计算重投影后装甲板4点距离差
    //** 枪管角度解算 */
    void Init(float data[3], float *quat);                              // 更新数据
    Eigen::Vector3d Barrel_Solve(Eigen::Vector3d position);             // 解算枪管移动角度
    
    cv::Point2f cam2pixel(Eigen::Vector3d cam_pos);             // 相机坐标系转换为像素坐标系
    Eigen::Vector3d cam2imu(Eigen::Vector3d cam_pos);           // 相机坐标系转换为世界坐标系
    Eigen::Vector3d imu2cam(Eigen::Vector3d imu_pos);           // 世界坐标系转换为相机坐标系

private:
    //========================参数部分========================

    /** 装甲板实际大小参数 */
    double big_w;                           // 大装甲板实际宽度
    double big_h;                           // 大装甲板实际高度
    double small_w;                         // 小装甲板实际宽度
    double small_h;                         // 小装甲板实际高度

    /** 符叶实际大小参数 */
    double buff_w;                          // 符叶实际宽度(垂直)
    double buff_h;                          // 符叶实际高度(垂直)
    //待调试

    /** 重投影坐标 */
    cv::Point2f lu,ld,ru,rd;                            // 定义4点坐标
    Eigen::Vector3d imuPoint_lu;                        // 世界坐标系 左上
    Eigen::Vector3d imuPoint_ld;                        // 世界坐标系 左下
    Eigen::Vector3d imuPoint_ru;                        // 世界坐标系 右上
    Eigen::Vector3d imuPoint_rd;                        // 世界坐标系 右下

    /** 相机参数 */
    cv::Mat F_MAT;                          // 相机内参矩阵(Mat)
    cv::Mat C_MAT;                          // 相机畸变矩阵(Mat)
    Eigen::Matrix<double,3,3> F_EGN;        // 相机内参矩阵(Eigen)
    Eigen::Matrix<double,1,5> C_EGN;        // 相机畸变矩阵(Eigen)

    /** 特征矩阵 */
    Eigen::Vector3d tv;                     // 平移向量
    Eigen::Matrix<double,3,3> rv;           // 旋转矩阵
    Eigen::Vector3d tv_buff;                // 平移向量
    Eigen::Matrix<double,3,3> rv_buff;      // 旋转矩阵

    /** 功能矩阵 */
    Eigen::Matrix<double,3,3> RotationMatrix_imu;           // 旋转矩阵
    Eigen::Matrix<double,3,3> RotationMatrix_imu2buff;      // 符叶旋转矩阵
    Eigen::Vector3d center_offset_position;                 // 世界坐标系和相机坐标系原点偏差
    Eigen::Vector3d Barrel_offset_angle;                    // 抬升枪口角度补偿

    //========================函数部分========================
    /** pnp测距 */
    Eigen::Vector3d pnpSolve(cv::Point2f *p, int type,Eigen::Matrix<double, 3, 1>& R);         // 测距函数
    Eigen::Vector3d pnpSolve_buff(cv::Point2f *p, int type);    // 测距函数(buff)

    /** 世界坐标系转换为像素坐标系(调用接口) */

    /** 像素坐标系转换为世界坐标系(调用接口) */
    Eigen::Vector3d pixel2cam(Armor &armor);                    // 像素坐标系转换为相机坐标系
    Eigen::Vector3d pixel2cam_buff(cv::Point2f *p, int type);   // 像素坐标系转换为相机坐标系(BUFF)

    /** 抬升角度计算 */
    float airResistanceSolve(Eigen::Vector3d Pos);
    float BulletModel(float x, float v, float angle);

};


#endif //RM_DETECTION_ANGLESOLVE_H
