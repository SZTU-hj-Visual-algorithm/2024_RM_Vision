# NUC_Version 

未使用ROS的版本,使用线程间通信完成

```
.
├── All_install.sh
├── CMakeLists.txt
├── include
│   ├── protocol.h
│   ├── robot_status.h
│   └── robot_struct.h
├── Init.sh
├── install.sh
├── main.cpp
├── Model
│   ├── 2023_4_9_hj_num_1.onnx
│   ├── buff_leaf_LeNet5_30x30_v7.onnx
│   └── buff_R_LeNet5_30x30_v4.onnx
├── MVS_Drive
│   ├── libMvCameraControl.so
│   ├── libMvCameraControl.so.3.2.2.1
│   └── libMVSDK.so
├── Package
│   ├── AngleSolve
│   │   ├── AngleSolve_test.cpp
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   └── AngleSolve.h
│   │   └── src
│   │       └── AngleSolve.cpp
│   ├── Armor_Detection
│   │   ├── Armor_Detection_test.cpp
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   ├── Armor_detection.h
│   │   │   └── Number_DNN.h
│   │   └── src
│   │       ├── Armor_detection.cpp
│   │       └── Number_DNN.cpp
│   ├── Armor_Observe
│   │   ├── Armor_Observe_test.cpp
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   ├── Armor_Observe.h
│   │   │   └── Circle_Kalman.h
│   │   └── src
│   │       ├── Armor_Observe.cpp
│   │       └── Circle_Kalman.cpp
│   ├── Armor_Track
│   │   ├── Armor_Track_test.cpp
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   ├── Armor_Track.h
│   │   │   ├── Kalman.h
│   │   │   └── SpinTracker.h
│   │   └── src
│   │       ├── Armor_Track.cpp
│   │       ├── Kalman.cpp
│   │       └── SpinTracker.cpp
│   ├── Buff_Detection
│   │   ├── include
│   │   │   ├── buff_detection2.h
│   │   │   └── buff_prediction.h
│   │   └── src
│   │       ├── buff_detection.cpp
│   │       └── buff_prediction.cpp
│   ├── HIK_Camera
│   │   ├── CMakeLists.txt
│   │   ├── HIK_SDK_test.cpp
│   │   ├── include
│   │   │   ├── HIK_Camera.h
│   │   │   └── HIK_Camera_H
│   │   │       ├── CameraParams.h
│   │   │       ├── MvCameraControl.h
│   │   │       ├── MvErrorDefine.h
│   │   │       ├── MvISPErrorDefine.h
│   │   │       └── PixelType.h
│   │   ├── README.md
│   │   └── src
│   │       └── HIK_Camera.cpp
│   ├── MD_Camera
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   ├── MD_Camera.h
│   │   │   └── MD_Camera_H
│   │   │       ├── CameraApi.h
│   │   │       ├── CameraDefine.h
│   │   │       └── CameraStatus.h
│   │   ├── MD_Camera_test.cpp
│   │   └── src
│   │       └── MD_Camera.cpp
│   └── Serial_Device
│       ├── CMakeLists.txt
│       ├── include
│       │   ├── crc.h
│       │   ├── serial_device.h
│       │   └── serial_main.h
│       ├── Serial_Device_test.cpp
│       ├── src
│       │   ├── crc.cpp
│       │   ├── serial_device.cpp
│       │   └── serial_main.cpp
│       └── udev
│           └── robomaster_usb.rules
├── package.sh
├── set_env.sh
├── SetParam_File
│   ├── Detect_data.yaml
│   ├── Dnn_data.yaml
│   ├── HIKCamera_Param.yaml
│   ├── Kalman_data.yaml
│   ├── Observe_data.yaml
│   ├── Solve_data.yaml
│   └── Track_data.yaml
└── Shell
    ├── install_progress.sh
    └── package_install.sh
```

MVS_Drive：海康相机驱动库

Model：模型集合包 (onnx模型)

include:：公共头文件包

Shell：脚本文件

SetParam_File：配置文件

Package=> AngleSolve：坐标系转换功能包

Package=> Armor_Detection：装甲板识别功能包

Package=> Armor_Track：装甲板跟踪功能包

Package=> Armor_Observe：整车观测功能包

Package=> Buff_Detection：能量机关功能包

Package=> HIK_Camera：海康相机功能包

Package=> MD_Camera：迈德威视功能包

Package=> Serial_Device：串口通信功能包

Package=> Serial_Device=>udev ：串口通信规则文件安装

