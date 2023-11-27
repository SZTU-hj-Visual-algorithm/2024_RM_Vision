//
// Created by steins_xin on 23-10-1.
//


//#define HIK_CAMERA

#include <opencv2/opencv.hpp>
#include <math.h>
#include "Armor_detection.h"
#include "AngleSolve.h"
#include "Armor_Track.h"
#include "HIK_Camera.h"
#include "Armor_Observe.h"

#define POINT_DIST(p1,p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)) // 计算两点距离(勾股定理)
#define MANHATTAN_DISTANCE_X(p1,p2) abs(p1.x-p2.x) // 计算两点距离(曼哈顿距离)
#define MANHATTAN_DISTANCE_Y(p1,p2) abs(p1.y-p2.y) // 计算两点距离(曼哈顿距离)

// 多线程
#include <pthread.h>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include <mutex>

// 定义一个共享计数器
int counter = 0;
bool start = false;

pthread_t thread_1;             // 线程1
pthread_t thread_2;             // 线程2

// 定义互斥锁
pthread_mutex_t Mutex;
pthread_cond_t Cond;

cv::Mat ALL_src;

void* HIK_Sample(void* Void){
    cv::Mat src;
    HIK_Camera HIK;

    //相机初始化
    if(HIK.HIKCamera_Init()){
        HIK.HIKCamera_SetParam(ON);        //设置相机参数
        HIK.HIKCamera_startGrabbing();   //相机开始取流
        while (HIK.Camera_OK){
            HIK.HIKCamera_read();        //读取相机图像


            ALL_src = HIK.src.clone();       //复制图像

            pthread_mutex_lock(&Mutex);

            start = true;
            pthread_cond_signal(&Cond);
            pthread_mutex_unlock(&Mutex);
            imshow("src",src);
            cv::waitKey(1);

            HIK.Bayer.release();         //释放资源
        }
        HIK.HIKCamera_close();           //关闭相机
    }
}

void* Image(void* Void){
    cv::Mat src_copy;
    ArmorDetector Detector;
    AngleSolve AS;
    ArmorTrack Tracker;
    ArmorObserve AO;
    std::vector<Armor> Targets;
    chrono_time time_temp;
    while (true){
        pthread_mutex_lock(&Mutex);
        while (!start) {
            pthread_cond_wait(&Cond, &Mutex);
        }
        start = false;
        AO._src = src_copy.clone();
        Targets = Detector.Detection(src_copy);
        //获取最终装甲板
        Tracker.Track(src_copy,Targets,std::chrono::high_resolution_clock::now());
        bool AO_OK = (Tracker.tracker_state == TRACKING) && Tracker.OB_Track[Tracker.tracking_id].is_initialized;
        // 开始拟合圆心
        if(AO_OK) {
            if(Tracker.OB_Track[Tracker.tracking_id].axesState == LONG)
                AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Long_axes);
            else if(Tracker.OB_Track[Tracker.tracking_id].axesState == SHORT)
                AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Short_axes);

            if(AO.Fit_OK) AO.ArmorObserve_show(AO.Smooth_position,Tracker.OB[Tracker.tracking_id],Tracker.OB_Track[Tracker.tracking_id]);
//            std::cout << Tracker.OB[Tracker.tracking_id].High_LOW_difference << endl;
        }
        pthread_mutex_unlock(&Mutex);
    }
}

int main(){
    using namespace std;
    using namespace cv;
#ifdef HIK_CAMERA
    cv::Mat src;
    HIK_Camera HIK;
    ArmorDetector Detector;
    AngleSolve AS;
    ArmorObserve AO;
    ArmorTrack Tracker;
    std::vector<Armor> Targets;

    //相机初始化
    if(HIK.HIKCamera_Init()){
        HIK.HIKCamera_SetParam();        //设置相机参数
        HIK.HIKCamera_startGrabbing();   //相机开始取流
        while (HIK.Camera_OK){
                HIK.HIKCamera_read();        //读取相机图像
                src = HIK.src.clone();       //复制图像
                AO._src = src.clone();
                HIK.Bayer.release();         //释放资源

                // ================识别+预测部分==================
                Targets = Detector.Detection(src);                          //获取最终装甲板
                Tracker.Track(src,Targets,std::chrono::high_resolution_clock::now());
                bool AO_OK = (Tracker.tracker_state == TRACKING) && Tracker.OB_Track[Tracker.tracking_id].is_initialized;
                // 开始拟合圆心
                if(AO_OK) {
                    if(Tracker.OB_Track[Tracker.tracking_id].axesState == LONG)
                        AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Long_axes,Tracker.OB[Tracker.tracking_id].Center_Armor_High);
                    else if(Tracker.OB_Track[Tracker.tracking_id].axesState == SHORT)
                        AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Short_axes,Tracker.OB[Tracker.tracking_id].Center_Armor_High);

                    if(AO.Fit_OK) AO.ArmorObserve_show(AO.Smooth_position,Tracker.OB[Tracker.tracking_id],Tracker.OB_Track[Tracker.tracking_id]);

                }
                cv::imshow("src",  src);
                cv::waitKey(1);
            }
            HIK.HIKCamera_close();           //关闭相机
        }
#else

#define A
#ifdef A
    cv::Mat src;
    ArmorDetector Detector;
    AngleSolve AS;
    ArmorTrack Tracker;
    ArmorObserve AO;
    std::vector<Armor> Targets;
    chrono_time time_temp;


    cv::VideoCapture video("/home/steins_xin/CLionProjects/RM_Detection/images/Video_1.avi");
//    cv::VideoCapture video("/home/steins_xin/lmx_new/images/example_1.mp4");

    while (1) {
        video >> src;
        AO._src = src.clone();
        Targets = Detector.Detection(src);
        //获取最终装甲板
        Tracker.Track(src,Targets,std::chrono::high_resolution_clock::now());
        bool AO_OK = (Tracker.tracker_state == TRACKING) && Tracker.OB_Track[Tracker.tracking_id].is_initialized;
        // 开始拟合圆心
        if(AO_OK) {
            if(Tracker.OB_Track[Tracker.tracking_id].axesState == LONG)
                AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Long_axes);
            else if(Tracker.OB_Track[Tracker.tracking_id].axesState == SHORT)
                AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Short_axes);

            if(AO.Fit_OK) AO.ArmorObserve_show(AO.Smooth_position,Tracker.OB[Tracker.tracking_id],Tracker.OB_Track[Tracker.tracking_id]);
//            std::cout << Tracker.OB[Tracker.tracking_id].High_LOW_difference << endl;
        }

        cv::imshow("src",src);
        cv::waitKey(0);
    }
#endif //A

    pthread_create(&thread_1, NULL, HIK_Sample, NULL);
    pthread_create(&thread_2, NULL, Image, NULL);

    // 等待线程1和线程2完成
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    // 销毁互斥锁
//    pthread_mutex_destroy(&Mutex);

#endif //HIK_CAMERA
}