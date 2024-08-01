//
// Created by steins_xin on 23-10-3.
//

#ifndef ARMOR_TRACK_SPINTRACKER_H
#define ARMOR_TRACK_SPINTRACKER_H

#include <opencv2/opencv.hpp>
// 外部头文件
#include "robot_struct.h"
#include "robot_status.h"

// 陀螺状态的跟踪器
class SpinTracker {
public:
    Armor last_armor;                       // 本次装甲板
    bool is_initialized;                    // 是否完成初始化
    chrono_time last_timestamp;             // 本次装甲板时间戳
    Axes_State axesState;                   // 当前跟踪装甲板长短轴状态
    Axes_State HeightState;                 // 当前跟踪装甲板高低轴状态

    SpinTracker(){is_initialized = false;axesState = UNAWARE;HeightState = UNAWARE;}
    explicit SpinTracker(const Armor& armor, chrono_time armor_timestamp);
    bool update_tracker(const Armor& new_armor, chrono_time timestamp);
};


#endif //ARMOR_TRACK_SPINTRACKER_H
