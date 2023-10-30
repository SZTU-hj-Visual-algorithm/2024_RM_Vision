"""
    海康工业相机 python 封装库
"""
import cv2
import sys
import numpy as np
sys.path.append("MvImport/include")
from MvImport.include.MvCameraControl_class import *

class HIK_Camera:

    def __init__(self,ExposureTime,Gain):
        # 相机参数(曝光,增益)
        self.ExposureTime = ExposureTime
        self.Gain = Gain
        # 相机参数变量
        self.nIndex = 0                                     # 相机索引
        self.camera = MvCamera()                            # 创建相机实例
        self.Camera_OK = True                               # 相机读取图片标志
        self.src = None                                     # 相机读取图片
        self.nRet = MV_OK                                   # 相机错误码
        self.deviceList = MV_CC_DEVICE_INFO_LIST()          # 枚举传输层
        self.tlayerType = MV_GIGE_DEVICE | MV_USB_DEVICE    # 设备列表
        # 相机读取图片原始数据
        self.stOutFrame = MV_FRAME_OUT()
        memset(byref(self.stOutFrame), 0, sizeof(self.stOutFrame))

    def HIKCamera_Init(self):
        # ============================= 枚举设备 =============================
        self.nRet = MvCamera.MV_CC_EnumDevices(self.tlayerType, self.deviceList)
        if self.nRet != MV_OK:
            print("MV_CC_EnumDevices fail! nRet [0x%x]" % self.nRet)
            return False

        if(self.deviceList.nDeviceNum > 0):
            for i in range(0, self.deviceList.nDeviceNum):
                mvcc_dev_info = cast(self.deviceList.pDeviceInfo[i], POINTER(MV_CC_DEVICE_INFO)).contents
                if mvcc_dev_info.nTLayerType == MV_USB_DEVICE:
                    print("HIKCamera Device: [%d]" % i)
        else:
            print("Find No HIKCamera!")
            return False

        # ========================= 选择设备并创建句柄 =========================
        stDeviceList = cast(self.deviceList.pDeviceInfo[int(self.nIndex)], POINTER(MV_CC_DEVICE_INFO)).contents       # 选择设备并创建句柄
        self.nRet = self.camera.MV_CC_CreateHandle(stDeviceList)     # 创建相机句柄
        if self.nRet != MV_OK:
            print("MV_CC_CreateHandle fail! nRet [0x%x]" % self.nRet)
            return False
        # ============================= 打开设备 =============================
        self.nRet = self.camera.MV_CC_OpenDevice(MV_ACCESS_Exclusive, 0)
        if self.nRet != MV_OK:
            print("MV_CC_OpenDevice fail! nRet [0x%x]" % self.nRet)
            return False

        return True

    def HIKCamera_startGrabbing(self):
        print("HIK_Camera open")
        # ========================= 相机开启取流 =========================
        self.nRet = self.camera.MV_CC_StartGrabbing()
        if self.nRet != MV_OK:
            print("start grabbing fail! nRet[0x%x]" % self.nRet)
            return False

        return True

    def HIKCamera_SetParam(self):
        # ========================= 设置相机曝光时间 =========================
        self.nRet = self.camera.MV_CC_SetFloatValue("ExposureTime", float(self.ExposureTime))
        if self.nRet != MV_OK:
            print("Set_ExposureTime fail! nRet[0x%x]" % self.nRet)
            return False
        # =========================== 设置相机增益 ==========================
        self.nRet = self.camera.MV_CC_SetFloatValue("Gain", float(self.Gain))
        if self.nRet != MV_OK:
            print("Set_Gain fail! nRet[0x%x]" % self.nRet)
            return False

        return True

    def HIKCamera_read(self):
        # ========================= 相机读取图像 =========================
        self.nRet = self.camera.MV_CC_GetImageBuffer(self.stOutFrame, 1000)
        if None == self.stOutFrame.pBufAddr or self.nRet != MV_OK:
            self.Camera_OK = False
            print("No data![0x%x]" % self.nRet)
        # ========================= 相机原始图像(Bayer) =========================
        pData = (c_ubyte * self.stOutFrame.stFrameInfo.nWidth * self.stOutFrame.stFrameInfo.nHeight)()
        cdll.msvcrt.memcpy(byref(pData), self.stOutFrame.pBufAddr,
                           self.stOutFrame.stFrameInfo.nWidth * self.stOutFrame.stFrameInfo.nHeight)
        data = np.frombuffer(pData, count=int(self.stOutFrame.stFrameInfo.nWidth * self.stOutFrame.stFrameInfo.nHeight),
                             dtype=np.uint8)
        image = data.reshape((self.stOutFrame.stFrameInfo.nHeight, self.stOutFrame.stFrameInfo.nWidth))
        # ========================== 相机转换图像(RGB) ==========================
        image = cv2.cvtColor(image,cv2.COLOR_BayerGB2RGB)
        self.src = image.copy()
        if (cv2.waitKey(1) == 27):
            self.Camera_OK = False
        # ========================== 相机释放图像缓存 ==========================
        self.camera.MV_CC_FreeImageBuffer(self.stOutFrame)

    def HIKCamera_close(self):
        print("HIKCamera close")
        # ========================= 相机停止取流 =========================
        self.nRet = self.camera.MV_CC_StopGrabbing()
        if self.nRet != MV_OK:
            print("stop grabbing fail! ret[0x%x]" % self.nRet)
            return False

        self.nRet = self.camera.MV_CC_CloseDevice()
        if self.nRet != MV_OK:
            print("close deivce fail! ret[0x%x]" % self.nRet)
            return False

        self.nRet = self.camera.MV_CC_DestroyHandle()
        if self.nRet != MV_OK:
            print("destroy handle fail! ret[0x%x]" % self.nRet)
            return False

        return True






