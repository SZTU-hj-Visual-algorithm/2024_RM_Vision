# Buff 能量机关

## 项目目录结构

```tree
.
├── CMakeLists.txt -> /opt/ros/noetic/share/catkin/cmake/toplevel.cmake
├── hik_camera
│   ├── CMakeLists.txt
│   ├── HIK_Camera_node.cpp
│   ├── include
│   │   ├── HIK_Camera.h
│   │   └── HIK_Camera_H
│   │       ├── CameraParams.h
│   │       ├── MvCameraControl.h
│   │       ├── MvErrorDefine.h
│   │       ├── MvISPErrorDefine.h
│   │       └── PixelType.h
│   ├── MVS_Drive
│   │   ├── libMvCameraControl.so
│   │   ├── libMvCameraControl.so.3.2.2.1
│   │   └── libMVSDK.so
│   ├── package.xml
│   ├── Receive_test.cpp
│   └── src
│       └── HIK_Camera.cpp
├── Model
│   ├── 2023_4_9_hj_num_1.onnx
│   ├── cache
│   │   └── 9947341645488059932.blob
│   ├── ep165-bz32-SOTA
│   │   ├── best.onnx
│   │   ├── best_openvino_model_fp16
│   │   │   ├── best.bin
│   │   │   ├── best.xml
│   │   │   └── metadata.yaml
│   │   ├── best_openvino_model_fp32
│   │   │   ├── best.bin
│   │   │   ├── best.xml
│   │   │   └── metadata.yaml
│   │   ├── best_openvino_model_int8
│   │   │   ├── best.bin
│   │   │   ├── best.xml
│   │   │   └── metadata.yaml
│   │   ├── best.pt
│   │   └── last.pt
│   ├── epz165-bz32-SOTA-additionData
│   │   ├── detectpointIndex改为31_模型使用须知.txt
│   │   └── weights
│   │       ├── best.onnx
│   │       ├── best_openvino_model_fp16
│   │       │   ├── best.bin
│   │       │   ├── best.xml
│   │       │   └── metadata.yaml
│   │       ├── best_openvino_model_int8
│   │       │   ├── best.bin
│   │       │   ├── best.xml
│   │       │   └── metadata.yaml
│   │       ├── best.pt
│   │       └── last.pt
│   ├── LeNet_Handle_Best_v2.onnx
│   └── LeNet_Leaf_Best_v2.onnx
├── public_include
│   ├── protocol.h
│   ├── robot_status.h
│   └── robot_struct.h
├── robot_detection
│   ├── Buff_Dubug.cpp
│   ├── CMakeLists.txt
│   ├── launch
│   │   ├── Buff_Dubug.launch
│   │   └── RM_Detection.launch
│   ├── package.xml
│   ├── Robot_Detection.cpp
│   ├── SetParam_File
│   │   ├── Buff_data.yaml
│   │   ├── Detect_data.yaml
│   │   ├── Dnn_data.yaml
│   │   ├── HIKCamera_Param.yaml
│   │   ├── Kalman_data.yaml
│   │   ├── Neural_Detect_data.yaml
│   │   ├── Observe_data.yaml
│   │   ├── Solve_data.yaml
│   │   └── Track_data.yaml
│   └── src
│       ├── AngleSolve
│       │   ├── include
│       │   │   └── AngleSolve.h
│       │   └── src
│       │       └── AngleSolve.cpp
│       ├── Armor_Detection
│       │   ├── include
│       │   │   ├── Armor_detection.h
│       │   │   └── Number_DNN.h
│       │   └── src
│       │       ├── Armor_detection.cpp
│       │       └── Number_DNN.cpp
│       ├── Armor_Observe
│       │   ├── include
│       │   │   ├── Armor_Observe.h
│       │   │   └── Circle_Kalman.h
│       │   └── src
│       │       ├── Armor_Observe.cpp
│       │       └── Circle_Kalman.cpp
│       ├── Armor_Track
│       │   ├── include
│       │   │   ├── Armor_Track.h
│       │   │   ├── Kalman.h
│       │   │   ├── Order_Kalman.h
│       │   │   └── SpinTracker.h
│       │   └── src
│       │       ├── Armor_Track.cpp
│       │       ├── Kalman.cpp
│       │       ├── Order_Kalman.cpp
│       │       └── SpinTracker.cpp
│       ├── Buff_Detection
│       │   ├── buff_logs
│       │   ├── include
│       │   │   ├── BuffDetection.h
│       │   │   └── Logger.h
│       │   └── src
│       │       └── Buff_Detection.cpp
│       ├── Buff_Prediction
│       │   ├── include
│       │   │   └── BuffPrediction.h
│       │   └── src
│       │       └── Buff_Prediction.cpp
│       ├── Neural_Armor_Detection
│       │   ├── include
│       │   │   └── Neural_Armor_Detection.h
│       │   ├── Neural_Armor_Detection_test.cpp
│       │   └── src
│       │       └── Neural_Armor_Detection.cpp
│       ├── Neural_Buff_Detection
│       │   ├── include
│       │   │   └── Neural_Buff_Detection.h
│       │   ├── Neural_Armor_Detection_test.cpp
│       │   └── src
│       │       └── Neural_Buff_Detection.cpp
│       └── Outpost_Observe
│           ├── include
│           │   ├── Outpost_Kalman.h
│           │   └── Outpost_Observe.h
│           ├── Readme.md
│           └── src
│               ├── Outpost_Kalman.cpp
│               └── Outpost_Observe.cpp
├── robot_msgs
│   ├── CMakeLists.txt
│   ├── msg
│   │   ├── Robot_ctrl.msg
│   │   └── Vision.msg
│   └── package.xml
└── serial_device
    ├── CMakeLists.txt
    ├── include
    │   ├── crc.h
    │   ├── serial_device.h
    │   └── serial_main.h
    ├── package.xml
    ├── Reveice_test.cpp
    ├── Send_test.cpp
    ├── Serial_Device_Receive_node.cpp
    ├── Serial_Device_Send_node.cpp
    ├── src
    │   ├── crc.cpp
    │   ├── serial_device.cpp
    │   └── serial_main.cpp
    └── udev
        ├── robomaster_usb.rules
        └── rule_setup.sh
```

功能包简介：

serial_device：串口通信功能包

robot_msgs：ROS自定义消息包

hik_camera：海康相机功能包

Model：模型集合包 (onnx模型)

public_include:：公共头文件包

robot_detection => SetParam_File：配置文件

robot_detection => AngleSolve：坐标系转换功能包

robot_detection => Armor_Detection：装甲板识别功能包

robot_detection => Armor_Track：装甲板跟踪功能包

robot_detection => Armor_Observe：整车观测功能包

robot_detection => Buff_Detection：能量机关识别功能包

robot_detection => Buff_Prediction：能量机关预测功能包

robot_detection => Neural_Armor_Detection：装甲板深度学习识别功能包 

robot_detection => Neural_Buff_Detection：能量机关深度学习识别功能包

robot_detection =>  Outpost_Observe：反前哨站功能包
