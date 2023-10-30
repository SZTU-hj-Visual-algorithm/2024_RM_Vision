RM_Detection

对去年代码的重构和功能添加(串口部分还未重构)

今年添加了整车观测的功能

文件目标结构说明：

```c++
.
├── Dynamic_library			// 放置编译之后的动态库
│   └── include				  // 功能包头文件
│       └── HIK_Camera_H				  // 相机头文件    
│ 
├── images				  // 测试资料
├── include				  // 公共头文件(功能包都会调用)
├── Model				  // 数字识别模型
├── MVS_Drive				  // 相机所需驱动
├── Package				  // 功能包
│   ├── AngleSolve				  // 坐标系转换
│   │   ├── include
│   │   └── src
│   ├── Armor_Detection				  // 装甲板识别
│   │   ├── include
│   │   └── src
│   ├── Armor_Observe				  // 整车观测
│   │   ├── include
│   │   └── src
│   ├── Armor_Track				  // 装甲板跟踪及反陀螺
│   │   ├── include
│   │   └── src
│   ├── HIK_Camera				  // 海康工业相机
│   │   ├── include
│   │   │   └── HIK_Camera_H
│   │   └── src
│   ├── MD_Camera				  // 迈德威视相机
│   │   ├── include
│   │   │   └── MD_Camera_H
│   │   ├── MD_Camera
│   │   │   └── camera
│   │   └── src
│   └── Thread				  // 多线程
│       ├── include
│       └── src
└── SetParam_File				  // 功能包参数文件
```

