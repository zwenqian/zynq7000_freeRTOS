/***********************************************************************************
* 版权所有(C)2020, 广州海格通信集团股份有限公司.
* 项目代号:    S023
* 文件名称:    dmaUser.c
* 运行平台:    GCC
* 编译环境:    GCC编译器
* 程序语言:    C
* 作       者:    张问谦
* 内容摘要:    DMA用户接口模块；
* 完成日期:    2020年11月
************************************************************************************/
/***********************************************************************************
 *                                   头 文 件
 ***********************************************************************************/

/******************************** Xilinx 头文件 *************************************/
#include "xaxidma.h"
#include "xscugic.h"

/********************************* 自定义头文件 ***************************************/
#include "dmaUser.h"
#include "dma_intr.h"
#include "sys_intr.h"

/***********************************************************************************
 *                                   全 局 变 量
 ***********************************************************************************/
/******************************* 内 部 全 局 变 量 ***************************************/
static  XAxiDma AxiDma;
static XScuGic Intc; //GIC
u8 * TxBufferPtr = ( u8 * )TX_BUFFER_BASE;
u8 * RxBufferPtr = ( u8 * )RX_BUFFER_BASE;
extern u32 g_TX_count;
extern u32 g_RX_count;
int localPos = 0;

/***********************************************************************************
 *                                   函 数 申 明
 ***********************************************************************************/

/***********************************************************************************
 *                                 函 数 原 型
 ***********************************************************************************/
/***********************************************************************************
  * 函数名称：	initIntrDMASys()；
  * 函数说明：	DMA初始化；
  * 输入参数：	无；
  * 输出参数 ：	无；
  * 返回参数 ：	无；
  * 备        注：	无；
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
  * 函数名称：	TxDMA()；
  * 函数说明：	DMA数据发送；
  * 输入参数：	无；
  * 输出参数 ：	无；
  * 返回参数 ：	无；
  * 备        注：	无；
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
  * 函数名称：	RxDMA()；
  * 函数说明：	DMA数据接收；
  * 输入参数：	无；
  * 输出参数 ：	无；
  * 返回参数 ：	无；
  * 备        注：	无；
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
