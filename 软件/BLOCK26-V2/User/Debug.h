#ifndef __DEBUG_H
#define __DEBUG_H

/*ͷ�ļ�*/
#include "air001xx_hal.h"
#include "Debug_log.h"
#include <stdlib.h>

/** Debug print ����
 * Print	������������ڿ��������ʼ����Ϣ
 * User		�û����������ϵͳ�����û����ƴ�log���
 * Test		�������������׶ο����Ƿ���������ڲ�����Ϣ
 */
// ����T_print�꣬����debug_enable��ֵ�����Ƿ����
// #define P_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)

// extern uint8_t U_PRINT_ENABLE; // �û���ӡʹ��
// #define U_printf(fmt, ...) if(U_PRINT_ENABLE){printf(fmt, ##__VA_ARGS__);}

// #define T_Debug_Enable 0
// #if T_Debug_Enable
// #define T_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
// #else
// #define T_printf(fmt, ...)
// #endif

extern UART_HandleTypeDef Debug_UartHandle;

typedef union // ����ֵ
{
	uint8_t uc;	 // uchar
	uint16_t us; // ushort
	uint32_t ui; // uint
	int i;		 // int
	float f;	 // float
	double lf;	 // double
} OPT_arg_type;

/*��������*/
void Debug_Init(void);
void Debug_RX_IT_Start(void);
void Debug_IT_Handler(void);
void Debug_Handler(void);

#endif
