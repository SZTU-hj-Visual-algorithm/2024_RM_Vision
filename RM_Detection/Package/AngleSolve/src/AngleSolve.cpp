//
// Created by steins_xin on 23-9-29.
//


//#define SHOW_MEASURE_RRECT          //绘制pnp测距结果

#include "../include/AngleSolve.h"

//================================内部封装函数================================

AngleSolve::AngleSolve() {

    cv::FileStorage fs(PATH "Solve_data.yaml", cv::FileStorage::READ);

    /** 装甲板实际大小参数读取 */
    big_w = (double)fs["big_w"];
    big_h = (double)fs["big_h"];
    small_w = (double)fs["small_w"];
    small_h = (double)fs["small_h"];

    /** 符叶实际参数读取 */
    buff_w = (double)fs["buff_w"];
    buff_h = (double)fs["buff_h"];

    /** 相机参数矩阵读取 */
    fs["F_MAT"] >> F_MAT;
    fs["C_MAT"] >> C_MAT;
    //将Mat转换为Eigen
    cv::cv2eigen(F_MAT,F_EGN);
    cv::cv2eigen(C_MAT,C_EGN);

    fs.release();
}


/**
 *  函数名: pnpSolve
 *  传入: Point2f *p , int type  Matrix<double, 3, 1>& R     (装甲板4点坐标,装甲板类型,旋转矩阵)
 *  传出: Vector3d tv                                        (平移向量)
 *  功能: 通过传入坐标和类型,计算出距离和装甲板朝向角                (像素坐标->相机坐标)
 */
Eigen::Vector3d AngleSolve::pnpSolve(cv::Point2f *p, int type,Eigen::Matrix<double, 3, 1>& R)
{
    std::vector<cv::Point3d> ps;        // 世界坐标系
    std::vector<cv::Point2f> pu;        // 像素坐标系
    double w;                           // 宽度
    double h;                           // 高度
    //=======================宽高选择=======================
    if(type == SMALL)                   // 小装甲板
    {
        w = small_w;
        h = small_h;
//        std::cout << "small" << std::endl;
    }
    else if(type == BIG)                // 大装甲板
    {
//        w = big_w;
//        h = big_h;
        w = small_w;
        h = small_h;
//        std::cout << "BIG" << std::endl;
    }

    tv = Eigen::Vector3d();                 //平移向量
    rv = Eigen::Matrix<double,3,3>();       //旋转矩阵
    //=======================坐标系========================
    /** 世界坐标系 */
    ps = {
            {-w / 2 , -h / 2, 0.},      //左上
            { w / 2 , -h / 2, 0.},      //右上
            { w / 2 ,  h / 2, 0.},      //右下
            {-w / 2 ,  h / 2, 0.}       //左下
    };
    /** 像素坐标系 */
    pu.push_back(p[3]);                          //左上
    pu.push_back(p[2]);                          //右上
    pu.push_back(p[1]);                          //右下
    pu.push_back(p[0]);                          //左下

    cv::Mat rvec;
    cv::Mat tvec;

    // pnp测距
    cv::solvePnP(ps, pu, F_MAT, C_MAT, rvec, tvec/*, SOLVEPNP_IPPE*/);
//    cv::solvePnP(ps, pu, F_MAT, C_MAT, rvec, tvec, false,cv::SOLVEPNP_IPPE);

    Eigen::Matrix<double, 3, 1> R_v;
    cv::cv2eigen(rvec,R_v);
    R = R_v;

    cv::Mat rv_mat;
    cv::Rodrigues(rvec,rv_mat);     // 将旋转向量转为旋转矩阵
    cv::cv2eigen(rv_mat,rv);
    cv::cv2eigen(tvec, tv);

#ifdef SHOW_MEASURE_RRECT
    cv::Mat pnp_check = _src.clone();
    std::cout<<"rv"<<rv<<std::endl;

    //=========================坐标系转换==============================

    cv::Point2f m_lu,m_ld,m_ru,m_rd;             //定义4点坐标
    Eigen::Vector3d imuPoint;                    //世界坐标系
    Eigen::Vector3d armorPoint;                  //相机坐标系

    imuPoint = {-w / 2 , -h / 2, 0.};   //世界坐标系左上
    armorPoint = rv*imuPoint + tv;
    m_lu = cam2pixel(armorPoint);

    imuPoint = {-w / 2 , h / 2, 0.};    //世界坐标系左下
    armorPoint = rv*imuPoint + tv;
    m_ld = cam2pixel(armorPoint);

    imuPoint = {w / 2 , -h / 2, 0.};    //世界坐标系左下
    armorPoint = rv*imuPoint + tv;
    m_ru = cam2pixel(armorPoint);

    imuPoint = {w / 2 , h / 2, 0.};     //世界坐标系左下
    armorPoint = rv*imuPoint + tv;
    m_rd = cam2pixel(armorPoint);

    /** 将坐标用圆形表示 */
    circle(pnp_check,m_lu,3,cv::Scalar(0,255,0),-1);
    circle(pnp_check,m_ld,3,cv::Scalar(255,255,0),-1);
    circle(pnp_check,m_ru,3,cv::Scalar(0,0,255),-1);
    circle(pnp_check,m_rd,3,cv::Scalar(0,255,255),-1);

    /** 将坐标用线连接 */
    line(pnp_check,m_lu,m_ld,cv::Scalar(0,0,0),2);
    line(pnp_check,m_ld,m_rd,cv::Scalar(255,0,0),2);
    line(pnp_check,m_rd,m_ru,cv::Scalar(255,0,255),2);
    line(pnp_check,m_ru,m_lu,cv::Scalar(255,255,0),2);

    /** 输出4点坐标和 */
    std::cout<<"m_lu:"<<m_lu<<std::endl;
    std::cout<<"m_ld:"<<m_ld<<std::endl;
    std::cout<<"m_ru:"<<m_ru<<std::endl;
    std::cout<<"m_rd:"<<m_rd<<std::endl;
    std::cout<<"tvec:"<<cam2pixel(tv)<<std::endl;

    imshow("pnp_check",pnp_check);
#endif
    return tv;
}

/**
 *  函数名: pnpSolve
 *  传入: Point2f *p , int type       (装甲板4点坐标,装甲板类型)
 *  传出: Vector3d tv                 (平移向量)
 *  功能: 通过传入坐标和类型,计算出距离    (像素坐标->相机坐标)
 */
Eigen::Vector3d AngleSolve::pnpSolve_buff(cv::Point2f *p, int type) {

    std::vector<cv::Point3d> ps;        // 世界坐标系
    std::vector<cv::Point2f> pu;        // 像素坐标系

    if(type == BUFF_NO){
        ps = {
                {-buff_w / 2 , -buff_h , 0.},
                { buff_w / 2 , -buff_h , 0.},
                { buff_w / 2 ,  buff_h , 0.},
                {-buff_w / 2 ,  buff_h , 0.},
                {0. , 0., 0.},
        };
        pu.push_back(p[3]);
        pu.push_back(p[2]);
        pu.push_back(p[1]);
        pu.push_back(p[0]);
        pu.push_back(p[4]);
    }
    else if(type == BUFF_YES){
        ps = {
                {-buff_w / 2 , -buff_h , 0.},
                { buff_w / 2 , -buff_h , 0.},
                { buff_w / 2 ,  buff_h , 0.},
                {-buff_w / 2 ,  buff_h , 0.},
                {0. , 0., 0.},
        };
        pu.push_back(p[3]);
        pu.push_back(p[2]);
        pu.push_back(p[1]);
        pu.push_back(p[0]);
        pu.push_back(p[4]);
    }
    else std::cout << "BUFF Category Error！";

    tv_buff = Eigen::Vector3d();                 //平移向量
    rv_buff = Eigen::Matrix<double,3,3>();       //旋转矩阵

    cv::Mat rvec;
    cv::Mat tvec;

    cv::solvePnP(ps, pu, F_MAT, C_MAT, rvec, tvec/*, SOLVEPNP_IPPE*/);
    cv::cv2eigen(tvec, tv);

    return tv_buff;
}



/**
 *  函数名: cam2pixel
 *  传入: Vector3d cam_pos            (相机坐标)
 *  传出: Point2f pixel_pos           (像素坐标)
 *  功能: 通过传入相机坐标,转换为像素坐标
 *  https://www.cnblogs.com/wangguchangqing/p/8126333.html  (推导)
 */
cv::Point2f AngleSolve::cam2pixel(Eigen::Vector3d cam_pos) {
    Eigen::Vector3d tmp_pixel;
    // 使用内参矩阵将相机坐标转换为像素坐标
    tmp_pixel = F_EGN * cam_pos;
    // 将z的距离归一化
    cv::Point2f pixel_pos = cv::Point2f((float)tmp_pixel[0]/tmp_pixel[2],(float)tmp_pixel[1]/tmp_pixel[2]);

    return pixel_pos;
}

/**
 *  函数名: imu2cam
 *  传入: Vector3d imu_pos            (世界坐标)
 *  传出: Vector3d cam_pos            (相机坐标)
 *  功能: 通过传入世界坐标,转换为相机坐标
 */
Eigen::Vector3d AngleSolve::imu2cam(Eigen::Vector3d imu_pos) {
    Eigen::Vector3d tmp_pos;

    // 使用逆矩阵将坐标系反推回去
    // 暂时未写到四元数
//    tmp_pos = RotationMatrix_imu.inverse() * imu_pos;   //逆矩阵（3x3）逆矩阵的定义是，逆矩阵乘以原矩阵等于单位矩阵
//    tmp_pos -= center_offset_position;                  //将原点偏差补偿回去
    tmp_pos = imu_pos;

    Eigen::Vector3d cam_pos;
    cam_pos = {tmp_pos[0],-tmp_pos[2],tmp_pos[1]};//将坐标系换回相机坐标系
    return cam_pos;
}

/**
 *  函数名: pixel2cam
 *  传入: Armor &armor                (装甲板结构体)
 *  传出: Vector3d camera_position    (相机坐标)
 *  功能: 为传入装甲板赋予相机坐标,返回相机坐标
 */
Eigen::Vector3d AngleSolve::pixel2cam(Armor &armor) {
    // 传入装甲板的坐标和类型
    armor.camera_position = pnpSolve(armor.armor_pt4,armor.type,armor.R);
    return armor.camera_position;
}

/**
 *  函数名: cam2imu
 *  传入: Vector3d cam_pos            (相机坐标)
 *  传出: Vector3d imu_pos            (世界坐标)
 *  功能: 通过传入相机坐标,转换为世界坐标
 */
Eigen::Vector3d AngleSolve::cam2imu(Eigen::Vector3d cam_pos) {
    Eigen::Vector3d pos_tmp;
    Eigen::Vector3d imu_pos;

    // 左右手系，单靠旋转矩阵转换不了，与其矩阵运算不如直接赋值
    pos_tmp = {cam_pos[0],cam_pos[2],-cam_pos[1]};      //世界坐标系
//    std::cout<<"cam_pos: "<<cam_pos.transpose()<<std::endl;
//    std::cout<<"pos_tmp: "<<pos_tmp.transpose()<<std::endl;

    // 暂时没写到四元数,将相机坐标系翻转之间得到世界坐标系
//    pos_tmp += center_offset_position;          //原点偏移
//    imu_pos = RotationMatrix_imu * pos_tmp;     //旋转矩阵
    imu_pos = pos_tmp;

    return imu_pos;
}

//================================API接口================================
/**
 *  函数名: imu2pixel
 *  传入: Vector3d imu_pos            (世界坐标)
 *  传出: Vector3d pixel_pos          (像素坐标)
 *  功能: 通过传入世界坐标,转换为像素坐标
 */
cv::Point2f AngleSolve::imu2pixel(Eigen::Vector3d imu_pos) {
    Eigen::Vector3d cam_pos = imu2cam(imu_pos);
    cv::Point2f pixel_pos = cam2pixel(cam_pos);
    return pixel_pos;
}


/**
 *  函数名: pixel2imu
 *  传入: Armor &armor                (装甲板结构体)
 *  传出: Vector3d camera_position    (相机坐标)
 *  功能: 通过传入装甲板结构体,转换为世界坐标
 */
Eigen::Vector3d AngleSolve::pixel2imu(Armor &armor) {
    armor.camera_position = pixel2cam(armor);
    Eigen::Vector3d imu_pos = cam2imu(armor.camera_position);
    return imu_pos;
}

/**
 *  函数名: quaternionToRotationMatrix
 *  传入: 无
 *  传出: Matrix3d R_x
 *  功能: 通过电控发来的四元数,计算出旋转举证
 */
Eigen::Matrix3d AngleSolve::quaternionToRotationMatrix() {
    Eigen::Matrix3d R_x;
    // 四元数
    float w = robot_state::quaternion[0];
    float x = robot_state::quaternion[1];
    float y = robot_state::quaternion[2];
    float z = robot_state::quaternion[3];
    R_x << 1-2*y*y-2*z*z, 2*x*y-2*z*w, 2*x*z+2*y*w,
            2*x*y+2*z*w, 1-2*x*x-2*z*z, 2*y*z-2*x*w,
            2*x*z-2*y*w, 2*y*z+2*w*x, 1-2*x*x-2*y*y;

//    float roll = atan2(2*y*z + 2*w*x,1 - 2*x*x - 2*y*y)/CV_PI * 180.0f;
//    float pitch = asin(2*w*y - 2*x*z)/CV_PI*180.0f;
//    float yaw = atan2(2*x*y + 2*w*z, 1 - 2*y*y - 2*z*z)/CV_PI*180.0f;
//     std::cout<<"----------[quaternion_euler]-----------"<<std::endl;
//     std::cout<<"[roll:]   |"<<roll<<std::endl;
//     std::cout<<"[pitch:]  |"<<pitch<<std::endl;
//     std::cout<<"[yaw:]    |"<<yaw<<std::endl;
//     std::cout<<"----------[get_from_euler]-----------"<<std::endl;
//     std::cout<<"[get_roll:]     |"<<ab_roll<<std::endl;
//     std::cout<<"[get_pitch:]    |"<<ab_pitch<<std::endl;
//     std::cout<<"[get_yaw:]      |"<<ab_yaw<<std::endl;

    return R_x;
}

/**
 *  函数名: pixel2cam_buff
 *  传入: cv::Point2f *p, int type
 *  传出: Matrix3d cam_pos
 *  功能: 将像素坐标系转换为相机坐标系(BUFF)
 */
Eigen::Vector3d AngleSolve::pixel2cam_buff(cv::Point2f *p, int type) {
    Eigen::Vector3d cam_pos = pnpSolve_buff(p,type);
    return cam_pos;
}

/**
 *  函数名: pixel2cam_buff
 *  传入: cv::Point2f *p, int type
 *  传出: Matrix3d cam_pos
 *  功能: 将像素坐标系转换为世界坐标系(BUFF)
 */
Eigen::Vector3d AngleSolve::pixel2imu_buff(cv::Point2f *p, int type) {
    Eigen::Vector3d cam_pos = pixel2cam_buff(p,type);
    Eigen::Vector3d imu_pos = cam2imu(cam_pos);
    return imu_pos;
}

/**
 *  函数名: World_projection
 *  传入: Armor &armor, double Angle
 *  传出: Matrix3d cam_pos
 *  功能: 计算重投影装甲板和实际装甲板4点坐标距离
 */
double AngleSolve::World_projection(Armor &armor, double Angle) {
    /** 世界坐标系转换 */
    // 手写坐标系进行装甲板重投影 | pitch = 15 | roll = 0 |
    // sin15度 = 0.258819045103
    // cos15度 = 0.965925826289
    /** 偏移参数 */
    double sin15 = 0.258819045103;
    double cos15 = 0.965925826289;
    double Yaw = Angle*CV_PI/180.0f;                        // Yaw值(弧度)
    double Armor_H_sin15 = sin15*(small_h/2);               // 装甲板在倾斜15度时投影宽度
    double Armor_H_cos15 = cos15*(small_h/2);               // 装甲板在倾斜15度时投影高度
    double cos_Armor_H_sin15 = Armor_H_sin15*cos(Yaw);   // 计算x轴方向投影
    double sin_Armor_H_sin15 = Armor_H_sin15*sin(Yaw);   // 计算y轴方向投影
    double Armor_cos_r = cos(Yaw)*(small_w/2);           // 装甲板宽在x轴投影
    double Armor_sin_r = sin(Yaw)*(small_w/2);           // 装甲板宽在y轴投影

    imuPoint_ld = {armor.world_position[0]-Armor_cos_r,armor.world_position[1]+Armor_sin_r,armor.world_position[2]-Armor_H_cos15};
    imuPoint_ld = {imuPoint_ld[0]-sin_Armor_H_sin15,imuPoint_ld[1]-cos_Armor_H_sin15,imuPoint_ld[2]};
    ld = imu2pixel(imuPoint_ld);         // 左下
    imuPoint_rd = {armor.world_position[0]+Armor_cos_r,armor.world_position[1]-Armor_sin_r,armor.world_position[2]-Armor_H_cos15};
    imuPoint_rd = {imuPoint_rd[0]-sin_Armor_H_sin15,imuPoint_rd[1]-cos_Armor_H_sin15,imuPoint_rd[2]};
    rd = imu2pixel(imuPoint_rd);         // 右下
    imuPoint_lu = {armor.world_position[0]-Armor_cos_r,armor.world_position[1]+Armor_sin_r,armor.world_position[2]+Armor_H_cos15};
    imuPoint_lu = {imuPoint_lu[0]+sin_Armor_H_sin15,imuPoint_lu[1]+cos_Armor_H_sin15,imuPoint_lu[2]};
    lu = imu2pixel(imuPoint_lu);         // 左上
    imuPoint_ru = {armor.world_position[0]+Armor_cos_r,armor.world_position[1]-Armor_sin_r,armor.world_position[2]+Armor_H_cos15};
    imuPoint_ru = {imuPoint_ru[0]+sin_Armor_H_sin15,imuPoint_ru[1]+cos_Armor_H_sin15,imuPoint_ru[2]};
    ru = imu2pixel(imuPoint_ru);         // 右上


    double dis_lu = POINT_DIST(lu,armor.armor_pt4[3]);      // 左上
    double dis_ld = POINT_DIST(ld,armor.armor_pt4[0]);      // 左下
    double dis_ru = POINT_DIST(ru,armor.armor_pt4[2]);      // 右上
    double dis_rd = POINT_DIST(rd,armor.armor_pt4[1]);      // 右下
    double Dis = (dis_lu+dis_ld+dis_ru+dis_rd);
//    double Dis = (dis_lu+dis_ld+dis_ru+dis_rd)/4;

    return Dis;
}

/**
 *  函数名: Armor_Angle
 *  传入: Armor &armor
 *  传出: double yaw                  (角度,自行转为弧度)
 *  功能: 实际重投影坐标与装甲板距离-yaw为下凹函数,使用三分法进行求解
 */
double AngleSolve::Armor_Angle(Armor &armor) {

    double yaw;
    double l = -90,r = 90;              // 角度范围
    double eps = 1e-8;                  // 精度

    while (r-l >= eps) {
        double thridPart = (r-l)/3;

        double lsec = l + thridPart;
        double rsec = r - thridPart;

        double ls_dis = World_projection(armor,lsec);
        double rs_dis = World_projection(armor,rsec);
        if(ls_dis > rs_dis)     l = lsec;
        else                    r = rsec;
    }
    // 处理异常情况
    if(World_projection(armor,l) > World_projection(armor,-l)) l = -l;
    yaw = l;
    return yaw;
}


/**
 *  函数名: EulerAngle2RotationMatrix
 *  传入: double pitch, double roll, double yaw       (pitch角度,roll角度,yaw角度)
 *  传出: Eigen::Matrix3d R                           (旋转矩阵)
 *  功能: 通过传入的欧拉角求出旋转矩阵
 *  PS: 为什么不使用静态欧拉角,因为计算次数过多,只要不传入两个90度就不会出现万向锁问题
 */
Eigen::Matrix3d AngleSolve::EulerAngle2RotationMatrix(double pitch, double roll, double yaw) {
    // 弧度制
    pitch *= CV_PI/180.0f;
    roll *= CV_PI/180.0f;
    yaw *= CV_PI/180.0f;

    Eigen::Matrix3d R_x;    // 计算旋转矩阵的X分量
    R_x <<
            1,              0,               0,
            0,  cos(pitch),  -sin(pitch),
            0,  sin(pitch),   cos(pitch);

    Eigen::Matrix3d R_y;    // 计算旋转矩阵的Y分量
    R_y <<
            cos(roll),   0, sin(roll),
            0,   1,             0,
            -sin(roll),  0, cos(roll);

    Eigen::Matrix3d R_z;    // 计算旋转矩阵的Z分量
    R_z <<
            cos(yaw), -sin(yaw), 0,
            sin(yaw),  cos(yaw), 0,
            0,              0,             1;
    Eigen::Matrix3d R = R_x * R_y * R_z;

    return R;
}

void AngleSolve::AngleSolve_show(Armor &armor) {
    // 装甲板中心世界坐标
    Eigen::Vector3d Center_World = pixel2imu(armor);
    // xyz三轴坐标,用于旋转
    Eigen::Vector3d X_World = {small_h,0.,0.};
    Eigen::Vector3d Y_World = {0.,0.,-small_h};
    Eigen::Vector3d Z_World = {0,-small_h,0.};

    // 绘制朝向角(两种)
    double yaw = Armor_Angle(armor);
//    double yaw = armor.R[1]*180.0/CV_PI;
    // 旋转矩阵
    Eigen::Matrix3d R = EulerAngle2RotationMatrix(-15,0,-yaw);
    X_World = R*X_World;
    Y_World = R*Y_World;
    Z_World = R*Z_World;
    // 添加坐标偏置
    X_World = {Center_World[0]+X_World[0],Center_World[1]+X_World[1],Center_World[2]+X_World[2]};
    Y_World = {Center_World[0]+Y_World[0],Center_World[1]+Y_World[1],Center_World[2]+Y_World[2]};
    Z_World = {Center_World[0]+Z_World[0],Center_World[1]+Z_World[1],Center_World[2]+Z_World[2]};
    // 坐标转换
    cv::Point2f x,y,z;
    x = imu2pixel(X_World);
    y = imu2pixel(Y_World);
    z = imu2pixel(Z_World);
    // 绘制坐标系
    line(_src,armor.center,x,cv::Scalar(0,255,0),4);
    line(_src,armor.center,y,cv::Scalar(255,0,0),4);
    line(_src,armor.center,z,cv::Scalar(0,0,255),4);
    // 输出结果
    cv::putText(_src,"yaw:"+ std::to_string(yaw),cv::Point(0,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
    cv::imshow("AngleSolve",_src);
}



