#include "buff_prediction.h"


double buff_prediction::SMALL_buff_PredictionAngle()
{
    return 11.22;
}


///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
////*
///*@brief  ：大符正弦拟合
///*@param  ：时间
///*@return： 预测角度
///*@exception ：适应性不高
///*@author  ：  Glb
///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////


double buff_prediction::BIG_buff_PredictionAngle(double time)
{
   return (a + b*sin(CV_PI*c*(time + delay_time + d)/e));
}