#include <iostream>
#include <opencv2/opencv.hpp>
class buff_prediction
{
public:

    //预测
    double SMALL_buff_PredictionAngle();

    double BIG_buff_PredictionAngle(double time);

private:

    //大符预测参数
//    float a=13.2;
//    float b=8.2;
//    float c=1;
//    float d=-3.1;
//    float e=96.6;
//(a + b*sin(CV_PI*c*(time + d)/e));
//
//
    double delay_time = 0.3;
    float a=8 ;
    float b=5;
    float c=0.3;
    float d=-3.6;
    float e=1.9;
};
