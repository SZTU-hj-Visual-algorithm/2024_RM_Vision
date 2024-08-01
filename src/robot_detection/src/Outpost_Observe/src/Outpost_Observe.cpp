//
// Created by steins_xin on 24-4-15.
//

#include "../include/Outpost_Observe.h"

Outpost_Observe::Outpost_Observe(){
    KF.Initial();                                       // 卡尔曼初始化
    Outpost_Angle_speed = 0.144;                        // 0.144°/ms | 前哨站转速 
    // Outpost_R = 0.553;                                  // 前哨站半径 | 还需要调整
    Outpost_R = 0.270;                                  // 前哨站半径
}


/**
 *  函数名: Reset
 *  传入: 无
 *  传出: 无
 *  功能: 重置所有状态,进行初始化
 */
void Outpost_Observe::Reset(){
    Init = false;

    /** 平滑结构体 */
    Smooth_Filter.fit = false;
    Smooth_Filter.Data_info.clear();

    /** 旋转状态 */
    Outpost_Spin_state = UNKNOWN;
    Spin_tarker.spin_score = 0;

    /** 卡尔曼 */
    KF.Initial();                       // 卡尔曼初始化
}

/**
 *  函数名: Spin_direction
 *  传入: Armor armor, const chrono_time &time 装甲板和当前时间戳
 *  传出: 无                      
 *  功能: 通过传入的装甲板和时间戳,对前哨站的陀螺状态进行判断
 */
void Outpost_Observe::Spin_direction(Armor armor, const chrono_time &time){
    // 初始化
    if(!Spin_tarker.Init){
        Spin_tarker.Init = true;
        Spin_tarker.last_armor = armor;
        Spin_tarker.last_timestamp = time;
        return;
    }

    // 重新初始化 | 更新时间间隔判断
    double dt = seconds_duration(time - Spin_tarker.last_timestamp).count();    // 计算时间间隔
    if(dt > 2){
        Spin_tarker.Init = false;
        Init = false;
        return;
    }

    // 进行陀螺状态对比 | x轴距离对比
    double distance = Spin_tarker.last_armor.world_position[0] - armor.world_position[0];
    if(distance > 0.01) Spin_tarker.spin_score++;
    else if(distance < -0.01) Spin_tarker.spin_score--;
    // printf("distance:%f  dt:%f  spin_score:%f",distance,dt,Spin_tarker.spin_score);
    
    // 是否旋转的判断 | 需要吗? | 如果需要的话,大致思路为判断是否有距离大的跳变瞬间来判断是否旋转 | 并不需要
    
    // 陀螺分数判断
    if(Spin_tarker.spin_score > 5) Outpost_Spin_state = CLOCKWISE;
    else if(Spin_tarker.spin_score < -5) Outpost_Spin_state = COUNTER_CLOCKWISE;

    // 更新数据
    Spin_tarker.last_armor = armor;
    Spin_tarker.last_timestamp = time;
}

/**
 *  函数名: Center_fitting
 *  传入: Armor armor                         装甲板
 *  传出: 无                      
 *  功能: 对前哨站进行全观测,计算击打装甲板和角度
 */
void Outpost_Observe::Center_fitting(Armor armor){
    Outpost_Angle_speed;                                        // 前哨站转速 | 0.144°/ms 
    Outpost_R;                                                  // 前哨站半径 | 0.270m

    Armor_yaw = armor.R[1];
    // 需要在相机坐标系下推理计算,再转到世界坐标系下
    center_position = armor.camera_position;                                            // 圆心坐标
    center_position = {center_position[0],center_position[2],-center_position[1]};      // 世界坐标系

    center_position[0] += sin(Armor_yaw)*Outpost_R;                                     // 三角函数解圆心
    center_position[1] += cos(abs(Armor_yaw))*Outpost_R;                                // 三角函数解圆心
    center_position = {center_position[0],-center_position[2],center_position[1]};      // 相机坐标系
    center_position = AS.cam2imu(center_position);                                      // 转换世界坐标系

    Smooth_Filter.update(center_position,Smooth_position);                // 更新数据,进行平滑

    cir = AS.imu2pixel(Smooth_position);                                                // 转换像素坐标


    // 平滑？可能需要 | 待确定 | 卡尔曼还是平滑
    Eigen::Matrix<double,6,1> predicted;            // 预测矩阵
    if(!Init) KF.setPosAndSpeed(center_position,Eigen::Vector3d(0,0,0));
    KF.setF(0.015);
    predicted = KF.predict();    
    predicted = KF.update(center_position);         // 更新预测值 返回的是校正后的结果
    // center_position = predicted.head(3);            // 滤波点

if(Smooth_Filter.fit){
    // 参数调整
    double compensate;                              // 弹道时间角度补偿
    double Dial_time;                               // 拨盘转动时间 | 拨弹补偿
    double time;                                    // 弹道飞行时间

    double Diff = sqrt(pow(Smooth_position[0],2)+pow(Smooth_position[1],2));
    
    // Diff = 2.4;  
    // printf("Diff_xy: %f Diff:%f\n Smooth_position[2]:%f",Diff_xy,Diff,Smooth_position[2]);
#ifndef Hero
    double bullet_speed = 25;                       // 更换实时弹速
#else
    double bullet_speed = 15;                       // 更换实时弹速
#endif //Hero
    double compensate_size = 1;                     // 倍率 TODO:测试可行,有上升空间(待测试)
    time = Diff/bullet_speed;                       // 单位: s
    Dial_time = 25;                                 // 单位: ms
    
    // 计算补偿角度 
    compensate = compensate_size*(time*1000)*(Outpost_Angle_speed) + Dial_time*(Outpost_Angle_speed);              

    if(Outpost_Spin_state == COUNTER_CLOCKWISE){
        // 逆时针
        spin_angle = Armor_yaw*(180.0f/CV_PI) - (Outpost_Angle_speed*15) - compensate;          // 陀螺装甲板所在角度(逆时针)
    }
    else if(Outpost_Spin_state == CLOCKWISE){
        // 顺时针
        spin_angle = Armor_yaw*(180.0f/CV_PI) + (Outpost_Angle_speed*15) + compensate;          // 陀螺装甲板所在角度(顺时针)
    }
    spin_angle *= (CV_PI/180.0f);
    Left_Armor_angle = spin_angle*(180.0f/CV_PI) + 120.0;
    Right_Armor_angle = spin_angle*(180.0f/CV_PI) - 120.0;
    // printf("spin_angle: %f  yaw:%f  d:%f \n",spin_angle*(180.0f/CV_PI),Armor_yaw*(180.0f/CV_PI),(Outpost_Angle_speed*15) + compensate);
    // printf("Left_Armor_angle: %f  Right_Armor_angle:%f \n",Left_Armor_angle,Right_Armor_angle);

    // 和整车观测不一样的方法 | 整车观测的计算过于复杂了
    double cos_r = cos(abs(spin_angle))*Outpost_R;                          
    double sin_r = sin(abs(spin_angle))*Outpost_R;                          

    Eigen::Vector3d center_temp;                                            // 圆心世界坐标(temp)
    center_temp = AS.imu2cam(Smooth_position);
    center_temp = {center_temp[0],center_temp[2],-center_temp[1]};          // 世界坐标系

    // 当前装甲板
    O_Armor[0] = center_temp[0] - sin(spin_angle)*Outpost_R;
    O_Armor[1] = center_temp[1] - cos(spin_angle)*Outpost_R;
    O_Armor[2] = center_temp[2];
    O_Armor = {O_Armor[0],-O_Armor[2],O_Armor[1]};      //相机坐标系
    O_Armor = AS.cam2imu(O_Armor);
    O_Armor_cir = AS.imu2pixel(O_Armor);

    // 左侧装甲板
    L_Armor[0] = center_temp[0] - sin(Left_Armor_angle)*Outpost_R;
    L_Armor[1] = center_temp[1] - cos(Left_Armor_angle)*Outpost_R;
    L_Armor[2] = center_temp[2];
    L_Armor = {L_Armor[0],-L_Armor[2],L_Armor[1]};      //相机坐标系
    L_Armor = AS.cam2imu(L_Armor);
    L_Armor_cir = AS.imu2pixel(L_Armor);

    // 右侧装甲板
    R_Armor[0] = center_temp[0] - sin(Right_Armor_angle)*Outpost_R;
    R_Armor[1] = center_temp[1] - cos(Right_Armor_angle)*Outpost_R;
    R_Armor[2] = center_temp[2];
    R_Armor = {R_Armor[0],-R_Armor[2],R_Armor[1]};      //相机坐标系
    R_Armor = AS.cam2imu(R_Armor);
    R_Armor_cir = AS.imu2pixel(R_Armor);
}
}

/**
 *  函数名: Outpost_Track
 *  传入: Armor armor, const chrono_time &time 装甲板和当前时间戳
 *  传出: 无                      
 *  功能: 
 */
void Outpost_Observe::Outpost_Track(Armor armor, const chrono_time &time){
    t = time;

    // 判断陀螺方向 | 目前看来有问题
    // 和4装甲板不同,3装甲板会出现丢失跟踪的情况,得使用其他方法判断陀螺方向
    Spin_direction(armor,time);

    // 陀螺圆心观测
    if(Outpost_Spin_state != UNKNOWN) 
        Center_fitting(armor);

    // 
}

/**
 *  函数名: Spin_State
 *  传入: 无
 *  传出: 无
 *  功能: 返回当前跟踪装甲板陀螺状态
 */
int Outpost_Observe::Spin_State(){
    return this->Outpost_Spin_state;
}

//==========================平滑滤波结构体==========================
bool Smooth_post::update(Eigen::Vector3d position,Eigen::Vector3d &Smooth) {
    fit = false;
    if (Data_info.size() < max_size)
    {
        Data_info.push_back(position);
    }
    else
    {
        // 隔3帧取一次或和上两帧平滑
        // 前者计算数据少
//        Data_info.clear();
        Data_info.pop_front();
        Data_info.push_back(position);
    }

    if(Data_info.size() == max_size) {
        fit = true;
        double x,y,z;
        for (int i = 0; i < max_size; ++i) {
            x += Data_info[i][0];
            y += Data_info[i][1];
           z += Data_info[i][2];
        }
        x /=  max_size;
        y /=  max_size;
        z /=  max_size;
        // z = position[2];
        Smooth[0] = x;
        Smooth[1] = y;
        Smooth[2] = z;
    }
    return true;
}