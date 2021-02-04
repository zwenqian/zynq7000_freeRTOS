/***********************************************************************************
* ��Ȩ����(C)2020, ���ݺ���ͨ�ż��Źɷ����޹�˾.
* ��Ŀ����:    S023
* �ļ�����:    freeRTOS_user.c
* ����ƽ̨:    GCC
* ���뻷��:    GCC������
* ��������:    C
* ��       ��:    ����ǫ
* ����ժҪ:    freeRTOS�û��ӿ��ļ�
* �������:    2020��11��
************************************************************************************/
/***********************************************************************************
 *                                   ͷ �� ��
 ***********************************************************************************/
/********************************** ��׼ͷ�ļ� ****************************************/
#include <stdio.h>
#include <limits.h>

/********************************* �Զ���ͷ�ļ� ***************************************/
#include "freeRTOS_user.h"

/******************************* freeRTOS ͷ�ļ� ************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/******************************** Xilinx ͷ�ļ� *************************************/
#include "xparameters.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xuartps_hw.h"

/***********************************************************************************
 *                                 ȫ �� �� ��
 ***********************************************************************************/
XScuGic xInterruptController;
XScuWdt xWatchDogInstance;

/***********************************************************************************
 *                                �� �� �� �� �� ��
 ***********************************************************************************/
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );

void vAssertCalled( const char * pcFile, unsigned long ulLine );
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
void vInitialiseTimerForRunTimeStats( void );
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
		StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/***********************************************************************************
 *                                 �� �� ԭ ��
 ***********************************************************************************/
/***********************************************************************************
  * �������ƣ�	prvSetupHardware()��
  * ����˵����	Ӳ����ʼ����
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	�ޣ�
************************************************************************************/
void prvSetupHardware( void )
{
BaseType_t xStatus;
XScuGic_Config *pxGICConfig;

	/* Ensure no interrupts execute while the scheduler is in an inconsistent
	state.  Interrupts are automatically enabled when the scheduler is
	started. */
	portDISABLE_INTERRUPTS();

	/* Obtain the configuration of the GIC. */
	pxGICConfig = XScuGic_LookupConfig( XPAR_SCUGIC_SINGLE_DEVICE_ID );

	/* Sanity check the FreeRTOSConfig.h settings are correct for the
	hardware. */
	configASSERT( pxGICConfig );
	configASSERT( pxGICConfig->CpuBaseAddress == ( configINTERRUPT_CONTROLLER_BASE_ADDRESS + configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET ) );
	configASSERT( pxGICConfig->DistBaseAddress == configINTERRUPT_CONTROLLER_BASE_ADDRESS );

	/* Install a default handler for each GIC interrupt. */
	xStatus = XScuGic_CfgInitialize( &xInterruptController, pxGICConfig, pxGICConfig->CpuBaseAddress );
	configASSERT( xStatus == XST_SUCCESS );
	( void ) xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* Initialise the LED port. */
//	vParTestInitialise();

	/* The Xilinx projects use a BSP that do not allow the start up code to be
	altered easily.  Therefore the vector table used by FreeRTOS is defined in
	FreeRTOS_asm_vectors.S, which is part of this project.  Switch to use the
	FreeRTOS vector table. */
	vPortInstallFreeRTOSVectorTable();

	/* Initialise UART for use with QEMU. */
//	XUartPs_ResetHw( 0xE0000000 );
//	XUartPs_WriteReg(0xE0000000, XUARTPS_CR_OFFSET,
//				((u32)XUARTPS_CR_RX_DIS | (u32)XUARTPS_CR_TX_EN |
//						(u32)XUARTPS_CR_STOPBRK));
}

/***********************************************************************************
  * �������ƣ�	vApplicationMallocFailedHook()��
  * ����˵����	Ӧ�ó����ڴ���䣻
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	�ޣ�
************************************************************************************/
void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/***********************************************************************************
  * �������ƣ�	vApplicationStackOverflowHook()��
  * ����˵����	Ӧ�ó����ջ����ص�������
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file.
************************************************************************************/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/***********************************************************************************
  * �������ƣ�	vApplicationIdleHook()��
  * ����˵����	Ӧ�ó�����лص�������
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file.
************************************************************************************/
void vApplicationIdleHook( void )
{
volatile size_t xFreeHeapSpace, xMinimumEverFreeHeapSpace;

	/* This is just a trivial example of an idle hook.  It is called on each
	cycle of the idle task.  It must *NOT* attempt to block.  In this case the
	idle task just queries the amount of FreeRTOS heap that remains.  See the
	memory management section on the http://www.FreeRTOS.org web site for memory
	management options.  If there is a lot of heap memory free then the
	configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
	RAM. */
	xFreeHeapSpace = xPortGetFreeHeapSize();
	xMinimumEverFreeHeapSpace = xPortGetMinimumEverFreeHeapSize();

	/* Remove compiler warning about xFreeHeapSpace being set but never used. */
	( void ) xFreeHeapSpace;
	( void ) xMinimumEverFreeHeapSpace;
}

/***********************************************************************************
  * �������ƣ�	vApplicationTickHook()��
  * ����˵����	Ӧ�ó���ʱ��ص���
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file.
************************************************************************************/
void vApplicationTickHook( void )
{
	#if( mainSELECTED_APPLICATION == 1 )
	{
		/* The full demo includes a software timer demo/test that requires
		prodding periodically from the tick interrupt. */
		vTimerPeriodicISRTests();

		/* Call the periodic queue overwrite from ISR demo. */
		vQueueOverwritePeriodicISRDemo();

		/* Call the periodic event group from ISR demo. */
		vPeriodicEventGroupsProcessing();

		/* Use task notifications from an interrupt. */
		xNotifyTaskFromISR();

		/* Use mutexes from interrupts. */
		vInterruptSemaphorePeriodicTest();

		/* Writes to stream buffer byte by byte to test the stream buffer trigger
		level functionality. */
		vPeriodicStreamBufferProcessing();

		/* Writes a string to a string buffer four bytes at a time to demonstrate
		a stream being sent from an interrupt to a task. */
		vBasicStreamBufferSendFromISR();

		#if( configUSE_QUEUE_SETS == 1 )
		{
			vQueueSetAccessQueueSetFromISR();
		}
		#endif

		/* Test flop alignment in interrupts - calling printf from an interrupt
		is BAD! */
		#if( configASSERT_DEFINED == 1 )
		{
		char cBuf[ 20 ];
		UBaseType_t uxSavedInterruptStatus;

			uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
			{
				sprintf( cBuf, "%1.3f", 1.234 );
			}
			portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

			configASSERT( strcmp( cBuf, "1.234" ) == 0 );
		}
		#endif /* configASSERT_DEFINED */
	}
	#endif
}

/***********************************************************************************
  * �������ƣ�	vAssertCalled()��
  * ����˵����	���ԣ�
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	�ޣ�
************************************************************************************/
void vAssertCalled( const char * pcFile, unsigned long ulLine )
{
volatile unsigned long ul = 0;

	( void ) pcFile;
	( void ) ulLine;

	taskENTER_CRITICAL();
	{
		/* Set ul to a non-zero value using the debugger to step out of this
		function. */
		while( ul == 0 )
		{
			portNOP();
		}
	}
	taskEXIT_CRITICAL();
}

/***********************************************************************************
  * �������ƣ�	vApplicationGetIdleTaskMemory()��
  * ����˵����	Ӧ�ó�����������ڴ��ȡ��
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task.��
************************************************************************************/
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/***********************************************************************************
  * �������ƣ�	vInitialiseTimerForRunTimeStats()��
  * ����˵����	��ʼ����ʱ��������ʱ��״̬��
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	�ޣ�
************************************************************************************/
void vInitialiseTimerForRunTimeStats( void )
{
XScuWdt_Config *pxWatchDogInstance;
uint32_t ulValue;
const uint32_t ulMaxDivisor = 0xff, ulDivisorShift = 0x08;

	 pxWatchDogInstance = XScuWdt_LookupConfig( XPAR_SCUWDT_0_DEVICE_ID );
	 XScuWdt_CfgInitialize( &xWatchDogInstance, pxWatchDogInstance, pxWatchDogInstance->BaseAddr );

	 ulValue = XScuWdt_GetControlReg( &xWatchDogInstance );
	 ulValue |= ulMaxDivisor << ulDivisorShift;
	 XScuWdt_SetControlReg( &xWatchDogInstance, ulValue );

	 XScuWdt_LoadWdt( &xWatchDogInstance, UINT_MAX );
	 XScuWdt_SetTimerMode( &xWatchDogInstance );
	 XScuWdt_Start( &xWatchDogInstance );
}

/***********************************************************************************
  * �������ƣ�	vApplicationGetTimerTaskMemory()��
  * ����˵����	Ӧ�ó����ȡ��ʱ�������ڴ棻
  * ���������	�ޣ�
  * ������� ��	�ޣ�
  * ���ز��� ��	�ޣ�
  * ��        ע��	configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task.��
************************************************************************************/
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
		StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
