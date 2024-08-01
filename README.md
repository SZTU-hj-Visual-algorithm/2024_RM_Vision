# Sentinel_ROS_Version

哨兵的ROS代码，分为左头和右头，两个头之间用多机通信进行通信

```
.
├── CMakeLists.txt
├── communication
│   ├── CMakeLists.txt
│   ├── Kalman
│   │   ├── Kalman.cpp
│   │   └── Kalman.h
│   ├── package.xml
│   ├── src
│   │   ├── PTZ_perception_L.cpp
│   │   ├── PTZ_perception_R.cpp
│   │   └── PTZ_Yaw_Receive.cpp
│   └── test
│       └── PTZ_test.cpp
├── hik_camera
│   ├── CMakeLists.txt
│   ├── HIK_Camera_L_node.cpp
│   ├── HIK_Camera_R_node.cpp
│   ├── include
│   │   ├── HIK_Camera.h
│   │   └── HIK_Camera_H
│   │       ├── CameraParams.h
│   │       ├── MvCameraControl.h
│   │       ├── MvErrorDefine.h
│   │       ├── MvISPErrorDefine.h
│   │       └── PixelType.h
│   ├── launch
│   │   ├── HIKCamera_L_test.launch
│   │   └── HIKCamera_R_test.launch
│   ├── MVS_Drive
│   │   ├── libMvCameraControl.so
│   │   ├── libMvCameraControl.so.3.2.2.1
│   │   └── libMVSDK.so
│   ├── package.xml
│   ├── Receive_L_test.cpp
│   ├── Receive_R_test.cpp
│   └── src
│       └── HIK_Camera.cpp
├── Model
│   ├── 2023_4_9_hj_num_1.onnx
│   ├── buff_leaf_LeNet5_30x30_v7.onnx
│   ├── buff_R_LeNet5_30x30_v4.onnx
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
│   └── epz165-bz32-SOTA-additionData
│       ├── detectpointIndex改为31_模型使用须知.txt
│       └── weights
│           ├── best.onnx
│           ├── best_openvino_model_fp16
│           │   ├── best.bin
│           │   ├── best.xml
│           │   └── metadata.yaml
│           ├── best_openvino_model_int8
│           │   ├── best.bin
│           │   ├── best.xml
│           │   └── metadata.yaml
│           ├── best.pt
│           └── last.pt
├── omni_perception
│   ├── CMakeLists.txt
│   ├── launch
│   │   └── omni_perception.launch
│   ├── Omnidirectional_decision.cpp
│   ├── package
│   │   ├── AngleSolve
│   │   │   ├── include
│   │   │   │   └── AngleSolve.h
│   │   │   └── src
│   │   │       └── AngleSolve.cpp
│   │   ├── Armor_Detection
│   │   │   ├── include
│   │   │   │   ├── Armor_detection.h
│   │   │   │   └── Number_DNN.h
│   │   │   └── src
│   │   │       ├── Armor_detection.cpp
│   │   │       └── Number_DNN.cpp
│   │   ├── Armor_Track
│   │   │   ├── include
│   │   │   │   ├── Armor_Track.h
│   │   │   │   ├── Kalman.h
│   │   │   │   ├── Order_Kalman.h
│   │   │   │   └── SpinTracker.h
│   │   │   └── src
│   │   │       ├── Armor_Track.cpp
│   │   │       ├── Kalman.cpp
│   │   │       ├── Order_Kalman.cpp
│   │   │       └── SpinTracker.cpp
│   │   └── Neural_Armor_Detection
│   │       ├── include
│   │       │   └── Neural_Armor_Detection.h
│   │       ├── Neural_Armor_Detection_test.cpp
│   │       └── src
│   │           └── Neural_Armor_Detection.cpp
│   ├── package.xml
│   ├── src
│   │   ├── camera_F_node.cpp
│   │   ├── camera_L_node.cpp
│   │   ├── camera_O_node.cpp
│   │   └── camera_R_node.cpp
│   ├── udev
│   │   ├── setup.sh
│   │   └── usb_camera.rules
│   └── USB_Camera_Param.yaml
├── public_include
│   ├── protocol.h
│   ├── robot_status.h
│   └── robot_struct.h
├── robot_detection
│   ├── CMakeLists.txt
│   ├── launch
│   │   ├── RM_Detection_L.launch
│   │   └── RM_Detection_R.launch
│   ├── package.xml
│   ├── problem.log
│   ├── Robot_Detection_L.cpp
│   ├── Robot_Detection_R.cpp
│   ├── SetParam_File
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
│       └── Neural_Armor_Detection
│           ├── include
│           │   └── Neural_Armor_Detection.h
│           ├── Neural_Armor_Detection_test.cpp
│           └── src
│               └── Neural_Armor_Detection.cpp
├── robot_msgs
│   ├── CMakeLists.txt
│   ├── msg
│   │   ├── omni_perception.msg
│   │   ├── PTZ_perception.msg
│   │   ├── PTZ_Yaw.msg
│   │   ├── robot_ctrl.msg
│   │   ├── Track_reset.msg
│   │   ├── usb_camera.msg
│   │   ├── vision.msg
│   │   └── Yaw_Decision.msg
│   └── package.xml
└── serial_device
    ├── CMakeLists.txt
    ├── include
    │   ├── crc.h
    │   ├── serial_device.h
    │   └── serial_main.h
    ├── launch
    │   ├── SerialDevice_L_test.launch
    │   └── SerialDevice_R_test.launch
    ├── package.xml
    ├── Serial_Device_Send_L_node.cpp
    ├── Serial_Device_Send_R_node.cpp
    ├── src
    │   ├── crc.cpp
    │   ├── serial_device.cpp
    │   └── serial_main.cpp
    └── udev
        ├── robomaster_usb.rules
        ├── Sentinel_usb.rules
        └── setup.sh
```

功能包简介：

communication：双头功能决策功能包

serial_device：串口通信功能包

robot_msgs：ROS自定义消息包

hik_camera：海康相机功能包

Model：模型集合包 (onnx模型)

public_include:：公共头文件包

omni_perception：全向感知功能包

omni_perception=>udev：USB相机udev规则

robot_detection => SetParam_File：配置文件

robot_detection => AngleSolve：坐标系转换功能包

robot_detection => Armor_Detection：装甲板识别功能包

robot_detection => Armor_Track：装甲板跟踪功能包

robot_detection => Armor_Observe：整车观测功能包

robot_detection => Neural_Armor_Detection：装甲板深度学习识别功能包

robot_detection =>  Outpost_Observe：反前哨站功能包