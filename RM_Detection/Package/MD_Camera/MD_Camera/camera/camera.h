#include "CameraApi.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>

class Camera
{
public:
    cv::Mat src;
    Camera() = default;
//=======================相机使用函数=======================
    bool init();
    bool read_frame_rgb();
    bool release_data();
    bool camera_record();
    bool record_start();

    ~Camera();
//=======================相机调试函数=======================
    bool Modify_Camera_Name();        //修改相机名字(只能一个相机修改)

    int record_state = RECORD_STOP;   //用来记录录像状态
    char *Camera_Name;                //选择相机名称
private:
    INT piNums = 3;                   //设备的个数指针
    tSdkCameraDevInfo camera_list[3]; //设备列表数组指针
    CameraHandle h_camera;            //相机句柄
    tSdkCameraCapbility capbility;    //相机设备的特性结构体
    tSdkFrameHead frame_h;//图像头指针
    tSdkFrameHead record_hframe;//录像所使用的图像头指针
    unsigned char* rgb_buffer = nullptr;//rgb图像数据缓冲区
    int channel = 3 ;                   //通道数选择图像格式
    BYTE* pbybuffer = nullptr;//原始图像数据缓冲区
    char path[100] = "1.avi";
};
