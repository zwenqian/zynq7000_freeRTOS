#ifndef _QUEUE_H_
#define _QUEUE_H_

/********************************** ͷ�ļ� *****************************************/
//#include "system_init.h"
#include "xil_types.h"
// ��� ARMָCPU0        DSPָCPU1      �����Է������

/********************************** �궨�� *****************************************/
//���ƹ���DDR�ڴ��ַ
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

#define FRAME_MPEG_TYPE_COUNT 28    //<=128��
#define MPEG_TYPE_MAX_COUNT        128

#define RETURN_CMD_TYPE_FPGA    0x0001
#define RETURN_CMD_TYPE_936x    0x0002
#define RETURN_CMD_TYPE_CPU1    0x0003

#define ARM2DSP_BUF_SIZE     0x00700000   // ��λ����   ��7Mbyte

#define DSP2FPGA_BUF1_SIZE   0x080000   // ��λ����   ��0.5Mbyte
#define DSP2FPGA_BUF2_SIZE   0x200000   // ��λ����   ��2Mbyte
#define DSP2FPGA_BUF3_SIZE   0x300000   // ��λ����   ��3Mbyte
#define DSP2FPGA_BUF4_SIZE   0x380000   // ��λ����   ��3.5Mbyte

#define FPGA2DSP_BUF_SIZE    0x00700000   // ��λ����   ��7Mbyte
#define DSP2ARM_BUF_SIZE     0x00700000   // ��λ����   ��7Mbyte

#define MAX_DATA_UNIT_LEN    2000       // ��λ���ֽ�  ������ݵ�Ԫ����

/********************************** ���ݽṹ *****************************************/
typedef struct
{
    // CPU0  --->   CPU1 �ռ�
    volatile unsigned char Arm2DspFrameData[ARM2DSP_BUF_SIZE - 8];
    volatile unsigned int Arm2DspWritePosition;   // 4�ֽڶ�ָ��
    volatile unsigned int Arm2DspReadPosition;    // 4�ֽ�дָ��

    // ��MPEG������1
    unsigned char DSP2FPGA1FrameData[DSP2FPGA_BUF1_SIZE - 8];
    unsigned int DSP2FPGA1WritePosition;   // 4�ֽڶ�ָ��
    unsigned int DSP2FPGA1ReadPosition;    // 4�ֽ�дָ��

    // ��MPEG������2
    unsigned char DSP2FPGA2FrameData[DSP2FPGA_BUF2_SIZE - 8];
    unsigned int DSP2FPGA2WritePosition;   // 4�ֽڶ�ָ��
    unsigned int DSP2FPGA2ReadPosition;    // 4�ֽ�дָ��

    // ��MPEG������3
    unsigned char DSP2FPGA3FrameData[DSP2FPGA_BUF3_SIZE - 8];
    unsigned int DSP2FPGA3WritePosition;   // 4�ֽڶ�ָ��
    unsigned int DSP2FPGA3ReadPosition;    // 4�ֽ�дָ��

    // ��MPEG������4
    unsigned char DSP2FPGA4FrameData[DSP2FPGA_BUF4_SIZE - 8];
    unsigned int DSP2FPGA4WritePosition;   // 4�ֽڶ�ָ��
    unsigned int DSP2FPGA4ReadPosition;    // 4�ֽ�дָ��

    // ��MPEG������
    unsigned char FPGA2DspFrameData[FPGA2DSP_BUF_SIZE - 8];
    unsigned int FPGA2DspWritePosition;   // 4�ֽڶ�ָ��
    unsigned int FPGA2DspReadPosition;    // 4�ֽ�дָ��

    // CPU1  --->   CPU0 �ռ�
    volatile unsigned char Dsp2ArmFrameData[DSP2ARM_BUF_SIZE - 8];
    volatile unsigned int Dsp2ArmWritePosition;   // 4�ֽڶ�ָ��
    volatile unsigned int Dsp2ArmReadPosition;    // 4�ֽ�дָ��

} DDR_ShareRam;

//startFlag : ���ݿ�ʼ��д��ǣ�
//type �� (1) ������ΪDVB����ʱ��[8..16]λ������8λ����ʾACM�е�ͨ������[4..7]λ��ʾ�������ȼ���
//           ��0λ��ʾ��MPEG�����ݣ�0�����Ƕμ����ݣ�1������1λ��ʾ��DVB���Σ�0������TDMA���Σ�1����[2:3]λ������
//       (2) ������ΪTDMA���в���ʱ��[8..16]λ������8λ����ʾ���뷽ʽ����MPEG���Ͷμ�����[4..7]λ��ʾ�������ȼ���
//           ��0λ��ʾ��MPEG�����ݣ�0�����Ƕμ����ݣ�1������1λ��ʾ��DVB���Σ�0������TDMA���Σ�1����[2:3]λ������
//       (3) �����շ��ͱ�ʾ���뷽ʽ���ͣ�
//len : ���ݳ��ȣ�
//Data : �������ݣ�
typedef struct
{
    volatile unsigned short startFlag;
    volatile unsigned short type;
    volatile unsigned short len;
    volatile unsigned char  Data[MAX_DATA_UNIT_LEN];
} MIN_TRANS_UNIT;


/******************************** �ⲿȫ�ֱ��� ***************************************/
typedef volatile MIN_TRANS_UNIT *pMIN_TRANS_UNIT;

extern volatile DDR_ShareRam *pDDRShareRam;

/******************************** �ⲿ�������� ***************************************/
void QueueInit(void);

u8 isCPUTransData(void);      // CPU0->CPU1 ���ݵ���
u8 isDSP2FPGA1HaveData(void); // FPGA1 BUF ��������
u8 isDSP2FPGA2HaveData(void); // FPGA2 BUF ��������
u8 isDSP2FPGA3HaveData(void); // FPGA3 BUF ��������
u8 isDSP2FPGA4HaveData(void); // FPGA4 BUF ��������
u8 isFPGA2DSPHaveData(void);  // FPGA������������
u8 isDSP2ARMHaveData(void);   // CPU1�����ݸ�CPU0

u8 writeData2DSP(unsigned short type, unsigned short len, unsigned short *Data);		 	// д���ݵ�DSP
u8 writeData2FPGA_BUF1(unsigned short type, unsigned short len, unsigned short *Data);  // д���ݵ�FPGA1��(��MPEG������)
u8 writeData2FPGA_BUF2(unsigned short type, unsigned short len, unsigned short *Data);  // д���ݵ�FPGA2��(��MPEG������)
u8 writeData2FPGA_BUF3(unsigned short type, unsigned short len, unsigned short *Data);  // д���ݵ�FPGA3��(��MPEG������)
u8 writeData2FPGA_BUF4(unsigned short type, unsigned short len, unsigned short *Data);  // д���ݵ�FPGA4��(��MPEG������)

u8 writeData2FPGAMPEGReportBUF(unsigned short type, unsigned short len, unsigned short *Data); // fpga�������ݷŵ����λ�û���
u8 writeData2ARM(unsigned short type, unsigned short len, unsigned short *Data); // ��CPU0����Ϣ


u8 readFromARM(void); // ��CPU0�����Ϣ
u8 readFromFPGAPriBuf(void); // �Ӵ�����MPEG���л����Ϣ
u8 readFromFPGAMPEGReportBUF(void); // ��FPGA��CPU1��MPEG�����л����Ϣ

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
