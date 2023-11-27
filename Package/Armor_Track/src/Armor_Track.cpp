//
// Created by steins_xin on 23-10-1.
//
//#define STATUS_DISPLAY                          // 显示当前状态
//#define SPIN_STATUS_DISPLAY                     // 显示陀螺状态信息

#include "../include/Armor_Track.h"

using namespace Robot;
ArmorTrack::ArmorTrack() {

    cv::FileStorage fs(PATH "Track_data.yaml", cv::FileStorage::READ);

    //=========================初始化=========================
    /** 卡尔曼 */
    KF.Initial();

    /** 初始化变量部分 */
    locate_target = false;                                  // 未锁定装甲板
    track_start = false;                                    // 第一帧初始化
    tracker_state = MISSING;                                // 状态设置为没有目标
    tracking_id = 0;                                        // 跟踪ID
    find_aim_number = 0;                                    // 锁定目标次数
    lost_aim_number = 0;                                    // 丢失目标次数

    /** 读取参数部分 */
    find_threshold = (int)fs["find_threshold"];             // 锁定目标阈值
    lost_threshold = (int)fs["lost_threshold"];             // 丢失目标阈值
    new_old_threshold = (double)fs["new_old_threshold"];    // 新旧坐标的距离阈值
    max_delta_t = (int)fs["max_delta_t"];                   // 维护跟踪器时间阈值

    spin_max_r_multiple = (double)fs["spin_max_r_multiple"];            // 反陀螺分数增加倍数
    spin_judge_low_threshold = (int)fs["spin_judge_low_threshold"];     // 陀螺最低分数阈值
    spin_judge_high_threshold = (int)fs["spin_judge_high_threshold"];   // 陀螺最高分数阈值

}

/**
 *  函数名: Initial
 *  传入: vector<Armor> &Armors      (最终装甲板)
 *  传出: bool                       (布尔变量)
 *  功能: 通过传入最终装甲板,选择锁定装甲板,进入初始化状态
 */
bool ArmorTrack::Initial(std::vector<Armor> &Armors) {
    if(Armors.empty())  return false;   //如果为空直接返回

    //====================预测初始化====================

    /** 根据装甲板的分数从大到小排序 */
    sort(Armors.begin(),Armors.end(),
         [](Armor &armor1,Armor &armor2){
             return armor1.grade > armor2.grade;});

    /** 锁定装甲板流程 */
    enemy_armor = Armors[0];                                        // 选择锁定装甲板
    enemy_armor.world_position = AS.pixel2imu(enemy_armor);      // 最终装甲板的世界坐标系为像素转换为imu坐标系
    tracking_id = enemy_armor.id;                                   // 设置跟踪id，根据最优装甲板选择
    tracker_state = DETECTING;                                      // 跟踪状态设置为初始化状态

    /** 初始化卡尔曼 */
    KF.Initial();    //卡尔曼初始化
    // 传入世界坐标系,设置初始速度为0
    KF.setPosAndSpeed(enemy_armor.world_position,Eigen::Vector3d(0,0,0));
    return true;
}

/**
 *  函数名: Reset
 *  传入: 无
 *  传出: 无
 *  功能: 重置所有状态,进行初始化
 */
void ArmorTrack::Reset() {
    /** 卡尔曼 */
    KF.Initial();                       // 卡尔曼初始化

    /** 跟踪初始化变量 */
    track_start = true;                 // 第一帧获取时间
    locate_target = false;              // 锁定装甲板

    /** 装甲板状态 */
    enemy_armor = Armor();              // 初始化锁定装甲板
    tracking_id = 0;                    // 初始化锁定ID
    tracker_state = MISSING;            // 将跟踪状态重置为没有目标

    /** 跟踪变量 */
    find_aim_number = 0;                // 锁定目标的次数初始化
    lost_aim_number = 0;                // 丢失目标的次数初始化
}

/**
 *  函数名: State_solve
 *  传入: bool match
 *  传出: 无
 *  功能: 对装甲板的4个状态进行处理(TRACKING,DETECTING,LOSING,MISSING)
 */
void ArmorTrack::State_solve(bool match) {
    /** DETECTING TRACKING状态的过渡 */
    if (tracker_state == DETECTING) {
        // DETECTING
        if (match) {                                // 匹配次数到达阈值则开始跟踪
            find_aim_number++;                      // 锁定目标次数++
            if (find_aim_number > find_threshold) {
                find_aim_number = 0;                // 清零
                tracker_state = TRACKING;           // 设置为跟踪状态
            }
        } else {  //没有目标，跳过该部分
            find_aim_number = 0;                    // 清零
            tracker_state = MISSING;                // 将状态重置为没有目标状态
        }
    }
    /** TRACKING 跟踪状态 */
    else if (tracker_state == TRACKING) {
        // TRACKING
        if (!match) {                               // 未匹配则丢失
            tracker_state = LOSING;                 // 丢失状态
            lost_aim_number++;                      // 丢失目标次数++
        }
    }
    /** LOSING 丢失状态 */
    else if (tracker_state == LOSING) {
        if (!match) {                             // 未匹配则累加丢失次数
            lost_aim_number++;
            if (lost_aim_number > lost_threshold) { // 超过丢失阈值,将跟踪器设置未没有目标
                lost_aim_number = 0;
                tracker_state = MISSING;            // 将状态重置为没有目标状态
            }
        } else {
            tracker_state = TRACKING;               // 匹配到目标则设置回跟踪状态
            lost_aim_number = 0;                    // 丢失次数清零
        }
    }
}

/**
 *  函数名: Lock_Armor
 *  传入: vector<Armor> &Armors double dt     (最终装甲板,间隔时间)
 *  传出: bool                                (布尔变量)
 *  功能: 通过传入最终装甲板和间隔时间,寻找跟踪装甲板
 */
bool ArmorTrack::Lock_Armor(std::vector<Armor> &Armors, double dt) {

    //=========================卡尔曼初始化=========================
    KF.setF(dt);                                             // 设置状态转移矩阵时间间隔
    predicted = KF.predict();                                   // 预测值矩阵
    //==========================跟踪装甲板==========================
    SpinHeading spin_status;                                    // 记录陀螺状态（未知，顺时针，逆时针）
    Armor matched_armor;                                        // 匹配装甲板
    bool match = false;                                         // 匹配状态

    //=========================锁定目标装甲板========================
    /** 装甲板不为空的情况,更新了匹配状态 */
    if(!Armors.empty()){
        double min_position_diff = DBL_MAX;                     // 初始化最小距离(DBL_MAX为double类型最大值)
        int Armor_number = 0;                                   // 当前锁定装甲板数量
        std::vector<Armor> Track_Armor;                         // 存放锁定装甲板
        /** 选择距离最近的装甲板 */
        for (auto &armor: Armors) {
            armor.world_position = AS.pixel2imu(armor);                 // 装甲板转换世界坐标系
            Eigen::Vector3d pre = predicted.head(3);                    // 预测的世界坐标系
            double position_diff = (pre - armor.world_position).norm();    // 计算坐标偏移量
            /** 判断最小距离 */
            if (position_diff < min_position_diff) {
                min_position_diff = position_diff;
                matched_armor = armor;
            }
            /** 记录跟踪装甲板ID */
            if(armor.id == tracking_id) {
                Armor_number++;
                Track_Armor.push_back(armor);
            }
        }
        /** 判断是否为锁定装甲板 */
        if (min_position_diff < new_old_threshold && matched_armor.id == tracking_id) {
            /** 匹配状态 */
            match = true;                                                   // 匹配成功
            /** 卡尔曼预测 */
            predicted = KF.update(matched_armor.world_position);        // 更新预测值 返回的是校正后的结果
        }
        /** 本帧相同装甲板 */
        else{
            /** 匹配相同ID的装甲板 */
            for (auto &armor: Armors){
                if (armor.id == tracking_id){
                    /** 匹配状态 */
                    match = true;                                                                   // 匹配成功
                    /** 卡尔曼重置 */
                    Eigen::Matrix<double, 6, 1> pos_speed;
                    KF.Initial();                                                                   // 卡尔曼初始化
                    pos_speed << armor.world_position, predicted.tail(3);                        // 保留速度变量
                    KF.setPosAndSpeed(armor.world_position, predicted.tail(3));   // 设置坐标点和3个速度变量
                    predicted = pos_speed;                                                          // 更新预测值
                    matched_armor = armor;
                    break;
                }
            }
        }

        //=========================获取整车观测所需数据========================
        // TODO: 待封装
        bool Data_capture_OK = false;                             // 数据采集
        bool Long_Short_OK = false;                               // 长短轴采集

        /** 同时出现两个装甲板 */
        if(Armor_number == 2){
            double Track_Armor_diff = abs(abs(Track_Armor[0].camera_position[1])-abs(Track_Armor[1].camera_position[1]));
            // 两装甲板Y轴距离小于阈值
            if(Track_Armor_diff < 0.1) {
                Long_Short_OK = true;
            }
            // 进入采集模式
            Data_capture_OK = true;
        }

        if(Data_capture_OK){
            Eigen::Vector3d Track_1 = Track_Armor[0].camera_position;
            Eigen::Vector3d Track_2 = Track_Armor[1].camera_position;
            Track_1 = {Track_1[0],Track_1[2],-Track_1[1]};
            Track_2 = {Track_2[0],Track_2[2],-Track_2[1]};
#define ONE // TODO: 两个计算方法,待测试
#ifdef ONE
            /** 通过装甲板朝向角和中心世界坐标系建立Y-X下的一次函数 */      //TODO: 找找更加稳定求半径的方法
            double angle = 180.0f/CV_PI;
            // 左装甲板一次函数
            double angle_1 = (90.0-Track_Armor[0].R[1]*angle)*(CV_PI/180.0f);
            double K_1 = tan(angle_1);
            double B_1 = Track_1[1]-K_1*Track_1[0];
            // 右装甲板一次函数
            double angle_2 = -(90.0-abs(Track_Armor[1].R[1])*angle)*(CV_PI/180.0f);
            double K_2 = tan(angle_2);
            double B_2 = Track_2[1]-K_2*Track_2[0];
#else
            /** 通过装甲板朝向角和中心世界坐标系建立Y-X下的一次函数 */      //TODO: 找找更加稳定求半径的方法
            double angle = 180.0f/CV_PI;
            // 左装甲板一次函数
//            double angle_1 = (90.0-Track_Armor[0].R[1]*angle)*(CV_PI/180.0f);
            double angle_1 = (90.0-abs(AS.Armor_Angle(Track_Armor[0])))*(CV_PI/180.0f);
            double K_1 = tan(angle_1);
            double B_1 = Track_1[1]-K_1*Track_1[0];
            // 右装甲板一次函数
//            double angle_2 = -(90.0-abs(Track_Armor[1].R[1])*angle)*(CV_PI/180.0f);
            double angle_2 = -(90.0-abs(AS.Armor_Angle(Track_Armor[1])))*(CV_PI/180.0f);
            double K_2 = tan(angle_2);
            double B_2 = Track_2[1]-K_2*Track_2[0];
#endif //ONE
            /** 使用矩阵计算联立方程求解交点 */
            /** 矩阵计算
             *  [ 1  -K1     [ Y     =    [ B1
             *    1  -K2 ]     X ]          B2 ]
             */

            Eigen::Matrix<double,2,2> K;
            Eigen::Matrix<double,2,1> X;
            Eigen::Matrix<double,2,1> Y;
            K << 1,-K_1,
                 1,-K_2;
            Y << B_1,
                 B_2;
            X = K.inverse()*Y;

            // 计算
            double R_1 = sqrt(pow(X[1]-Track_Armor[0].world_position[0],2)+pow(X[0]-Track_Armor[0].world_position[1],2));
            double R_2 = sqrt(pow(X[1]-Track_Armor[1].world_position[0],2)+pow(X[0]-Track_Armor[1].world_position[1],2));

            // 只有达到采集要求才会更新
            if(Long_Short_OK) OB[tracking_id].update(R_1,R_2);

//#define FUNCTION_INFO
#ifdef FUNCTION_INFO    // 输出两个一次函数方程
            std::cout << "1: " << Track_Armor[0].world_position.transpose() << std::endl;
            printf("f(x)_1: Y = %fx + %f\n",K_1,B_1);
            std::cout << "2: " << Track_Armor[1].world_position.transpose() << std::endl;
            printf("f(x)_2: Y = %fx + %f\n",K_2,B_2);
#endif // FUNCTION_INFO

            // 对左侧装甲板进行分析,判断是高长轴还是矮长轴
            if(Track_Armor[0].world_position[2] > Track_Armor[1].world_position[2]){
                if(R_1 > R_2) OB[tracking_id].Data_record[HIGH*LONG]++;      // 高长轴
                else OB[tracking_id].Data_record[HIGH*SHORT]++;              // 高短轴
            } else{
                if(R_1 > R_2) OB[tracking_id].Data_record[LOW*LONG]++;       // 矮长轴
                else OB[tracking_id].Data_record[LOW*SHORT]++;               // 矮短轴
            }
            OB[tracking_id].High_LOW_difference = abs(Track_Armor[0].world_position[2]-Track_Armor[1].world_position[2]);
            OB[tracking_id].Center_Z = (Track_Armor[0].world_position[2]+Track_Armor[1].world_position[2])/2;

            /** 判断长短轴 */
            if(OB[tracking_id].Observe_OK && tracker_state == TRACKING){
                double All = OB[tracking_id].Data_record[HIGH*LONG]+OB[tracking_id].Data_record[HIGH*SHORT]+OB[tracking_id].Data_record[LOW*LONG]+OB[tracking_id].Data_record[LOW*SHORT];
                OB[tracking_id].HIGH_LONG_pro = (OB[tracking_id].Data_record[HIGH*LONG]+OB[tracking_id].Data_record[LOW*SHORT])/All;   // 高长轴概率
                OB[tracking_id].LOW_LONG_pro = (OB[tracking_id].Data_record[LOW*LONG]+OB[tracking_id].Data_record[HIGH*SHORT])/All;    // 矮长轴概率
//                std::cout << "HIGH*LONG: " << OB[tracking_id].HIGH_LONG_pro << "    "
//                           << "LOW*LONG: " << OB[tracking_id].LOW_LONG_pro << std::endl;
                /** 更新装甲板(初始化) */
                if(!OB_Track[tracking_id].is_initialized)   // 避免出现两个装甲板时直接更新,导致无法检测跳变
                    OB_Track[tracking_id].update_tracker(matched_armor,t);
            }
//            cv::waitKey(0);
        }
    }

    //=========================跟踪状态处理===========================
    // 更新最终装甲板
    if (match) enemy_armor = matched_armor;

    /** 处理跟踪状态(MISSING,DETECTING,LOSING,TRACKING) */
    State_solve(match);
    /** 识别到两个装甲板且处于跟踪状态 */
    if(OB_Track[tracking_id].is_initialized && tracker_state == TRACKING){
        // TODO:x轴上的距离差 (解决了跳变问题)
        double diff = abs(OB_Track[tracking_id].last_armor.world_position[0] - enemy_armor.world_position[0]);

        if(diff > new_old_threshold && matched_armor.id == tracking_id ){
            // 对高低状态进行判断
            if(OB_Track[tracking_id].last_armor.world_position[2]>enemy_armor.world_position[2])
                OB_Track[tracking_id].HeightState = LOW;
            else if(OB_Track[tracking_id].last_armor.world_position[2]<enemy_armor.world_position[2])
                OB_Track[tracking_id].HeightState = HIGH;
        } else{
            // 计算角速度 TODO: 需要在跟踪器更新前
            double old_angle,new_angle,diff_angle;
            old_angle = OB_Track[tracking_id].last_armor.R[1];  // 旧角度
            new_angle = enemy_armor.R[1];                       // 新角度
            diff_angle = abs(abs(old_angle)-abs(new_angle));
            double angle_speed,time;
            time = 0.05;                        // 时间
            angle_speed = diff_angle/time;      // 角速度(rad/s)
        }

//#define ARMOR_DISPLAY
#ifdef ARMOR_DISPLAY
        // 绘制上一帧锁定装甲板
        for (int i = 0; i < 4; i++)
            line(_src, OB_Track[tracking_id].last_armor.armor_pt4[i], OB_Track[tracking_id].last_armor.armor_pt4[(i + 1) % 4], CV_RGB(255, 0, 0), 2);

        // 输出参数,待封装
        cv::putText(_src,"dif: "+ std::to_string(diff),cv::Point(0,240),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"high_old: "+ std::to_string(OB_Track[tracking_id].last_armor.world_position[2]),cv::Point(0,280),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"high_new: "+ std::to_string(enemy_armor.world_position[2]),cv::Point(0,320),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
#endif //ARMOR_DISPLAY

        OB_Track[tracking_id].update_tracker(enemy_armor, t);               // 更新长短轴跟踪器
        // 判断跟踪装甲板长短轴
        if(OB_Track[tracking_id].HeightState == HIGH){
            if(OB[tracking_id].HIGH_LONG_pro > OB[tracking_id].LOW_LONG_pro)  OB_Track[tracking_id].axesState = LONG;
            else                                                              OB_Track[tracking_id].axesState = SHORT;
        }
        else if(OB_Track[tracking_id].HeightState == LOW){
            if(OB[tracking_id].HIGH_LONG_pro > OB[tracking_id].LOW_LONG_pro)  OB_Track[tracking_id].axesState = SHORT;
            else                                                              OB_Track[tracking_id].axesState = LONG;
        }

#ifdef ARMOR_DISPLAY
        // 高低长短轴状态
        std::string axesState;
        std::string HightState;
        if(OB_Track[tracking_id].axesState == LONG) axesState = "LONG";
        else if(OB_Track[tracking_id].axesState == SHORT) axesState = "SHORT";
        if(OB_Track[tracking_id].HeightState == HIGH) HightState = "HIGH";
        else if(OB_Track[tracking_id].HeightState == LOW) HightState = "LOW";
        cv::putText(_src,"axesState: "+HightState+" "+axesState,cv::Point(0,200),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
#endif //ARMOR_DISPLAY


        // 绘制锁定目标
//        Angle_selection(enemy_armor,10);
    }

    /** LOSING 丢失目标卡尔曼继续预测 */
    if(tracker_state == LOSING)
    {
        enemy_armor.world_position = predicted.head(3);
        KF.setPosAndSpeed(enemy_armor.world_position, predicted.tail(3));
    }
    /** MISSING 没有目标状态 */
    if (tracker_state == MISSING)                   // 跟踪器处于没有目标状态
    {
        Reset();                                    // 重新初始化
        return false;
    }

#ifdef STATUS_DISPLAY
    std::string TrackerState;
    std::string Spin_State;
    if(tracker_state == DETECTING) TrackerState = "DETECTING";
    else if(tracker_state == MISSING) TrackerState = "MISSING";
    else if(tracker_state == TRACKING) TrackerState = "TRACKING";
    else if(tracker_state == LOSING) TrackerState = "LOSING";

    if(spin_status_map[tracking_id] == UNKNOWN) Spin_State = "UNKNOWN";
    else if(spin_status_map[tracking_id] == CLOCKWISE)Spin_State = "CLOCKWISE";
    else if(spin_status_map[tracking_id] == COUNTER_CLOCKWISE)Spin_State = "COUNTER_CLOCKWISE";

    cv::putText(_src,"TrackerState:"+TrackerState,cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(_src,"Spin_State"+Spin_State,cv::Point(0,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
#endif //STATUS_DISPLAY

    return true;
}

/**
 *  函数名: spin_detect
 *  传入: 无
 *  传出: 无
 *  功能: 通过enemy_armor(最终装甲板)选择跟踪目标,并为跟踪ID进行陀螺打分
 */
void ArmorTrack::Spin_detect() {

    int armor_id = tracking_id;                     // 获取跟踪ID
    Armor detect_armor = enemy_armor;               // 检测装甲板
    bool new_armors = false;                        // 判断是否有新增装甲板

    auto trackers_map_size = trackers_map.count(armor_id);     // 已存在类型的跟踪器数量

    /** 跟踪器只有1个 */
    if(trackers_map_size == 1){
        auto candidate = trackers_map.find(armor_id);           // 原有的同ID装甲板
        auto delta_dist = (detect_armor.world_position - (*candidate).second.last_armor.world_position).norm();  // 计算装甲板距离
        /** 如果距离小于阈值 */
        if (delta_dist < new_old_threshold) {
            (*candidate).second.update_tracker(detect_armor, t);  // 更新陀螺跟踪器
        }
        /** 跳变情况(为新增装甲板设置跟踪器 1->2 )或速度超出阈值 */
        else{
            SpinTracker spinTracker(detect_armor, t);       // 重新创建跟踪器
            trackers_map.insert(std::make_pair(armor_id, spinTracker)); // 预测器Map添加装甲板
            new_armors = true;                                                    // 判断为有新增装甲板
        }
    }
    /** 没有跟踪器或多个跟踪器(多个跟踪器中选择跟踪目标) */
    else{
        // 1e9无实际意义，仅用于非零初始化
        double min_delta_dist = 1e9;                                            // 最小距离
        double min_delta_t = 1e9;                                               // 最小时间
        bool is_best_candidate_exist = false;                                   // 判断最优装甲板

        std::multimap<int, SpinTracker>::iterator best_candidate;               // 记录最优的装甲板
        auto candidates = trackers_map.equal_range(armor_id);       // 获取所有当前跟踪ID跟踪器
        /** 跟踪ID相同的跟踪器(在所有跟踪目标中选择与当前锁定目标最接近的) */
        for (auto iter = candidates.first; iter != candidates.second; ++iter) {
            auto delta_dist = (detect_armor.world_position - (*iter).second.last_armor.world_position).norm();    // 计算距离
            auto delta_t = milliseconds_duration (t - (*iter).second.last_timestamp).count();                    // 计算时间间隔

            /** 在同一位置存在过装甲板且时间最接近设为最高优先级(寻找到与当前锁定目标最接近的装甲板) */
            if (delta_dist <= new_old_threshold && delta_dist <= min_delta_dist && /*时间*/delta_t < min_delta_t)  // 距离需要调试
            {
                min_delta_dist = delta_dist;
                min_delta_t = delta_t;
                best_candidate = iter;
                is_best_candidate_exist = true;
            }
        }
        /** 存在最优装甲板 */
        if (is_best_candidate_exist)  (*best_candidate).second.update_tracker(detect_armor, t); // 更新跟踪装甲板
        /** 初始化过程或跟踪丢失(跳变2->1) */
        else{
            /** 为当前锁定装甲板添加跟踪器 */
            SpinTracker spinTracker(detect_armor, t);
            trackers_map.insert(std::make_pair(static_cast<int&&>(armor_id), static_cast<SpinTracker&&>(spinTracker)));
        }
    }

    /** 跟踪器维护 */
    if (!trackers_map.empty()) {
        // 维护跟踪器Map，删除过久之前的装甲板
        for (auto iter = trackers_map.begin(); iter != trackers_map.end();) {
            // 删除元素后迭代器会失效，需先行获取下一元素
            if (milliseconds_duration(t - (*iter).second.last_timestamp).count() > max_delta_t){  //TODO：时间需要测试
                iter = trackers_map.erase(iter);
//                std::cout << "[MAP_size]: "<< trackers_map.size() << std::endl;
            } else{
                ++iter;
            }
        }
    }

    /** 出现新增装甲板 */
    if(new_armors){
        auto same_armors_number = trackers_map.count(armor_id);    // 相同的装甲板数量
        /** 如果有2个跟踪器 */
        if (same_armors_number == 2) {
            SpinTracker *new_tracker = nullptr;                                 // 新跟踪器
            SpinTracker *last_tracker = nullptr;                                // 旧跟踪器
            double new_armor_center;                                            // 新跟踪器中心x值
            double last_armor_center;                                           // 旧跟踪器中心x值

            auto candiadates = trackers_map.equal_range(armor_id);  // 获取同类型的装甲板
            /** 循环遍历跟踪器 */
            for (auto iter = candiadates.first; iter != candiadates.second; ++iter) {
                // 若未完成初始化则视为新增tracker
                if (!(*iter).second.is_initialized) {
                    new_tracker = &(*iter).second;
                } else {
                    last_tracker = &(*iter).second;
                }
            }
            /** 判断有新旧跟踪器 */
            if (new_tracker != nullptr && last_tracker != nullptr) {
                // 获取新旧装甲板的中心x值
                new_armor_center = new_tracker->last_armor.center.x;
                last_armor_center = last_tracker->last_armor.center.x;

                auto spin_movement = new_armor_center - last_armor_center;  // 中心x坐标： 新 - 旧
                if (abs(spin_movement) > 10) {
                    // 若无该元素则插入新元素
                    if (spin_score_map.count(armor_id) == 0) {
                        spin_score_map[armor_id] = 1000 * spin_movement / abs(spin_movement);
                    }
                    // 若已有该元素且目前旋转方向与记录不同,则对目前分数进行惩罚
                    else if (spin_movement * spin_score_map[armor_id] < 0) {
                        spin_score_map[armor_id] *= 0.3;
//                        std::cout << "-----------新旧装甲板判定---------" << std::endl;
//                        std::cout << "new_tracker_x:" << new_tracker->last_armor.center.x << std::endl;
//                        std::cout << "last_tracker_x:" << last_tracker->last_armor.center.x << std::endl;
//                        std::cout << "new_armors:" << new_armors << std::endl;
                    }
                    // 若已有该元素则更新元素
                    else {
                        spin_score_map[armor_id] = spin_max_r_multiple * spin_score_map[armor_id];
                    }
                }
            }
        }
    }
    /** 对陀螺分数map进行维护和判断 */
    UpdateSpinScore();

#ifdef SPIN_STATUS_DISPLAY
    cv::Mat src = _src.clone();
    // 绘制当前锁定装甲板
    for (int i = 0; i < 4; i++)
    {
        line(src, detect_armor.armor_pt4[i], detect_armor.armor_pt4[(i + 1) % 4], CV_RGB(0, 255, 255), 2);
        circle(src,detect_armor.center,4,CV_RGB(0, 255, 255),-1);
    }
    // 输出角度
    std::string yaw_angle = std::to_string(enemy_armor.R[1]*180.0f/CV_PI);
    cv::putText(src,"yaw:"+yaw_angle,cv::Point(0,120),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    // 输出陀螺状态
    std::string Spin_State;
    if(spin_status_map[tracking_id] == UNKNOWN) Spin_State = "UNKNOWN";
    else if(spin_status_map[tracking_id] == CLOCKWISE)Spin_State = "CLOCKWISE";
    else if(spin_status_map[tracking_id] == COUNTER_CLOCKWISE)Spin_State = "COUNTER_CLOCKWISE";
    cv::putText(src,"Spin_State:"+Spin_State,cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    // 输出当前跟踪ID
    cv::putText(src,"Tracker_ID:"+ std::to_string(tracking_id),cv::Point(0,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    // 输出陀螺分数
    for (auto score = spin_score_map.begin(); score != spin_score_map.end();) {
        cv::putText(src,"Spin_Score:"+ std::to_string((*score).second),cv::Point(0,160),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        score++;
    }
    cv::imshow("spin_status",src);
#endif //SPIN_STATUS_DISPLAY
}

/**
 *  函数名: UpdateSpinScore
 *  传入: 无
 *  传出: 无
 *  功能: 自然衰减状态分数，将分数低于阈值的去除，高于最大阈值的限制在最大值,维护反陀螺map
 */
void ArmorTrack::UpdateSpinScore() {
    // spin_score_map记录各装甲板小陀螺可能性分数，大于0顺时针旋转，小于0为逆时针旋转
    for (auto score = spin_score_map.begin(); score != spin_score_map.end();){
        SpinHeading spin_status;                                // 记录陀螺状态（未知，顺时针，逆时针）
        /** spin_status_map不存在跟踪ID元素 */
        if (spin_status_map.count((*score).first) == 0)
            spin_status = UNKNOWN;
        else
            spin_status = spin_status_map[(*score).first];

        /** 陀螺分数过低 */
        if (abs((*score).second) <= spin_judge_low_threshold && spin_status != UNKNOWN){
            /** 将储存数据清空 */
            spin_status_map.erase((*score).first);           // 删除陀螺状态记录
            score = spin_score_map.erase(score);        // 删除陀螺分数记录
            continue;                                           // 跳转下一次循环
        }

        /** 陀螺分数自然衰减 */
        if (spin_status != UNKNOWN)
            (*score).second = 0.933 * (*score).second;
        else
            (*score).second = 0.987 * (*score).second;

        /** 异常情况处理 */
        if (abs((*score).second) < 20 || isnan((*score).second)){
            spin_status_map.erase((*score).first);
            score = spin_score_map.erase(score);
            continue;
        }

        /** 陀螺情况判别 */
        if (abs((*score).second) >= spin_judge_high_threshold){
            // 根据分数的正负判断陀螺方向
            (*score).second = spin_judge_high_threshold * abs((*score).second) / (*score).second;
            if ((*score).second > 0){
                spin_status_map[(*score).first] = CLOCKWISE;
            }
            else if((*score).second < 0){
                spin_status_map[(*score).first] = COUNTER_CLOCKWISE;
            }
        }
        ++score;
    }
}

bool ArmorTrack::Track(const cv::Mat &src, std::vector<Armor> &Armors, const chrono_time &time) {
    _src=src;                                                   // 复制原始图像给_src
    /** 处于未锁定装甲板状态 */
    if(!locate_target){
        // 初始化成功进入锁定装甲板状态
        if(Initial(Armors)) locate_target = true;
        else locate_target = false;
        // 接收电控发的角度
        Solve_pitch = AS.Robot_msg.Controller_pitch;
        Solve_yaw = AS.Robot_msg.Controller_yaw;
        return false;
    }
    /** 锁定装甲板状态 */
    else{
        if(track_start){
            t = time;
            track_start = false;
            return false;
        }
        double dt = seconds_duration (time - t).count();    // 计算时间间隔
        t = time;                                              // 更新时间
        /** 锁定装甲板 */
        if(!Lock_Armor(Armors,dt)) return false;
        /** 已锁定装甲板,进行枪管角度计算 */
        Eigen::Vector3d rpy = AS.Barrel_Solve(enemy_armor.world_position);
        Solve_pitch = rpy[1];
        Solve_yaw = rpy[2];

        /** 进入跟踪状态,进行陀螺检测 */
        if(tracker_state == TRACKING){
            Spin_detect();                                      // 陀螺状态检测
        }

    }
    return true;
}

/**
 *  函数名: Angle_selection
 *  传入: Armor &armor, double angle                      (装甲板,击打角度)
 *  传出: 无
 *  功能: 通过传入的装甲板和角度,绘制出在击打范围内的装甲板
 */
void ArmorTrack::Angle_selection(Armor &armor, double angle) {
    double yaw = AS.Armor_Angle(armor);
    cv::Mat src = _src.clone();
    if(abs(yaw) < angle){
        // 绘制锁定装甲板
        for (int i = 0; i < 4; i++)
            line(src, armor.armor_pt4[i], armor.armor_pt4[(i + 1) % 4], CV_RGB(255,0, 255), 2);
        cv::putText(src,"yaw:"+std::to_string(yaw),cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    }
    cv::imshow("Angle_selection",src);
}

/**
 *  函数名: show
 *  传入: 无
 *  传出: 无
 *  功能: 展示结果
 */
void ArmorTrack::show() {
    cv::Mat src = _src.clone();
    // 绘制当前锁定装甲板
    for (int i = 0; i < 4; i++)
    {
        line(src, enemy_armor.armor_pt4[i], enemy_armor.armor_pt4[(i + 1) % 4], CV_RGB(0, 255, 255), 2);
        circle(src,enemy_armor.center,4,CV_RGB(0, 255, 255),-1);
    }
    //!             putText部分
    // 跟踪状态
    std::string Tracker_State[4] = {"MISSING","DETECTING","LOSING","TRACKING"};
    cv::putText(src,"Tracker_State:"+ Tracker_State[tracker_state],cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"Tracker_ID:"+ std::to_string(tracking_id),cv::Point(0,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

    // 串口信息(原始数据)
    cv::putText(src,"Controller_pitch:"+ std::to_string(AS.Robot_msg.Controller_pitch),cv::Point(0,140),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"Controller_yaw:"+ std::to_string(AS.Robot_msg.Controller_yaw),cv::Point(0,180),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"Controller_roll:"+ std::to_string(AS.Robot_msg.Controller_roll),cv::Point(0,220),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

    // 解算数据
    cv::putText(src,"Solve_pitch:"+ std::to_string(Solve_pitch),cv::Point(0,280),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"Solve_yaw:"+ std::to_string(Solve_yaw),cv::Point(0,320),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

    // 跟踪装甲板世界坐标和相机坐标
    cv::putText(src,"World_x:"+ std::to_string(enemy_armor.world_position[0]),cv::Point(1000,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"World_y:"+ std::to_string(enemy_armor.world_position[1]),cv::Point(1000,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"World_z:"+ std::to_string(enemy_armor.world_position[2]),cv::Point(1000,120),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

    cv::putText(src,"Camera_x:"+ std::to_string(enemy_armor.camera_position[0]),cv::Point(1000,180),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"Camera_y:"+ std::to_string(enemy_armor.camera_position[1]),cv::Point(1000,220),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(src,"Camera_z:"+ std::to_string(enemy_armor.camera_position[2]),cv::Point(1000,240),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

    cv::imshow("Track_show",src);
    cv::waitKey(1);
}

//=========================整车观测结构体=======================

bool Observe::update(double R_1, double R_2) {
    Observe_OK = false;
    double Long;
    double Short;
    // 判断长短
    if(R_1 > R_2) {
        Long = R_1;
        Short = R_2;
    } else{
        Long = R_2;
        Short = R_1;
    }

    if (Data_info.size() < max_size)
    {
        Data_info.emplace_back(Long,Short);
    }
    else
    {
        // 隔3帧取一次或和上两帧平滑
        // 前者计算数据少
//        Data_info.clear();
        Data_info.pop_front();
        Data_info.emplace_back(Long,Short);
    }

    if(Data_info.size() == max_size) {
        Observe_OK = true;
        // 计算长短轴距离
        double Long_temp = 0;
        double Short_temp = 0;
        for (int i = 0; i < max_size; ++i) {
            Long_temp += Data_info[i][0];
            Short_temp += Data_info[i][1];
        }
        Long_temp /= max_size;
        Short_temp /= max_size;
        Long_axes = Long_temp;
        Short_axes = Short_temp;

    }
    return true;
}


