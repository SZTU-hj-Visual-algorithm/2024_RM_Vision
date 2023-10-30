#pragma once
#ifndef _CAMERA_DEFINE_H_
#define _CAMERA_DEFINE_H_

#include "camera/CameraStatus.h"

#define MAX_CROSS_LINE 9

//����ľ�����Ͷ���
typedef int CameraHandle;
typedef int INT;
typedef int LONG;
typedef unsigned int UINT;
typedef unsigned long long UINT64;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef void* PVOID;
typedef void* HWND;
typedef char* LPCTSTR;
typedef unsigned short USHORT;
typedef short SHORT;
 typedef unsigned char* LPBYTE;
typedef char CHAR;
typedef char TCHAR;
typedef  unsigned short WORD;
typedef INT HANDLE;
typedef void VOID;
typedef unsigned int ULONG;
typedef void* LPVOID;
typedef unsigned char UCHAR;
typedef void* HMODULE;

#define TRUE 1
#define FALSE 0
//ͼ����任�ķ�ʽ
typedef enum
{
    LUTMODE_PARAM_GEN=0,//ͨ�����ڲ�����̬����LUT��
    LUTMODE_PRESET,     //ʹ��Ԥ���LUT��
    LUTMODE_USER_DEF    //ʹ���û��Զ����LUT��
}emSdkLutMode;

//�������Ƶ������
typedef enum
{
    RUNMODE_PLAY=0,    //����Ԥ��������ͼ�����ʾ�������������ڴ���ģʽ�����ȴ�����֡�ĵ�����
    RUNMODE_PAUSE,     //��ͣ������ͣ�����ͼ�������ͬʱҲ����ȥ����ͼ��
    RUNMODE_STOP       //ֹͣ�������������ʼ��������ʹ���ֹͣģʽ
}emSdkRunMode;

//SDK�ڲ���ʾ�ӿڵ���ʾ��ʽ
typedef enum
{
    DISPLAYMODE_SCALE=0, //������ʾģʽ�����ŵ���ʾ�ؼ��ĳߴ�
    DISPLAYMODE_REAL     //1:1��ʾģʽ����ͼ��ߴ������ʾ�ؼ��ĳߴ�ʱ��ֻ��ʾ�ֲ�
}emSdkDisplayMode;

//¼��״̬
typedef enum
{
  RECORD_STOP = 0,  //ֹͣ
  RECORD_START,     //¼����
  RECORD_PAUSE      //��ͣ
}emSdkRecordMode;

//ͼ��ľ������
typedef enum
{
    MIRROR_DIRECTION_HORIZONTAL = 0,//ˮƽ����
    MIRROR_DIRECTION_VERTICAL       //��ֱ����
}emSdkMirrorDirection;

/// @ingroup MV_ENUM_TYPE
/// \~chinese ͼ�����ת����
/// \~english Rotation of the image
typedef enum
{
	ROTATE_DIRECTION_0=0,		///< \~chinese ����ת		\~english Do not rotate
	ROTATE_DIRECTION_90=1,		///< \~chinese ��ʱ��90��	\~english Counterclockwise 90 degrees
	ROTATE_DIRECTION_180=2,		///< \~chinese ��ʱ��180��	\~english Counterclockwise 180 degrees
	ROTATE_DIRECTION_270=3,		///< \~chinese ��ʱ��270��	\~english Counterclockwise 270 degrees
}emSdkRotateDirection;

//�����Ƶ��֡��
typedef enum
{
    FRAME_SPEED_LOW = 0,  //����ģʽ
    FRAME_SPEED_NORMAL,   //��ͨģʽ
    FRAME_SPEED_HIGH,     //����ģʽ(��Ҫ�ϸߵĴ������,���豸���������ʱ���֡�ʵ��ȶ�����Ӱ��)
    FRAME_SPEED_SUPER     //������ģʽ(��Ҫ�ϸߵĴ������,���豸���������ʱ���֡�ʵ��ȶ�����Ӱ��)
}emSdkFrameSpeed;

//�����ļ��ĸ�ʽ����
typedef enum
{
    FILE_JPG = 1,//JPG
    FILE_BMP = 2,//BMP
    FILE_RAW = 4,//��������bayer��ʽ�ļ�,���ڲ�֧��bayer��ʽ���������޷�����Ϊ�ø�ʽ
    FILE_PNG = 8, //PNG
    FILE_BMP_8BIT = 16,	  ///< \~chinese BMP 8bit		\~english BMP 8bit
    FILE_PNG_8BIT = 32,   ///< \~chinese PNG 8bit		\~english PNG 8bit
	  FILE_RAW_16BIT = 64,	///< \~chinese RAW 16bit	\~english RAW 16bit
}emSdkFileType;

//����е�ͼ�񴫸����Ĺ���ģʽ
typedef enum
{
    CONTINUATION = 0,//�����ɼ�ģʽ
    SOFT_TRIGGER,    //�������ģʽ�����������ָ��󣬴�������ʼ�ɼ�ָ��֡����ͼ�񣬲ɼ���ɺ�ֹͣ���
    EXTERNAL_TRIGGER //Ӳ������ģʽ�������յ��ⲿ�źţ���������ʼ�ɼ�ָ��֡����ͼ�񣬲ɼ���ɺ�ֹͣ���
} emSdkSnapMode;

//�Զ��ع�ʱ��Ƶ����Ƶ��
typedef enum
{
    LIGHT_FREQUENCY_50HZ = 0,//50HZ,һ��ĵƹⶼ��50HZ
    LIGHT_FREQUENCY_60HZ     //60HZ,��Ҫ��ָ��ʾ����
}emSdkLightFrequency;

//��������ò�������ΪA,B,C,D 4����б��档
typedef enum
{
    PARAMETER_TEAM_DEFAULT = 0xff,
    PARAMETER_TEAM_A = 0,
    PARAMETER_TEAM_B = 1,
    PARAMETER_TEAM_C = 2,
    PARAMETER_TEAM_D = 3
}emSdkParameterTeam;


/*emSdkParameterMode �����������ģʽ���������ط�Ϊ���ļ��ʹ��豸�������ַ�ʽ

PARAM_MODE_BY_MODEL:����ͬ�ͺŵ��������ABCD��������ļ����޸�
             һ̨����Ĳ����ļ�����Ӱ�쵽����ͬ�ͺŵ�
             ����������ء�

PARAM_MODE_BY_NAME:�����豸����ͬ�����������ABCD��������ļ���
         Ĭ������£���������ֻ����ĳ�ͺ�һ̨���ʱ��
         �豸������һ���ģ�����ϣ��ĳһ̨����ܹ�����
         ��ͬ�Ĳ����ļ��������ͨ���޸����豸���ķ�ʽ
         ���������ָ���Ĳ����ļ���

PARAM_MODE_BY_SN:��������Լ���Ψһ���к�������ABCD��������ļ���
         ���к��ڳ���ʱ�Ѿ��̻�������ڣ�ÿ̨��������к�
         ������ͬ��ͨ�����ַ�ʽ��ÿ̨����Ĳ����ļ����Ƕ����ġ�

�����Ը����Լ���ʹ�û��������ʹ�����ϼ��ַ�ʽ���ز��������磬��
MV-U300Ϊ������ϣ����̨���ͺŵ���������� �����϶�����4���������ô��
ʹ��PARAM_MODE_BY_MODEL��ʽ;�����ϣ������ĳһ̨����ĳ��̨MV-U300��
ʹ���Լ������ļ��������MV-U300��Ҫʹ����ͬ�Ĳ����ļ�����ôʹ��
PARAM_MODE_BY_NAME��ʽ;�����ϣ��ÿ̨MV-U300��ʹ�ò�ͬ�Ĳ����ļ�����ô
ʹ��PARAM_MODE_BY_SN��ʽ��
�����ļ����ڰ�װĿ¼�� \Camera\Configs Ŀ¼�£���configΪ��׺�����ļ���
*/
typedef enum
{
  PARAM_MODE_BY_MODEL = 0,  //��������ͺ������ļ��м��ز���������MV-U300
  PARAM_MODE_BY_NAME,       //�����豸�ǳ�(tSdkCameraDevInfo.acFriendlyName)���ļ��м��ز���������MV-U300,���ǳƿ��Զ���
  PARAM_MODE_BY_SN,         //�����豸��Ψһ���кŴ��ļ��м��ز��������к��ڳ���ʱ�Ѿ�д���豸��ÿ̨���ӵ�в�ͬ�����кš�
  PARAM_MODE_IN_DEVICE      //���豸�Ĺ�̬�洢���м��ز������������е��ͺŶ�֧�ִ�����ж�д�����飬��tSdkCameraCapbility.bParamInDevice����
}emSdkParameterMode;


//SDK���ɵ��������ҳ������ֵ
typedef enum
{
  PROP_SHEET_INDEX_EXPOSURE = 0,
  PROP_SHEET_INDEX_ISP_COLOR,
  PROP_SHEET_INDEX_ISP_LUT,
  PROP_SHEET_INDEX_ISP_SHAPE,
  PROP_SHEET_INDEX_VIDEO_FORMAT,
  PROP_SHEET_INDEX_RESOLUTION,
  PROP_SHEET_INDEX_IO_CTRL,
  PROP_SHEET_INDEX_TRIGGER_SET,
  PROP_SHEET_INDEX_OVERLAY,
  PROP_SHEET_INDEX_DEVICE_INFO,
  PROP_SHEET_INDEX_WDR,
  PROP_SHEET_INDEX_MULTI_EXPOSURE
}emSdkPropSheetMask;

//SDK���ɵ��������ҳ��Ļص���Ϣ����
typedef enum
{
  SHEET_MSG_LOAD_PARAM_DEFAULT = 0, //�������ָ���Ĭ�Ϻ󣬴�������Ϣ
  SHEET_MSG_LOAD_PARAM_GROUP,       //����ָ�������飬��������Ϣ
  SHEET_MSG_LOAD_PARAM_FROMFILE,    //��ָ���ļ����ز����󣬴�������Ϣ
  SHEET_MSG_SAVE_PARAM_GROUP        //��ǰ�����鱻����ʱ����������Ϣ
}emSdkPropSheetMsg;

//���ӻ�ѡ��ο����ڵ�����
typedef enum
{
  REF_WIN_AUTO_EXPOSURE = 0,
  REF_WIN_WHITE_BALANCE,
}emSdkRefWinType;

//���ӻ�ѡ��ο����ڵ�����
typedef enum
{
  RES_MODE_PREVIEW = 0,
  RES_MODE_SNAPSHOT,
}emSdkResolutionMode;

//��ƽ��ʱɫ��ģʽ
typedef enum
{
  CT_MODE_AUTO = 0, //�Զ�ʶ��ɫ��
  CT_MODE_PRESET,   //ʹ��ָ����Ԥ��ɫ��
  CT_MODE_USER_DEF  //�Զ���ɫ��(����;���)
}emSdkClrTmpMode;

//LUT����ɫͨ��
typedef enum
{
  LUT_CHANNEL_ALL = 0,//R,B,G��ͨ��ͬʱ����
  LUT_CHANNEL_RED,    //��ɫͨ��
  LUT_CHANNEL_GREEN,  //��ɫͨ��
  LUT_CHANNEL_BLUE,   //��ɫͨ��
}emSdkLutChannel;

//ISP����Ԫ
typedef enum
{
  ISP_PROCESSSOR_PC = 0,//ʹ��PC�����ISPģ��
  ISP_PROCESSSOR_DEVICE //ʹ������Դ���Ӳ��ISPģ��
}emSdkIspProcessor;

//������źſ��Ʒ�ʽ
typedef enum
{
  STROBE_SYNC_WITH_TRIG_AUTO = 0,    //�ʹ����ź�ͬ������������������ع�ʱ���Զ�����STROBE�źš���ʱ����Ч���Կ�����(CameraSetStrobePolarity)��
  STROBE_SYNC_WITH_TRIG_MANUAL,      //�ʹ����ź�ͬ����������STROBE��ʱָ����ʱ���(CameraSetStrobeDelayTime)���ٳ���ָ��ʱ�������(CameraSetStrobePulseWidth)����Ч���Կ�����(CameraSetStrobePolarity)��
  STROBE_ALWAYS_HIGH,                //ʼ��Ϊ�ߣ�����STROBE�źŵ���������
  STROBE_ALWAYS_LOW                  //ʼ��Ϊ�ͣ�����STROBE�źŵ���������
}emStrobeControl;

//Ӳ���ⴥ�����ź�����
typedef enum
{
  EXT_TRIG_LEADING_EDGE = 0,     //�����ش�����Ĭ��Ϊ�÷�ʽ
  EXT_TRIG_TRAILING_EDGE,        //�½��ش���
  EXT_TRIG_HIGH_LEVEL,           //�ߵ�ƽ����,��ƽ��Ⱦ����ع�ʱ�䣬�������ͺŵ����֧�ֵ�ƽ������ʽ��
  EXT_TRIG_LOW_LEVEL             //�͵�ƽ����,
}emExtTrigSignal;

//Ӳ���ⴥ��ʱ�Ŀ��ŷ�ʽ
typedef enum
{
  EXT_TRIG_EXP_STANDARD = 0,     //��׼��ʽ��Ĭ��Ϊ�÷�ʽ��
  EXT_TRIG_EXP_GRR,              //ȫ�ָ�λ��ʽ�����ֹ������ŵ�CMOS�ͺŵ����֧�ָ÷�ʽ������ⲿ��е���ţ����Դﵽȫ�ֿ��ŵ�Ч�����ʺ��ĸ����˶�������
}emExtTrigShutterMode;

// GPIOģʽ
typedef enum
{
  IOMODE_TRIG_INPUT=0,		    ///< \~chinese �������� \~english Trigger input
  IOMODE_STROBE_OUTPUT=1,		  ///< \~chinese �������� \~english Strobe output
  IOMODE_GP_INPUT=2,			    ///< \~chinese ͨ�������� \~english Universal input
  IOMODE_GP_OUTPUT=3,			    ///< \~chinese ͨ������� \~english Universal output
  IOMODE_PWM_OUTPUT=4,		    ///< \~chinese PWM����� \~english PWM output
  IOMODE_ROTARYENC_INPUT=5,   ///< \~chinese ���������� \~english rotary input
}emCameraGPIOMode;

/// @ingroup MV_ENUM_TYPE
/// \~chinese GPIO ��ʽ
/// \~english GPIO Format
typedef enum 
{
	IOFORMAT_SINGLE=0,			///< \~chinese ����  \~english single ended
	IOFORMAT_RS422=1,			  ///< \~chinese ���RS422 \~english Differential RS422
	IOFORMAT_RS422_TERM=2,	///< \~chinese ���RS422���ն˵��� \~english Differential RS422 and Termination Enable
}emCameraGPIOFormat;

/// @ingroup MV_ENUM_TYPE
/// \~chinese ȡͼ���ȼ�
/// \~english Get Image priority
typedef enum
{
	CAMERA_GET_IMAGE_PRIORITY_OLDEST=0,		///< \~chinese ��ȡ��������ɵ�һ֡ \~english	Get the oldest frame in the cache
	CAMERA_GET_IMAGE_PRIORITY_NEWEST=1,		///< \~chinese ��ȡ���������µ�һ֡���ȴ�֡�ɵĽ�ȫ�������� \~english Get the latest frame in the cache (older than this frame will be discarded)

	/// \~chinese ���������е�����֡����������˿���������ع���佫�ᱻ������ϣ��ȴ�������һ֡
	/// \note ĳЩ�ͺŵ������֧�ִ˹��ܣ����ڲ�֧�ִ˹��ܵ���������־�൱��@link #CAMERA_GET_IMAGE_PRIORITY_OLDEST @endlink
	/// \~english All frames in the cache are discarded, and if the MD_Camera_H is now being exposed or transmitted it will be immediately interrupted, waiting to receive the next frame
	/// \note Some models do not support this feature. For cameras that do not support this feature this flag is equivalent to @link #CAMERA_GET_IMAGE_PRIORITY_OLDEST @endlink
	CAMERA_GET_IMAGE_PRIORITY_NEXT=2,
}emCameraGetImagePriority;

/// @ingroup MV_ENUM_TYPE
/// \~chinese �������ܱ�־
/// \~english Soft trigger function flag
typedef enum
{
	CAMERA_ST_CLEAR_BUFFER_BEFORE	= 0x1,	///< \~chinese ������֮ǰ���������ѻ����֡ \~english Empty MD_Camera_H-cached frames before soft triggering
}emCameraSoftTriggerExFlags;

//������豸��Ϣ
typedef struct
{
    char acProductSeries[32];   // ��Ʒϵ��
    char acProductName[32];     // ��Ʒ����
    char acFriendlyName[32];    // ��Ʒ�ǳƣ��û����Զ�����ǳƣ�����������ڣ��������ֶ�����ͬʱʹ��,������CameraSetFriendlyName�ӿڸı���ǳƣ��豸��������Ч��
    char acLinkName[32];        // �ں˷������������ڲ�ʹ��
    char acDriverVersion[32];   // �����汾
    char acSensorType[32];      // sensor����
    char acPortType[32];        // �ӿ�����
    char acSn[32];              // ��ƷΨһ���к�
    UINT uInstance;             // ���ͺ�����ڸõ����ϵ�ʵ�������ţ���������ͬ�ͺŶ����
} tSdkCameraDevInfo;

#define EXT_TRIG_MASK_GRR_SHUTTER  1	///< \~chinese ����֧��GRRģʽ \~english Shutter supports GRR mode
#define EXT_TRIG_MASK_LEVEL_MODE   2	///< \~chinese ֧�ֵ�ƽ���� \~english Support level trigger
#define EXT_TRIG_MASK_DOUBLE_EDGE  4	///< \~chinese ֧��˫���ش��� \~english Supports bilateral triggering

//tSdkResolutionRange�ṹ����SKIP�� BIN��RESAMPLEģʽ������ֵ
#define MASK_2X2_HD     (1<<0)    //Ӳ��SKIP��BIN���ز��� 2X2
#define MASK_3X3_HD     (1<<1)
#define MASK_4X4_HD     (1<<2)
#define MASK_5X5_HD     (1<<3)
#define MASK_6X6_HD     (1<<4)
#define MASK_7X7_HD     (1<<5)
#define MASK_8X8_HD     (1<<6)
#define MASK_9X9_HD     (1<<7)
#define MASK_10X10_HD   (1<<8)
#define MASK_11X11_HD   (1<<9)
#define MASK_12X12_HD   (1<<10)
#define MASK_13X13_HD   (1<<11)
#define MASK_14X14_HD   (1<<12)
#define MASK_15X15_HD   (1<<13)
#define MASK_16X16_HD   (1<<14)
#define MASK_17X17_HD   (1<<15)
#define MASK_2X2_SW     (1<<16)   //Ӳ��SKIP��BIN���ز��� 2X2
#define MASK_3X3_SW     (1<<17)
#define MASK_4X4_SW     (1<<18)
#define MASK_5X5_SW     (1<<19)
#define MASK_6X6_SW     (1<<20)
#define MASK_7X7_SW     (1<<21)
#define MASK_8X8_SW     (1<<22)
#define MASK_9X9_SW     (1<<23)
#define MASK_10X10_SW   (1<<24)
#define MASK_11X11_SW   (1<<25)
#define MASK_12X12_SW   (1<<26)
#define MASK_13X13_SW   (1<<27)
#define MASK_14X14_SW   (1<<28)
#define MASK_15X15_SW   (1<<29)
#define MASK_16X16_SW   (1<<30)
#define MASK_17X17_SW   (1<<31)

//����ķֱ����趨��Χ�����ڹ���UI
typedef struct
{
  INT iHeightMax;             //ͼ�����߶�
  INT iHeightMin;             //ͼ����С�߶�
  INT iWidthMax;              //ͼ�������
  INT iWidthMin;              //ͼ����С���
  UINT uSkipModeMask;         //SKIPģʽ���룬Ϊ0����ʾ��֧��SKIP ��bit0Ϊ1,��ʾ֧��SKIP 2x2 ;bit1Ϊ1����ʾ֧��SKIP 3x3....
  UINT uBinSumModeMask;       //BIN(���)ģʽ���룬Ϊ0����ʾ��֧��BIN ��bit0Ϊ1,��ʾ֧��BIN 2x2 ;bit1Ϊ1����ʾ֧��BIN 3x3....
  UINT uBinAverageModeMask;   //BIN(���ֵ)ģʽ���룬Ϊ0����ʾ��֧��BIN ��bit0Ϊ1,��ʾ֧��BIN 2x2 ;bit1Ϊ1����ʾ֧��BIN 3x3....
  UINT uResampleMask;         //Ӳ���ز���������
} tSdkResolutionRange;


//����ķֱ�������
typedef struct
{
  INT     iIndex;             // �����ţ�[0,N]��ʾԤ��ķֱ���(N ΪԤ��ֱ��ʵ���������һ�㲻����20),OXFF ��ʾ�Զ���ֱ���(ROI)
  char    acDescription[32];  // �÷ֱ��ʵ�������Ϣ����Ԥ��ֱ���ʱ����Ϣ��Ч���Զ���ֱ��ʿɺ��Ը���Ϣ
  UINT    uBinSumMode;        // BIN(���)��ģʽ,��Χ���ܳ���tSdkResolutionRange��uBinSumModeMask
  UINT    uBinAverageMode;    // BIN(���ֵ)��ģʽ,��Χ���ܳ���tSdkResolutionRange��uBinAverageModeMask
  UINT    uSkipMode;          // �Ƿ�SKIP�ĳߴ磬Ϊ0��ʾ��ֹSKIPģʽ����Χ���ܳ���tSdkResolutionRange��uSkipModeMask
  UINT    uResampleMask;      // Ӳ���ز���������
  INT     iHOffsetFOV;        // �ɼ��ӳ������Sensor����ӳ����ϽǵĴ�ֱƫ��
  INT     iVOffsetFOV;        // �ɼ��ӳ������Sensor����ӳ����Ͻǵ�ˮƽƫ��
  INT     iWidthFOV;          // �ɼ��ӳ��Ŀ��
  INT     iHeightFOV;         // �ɼ��ӳ��ĸ߶�
  INT     iWidth;             // ������������ͼ��Ŀ��
  INT     iHeight;            // ������������ͼ��ĸ߶�
  INT     iWidthZoomHd;       // Ӳ�����ŵĿ��,����Ҫ���д˲����ķֱ��ʣ��˱�������Ϊ0.
  INT     iHeightZoomHd;      // Ӳ�����ŵĸ߶�,����Ҫ���д˲����ķֱ��ʣ��˱�������Ϊ0.
  INT     iWidthZoomSw;       // ������ŵĿ��,����Ҫ���д˲����ķֱ��ʣ��˱�������Ϊ0.
  INT     iHeightZoomSw;      // ������ŵĸ߶�,����Ҫ���д˲����ķֱ��ʣ��˱�������Ϊ0.
} tSdkImageResolution;

//�����ƽ��ɫ��ģʽ������Ϣ
typedef struct
{
    INT  iIndex;            // ģʽ������
    char acDescription[32]; // ������Ϣ
} tSdkColorTemperatureDes;

//���֡��������Ϣ
typedef struct
{
    INT  iIndex;             // ֡�������ţ�һ��0��Ӧ�ڵ���ģʽ��1��Ӧ����ͨģʽ��2��Ӧ�ڸ���ģʽ
    char acDescription[32];  // ������Ϣ
} tSdkFrameSpeed;

//����ع⹦�ܷ�Χ����
typedef struct
{
    UINT  uiTargetMin;      //�Զ��ع�����Ŀ����Сֵ
    UINT  uiTargetMax;      //�Զ��ع�����Ŀ�����ֵ
    UINT  uiAnalogGainMin;  //ģ���������Сֵ����λΪfAnalogGainStep�ж���
    UINT  uiAnalogGainMax;  //ģ����������ֵ����λΪfAnalogGainStep�ж���
    float fAnalogGainStep;  //ģ������ÿ����1����Ӧ�����ӵķŴ��������磬uiAnalogGainMinһ��Ϊ16��fAnalogGainStepһ��Ϊ0.125����ô��С�Ŵ�������16*0.125 = 2��
    UINT  uiExposeTimeMin;  //�ֶ�ģʽ�£��ع�ʱ�����Сֵ����λ:�С�����CameraGetExposureLineTime���Ի��һ�ж�Ӧ��ʱ��(΢��),�Ӷ��õ���֡���ع�ʱ��
    UINT  uiExposeTimeMax;  //�ֶ�ģʽ�£��ع�ʱ������ֵ����λ:��
} tSdkExpose;

//����ģʽ����
typedef struct
{
  INT   iIndex;            //ģʽ������
  char  acDescription[32]; //��ģʽ��������Ϣ
} tSdkTrigger;

//����ְ���С����(��Ҫ��������������Ч)
typedef struct
{
    INT  iIndex;              //�ְ���С������
    char acDescription[32];   //��Ӧ��������Ϣ
    UINT iPackSize;
} tSdkPackLength;

//Ԥ���LUT������
typedef struct
{
    INT  iIndex;                //���
    char acDescription[32];     //������Ϣ
} tSdkPresetLut;

//AE�㷨����
typedef struct
{
    INT  iIndex;                //���
    char acDescription[32];     //������Ϣ
} tSdkAeAlgorithm;

//RAWתRGB�㷨����
typedef struct
{
    INT  iIndex;                //���
    char acDescription[32];     //������Ϣ
} tSdkBayerDecodeAlgorithm;


//֡��ͳ����Ϣ
typedef struct
{
  INT iTotal;           //��ǰ�ɼ�����֡������������֡��
    INT iCapture;       //��ǰ�ɼ�����Ч֡������
    INT iLost;          //��ǰ��֡������
} tSdkFrameStatistic;

//��������ͼ�����ݸ�ʽ
typedef struct
{
  INT     iIndex;             //��ʽ������
  char    acDescription[32];  //������Ϣ
  UINT    iMediaType;         //��Ӧ��ͼ���ʽ���룬��CAMERA_MEDIA_TYPE_BAYGR8���ڱ��ļ����ж��塣
} tSdkMediaType;

//٤����趨��Χ
typedef struct
{
  INT iMin;       //��Сֵ
  INT iMax;       //���ֵ
} tGammaRange;

//�Աȶȵ��趨��Χ
typedef struct
{
    INT iMin;   //��Сֵ
    INT iMax;   //���ֵ
} tContrastRange;

//RGB��ͨ������������趨��Χ
typedef struct
{
    INT iRGainMin;    //��ɫ�������Сֵ
    INT iRGainMax;    //��ɫ��������ֵ
    INT iGGainMin;    //��ɫ�������Сֵ
    INT iGGainMax;    //��ɫ��������ֵ
    INT iBGainMin;    //��ɫ�������Сֵ
    INT iBGainMax;    //��ɫ��������ֵ
} tRgbGainRange;

//���Ͷ��趨�ķ�Χ
typedef struct
{
    INT iMin;   //��Сֵ
    INT iMax;   //���ֵ
} tSaturationRange;

//�񻯵��趨��Χ
typedef struct
{
  INT iMin;   //��Сֵ
  INT iMax;   //���ֵ
} tSharpnessRange;

//ISPģ���ʹ����Ϣ
typedef struct
{
    BOOL bMonoSensor;       //��ʾ���ͺ�����Ƿ�Ϊ�ڰ����,����Ǻڰ����������ɫ��صĹ��ܶ��޷�����
    BOOL bWbOnce;           //��ʾ���ͺ�����Ƿ�֧���ֶ���ƽ�⹦��
    BOOL bAutoWb;           //��ʾ���ͺ�����Ƿ�֧���Զ���ƽ�⹦��
    BOOL bAutoExposure;     //��ʾ���ͺ�����Ƿ�֧���Զ��ع⹦��
    BOOL bManualExposure;   //��ʾ���ͺ�����Ƿ�֧���ֶ��ع⹦��
    BOOL bAntiFlick;        //��ʾ���ͺ�����Ƿ�֧�ֿ�Ƶ������
    BOOL bDeviceIsp;        //��ʾ���ͺ�����Ƿ�֧��Ӳ��ISP����
    BOOL bForceUseDeviceIsp;//bDeviceIsp��bForceUseDeviceIspͬʱΪTRUEʱ����ʾǿ��ֻ��Ӳ��ISP������ȡ����
    BOOL bZoomHD;           //���Ӳ���Ƿ�֧��ͼ���������(ֻ������С)��
} tSdkIspCapacity;

/* �������ϵ��豸������Ϣ����Щ��Ϣ�������ڶ�̬����UI */
typedef struct
{

  tSdkTrigger   *pTriggerDesc;          // ����ģʽ
  INT           iTriggerDesc;           // ����ģʽ�ĸ�������pTriggerDesc����Ĵ�С

  tSdkImageResolution   *pImageSizeDesc;// Ԥ��ֱ���ѡ��
  INT                   iImageSizeDesc; // Ԥ��ֱ��ʵĸ�������pImageSizeDesc����Ĵ�С

  tSdkColorTemperatureDes *pClrTempDesc;// Ԥ��ɫ��ģʽ�����ڰ�ƽ��
  INT                     iClrTempDesc;

  tSdkMediaType     *pMediaTypeDesc;    // ������ͼ���ʽ
  INT               iMediaTypdeDesc;    // ������ͼ���ʽ�������������pMediaTypeDesc����Ĵ�С��

  tSdkFrameSpeed    *pFrameSpeedDesc;   // �ɵ���֡�����ͣ���Ӧ��������ͨ ���� �ͳ��������ٶ�����
  INT               iFrameSpeedDesc;    // �ɵ���֡�����͵ĸ�������pFrameSpeedDesc����Ĵ�С��

  tSdkPackLength    *pPackLenDesc;      // ��������ȣ�һ�����������豸
  INT               iPackLenDesc;       // �ɹ�ѡ��Ĵ���ְ����ȵĸ�������pPackLenDesc����Ĵ�С��

  INT           iOutputIoCounts;        // �ɱ�����IO�ĸ���
  INT           iInputIoCounts;         // �ɱ������IO�ĸ���

  tSdkPresetLut  *pPresetLutDesc;       // ���Ԥ���LUT��
  INT            iPresetLut;            // ���Ԥ���LUT��ĸ�������pPresetLutDesc����Ĵ�С

  INT           iUserDataMaxLen;        // ָʾ����������ڱ����û�����������󳤶ȡ�Ϊ0��ʾ�ޡ�
  BOOL          bParamInDevice;         // ָʾ���豸�Ƿ�֧�ִ��豸�ж�д�����顣1Ϊ֧�֣�0��֧�֡�

  tSdkAeAlgorithm   *pAeAlmSwDesc;      // ����Զ��ع��㷨����
  int                iAeAlmSwDesc;      // ����Զ��ع��㷨����

  tSdkAeAlgorithm    *pAeAlmHdDesc;     // Ӳ���Զ��ع��㷨������ΪNULL��ʾ��֧��Ӳ���Զ��ع�
  int                iAeAlmHdDesc;      // Ӳ���Զ��ع��㷨������Ϊ0��ʾ��֧��Ӳ���Զ��ع�

  tSdkBayerDecodeAlgorithm   *pBayerDecAlmSwDesc; // ���Bayerת��ΪRGB���ݵ��㷨����
  int                        iBayerDecAlmSwDesc;  // ���Bayerת��ΪRGB���ݵ��㷨����

  tSdkBayerDecodeAlgorithm   *pBayerDecAlmHdDesc; // Ӳ��Bayerת��ΪRGB���ݵ��㷨������ΪNULL��ʾ��֧��
  int                        iBayerDecAlmHdDesc;  // Ӳ��Bayerת��ΪRGB���ݵ��㷨������Ϊ0��ʾ��֧��

  /* ͼ������ĵ��ڷ�Χ����,���ڶ�̬����UI*/
  tSdkExpose            sExposeDesc;      // �ع�ķ�Χֵ
  tSdkResolutionRange   sResolutionRange; // �ֱ��ʷ�Χ����
  tRgbGainRange         sRgbGainRange;    // ͼ���������淶Χ����
  tSaturationRange      sSaturationRange; // ���Ͷȷ�Χ����
  tGammaRange           sGammaRange;      // ٤��Χ����
  tContrastRange        sContrastRange;   // �Աȶȷ�Χ����
  tSharpnessRange       sSharpnessRange;  // �񻯷�Χ����
  tSdkIspCapacity       sIspCapacity;     // ISP��������


} tSdkCameraCapbility;


//ͼ��֡ͷ��Ϣ
typedef struct
{
  UINT    uiMediaType;    // ͼ���ʽ,Image Format
  UINT    uBytes;         // ͼ�������ֽ���,Total bytes
  INT     iWidth;         // ͼ��Ŀ�ȣ�����ͼ�������󣬸ñ������ܱ���̬�޸ģ���ָʾ������ͼ��ߴ�
  INT     iHeight;        // ͼ��ĸ߶ȣ�����ͼ�������󣬸ñ������ܱ���̬�޸ģ���ָʾ������ͼ��ߴ�
  INT     iWidthZoomSw;   // ������ŵĿ��,����Ҫ��������ü���ͼ�񣬴˱�������Ϊ0.
  INT     iHeightZoomSw;  // ������ŵĸ߶�,����Ҫ��������ü���ͼ�񣬴˱�������Ϊ0.
  BOOL    bIsTrigger;     // ָʾ�Ƿ�Ϊ����֡ is trigger
  UINT    uiTimeStamp;    // ��֡�Ĳɼ�ʱ�䣬��λ0.1����
  UINT    uiExpTime;      // ��ǰͼ����ع�ֵ����λΪ΢��us
  float   fAnalogGain;    // ��ǰͼ���ģ�����汶��
  INT     iGamma;         // ��֡ͼ���٤���趨ֵ������LUTģʽΪ��̬��������ʱ��Ч������ģʽ��Ϊ-1
  INT     iContrast;      // ��֡ͼ��ĶԱȶ��趨ֵ������LUTģʽΪ��̬��������ʱ��Ч������ģʽ��Ϊ-1
  INT     iSaturation;    // ��֡ͼ��ı��Ͷ��趨ֵ�����ںڰ���������壬Ϊ0
  float   fRgain;         // ��֡ͼ����ĺ�ɫ�������汶�������ںڰ���������壬Ϊ1
  float   fGgain;         // ��֡ͼ�������ɫ�������汶�������ںڰ���������壬Ϊ1
  float   fBgain;         // ��֡ͼ�������ɫ�������汶�������ںڰ���������壬Ϊ1
}tSdkFrameHead;

//ͼ��֡����
typedef struct sCameraFrame
{
  tSdkFrameHead   head;     //֡ͷ
  BYTE *          pBuffer;  //������
}tSdkFrame;

//ͼ�񲶻�Ļص���������
typedef void (*CAMERA_SNAP_PROC)(CameraHandle hCamera, BYTE *pFrameBuffer, tSdkFrameHead* pFrameHead,PVOID pContext);

//SDK���ɵ��������ҳ�����Ϣ�ص���������
typedef void (*CAMERA_PAGE_MSG_PROC)(CameraHandle hCamera,UINT MSG,UINT uParam,PVOID pContext);

/// @ingroup API_RECONNECT
/// \~chinese �������״̬�ص�
/// \param [in] hCamera ������
/// \param [in] MSG ��Ϣ��0: ������ӶϿ�    1: ������ӻָ�
/// \param [in] uParam ������Ϣ
/// \param [in] pContext �û�����
/// \return ��
/// \note USB���uParamȡֵ��
/// \note 		δ����
/// \note �������uParamȡֵ��
/// \note		��MSG=0ʱ��δ����
/// \note		��MSG=1ʱ��
/// \note			0���ϴε���ԭ������ͨѶʧ��
/// \note			1���ϴε���ԭ���������
/// \~english Camera connection status callback
/// \param [in] hCamera Camera handle
/// \param [in] MSG message, 0: Camera disconnected 1: Camera connection restored
/// \param [in] uParam Additional Information
/// \param [in] pContext user data
/// \return None
/// \note USB MD_Camera_H uParam value:
/// \note       Undefined
/// \note network MD_Camera_H uParam value:
/// \note       When MSG=0: Undefined
/// \note       When MSG=1:
/// \note           0: The last dropped reason, network communication failed
/// \note           1: The last dropped reason, the MD_Camera_H lost power
typedef void (*CAMERA_CONNECTION_STATUS_CALLBACK)(CameraHandle hCamera,UINT MSG,UINT uParam,PVOID pContext);


//----------------------------IMAGE FORMAT DEFINE------------------------------------
//----------------------------ͼ���ʽ����-------------------------------------------
#define CAMERA_MEDIA_TYPE_MONO                           0x01000000
#define CAMERA_MEDIA_TYPE_RGB                            0x02000000
#define CAMERA_MEDIA_TYPE_COLOR                          0x02000000
#define CAMERA_MEDIA_TYPE_CUSTOM                         0x80000000
#define CAMERA_MEDIA_TYPE_COLOR_MASK                     0xFF000000
#define CAMERA_MEDIA_TYPE_OCCUPY1BIT                     0x00010000
#define CAMERA_MEDIA_TYPE_OCCUPY2BIT                     0x00020000
#define CAMERA_MEDIA_TYPE_OCCUPY4BIT                     0x00040000
#define CAMERA_MEDIA_TYPE_OCCUPY8BIT                     0x00080000
#define CAMERA_MEDIA_TYPE_OCCUPY10BIT                    0x000A0000
#define CAMERA_MEDIA_TYPE_OCCUPY12BIT                    0x000C0000
#define CAMERA_MEDIA_TYPE_OCCUPY16BIT                    0x00100000
#define CAMERA_MEDIA_TYPE_OCCUPY24BIT                    0x00180000
#define CAMERA_MEDIA_TYPE_OCCUPY32BIT                    0x00200000
#define CAMERA_MEDIA_TYPE_OCCUPY36BIT                    0x00240000
#define CAMERA_MEDIA_TYPE_OCCUPY48BIT                    0x00300000
#define CAMERA_MEDIA_TYPE_OCCUPY64BIT					 0x00400000

#define CAMERA_MEDIA_TYPE_EFFECTIVE_PIXEL_SIZE_MASK      0x00FF0000
#define CAMERA_MEDIA_TYPE_EFFECTIVE_PIXEL_SIZE_SHIFT     16

#define CAMERA_MEDIA_TYPE_PIXEL_SIZE(type)                 (((type) & CAMERA_MEDIA_TYPE_EFFECTIVE_PIXEL_SIZE_MASK)>>CAMERA_MEDIA_TYPE_EFFECTIVE_PIXEL_SIZE_SHIFT)

#define CAMERA_MEDIA_TYPE_ID_MASK                        0x0000FFFF
#define CAMERA_MEDIA_TYPE_COUNT                          0x46

/*mono*/
#define CAMERA_MEDIA_TYPE_MONO1P             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY1BIT | 0x0037)
#define CAMERA_MEDIA_TYPE_MONO2P             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY2BIT | 0x0038)
#define CAMERA_MEDIA_TYPE_MONO4P             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY4BIT | 0x0039)
#define CAMERA_MEDIA_TYPE_MONO8              (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x0001)
#define CAMERA_MEDIA_TYPE_MONO8S             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x0002)
#define CAMERA_MEDIA_TYPE_MONO10             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0003)
#define CAMERA_MEDIA_TYPE_MONO10_PACKED      (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0004)
#define CAMERA_MEDIA_TYPE_MONO12             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0005)
#define CAMERA_MEDIA_TYPE_MONO12_PACKED      (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0006)
#define CAMERA_MEDIA_TYPE_MONO14             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0025)
#define CAMERA_MEDIA_TYPE_MONO16             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0007)

/*Bayer */
#define CAMERA_MEDIA_TYPE_BAYGR8             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x0008)
#define CAMERA_MEDIA_TYPE_BAYRG8             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x0009)
#define CAMERA_MEDIA_TYPE_BAYGB8             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x000A)
#define CAMERA_MEDIA_TYPE_BAYBG8             (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x000B)

#define CAMERA_MEDIA_TYPE_BAYGR10_MIPI       (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY10BIT | 0x0026)
#define CAMERA_MEDIA_TYPE_BAYRG10_MIPI       (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY10BIT | 0x0027)
#define CAMERA_MEDIA_TYPE_BAYGB10_MIPI       (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY10BIT | 0x0028)
#define CAMERA_MEDIA_TYPE_BAYBG10_MIPI       (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY10BIT | 0x0029)


#define CAMERA_MEDIA_TYPE_BAYGR10            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x000C)
#define CAMERA_MEDIA_TYPE_BAYRG10            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x000D)
#define CAMERA_MEDIA_TYPE_BAYGB10            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x000E)
#define CAMERA_MEDIA_TYPE_BAYBG10            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x000F)

#define CAMERA_MEDIA_TYPE_BAYGR12            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0010)
#define CAMERA_MEDIA_TYPE_BAYRG12            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0011)
#define CAMERA_MEDIA_TYPE_BAYGB12            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0012)
#define CAMERA_MEDIA_TYPE_BAYBG12            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0013)


#define CAMERA_MEDIA_TYPE_BAYGR10_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0026)
#define CAMERA_MEDIA_TYPE_BAYRG10_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0027)
#define CAMERA_MEDIA_TYPE_BAYGB10_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0028)
#define CAMERA_MEDIA_TYPE_BAYBG10_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0029)

#define CAMERA_MEDIA_TYPE_BAYGR12_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x002A)
#define CAMERA_MEDIA_TYPE_BAYRG12_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x002B)
#define CAMERA_MEDIA_TYPE_BAYGB12_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x002C)
#define CAMERA_MEDIA_TYPE_BAYBG12_PACKED     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x002D)

#define CAMERA_MEDIA_TYPE_BAYGR16            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x002E)
#define CAMERA_MEDIA_TYPE_BAYRG16            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x002F)
#define CAMERA_MEDIA_TYPE_BAYGB16            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0030)
#define CAMERA_MEDIA_TYPE_BAYBG16            (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0031)

/*RGB */
#define CAMERA_MEDIA_TYPE_RGB8               (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY24BIT | 0x0014)
#define CAMERA_MEDIA_TYPE_BGR8               (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY24BIT | 0x0015)
#define CAMERA_MEDIA_TYPE_RGBA8              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY32BIT | 0x0016)
#define CAMERA_MEDIA_TYPE_BGRA8              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY32BIT | 0x0017)
#define CAMERA_MEDIA_TYPE_RGB10              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x0018)
#define CAMERA_MEDIA_TYPE_BGR10              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x0019)
#define CAMERA_MEDIA_TYPE_RGB12              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x001A)
#define CAMERA_MEDIA_TYPE_BGR12              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x001B)
#define CAMERA_MEDIA_TYPE_RGB16              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x0033)
#define CAMERA_MEDIA_TYPE_BGR16              (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x004B)
#define CAMERA_MEDIA_TYPE_RGBA16             (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY64BIT | 0x0064)
#define CAMERA_MEDIA_TYPE_BGRA16             (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY64BIT | 0x0051)
#define CAMERA_MEDIA_TYPE_RGB10V1_PACKED     (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY32BIT | 0x001C)
#define CAMERA_MEDIA_TYPE_RGB10P32           (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY32BIT | 0x001D)
#define CAMERA_MEDIA_TYPE_RGB12V1_PACKED     (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY36BIT | 0X0034)
#define CAMERA_MEDIA_TYPE_RGB565P            (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0035)
#define CAMERA_MEDIA_TYPE_BGR565P            (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0X0036)

/*YUV and YCbCr*/
#define CAMERA_MEDIA_TYPE_YUV411_8_UYYVYY    (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x001E)
#define CAMERA_MEDIA_TYPE_YUV422_8_UYVY      (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x001F)
#define CAMERA_MEDIA_TYPE_YUV422_8           (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0032)
#define CAMERA_MEDIA_TYPE_YUV8_UYV           (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY24BIT | 0x0020)
#define CAMERA_MEDIA_TYPE_YCBCR8_CBYCR       (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY24BIT | 0x003A)
//CAMERA_MEDIA_TYPE_YCBCR422_8 : YYYYCbCrCbCr
#define CAMERA_MEDIA_TYPE_YCBCR422_8             (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x003B)
#define CAMERA_MEDIA_TYPE_YCBCR422_8_CBYCRY      (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0043)
#define CAMERA_MEDIA_TYPE_YCBCR411_8_CBYYCRYY    (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x003C)
#define CAMERA_MEDIA_TYPE_YCBCR601_8_CBYCR       (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY24BIT | 0x003D)
#define CAMERA_MEDIA_TYPE_YCBCR601_422_8         (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x003E)
#define CAMERA_MEDIA_TYPE_YCBCR601_422_8_CBYCRY  (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0044)
#define CAMERA_MEDIA_TYPE_YCBCR601_411_8_CBYYCRYY    (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x003F)
#define CAMERA_MEDIA_TYPE_YCBCR709_8_CBYCR           (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY24BIT | 0x0040)
#define CAMERA_MEDIA_TYPE_YCBCR709_422_8             (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0041)
#define CAMERA_MEDIA_TYPE_YCBCR709_422_8_CBYCRY      (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY16BIT | 0x0045)
#define CAMERA_MEDIA_TYPE_YCBCR709_411_8_CBYYCRYY    (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0042)

/*RGB Planar */
#define CAMERA_MEDIA_TYPE_RGB8_PLANAR        (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY24BIT | 0x0021)
#define CAMERA_MEDIA_TYPE_RGB10_PLANAR       (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x0022)
#define CAMERA_MEDIA_TYPE_RGB12_PLANAR       (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x0023)
#define CAMERA_MEDIA_TYPE_RGB16_PLANAR       (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY48BIT | 0x0024)



/*MindVision 12bit packed bayer*/
#define CAMERA_MEDIA_TYPE_BAYGR12_PACKED_MV     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0060)
#define CAMERA_MEDIA_TYPE_BAYRG12_PACKED_MV     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0061)
#define CAMERA_MEDIA_TYPE_BAYGB12_PACKED_MV     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0062)
#define CAMERA_MEDIA_TYPE_BAYBG12_PACKED_MV     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0063)

/*MindVision 12bit packed monochome*/
#define CAMERA_MEDIA_TYPE_MONO12_PACKED_MV     (CAMERA_MEDIA_TYPE_MONO | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0064)
#define CAMERA_MEDIA_TYPE_YUV420P_MV			     (CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0065)

/*planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte V and the following byte U)*/
#define CAMERA_MEDIA_TYPE_YUV_NV21_MV			(CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY12BIT | 0x0066)

/* H264 H265 */
#define CAMERA_MEDIA_TYPE_H264_MV				(CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x0067)
#define CAMERA_MEDIA_TYPE_H265_MV				(CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x0068)

/* JPEG */
#define CAMERA_MEDIA_TYPE_JPEG_MV				(CAMERA_MEDIA_TYPE_COLOR | CAMERA_MEDIA_TYPE_OCCUPY8BIT | 0x0069)

#endif
