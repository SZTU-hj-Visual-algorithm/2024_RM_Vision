//
// Created by steins_xin on 23-9-16.
//

#include "../include/HIK_Camera.h"

HIK_Camera::HIK_Camera() {
    //============================= 参数文件路径 =============================
    cv::FileStorage fs( PATH "HIKCamera_Param.yaml", cv::FileStorage::READ);
    //============================= 相机读取参数 =============================
    ExposureTime = (float)fs["ExposureTime"];
    Gain = (float)fs["Gain"];
    fs.release();
}

//============================= 相机使用函数 =============================

bool HIK_Camera::HIKCamera_Init() {
    //============================= 枚举设备 =============================
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet) {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
        return false;
    }
    if (stDeviceList.nDeviceNum > 0) {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++) {
            printf("[HIKCamera Device %d]\n", i);
            MV_CC_DEVICE_INFO *pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo) {
                return false;
            }
        }
    } else {
        printf("Find No HIKCamera!\n");
        return false;
    }
    //========================= 选择设备并创建句柄 =========================
    nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
    if (MV_OK != nRet) {
        printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
        return false;
    }
    //============================= 打开设备 =============================
    nRet = MV_CC_OpenDevice(handle);
    if (MV_OK != nRet) {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
        return false;
    }
    return true;
}


bool HIK_Camera::HIKCamera_SetParam() {
    //========================= 设置相机曝光时间 =========================
    nRet = MV_CC_SetFloatValue(handle,"ExposureTime",ExposureTime); //调节曝光  9~10000000  MV-CU013-A0UC
    if (MV_OK != nRet) {
        printf("Set_ExposureTime fail! nRet [%x]\n", nRet);
        return false;
    }
    //=========================== 设置相机增益 ==========================
    nRet = MV_CC_SetFloatValue(handle, "Gain", Gain);  //调节增益 0~16.3704 MV-CU013-A0UC
    if (MV_OK != nRet) {
        printf("Set_Gain fail! nRet [%x]\n", nRet);
        return false;
    }
    //========================== 设置相机白平衡 ==========================
    nRet = MV_CC_SetEnumValue(handle, "BalanceWhiteAuto", 1);  //设置自动白平衡(连续) 0:Off 1:Continuous 2:Once
    if(MV_OK != nRet){
        printf("Set_BalanceWhiteAuto fail! nRet [%x]\n", nRet);
        return false;
    }
    return true;
}

bool HIK_Camera::HIKCamera_startGrabbing() {
    printf("HIK_Camera open\n");
    //========================= 相机开启取流 =========================
    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
        return false;
    }
    return true;
}

void HIK_Camera::HIKCamera_read(){
    //========================= 相机读取图像 =========================
    nRet = MV_CC_GetImageBuffer(handle, &stOutFrame, 1000);
    if (nRet == MV_OK){}
    else {
        printf("No data[0x%x]\n", nRet);
        Camera_OK = false;
    }
    //========================= 相机原始图像(Bayer) =========================
    Bayer = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8UC1, stOutFrame.pBufAddr);
    //========================== 相机转换图像(RGB) ==========================
    cv::cvtColor(Bayer,src,cv::COLOR_BayerGB2RGB);
    if(cv::waitKey(1) == 27) Camera_OK = false;
    //========================== 相机释放图像缓存 ==========================
    MV_CC_FreeImageBuffer(handle, &stOutFrame);
}


bool HIK_Camera::HIKCamera_close() {
    printf("HIKCamera close\n");
    //========================= 相机停止取流 =========================
    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
        return false;
    }
    //========================== 关闭设备 ===========================
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_CloseDevice fail! nRet [%x]\n", nRet);
        return false;
    }
    //========================== 销毁句柄 ===========================
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
        return false;
    }
    printf("Exit\n");
    return true;
}

//============================= 相机调试函数 =============================

bool HIK_Camera::HIKCamera_outParam() {

    //========================== 相机曝光时间 ==========================
    MVCC_FLOATVALUE stExposureTime = {0};
    nRet = MV_CC_GetFloatValue(handle, "ExposureTime", &stExposureTime);
    if (MV_OK == nRet)
    {
        printf("ExposureTime current value:%f\n", stExposureTime.fCurValue);   //当前曝光
        printf("ExposureTime max value:%f\n", stExposureTime.fMax);           //最大设置曝光
        printf("ExposureTime min value:%f\n\n", stExposureTime.fMin);        //最小设置曝光
    }
    else
    {
        printf("Get ExposureTime failed! nRet [%x]\n\n", nRet);
        return false;
    }

    //=========================== 相机增益 ==========================
    MVCC_FLOATVALUE GainValue = {0};
    nRet = MV_CC_GetFloatValue(handle, "Gain", &GainValue);
    if (MV_OK == nRet)
    {
        printf("Gain current value:%f\n", GainValue.fCurValue);   //当前增益
        printf("Gain max value:%f\n", GainValue.fMax);           //最大设置增益
        printf("Gain min value:%f\n\n", GainValue.fMin);        //最小设置增益
    }
    else
    {
        printf("Get Gain failed! nRet [%x]\n\n", nRet);
        return false;
    }

    return true;
}

bool HIK_Camera::HIKCamera_FPS() {

    //=========================== 相机帧率 ==========================
    MVCC_FLOATVALUE FPS = {0};
    nRet = MV_CC_GetFloatValue(handle, "ResultingFrameRate", &FPS);
    if (MV_OK == nRet)
    {
        printf("FPS current value:%f\n", FPS.fCurValue);   //当前帧率
    }
    else
    {
        printf("FPS failed! nRet [%x]\n\n", nRet);
        return false;
    }

    return true;
}




