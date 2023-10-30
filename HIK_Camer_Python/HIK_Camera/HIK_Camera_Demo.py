"""
    海康工业相机 Demo
    使用方法: 按esc退出
"""
import sys
import cv2
sys.path.append("MvImport")
from MvImport.HIK_Camera import HIK_Camera

src = None
if __name__ == "__main__":
    HIK = HIK_Camera(5000.0,10.0)        # 传入曝光和增益
    # 相机初始化
    if HIK.HIKCamera_Init():
        HIK.HIKCamera_SetParam()        # 设置相机曝光和增益
        HIK.HIKCamera_startGrabbing()   # 相机取流
        while HIK.Camera_OK:
            HIK.HIKCamera_read()        # 读取图片
            src = HIK.src.copy()        # 复制图片
            cv2.imshow("src",src)
        HIK.HIKCamera_close()           # 关闭相机