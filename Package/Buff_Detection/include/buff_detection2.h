//
// Created by MG on 2023/9/15.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <Eigen/Eigen>
#include "armor_detection.h"
#include "gimbal_control.h"
#include <opencv2/dnn.hpp>
#include <cmath>
#include "buff_prediction.h"
#include "buff_track.h"

#define POINT_DIST(p1, p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))


namespace robot_detection {

    //大符R标结构体
    struct R {

        //距离
        double distance;

        R() = default;
    };

    //存储所有矩形的信息结构体
    struct candidateRect_Buff
            {
                float area;
                cv::RotatedRect rect;
                candidateRect_Buff() = default;
            };

    //大符目标结构体
    struct  buff_target
    {
        float angle;
        cv::RotatedRect rect;
        cv::Point2f center;
        //距离
        double distance;
        cv::Point2f AS_input[4];
        buff_target() = default;
    };

    //大符爷爷类
    class buff_detection {
    public:
        //构造函数
        buff_detection();

        //数据输出
        void show();

        //程序控制
        void Procedure();

        //图像处理
        void SetImage(const cv::Mat& frame);

        //找轮廓，初级筛选
        void findCandidateRect(const cv::Mat &images);

        //处理矩形
        bool SetRect_classification();

        //找R标，将初级的矩形进一步筛选
        void findR(candidateRect_Buff &rect  );

        //确认是否是R标,传入R的结构体
        bool isR(candidateRect_Buff &R_rect);

        //圆拟合
        bool fitCircleDirect(const std::vector<cv::Point2f>& points, cv::Point2f& center , cv::Point2f &buff_center);

        //寻找符叶
        void findLeaf(cv::RotatedRect &leaf);

        //判断是否
        bool isLeaf(cv::RotatedRect &leaf) const;

        //深度学习分类
        bool classifier(cv::Mat & src);

        //大符的最终处理
        bool final_buff();

        //记录已击打数目  + 虚拟装甲板  + 击打响应
        bool Recording_buff (const cv::Rect& mainRect);

        // 判断顺逆时针
        bool KnowDirection ( float angle);

        //R标的图像匹配
        bool R_classifier(cv::Mat &src  );

        //获取最终目标大符信息（解决两个目标问题）
        void Get_final_buff();

        //处理计算击打数
        void GetNumber_Recording_buff(cv::Point2f& center);

        //获取预测点坐标
        void get_pre();

   //////////////////////////////////////工具函数/////////////////////////////////////////////
        //保存照片到本地
        void ImWrite_frame (int index ,cv::Mat &Write_image);

        //打印函数工具
        bool printf(const float &data);

        //修复角度
     private:
        //对象定义
        enum COLOR { buff_BLUE, buff_RED, MORE};
        COLOR color ;
        AngleSolve AS;
        buff_prediction BP;
        buff_track BT;
        R R;
        buff_target buff_target_;
        candidateRect_Buff candidateRect_buff;


        bool BigBuff = false;
        bool clear = false;
        bool findR_ok =false;
        bool know_direction = false;
        bool know_direction_ONOFF = true;
        bool find_first_BigBuff = false;

        //buff_no
        size_t number_id_0 =0;
        size_t number_id_1 =0;
        size_t number_id_2 =0;
        size_t number_id_3 =0;
        size_t number_id_4 =0;

        //图像定义
        cv::Mat frame;
        cv::Mat binary;
        cv::Mat handle_binary;
        cv::Mat show_src;
        cv::Mat outputImage;

        //参数定义

        size_t classId;

        int br_blue_threshold;
        int br_red_threshold;
        int rb_red_threshold;
        int rb_blue_threshold;
        int more_threshold;
        int r_max_area;
        int r_min_area;
        int leaf_max_area;
        int leaf_min_area;
        int leaf_r_max_distance;
        int leaf_r_min_distance;

        int knowDirection_total_pass_score;
        int leaf_classifier_confidence;
        int recording_buff_area_ratio;

        int loss_frame_count = 0;
        int frameCount = 0;
        int hit_num = 0;

        float r_min_hb_ratio;
        float r_max_hb_ratio;
        float r_min_hw_ratio;
        float r_max_hw_ratio;

        float leaf_min_hw_ratio;
        float leaf_max_hw_ratio;

        float r_min_are_ratio;
        float r_max_are_ratio;

        float KnowDirection_Total_score=0;
        float angle;
        float last_angle = 360;
        float ratio = 1 ;



        double confidence;
        double R_classifier_confidence;
        double time;
        double dis;
        double R_B_dis;


        std::chrono::high_resolution_clock::time_point BigBuff_startTime;
        std::chrono::high_resolution_clock::time_point bigBuff_endTime;

        cv::RotatedRect buff;




        //虚拟装甲板
        cv::Point2f buff_id_0;
        cv::Point2f buff_id_1;
        cv::Point2f buff_id_2;
        cv::Point2f buff_id_3;
        cv::Point2f buff_id_4;

        cv::Point2f last_buff_target_center;
        cv::Point2f R_point;

        //容器定义
        std::vector<std::vector<cv::Point>> all_contours;
        std::vector<cv::Vec4i> all_hierarchy;
        std::vector<std::vector<cv::Point>> roi_contours;
        std::vector<candidateRect_Buff> candidateRect;
        std::vector<buff_target> first_buff_target;
        std::vector<buff_target> final_buff_target;
        std::vector<candidateRect_Buff> first_r_target;
        std::vector<cv::Point2f> buff_target_points;

        //路径
        std::string Video_PATH;
        std::string R_PATH_1 ;
        std::string R_PATH_2 ;
        std::string buff_classifier_model_PATH;
        std::string r_classifier_model_PATH;
        std::string image_write_PATH;

        //额外的
        int waitKey;
        double fps;
    };

}




