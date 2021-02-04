/***********************************************************************************
* 版权所有(C)2020, 广州海格通信集团股份有限公司.
* 项目代号:    S023
* 文件名称:    freeRTOSIP_user.c
* 运行平台:    GCC
* 编译环境:    GCC编译器
* 程序语言:    C
* 作       者:    张问谦
* 内容摘要:    freeRTOS IP模块用户接口文件
* 完成日期:    2020年11月
************************************************************************************/
/***********************************************************************************
 *                                   头 文 件
 ***********************************************************************************/
/********************************** 标准头文件 ****************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/********************************* 自定义头文件 ***************************************/
#include "freeRTOSIP_user.h"
#include "hub_queue.h"

/******************************* freeRTOS 头文件 ************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/***************************** freeRTOS+UDP 头文件 **********************************/
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/******************************** Xilinx 头文件 *************************************/
#include "xil_printf.h"

/***********************************************************************************
 *                                   宏 定 义
 ***********************************************************************************/
#define mainHOST_NAME				"RTOSDemo"
#define mainDEVICE_NICK_NAME		"ZYNQ_demo"
#define mainWait_FREQUENCY_MS			( 1000 / portTICK_PERIOD_MS )
#define LOCALPORT 9998

/***********************************************************************************
 *                                 全 局 变 量
 ***********************************************************************************/
static UBaseType_t ulNextRand;
/* The default IP and MAC address used by the demo.  The address configuration
defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
1 but a DHCP server could not be contacted.  See the online documentation for
more information. */
static const uint8_t ucIPAddress[ 4 ] = { configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 };
static const uint8_t ucNetMask[ 4 ] = { configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3 };
static const uint8_t ucGatewayAddress[ 4 ] = { configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3 };
static const uint8_t ucDNSServerAddress[ 4 ] = { configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3 };
const uint8_t ucMACAddress[ 6 ] = { configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 };
const BaseType_t xLogToStdout = pdTRUE, xLogToFile = pdFALSE, xLogToUDP = pdFALSE;

extern u32 g_RX_sock_count;

/***********************************************************************************
 *                                内 部 函 数 申 明
 ***********************************************************************************/
UBaseType_t uxRand( void );
void vLoggingPrintf( const char *pcFormatString, ... );
BaseType_t xApplicationDNSQueryHook( const char *pcName );
uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
													uint16_t usSourcePort,
													uint32_t ulDestinationAddress,
													uint16_t usDestinationPort );
static void prvMiscInitialisation( void );
static void prvSRand( UBaseType_t ulSeed );
static void prvUDPRXTask( void *pvParameters );

/***********************************************************************************
 *                                 函 数 原 型
 ***********************************************************************************/

void user_IPinit()
{
	prvMiscInitialisation();
	FreeRTOS_IPInit( ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress );
}

/***********************************************************************************
  * 函数名称：	vLoggingPrintf()；
  * 函数说明：	freeRTOS+UDP消息打印；
  * 输入参数：	无；
  * 输出参数 ：	无；
  * 返回参数 ：	无；
  * 备        注：	无；
************************************************************************************/
void vLoggingPrintf( const char *pcFormatString, ... )
{
	xil_printf( pcFormatString );
	xil_printf( "\r\n" );
}


#if( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )

	const char *pcApplicationHostnameHook( void )
	{
		/* Assign the name "FreeRTOS" to this network node.  This function will
		be called during the DHCP: the machine will be registered with an IP
		address plus this name. */
		return mainHOST_NAME;
	}

#endif

#if( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )

	BaseType_t xApplicationDNSQueryHook( const char *pcName )
	{
	BaseType_t xReturn;

		/* Determine if a name lookup is for this node.  Two names are given
		to this node: that returned by pcApplicationHostnameHook() and that set
		by mainDEVICE_NICK_NAME. */
		if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )
		{
			xReturn = pdPASS;
		}
		else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
		{
			xReturn = pdPASS;
		}
		else
		{
			xReturn = pdFAIL;
		}

		return xReturn;
	}

#endif

uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
													uint16_t usSourcePort,
													uint32_t ulDestinationAddress,
													uint16_t usDestinationPort )
{
	( void ) ulSourceAddress;
	( void ) usSourcePort;
	( void ) ulDestinationAddress;
	( void ) usDestinationPort;

	return uxRand();
}

UBaseType_t uxRand( void )
{
	const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;

	/* Utility function to generate a pseudo random number. */

	ulNextRand = ( ulMultiplier * ulNextRand ) + ulIncrement;

	return( ( int ) ( ulNextRand >> 16UL ) & 0x7fffUL );
}

BaseType_t xApplicationGetRandomNumber( uint32_t *pulNumber )
{
	 *( pulNumber ) = uxRand();

	return pdTRUE;
}

/* Called by FreeRTOS+TCP when the network connects or disconnects.  Disconnect
events are only received if implemented in the MAC driver. */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
char cBuffer[ 16 ];
static BaseType_t xTasksAlreadyCreated = pdFALSE;

	/* If the network has just come up...*/
	if( eNetworkEvent == eNetworkUp )
	{
		/* Create the tasks that use the IP stack if they have not already been
		created. */
		if( xTasksAlreadyCreated == pdFALSE )
		{
//			/* See the comments above the definitions of these pre-processor
//			macros at the top of this file for a description of the individual
//			demo tasks. */
//			#if( mainCREATE_SIMPLE_UDP_CLIENT_SERVER_TASKS == 1 )
//			{
//				vStartSimpleUDPClientServerTasks( configMINIMAL_STACK_SIZE, mainSIMPLE_UDP_CLIENT_SERVER_PORT, mainSIMPLE_UDP_CLIENT_SERVER_TASK_PRIORITY );
//			}
//			#endif /* mainCREATE_SIMPLE_UDP_CLIENT_SERVER_TASKS */
//
//			#if( mainCREATE_TCP_ECHO_TASKS_SINGLE == 1 )
//			{
//				vStartTCPEchoClientTasks_SingleTasks( mainECHO_CLIENT_TASK_STACK_SIZE, mainECHO_CLIENT_TASK_PRIORITY );
//			}
//			#endif /* mainCREATE_TCP_ECHO_TASKS_SINGLE */
//
//			#if( mainCREATE_TCP_ECHO_SERVER_TASK == 1 )
//			{
//				vStartSimpleTCPServerTasks( mainECHO_SERVER_TASK_STACK_SIZE, mainECHO_SERVER_TASK_PRIORITY );
//			}
//			#endif
//
			xTaskCreate( prvUDPRXTask, "UDPRX", configMINIMAL_STACK_SIZE * 5, NULL,
					tskIDLE_PRIORITY + 2, NULL );

			xTasksAlreadyCreated = pdTRUE;
		}

		/* Print out the network configuration, which may have come from a DHCP
		server. */
		FreeRTOS_GetAddressConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress );
		FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
		xil_printf( "\n\rIP Address: %s\n\r", cBuffer );

		FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
		xil_printf( "Subnet Mask: %s\r\n", cBuffer );

		FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
		xil_printf( "Gateway Address: %s\r\n", cBuffer );

		FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
		xil_printf( "DNS Server Address: %s\r\n\r\n\r\n", cBuffer );
	}
}

static void prvUDPRXTask( void *pvParameters )
{
	int nRet;

	( void ) pvParameters;

	xil_printf( "\n\r prvUDPRXTask! \n\r" );
	xSocket_t xSocket = FREERTOS_INVALID_SOCKET;
	struct freertos_sockaddr xServer;
	struct freertos_sockaddr xClient;
//	char cLocalBuffer[ 2000 ];
	char *pucRecvUDPPayload;
	socklen_t xClientAddressLength = 0;

	xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
	if( xSocket != FREERTOS_INVALID_SOCKET)
	{
		/* Zero out the server structure. */
		memset( ( void * ) &xServer, 0x00, sizeof( xServer ) );

		/* Set family and port. */
		xServer.sin_port = FreeRTOS_htons( LOCALPORT );

		/* Bind the address to the socket. */
		if( FreeRTOS_bind( xSocket, &xServer, sizeof( xServer ) ) == -1 )
		{
			xil_printf("\n\r FreeRTOS_bind fail! xSocket = %d \n\r", xSocket );
			FreeRTOS_closesocket( xSocket );
			xSocket = FREERTOS_INVALID_SOCKET;
		}
	}
	else
	{
		xil_printf("\n\r FreeRTOS_socket fail! xSocket = %d \n\r", xSocket );
		vTaskDelete( NULL );
	}

	xil_printf("\n\r FreeRTOS_socket success! xSocket = %d \n\r", xSocket );

	while( 1 )
	{
//		nRet = FreeRTOS_recvfrom( xSocket, ( void * ) cLocalBuffer,
//				2000, 0, &xClient, &xClientAddressLength );
		nRet = FreeRTOS_recvfrom( xSocket, &pucRecvUDPPayload,
				0, FREERTOS_ZERO_COPY, &xClient, &xClientAddressLength );
		if( nRet <= 0 ) continue;
		g_RX_sock_count += nRet;
//		xil_printf( " nRet = %d \r\n", nRet );
//		for( int i = 0; i < nRet; i++ ) xil_printf( " %2x", pucRecvUDPPayload[i] ); xil_printf( "\r\n" );
//		lwip_CopyData2FPGA_BUF2( pucRecvUDPPayload, nRet );
		FreeRTOS_ReleaseUDPPayloadBuffer( ( void * )pucRecvUDPPayload );
	}
	vTaskDelete( NULL );
}

static void prvMiscInitialisation( void )
{
time_t xTimeNow = 0;
//uint32_t ulLoggingIPAddress;

//	ulLoggingIPAddress = FreeRTOS_inet_addr_quick( configECHO_SERVER_ADDR0, configECHO_SERVER_ADDR1, configECHO_SERVER_ADDR2, configECHO_SERVER_ADDR3 );
//	vLoggingInit( xLogToStdout, xLogToFile, xLogToUDP, ulLoggingIPAddress, configPRINT_PORT );

	/* Seed the random number generator. */
//    xTimeNow = time( ( time_t * )NULL );
    xTimeNow = xTaskGetTickCount();
	xil_printf( "Seed for randomiser: %lu\n\r", xTimeNow );
	prvSRand( ( uint32_t ) xTimeNow );
	xil_printf( "Random numbers: %08X %08X %08X %08X\n\r", ipconfigRAND32(), ipconfigRAND32(), ipconfigRAND32(), ipconfigRAND32() );
}

static void prvSRand( UBaseType_t ulSeed )
{
	/* Utility function to seed the pseudo random number generator. */
	ulNextRand = ulSeed;
}
