//
// Created by steins_xin on 23-10-11.
//

//#define HIK_CAMERA
//#define CIRCLE_KALMAN

#include <opencv2/opencv.hpp>
// 内部头文件
#include "Circle_Kalman.h"
#include "Armor_Observe.h"
// 外部头文件
#include "Armor_detection.h"
#include "AngleSolve.h"
#include "Armor_Track.h"
#include "HIK_Camera.h"


int main() {
    cv::Mat src;
    ArmorDetector Detector;
    AngleSolve AS;
    ArmorTrack Tracker;
    ArmorObserve AO;
    Circle_Kalman CKF;
    Eigen::Matrix<double, 4, 1> pre;
    Eigen::Matrix<double, 4, 1> real;
    std::vector<Armor> Targets;
    HIK_Camera HIK;
    chrono_time time_temp;


#ifdef HIK_CAMERA
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
            AO.Center_fitting(Tracker.enemy_armor);
            AO.World_projection_show();
            AO.ArmorObserve_show();
            cv::imshow("src",  src);
            cv::waitKey(1);
        }
        HIK.HIKCamera_close();           //关闭相机
    }

#else
    cv::VideoCapture video("/home/steins_xin/CLionProjects/RM_Detection/images/Video_2.avi");
//    cv::VideoCapture video("/home/steins_xin/lmx_new/images/example_1.mp4");
    CKF.Initial();
    bool init = true;
    while (1) {
        CKF.setF(1);
        video >> src;
        AO._src = src.clone();
        Targets = Detector.Detection(src);
        //获取最终装甲板
        Tracker.Track(src, Targets, std::chrono::high_resolution_clock::now());
        if(Tracker.tracker_state == TRACKING) {
            if(init) {
                CKF.setPosAndSpeed(Tracker.enemy_armor.world_position,Eigen::Vector2d(0,0));
                init = false;
            }
            AO.Center_fitting(Tracker.enemy_armor,0.23);
            AO.World_projection_show();
//            if(AO.Fit_OK) AO.ArmorObserve_show(AO.Smooth_position);
            double angle = -50.5;
            double dis = AS.World_projection(Tracker.enemy_armor,angle);
//            std::cout << dis << std::endl;

            double l = -50,r = 50;             // 左右点
            double eps = 1e-8;      // 精度
            while (r-l >= eps) {
                double thridPart = (r-l)/3;

                double ls = l + thridPart;
                double rs = r - thridPart;

                double ls_dis = AS.World_projection(Tracker.enemy_armor,ls);
                double rs_dis = AS.World_projection(Tracker.enemy_armor,rs);
                if(ls_dis > rs_dis) {
                    l = ls;
                }
                else {
                    r = rs;
                }
            }
//            std::cout << l << "  " << r << std::endl;
            std::cout << AS.World_projection(Tracker.enemy_armor,l) << "    "
                     << AS.World_projection(Tracker.enemy_armor,-l) << std::endl;
            if(AS.World_projection(Tracker.enemy_armor,l) > AS.World_projection(Tracker.enemy_armor,-l))
                l = -l;
            cv::putText(src,"yaw:"+ std::to_string(l),cv::Point(0,280),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        }
        cv::imshow("src",src);
        cv::waitKey(0);
    }

#endif //HIK_CAMERA

#ifdef CIRCLE_KALMAN
    using namespace std;
    using namespace cv;
    Point points[20] = {Point(50,300),Point(100,300),
                        Point(150,300),Point(200,300),
                        Point(250,300),Point(300,300),
                        Point(350,300),Point(400,300),
                        Point(450,300),Point(500,300),
                        Point(550,300),Point(600,300),
                        Point(650,300),Point(700,300),
                        Point(750,300),Point(800,300)};
    CKF.Initial();
    CKF.setPosAndSpeed(Eigen::Vector3d(0,0,0), Eigen::Vector2d (0,0));
    for (int i = 0; i < 15; ++i) {
        CKF.setF(1);
        pre = CKF.predict();
        real = CKF.update(Eigen::Vector2d(points[i].x,points[i].y));
        cout << "pre: " << pre[0] << "  " << pre[1] << "  " << pre[2] << "  " << pre[3] << endl;
        cout << "update: " << real[0] << "  " << real[1] << "  " << real[2] << "  " << real[3] << endl;
        cout << "Y: " << points[i].x << "  " << points[i].y << endl;
    }
#endif //CIRCLE_KALMAN
}