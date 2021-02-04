//#include "system_init.h"
#include "string.h"
#include "hub_queue.h"
#include "xil_cache.h"
#include "dmaUser.h"
#include "xil_printf.h"
//#include "../Modu/pl_interface.h"

int  FPGA_REPORT_MPEGNum1 = 0;
int  FPGA_REPORT_MPEGNum2 = 0;
signed char g_DSP2FPGA_QUEUE_EMPTY = 0;
extern int ocmIntr_wait_semphore;
extern int ocmIntr_avoid_reIn;
extern int cpu0_data_semphore1;
extern int cpu0_ocm_yw_missIntr;
//volatile char *_DDR = (char *)SHARED_DDR_MEMORY_BASE;

extern u16 YW_PID;
extern u16 CardType;
extern u64 g_UserSite_MY_MAC;
extern unsigned short g_UserSite_setTBTPflag;
//extern _CACHE_BUF_ CacheBuf[4];
extern u8  DSP2FPAG_BUF_READY[4];
extern int console_process_ocm_cmd(char * received_cmd);

#define OCM_BASE_ADDR                    0xFFFE0000
#define CPU0_WRITE_CMD_OFFSET            0x00000
#define CPU0_WRITE_DATA_OFFSET           0x00400
#define CPU0_READ_CMD_OFFSET             0x10000
#define CPU0_READ_DATA_OFFSET            0x10400
#define WRITE_DATA_MAX_LEN               0x0FC00

#define AXI_INTERFACE_OPTIMIZE_TX
#define AXI_INTERFACE_OPTIMIZE_RX

//MPEG����, ������, ֡����
unsigned short FRAME_TYPE_TABLE[FRAME_MPEG_TYPE_COUNT][3] = {
//		{0x00, 0x00 + MPEG_TYPE_MAX_COUNT, 188},//��������Ч����;
		{0x01, 0x01 + MPEG_TYPE_MAX_COUNT, 80}, //BPSK-1/3;
		{0x02, 0x02 + MPEG_TYPE_MAX_COUNT, 96}, //BPSK-2/5;
		{0x03, 0x03 + MPEG_TYPE_MAX_COUNT, 120}, //BPSK-1/2;
		{0x04, 0x04 + MPEG_TYPE_MAX_COUNT, 160}, //BPSK-2/3;
		{0x05, 0x05 + MPEG_TYPE_MAX_COUNT, 180}, //BPSK-3/4;
		{0x06, 0x06 + MPEG_TYPE_MAX_COUNT, 192}, //BPSK-4/5;
		{0x07, 0x07 + MPEG_TYPE_MAX_COUNT, 200}, //BPSK-5/6;
		{0x08, 0x08 + MPEG_TYPE_MAX_COUNT, 160}, //QPSK-1/3;
		{0x09, 0x09 + MPEG_TYPE_MAX_COUNT, 192}, //QPSK-2/5;
		{0x0A, 0x0A + MPEG_TYPE_MAX_COUNT, 240}, //QPSK-1/2;
		{0x0B, 0x0B + MPEG_TYPE_MAX_COUNT, 320}, //QPSK-2/3;
		{0x0C, 0x0C + MPEG_TYPE_MAX_COUNT, 360}, //QPSK-3/4;
		{0x0D, 0x0D + MPEG_TYPE_MAX_COUNT, 384}, //QPSK-4/5;
		{0x0E, 0x0E + MPEG_TYPE_MAX_COUNT, 400}, //QPSK-5/6;
		{0x0F, 0x0F + MPEG_TYPE_MAX_COUNT, 432}, //QPSK-9/10;
		{0x10, 0x10 + MPEG_TYPE_MAX_COUNT, 240}, //8PSK-1/3;
		{0x11, 0x11 + MPEG_TYPE_MAX_COUNT, 288}, //8PSK-2/5;
		{0x12, 0x12 + MPEG_TYPE_MAX_COUNT, 360}, //8PSK-1/2;
		{0x13, 0x13 + MPEG_TYPE_MAX_COUNT, 480}, //8PSK-2/3;
		{0x14, 0x14 + MPEG_TYPE_MAX_COUNT, 288}, //8PSK-4/5;
		{0x15, 0x15 + MPEG_TYPE_MAX_COUNT, 300}, //8PSK-5/6;
		{0x16, 0x16 + MPEG_TYPE_MAX_COUNT, 272}, //8PSK-3/4;
		{0x18, 0x18 + MPEG_TYPE_MAX_COUNT, 320}, //16PSK-2/3;
		{0x19, 0x19 + MPEG_TYPE_MAX_COUNT, 480}, //16PSK-1/2;
		{0x1A, 0x1A + MPEG_TYPE_MAX_COUNT, 320}, //16PSK-1/3;
		{0x1B, 0x1B + MPEG_TYPE_MAX_COUNT, 384}, //16PSK-2/5;
		{0x20, 0x20 + MPEG_TYPE_MAX_COUNT, 400}, //32PSK-1/3;
		{0x7F, 0x7F + MPEG_TYPE_MAX_COUNT, 188}  //DVB;
};

//�������ڴ��ַ��ֵ���ṹ��
volatile DDR_ShareRam *pDDRShareRam = (DDR_ShareRam *)SHARED_DDR_MEMORY_BASE;

#define ARM2DSP_READ_ADDR    &pDDRShareRam->Arm2DspFrameData[pDDRShareRam->Arm2DspReadPosition]		//��ָ��
#define ARM2DSP_WRITE_ADDR   &pDDRShareRam->Arm2DspFrameData[pDDRShareRam->Arm2DspWritePosition]	//дָ��
#define ARM2DSP_ARESTLIMIT   (ARM2DSP_BUF_SIZE  - pDDRShareRam->Arm2DspWritePosition)  			//�������β�Ŀռ��С
#define ARM2DSP_LAST_AREA	 (pDDRShareRam->Arm2DspReadPosition == pDDRShareRam->Arm2DspWritePosition)?(ARM2DSP_BUF_SIZE):(ARM2DSP_BUF_SIZE + pDDRShareRam->Arm2DspReadPosition - pDDRShareRam->Arm2DspWritePosition)%(ARM2DSP_BUF_SIZE) //ʣ���д�ռ� ������������


#define DSP2FPGA1_READ_ADDR  &pDDRShareRam->DSP2FPGA1FrameData[pDDRShareRam->DSP2FPGA1ReadPosition]
#define DSP2FPGA1_WRITE_ADDR &pDDRShareRam->DSP2FPGA1FrameData[pDDRShareRam->DSP2FPGA1WritePosition]
#define DSP2FPGA1_ARESTLIMIT (DSP2FPGA_BUF1_SIZE  - pDDRShareRam->DSP2FPGA1WritePosition)
#define DSP2FPGA1_LAST_AREA  (pDDRShareRam->DSP2FPGA1ReadPosition == pDDRShareRam->DSP2FPGA1WritePosition)?(DSP2FPGA_BUF1_SIZE):(DSP2FPGA_BUF1_SIZE + pDDRShareRam->DSP2FPGA1ReadPosition - pDDRShareRam->DSP2FPGA1WritePosition)%(DSP2FPGA_BUF1_SIZE)

#define DSP2FPGA2_READ_ADDR  &pDDRShareRam->DSP2FPGA2FrameData[pDDRShareRam->DSP2FPGA2ReadPosition]
#define DSP2FPGA2_WRITE_ADDR &pDDRShareRam->DSP2FPGA2FrameData[pDDRShareRam->DSP2FPGA2WritePosition]
#define DSP2FPGA2_ARESTLIMIT (DSP2FPGA_BUF2_SIZE  - pDDRShareRam->DSP2FPGA2WritePosition)
#define DSP2FPGA2_LAST_AREA  (pDDRShareRam->DSP2FPGA2ReadPosition == pDDRShareRam->DSP2FPGA2WritePosition)?(DSP2FPGA_BUF2_SIZE):(DSP2FPGA_BUF2_SIZE + pDDRShareRam->DSP2FPGA2ReadPosition - pDDRShareRam->DSP2FPGA2WritePosition)%(DSP2FPGA_BUF2_SIZE)

#define DSP2FPGA3_READ_ADDR  &pDDRShareRam->DSP2FPGA3FrameData[pDDRShareRam->DSP2FPGA3ReadPosition]
#define DSP2FPGA3_WRITE_ADDR &pDDRShareRam->DSP2FPGA3FrameData[pDDRShareRam->DSP2FPGA3WritePosition]
#define DSP2FPGA3_ARESTLIMIT (DSP2FPGA_BUF3_SIZE  - pDDRShareRam->DSP2FPGA3WritePosition)
#define DSP2FPGA3_LAST_AREA  (pDDRShareRam->DSP2FPGA3ReadPosition == pDDRShareRam->DSP2FPGA3WritePosition)?(DSP2FPGA_BUF3_SIZE):(DSP2FPGA_BUF3_SIZE + pDDRShareRam->DSP2FPGA3ReadPosition - pDDRShareRam->DSP2FPGA3WritePosition)%(DSP2FPGA_BUF3_SIZE)

#define DSP2FPGA4_READ_ADDR  &pDDRShareRam->DSP2FPGA4FrameData[pDDRShareRam->DSP2FPGA4ReadPosition]
#define DSP2FPGA4_WRITE_ADDR &pDDRShareRam->DSP2FPGA4FrameData[pDDRShareRam->DSP2FPGA4WritePosition]
#define DSP2FPGA4_ARESTLIMIT (DSP2FPGA_BUF4_SIZE  - pDDRShareRam->DSP2FPGA4WritePosition)
#define DSP2FPGA4_LAST_AREA  (pDDRShareRam->DSP2FPGA4ReadPosition == pDDRShareRam->DSP2FPGA4WritePosition)?(DSP2FPGA_BUF4_SIZE):(DSP2FPGA_BUF4_SIZE + pDDRShareRam->DSP2FPGA4ReadPosition - pDDRShareRam->DSP2FPGA4WritePosition)%(DSP2FPGA_BUF4_SIZE)

#define FPGA2DSP_READ_ADDR   &pDDRShareRam->FPGA2DspFrameData[pDDRShareRam->FPGA2DspReadPosition]
#define FPGA2DSP_WRITE_ADDR  &pDDRShareRam->FPGA2DspFrameData[pDDRShareRam->FPGA2DspWritePosition]
#define FPGA2DSP_ARESTLIMIT  (FPGA2DSP_BUF_SIZE  - pDDRShareRam->FPGA2DspWritePosition)
#define FPGA2DSP_LAST_AREA   (pDDRShareRam->FPGA2DspReadPosition == pDDRShareRam->FPGA2DspWritePosition)?(FPGA2DSP_BUF_SIZE):(FPGA2DSP_BUF_SIZE + pDDRShareRam->FPGA2DspReadPosition - pDDRShareRam->FPGA2DspWritePosition)%(FPGA2DSP_BUF_SIZE)

#define DSP2ARM_READ_ADDR    &pDDRShareRam->Dsp2ArmFrameData[pDDRShareRam->Dsp2ArmReadPosition]
#define DSP2ARM_WRITE_ADDR   &pDDRShareRam->Dsp2ArmFrameData[pDDRShareRam->Dsp2ArmWritePosition]
#define DSP2ARM_ARESTLIMIT   (DSP2ARM_BUF_SIZE  - pDDRShareRam->Dsp2ArmWritePosition)
#define DSP2ARM_LAST_AREA    (pDDRShareRam->Dsp2ArmReadPosition == pDDRShareRam->Dsp2ArmWritePosition)?(DSP2ARM_BUF_SIZE):(DSP2ARM_BUF_SIZE + pDDRShareRam->Dsp2ArmReadPosition - pDDRShareRam->Dsp2ArmWritePosition)%(DSP2ARM_BUF_SIZE)

/*******************************************************************************
  * �������ƣ�QueueInit()��
  * ����˵�������г�ʼ�������������дָ�����㣻
  * ����������ޣ�
  * ����������ޣ�
  * ���ز��� ���ޣ�
  * ��ע	      ���ޣ�
*******************************************************************************/
void QueueInit(void)
{
    // ��������ʼ����
    pDDRShareRam->Arm2DspWritePosition   = 0;
    pDDRShareRam->Arm2DspReadPosition    = 0;

    pDDRShareRam->DSP2FPGA1WritePosition = 0;
    pDDRShareRam->DSP2FPGA1ReadPosition  = 0;
    pDDRShareRam->DSP2FPGA2WritePosition = 0;
    pDDRShareRam->DSP2FPGA2ReadPosition  = 0;
    pDDRShareRam->DSP2FPGA3WritePosition = 0;
    pDDRShareRam->DSP2FPGA3ReadPosition  = 0;
    pDDRShareRam->DSP2FPGA4WritePosition = 0;
    pDDRShareRam->DSP2FPGA4ReadPosition  = 0;

    pDDRShareRam->FPGA2DspWritePosition  = 0;
    pDDRShareRam->FPGA2DspReadPosition   = 0;

    pDDRShareRam->Dsp2ArmWritePosition   = 0;
    pDDRShareRam->Dsp2ArmReadPosition    = 0;

//    CacheBuf[0].valid = 0;
//    CacheBuf[1].valid = 0;
//    CacheBuf[2].valid = 0;
//    CacheBuf[3].valid = 0;
}

/*******************************************************************************
  * �������ƣ� isCPUTransData()��
  * ����˵���� �ж�CPU0->CPU1�Ƿ������ݿɶ���
  * ��������� �ޣ�
  * ������� ���ޣ�
  * ���ز��� ���ޣ�
  * ��   ע     ���ռ��С 7M��
*******************************************************************************/
u8 isCPUTransData(void)
{
    if(pDDRShareRam->Arm2DspReadPosition != pDDRShareRam->Arm2DspWritePosition) return TRUE;
    return FALSE;
}

/*******************************************************************************
  * �������ƣ� isDSP2FPGA1HaveData()��
  * ����˵���� �ж�CPU1->FPGA1�Ƿ������ݿɶ���
  * ��������� �ޣ�
  * ��������� �ޣ�
  * ���ز����� �ޣ�
  * ��  ע	    �� �ռ��С 0.5M��
*******************************************************************************/
u8 isDSP2FPGA1HaveData(void)
{
    if(pDDRShareRam->DSP2FPGA1ReadPosition != pDDRShareRam->DSP2FPGA1WritePosition) return TRUE;
    return FALSE;
}

/*******************************************************************************
  * �������ƣ� isDSP2FPGA2HaveData()��
  * ����˵���� �ж�CPU1->FPGA2�Ƿ������ݿɶ���
  * ��������� �ޣ�
  * ��������� �ޣ�
  * ���ز����� �ޣ�
  * ��  ע	    �� �ռ��С 2M��
*******************************************************************************/
u8 isDSP2FPGA2HaveData(void)
{
    if(pDDRShareRam->DSP2FPGA2ReadPosition != pDDRShareRam->DSP2FPGA2WritePosition) return TRUE;
    return FALSE;
}

/*******************************************************************************
  * �������ƣ� isDSP2FPGA3HaveData()��
  * ����˵���� �ж�CPU1->FPGA3�Ƿ������ݿɶ���
  * ��������� �ޣ�
  * ��������� �ޣ�
  * ���ز����� �ޣ�
  * ��  ע	    �� �ռ��С 3M��
*******************************************************************************/
u8 isDSP2FPGA3HaveData(void)
{
    if(pDDRShareRam->DSP2FPGA3ReadPosition != pDDRShareRam->DSP2FPGA3WritePosition) return TRUE;
    return FALSE;
}

/*******************************************************************************
  * �������ƣ� isDSP2FPGA4HaveData()��
  * ����˵���� �ж�CPU1->FPGA4�Ƿ������ݿɶ���
  * ��������� �ޣ�
  * ��������� �ޣ�
  * ���ز����� �ޣ�
  * ��  ע	    �� �ռ��С 3.5M��
*******************************************************************************/
u8 isDSP2FPGA4HaveData(void)
{
    if(pDDRShareRam->DSP2FPGA4ReadPosition != pDDRShareRam->DSP2FPGA4WritePosition) return TRUE;
    return FALSE;
}

/*******************************************************************************
  * �������ƣ� isFPGA2DSPHaveData()��
  * ����˵���� �ж�FPGA->CPU1�Ƿ������ݿɶ���
  * ��������� �ޣ�
  * ��������� �ޣ�
  * ���ز����� �ޣ�
  * ��  ע	    �� �ռ��С 7M��
*******************************************************************************/
u8 isFPGA2DSPHaveData(void)
{
    if(pDDRShareRam->FPGA2DspWritePosition != pDDRShareRam->FPGA2DspReadPosition) return TRUE;
    return FALSE;
}

/*******************************************************************************
  * �������ƣ� isDSP2ARMHaveData()��
  * ����˵���� �ж�CPU1->CPU0�Ƿ������ݿɶ���
  * ��������� �ޣ�
  * ��������� �ޣ�
  * ���ز����� �ޣ�
  * ��  ע	    �� �ռ��С 7M��
*******************************************************************************/
u8 isDSP2ARMHaveData(void)
{
    if(pDDRShareRam->Dsp2ArmWritePosition != pDDRShareRam->Dsp2ArmReadPosition) return TRUE;
    return FALSE;
}

/*******************************************************************************
  * ��������	writeData2DSP()
  * ����˵��	CPU0��CPU1д����
  * �������	type:��������
  * 		0:��ջ� 1:MPEG֡(DVB֡) 2:MPEG(����QP1/2)
  * 		3:MPEG(����QP2/3) 4:MPEG(����QP3/4) 5:DVB���ݶ�
  * 		6:QP1/2 ���ݶ� 7:QP2/3 ���ݶ� 8:QP3/4 ���ݶ�
  * 		len: ���ݳ���
  * 		Data:����ָ��
  * ������� 	��
  * ���ز��� 	TRUE 1 :  �ɹ�
  * 		FLASE 0 : ʧ��
  * ��ע		��
*******************************************************************************/
u8 writeData2DSP(unsigned short type, unsigned short len, unsigned short *Data)
{
    volatile pMIN_TRANS_UNIT pData = NULL;

    // ����ڶ���β����ʣ��ռ�Ų���
    //if(ARM2DSP_ARESTLIMIT < 100)
    //{
    //	xil_printf("ARM2DSP_LAST_AREA = %d\r\n",ARM2DSP_ARESTLIMIT);
    //}

    //�������8�ֽڶ�дָ�벻�ᱻ����
    //���ʣ��Ŀռ䲻����д�����ݣ�ͬʱʣ��ռ�=>4�ֽ�ʱ,�����ջ�
    if(((ARM2DSP_ARESTLIMIT - 8) < (len+6)) && ((ARM2DSP_ARESTLIMIT - 8) > 3))
    {
    	//д������
    	pData = (pMIN_TRANS_UNIT)ARM2DSP_WRITE_ADDR;
		// �����β
		pData->type = FRAME_TYPE_INVALID;	//��ջ�
		pData->startFlag = DATA_VALID;

		pDDRShareRam->Arm2DspWritePosition = 0;
    }
    else if(((ARM2DSP_ARESTLIMIT - 8) < (len+6)) && ((ARM2DSP_ARESTLIMIT - 8) <= 3))
    {
    	//û���㹻�ռ�����ջ�
    	pDDRShareRam->Arm2DspWritePosition = 0;
    }

    pData = (pMIN_TRANS_UNIT)ARM2DSP_WRITE_ADDR;

    // ��ʼ����
    memcpy((void *)(pData->Data),Data,len);
    // д���� ���� ��ɱ��
    pData->type = type;
    pData->len = len;
    pData->startFlag = DATA_VALID;
    pDDRShareRam->Arm2DspWritePosition += (pData->len + 6);
//    xil_printf("writeData2DSP Arm2DspWritePosition = %d\r\n", pDDRShareRam->Arm2DspWritePosition);

    return TRUE;
}

u8 writeData2FPGA_BUF1(unsigned short type, unsigned short len, unsigned short *Data)
{
	pMIN_TRANS_UNIT pData = NULL;

    // ����ڶ���β����ʣ��ռ�Ų���
    if(((DSP2FPGA1_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA1_ARESTLIMIT -8) > 3))
    {
        pData = (pMIN_TRANS_UNIT)DSP2FPGA1_WRITE_ADDR;
        // �����β
        pData->type = FRAME_TYPE_INVALID;
        pData->startFlag = DATA_VALID;

        pDDRShareRam->DSP2FPGA1WritePosition = 0;  // λ�÷ŵ�����ͷ
        return FALSE;
    }
    else if(((DSP2FPGA1_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA1_ARESTLIMIT - 8) <= 3))
    {
    	//û���㹻�ռ�����ջ�
    	pDDRShareRam->DSP2FPGA1WritePosition = 0;
    	return FALSE;
    }

    pData = (pMIN_TRANS_UNIT)DSP2FPGA1_WRITE_ADDR;
    // ��ʼ����
    memcpy((void *)(pData->Data), Data, len);
    // д���� ���� ��ɱ��
    pData->type = type;
    pData->len = len;
    pData->startFlag = DATA_VALID;
    pDDRShareRam->DSP2FPGA1WritePosition += (pData->len+6);

    return TRUE;
}

u8 writeData2FPGA_BUF2(unsigned short type, unsigned short len, unsigned short *Data)
{
    pMIN_TRANS_UNIT pData = NULL;

    // ����ڶ���β����ʣ��ռ�Ų���
	if(((DSP2FPGA2_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA2_ARESTLIMIT -8) > 3))
	{
		pData = (pMIN_TRANS_UNIT)DSP2FPGA2_WRITE_ADDR;
		// �����β
		pData->type = FRAME_TYPE_INVALID;
		pData->startFlag = DATA_VALID;

		pDDRShareRam->DSP2FPGA2WritePosition = 0;  // λ�÷ŵ�����ͷ
		return FALSE;
	}
	else if(((DSP2FPGA2_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA2_ARESTLIMIT - 8) <= 3))
	{
		//û���㹻�ռ�����ջ�
		pDDRShareRam->DSP2FPGA2WritePosition = 0;
		return FALSE;
	}

    pData = (pMIN_TRANS_UNIT)DSP2FPGA2_WRITE_ADDR;
    // ��ʼ����
    memcpy((void *)(pData->Data),Data,len);
    // д���� ���� ��ɱ��
    pData->type = type;
    pData->len = len;
    pData->startFlag = DATA_VALID;
    pDDRShareRam->DSP2FPGA2WritePosition += (pData->len+6);

    return TRUE;
}

u8 writeData2FPGA_BUF3(unsigned short type, unsigned short len, unsigned short *Data)
{
    pMIN_TRANS_UNIT pData = NULL;

    // ����ڶ���β����ʣ��ռ�Ų���
 	if(((DSP2FPGA3_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA3_ARESTLIMIT -8) > 3))
 	{
 		pData = (pMIN_TRANS_UNIT)DSP2FPGA3_WRITE_ADDR;
 		// �����β
 		pData->type = FRAME_TYPE_INVALID;
 		pData->startFlag = DATA_VALID;

 		pDDRShareRam->DSP2FPGA3WritePosition = 0;  // λ�÷ŵ�����ͷ
 		return FALSE;
 	}
 	else if(((DSP2FPGA3_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA3_ARESTLIMIT - 8) <= 3))
 	{
 		//û���㹻�ռ�����ջ�
 		pDDRShareRam->DSP2FPGA3WritePosition = 0;
 		return FALSE;
 	}

    pData = (pMIN_TRANS_UNIT)DSP2FPGA3_WRITE_ADDR;
    // ��ʼ����
    memcpy((void *)(pData->Data),Data,len);
    // д���� ���� ��ɱ��
    pData->type = type;
    pData->len = len;
    pData->startFlag = DATA_VALID;
    pDDRShareRam->DSP2FPGA3WritePosition += (pData->len+6);

    return TRUE;
}

u8 writeData2FPGA_BUF4(unsigned short type, unsigned short len, unsigned short *Data)
{
    pMIN_TRANS_UNIT pData = NULL;

    // ����ڶ���β����ʣ��ռ�Ų���
 	if(((DSP2FPGA4_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA4_ARESTLIMIT -8) > 3))
 	{
 		pData = (pMIN_TRANS_UNIT)DSP2FPGA4_WRITE_ADDR;
 		// �����β
 		pData->type = FRAME_TYPE_INVALID;
 		pData->startFlag = DATA_VALID;

 		pDDRShareRam->DSP2FPGA4WritePosition = 0;  // λ�÷ŵ�����ͷ
 		return FALSE;
 	}
 	else if(((DSP2FPGA4_ARESTLIMIT - 8) < (len+6)) && ((DSP2FPGA4_ARESTLIMIT - 8) <= 3))
 	{
 		//û���㹻�ռ�����ջ�
 		pDDRShareRam->DSP2FPGA4WritePosition = 0;
 		return FALSE;
 	}

    pData = (pMIN_TRANS_UNIT)DSP2FPGA4_WRITE_ADDR;
    // ��ʼ����
    memcpy((void *)(pData->Data),Data,len);
    // д���� ���� ��ɱ��
    pData->type = type;
    pData->len = len;
    pData->startFlag = DATA_VALID;
    pDDRShareRam->DSP2FPGA4WritePosition += (pData->len+6);

    return TRUE;
}

//fpga�������ݺ󣬷ŵ����λ��
u8 writeData2FPGAMPEGReportBUF(unsigned short type, unsigned short len, unsigned short *Data)
{
    pMIN_TRANS_UNIT pData = NULL;
    unsigned int lastArea,needArea;

    // ����ڶ���β����ʣ��ռ�Ų���
  	if(((FPGA2DSP_ARESTLIMIT - 8) < (len+6)) && ((FPGA2DSP_ARESTLIMIT -8) > 3))
  	{
  		pData = (pMIN_TRANS_UNIT)FPGA2DSP_WRITE_ADDR;
  		// �����β
  		pData->type = FRAME_TYPE_INVALID;
  		pData->startFlag = DATA_VALID;

  		pDDRShareRam->FPGA2DspWritePosition = 0;  // λ�÷ŵ�����ͷ
  	}
  	else if(((FPGA2DSP_ARESTLIMIT - 8) < (len+6)) && ((FPGA2DSP_ARESTLIMIT - 8) <= 3))
  	{
  		//û���㹻�ռ�����ջ�
  		pDDRShareRam->FPGA2DspWritePosition = 0;
  	}

  	// �п���û�пռ��ˣ��Ų���ȥ
	lastArea = (FPGA2DSP_LAST_AREA);
//	needArea = 400 + len; // �������һ��λ�����ܷ���
	needArea = 2006 + len; // �������һ��λ�����ܷ���,2006������ݳ��ȣ�
	if(lastArea < needArea)
	{
		return FALSE;  // �������ˣ�û�а취���Ų���ȥ��
	}

    pData = (pMIN_TRANS_UNIT)FPGA2DSP_WRITE_ADDR;
    // ��ʼ����
    memcpy((void *)(pData->Data),Data,len);
    // д���� ���� ��ɱ��
    pData->type = type;
    pData->len = len;
    pData->startFlag = DATA_VALID;
    pDDRShareRam->FPGA2DspWritePosition += (pData->len+6);
    FPGA_REPORT_MPEGNum1++;
    return TRUE;
}

u8 writeData2ARM(unsigned short type, unsigned short len, unsigned short *Data)
{
    pMIN_TRANS_UNIT pData = NULL;
    unsigned int lastArea,needArea ;

    // ����ڶ���β����ʣ��ռ�Ų���
  	if(((DSP2ARM_ARESTLIMIT - 8) < (len+6)) && ((DSP2ARM_ARESTLIMIT -8) > 3))
  	{
  		pData = (pMIN_TRANS_UNIT)DSP2ARM_WRITE_ADDR;
  		// �����β
  		pData->type = FRAME_TYPE_INVALID;
  		pData->startFlag = DATA_VALID;

  		pDDRShareRam->Dsp2ArmWritePosition = 0;  // λ�÷ŵ�����ͷ
  	}
  	else if(((DSP2ARM_ARESTLIMIT - 8) < (len+6)) && ((DSP2ARM_ARESTLIMIT - 8) <= 3))
  	{
  		//û���㹻�ռ�����ջ�
  		pDDRShareRam->Dsp2ArmWritePosition = 0;
  	}

  	// �п���û�пռ��ˣ��Ų���ȥ
  	lastArea = (DSP2ARM_LAST_AREA);
  	needArea = 2006 + len;
  	if(lastArea < needArea) return FALSE;

  	// ׼���ÿռ���
    pData = (pMIN_TRANS_UNIT)DSP2ARM_WRITE_ADDR;
    // ��ʼ����
    memcpy((void *)(pData->Data),Data,len);
    // д���� ���� ��ɱ��
    pData->type = type;
    pData->len = len;
    pData->startFlag = DATA_VALID;
    pDDRShareRam->Dsp2ArmWritePosition += (pData->len+6);

    return TRUE;
}

/*******************************************************************************
  * �������ƣ� lwip_CopyData2FPGA_BUF2()��
  * ����˵���� �����ڽ��յ�������ֱ�ӿ�����FPGA_BUF2�У�
  * ��������� �ޣ�
  * ��������� �ޣ�
  * ���ز����� �ޣ�
  * ��  ע	    �� �ռ��С 2M��
*******************************************************************************/
u8 lwip_CopyData2FPGA_BUF2( char *Data, int len )
{
    //����ڶ���β����ʣ��ռ�Ų���,��ȥ8����Ϊ8���ֽڷŶ�д��ַ��
	if( ( ( DSP2FPGA2_ARESTLIMIT - 8 ) <  1500 ) )
	{
		pDDRShareRam->DSP2FPGA2WritePosition = 0;  //λ�÷ŵ�����ͷ
//		return FALSE;
	}

    // ��ʼ����
    memcpy( (void *)DSP2FPGA2_WRITE_ADDR, Data, len );
    pDDRShareRam->DSP2FPGA2WritePosition += len;

    return TRUE;
}

u8 lwip_readFromARM(void)
{
	unsigned int lastArea,needArea;

	volatile pMIN_TRANS_UNIT pData = NULL;

    if(!isCPUTransData()) return FALSE;

    pData = (pMIN_TRANS_UNIT)ARM2DSP_READ_ADDR;

    //xil_printf("pData = %x,flag=%x\r\n",pData,pData->startFlag);
    if(((ARM2DSP_BUF_SIZE  - pDDRShareRam->Arm2DspReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
	{
		pDDRShareRam->Arm2DspReadPosition = 0;
		pData = (pMIN_TRANS_UNIT)ARM2DSP_READ_ADDR;
	}

    if(pData->startFlag != DATA_VALID) return FALSE;
//    if(pData->startFlag != DATA_VALID) // �����ݣ���û����5555  �쳣���
//	{
//		pDDRShareRam->Arm2DspReadPosition = 0;
//		return FALSE;
//	}

    if(pData->type == FRAME_TYPE_INVALID)
    {
        pData->startFlag = DATA_EMPTY;
        pDDRShareRam->Arm2DspReadPosition = 0;
        return FALSE;
    }

	if(writeData2FPGA_BUF1(1, pData->len, (unsigned short *)pData->Data))
	{
	    pData->startFlag = DATA_EMPTY;
	    pDDRShareRam->Arm2DspReadPosition += (pData->len+6);
//	    xil_printf("lwip_readFromARM 1 Arm2DspReadPosition = %d\r\n", pDDRShareRam->Arm2DspReadPosition);
	    return TRUE;
	}

	if(writeData2FPGA_BUF2(2, pData->len, (unsigned short *)pData->Data))
	{
	    pData->startFlag = DATA_EMPTY;
	    pDDRShareRam->Arm2DspReadPosition += (pData->len+6);
//	    xil_printf("lwip_readFromARM 2 Arm2DspReadPosition = %d\r\n", pDDRShareRam->Arm2DspReadPosition);
		return TRUE;
	}
	if(writeData2FPGA_BUF3(3, pData->len, (unsigned short *)pData->Data))
	{
		pData->startFlag = DATA_EMPTY;
		pDDRShareRam->Arm2DspReadPosition += (pData->len+6);
//		xil_printf("lwip_readFromARM 3 Arm2DspReadPosition = %d\r\n", pDDRShareRam->Arm2DspReadPosition);
		return TRUE;
	}
	if(writeData2FPGA_BUF4(4, pData->len, (unsigned short *)pData->Data))
	{
	    pData->startFlag = DATA_EMPTY;
	    pDDRShareRam->Arm2DspReadPosition += (pData->len+6);
//	    xil_printf("lwip_readFromARM 4 Arm2DspReadPosition = %d\r\n", pDDRShareRam->Arm2DspReadPosition);
		return TRUE;
	}

//	return TRUE;
}

u8 lwip_readFromFPGAPriBuf(void)
{
	unsigned char *startAddr,*endAddr;
	unsigned int  i,chl;
    unsigned char   PriFlag = 0;
    pMIN_TRANS_UNIT pData = NULL;

    if(isDSP2FPGA1HaveData())
    {
    	PriFlag = 1;
        pData = (pMIN_TRANS_UNIT)DSP2FPGA1_READ_ADDR;

        if(((DSP2FPGA_BUF1_SIZE  - pDDRShareRam->DSP2FPGA1ReadPosition) - 8) < 1500) // 20171020�����߽���ռ䲻��
		{
			pDDRShareRam->DSP2FPGA1ReadPosition = 0;
			pData = (pMIN_TRANS_UNIT)DSP2FPGA1_READ_ADDR;
		}
    }
    else if(isDSP2FPGA2HaveData())
    {
        PriFlag = 2;
        pData = (pMIN_TRANS_UNIT)DSP2FPGA2_READ_ADDR;

        if(((DSP2FPGA_BUF2_SIZE  - pDDRShareRam->DSP2FPGA2ReadPosition) - 8) < 1500) // 20171020�����߽���ռ䲻��
		{
			pDDRShareRam->DSP2FPGA2ReadPosition = 0;
			pData = (pMIN_TRANS_UNIT)DSP2FPGA2_READ_ADDR;
		}
    }
    else if(isDSP2FPGA3HaveData())
    {
        PriFlag = 3;
        pData = (pMIN_TRANS_UNIT)DSP2FPGA3_READ_ADDR;

        if(((DSP2FPGA_BUF3_SIZE  - pDDRShareRam->DSP2FPGA3ReadPosition) - 8) < 1500) // 20171020�����߽���ռ䲻��
		{
			pDDRShareRam->DSP2FPGA3ReadPosition = 0;
			pData = (pMIN_TRANS_UNIT)DSP2FPGA3_READ_ADDR;
		}
    }
    else if(isDSP2FPGA4HaveData()) // ���һ�����в����ж��Ƿ�׼���ã���Ϊ�����Ѿ����������ȼ�����
    {
        PriFlag = 4;
        pData = (pMIN_TRANS_UNIT)DSP2FPGA4_READ_ADDR;

        if(((DSP2FPGA_BUF4_SIZE  - pDDRShareRam->DSP2FPGA4ReadPosition) - 8) < 1500) // 20171020�����߽���ռ䲻��
		{
			pDDRShareRam->DSP2FPGA4ReadPosition = 0;
			pData = (pMIN_TRANS_UNIT)DSP2FPGA4_READ_ADDR;
		}
    }
    else  // ���п��ˣ�����У��Ž�ȥ(���ж����Ҫ����,�������źű�ʾ����ֱ�Ӳ���)
    {
    	return FALSE;
    }

    if(pData == NULL)
    {
    	return FALSE;
    }

    if(pData->startFlag != DATA_VALID)//
	{
    	xil_printf( "\n\r DATA_VALID! startFlag = %x \n\r ", pData->startFlag );
        pData->startFlag = DATA_EMPTY;
        if(PriFlag == 1) pDDRShareRam->DSP2FPGA1ReadPosition += ( pData->len + 6 );
        else if(PriFlag == 2) pDDRShareRam->DSP2FPGA2ReadPosition += ( pData->len + 6 );
        else if(PriFlag == 3) pDDRShareRam->DSP2FPGA3ReadPosition += ( pData->len + 6 );
        else if(PriFlag == 4) pDDRShareRam->DSP2FPGA4ReadPosition += ( pData->len + 6 );
    	return FALSE;
	}
    if(pData->type == FRAME_TYPE_INVALID)//��ǰ֡�Ƿ���������  			   //������ջ����ָ�븴λ
    {
    	xil_printf( "\n\r FRAME_TYPE_INVALID! \n\r " );
        pData->startFlag = DATA_EMPTY;
        if(PriFlag == 1) pDDRShareRam->DSP2FPGA1ReadPosition += ( pData->len + 6 );
        else if(PriFlag == 2) pDDRShareRam->DSP2FPGA2ReadPosition += ( pData->len + 6 );
        else if(PriFlag == 3) pDDRShareRam->DSP2FPGA3ReadPosition += ( pData->len + 6 );
        else if(PriFlag == 4) pDDRShareRam->DSP2FPGA4ReadPosition += ( pData->len + 6 );
        return FALSE;
    }

//    writeData2FPGAMPEGReportBUF(pData->type, pData->len, (unsigned short *)pData->Data);
    if( 0 != TxDMA( ( const char * )pData->Data, pData->len + 6 ) )
    {
    	xil_printf( "\n\r TxDMA fail! \n\r " );
    	return FALSE;
    }

    pData->startFlag = DATA_EMPTY;
    if(PriFlag == 1) pDDRShareRam->DSP2FPGA1ReadPosition += ( pData->len + 6 );
    else if(PriFlag == 2) pDDRShareRam->DSP2FPGA2ReadPosition += ( pData->len + 6 );
    else if(PriFlag == 3) pDDRShareRam->DSP2FPGA3ReadPosition += ( pData->len + 6 );
    else if(PriFlag == 4) pDDRShareRam->DSP2FPGA4ReadPosition += ( pData->len + 6 );

    return TRUE;
}

u8 lwip_readFromFPGAMPEGReportBUF(void)
{
    pMIN_TRANS_UNIT  pData = NULL;

    if(!isFPGA2DSPHaveData()) return FALSE;

    pData = (pMIN_TRANS_UNIT)FPGA2DSP_READ_ADDR;

    if(((FPGA2DSP_BUF_SIZE  - pDDRShareRam->FPGA2DspReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
    {
    	pDDRShareRam->FPGA2DspReadPosition = 0;
    	pData = (pMIN_TRANS_UNIT)FPGA2DSP_READ_ADDR;
    }
    if(pData->startFlag != DATA_VALID) return FALSE;
//	if(pData->startFlag != DATA_VALID) // �����ݣ���û����5555  �쳣���
//	{
//		pDDRShareRam->FPGA2DspReadPosition = 0;
//		return FALSE;
//	}

    if(pData->type == FRAME_TYPE_INVALID)
    {
        pData->startFlag = DATA_EMPTY;
        pDDRShareRam->FPGA2DspReadPosition = 0;
        return FALSE;
    }

    // FPGA���������ݣ�Ҫ����MPEG����ȥ����
//    if(FALSE == recvMpeg((u8 *)pData->Data,(u16)pData->len)) return FALSE;
    writeData2ARM(pData->type, pData->len, (unsigned short *)pData->Data);

    // �������
    pData->startFlag = DATA_EMPTY;
    pDDRShareRam->FPGA2DspReadPosition += (pData->len+6);
//    xil_printf("lwip_readFromFPGAMPEGReportBUF FPGA2DspReadPosition = %d\r\n", pDDRShareRam->FPGA2DspReadPosition);

    return TRUE;
}

u16 lwip_readFromDSP2ARM(unsigned char *Data)
{
	if(!isDSP2ARMHaveData()) return 0;

	pMIN_TRANS_UNIT  pData = NULL;

	pData = (MIN_TRANS_UNIT *)DSP2ARM_READ_ADDR;

	// �������������ռ��С�ˣ�Ҫ�ص�0��ַ��
	if(((DSP2ARM_BUF_SIZE  - pDDRShareRam->Dsp2ArmReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
	{
		pDDRShareRam->Dsp2ArmReadPosition = 0;
		pData = (MIN_TRANS_UNIT *)DSP2ARM_READ_ADDR;
	}
	if(pData->startFlag != DATA_VALID) return 0;

	if(pData->type == FRAME_TYPE_INVALID) // �жϿ�����
	{
		pData->startFlag = DATA_EMPTY;
		pDDRShareRam->Dsp2ArmReadPosition = 0;
		return 0; // ���ζ˵㣬�ص���ͷ
	}
	memcpy(Data, (void *)pData->Data, pData->len);
	pDDRShareRam->Dsp2ArmReadPosition += (pData->len + 6);

	return pData->len;
}


//u8 readFromARM(void)
//{
//	//int i;
//	u8 priority = 0;
////	u8 type = 0;
//	unsigned int lastArea,needArea;
//
//	volatile pMIN_TRANS_UNIT pData = NULL;
//
//    //xil_printf("dsp:r=%x,w=%x\n",pDDRShareRam->Arm2DspReadPosition,pDDRShareRam->Arm2DspWritePosition);
//    if(!isCPUTransData()) return FALSE;
//
//    pData = (pMIN_TRANS_UNIT)ARM2DSP_READ_ADDR;
//
//    //xil_printf("pData = %x,flag=%x\r\n",pData,pData->startFlag);
//    if(((ARM2DSP_BUF_SIZE  - pDDRShareRam->Arm2DspReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//	{
//		pDDRShareRam->Arm2DspReadPosition = 0;
//		pData = (pMIN_TRANS_UNIT)ARM2DSP_READ_ADDR;
//	}
//
//    if(pData->startFlag != DATA_VALID) return FALSE;
////    if(pData->startFlag != DATA_VALID) // �����ݣ���û����5555  �쳣���
////	{
////		pDDRShareRam->Arm2DspReadPosition = 0;
////		return FALSE;
////	}
//
//    if(pData->type == FRAME_TYPE_INVALID)
//    {
//        pData->startFlag = DATA_EMPTY;
//        pDDRShareRam->Arm2DspReadPosition = 0;
//        return FALSE;
//    }
//
//    priority = (pData->type) & 0x30;		//���ȼ�
//
//    // ����Ҫ�ж���Ӧ�����ȼ��������滹��û��λ��,û��λ��ֱ�ӷ��ؾͺ��ˣ��Ͳ��÷���
//    needArea = pData->len + 400; //�౸һЩλ��
//    if(priority == 0x00) lastArea = (DSP2FPGA1_LAST_AREA);
//    else if(priority == 0x10) lastArea = (DSP2FPGA2_LAST_AREA);
//    else if(priority == 0x20) lastArea = (DSP2FPGA3_LAST_AREA);
//    else
//    {
//    	priority = 0x30; // ��ֹ�д�������ȼ�����
//    	lastArea = (DSP2FPGA4_LAST_AREA);
//    }
//    if(lastArea < needArea) return FALSE;
//
//
////    type = (pData->type) & 0x0f;			//֡����
//
//    // ��ʼ��������  �����MPEGֱ�ӿ�������������ݶΣ���Ҫ�ֶδ���
////    if(type <= FRAME_MPEG_TYPE_CNT)
//    if(0 == (pData->type & 0x0001))//�жϵ�0λ��0����MPEG�����ݣ�ֱ���͸�FPGA��
//    {										//����.......
////    	xil_printf("pData->type = 0x%02x, priority = 0x%2x, pData->len = %d\r\n", pData->type, priority, pData->len);
//    	if(priority == 0x00) writeData2FPGA_BUF1(pData->type,pData->len,(unsigned short *)pData->Data);
//        else if(priority == 0x10) writeData2FPGA_BUF2(pData->type,pData->len,(unsigned short *)pData->Data);
//        else if(priority == 0x20) writeData2FPGA_BUF3(pData->type,pData->len,(unsigned short *)pData->Data);
//        else if(priority == 0x30) writeData2FPGA_BUF4(pData->type,pData->len,(unsigned short *)pData->Data);
//    	DSP2FPAG_BUF_READY[priority] = 1;
//    }
//    else //if(type <= FRAME_ALL_TYPE_CNT)��������Ƕμ����ݣ�
//    {
//    	if(0 == (pData->type & (0x0001 << 1)))//�жϵ�1λ��0����DVB���Σ�
//    	{
//    		if(FRAME_TYPE_TABLE[FRAME_MPEG_TYPE_COUNT][1] == ((pData->type >> 8) & 0xFF))//��һ���ж��Ƿ���DVB���ݶΣ�
//    			SendSection((u8 *)pData->Data, (u16)pData->len, DVB_DATA_SIZE, pData->type);
//    	}
//    	else//�������TDMA���Σ�
//    	{
//    		int typePos;
//    		for(typePos = 0; typePos < FRAME_MPEG_TYPE_COUNT; typePos++)
//    		{
//    			if(FRAME_TYPE_TABLE[typePos][1] == ((pData->type >> 8) & 0xFF))//�ж϶����ͣ�
//    			{
//    				SendSection((u8 *)pData->Data, (u16)pData->len, FRAME_TYPE_TABLE[typePos][2],pData->type);
//    			}
//    		}
//    	}
//    	//��ʼ���������������Ȼ��Ž��ڴ���
//    	//xil_printf("pData->type=%d\n",pData->type);
//    	// ���ݶ�
///*    	if(((pData->type) & 0x0f) == 0x05)		//DVB���ݶ�
//    	{
//    		SendSection((u8 *)pData->Data,(u16)pData->len,DVB_DATA_SIZE,pData->type);
//    	}
//    	else if(((pData->type) & 0x0f) == 0x06)		//QP 1/2���ݶ�
//		{
//    		SendSection((u8 *)pData->Data,(u16)pData->len,QP1_2_SIZE,pData->type);
//		}
//    	else if(((pData->type) & 0x0f) == 0x07)		//QP 2/3���ݶ�
//		{
//    		SendSection((u8 *)pData->Data,(u16)pData->len,QP2_3_SIZE,pData->type);
//		}
//    	else if(((pData->type) & 0x0f) == 0x08)		//QP 3/4���ݶ�
//		{
//    		SendSection((u8 *)pData->Data,(u16)pData->len,QP3_4_SIZE,pData->type);
//		}
//*/
//    }
//
//    // �������
//    pData->startFlag = DATA_EMPTY;
//    pDDRShareRam->Arm2DspReadPosition += (pData->len+6);
//    return TRUE;
//}

//u8 readFromFPGAPriBuf(void)
//{
//	unsigned char *startAddr,*endAddr;
//	unsigned int  i,chl;
//#ifndef  AXI_INTERFACE_OPTIMIZE_TX
//	unsigned char getData[4];
//#endif
//
//    unsigned char   PriFlag = 0;
//    pMIN_TRANS_UNIT pData = NULL;
//
//    // �����ж�����һ�������Ķ���û�з���
//    if(isDSP2FPGA4HaveData()) // ������ȼ�4����������
//    {
//    	PriFlag = 4;
//    	// �ж��Ƿ���һ���ε���ʼ���������ʼ�򲻷��ͣ������͸ð���
//    	pData = (pMIN_TRANS_UNIT)DSP2FPGA4_READ_ADDR; // �õ���ʼָ��
//		if(((DSP2FPGA_BUF4_SIZE  - pDDRShareRam->DSP2FPGA4ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA4ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA4_READ_ADDR;
//		}
//
//		if(!(((pData->Data[1] & 0x40) == 0x40) && (pData->Data[4] == 0))) // ����ͷ
//		{
//			goto NormalSend;
//		}
//    }
//
//    if(isDSP2FPGA3HaveData()) // ������ȼ�3����������
//	{
//		PriFlag = 3;
//		// �ж��Ƿ���һ���ε���ʼ���������ʼ�򲻷��ͣ������͸ð���
//		pData = (pMIN_TRANS_UNIT)DSP2FPGA3_READ_ADDR; // �õ���ʼָ��
//		if(((DSP2FPGA_BUF3_SIZE  - pDDRShareRam->DSP2FPGA3ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA3ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA3_READ_ADDR;
//		}
//
//		if(!(((pData->Data[1] & 0x40) == 0x40) && (pData->Data[4] == 0))) // ����ͷ
//		{
//			goto NormalSend;
//		}
//	}
//
//    if(isDSP2FPGA2HaveData()) // ������ȼ�2����������
//	{
//		PriFlag = 2;
//		// �ж��Ƿ���һ���ε���ʼ���������ʼ�򲻷��ͣ������͸ð���
//		pData = (pMIN_TRANS_UNIT)DSP2FPGA2_READ_ADDR; // �õ���ʼָ��
//		if(((DSP2FPGA_BUF2_SIZE  - pDDRShareRam->DSP2FPGA2ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA2ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA2_READ_ADDR;
//		}
//
//		if(!(((pData->Data[1] & 0x40) == 0x40) && (pData->Data[4] == 0))) // ����ͷ
//		{
//			goto NormalSend;
//		}
//	}
//
//    if(isDSP2FPGA1HaveData()) // ������ȼ�1����������
//	{
//		PriFlag = 1;
//		// �ж��Ƿ���һ���ε���ʼ���������ʼ�򲻷��ͣ������͸ð���
//		pData = (pMIN_TRANS_UNIT)DSP2FPGA1_READ_ADDR; // �õ���ʼָ��
//		if(((DSP2FPGA_BUF1_SIZE  - pDDRShareRam->DSP2FPGA1ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA1ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA1_READ_ADDR;
//		}
//
//		if(!(((pData->Data[1] & 0x40) == 0x40) && (pData->Data[4] == 0))) // ����ͷ
//		{
//			goto NormalSend;
//		}
//	}
//
//
//    // ���������ȼ��߼�
//    if(isDSP2FPGA1HaveData() && (DSP2FPAG_BUF_READY[0] == 1))
//    {
//        PriFlag = 1;
//        pData = (pMIN_TRANS_UNIT)DSP2FPGA1_READ_ADDR;
//
//        if(((DSP2FPGA_BUF1_SIZE  - pDDRShareRam->DSP2FPGA1ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA1ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA1_READ_ADDR;
//		}
//    }
//    else if(isDSP2FPGA2HaveData() && (DSP2FPAG_BUF_READY[1] == 1))
//    {
//        PriFlag = 2;
//        pData = (pMIN_TRANS_UNIT)DSP2FPGA2_READ_ADDR;
//
//        if(((DSP2FPGA_BUF2_SIZE  - pDDRShareRam->DSP2FPGA2ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA2ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA2_READ_ADDR;
//		}
//    }
//    else if(isDSP2FPGA3HaveData() && (DSP2FPAG_BUF_READY[2] == 1))
//    {
//        PriFlag = 3;
//        pData = (pMIN_TRANS_UNIT)DSP2FPGA3_READ_ADDR;
//
//        if(((DSP2FPGA_BUF3_SIZE  - pDDRShareRam->DSP2FPGA3ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA3ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA3_READ_ADDR;
//		}
//    }
//    else if(isDSP2FPGA4HaveData()) // ���һ�����в����ж��Ƿ�׼���ã���Ϊ�����Ѿ����������ȼ�����
//    {
//        PriFlag = 4;
//        pData = (pMIN_TRANS_UNIT)DSP2FPGA4_READ_ADDR;
//
//        if(((DSP2FPGA_BUF4_SIZE  - pDDRShareRam->DSP2FPGA4ReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//		{
//			pDDRShareRam->DSP2FPGA4ReadPosition = 0;
//			pData = (pMIN_TRANS_UNIT)DSP2FPGA4_READ_ADDR;
//		}
//    }
//    else  // ���п��ˣ�����У��Ž�ȥ(���ж����Ҫ����,�������źű�ʾ����ֱ�Ӳ���)
//    {
//    	g_DSP2FPGA_QUEUE_EMPTY = 1;
//    }
//
//    if(pData == NULL)
//    {
//    	return FALSE;
//    }
//
//NormalSend:
//    if(pData->startFlag != DATA_VALID) return FALSE;
////    if(pData->startFlag != DATA_VALID) // �����ݣ���û����5555  �쳣���
////	{
////    	if(PriFlag == 1) pDDRShareRam->DSP2FPGA1ReadPosition = 0;
////		else if(PriFlag == 2) pDDRShareRam->DSP2FPGA2ReadPosition = 0;
////		else if(PriFlag == 3) pDDRShareRam->DSP2FPGA3ReadPosition = 0;
////		else if(PriFlag == 4) pDDRShareRam->DSP2FPGA4ReadPosition = 0;
////		return FALSE;
////	}
//
//    if(pData->type == FRAME_TYPE_INVALID)  			   //������ջ����ָ�븴λ
//    {
//        pData->startFlag = DATA_EMPTY;
//        if(PriFlag == 1) pDDRShareRam->DSP2FPGA1ReadPosition = 0;
//        else if(PriFlag == 2) pDDRShareRam->DSP2FPGA2ReadPosition = 0;
//        else if(PriFlag == 3) pDDRShareRam->DSP2FPGA3ReadPosition = 0;
//        else if(PriFlag == 4) pDDRShareRam->DSP2FPGA4ReadPosition = 0;
//        return FALSE;
//    }
//
//    // ����DMA�ӿڣ�����Ϣ���ͳ�ȥ ��FPGA
//#if 0 // for test
//    writeData2FPGAMPEGReportBUF(pData->type,pData->len,(unsigned short *)pData->Data);
//#else
//	i = pData->len;
//	i = (i << 16);
//	// �ӿڵ�һ���֣���16λ��ʶ���ȣ��м�8λ��ʶ0��ʶDVBS2��1��ʶTDMA��
//	if(pData->len != 188) i |= 0x00000100; // TDMA��ʽ
//
//	// Ҫ���ĸ�ʹ����Ч����Ч�Ĳ��ܷ�
//    chl = ((pData->type >> 8) & 0x3);  // FIXME:��ʱ֧��2��FECͨ������
//    if(chl == 0)
//    {
//    	if(1 != XGpioPs_ReadPin(&psGpioInstancePtr, 55)) return FALSE; // ������
//    }
//    else if(chl == 1)
//    {
//    	if(1 != XGpioPs_ReadPin(&psGpioInstancePtr, 54)) return FALSE; // ������
//    }
//    else if(chl == 2)
//    {
//    	if(1 != XGpioPs_ReadPin(&psGpioInstancePtr, 57)) return FALSE; // ������
//    }
//	// FPGA��3�����У���ַ��10-11λ // MARK,  FIXME:��ʱ֧��2��FECͨ������
//	startAddr = (unsigned char *)((XPAR_BRAM_1_BASEADDR+4096) | (chl << 10));
//	endAddr = (unsigned char *)(((XPAR_BRAM_1_BASEADDR+4096) + 512) | (chl << 10));
//
//	// ��ַ�������
//	Xil_Out32((unsigned int)startAddr,i); //Xil_Out32((XPAR_BRAM_1_BASEADDR+4096),i);  // д����
//    startAddr += 4;
//    i = 0;
//    while(startAddr < endAddr)
//    {
//#ifndef AXI_INTERFACE_OPTIMIZE_TX
//    	getData[0] = pData->Data[i++];    // ���Ļ���buf ������4�ֽڶ���
//    	getData[1] = pData->Data[i++];
//    	getData[2] = pData->Data[i++];
//    	getData[3] = pData->Data[i++];
//    	Xil_Out32((int)startAddr,*(unsigned int *)getData);
//    	startAddr += 4;
//#else
//    	Xil_Out32((int)startAddr,*(unsigned int *)(&pData->Data[i]));
//    	i += 4;
//		startAddr += 4;
//		if(i >= pData->len) break;
//#endif
//    }
//
//#ifdef AXI_INTERFACE_OPTIMIZE_TX
//    if(startAddr < endAddr) Xil_Out32((int)(endAddr-4),0);	 // �������һ����ַ
//#endif
//
//#endif
//
//    // DMA����ն�֮�������
//    pData->startFlag = DATA_EMPTY;
//    if(PriFlag == 1) pDDRShareRam->DSP2FPGA1ReadPosition += (pData->len+6);
//    else if(PriFlag == 2) pDDRShareRam->DSP2FPGA2ReadPosition += (pData->len+6);
//    else if(PriFlag == 3) pDDRShareRam->DSP2FPGA3ReadPosition += (pData->len+6);
//    else if(PriFlag == 4) pDDRShareRam->DSP2FPGA4ReadPosition += (pData->len+6);
//
//    return TRUE;
//}

//u8 readFromFPGAMPEGReportBUF(void)
//{
//    pMIN_TRANS_UNIT  pData       = NULL;
//
//    if(!isFPGA2DSPHaveData()) return FALSE;
//
//    pData = (pMIN_TRANS_UNIT)FPGA2DSP_READ_ADDR;
//
//    if(((FPGA2DSP_BUF_SIZE  - pDDRShareRam->FPGA2DspReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//    {
//    	pDDRShareRam->FPGA2DspReadPosition = 0;
//    	pData = (pMIN_TRANS_UNIT)FPGA2DSP_READ_ADDR;
//    }
//    if(pData->startFlag != DATA_VALID) return FALSE;
////	if(pData->startFlag != DATA_VALID) // �����ݣ���û����5555  �쳣���
////	{
////		pDDRShareRam->FPGA2DspReadPosition = 0;
////		return FALSE;
////	}
//
//    if(pData->type == FRAME_TYPE_INVALID)
//    {
//        pData->startFlag = DATA_EMPTY;
//        pDDRShareRam->FPGA2DspReadPosition = 0;
//        return FALSE;
//    }
//
//    // FPGA���������ݣ�Ҫ����MPEG����ȥ����
//    if(FALSE == recvMpeg((u8 *)pData->Data,(u16)pData->len)) return FALSE;
//
//    // �������
//    pData->startFlag = DATA_EMPTY;
//    pDDRShareRam->FPGA2DspReadPosition += (pData->len+6);
//
//    return TRUE;
//}


/******************************���º�AXI���**********************************************************/
//void Read_MAC_AXI_Data(void)
//{
//	unsigned int  i;
//#ifndef AXI_INTERFACE_OPTIMIZE_RX
//	unsigned char getData[4];
//#endif
//	unsigned char *startAddr,*endAddr;
//	unsigned int  AXI_DATA_TYPE;
//	MIN_TRANS_UNIT RecvData;
//
//	startAddr = (unsigned char *)XPAR_BRAM_0_BASEADDR;
//	endAddr = (unsigned char *)(XPAR_BRAM_0_BASEADDR + 512);
//
//	// ȡ����
//	AXI_DATA_TYPE = Xil_In32((int)startAddr);
//	startAddr += 4;
//
//	RecvData.len = (AXI_DATA_TYPE >> 16);  // �����ڸ�λ
//
//    // ȡ����
//	i = 0;
//	while(startAddr < endAddr)
//	{
//#ifndef AXI_INTERFACE_OPTIMIZE_RX
//		*(unsigned int *)getData = Xil_In32((int)startAddr);  // ���Ļ���buf ������4�ֽڶ���
//		RecvData.Data[i++] = getData[0];
//		RecvData.Data[i++] = getData[1];
//		RecvData.Data[i++] = getData[2];
//		RecvData.Data[i++] = getData[3];
//		startAddr += 4;
//#else
//		*(unsigned int *)(&RecvData.Data[i]) = Xil_In32((int)startAddr);
//		i += 4;
//		startAddr += 4;
//		if(i >= RecvData.len) break;
//#endif
//	}
//
//#ifdef AXI_INTERFACE_OPTIMIZE_RX
//	if(startAddr < endAddr) Xil_In32((int)(endAddr-8));	 // �������һ����ַ
//	Xil_Out32((int)(endAddr-4),0xAAAAAAAA);
//	//Xil_Out32((int)(endAddr-4),0x55555555);
//#endif
//
////	if(RecvData.len < 188) return;  // ����Ķ���,����û�б�Ҫ��
//
///*	if(QP1_2_SIZE == RecvData.len) RecvData.type = FRAME_TYPE_MPEG_QP1_2;
//	else if(QP2_3_SIZE == RecvData.len) RecvData.type = FRAME_TYPE_MPEG_QP2_3;
//	else if(QP3_4_SIZE == RecvData.len) RecvData.type = FRAME_TYPE_MPEG_QP3_4;
//	else RecvData.type = FRAME_TYPE_MPEG_DVB;
//*/
//	int typePos;
//	for(typePos = 0; typePos < FRAME_MPEG_TYPE_COUNT; typePos++)
//	{
//		if(FRAME_TYPE_TABLE[typePos][2] == RecvData.len)//����MPEG�ı��뷽ʽ���ж�֡����
//		{
//			RecvData.type = FRAME_TYPE_TABLE[typePos][0];
//			break;
//		}
//	}
//	if(FRAME_MPEG_TYPE_COUNT <= typePos) return;//����û�У��򷵻أ�
//
//	if((RecvData.Data[0] == 0x47) && (RecvData.Data[1] != 0x16))
//	writeData2FPGAMPEGReportBUF(RecvData.type,RecvData.len,(unsigned short *)RecvData.Data);
//
//	return;
//}


/******************************���º�OCM���**********************************************************/
/* 20170920 ok */
//void ReadOCMMsg(char flag)
//{
//	volatile unsigned char * pBaseAddr =  NULL;
//	volatile pMIN_TRANS_UNIT  pData = NULL;
//
//	// ��ֹ����
//	if((ocmIntr_avoid_reIn == 1) && (flag == 0))
//	{
//		cpu0_ocm_yw_missIntr = 1;
//		return; // �ȴ����������꣬�ܿ�ʹ�����
//	}
//
//	// ���жϿ�������
//	pBaseAddr = (unsigned char *)(OCM_BASE_ADDR + CPU0_WRITE_CMD_OFFSET); // CPU0дCPU1��������
//	if(*(unsigned short *)pBaseAddr == DATA_VALID) // CPU0��������
//	{
//		unsigned short length = *(volatile unsigned short *)(pBaseAddr+2);
//		unsigned short type = *(volatile unsigned short *)(pBaseAddr+4);
//		volatile unsigned char * pCmdAddr  =  (pBaseAddr+6);
//		switch(type)
//		{
//		case RETURN_CMD_TYPE_CPU1:
//			if(*(unsigned short *)pCmdAddr == 0x001) // ����PID
//			{
//				if(*(unsigned short *)(pCmdAddr+2) == 4)
//				{
//					YW_PID = *(unsigned short *)(pCmdAddr+4);
//					CardType = *(unsigned short *)(pCmdAddr+6);
//				}
//				else if(*(unsigned short *)(pCmdAddr+2) == 12)
//				{
//					YW_PID = *(unsigned short *)(pCmdAddr+4);
//					CardType = *(unsigned short *)(pCmdAddr+6);
//					g_UserSite_MY_MAC = *(u64 *)(pCmdAddr+8);  // MAC��ַҲ���ϵ�����
//				}
//			}
//			else if(*(unsigned short *)pCmdAddr == 0x002) // DSP�Ƿ�����TBTP�����ã�Ĭ�ϲ����ã�
//			{
//				if(*(unsigned short *)(pCmdAddr+2) == 2)
//				{
//				    g_UserSite_setTBTPflag = *(unsigned short *)(pCmdAddr+4);
//				}
//			}
//			break;
//		case RETURN_CMD_TYPE_FPGA:
//			// ֱ���͸�FPGA
//			PL_SetPara((unsigned char *)pCmdAddr);
//			break;
//		case RETURN_CMD_TYPE_936x:
//			if(length > 36) break; // 9364�������ܳ�
//			pBaseAddr[length] = 0x0D;pBaseAddr[length+1] = 0x0A;pBaseAddr[length+2] = 0x00; // �ַ��������ݴ�
//			console_process_ocm_cmd((char *)(pBaseAddr+6));
//		    break;
//		default:
//		    break;
//		}
//		*(unsigned short *)pBaseAddr = DATA_EMPTY;
//	}
//
//	// �ٴ�����������
//	pBaseAddr = (unsigned char *)(OCM_BASE_ADDR + CPU0_WRITE_DATA_OFFSET); // CPU0дCPU1��������
//    if(*(unsigned short *)pBaseAddr == DATA_VALID) // CPU0�������������Ƶ�DDR
//    {
//    	if(ocmIntr_wait_semphore == 1)
//		{
//			cpu0_ocm_yw_missIntr = 1;
//			return; // �ȴ����������꣬�ܿ�ʹ�����
//		}
//
//    	unsigned int lastArea = (ARM2DSP_LAST_AREA);
//    	unsigned int needArea = (2000 + (*(unsigned short *)(pBaseAddr+2)));
//    	// �����ж���û��λ�÷�
//    	if(lastArea > needArea)  // ��λ�÷ţ�Ų�ߣ�û��λ�þ���һ����˵
//    	{
//			// ��ʼ��
//			pData = (pMIN_TRANS_UNIT)(pBaseAddr + 4);
//			while(pData->startFlag == DATA_VALID)
//			{
//				writeData2DSP(pData->type,pData->len,(unsigned short *)pData->Data);
//				pData = (pMIN_TRANS_UNIT)(((unsigned char *)pData)+pData->len+6);
//				cpu0_data_semphore1++;  // ����Ч�����ݲŸ��ź�
//			}
//			*(unsigned short *)pBaseAddr = DATA_EMPTY;
//			cpu0_ocm_yw_missIntr = 0;
//    	}
//    	else
//    	{
//    	    // û��λ����ʱ����
//    		cpu0_ocm_yw_missIntr = 1;
//    	}
//    }
//}

/* 20170921 ok */
//u8 IsOCMDataEMPTY(void)
//{
//	unsigned char * pBaseAddr =  NULL;
//	pBaseAddr = (unsigned char *)(OCM_BASE_ADDR + CPU0_READ_DATA_OFFSET); // CPU1дCPU0��������
//	if(*(unsigned short *)pBaseAddr == DATA_EMPTY)
//	{
//		return TRUE;
//	}
//	return FALSE;
//}

/* 20170921 ok */
//u8 WriteYWDataToOCM(void)
//{
//	unsigned char * pBaseAddr =  NULL;
//	pBaseAddr = (unsigned char *)(OCM_BASE_ADDR + CPU0_READ_DATA_OFFSET); // CPU1дCPU0��������
//	if(*(unsigned short *)pBaseAddr == DATA_EMPTY)
//	{
//		MIN_TRANS_UNIT *pData;
//		unsigned short p_ocm_tmp_send_offset = 4;
//
//		while(TRUE == isDSP2ARMHaveData()) // �����ݣ��пռ��һֱ��
//		{
//			pData = (MIN_TRANS_UNIT *)DSP2ARM_READ_ADDR;
//
//			// �������������ռ��С�ˣ�Ҫ�ص�0��ַ��
//			if(((DSP2ARM_BUF_SIZE  - pDDRShareRam->Dsp2ArmReadPosition) - 8) <= 3) // 20171020�����߽���ռ䲻��
//			{
//				pDDRShareRam->Dsp2ArmReadPosition = 0;
//				pData = (MIN_TRANS_UNIT *)DSP2ARM_READ_ADDR;
//			}
//			if(pData->startFlag != DATA_VALID) break;
////			if(pData->startFlag != DATA_VALID) // �����ݣ���û����5555  �쳣���
////			{
////				pDDRShareRam->Dsp2ArmReadPosition = 0;
////				continue;
////			}
//			if(pData->type == FRAME_TYPE_INVALID) // �жϿ�����
//			{
//				pData->startFlag = DATA_EMPTY;
//				pDDRShareRam->Dsp2ArmReadPosition = 0;
//				continue; // ���ζ˵㣬�ص���ͷ
//			}
//			if((p_ocm_tmp_send_offset + pData->len + 6) < WRITE_DATA_MAX_LEN) // ��λ�÷�
//			{
//				memcpy(&pBaseAddr[p_ocm_tmp_send_offset],(void *)pData,(pData->len + 6));
//				p_ocm_tmp_send_offset += (pData->len + 6);
//				pDDRShareRam->Dsp2ArmReadPosition += (pData->len + 6);
//			}
//			else
//			{
//				break;
//			}
//		}
//		if(p_ocm_tmp_send_offset > 8)
//		{
//			*(unsigned short *)(pBaseAddr+p_ocm_tmp_send_offset) = DATA_EMPTY; // ������˲���β
//			*(unsigned short *)(pBaseAddr+2) = p_ocm_tmp_send_offset;
//			*(unsigned short *)(pBaseAddr) = DATA_VALID;
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

/* 20170922 ������   */
//u8 WriteCmdToOCM(unsigned short type, unsigned char * cmd, int len)
//{
//	unsigned char * pBaseAddr =  NULL;
//	pBaseAddr = (unsigned char *)(OCM_BASE_ADDR + CPU0_READ_CMD_OFFSET); // CPU1дCPU0��������
//
//	if(*(unsigned short *)pBaseAddr == DATA_EMPTY) // ʧ�ܾ�����
//	{
//		*(unsigned short *)(pBaseAddr+4) = type; // дtype
//		memcpy(&pBaseAddr[6],cmd,len);
//		*(unsigned short *)(pBaseAddr+2) = len+6;
//		*(unsigned short *)(pBaseAddr) = DATA_VALID;
//		// �����ж�
//		*p_ICDSGIR = 0x10009;			//�����жϸ�CPU0
//		return TRUE;
//	}
//	return FALSE;
//}

