#pragma once

#include <opencv2/opencv.hpp>

//! 空气阻力模型参数
#define GRAVITY 9.78
#define SMALL_AIR_K 0.01903
#define BIG_AIR_K 0.00556
#define BIG_LIGHT_AIR_K 0.00530

//! 计算宏定义
#define POINT_DIST(p1,p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)) // 计算两点距离(勾股定理)
#define COLOR(str) std::strcmp(str.c_str(),"RED") == 0? RED : BLUE // 判断颜色

//! 参数文件路径
#define PATH "/home/steins_xin/CLionProjects/RM_Detection_install/SetParam_File/"

/**
 * 重命名模块名
 * std::chrono::duration<intmax_t N, intmax_t D = 1>  N表示分子，D表示分母
 *
 *  使用示例:
 *  auto start = std::chrono::high_resolution_clock::now();
 *  for (int i = 0; i < 1000000000; ++i) {}
 *  auto end = std::chrono::high_resolution_clock::now();
 *  auto duration = milliseconds_duration(end-start).count();
 *  std::cout << duration;
 */
using seconds_duration = std::chrono::duration<double>;                     //秒
using milliseconds_duration = std::chrono::duration<double,std::milli>;     //毫秒
using microseconds_duration = std::chrono::duration<double,std::micro>;     //微秒
using chrono_time = decltype(std::chrono::high_resolution_clock::now());    //当前时间 decltype()获取返回变量类型

//! 状态枚举结构体
namespace Robot{    // 设置命名空间
    enum EnemyColor { RED = 1, BLUE = 2 };  //颜色状态
    enum EnemyTypeEnemyType  { SMALL = 1, BIG = 2, BUFF_NO = 3, BUFF_YES = 4};
    enum EnemyState { RUN = 1, SPIN = 2};   //运动 | 陀螺
    enum SpinHeading { UNKNOWN, CLOCKWISE, COUNTER_CLOCKWISE }; // 陀螺的状态 (未知,顺时针,逆时针)
    enum Axes_State{ UNAWARE, LONG, SHORT, HIGH, LOW};                // 轴状态 (未知,长轴,短轴,高轴,矮轴)
    enum Balance{HIK_ON = 1,HIK_OFF = 2};                      // ON:自动白平衡     OFF:手动白平衡
    enum MDCamera{MD_ON = 1,MD_OFF = 2};                      //MD_ON: 开启相机名字模式 MD_OFF：关闭相机名字模式
}
using namespace Robot;


//! 电控接口
class robot_state
{
public:
    //电控发来的角度和弹速
    float Controller_pitch;         // P轴
    float Controller_yaw;           // Y轴
    float Controller_roll;          // R轴
    float quaternion[4];            // 四元数
    float bullet_speed;             // 弹道速度

    robot_state() = default;
};
