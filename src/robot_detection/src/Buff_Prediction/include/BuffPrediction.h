#include <iostream>
#include <opencv2/opencv.hpp>
#include "AngleSolve.h"
#include "robot_status.h"
#include "robot_struct.h"
 
#include <ros/package.h>
#define POINT_DIST(p1, p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))
#define waitTime 50
#define TryShootTime 50


enum SPEED_TYPE {
    SPEED_UP = 0,
    SPEED_DOWN
};


// 角速度信息结构体
struct RotateSpeed {
    float lastRotateSpeed = 0.0f;
    float nowRotateSpeed = 0.0f;
    float realRotateSpeed = 0.0f;

    RotateSpeed() = default;
};

//速度范围结构体
struct SpeedRange {
    float nowMinSpeed = 100.0f;
    float realMinSpeed = 0.0f;
    float nowMaxSpeed = 0.0f;
    float realMaxSpeed = 0.0f;
    int minSameNumber = 0;
    int maxSameNumber = 0;
    bool minSpeedFlag = false;
    bool maxSpeedFlag = false;

    SpeedRange() = default;
};


//拟合数据结构体
struct FittingData {
    FittingData() = default;

    FittingData(float angle_speed_, double time_) {
        angle_speed = angle_speed_;
        time = time_;
    }

    float angle_speed;  //角速度
    double time; //对应的时间
};

//   // 能量机关自动开火模式
//     enum Fire_Mode 
//     {
//         Mixed ,      //  混合
//         Hight ,      //  只打最快时刻
//         Midden ,     //  只打中间时刻
//         Low          //  只在最慢时刻
//     };


//判断是否需要开火的结构体
struct Fire {
    bool Start_Fire = false;  // 开火

    bool FireSameLeaf = false; // 是否同一个叶子

    bool Is_High = false ;     //  是否在最快时刻

    bool Is_midian = false ;     //  是否在中间时刻

    bool Is_low = false ;     //  是否在最慢时刻

    bool Have_shoot = false ;

    bool Try_shoot = false ;

    cv::Point2f now_Point;

    cv::Point2f last_Point;

    size_t shoot_time = 0 ;

    size_t wait_time = 0 ;

    size_t SameLeafTime = 0;  // 相同叶子的数目

    float GetNowSpeed = 0.0f; // 获取此刻的角速度

    float DistancePreAndReal = 0.0f;    //实际目标位置和预测位置

    float Prediction_angle = 0.0f;      // 目前的预测补偿角度

    std::vector<float> fire_dataList;  // 数据容器 （用来看采样数据是否平滑）用于判断是否需要击打

    // Fire_Mode fire_mode ;

    void fire_init()                    // 初始化阶段
    {
        Start_Fire = false;
        Is_High = false;   
        Is_low = false;
        Is_midian = false;
    }

    bool Is_smooth_datas()              // 数据是否平滑
    {
        double data_rates_sum = 0.0;
        double data_rates_average = 0.0;

        for(auto X : fire_dataList)
        {
            data_rates_sum += X ;
        }
        data_rates_average = data_rates_sum / fire_dataList.size() ;

        std::cout<<data_rates_average<<"\n";
        fire_dataList.clear() ;
 
        return true ;
    }

    Fire() = default;
};


//大能量机关预测类
class BuffPrediction {

public:
    BuffPrediction();

    //主函数API接口
    void Prediction(cv::Mat &src, BuffTarget &Target);

    void Prediction_Init();

    //AS接口 公有
    double Solve_pitch;
    double Solve_yaw;
    bool Solve_OK;
    AngleSolve AS;
    int Buff_Mode;
    int Dubug = 0;
    int clear = 50;
    //开火
    size_t fire_Ok;

//###############################################################
    float T0_N = 76;     // 相位采样数   越低越卡顿
    float N = 34;       // 角速度采样数    太低的话在最低点跳动
    float SMALL_Pre_Angle_Gain = 0.0;
    double DELAY_TIME_ = -0.4;  // 弹道延迟时间

//###############################################################
/*
 *  结构体/枚举的定义
 */
    RotateSpeed _rotateSpeed;
private:

    //方向的枚举
    enum Direction {
        STOP = 0,
        CLOCKWISE,  //顺时针
        ANTICLOCKWISE,  //逆时针
        UNKNOW,
    };
  
    /*
     * 大能量机关预测
     * 拟合 根据离散傅里叶拟合三角函数
     */
    float fittingCurve();

    // 求角频率、振幅

    void fitting_a_w();

    // 求相位差

    void fitting_t();

    // 求不同相位时的积分,规整化速度值

    float get_integral(float t_);

    // 离散傅里叶获得第n项的值，规整化速度值

    float get_F(int n, int _N);

    // 离散傅里叶获得余弦项值

    float get_F_c(int n, float f_k, int k, int _N);

    // 离散傅里叶获得正弦项值
    
    float get_F_s(int n, float f_k, int k, int _N);

    float BigPrediction;

    /*
     * 小能量机关的预测
     */
    static float SMALL_buff_PredictionAngle();


    // 获取旋转方向

    void KnowDirection(BuffTarget &buffTargetPrediction);

    // 计算旋转速度

    void calculateRotateSpeed(BuffTarget &buffTargetPrediction);

    // 计算预测点坐标

    void get_pre_points(BuffTarget &buffTargetPrediction);

    // 计算预测提前量
    void addAngele(BuffTarget &buffTargetPrediction);

    // 开火逻辑

    void OpenFire();  

    // 可视化数据

    void show_message(cv::Mat &show_src, BuffTarget &buffTargetPrediction);

    cv::Point2f outPoint_0 ;
    cv::Point2f outPoint_1 ;
    cv::Point2f outPoint_2 ;
    cv::Point2f outPoint_3 ;

    SpeedRange _speedRange;
 
    Direction direction = UNKNOW;
 
    Fire fire;

    std::vector<float> _buffAngleList;   // 判断方向的
    std::vector<FittingData> data;       // 大能量机关拟合的

    cv::Point2f prediction_point;


    bool Direction_Init = false;
    bool _sameLeaf = false;

    float Gain_Angle;  // 预测增益
    float distance{};
    
    float delay_time = 0.3;
    float start_time = 0.0;
    float _a = 0.9;  // 振幅 [0.780, 1.045]
    float _w = 1.9;  // 频率 [1.884, 2.000]
    float t_0 = 0.0; // 初相
    float last_angle = 0.0;
    float MAX_T0 = 3.34; // 最大周期

    float DT = 0.01;     // 采样时间间隔，单位：秒
 

    size_t DN = 1; // 逐差法测速度间距

};
