/***********************************************************************************
* ��Ȩ����(C)2020, ���ݺ���ͨ�ż��Źɷ����޹�˾.
* ��Ŀ����:    S023
* �ļ�����:    dmaUser.c
* ����ƽ̨:    GCC
* ���뻷��:    GCC������
* ��������:    C
* ��       ��:    ����ǫ
* ����ժҪ:    DMA�û��ӿ�ģ�飻
* �������:    2020��11��
************************************************************************************/
/***********************************************************************************
 *                                   ͷ �� ��
 ***********************************************************************************/

/******************************** Xilinx ͷ�ļ� *************************************/
#include "xaxidma.h"
#include "xscugic.h"

/********************************* �Զ���ͷ�ļ� ***************************************/
#include "dmaUser.h"
#include "dma_intr.h"
#include "sys_intr.h"

/***********************************************************************************
 *                                   ȫ �� �� ��
 ***********************************************************************************/
/******************************* �� �� ȫ �� �� �� ***************************************/
static  XAxiDma AxiDma;
static XScuGic Intc; //GIC
u8 * TxBufferPtr = ( u8 * )TX_BUFFER_BASE;
u8 * RxBufferPtr = ( u8 * )RX_BUFFER_BASE;
extern u32 g_TX_count;
extern u32 g_RX_count;
int localPos = 0;

/***********************************************************************************
 *                                   �� �� �� ��
 ***********************************************************************************/

/***********************************************************************************
 *                                 �� �� ԭ ��
 ***********************************************************************************/
/***********************************************************************************
  * �������ƣ�	initIntrDMASys()��
  * ����˵����	DMA��ʼ����
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	�ޣ�
************************************************************************************/
int initIntrDMASys( void )
{
	DMA_Intr_Init( &AxiDma, 0 );//initial interrupt system
	Init_Intr_System( &Intc ); // initial DMA interrupt system
	Setup_Intr_Exception( &Intc );
	DMA_Setup_Intr_System( &Intc, &AxiDma, TX_INTR_ID, RX_INTR_ID );//setup dma interrpt system
	DMA_Intr_Enable( &Intc, &AxiDma );

	return 0;
}

/***********************************************************************************
  * �������ƣ�	TxDMA()��
  * ����˵����	DMA���ݷ��ͣ�
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	�ޣ�
************************************************************************************/
int TxDMA( const char * txData, int txLen )
{
	int Status;
//	static int pos = 0;

	TxDone = 0;

//	memcpy( &TxBufferPtr[ pos * txLen ], txData, txLen );
//	if( pos < 10 )
//	{
//		pos++;
//		return XST_SUCCESS;
//	}

	Xil_DCacheFlushRange( ( u32 )TxBufferPtr, MAX_PKT_LEN );
	Status = XAxiDma_SimpleTransfer( &AxiDma, ( u32 )TxBufferPtr,
			txLen, XAXIDMA_DMA_TO_DEVICE );

	if( Status != XST_SUCCESS ) return XST_FAILURE;

	while( !TxDone );

	g_TX_count += MAX_PKT_LEN;
//	pos = 0;

//	xil_printf( "DMA TX %d : ", txLen );
//	for( int Index = 0; Index < txLen; Index++ ) xil_printf( " %2X", TxBufferPtr[Index] ); xil_printf( "\r\n" );

	return XST_SUCCESS;
}

/***********************************************************************************
  * �������ƣ�	RxDMA()��
  * ����˵����	DMA���ݽ��գ�
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	�ޣ�
************************************************************************************/
int RxDMA()
{
	int Status;

	RxDone = 0;

	Xil_DCacheFlushRange( ( u32 )RxBufferPtr, MAX_PKT_LEN );

	Status = XAxiDma_SimpleTransfer( &AxiDma, ( u32 )RxBufferPtr,
			MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA );

	if( Status != XST_SUCCESS ) return XST_FAILURE;

	while( !RxDone );

	g_RX_count += MAX_PKT_LEN;

//	xil_printf( "DMA RX %d : %2X ", MAX_PKT_LEN, RxBufferPtr[0] );
//	for( int Index = ( MAX_PKT_LEN - 10 ); Index < MAX_PKT_LEN; Index++ ) xil_printf( " %2X", RxBufferPtr[Index] ); xil_printf( "\r\n" );

	return XST_SUCCESS;
}
