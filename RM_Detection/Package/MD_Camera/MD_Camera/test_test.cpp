//
// Created by steins_xin on 23-9-19.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include<X11/Xlib.h>
#include <stdio.h>
#include "camera/camera.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <armadillo>

#include <armor_detection.h>
#include <opencv2/core/cvstd.hpp>
#include <mutex>
#include <string>
#include <cstdio>
#include <chrono>


using namespace std;
using namespace cv;

pthread_mutex_t mutex_new;
pthread_cond_t cond_new;


void* Camera_1(void *PARAM);
void* Camera_2(void *PARAM);
void* Camera_3(void *PARAM);
void* Camera_show(void *PARAM);

Mat src_1;
Mat src_2;
Mat src_3;

int main(){

//    Mat get_src;
//    auto camera_warper = new Camera;
//    printf("camera_open-\n");
//    camera_warper->Camera_Name = "Camera_1";
//
//    ArmorDetector Detect;
//    std::vector<Armor> Targets;
//
//    if (camera_warper->init()) {
//        while ((waitKey(1) != 27)){
//            int64 t1 = getTickCount();
//
//            camera_warper->read_frame_rgb();
//            namedWindow("src",WINDOW_NORMAL);
//            get_src = camera_warper->src.clone();
//            get_src.copyTo(src);
//            camera_warper->release_data();
//            Targets = Detect.autoAim(get_src);
//            int64 t2 = getTickCount();
//
//            double spendtime =  (double)(t2-t1) / (getTickFrequency());
//            double FPS = 1 / spendtime;
//
//            String fps = "FPS: " +  to_string(FPS);
//            putText(src, fps,Point(100, 100), 2, 3, Scalar(255, 255, 255));
//            imshow("src",src);
//            waitKey(5);
//        }
//        camera_warper->~Camera();
//    }
//    else
//    {
//        printf("No camera!!\n");
//    }

//    XInitThreads();
    pthread_mutex_init(&mutex_new, NULL);
    pthread_cond_init(&cond_new, NULL);
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t show;

    pthread_create(&thread1, NULL, Camera_1, NULL);
    pthread_create(&thread2, NULL, Camera_2, NULL);
    pthread_create(&thread3, NULL, Camera_3, NULL);
    pthread_create(&show, NULL, Camera_show, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(show, NULL);
}
void* Camera_show(void *PARAM){
    sleep(10);
    while (1){
        Mat src;
        Mat src_12;
        hconcat(src_1,src_2,src_12);
        hconcat(src_12,src_3,src);
        namedWindow("src",WINDOW_NORMAL);
        imshow("src",src);
        waitKey(1);
    }
}

void* Camera_1(void *PARAM){

    Mat get_src;
    Mat _src;
    auto camera_warper_1 = new Camera;
    printf("camera_open-\n");

    ArmorDetector Detect;
    std::vector<Armor> Targets;
    camera_warper_1->Camera_Name = "Camera_1";
    Detect.show = "Camera_1_show";
    if (camera_warper_1->init()) {

        while ((waitKey(1) != 27)){
            int64 t1 = getTickCount();

            camera_warper_1->read_frame_rgb();
//            namedWindow("src_1",WINDOW_NORMAL);
            get_src = camera_warper_1->src.clone();
            get_src.copyTo(src_1);
            camera_warper_1->release_data();
            Targets = Detect.autoAim(get_src);

            int64 t2 = getTickCount();
            double spendtime =  (double)(t2-t1) / (getTickFrequency());
            double FPS = 1 / spendtime;

            String fps = "FPS: " +  to_string(FPS);
//            putText(get_src, fps,Point(100, 100), 2, 3, Scalar(255, 255, 255));
//            imshow("src_1",get_src);
//            waitKey(5);
        }
        camera_warper_1->~Camera();
    }
    else
    {
        printf("No camera!!\n");
    }
}
void* Camera_2(void *PARAM){
    sleep(4);
    Mat get_src;
    Mat _src;
    auto camera_warper_2 = new Camera;
    printf("camera_open-\n");

    ArmorDetector Detect;
    std::vector<Armor> Targets;
    camera_warper_2->Camera_Name = "Camera_2";
    Detect.show = "Camera_2_show";
    if (camera_warper_2->init()) {
        while ((waitKey(1) != 27)){
            int64 t1 = getTickCount();

            camera_warper_2->read_frame_rgb();
//            namedWindow("src_2",WINDOW_NORMAL);
            get_src = camera_warper_2->src.clone();
            get_src.copyTo(src_2);
            camera_warper_2->release_data();

            Targets = Detect.autoAim(get_src);

            int64 t2 = getTickCount();
            double spendtime =  (double)(t2-t1) / (getTickFrequency());
            double FPS = 1 / spendtime;

            String fps = "FPS: " +  to_string(FPS);
//            putText(get_src, fps,Point(100, 100), 2, 3, Scalar(255, 255, 255));
//            imshow("src_2",get_src);
//            waitKey(5);
        }
        camera_warper_2->~Camera();
    }
    else
    {
        printf("No camera!!\n");
    }
}
void* Camera_3(void *PARAM){
    sleep(8);
    Mat get_src;
    Mat _src;
    auto camera_warper_3 = new Camera;
    printf("camera_open-\n");

    ArmorDetector Detect;
    std::vector<Armor> Targets;
    camera_warper_3->Camera_Name = "Camera_3";
    Detect.show = "Camera_3_show";
    if (camera_warper_3->init()) {

        while ((waitKey(1) != 27)){
            int64 t1 = getTickCount();

            camera_warper_3->read_frame_rgb();
//            namedWindow("src_3",WINDOW_NORMAL);
            get_src = camera_warper_3->src.clone();
            get_src.copyTo(src_3);
            camera_warper_3->release_data();

            Targets = Detect.autoAim(get_src);

            int64 t2 = getTickCount();
            double spendtime =  (double)(t2-t1) / (getTickFrequency());
            double FPS = 1 / spendtime;

            String fps = "FPS: " +  to_string(FPS);
//            putText(get_src, fps,Point(100, 100), 2, 3, Scalar(255, 255, 255));
//            imshow("src_3",get_src);
//            waitKey(5);
        }
        camera_warper_3->~Camera();
    }
    else
    {
        printf("No camera!!\n");
    }
}

