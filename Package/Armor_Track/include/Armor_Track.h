//
// Created by steins_xin on 23-10-1.
//

#ifndef RM_DETECTION_ARMOR_TRACK_H
#define RM_DETECTION_ARMOR_TRACK_H

#include <opencv2/opencv.hpp>
// 内部头文件
#include "Kalman.h"
#include "SpinTracker.h"
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"
#include "AngleSolve.h"

/** 跟踪状态 */
enum TrackerState {
    MISSING,            // 没有目标，跳过该部分
    DETECTING,          // 还未开始跟踪，作为跟踪第一帧的切换，初始化好卡尔曼
    LOSING,             // 处于丢失状态，还会保留预测
    TRACKING,           // 处于跟踪状态
};

/** 整车观测 */
struct Observe{
public:
    double Long_axes;                                               // 长轴长度
    double Short_axes;                                              // 短轴长度

    double HIGH_LONG_pro;                                           // 高长轴概率
    double LOW_LONG_pro;                                            // 矮长轴概率

    bool Observe_OK = false;                                        // 整车观测完成的标志

    double High_LOW_difference;                                     // 装甲板世界高低差
    double center_Z;
    std::map<int,int>  Data_record;                                 // 记录高低装甲板对应的长短轴次数
    //========================API接口=======================
    bool update(double R_1,double R_2);                             // 更新数据
private:
    int max_size = 4;                                               // 队列最大长度(隔帧拟合)
    std::deque<Eigen::Vector2d> Data_info;                          // 数据队列
};


class ArmorTrack {
public:
    ArmorTrack();
    cv::Mat _src;
    Armor enemy_armor;                              // 最终选择的装甲板

    /** 跟踪信息 */
    int tracker_state;                              // 此时跟踪器的状态
    int tracking_id;                                // 跟踪的敌方ID

    /** 角度信息 */
    AngleSolve AS;                                  // 坐标系转换
    float Solve_pitch;                              // 解算出的pitch
    float Solve_yaw;                                // 解算出的yaw
    double Angle_Speed;                             // 角速度
    bool Angle_Speed_OK = false;                             // 角速度
    std::deque<double> Angle_Speed_smooth;                          // 角速度平滑
    std::deque<double> Angle_Speed_FPS;                          // 角速度帧率平滑

    cv::Point2f temp_cir;                           // 圆心像素坐标
double angle_speed;
    /** 整车观测 */
    std::map<int,Observe> OB;                       // 整车观测结构体
    std::map<int,SpinTracker> OB_Track;             // 长短轴跟踪器
    //========================API接口========================
    bool Track(const cv::Mat& src, std::vector<Armor> &armors, const chrono_time &time);    // 预测流程入口函数
    void Reset();                                                                           // 重置初始化
    void show();
private:
    //========================参数部分=======================
    /** 跟踪变量参数 */
    bool locate_target;                             // 锁定装甲板
    bool track_start;                               // 启动跟踪

    /** 跟踪阈值参数 */
    int find_aim_number;                            // 成功锁定目标的次数
    int find_threshold;                             // 锁定目标次数的阈值
    int lost_aim_number;                            // 丢失锁定目标的次数
    int lost_threshold;                             // 丢失目标次数的阈值
    double new_old_threshold;                       // 新旧坐标的距离阈值

    /** 陀螺参数 */
    double spin_max_r_multiple;                     // 符合陀螺条件，反陀螺分数增加倍数
    int spin_judge_low_threshold;                   // 小于该阈值认为该车已关闭陀螺
    int spin_judge_high_threshold;                  // 大于该阈值认为该车已开启陀螺
    int max_delta_t;                                // 维护陀螺跟踪器时间阈值
    std::map<int,double> spin_score_map;            // 记录各装甲板小陀螺可能性分数，大于0为顺时针旋转，小于0为逆时针旋转
    std::map<int,SpinHeading> spin_status_map;      // 记录该车小陀螺状态（未知，顺时针，逆时针）
    std::multimap<int, SpinTracker> trackers_map;   // 陀螺跟踪器Map

    /** 其他参数 */
    chrono_time t;                                  // 当前时间戳
    std::vector<double> Angle_Speed_recore;                // 角速度计算
    chrono_time Angle_t;                                  // 当前时间戳
    bool Angle_t_record = false;
    double dt;
    double test_dt;


    /** 卡尔曼参数 */
    KalmanFilter KF;                                // 卡尔曼
    Eigen::Matrix<double,6,1> predicted;            // 预测矩阵

    //========================函数部分=======================
    /** 跟踪观测函数 */
    bool Initial(std::vector<Armor> &Armors);                       // 初始化函数
    bool Lock_Armor(std::vector<Armor> &find_armors, double dt);    // 两帧之间锁定装甲板
    void State_solve(bool match);                                   // 对跟踪状态进行处理

    /** 陀螺观测函数 */
    void UpdateSpinScore();
    void Spin_detect();

    /** 选择击打函数 */
    double Solve_Radius(Armor &armor,double yaw,double x);          // 三分法求解半径法
    void Angle_selection(Armor &armor,double angle);                // 选择击打装甲板角度
};


#endif //RM_DETECTION_ARMOR_TRACK_H
