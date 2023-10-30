//
// Created by steins_xin on 23-10-11.
//

#define CENTER_FIT

#include "../include/Armor_Observe.h"

ArmorObserve::ArmorObserve() {
    small_w = 0.135;
    small_h = 0.057;
    CKF.Initial();
    Fit_OK = false;
}

/**
 *  函数名: Orientation_angle_fitting
 *  传入: double x                                (计算得到4点装甲板距离差)
 *  传出: double yaw                              (求得朝向角弧度)
 *  功能: 这是由数据拟合出的函数,传入距离差,返回yaw值
 */
double ArmorObserve::Orientation_angle_fitting(double x) {
    double yaw = (-1.13047e-5)* pow(x,4) + (0.00163)* pow(x,3) + (-0.08953)* pow(x,2) + (2.68784)* pow(x,1) - 0.50433;
    return yaw;
}

//==========================平滑滤波结构体==========================
int Smooth::deque_length() {
    return Data_info.size();
}

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
//            z += Data_info[i][2];
        }
        x /=  max_size;
        y /=  max_size;
//        z /=  max_size;
        z = position[2];
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
void ArmorObserve::Center_fitting(Armor &armor,double axes_length) {
    center_position = armor.world_position;                                 // 圆心坐标
    Armor_distance = AS.World_projection(armor,0);                // 获取装甲板4点距离差
    yaw = Orientation_angle_fitting(Armor_distance);                    // 获取装甲板朝向角(角度)
//#define A
#ifdef A
    /** 对yaw角度进行处理 */
    if(yaw < 0) yaw = 0;                                                   // 特殊情况
    double Projection_armor_LY = lu.y + ld.y;                               // 重投影装甲板的高
    double Projection_armor_RY = ru.y + rd.y;                               // 重投影装甲板的高
    double Real_armor_LY = armor.armor_pt4[0].y + armor.armor_pt4[3].y;     // 实际装甲板的高
    double Real_armor_RY = armor.armor_pt4[1].y + armor.armor_pt4[2].y;     // 实际装甲板的高
    double Height_difference = (armor.armor_pt4[0].y-armor.armor_pt4[3].y)-(armor.armor_pt4[1].y-armor.armor_pt4[2].y);
    if( Height_difference > 0) yaw = -yaw;                                  // 通过高度差判断在左边还是右边
    if(yaw > 10 && armor.R[1] < 0) yaw = -yaw;                              // 根据pnp的旋转向量确定方向
    yaw = yaw*CV_PI/180.0f;                                                 // 转换为弧度
    if(abs(armor.R[1]*180.0f/CV_PI) < 5) yaw = armor.R[1];               // 5度之内pnp效果更好
#else
    yaw = AS.Armor_Angle(armor);                                            // 获取装甲板yaw角度
    yaw = yaw*CV_PI/180.0f;                                                 // 转换为弧度
#endif //A


#ifdef CENTER_FIT
    cv::putText(_src,"yaw:"+ std::to_string(yaw*180.0f/CV_PI),cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(_src,"Distance:"+ std::to_string(armor.world_position[1]),cv::Point(0,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::putText(_src,"Armor_Distance:"+ std::to_string(Armor_distance),cv::Point(0,120),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
#endif //CENTER_FIT

    /** 计算圆心坐标 */
    center_position[0] += sin(yaw)*axes_length;                                   // 三角函数解圆心
    center_position[1] += cos(abs(yaw))*axes_length;                           // 三角函数解圆心
    Smooth_Filter.update(center_position,Smooth_position);                // 更新数据,进行平滑
    cir = AS.imu2pixel(Smooth_position);                                     // 转换像素坐标
//    cir = AS.imu2pixel(center_position);                                           // 转换像素坐标
    // 数据平滑
    if(Smooth_Filter.fit){
        /** 车圆心卡尔曼预测 */
        if(CKF.Set_init) CKF.setPosAndSpeed(Smooth_position,Eigen::Vector2d(0,0));
        CKF.setF(1);                                                    // 设置时间间隔
        //============预测步============
        pre = CKF.predict();
        //============更新步============
        CKF.update(Eigen::Vector2d(Smooth_position[0],Smooth_position[1]));
        // 预测距离差值
        double x = pow(pre[0]-center_position[0],2);
        double y = pow(pre[1]-center_position[1],2);
        double dis = std::sqrt(x+y);
//        if(dis > 0.30 && CKF.Track_OK) Smooth_Filter.fit = false;
        // 更新拟合状态
        Fit_OK = Smooth_Filter.fit && CKF.Track_OK;

        cv::putText(_src,"Smooth_position_x:"+ std::to_string(Smooth_position[0]),cv::Point(0,280),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"Smooth_position_y:"+ std::to_string(Smooth_position[1]),cv::Point(0,320),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(_src,"Smooth_position_z:"+ std::to_string(Smooth_position[2]),cv::Point(0,360),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);

//        circle(_src,cir,5,cv::Scalar(0,0,255),-1);
//        cv::imshow("Observe_src",_src);
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




