//
// Created by steins_xin on 23-10-11.
//

#define CENTER_FIT

#include "../include/Armor_Observe.h"

ArmorObserve::ArmorObserve() {
    //============================= 参数文件路径 =============================
    cv::FileStorage fs(PATH "Observe_data.yaml", cv::FileStorage::READ);

    /** 装甲板实际大小参数读取 */
    big_w = (double)fs["big_w"];
    big_h = (double)fs["big_h"];
    small_w = (double)fs["small_w"];
    small_h = (double)fs["small_h"];

    fs.release();
    // 初始化
    CKF.Initial();
    Fit_OK = false;
    CKF.Set_init = true;
}


//==========================平滑滤波结构体==========================
bool Smooth::update(Eigen::Vector3d position,Eigen::Vector3d &Smooth) {
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

/**
 *  函数名: Center_fitting
 *  传入: Armor &armor   double r                     (装甲板,当前装甲板对应长短轴)
 *  传出: 无
 *  功能: 通过计算得到的yaw值,车的半径,拟合车的圆心
 */
void ArmorObserve::Center_fitting(Armor &armor,double axes_length,double z,Observe OB,SpinTracker OB_Track) {
    // center_position = armor.world_position;                                 // 圆心坐标

    center_position = armor.camera_position;                                 // 圆心坐标
    center_position = {center_position[0],center_position[2],-center_position[1]};      //世界坐标系
    
    Armor_distance = AS.World_projection(armor,0);                // 获取装甲板4点距离差

#define A
#ifdef A
    yaw = armor.R[1];
#else
    yaw = AS.Armor_Angle(armor);                                            // 获取装甲板yaw角度
    yaw = yaw*CV_PI/180.0f;                                                 // 转换为弧度
#endif //A


#ifdef CENTER_FIT
    cv::putText(_src,"yaw:"+ std::to_string(yaw*180.0f/CV_PI),cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(_src,"Armor_Distance:"+ std::to_string(Armor_distance),cv::Point(0,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
#endif //CENTER_FIT

    /** 高低装甲板和长短轴参数 */
    double OF_length;                                               // 前后装甲板高度
    double RL_length;                                               // 左右装甲板高度
    double OF_height;                                               // 前后装甲板高度
    double RL_height;                                               // 左右装甲板高度

    // 判断当前装甲板是什么轴
    if(OB_Track.axesState == LONG) {                                // 长轴
        OF_length = OB.Long_axes;
        RL_length = OB.Short_axes;
    }
    else if(OB_Track.axesState == SHORT) {                          // 短轴
        OF_length = OB.Short_axes;
        RL_length = OB.Long_axes;
    }
    if(OB_Track.HeightState == HIGH){                               // 高轴
        RL_height = -OB.High_LOW_difference;
    }
    else if(OB_Track.HeightState == LOW) {                          // 矮轴
        RL_height = OB.High_LOW_difference;
    }

    /** 计算装甲板中心点坐标参数 */
    double OF_cos_r = cos(abs(spin_angle))*OF_length;                           // 半径在x轴方向投影
    double RL_cos_r = cos(abs(spin_angle))*RL_length;                           // 半径在x轴方向投影
    double OF_sin_r = sin(abs(spin_angle))*OF_length;                           // 半径在y轴方向投影
    double RL_sin_r = sin(abs(spin_angle))*RL_length;                           // 半径在y轴方向投影


    /** 计算圆心坐标 */  // TODO: 需要将在相机坐标系下操作再转换到世界坐标系
    center_position[0] += sin(yaw)*axes_length;                                   // 三角函数解圆心
    center_position[1] += cos(abs(yaw))*axes_length;                           // 三角函数解圆心
    center_position = {center_position[0],-center_position[2],center_position[1]};      //相机坐标系
    center_position = AS.cam2imu(center_position);
    Smooth_Filter.update(center_position,Smooth_position);                // 更新数据,进行平滑

    // 平滑
    // Smooth_position = {Smooth_position[0],-Smooth_position[2],Smooth_position[1]};      //相机坐标系
    // Smooth_position = AS.cam2imu(Smooth_position);
//    Smooth_position[2] = z;
    Smooth_position[2] = OB.center_Z;
    cir = AS.imu2pixel(Smooth_position);                                           // 转换像素坐标

    // 非平滑
    // center_position = {center_position[0],-center_position[2],center_position[1]};      //相机坐标系
    // center_position = AS.cam2imu(center_position);
    // center_position[2] = z;
    // cir = AS.imu2pixel(center_position);                                           // 转换像素坐标

    // 数据平滑
    if(Smooth_Filter.fit){

        /** 车圆心卡尔曼预测 */ //TODO: 后面研究
//        if(CKF.Set_init) {
//            CKF.setPosAndSpeed(Smooth_position,Eigen::Vector2d(0,0));
//            CKF.predict();
//        }
//        // CKF.setPosAndSpeed(Smooth_position,Eigen::Vector2d(0,0));
//        //============更新步============
//        CKF.update(Eigen::Vector2d(Smooth_position[0],Smooth_position[1]));
//        //============预测步============
//        CKF.setF(0.1);                                                    // 设置时间间隔(计算出实际速度)
//        pre = CKF.predict();
//        // std::cout << pre.transpose() << std::endl;
//
//        cv::Point2f pre_cir;                                    // 圆心像素坐标
//        pre_pos = {pre[0],pre[1],z};
//
//        // TODO: 需要调整倍率,达到实际圆心点
//        // pre_pos[0] = pre_pos[0] + 0.75*pre[2];
//        // pre_pos[1] = pre_pos[1] + pre[3];
//
//        // 预测装甲板
//        pre_Armor = AS.imu2cam(pre_pos);
//        pre_Armor = {pre_Armor[0],pre_Armor[2],-pre_Armor[1]};  // 世界坐标系
//
//        pre_Armor[0] -= sin(yaw)*axes_length;
//        pre_Armor[1] -= cos(abs(yaw))*axes_length;
//
//        pre_cir = AS.imu2pixel(pre_pos);



        cv::putText(_src,"Smooth_position_x:"+ std::to_string(Smooth_position[0]),cv::Point(0,280),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"Smooth_position_y:"+ std::to_string(Smooth_position[1]),cv::Point(0,320),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"Smooth_position_z:"+ std::to_string(Smooth_position[2]),cv::Point(0,360),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"spend:"+ std::to_string(pre[2]),cv::Point(0,400),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"anglespend:"+ std::to_string(Angle_Speed),cv::Point(0,440),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"OF_length:"+ std::to_string(OF_length),cv::Point(0,480),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"RL_length:"+ std::to_string(RL_length),cv::Point(0,520),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"RL_height:"+ std::to_string(RL_height),cv::Point(0,560),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

        // TODO：注意顺逆时针 这里都是逆时针
        Eigen::Vector3d center_temp;                    // 圆心世界坐标(temp)
        cv::Point2f spin_Aromor_cir;                    // 陀螺装甲板
        double FPS_size = 1;                            // 帧率倍率(待调整) TODO:不能调过大,会导致跳过角度检测,试试计算(距离/运动时间*角速度)进行补偿   慢速:10(展示)(已弃)
        // 已进入调参阶段
        double compensate;                              // 弹道时间角度补偿
        double Dial_time;                               // 拨盘转动时间 (新增)（待调整）
        double time;                                    // 距离使用车中心试试？还是使用装甲板的距离？ 现使用车中心看看

        double Diff = sqrt(pow(Smooth_position[0],2)+pow(Smooth_position[1],2));
        double bullet_speed = 25;
        double compensate_size = 1;                     // 倍率 TODO:测试可行,有上升空间(待测试)
        time = Diff/bullet_speed;                       // 单位: s
        Dial_time = 80;                                // 单位: ms
        compensate = compensate_size*(time*1000)*(Angle_Speed) + Dial_time*(Angle_Speed);               // 计算补偿角度 (等待测试看效果)
        std::cout << "compensate:" << compensate << std::endl;
        // std::cout << "anglespend:" << Angle_Speed << std::endl;
        // std::cout << "time:" << time << std::endl;
        cv::putText(_src,"compensate:"+ std::to_string(compensate),cv::Point(0,600),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

        // 计算预测角度 (帧数补偿设置为15ms)
        spin_angle = yaw*(180.0f/CV_PI) - FPS_size*(Angle_Speed*15) - compensate;          // 陀螺装甲板所在角度(逆时针)
        spin_angle *= (CV_PI/180.0f);
        center_temp = AS.imu2cam(Smooth_position);
        center_temp = {center_temp[0],center_temp[2],-center_temp[1]}; // 世界坐标系

        spin_Aromor[0] = center_temp[0] - sin(spin_angle)*OF_length;
        spin_Aromor[1] = center_temp[1] - cos(spin_angle)*OF_length;
        spin_Aromor[2] = center_temp[2]-(RL_height/2);

        spin_Aromor = {spin_Aromor[0],-spin_Aromor[2],spin_Aromor[1]};      //相机坐标系
        spin_Aromor = AS.cam2imu(spin_Aromor);
        spin_Aromor_cir = AS.imu2pixel(spin_Aromor);
        // 计算其他点位
        Left_Armor_angle = spin_angle*(180.0f/CV_PI) + 90.0;
        Right_Armor_angle = spin_angle*(180.0f/CV_PI) - 90.0;

        std::cout << spin_angle*(180.0f/CV_PI) << std::endl;
        std::cout << Left_Armor_angle << std::endl;

        // TODO:长短轴和高低装甲板问题(Test中)

        // L_center_imu = {center[0]-RL_cos_r,center[1]+RL_sin_r,center[2]+RL_height};
        // L_center_imu = {center[0]-RL_cos_r,center[1]-RL_sin_r,center[2]+RL_height};
        // R_center_imu = {center[0]+RL_cos_r,center[1]-RL_sin_r,center[2]+RL_height};
        // R_center_imu = {center[0]+RL_cos_r,center[1]+RL_sin_r,center[2]+RL_height};
        // O_center_imu = {center[0]+OF_sin_r,center[1]+OF_cos_r,center[2]};
        // O_center_imu = {center[0]-OF_sin_r,center[1]+OF_cos_r,center[2]};
        // F_center_imu = {center[0]-OF_sin_r,center[1]-OF_cos_r,center[2]};
        // F_center_imu = {center[0]+OF_sin_r,center[1]-OF_cos_r,center[2]};
        if(yaw > 0){

            // Left_Armor = {center_temp[0] - RL_cos_r,center_temp[1] + RL_sin_r,center_temp[2]+(RL_height/2)};
            Left_Armor = {center_temp[0] - RL_cos_r,center_temp[1] - RL_sin_r,center_temp[2]};//test
            Right_Armor = {center_temp[0] + RL_cos_r,center_temp[1] - RL_sin_r,center_temp[2]+(RL_height/2)};
            O_Armor = {center_temp[0] + OF_sin_r,center_temp[1] + OF_cos_r,center_temp[2]-(RL_height/2)};

        }else{
            // Left_Armor = {center_temp[0] - RL_cos_r,center_temp[1] - RL_sin_r,center_temp[2]+(RL_height/2)}; 
            Left_Armor = {center_temp[0] - RL_cos_r,center_temp[1] - RL_sin_r,center_temp[2]};//test
            Right_Armor = {center_temp[0] + RL_cos_r,center_temp[1] + RL_sin_r,center_temp[2]+(RL_height/2)};
            O_Armor = {center_temp[0] - OF_sin_r,center_temp[1] + OF_cos_r,center_temp[2]-(RL_height/2)};
        }
        // 转换部分,待优化
        Left_Armor = {Left_Armor[0],-Left_Armor[2],Left_Armor[1]};      //相机坐标系
        Left_Armor = AS.cam2imu(Left_Armor);
        Left_Armor_cir = AS.imu2pixel(Left_Armor);
        // 转换部分,待优化
        Right_Armor = {Right_Armor[0],-Right_Armor[2],Right_Armor[1]};      //相机坐标系
        Right_Armor = AS.cam2imu(Right_Armor);
        Right_Armor_cir = AS.imu2pixel(Right_Armor);
          // 转换部分,待优化
        O_Armor = {O_Armor[0],-O_Armor[2],O_Armor[1]};      //相机坐标系
        O_Armor = AS.cam2imu(O_Armor);
        O_Armor_cir = AS.imu2pixel(O_Armor);

        // 计算左右30度(暂定,可调) TODO:长短轴高低装甲板等待计算处理

        cv::Point2f left_target_Armor;                    // 左侧装甲板点位
        cv::Point2f right_target_Armor;                   // 右侧装甲板点位
        left_target[0] = center_temp[0] - sin(30*(CV_PI/180.0f))*RL_length;
        left_target[1] = center_temp[1] - cos(30*(CV_PI/180.0f))*RL_length;
        left_target[2] = center_temp[2]+(RL_height/2);
        right_target[0] = center_temp[0] + sin(30*(CV_PI/180.0f))*RL_length;
        right_target[1] = center_temp[1] - cos(30*(CV_PI/180.0f))*RL_length;
        right_target[2] = center_temp[2]+(RL_height/2);
        // 转换部分,待优化
        left_target = {left_target[0],-left_target[2],left_target[1]};      //相机坐标系
        left_target = AS.cam2imu(left_target);
        left_target_Armor = AS.imu2pixel(left_target);
        // 转换部分,待优化
        right_target = {right_target[0],-right_target[2],right_target[1]};      //相机坐标系
        right_target = AS.cam2imu(right_target);
        right_target_Armor = AS.imu2pixel(right_target);
        

        // 更新拟合状态
        // Fit_OK = Smooth_Filter.fit && CKF.Track_OK;
        Fit_OK = Smooth_Filter.fit;
        // Fit_OK = true;

        cv::Point2f Armor_cir;                                    
        cv::Point2f pre_Armor_cir;                                    
        Armor_cir = AS.imu2pixel(armor.world_position);
        pre_Armor = {pre_Armor[0],-pre_Armor[2],pre_Armor[1]};      //相机坐标系
        pre_Armor = AS.cam2imu(pre_Armor);
        pre_Armor_cir = AS.imu2pixel(pre_Armor);

        circle(_src,cir,5,cv::Scalar(0,0,255),-1);
        // circle(_src,Armor_cir,5,cv::Scalar(0,255,255),-1);
        circle(_src,spin_Aromor_cir,5,cv::Scalar(255,0,255),-1);

        // 左右预测点位
        // circle(_src,Left_Armor_cir,5,cv::Scalar(255,0,255),-1);
        // circle(_src,Right_Armor_cir,5,cv::Scalar(255,0,255),-1);
        // circle(_src,O_Armor_cir,5,cv::Scalar(255,0,255),-1);

        // 移动点位
        // circle(_src,left_target_Armor,5,cv::Scalar(0,255,0),-1);
        circle(_src,right_target_Armor,5,cv::Scalar(0,255,0),-1);

        // circle(_src,pre_Armor_cir,5,cv::Scalar(255,0,255),-1);
        // circle(_src,pre_cir,5,cv::Scalar(0,255,0),-1);
        // std::cout << pre_cir << std::endl;

        // cv::imshow("Observe_src",_src);
    }
}

/**
 *  函数名: World_projection_show
 *  传入: 无
 *  传出: 无
 *  功能: 绘制重投影装甲板
 */
void ArmorObserve::World_projection_show() {
    line(_src,lu,ld,cv::Scalar(255,0,255),2);
    line(_src,ld,rd,cv::Scalar(255,0,255),2);
    line(_src,rd,ru,cv::Scalar(255,0,255),2);
    line(_src,ru,lu,cv::Scalar(255,0,255),2);
}

/**
 *  函数名: ArmorObserve_show
 *  传入: 无
 *  传出: 无
 *  功能: 绘制整车观侧的4个装甲板和圆心
 */
void ArmorObserve::ArmorObserve_show(Eigen::Matrix<double, 3, 1> center,Observe OB,SpinTracker OB_Track) {

    /** 绘制圆心 */
    circle(_src,cir,5,cv::Scalar(0,0,255),-1);
    /** 相对当前装甲板方位 */
    cv::Point2f L_Armor_pt4[4];                                             // 左下角开始逆时针
    cv::Point2f R_Armor_pt4[4];                                             // 左下角开始逆时针
    cv::Point2f O_Armor_pt4[4];                                             // 左下角开始逆时针
    cv::Point2f F_Armor_pt4[4];                                             // 左下角开始逆时针
    cv::Point2f L_center_P,R_center_P,O_center_P,F_center_P;                // 左、右、后、前装甲板中心像素坐标
    Eigen::Vector3d L_center_imu,R_center_imu,O_center_imu,F_center_imu;    // 左、右、后、前装甲板中心世界坐标

    /** 装甲板计算参数(世界坐标参考系下) */
    double Armor_H_sin15 = 0.007376;                                // 装甲板在倾斜15度时对地面的投影长度的一半
    double cos_Armor_H_sin15 = Armor_H_sin15*cos(abs(yaw));   // 计算x轴方向投影
    double sin_Armor_H_sin15 = Armor_H_sin15*sin(abs(yaw));   // 计算y轴方向投影
    double Armor_cos_r = cos(abs(yaw))*(small_w/2);           // 装甲板宽在x轴投影
    double Armor_sin_r = sin(abs(yaw))*(small_w/2);           // 装甲板宽在y轴投影
    /** 高低装甲板和长短轴参数 */
    double OF_length;                                               // 前后装甲板高度
    double RL_length;                                               // 左右装甲板高度
    double OF_height;                                               // 前后装甲板高度
    double RL_height;                                               // 左右装甲板高度

    // 判断当前装甲板是什么轴
    if(OB_Track.axesState == LONG) {                                // 长轴
        OF_length = OB.Long_axes;
        RL_length = OB.Short_axes;
    }
    else if(OB_Track.axesState == SHORT) {                          // 短轴
        OF_length = OB.Short_axes;
        RL_length = OB.Long_axes;
    }
    if(OB_Track.HeightState == HIGH){                               // 高轴
        RL_height = -OB.High_LOW_difference;
    }
    else if(OB_Track.HeightState == LOW) {                          // 矮轴
        RL_height = OB.High_LOW_difference;
    }

    /** 计算装甲板中心点坐标参数 */
    double OF_cos_r = cos(abs(yaw))*OF_length;                           // 半径在x轴方向投影
    double RL_cos_r = cos(abs(yaw))*RL_length;                           // 半径在x轴方向投影
    double OF_sin_r = sin(abs(yaw))*OF_length;                           // 半径在y轴方向投影
    double RL_sin_r = sin(abs(yaw))*RL_length;                           // 半径在y轴方向投影

/**
 *  这里有两种方法,一种是使用旋转矩阵进行位姿解算(耗时长,比较难理解,但是理解之后方便修改代码)
 *  另一种是直接计算偏差进行位姿解算(代码过长,不利于维护,之间赋值的方法节省了时间,易理解)
 */
//#define EULER_ANGLE
#ifdef EULER_ANGLE
    //! (世界坐标系)
    // TODO: 特别注意EulerAngle2RotationMatrix返回的的顺序 R_z * R_y * R_x 是先*R_x
    //       在这部分坐标系转换中,必须先对 pitch 进行转换(R_x)

    if(yaw > 0){
        L_center_imu = {center[0]-RL_cos_r,center[1]+RL_sin_r,center[2]+RL_height};
        R_center_imu = {center[0]+RL_cos_r,center[1]-RL_sin_r,center[2]+RL_height};
        O_center_imu = {center[0]+OF_sin_r,center[1]+OF_cos_r,center[2]};
        F_center_imu = {center[0]-OF_sin_r,center[1]-OF_cos_r,center[2]};
    }
    else {
        L_center_imu = {center[0]-RL_cos_r,center[1]-RL_sin_r,center[2]+RL_height};
        R_center_imu = {center[0]+RL_cos_r,center[1]+RL_sin_r,center[2]+RL_height};
        O_center_imu = {center[0]-OF_sin_r,center[1]+OF_cos_r,center[2]};
        F_center_imu = {center[0]+OF_sin_r,center[1]-OF_cos_r,center[2]};
    }
    //! 左侧装甲板
    Eigen::Vector3d temp;                               // 临时变量
    double RL_yaw = 90.0-(yaw*180.0f/CV_PI);

    Eigen::Matrix3d L_R = AS.EulerAngle2RotationMatrix(15,0,RL_yaw);
    temp = {-small_w/2,0,-(small_h/2)};     temp = L_R*temp;
    temp = {L_center_imu[0]+temp[0],L_center_imu[1],L_center_imu[2]+temp[2]};
    L_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
    temp = {small_w/2,0,-(small_h/2)};      temp = L_R*temp;
    temp = {L_center_imu[0]+temp[0],L_center_imu[1],L_center_imu[2]+temp[2]};
    L_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
    temp = {small_w/2,0,(small_h/2)};       temp = L_R*temp;
    temp = {L_center_imu[0]+temp[0],L_center_imu[1],L_center_imu[2]+temp[2]};
    L_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
    temp = {-small_w/2,0,(small_h/2)};      temp = L_R*temp;
    temp = {L_center_imu[0]+temp[0],L_center_imu[1],L_center_imu[2]+temp[2]};
    L_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

    //! 右侧装甲板
    Eigen::Matrix3d R_R = AS.EulerAngle2RotationMatrix(-15,0,RL_yaw);
    temp = {-small_w/2,0,-(small_h/2)};     temp = R_R*temp;
    temp = {R_center_imu[0]+temp[0],R_center_imu[1],R_center_imu[2]+temp[2]};
    R_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
    temp = {small_w/2,0,-(small_h/2)};      temp = R_R*temp;
    temp = {R_center_imu[0]+temp[0],R_center_imu[1],R_center_imu[2]+temp[2]};
    R_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
    temp = {small_w/2,0,(small_h/2)};       temp = R_R*temp;
    temp = {R_center_imu[0]+temp[0],R_center_imu[1],R_center_imu[2]+temp[2]};
    R_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
    temp = {-small_w/2,0,(small_h/2)};      temp = R_R*temp;
    temp = {R_center_imu[0]+temp[0],R_center_imu[1],R_center_imu[2]+temp[2]};
    R_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

    //! 后侧装甲板
    double OF_yaw = -(yaw*180.0f/CV_PI);
    Eigen::Matrix3d O_R = AS.EulerAngle2RotationMatrix(15,0,OF_yaw);
    temp = {-small_w/2,0,-(small_h/2)};     temp = O_R*temp;
    temp = {O_center_imu[0]+temp[0],O_center_imu[1],O_center_imu[2]+temp[2]};
    O_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
    temp = {small_w/2,0,-(small_h/2)};      temp = O_R*temp;
    temp = {O_center_imu[0]+temp[0],O_center_imu[1],O_center_imu[2]+temp[2]};
    O_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
    temp = {small_w/2,0,(small_h/2)};       temp = O_R*temp;
    temp = {O_center_imu[0]+temp[0],O_center_imu[1],O_center_imu[2]+temp[2]};
    O_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
    temp = {-small_w/2,0,(small_h/2)};      temp = O_R*temp;
    temp = {O_center_imu[0]+temp[0],O_center_imu[1],O_center_imu[2]+temp[2]};
    O_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

    //! 当前装甲板
    Eigen::Matrix3d F_R = AS.EulerAngle2RotationMatrix(-15,0,OF_yaw);
    temp = {-small_w/2,0,-(small_h/2)};     temp = F_R*temp;
    temp = {F_center_imu[0]+temp[0],F_center_imu[1],F_center_imu[2]+temp[2]};
    F_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
    temp = {small_w/2,0,-(small_h/2)};      temp = F_R*temp;
    temp = {F_center_imu[0]+temp[0],F_center_imu[1],F_center_imu[2]+temp[2]};
    F_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
    temp = {small_w/2,0,(small_h/2)};       temp = F_R*temp;
    temp = {F_center_imu[0]+temp[0],F_center_imu[1],F_center_imu[2]+temp[2]};
    F_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
    temp = {-small_w/2,0,(small_h/2)};      temp = F_R*temp;
    temp = {F_center_imu[0]+temp[0],F_center_imu[1],F_center_imu[2]+temp[2]};
    F_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

#else
    /* 装甲板在左侧的情况 */
    if(yaw > 0) {
        /** 先构建平面装甲板,再创建倾斜角度15度！！ */
        Eigen::Vector3d temp;                           // 临时变量
        /** 左侧装甲板中心坐标 */
        L_center_imu = {center[0]-RL_cos_r,center[1]+RL_sin_r,center[2]+RL_height};

        temp = {L_center_imu[0]+Armor_sin_r,L_center_imu[1]+Armor_cos_r,L_center_imu[2]-(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {L_center_imu[0]-Armor_sin_r,L_center_imu[1]-Armor_cos_r,L_center_imu[2]-(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {L_center_imu[0]-Armor_sin_r,L_center_imu[1]-Armor_cos_r,L_center_imu[2]+(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {L_center_imu[0]+Armor_sin_r,L_center_imu[1]+Armor_cos_r,L_center_imu[2]+(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

        /** 右侧装甲板中心坐标 */
        R_center_imu = {center[0]+RL_cos_r,center[1]-RL_sin_r,center[2]+RL_height};

        temp = {R_center_imu[0]-Armor_sin_r,R_center_imu[1]-Armor_cos_r,R_center_imu[2]-(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {R_center_imu[0]+Armor_sin_r,R_center_imu[1]+Armor_cos_r,R_center_imu[2]-(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {R_center_imu[0]+Armor_sin_r,R_center_imu[1]+Armor_cos_r,R_center_imu[2]+(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {R_center_imu[0]-Armor_sin_r,R_center_imu[1]-Armor_cos_r,R_center_imu[2]+(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

        /** 后侧装甲板中心坐标 */
        O_center_imu = {center[0]+OF_sin_r,center[1]+OF_cos_r,center[2]};

        temp = {O_center_imu[0]+Armor_cos_r,O_center_imu[1]-Armor_sin_r,O_center_imu[2]-(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {O_center_imu[0]-Armor_cos_r,O_center_imu[1]+Armor_sin_r,O_center_imu[2]-(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {O_center_imu[0]-Armor_cos_r,O_center_imu[1]+Armor_sin_r,O_center_imu[2]+(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {O_center_imu[0]+Armor_cos_r,O_center_imu[1]-Armor_sin_r,O_center_imu[2]+(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

        /** 当前装甲板 */
        F_center_imu = {center[0]-OF_sin_r,center[1]-OF_cos_r,center[2]};

        temp = {F_center_imu[0]-Armor_cos_r,F_center_imu[1]+Armor_sin_r,F_center_imu[2]-(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {F_center_imu[0]+Armor_cos_r,F_center_imu[1]-Armor_sin_r,F_center_imu[2]-(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {F_center_imu[0]+Armor_cos_r,F_center_imu[1]-Armor_sin_r,F_center_imu[2]+(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {F_center_imu[0]-Armor_cos_r,F_center_imu[1]+Armor_sin_r,F_center_imu[2]+(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上
    }
    /* 装甲板在右侧的情况 */
    else{
        Eigen::Vector3d temp;                           // 临时变量
        /** 左侧装甲板中心坐标 */
        L_center_imu = {center[0]-RL_cos_r,center[1]-RL_sin_r,center[2]+RL_height};

        temp = {L_center_imu[0]-Armor_sin_r,L_center_imu[1]+Armor_cos_r,L_center_imu[2]-(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {L_center_imu[0]+Armor_sin_r,L_center_imu[1]-Armor_cos_r,L_center_imu[2]-(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {L_center_imu[0]+Armor_sin_r,L_center_imu[1]-Armor_cos_r,L_center_imu[2]+(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {L_center_imu[0]-Armor_sin_r,L_center_imu[1]+Armor_cos_r,L_center_imu[2]+(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        L_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

        /** 右侧装甲板中心坐标 */
        R_center_imu = {center[0]+RL_cos_r,center[1]+RL_sin_r,center[2]+RL_height};

        temp = {R_center_imu[0]+Armor_sin_r,R_center_imu[1]-Armor_cos_r,R_center_imu[2]-(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {R_center_imu[0]-Armor_sin_r,R_center_imu[1]+Armor_cos_r,R_center_imu[2]-(small_h/2)};
        temp = {temp[0]+cos_Armor_H_sin15,temp[1]+sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {R_center_imu[0]-Armor_sin_r,R_center_imu[1]+Armor_cos_r,R_center_imu[2]+(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {R_center_imu[0]+Armor_sin_r,R_center_imu[1]-Armor_cos_r,R_center_imu[2]+(small_h/2)};
        temp = {temp[0]-cos_Armor_H_sin15,temp[1]-sin_Armor_H_sin15,temp[2]};
        R_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

        /** 后侧装甲板中心坐标 */
        O_center_imu = {center[0]-OF_sin_r,center[1]+OF_cos_r,center[2]};

        temp = {O_center_imu[0]+Armor_cos_r,O_center_imu[1]+Armor_sin_r,O_center_imu[2]-(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {O_center_imu[0]-Armor_cos_r,O_center_imu[1]-Armor_sin_r,O_center_imu[2]-(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {O_center_imu[0]-Armor_cos_r,O_center_imu[1]-Armor_sin_r,O_center_imu[2]+(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {O_center_imu[0]+Armor_cos_r,O_center_imu[1]+Armor_sin_r,O_center_imu[2]+(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        O_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上

        /** 当前装甲板 */
        F_center_imu = {center[0]+OF_sin_r,center[1]-OF_cos_r,center[2]};

        temp = {F_center_imu[0]-Armor_cos_r,F_center_imu[1]-Armor_sin_r,F_center_imu[2]-(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[0] = AS.imu2pixel(temp);        // 左下
        temp = {F_center_imu[0]+Armor_cos_r,F_center_imu[1]+Armor_sin_r,F_center_imu[2]-(small_h/2)};
        temp = {temp[0]+sin_Armor_H_sin15,temp[1]-cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[1] = AS.imu2pixel(temp);        // 右下
        temp = {F_center_imu[0]+Armor_cos_r,F_center_imu[1]+Armor_sin_r,F_center_imu[2]+(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[2] = AS.imu2pixel(temp);        // 右上
        temp = {F_center_imu[0]-Armor_cos_r,F_center_imu[1]-Armor_sin_r,F_center_imu[2]+(small_h/2)};
        temp = {temp[0]-sin_Armor_H_sin15,temp[1]+cos_Armor_H_sin15,temp[2]};
        F_Armor_pt4[3] = AS.imu2pixel(temp);        // 左上
    }
#endif //EULER_ANGLE

    /** 转换为中心像素坐标 */
    L_center_P = AS.imu2pixel(L_center_imu);
    R_center_P = AS.imu2pixel(R_center_imu);
    O_center_P = AS.imu2pixel(O_center_imu);
    F_center_P = AS.imu2pixel(F_center_imu);

    /** 绘制中心像素坐标 */
    circle(_src,L_center_P,4,cv::Scalar(255,255,255),-1);
    circle(_src,R_center_P,4,cv::Scalar(255,255,255),-1);
    circle(_src,O_center_P,4,cv::Scalar(255,255,255),-1);
    circle(_src,F_center_P,4,cv::Scalar(255,255,255),-1);

    /** 绘制装甲板 */
    for (int i = 0; i < 4; ++i) {
        /** 左侧装甲板 */
        line(_src,L_Armor_pt4[0],L_Armor_pt4[1],cv::Scalar(0,255,0),2);
        line(_src,L_Armor_pt4[1],L_Armor_pt4[2],cv::Scalar(0,255,0),2);
        line(_src,L_Armor_pt4[2],L_Armor_pt4[3],cv::Scalar(0,255,0),2);
        line(_src,L_Armor_pt4[3],L_Armor_pt4[0],cv::Scalar(0,255,0),2);
        /** 右侧装甲板 */
        line(_src,R_Armor_pt4[0],R_Armor_pt4[1],cv::Scalar(0,255,0),2);
        line(_src,R_Armor_pt4[1],R_Armor_pt4[2],cv::Scalar(0,255,0),2);
        line(_src,R_Armor_pt4[2],R_Armor_pt4[3],cv::Scalar(0,255,0),2);
        line(_src,R_Armor_pt4[3],R_Armor_pt4[0],cv::Scalar(0,255,0),2);
        /** 后侧装甲板 */
        line(_src,O_Armor_pt4[0],O_Armor_pt4[1],cv::Scalar(0,255,0),2);
        line(_src,O_Armor_pt4[1],O_Armor_pt4[2],cv::Scalar(0,255,0),2);
        line(_src,O_Armor_pt4[2],O_Armor_pt4[3],cv::Scalar(0,255,0),2);
        line(_src,O_Armor_pt4[3],O_Armor_pt4[0],cv::Scalar(0,255,0),2);
        /** 前侧装甲板 */
        line(_src,F_Armor_pt4[0],F_Armor_pt4[1],cv::Scalar(0,255,0),2);
        line(_src,F_Armor_pt4[1],F_Armor_pt4[2],cv::Scalar(0,255,0),2);
        line(_src,F_Armor_pt4[2],F_Armor_pt4[3],cv::Scalar(0,255,0),2);
        line(_src,F_Armor_pt4[3],F_Armor_pt4[0],cv::Scalar(0,255,0),2);
    }
//    std::cout << "F_imu: " << F_center_imu << std::endl;
    cv::putText(_src,"Long_axes:"+ std::to_string(OB.Long_axes),cv::Point(0,200),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(_src,"Short_axes:"+ std::to_string(OB.Short_axes),cv::Point(0,240),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

    cv::imshow("Observe_src",_src);
}




