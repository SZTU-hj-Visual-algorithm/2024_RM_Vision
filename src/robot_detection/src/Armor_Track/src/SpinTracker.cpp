//
// Created by steins_xin on 23-10-3.
//

#include "SpinTracker.h"

SpinTracker::SpinTracker(const Armor &armor, chrono_time armor_timestamp) {
    last_armor = armor;                             // 保存本次装甲板
    last_timestamp = armor_timestamp;               // 记录本次装甲板的时间戳
    is_initialized = false;                         // 初始化状态为false
}

/**
 *  函数名: update_tracker
 *  传入: Armor &new_armor int timestamp          (装甲板,时间戳)
 *  传出: bool                                    (布尔变量)
 *  功能: 通过传入装甲板和时间戳,更新当前对象的最后装甲板和时间戳，将初始装甲板设为true
 */
bool SpinTracker::update_tracker(const Armor &new_armor, chrono_time timestamp) {

    is_initialized = true;                          // 初始化状态为true
    last_armor = new_armor;                         // 更新本次装甲板
    last_timestamp = timestamp;                     // 更新本次装甲板的时间戳

    return true;
}
