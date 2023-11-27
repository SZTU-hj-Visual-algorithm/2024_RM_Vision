
#include <iostream>
#include "HIK_Camera.h"
#include "Armor_detection.h"
#include "AngleSolve.h"
#include "Armor_Track.h"
#include "Armor_Observe.h"
#include "serial_main.h"
#include "MD_Camera.h"
// 多线程
#include <pthread.h>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include <mutex>


bool start = false;
pthread_t thread_1;             // 线程1
pthread_t thread_2;             // 线程2

// 定义互斥锁
pthread_mutex_t Mutex;
pthread_cond_t Cond;

cv::Mat ALL_src;

SerialMain serial;  // usb
form _send_data;

bool Record_OK = false;

void* MD_Sample(void* Void){
    cv::Mat get_src;
    int first_get/* = true*/;   // usb
    chrono_time time_temp;
    auto MD = new MD_Camera;
    cv::VideoWriter outputVideo;   // MD_Camera保存视频使用
    if (Record_OK){
        // opencv保存视频
        int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D'); //选择编码格式
        int fps = 60;  //设置帧率
        std::string outputVideoPath = "MD_Camera.avi";   //保存视频的文件名
        outputVideo.open(outputVideoPath, codec, fps, cv::Size(1280,1024), true);//创建保存视频文件的视频流
    }

    // 初始化(MD_ON: 开启相机名字模式 MD_OFF: 关闭相机名字模式)
    if (MD->init(Robot::MD_OFF,"steins_xin")) {
        printf("camera_open-\n");
        MD->MDCamera_outParam();            // 输出参数
        while ((cv::waitKey(1) != 27)){
            MD->read_frame_rgb();       // 读取图片
            get_src = MD->src.clone();  // 复制图片
            get_src.copyTo(ALL_src);     // 复制图片
            if (Record_OK) outputVideo.write(get_src); //将video的图像数据一帧图像写入
            pthread_mutex_lock(&Mutex);
            {
                time_temp = std::chrono::high_resolution_clock::now();
                first_get = serial.ReceiverMain();                                          // usb
                // usb  //TODO: 这个值需要变化，用来模式切换 serial.vision_msg_.mode
                _send_data = {0x21,first_get, //! 0x21应该换成serial.vision_msg_.mode赋值，方便调试默认给的0x21
                              {serial.vision_msg_.pitch,
                               serial.vision_msg_.yaw,
                               serial.vision_msg_.shoot},
                              {serial.vision_msg_.quaternion[0],
                               serial.vision_msg_.quaternion[1],
                               serial.vision_msg_.quaternion[2],
                               serial.vision_msg_.quaternion[3]},
                              std::vector<Armor>(),time_temp};
            }
            start = true;
            pthread_cond_signal(&Cond);
            pthread_mutex_unlock(&Mutex);
            MD->release_data();         // 释放缓存
            imshow("src",get_src);
        }
        MD->~MD_Camera();
    }
    else    printf("No camera!!\n");

}


void* HIK_Sample(void* Void){
    cv::Mat src;
    HIK_Camera HIK;
    int first_get/* = true*/;   // usb
    chrono_time time_temp;

    //相机初始化
    if(HIK.HIKCamera_Init()){
        HIK.HIKCamera_SetParam(1);         //设置相机参数(自动白平衡)
        HIK.HIKCamera_startGrabbing();              //相机开始取流
        if (Record_OK) HIK.HIKCamera_StartRecord();
        while (HIK.Camera_OK){
            HIK.HIKCamera_read();        //读取相机图像
            ALL_src = HIK.src.clone();       //复制图像
            pthread_mutex_lock(&Mutex);
            {
                time_temp = std::chrono::high_resolution_clock::now();
                first_get = serial.ReceiverMain();                                          // usb
                // usb  //TODO: 这个值需要变化，用来模式切换 serial.vision_msg_.mode
                _send_data = {0x21,first_get, //! 0x21应该换成serial.vision_msg_.mode赋值，方便调试默认给的0x21
                              {serial.vision_msg_.pitch,
                               serial.vision_msg_.yaw,
                               serial.vision_msg_.shoot},
                              {serial.vision_msg_.quaternion[0],
                               serial.vision_msg_.quaternion[1],
                               serial.vision_msg_.quaternion[2],
                               serial.vision_msg_.quaternion[3]},
                              std::vector<Armor>(),time_temp};
            }

            start = true;
            pthread_cond_signal(&Cond);
            pthread_mutex_unlock(&Mutex);
            HIK.Bayer.release();         //释放资源
        }
        if (Record_OK) HIK.HIKCamera_StopRecord();
        HIK.HIKCamera_close();           //关闭相机
    }
}
void* Image(void* Void){
    cv::Mat src;
    chrono_time time_temp;
    ArmorDetector Detector;
    AngleSolve AS;
    ArmorTrack Tracker;
    ArmorObserve AO;
    std::vector<Armor> Targets;
    std::vector<double> vdata(3);

    while (true){
        pthread_mutex_lock(&Mutex);
        while (!start) {
            pthread_cond_wait(&Cond, &Mutex);
        }
        start = false;
        src = ALL_src.clone();
        // ========================读取结构体部分========================
        Tracker.AS.Init(_send_data.data, _send_data.quat);
	AO.AS.Init(_send_data.data, _send_data.quat);
        int mode_temp = _send_data.mode;
        pthread_mutex_unlock(&Mutex);          // 释放互斥锁
        // ============================END============================

        // ========================在此区域进行图像处理========================

        if(mode_temp == 0x21){
            Targets = Detector.Detection(src);
            //获取最终装甲板
            Tracker.Track(src,Targets,std::chrono::high_resolution_clock::now());
            Tracker.show();
            //! 整车观测部分
//            AO._src = src.clone();
//            bool AO_OK = (Tracker.tracker_state == TRACKING) && Tracker.OB_Track[Tracker.tracking_id].is_initialized;
//            // 开始拟合圆心
//            if(AO_OK) {
//                if(Tracker.OB_Track[Tracker.tracking_id].axesState == LONG)
//                    AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Long_axes);
//                else if(Tracker.OB_Track[Tracker.tracking_id].axesState == SHORT)
//                    AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].Short_axes);
//
//                if(AO.Fit_OK) AO.ArmorObserve_show(AO.Smooth_position,Tracker.OB[Tracker.tracking_id],Tracker.OB_Track[Tracker.tracking_id]);
//            }
            //! ===END===
            if(Tracker.tracker_state == TRACKING){
                // TODO: 弹速设置为25,未改
                vdata = { Tracker.Solve_pitch, Tracker.Solve_yaw, 0x31 };
            } else{
                //    原数据，无自瞄
                vdata = { Tracker.AS.Robot_msg.Controller_pitch,Tracker.AS.Robot_msg.Controller_yaw, 0x32 };
            }
            serial.SenderMain(vdata);   // 启动跟踪自瞄
        }

        // ================================================================
        cv::imshow("src",src);
        cv::waitKey(1);

    }
}

// #define HIK
#define MD
int main(){
    Record_OK = true;
#ifdef HIK
    pthread_create(&thread_1, NULL, HIK_Sample, NULL);
    pthread_create(&thread_2, NULL, Image, NULL);

    // 等待线程1和线程2完成
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    // 销毁互斥锁
    pthread_mutex_destroy(&Mutex);
#endif //HIK

#ifdef MD
    pthread_create(&thread_1, NULL, MD_Sample, NULL);
    pthread_create(&thread_2, NULL, Image, NULL);

    // 等待线程1和线程2完成
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    // 销毁互斥锁
    pthread_mutex_destroy(&Mutex);
#endif //MD

}
