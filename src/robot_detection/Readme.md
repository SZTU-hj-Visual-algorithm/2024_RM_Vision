### 文件介绍
---
**Buff_Dubug.cpp**
这个文件主要是赛前快速调参、场上快速调参。
在文件中，我将主要的参数提取出，并应用OpenCV滑动条的方法，修改参数。
>值得注意的是这样方法，只能修改整型数据，浮点数还得自行/1000(自己根据大小缩放)

部分核心代码如下：
~~~ c++
void on_trackbar(int , void*)
{
     BD->br_blue_threshold = debug_br_blue_threshold;
     BD->br_red_threshold  = debug_br_red_threshold;
     BD->rb_red_threshold  = debug_rb_red_threshold;
     BD->rb_blue_threshold = debug_rb_blue_threshold;
     BD->r_max_area = debug_r_max_area;
     BD->r_min_area = debug_r_min_area;
     BD->leaf_max_area = debug_leaf_max_area;
     BD->leaf_min_area = debug_leaf_min_area;
     BD->leaf_r_max_distance = debug_leaf_r_max_distance;
     BD->leaf_r_min_distance = debug_leaf_r_min_distance;
     BD->r_min_hb_ratio = debug_r_min_hb_ratio / 1000.0f ; 
     BD->r_max_hb_ratio = debug_r_max_hb_ratio / 1000.0f ; 
     BD->r_min_hw_ratio = debug_r_min_hw_ratio / 1000.0f ; 
     BD->r_max_hw_ratio = debug_r_max_hw_ratio / 1000.0f ; 
     BD->leaf_min_hw_ratio = debug_leaf_min_hw_ratio / 1000.0f ; 
     BD->leaf_max_hw_ratio = debug_leaf_max_hw_ratio / 1000.0f ; 
     BD->r_min_are_ratio = debug_r_min_are_ratio / 1000.0f ; 
     BD->r_max_are_ratio = debug_r_max_are_ratio / 1000.0f ; 
     BP.SMALL_Pre_Angle_Gain = debug_SMALL_Pre_Angle_Gain / 1000.0f ; 
     BP.DELAY_TIME_ = -debug_Delay_Time_BIGBUFF / 1000.0f;
     BP.Dubug = Dubug_Dubg;
     BP.T0_N = debug_To_N * 1.0f ;
     BP.N = debug_N * 1.0f ;
    BP.clear = debug_clear;
    }
~~~





---
在RoboDetection中，想要进入能量机关识别，需要按下C键或者X键（进入大能量机关或者小能量机关模式）

**数据初始化方法**
* ROS杀死能量机关程序，清空内存(有效且便捷**当前采用**)
* 自己写初始化函数(**可能有遗漏初始化的数据**)

~~~ c++
#endif //DEBUG
if (Vision_data.mode == 0x23 || Vision_data.mode == 0x22)
    {  

        Mode = Buff;
        // 初始化判断
        double dt = (ros::Time::now() - Buff_Time).toSec();    // 计算时间间隔
        ROS_INFO("dt:%f",dt);
        if(dt > 0.2) BD = new BuffDetection;
        Buff_Time = ros::Time::now(); 

        // TODO： 注意能量机关是击打自己颜色的！
        if (Vision_data.enemy_color == 107){
            BD->our_color = BLUE;
        }
        else if (Vision_data.enemy_color == 7){
            BD->our_color = RED;
        }
        // std::cout<<BD.our_color<<std::endl;
        if (buff_init == false)
        {
            BP.Solve_OK = false;
            BD->Detection_Init();   //
            BP.Prediction_Init();
            buff_init = true;
            ROS_INFO("INIT");
         }

        // 0x23 c键    0x22 x键  BIGBUFF
        if (Vision_data.mode == 0x22)
        {
           
            BP.Buff_Mode = SMALL;
            BD->buff_mode = SMALL;
        }
        else if (Vision_data.mode == 0x23)
        {
            BP.Buff_Mode = BIG;
            BD->buff_mode = BIG;  
        }


        // FIXEME
        //  NeuralBuffDetector neuralBuff;
        //  std::vector<NBDerection> myBuff = neuralBuff.detect(src);

        // 进行识别处理
 
        std::vector<BuffTarget> finalBuffTarget;
        finalBuffTarget = BD->Detection(src);
        if(!finalBuffTarget.empty()){
            Target  =finalBuffTarget[0];
            BP.Prediction(src, Target);
        }

 
        cv::imshow("src", src);
        cv::waitKey(1);
   
        // 创建发送数据
        robot_msgs::Robot_ctrl Robot_ctrl_t;

        // 填充数据
        if (BP.Solve_OK && abs(BP.Solve_pitch)<100 && abs(BP.Solve_yaw)<100)
        {
            Robot_ctrl_t.pitch = BP.Solve_pitch;
            Robot_ctrl_t.yaw = BP.Solve_yaw;
            Robot_ctrl_t.fire_command = BP.fire_Ok;
            Robot_ctrl_t.target_lock = 0x31;
        }
        else
        {
            Robot_ctrl_t.pitch = BP.AS.Robot_msg.Controller_pitch;
            Robot_ctrl_t.yaw = BP.AS.Robot_msg.Controller_yaw;
            Robot_ctrl_t.fire_command = BP.fire_Ok;
            Robot_ctrl_t.target_lock = 0x32;
        }

        // 发送数据
        Vision_pub.publish(Robot_ctrl_t);
    }
    else
    {       
        buff_init = false;  //不进入程序就初始化改成flase ，这样每次进入模式都是重新初始化
    }
}
~~~
---
### **如何调试**
* 开启小电脑，连接图传（需要下载大疆遥控驱动程序、安装RoboMaster客户端、图传设备）**ps:相当不建议(步骤繁琐且图传设备稀有）
* **叫电控烧录代码---将上下拨拨轮改为大小符即（Vision_data.mode == 0x23 || Vision_data.mode == 0x22），同时要修改掉原来下拨自瞄、上拨弹舱盖的功能，避免功能键重复**

---
# 识别逻辑
---
>下面我会分别讲述对R标识别和Leaf_target的识别

### **R标识别逻辑**
* 方案一：采用传统视觉识别  *（传统识别不同环境，噪声非常大，误识别很高。而且不知道比赛环境的场地曝光，如果单纯传统识别R标可能风险大。）*
* 方案二：采用神经网络识别   *（神经网络识别要求很高：1、速度快  2、准确率高 。我尝试过多种模型，发现误识别率非常高（即使速度很快）（果断放弃了）*
* 方案三: 手动对准    **（有优劣吧，优点：效果很好、只有识别一次！然后自动更新   劣势：显然是需要操作手手动将海康相机图像准心对准R标。**



**补充**
海康相机中心与图传中心 不是一样的。需要额外绘制一个图像中心点
![image](https://github.com/user-attachments/assets/02e50b4b-ce7c-4ab4-b4f7-c61ccb5c7d16)

  
  
