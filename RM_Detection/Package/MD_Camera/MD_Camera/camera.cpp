#include "camera/camera.h"
#include <iostream>



bool Camera::init()
{
	//初始化sdk相机
	CameraSdkInit(1);
	//获取连接到当前pc端的相机设备
	int camera_status = CameraEnumerateDevice(camera_list, &piNums);
	if (camera_status != CAMERA_STATUS_SUCCESS)
	{
		std::cout << "CameraEnumerateDevice fail with" << camera_status << "!" << std::endl;
		return false;
	}

    //初始化相机设备并创建相机句柄
    if (CameraInitEx2(Camera_Name,&h_camera) != CAMERA_STATUS_SUCCESS)
    {
        CameraUnInit(h_camera);
        return false;
    }
    printf("Camera_Name = %s\n", Camera_Name);
	//获取相机设备的特性结构体
	auto status = CameraGetCapability(h_camera, &capbility);

	//获取当前相机的帧率模式数量
	//std::cout << capbility.iFrameSpeedDesc << std::endl;



	if (status != CAMERA_STATUS_SUCCESS)
	{
		std:: cout << "get capbility failed" << std::endl;
		return false;
	}

	//创建rgb图像数据缓冲区
	rgb_buffer = (unsigned char*)malloc(capbility.sResolutionRange.iHeightMax *
										capbility.sResolutionRange.iWidthMax * 3
	);

	//设置为手动设置曝光时间，用来防止拍摄图片拖影，防止拖影需要曝光时间保持在2毫秒以下
	CameraSetAeState(h_camera, false);

	//降低曝光时间，一般为了防止图像拖影将曝光时间调为2毫秒以下，该函数以微秒为单位
	CameraSetExposureTime(h_camera, 1200.);
	//CameraSetExposureTime(h_camera, 15000.0);

	//调高图像模拟增益值，使得图像亮度提高
	CameraSetAnalogGain(h_camera, 152);  //152
	
	CameraSetWbMode(h_camera,true);

	//CameraSetFrameSpeed(h_camera, 3);

	//int g;
	//double t;
	//获取相机当前的曝光时间
	//CameraGetExposureTime(h_camera, &t);
	//获取相机的模拟增益值大，亮度越大，最大192，超过自动截断）
	//CameraGetAnalogGain(h_camera, &g);
	//相机开始图像采集
	CameraPlay(h_camera);

	//std::cout << t / 1000.0 << std::endl;
	//std::cout << g << std::endl;

	//设置getimagebuffer的输出格式
	if (capbility.sIspCapacity.bMonoSensor)
	{
		channel = 1;
		CameraSetIspOutFormat(h_camera, CAMERA_MEDIA_TYPE_MONO8);

	}
	else
	{
		channel = 3;
		CameraSetIspOutFormat(h_camera, CAMERA_MEDIA_TYPE_BGR8);

	}
	return true;
}

bool Camera::read_frame_rgb()
{

	if (CameraGetImageBuffer(h_camera, &frame_h, &pbybuffer, 10000000) == CAMERA_STATUS_SUCCESS)
	{
		//将原始图像数据转为bgr图像数据储存到rgb_buffer中
		CameraImageProcess(h_camera, pbybuffer, rgb_buffer, &frame_h);

        src = cv::Mat(cvSize(frame_h.iWidth,frame_h.iHeight),CV_8UC3,rgb_buffer);

		return true;
	}
	else
	{

		return false;
	}

}

bool Camera::release_data()
{
	//将图像缓冲区中的图像数据释放掉
	CameraReleaseImageBuffer(h_camera, pbybuffer);

	return true;
}


bool Camera::record_start()
{
	if (record_state != RECORD_START)
	{
		int FormatList[] = {1,2,3,4,0};
		for (int i = 0; i < 5; i++)
		{
			//printf("a\n");
			int iret = CameraInitRecord(h_camera, FormatList[i],path, FALSE, 100,30);
			if (iret == CAMERA_STATUS_SUCCESS)
			{
				record_state = RECORD_START;
				printf("record start successfully!!!\n");
				return true;

			}
		}
	}
	return false;
}

bool Camera::camera_record()
{
	if (record_state == RECORD_START)
	{
		//图像头指针复制，第一个参数是复制到的地址，第二个参数是提供复制数据的指针
		memcpy(&record_hframe, &frame_h, sizeof(tSdkFrameHead));
		CameraFlipFrameBuffer(rgb_buffer, &record_hframe, 1);
		CameraPushFrame(h_camera, rgb_buffer, &record_hframe);
		return true;
	}

    return false;

}

Camera::~Camera()
{
	CameraUnInit(h_camera);
	if (rgb_buffer != nullptr)
		free(rgb_buffer);
}

bool Camera::Modify_Camera_Name() {
    int                     iCameraCounts = 1;
    int                     iStatus=-1;
    tSdkCameraDevInfo       tCameraEnumList;
    int                     hCamera;

    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    iStatus = CameraEnumerateDevice(&tCameraEnumList,&iCameraCounts);
    printf("state = %d\n", iStatus);

    printf("count = %d\n", iCameraCounts);
    printf("Camera_Name = %s\n", tCameraEnumList.acFriendlyName);

    //没有连接设备
    if(iCameraCounts==0){
        return -1;
    }


    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList,-1,-1,&hCamera);
//    CameraSetFriendlyName(hCamera,"Camera_1");
    //使用相机名字来调用相机
//    iStatus = CameraInitEx2("Camera_1",&hCamera);

    //初始化失败
    printf("state = %d\n", iStatus);
    if(iStatus!=CAMERA_STATUS_SUCCESS){
        return -1;
    }

    return true;
}

