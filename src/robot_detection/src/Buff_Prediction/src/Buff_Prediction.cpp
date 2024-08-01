#include "BuffPrediction.h"



BuffPrediction::BuffPrediction()
{
    std::string package_path = ros::package::getPath("robot_detection");
    cv::FileStorage fs(package_path + "/SetParam_File/Buff_data.yaml", cv::FileStorage::READ);
   

    SMALL_Pre_Angle_Gain = (float) fs["SMALL_Pre_Angle_Gain"];
    DELAY_TIME_ = (double) fs["DELAY_TIME_"] ;

    fs.release();



}

//转换成弧度
float angleToRadian(float p)
{
    return p * CV_PI  / 180.0f;
}

//程序接口

void BuffPrediction::Prediction( cv::Mat &src , BuffTarget & Target)
{
    /**
            @brief 计算帧率
             unsigned int startTime = cv::getTickCount();
             unsigned int elapsedTime = cv::getTickCount() - startTime;
             fps = cv::getTickFrequency() / elapsedTime;
     */

//        src.copyTo(frame);

    KnowDirection(Target);

    addAngele(Target);

    get_pre_points(Target);

    // OpenFire();

    show_message(src, Target);

}

void BuffPrediction::Prediction_Init()
{
    _sameLeaf = false;
    Direction_Init = false;
    last_angle = 0 ;
    if(!_buffAngleList.empty()){_buffAngleList.clear();}
    if(!data.empty()){data.clear();}
    if(!fire.fire_dataList.empty()){fire.fire_dataList.clear();} 
}

float BuffPrediction::SMALL_buff_PredictionAngle()
{
    return 12.55;
}


//用来判断顺逆时针旋转
void BuffPrediction::KnowDirection(BuffTarget &buffTargetPrediction) {
    if(!Direction_Init){ // 如何没有初始化
        static float lastCircleAngle = 0.0f;
        static float circleAngleBias = 0.0f;
        static cv::Point2f lastTargetRectCenter = cv::Point2f(0.0f, 0.0f);
        circleAngleBias = buffTargetPrediction.leaf_angle - lastCircleAngle;
        //判断是否是同一片叶片
        if ((fabsf(circleAngleBias) > 10.0f || (fabsf(lastTargetRectCenter.y - buffTargetPrediction.buff_center.y) > 40.0f) ||
             (fabsf(lastTargetRectCenter.x - buffTargetPrediction.buff_center.x) > 40.0f))) {
            _sameLeaf = false;
        } else {
            _sameLeaf = true;
        }

        if (!_sameLeaf) {
            _buffAngleList.clear();//角度变化过大认为叶片跳变

        } else {
            _buffAngleList.push_back(buffTargetPrediction.leaf_angle);
        }
        
        
            //数据足够判断方向
            if (_buffAngleList.size() > 10) {//存十个数据用来判断
                float averageAngle = 0.0f;
                float buff = 0.0f;
                uint8_t count = 0;
                //逐差法计算
                for (size_t i = 0; i < _buffAngleList.size() / 2; i++) {
                    buff = _buffAngleList[_buffAngleList.size() / 2 + i] - _buffAngleList[i];
                    if (!buff) {
                        continue;
                    } else {
                        averageAngle += buff;
                        count++;
                    }
                }
                if (count) {
                    averageAngle = averageAngle / count;
                }
                //判断旋转方向
                if (averageAngle < -1.0f) {
                    direction = CLOCKWISE;
                } else if (averageAngle > 1.0f) {
                    direction = ANTICLOCKWISE;
                } else {
                    direction = STOP;
                }

                Direction_Init = true;
            }
            else
            {
                direction = STOP;
            }
   

        lastCircleAngle = buffTargetPrediction.leaf_angle;
        lastTargetRectCenter = buffTargetPrediction.buff_center;
     }
}



void BuffPrediction::calculateRotateSpeed(BuffTarget &buffTargetPrediction)
{
    //定义静态过去和现在角度；
    static double nowAngle = 0.0f;
    static double lastAngle = 0.0f;
    static int count = 0;
    //定义过去和现在时间
    static double lastTime = (double) cv::getTickCount() / cv::getTickFrequency() * 1000; // ms
    double curTime = (double) cv::getTickCount() / cv::getTickFrequency() * 1000;
//    double curTime_ = (double) cv::getTickCount() /  cv::getTickFrequency()*1000.0  ;
//    std::cout<<curTime_<<std::endl;
    //如果叶片没有跳变，则把过去和现在角度以及过去和现在速度置零
//        std::cout<<leaf_.angle<<std::endl;
    if (!_sameLeaf) {
        lastAngle = nowAngle = _rotateSpeed.lastRotateSpeed = _rotateSpeed.nowRotateSpeed = 0.0f;
        return;
    }

    //如果过去角度已经被清零，则过去角度进行初始化为现在绝对角度
    if (lastAngle == 0.0f) {
        lastAngle = buffTargetPrediction.leaf_angle;
        return;
    }

    //每0.1s一次数据刷新
    if (curTime - lastTime < 100 ) {
        return;
    }
    //帧数递增
    count++;
    nowAngle = buffTargetPrediction.leaf_angle;
    //计算实时角速度
    _rotateSpeed.nowRotateSpeed = (float) fabs( angleToRadian((nowAngle - lastAngle)) * (1000.0f / (curTime - lastTime)));


    //过去角度和时间更新
    lastAngle = nowAngle;
    lastTime = curTime;
    //如果过去角速度已被清零，则对过去速度进行更新
    if (_rotateSpeed.lastRotateSpeed == 0.0f) {
        _rotateSpeed.lastRotateSpeed = _rotateSpeed.nowRotateSpeed;
        return;
    }
    //防止出现异常数据
    if (_rotateSpeed.nowRotateSpeed > 5 || _rotateSpeed.nowRotateSpeed < -5) {
        return;
    }

    //如果速度没有替换最小速度，则计数加1
    if (_speedRange.nowMinSpeed > _rotateSpeed.nowRotateSpeed) {
        _speedRange.nowMinSpeed = _rotateSpeed.nowRotateSpeed;
    } else {
        _speedRange.minSameNumber++;
    }
    //如果速度没有替换最大速度，则计数加1
    if (_speedRange.nowMaxSpeed < _rotateSpeed.nowRotateSpeed) {
        _speedRange.nowMaxSpeed = _rotateSpeed.nowRotateSpeed;
    } else {
        _speedRange.maxSameNumber++;
    }
    //如果连续20帧没有刷新最小速度，则该速度为波谷速度（该速度一旦更新，便不再更新）
    if (_speedRange.minSameNumber > 30 && !_speedRange.minSpeedFlag) {
        _speedRange.realMinSpeed = _speedRange.nowMinSpeed;
        _speedRange.minSpeedFlag = true;
    }
    //如果连续20帧没有刷新最大速度，则该速度为波峰速度（该速度一旦更新，便不再更新）
    if (_speedRange.maxSameNumber > 30 && !_speedRange.maxSpeedFlag) {
        _speedRange.realMaxSpeed = _speedRange.nowMaxSpeed;
        _speedRange.maxSpeedFlag = true;
    }

    _rotateSpeed.realRotateSpeed = _rotateSpeed.nowRotateSpeed;

    //赋予开火结构体现在的预测角度
    fire.GetNowSpeed = _rotateSpeed.realRotateSpeed;
}



void BuffPrediction::addAngele(BuffTarget &buffTargetPrediction)
{

    //计算实时角速度
    calculateRotateSpeed(buffTargetPrediction);

    //计时器
    double curTime_ = (double) cv::getTickCount() / cv::getTickFrequency()  ;


    //传入data容器
    data.emplace_back(_rotateSpeed.realRotateSpeed , curTime_);

}


    /*
     * 拟合三角函数曲线
     */

    float BuffPrediction::fittingCurve()
    {
        if(data.empty()){ return 0;}

        if(data.size()>clear)
        {
            fitting_a_w();
            if (isnan(_a) ||!_sameLeaf)
            {
                _a = 0.9;
                _w = 1.9;
                t_0 = 0;
                data.clear();
                return 0;
            } 
            fitting_t();

            if (data.size() > 100)data.erase(data.begin());

            
            
            auto t = (double) (data[data.size() - 1].time - start_time);


            // 求积分算出预测角度
            BigPrediction = -((-_a / _w) * (cos(_w * (t + DELAY_TIME_ + t_0)) - cos(_w * (t + t_0))) +
                                 (2.090 - _a) * DELAY_TIME_) * 180 / float(CV_PI) ;  

            // 数值更加平滑点
            BigPrediction = 0.3*BigPrediction + 0.7*last_angle; 

            last_angle = BigPrediction;

            // logger.logData(std::to_string(BigPrediction));

            fire.Prediction_angle = BigPrediction ;
            return BigPrediction;
        }
    }



/**
 *  @brief  离散傅里叶获得正弦项值
 */
float BuffPrediction::get_F_s(int n, float f_k, int k, int _N)
{
    return f_k * sin(2.0 * M_PI * (float)n / (float)_N * (float)k * DT);
}

/**
 *  @brief  离散傅里叶获得余弦项值
 */
float BuffPrediction::get_F_c(int n, float f_k, int k, int _N)
{
    return f_k * cos(2.0 * M_PI * (float)n / (float)_N * (float)k * DT);
}

/**
 *  @brief 离散傅里叶获得第n项的值，规整化速度值
 *  @return 模的平方
 */
float BuffPrediction::get_F(int n, int _N)
{
    float c = 0.0, s = 0.0;
    if (CLOCKWISE )
        for (int i = 0; i < data.size(); i++)
        {
            c += get_F_c(n, (data[i].angle_speed - (2.090 - _a)), i, N);
            s += get_F_s(n, (data[i].angle_speed - (2.090 - _a)), i, N);
        }
    else
        for (int i = 0; i < data.size(); i++)
        {
            c += get_F_c(n, (-data[i].angle_speed - (2.090 - _a)), i, N);
            s += get_F_s(n, (-data[i].angle_speed - (2.090 - _a)), i, N);
        }

    return sqrt(c * c + s * s);
}

/**
 *  @brief  求不同相位时的积分,规整化速度值
 */
float BuffPrediction::get_integral(float t_)
{
    float sum = 0;
    if (CLOCKWISE)
        for (int i = 0; i < data.size(); i++)
        {
            sum += sin((i * DT + t_) * _w) * (data[i].angle_speed - (2.090 - _a)) / _a;
        }
    else
        for (int i = 0; i < data.size(); i++)
        {
            sum += sin((i * DT + t_) * _w) * (-data[i].angle_speed - (2.090 - _a)) / _a;
        }

    return sum;
}

/**
 *  @brief  求角频率、振幅
 */
void BuffPrediction::fitting_a_w()
{
    int n_min = 1.884 / (2 * M_PI) * N;
    int n_max = 2.0 / (2 * M_PI) * N + 1;

    float max_i = n_min;
    float max_value = get_F(n_min, N), value = 0.0;
    for (int i = n_min + 1; i < n_max; i++)
    {
        value = get_F(i, N);
        if (value > max_value)
        {
            max_i = (float)i;
            max_value = value;
        }
    }
    _w = max_i / (float)N * 2.0 * M_PI;
    _a = max_value / N * 2;
    if (_a > 1.045) _a = 1.045;
    else if (_a < 0.780) _a = 0.780;
}

/**
 *  @brief  求相位差
 */

void BuffPrediction::fitting_t()
{
    float max_value = 0.0, value = 0.0;
    int max_i = 0;
    for (int i = 0; i < T0_N + 1; i++)
    {
        value = get_integral((float)i * MAX_T0 / T0_N);
        if (value > max_value)
        {
            max_i = i;
            max_value = value;
        }
    }
    t_0 = (float)max_i * MAX_T0 / T0_N;
    start_time = data[0].time;
}
 

// 计算旋转角度
float circleAngle(cv::Point2f pt0, cv::Point2f pt1 , cv::Point2f pt2 ){
    
    float  circleAngle;
    float dx1 = pt1.x - pt0.x;
    float dy1 = pt1.y - pt0.y;
    float dx2 = pt2.x - pt0.x;
    float dy2 = pt2.y - pt0.y;
    float angle_line = (dx1 * dx2 + dy1 * dy2) / sqrtf((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10f);
    circleAngle =  acosf(angle_line) * 180.0f / 3.141592653f;
    if (pt0.y < pt1.y) {
        circleAngle = -circleAngle;
    }

    return circleAngle;
}


/**
 *  @brief  求预测点
 */

void BuffPrediction::get_pre_points(BuffTarget &buffTargetPrediction)
{
    Solve_OK = false;



    buffTargetPrediction.target_angle = circleAngle( buffTargetPrediction.buff_center,
    cv::Point2f(3000.0f, buffTargetPrediction.r_center.y),
    buffTargetPrediction.r_center);

    float buff_r_distance = POINT_DIST(buffTargetPrediction.r_center, buffTargetPrediction.buff_center);

    if(Buff_Mode == SMALL)   
    {

        Gain_Angle = SMALL_Pre_Angle_Gain ;
        if(direction == CLOCKWISE)
        {
            prediction_point.x = buff_r_distance * cos((-buffTargetPrediction.target_angle + 180 +  Gain_Angle) * CV_PI / 180);
            prediction_point.y = buff_r_distance * sin((-buffTargetPrediction.target_angle + 180 +  Gain_Angle) * CV_PI / 180);
            prediction_point += buffTargetPrediction.r_center;

        }
        else if ( direction == ANTICLOCKWISE)
        {
            prediction_point.x = buff_r_distance * cos((-buffTargetPrediction.target_angle - 180 -  Gain_Angle) * CV_PI / 180);
            prediction_point.y = buff_r_distance * sin((-buffTargetPrediction.target_angle - 180 -  Gain_Angle) * CV_PI / 180);
            prediction_point += buffTargetPrediction.r_center;

        }
        else if(direction== STOP)
        {
            prediction_point = buffTargetPrediction.buff_center;
        }
    }
    else if(Buff_Mode == BIG)  
    {
        
        if(direction == CLOCKWISE) 
        {

            prediction_point.x = buff_r_distance * cos((-buffTargetPrediction.target_angle  +   180 +  fittingCurve( )) * CV_PI / 180);
            prediction_point.y = buff_r_distance * sin((-buffTargetPrediction.target_angle  +   180 +  fittingCurve( )) * CV_PI / 180);
            prediction_point += buffTargetPrediction.r_center;
        }
        else if( direction == ANTICLOCKWISE)
        {
            prediction_point.x = buff_r_distance * cos((-buffTargetPrediction.target_angle  -  180  -  fittingCurve( )) * CV_PI / 180);
            prediction_point.y = buff_r_distance * sin((-buffTargetPrediction.target_angle  -  180  -  fittingCurve( )) * CV_PI / 180);
            prediction_point += buffTargetPrediction.r_center;

        }
        else if(direction== STOP)
        {
            prediction_point = buffTargetPrediction.buff_center;
        }
    }


    // fire.now_Point = buffTargetPrediction.buff_center ;
//    cv::Rect pre_rect(pre.x - buffTargetPrediction.buff_rect.size.width / 2, pre.y -  buffTargetPrediction.buff_rect.size.height / 2,  buffTargetPrediction.buff_rect.size.width  ,  buffTargetPrediction.buff_rect.size.height  );
//    cv::rectangle(frame , pre_rect , cv::Scalar (0,255,0),3);



    buffTargetPrediction.AS_pre_input[0] = cv::Point2f (prediction_point.x - buffTargetPrediction.buff_rect.size.width / 2 +  buffTargetPrediction.buff_rect.size.height  , prediction_point.y - buffTargetPrediction.buff_rect.size.height / 2  );
    buffTargetPrediction.AS_pre_input[1] = cv::Point2f (prediction_point.x - buffTargetPrediction.buff_rect.size.width / 2 ,prediction_point.y - buffTargetPrediction.buff_rect.size.height / 2 );
    buffTargetPrediction.AS_pre_input[2] = cv::Point2f (prediction_point.x - buffTargetPrediction.buff_rect.size.width / 2 ,prediction_point.y - buffTargetPrediction.buff_rect.size.height / 2 + buffTargetPrediction.buff_rect.size.width );
    buffTargetPrediction.AS_pre_input[3] = cv::Point2f (prediction_point.x - buffTargetPrediction.buff_rect.size.width / 2 +  buffTargetPrediction.buff_rect.size.height  , prediction_point.y - buffTargetPrediction.buff_rect.size.height / 2 + buffTargetPrediction.buff_rect.size.width );


 

    /**
     * @param AS接入
    cv::Point2f AS_center_input[4];   //AS四点的输入 顺序确定
    cv::Point2f AS_pre_input[4];       // AS预测四点
     */
    if(Dubug)
    {
        Eigen::Vector3d test = AS.pixel2imu_buff(buffTargetPrediction.AS_center_input, 3);
        buffTargetPrediction.distance =sqrt(test[1] * test[1] + test[0] * test[0]);;
        distance = buffTargetPrediction.distance;
        Solve_OK = true;

        Eigen::Vector3d rpy = AS.Barrel_Solve(test);
        Solve_pitch = rpy[1];
        Solve_yaw = rpy[2];
    }
    else
    {
        Eigen::Vector3d test = AS.pixel2imu_buff(buffTargetPrediction.AS_pre_input, 3);
        buffTargetPrediction.distance =sqrt(test[1] * test[1] + test[0] * test[0]);;
        distance = buffTargetPrediction.distance;
        Solve_OK = true;

        Eigen::Vector3d rpy = AS.Barrel_Solve(test);
        Solve_pitch = rpy[1];
        Solve_yaw = rpy[2];
    }


}
/**
 * @brief 开火程序后期再优化
 */




 

void BuffPrediction::OpenFire()
{
    // 先判断有没有切换目标，如果没有切换则为 True
    if (POINT_DIST(fire.now_Point, fire.last_Point) > 100 && (fabs(fire.now_Point.x - fire.last_Point.x) > 60 || abs(fire.now_Point.y - fire.last_Point.y) > 60))
    {
        std::printf("Change!\n");
        fire.FireSameLeaf = true;
        fire.Have_shoot = false; // 切换目标都要Have_shoot
        fire.shoot_time = 0;     // 初始化等待时间
        fire.fire_dataList.clear() ;
    }

    fire.last_Point = fire.now_Point;

    if (Buff_Mode == SMALL)
    {
        if (fire.FireSameLeaf)
        {
            fire.SameLeafTime++;
        }
        // std::cout<<fire.SameLeafTime<<std::endl;

        if (fire.SameLeafTime >= 50)
        {
            fire.SameLeafTime = 0;
            fire.Start_Fire = true ;
 
        }
        else
        {
            fire.Start_Fire = false ;
        }
    
    }
    else if (Buff_Mode == BIG) // 混合模式在最慢时刻打+最快时刻打。
    {
        // 开火初始化
        fire.fire_init();
        fire.wait_time++;
        // 误差值
        double loss = 0;

        // 判断当前所处的区间状态
        if (fire.Prediction_angle > 30)
        {
            fire.Is_High = true;
            fire.fire_dataList.push_back(fire.Prediction_angle);
        }
        else if (fire.Prediction_angle > 15 && fire.Prediction_angle < 25)
        {
            fire.Is_midian = true;
            fire.fire_dataList.push_back(fire.Prediction_angle);
        }
        else if (fire.Prediction_angle < 10)
        {
            fire.Is_low = true;
            fire.fire_dataList.push_back(fire.Prediction_angle);
        }

        // 判断数据误差
        if (fire.fire_dataList.size() > 7)
        {

            float Median_val = fire.fire_dataList[4];
            for (size_t i = 0; i < fire.fire_dataList.size(); i++)
            {
                loss = loss + std::sqrt(abs(Median_val * Median_val - fire.fire_dataList[i] * fire.fire_dataList[i]));
            }

            fire.fire_dataList.clear();
        }

        // 确定是否可以发射    时机因素 + 后期因素
        if (fire.Is_low && loss < 10 && fire.FireSameLeaf && (!fire.Have_shoot || fire.Try_shoot) && fire.wait_time > 30)
        {
 
            fire.Start_Fire = true;
            fire.Have_shoot = true;
            fire.Try_shoot = false;
            fire.wait_time = 0;
        }
        else if (fire.Is_midian && loss < 15 && fire.FireSameLeaf && (!fire.Have_shoot || fire.Try_shoot) && fire.wait_time > 30)
        {
 
            fire.Start_Fire = true;
            fire.Have_shoot = true;
            fire.Try_shoot = false;
            fire.wait_time = 0;
        }
        else if (fire.Is_High && loss < 20 && fire.FireSameLeaf && (!fire.Have_shoot || fire.Try_shoot) && fire.wait_time > 30)
        {
 
            fire.Start_Fire = true;
            fire.Have_shoot = true;
            fire.Try_shoot = false;
            fire.wait_time = 0;
        }

        // 如果发射了一次还不成功再尝试

        if (fire.Have_shoot)
        {
            fire.shoot_time++;

            if (fire.shoot_time == TryShootTime) // 后期建议改为时间
            {
                std::printf( "Try again!\n");
                fire.Try_shoot = true;
            }
        }
    }

    if (fire.Start_Fire)
    {
        fire_Ok = 1;
    }
    else
    {
        fire_Ok = 0;
    }
}
/**
 *  @brief  可视化数据
 */

void BuffPrediction::show_message(cv::Mat &show_src ,BuffTarget &buffTargetPrediction )
{
    // putText(show_src, "0", outPoint_0, cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(0, 28, 166), 2);
    // putText(show_src, "1", outPoint_1, cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(0, 28, 166), 2);
    // putText(show_src, "2", outPoint_2, cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(0, 28, 166), 2);
    // putText(show_src, "3", outPoint_3, cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(0, 28, 166), 2);

    cv::line(show_src, buffTargetPrediction.r_center,      prediction_point,cv::Scalar (255,255,255),2);
    // cv::line(show_src, buffTargetPrediction.AS_pre_input[0], buffTargetPrediction.AS_pre_input[1], cv::Scalar(255, 28, 166), 2);
    // cv::line(show_src, buffTargetPrediction.AS_pre_input[1], buffTargetPrediction.AS_pre_input[2], cv::Scalar(255, 28, 166), 2);
    // cv::line(show_src, buffTargetPrediction.AS_pre_input[2], buffTargetPrediction.AS_pre_input[3], cv::Scalar(255, 28, 166), 2);
    // cv::line(show_src, buffTargetPrediction.AS_pre_input[3], buffTargetPrediction.AS_pre_input[0], cv::Scalar(255, 28, 166), 2);

    cv::putText(show_src, "Prediction_Point", prediction_point, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(123, 255, 22),
                2, 3);


    cv::circle(show_src, prediction_point, 5, cv::Scalar(123, 255, 22), -1);
    // if (fire.Start_Fire)
    // {
    //     cv::putText(show_src, "fire:  shoot!!! " + std::to_string(fire.shoot_time) , cv::Point2f(11, 170),
    //                 cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);
    // }
    // else
    // {
    //     cv::putText(show_src, "fire:  No!!!" , cv::Point2f(11, 170),
    //                 cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);
    // }

    // if (fire.Is_low)
    // {
    //     cv::putText(show_src, "LOW!!!", cv::Point2f(11, 50),
    //                 cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);
    // }
    // else if(fire.Is_midian)
    // {
    //      cv::putText(show_src, "Midian!!!", cv::Point2f(11, 50),
    //                 cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);
    // }
    // else if (fire.Is_High)
    // {
    //      cv::putText(show_src, "High!!!", cv::Point2f(11, 50),
    //                 cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);
    // }
    

    cv::putText(show_src, "pitch : " + std::to_string(AS.Robot_msg.Controller_pitch), cv::Point2f(11, 400),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);

    cv::putText(show_src, "yaw   : " + std::to_string(AS.Robot_msg.Controller_yaw), cv::Point2f(11, 450),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);

    cv::putText(show_src, "roll  : " + std::to_string(AS.Robot_msg.Controller_roll), cv::Point2f(11, 500),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);

    cv::putText(show_src, "distance: " + std::to_string(distance) + "m", cv::Point2f(11, 550),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);


    if(Buff_Mode == SMALL ) {
        cv::putText(show_src, "Gain Angle : " + std::to_string(Gain_Angle), cv::Point2f(11, 325),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);
        cv::putText(show_src, "SMALL-BUFF-MODE", cv::Point2f(11, 620),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(123, 255, 22), 1, 1);
        
    }
    else{
        cv::putText(show_src, "Gain Angle : " + std::to_string(BigPrediction), cv::Point2f(11, 325),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, 3);
        cv::putText(show_src, "BIG-BUFF-MODE", cv::Point2f(11, 620),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(123, 255, 22), 1, 1);

    }
  

//    putText(show_src, "0", buffTargetPrediction.AS_pre_input[0], cv::FONT_HERSHEY_SIMPLEX, 1.0,
//            cv::Scalar(255, 28, 166), 2);
//
//    putText(show_src, "1", buffTargetPrediction.AS_pre_input[1], cv::FONT_HERSHEY_SIMPLEX, 1.0,
//            cv::Scalar(255, 28, 166), 2);
//
//    putText(show_src, "2", buffTargetPrediction.AS_pre_input[2], cv::FONT_HERSHEY_SIMPLEX, 1.0,
//            cv::Scalar(255, 28, 166), 2);
//
//    putText(show_src, "3", buffTargetPrediction.AS_pre_input[3], cv::FONT_HERSHEY_SIMPLEX, 1.0,
//            cv::Scalar(255, 28, 166), 2);

    

 }