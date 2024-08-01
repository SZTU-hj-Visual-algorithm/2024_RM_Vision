#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include "MD_Camera_H/CameraApi.h"
#include "robot_status.h"
#include "robot_struct.h"

class MD_Camera
{
public:
    cv::Mat src;
    MD_Camera() = default;
//=======================相机使用函数=======================
    bool init(int Camera_State,char *Camera_Name);      // 初始化相机
    bool read_frame_rgb();                              // 读取图片
    bool release_data();                                // 释放图片缓存

    ~MD_Camera();
//=======================相机调试函数=======================
    bool Modify_Camera_Name(char *Camera_name);         // 修改相机名字(只能一个相机修改)
    void Camera_Name();                                 // 输出相机名字(只能一个相机使用)
    void MDCamera_outParam();                           // 输出相机参数

    int record_state = RECORD_STOP;                     // 用来记录录像状态
private:
    CameraHandle h_camera;                              // 相机句柄

    INT piNums = 1;                                     // 设备的个数指针
    tSdkCameraDevInfo camera_list[3];                   // 设备列表数组指针
    tSdkCameraCapbility capbility;                      // 相机设备的特性结构体

    tSdkFrameHead frame_h;                              // 图像头指针
    tSdkFrameHead record_hframe;                        // 录像所使用的图像头指针

    unsigned char* rgb_buffer = nullptr;                // rgb图像数据缓冲区
    int channel = 3 ;                                   // 通道数选择图像格式
    BYTE* pbybuffer = nullptr;                          // 原始图像数据缓冲区

    char path[100] = "1.avi";
};
