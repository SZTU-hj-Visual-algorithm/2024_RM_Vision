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

>下面我会分别讲述对R标识别和Leaf_target的识别

### **R标识别逻辑**
* 方案一：采用传统视觉识别  *（传统识别不同环境，噪声非常大，误识别很高。而且不知道比赛环境的场地曝光，如果单纯传统识别R标可能风险大。）*
* 方案二：采用神经网络识别   *（神经网络识别要求很高：1、速度快  2、准确率高 。我尝试过多种模型，发现误识别率非常高（即使速度很快）（果断放弃了）*
* 方案三: 手动对准    **（有优劣吧，优点：效果很好、只有识别一次！然后自动更新   劣势：显然是需要操作手手动将海康相机图像准心对准R标。**



**补充**
海康相机中心与图传中心 不是一样的。需要额外绘制一个图像中心点
<img src="https://github.com/user-attachments/assets/02e50b4b-ce7c-4ab4-b4f7-c61ccb5c7d16"  style="width: 70%; margin: auto; display: block;" />

 
**识别逻辑**
* 1、先传统识别一下R（物理信息）
* 2、如果识别到的R标，距离中心很近（范围内）就可以确定这是R标
* 3、得到这个R标，先获取到其三维坐标系下的三维坐标并储存
* 4、不用进入传统识别程序内，进入坐标转换程序内：不断获取三维坐标转图像坐标。此时这个图像坐标就是R标位置。（**ps：如果不更新三维坐标的话，三维坐标会漂移进而导致映射到图像上坐标也会漂移。因此需要更新**）
* 5、更新：设置更新时间，如果进入更新模式，那么距离上一帧R坐标最近那个目标坐标就是当前R标的实际坐标。更新三维坐标
* 6、闭环完成

**R标的重要性**
R标的位置，可以帮助我们通过角度、半径计算目标预测点位置。

---
### **Leaf_Target识别流程**

> 击打目标图案，可以使用简化识别实现。

当时识别方案
* 深度学习大模型     （耗时太大）
* 传统     （会面临鲁棒性差的问题。视频跑的好但是实际测试会面临各种曝光问题，会导致的灯条粘连，从而影响识别的思路）
* 传统 + 神经网络小模型 （速度快，且图案就那几个，鲁棒性可以）

**神经网络模型**

*调参是艺术活*
~~~python
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import torchvision.transforms as transforms
from matplotlib import pyplot as plt
from torchvision.datasets import ImageFolder
from torch.utils.data import DataLoader
import torch.onnx
from tqdm import tqdm
from tool.Matplot_Tool import plot_data
from tool.Train_Tool import train_model, SaveModel, EarlyStopping
from tool.Train_Tool import evaluate_model

from torch.optim.lr_scheduler import CosineAnnealingLR
from torch.nn.utils import clip_grad_norm_

# 数据转换和加载
transform = transforms.Compose([
    transforms.Grayscale(num_output_channels=1) ,
    # transforms.RandomHorizontalFlip(),
    # transforms.RandomVerticalFlip(),
    # transforms.RandomRotation(10),
    transforms.ToTensor(),
 ])
test_transform = transforms.Compose([
    transforms.Grayscale(num_output_channels=1),
    transforms.ToTensor()


])
# 数据集的根目录包含两个子文件夹，每个子文件夹对应一个类别
train_dataset = ImageFolder(root='HandleDatasets/train', transform=transform)
test_dataset = ImageFolder(root='HandleDatasets/test', transform=test_transform)

train_loader = DataLoader(train_dataset, batch_size=32, shuffle=True)
test_loader = DataLoader(test_dataset, batch_size=32, shuffle=False)


class SimpleLeNet5(nn.Module):
    def __init__(self, num_classes):
        super(SimpleLeNet5, self).__init__()
        self.conv1 = nn.Conv2d(1, 6, kernel_size=2)

        self.pool1 = nn.MaxPool2d(kernel_size=2, stride=2)
        self.conv2 = nn.Conv2d(6, 12, kernel_size=2)

        self.pool2 = nn.MaxPool2d(kernel_size=2, stride=2)
        self.fc1 = nn.Linear(12 * 6 * 6, 256)   #432个参数
        self.dropout= nn.Dropout(0.5)
        self.fc2 = nn.Linear(256, 128)
        self.fc3 = nn.Linear(128, 64)
        self.fc4 = nn.Linear(64, num_classes)


    def forward(self, x):
        # print(x.shape)
        x = self.pool1(torch.relu(self.conv1(x)))
        x = self.pool2(torch.relu(self.conv2(x)))
        # print(x.shape)
        x = x.view(x.size(0), -1)
        x = torch.relu(self.fc1(x))
        x = self.dropout(x)
        x = torch.relu(self.fc2(x))
        x = self.dropout(x)
        x = torch.relu(self.fc3(x))
        x = self.dropout(x)
        x = self.fc4(x)
        return x

def main():
    # 创建模型并将其移动到GPU上
    num_classes = 2
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

    model = SimpleLeNet5(num_classes=num_classes).to(device)
    # 实例化一个TensorBoard写入器
    # writer = SummaryWriter()
    # 定义损失函数和优化器
    num_epochs = 100
    patience = 10  # 设置为你认为合适的值

    learning_rate = 0.005
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate)
    scheduler = CosineAnnealingLR(optimizer, eta_min=0.0001 , T_max=num_epochs)

    # 定义余弦退火学习率调度器
    # 训练模型

    train_losses, test_losses, train_accs, test_accs = [], [], [], []
    learning_rates = []
    best_val_acc = 0.0
    best_epoch = 0
    early_stopping = EarlyStopping(patience=10, verbose=True)
    for epoch in range(num_epochs):
        progress_bar_train = tqdm(train_loader, desc=f'Train epoch {epoch + 1} / {num_epochs}', mininterval=0.3)
        train_loss, train_acc = train_model(model, progress_bar_train, criterion, optimizer, device, epoch,
                                            progress_bar_train)
        train_losses.append(train_loss)
        train_accs.append(train_acc)

        progress_bar_val = tqdm(test_loader, desc=f'Val epoch {epoch + 1} / {num_epochs}', mininterval=0.3)
        test_loss, test_acc = evaluate_model(model, progress_bar_val, criterion, device)
        test_losses.append(test_loss)
        test_accs.append(test_acc)

        # 更新学习率
        scheduler.step()
        # 记录学习率
        current_lr = optimizer.param_groups[0]['lr']
        learning_rates.append(current_lr)
        # 检查是否发生了过拟合
        early_stopping(test_loss,model,device)
        # 如果早停法触发，则提前结束训练
        if early_stopping.early_stop:
            # SaveModel(model, device)
            print("Early stopping")
            break

        # 打印训练和验证信息
        print(f'Train - Epoch [{epoch + 1}/{num_epochs}] - Loss: {train_loss:.6f}, Accuracy: {train_acc:.2f}%')
        print(f'Validation - Epoch [{epoch + 1}/{num_epochs}] - Loss: {test_loss:.6f}, Accuracy: {test_acc:.2f}%')

        # 绘制损失和准确率曲线
    plot_data(train_losses, test_losses, train_accs, test_accs)

    # 绘制学习率变化图像
    plt.figure()
    plt.plot(range(1, len(learning_rates) + 1), learning_rates, label='Learning Rate')
    plt.xlabel('Epoch')
    plt.ylabel('Learning Rate')
    plt.title('Learning Rate Schedule')
    plt.legend()
    plt.show()
    # 导出为ONNX格式（尽量使ONNX文件小于1MB）
    SaveModel(model , device)
    # 绘制Loss曲线图

if __name__ == "__main__":
    main()

~~~

**数据集**

*falseHandle*

<img src="https://github.com/user-attachments/assets/f07e7d59-2059-49ec-9be8-a36ae1ab3161"  style="width: 50% ;height:50%; margin: auto; display: block;" />

*trueHanle*

<img src="https://github.com/user-attachments/assets/19ec96aa-c528-41d8-9da8-f4de84345235"  style="width: 50% ;height:50%; margin: auto; display: block;" />
 
*true*

<img src="https://github.com/user-attachments/assets/62694466-9d63-4d23-9241-af435492cefd"  style="width: 50% ;height:50%; margin: auto; display: block;" />

*false*

<img src="https://github.com/user-attachments/assets/a16ed089-c4d7-44e8-8ac7-3af0938c09aa"  style="width: 50% ;height:50%; margin: auto; display: block;" />

*false1*

<img src="https://github.com/user-attachments/assets/45293255-30f9-4dbe-8c00-5b396408245d"  style="width: 50% ;height:50%; margin: auto; display: block;" />
 


 
 

  
