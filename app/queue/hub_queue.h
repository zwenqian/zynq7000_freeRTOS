#ifndef _QUEUE_H_
#define _QUEUE_H_

/********************************** 头文件 *****************************************/
//#include "system_init.h"
#include "xil_types.h"
// 术语： ARM指CPU0        DSP指CPU1      命名以方便辨认

/********************************** 宏定义 *****************************************/
//定制共享DDR内存地址
#define SHARED_DDR_MEMORY_BASE   0x18000000

#define DATA_EMPTY           0xAAAA
#define DATA_VALID           0x5555

#define FRAME_TYPE_INVALID      0x0
//#define FRAME_TYPE_MPEG_DVB     0x1
//#define FRAME_TYPE_MPEG_QP1_2   0x2
//#define FRAME_TYPE_MPEG_QP2_3   0x3
//#define FRAME_TYPE_MPEG_QP3_4   0x4

//#define FRAME_MPEG_TYPE_CNT     4

//#define FRAME_TYPE_SEC_DVB      0x5
//#define FRAME_TYPE_SEC_QP1_2    0x6
//#define FRAME_TYPE_SEC_QP2_3    0x7
//#define FRAME_TYPE_SEC_QP3_4    0x8

//#define FRAME_ALL_TYPE_CNT      8

#define FRAME_MPEG_TYPE_COUNT 28    //<=128；
#define MPEG_TYPE_MAX_COUNT        128

#define RETURN_CMD_TYPE_FPGA    0x0001
#define RETURN_CMD_TYPE_936x    0x0002
#define RETURN_CMD_TYPE_CPU1    0x0003

#define ARM2DSP_BUF_SIZE     0x00700000   // 单位：字   共7Mbyte

#define DSP2FPGA_BUF1_SIZE   0x080000   // 单位：字   共0.5Mbyte
#define DSP2FPGA_BUF2_SIZE   0x200000   // 单位：字   共2Mbyte
#define DSP2FPGA_BUF3_SIZE   0x300000   // 单位：字   共3Mbyte
#define DSP2FPGA_BUF4_SIZE   0x380000   // 单位：字   共3.5Mbyte

#define FPGA2DSP_BUF_SIZE    0x00700000   // 单位：字   共7Mbyte
#define DSP2ARM_BUF_SIZE     0x00700000   // 单位：字   共7Mbyte

#define MAX_DATA_UNIT_LEN    2000       // 单位：字节  最大数据单元长度

/********************************** 数据结构 *****************************************/
typedef struct
{
    // CPU0  --->   CPU1 空间
    volatile unsigned char Arm2DspFrameData[ARM2DSP_BUF_SIZE - 8];
    volatile unsigned int Arm2DspWritePosition;   // 4字节读指针
    volatile unsigned int Arm2DspReadPosition;    // 4字节写指针

    // 发MPEG缓冲区1
    unsigned char DSP2FPGA1FrameData[DSP2FPGA_BUF1_SIZE - 8];
    unsigned int DSP2FPGA1WritePosition;   // 4字节读指针
    unsigned int DSP2FPGA1ReadPosition;    // 4字节写指针

    // 发MPEG缓冲区2
    unsigned char DSP2FPGA2FrameData[DSP2FPGA_BUF2_SIZE - 8];
    unsigned int DSP2FPGA2WritePosition;   // 4字节读指针
    unsigned int DSP2FPGA2ReadPosition;    // 4字节写指针

    // 发MPEG缓冲区3
    unsigned char DSP2FPGA3FrameData[DSP2FPGA_BUF3_SIZE - 8];
    unsigned int DSP2FPGA3WritePosition;   // 4字节读指针
    unsigned int DSP2FPGA3ReadPosition;    // 4字节写指针

    // 发MPEG缓冲区4
    unsigned char DSP2FPGA4FrameData[DSP2FPGA_BUF4_SIZE - 8];
    unsigned int DSP2FPGA4WritePosition;   // 4字节读指针
    unsigned int DSP2FPGA4ReadPosition;    // 4字节写指针

    // 收MPEG缓冲区
    unsigned char FPGA2DspFrameData[FPGA2DSP_BUF_SIZE - 8];
    unsigned int FPGA2DspWritePosition;   // 4字节读指针
    unsigned int FPGA2DspReadPosition;    // 4字节写指针

    // CPU1  --->   CPU0 空间
    volatile unsigned char Dsp2ArmFrameData[DSP2ARM_BUF_SIZE - 8];
    volatile unsigned int Dsp2ArmWritePosition;   // 4字节读指针
    volatile unsigned int Dsp2ArmReadPosition;    // 4字节写指针

} DDR_ShareRam;

//startFlag : 数据开始读写标记；
//type ： (1) 当上行为DVB波形时，[8..16]位（即高8位）表示ACM中的通道数，[4..7]位表示数据优先级，
//           第0位表示是MPEG级数据（0）或是段级数据（1），第1位表示是DVB波形（0）还是TDMA波形（1），[2:3]位保留；
//       (2) 当上行为TDMA自研波形时，[8..16]位（即高8位）表示编码方式（分MPEG级和段级），[4..7]位表示数据优先级，
//           第0位表示是MPEG级数据（0）或是段级数据（1），第1位表示是DVB波形（0）还是TDMA波形（1），[2:3]位保留；
//       (3) 下行收方就表示解码方式类型；
//len : 数据长度；
//Data : 数据内容；
typedef struct
{
    volatile unsigned short startFlag;
    volatile unsigned short type;
    volatile unsigned short len;
    volatile unsigned char  Data[MAX_DATA_UNIT_LEN];
} MIN_TRANS_UNIT;


/******************************** 外部全局变量 ***************************************/
typedef volatile MIN_TRANS_UNIT *pMIN_TRANS_UNIT;

extern volatile DDR_ShareRam *pDDRShareRam;

/******************************** 外部函数声明 ***************************************/
void QueueInit(void);

u8 isCPUTransData(void);      // CPU0->CPU1 数据到达
u8 isDSP2FPGA1HaveData(void); // FPGA1 BUF 中有数据
u8 isDSP2FPGA2HaveData(void); // FPGA2 BUF 中有数据
u8 isDSP2FPGA3HaveData(void); // FPGA3 BUF 中有数据
u8 isDSP2FPGA4HaveData(void); // FPGA4 BUF 中有数据
u8 isFPGA2DSPHaveData(void);  // FPGA发来的有数据
u8 isDSP2ARMHaveData(void);   // CPU1有数据给CPU0

u8 writeData2DSP(unsigned short type, unsigned short len, unsigned short *Data);		 	// 写数据到DSP
u8 writeData2FPGA_BUF1(unsigned short type, unsigned short len, unsigned short *Data);  // 写数据到FPGA1区(发MPEG缓冲区)
u8 writeData2FPGA_BUF2(unsigned short type, unsigned short len, unsigned short *Data);  // 写数据到FPGA2区(发MPEG缓冲区)
u8 writeData2FPGA_BUF3(unsigned short type, unsigned short len, unsigned short *Data);  // 写数据到FPGA3区(发MPEG缓冲区)
u8 writeData2FPGA_BUF4(unsigned short type, unsigned short len, unsigned short *Data);  // 写数据到FPGA4区(发MPEG缓冲区)

u8 writeData2FPGAMPEGReportBUF(unsigned short type, unsigned short len, unsigned short *Data); // fpga来的数据放到这个位置缓冲
u8 writeData2ARM(unsigned short type, unsigned short len, unsigned short *Data); // 给CPU0送消息


u8 readFromARM(void); // 从CPU0获得消息
u8 readFromFPGAPriBuf(void); // 从待发送MPEG队列获得消息
u8 readFromFPGAMPEGReportBUF(void); // 从FPGA给CPU1的MPEG缓存中获得消息

u8 IsOCMDataEMPTY(void);
u8 WriteYWDataToOCM(void);
u8 WriteCmdToOCM(unsigned short type, unsigned char * cmd, int len);

void Read_MAC_AXI_Data(void);

u8 lwip_readFromARM(void);
u8 lwip_readFromFPGAPriBuf(void);
u8 lwip_readFromFPGAMPEGReportBUF(void);
u16 lwip_readFromDSP2ARM(unsigned char *Data);

u8 lwip_CopyData2FPGA_BUF2( char *Data, int len );

#endif
