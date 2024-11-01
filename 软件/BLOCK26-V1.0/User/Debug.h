#ifndef __DEBUG_H
#define __DEBUG_H

/*头文件*/
#include "air001xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include "LED.h"
#include "TOF050.h"
#include "flash.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t; // 16 bits
typedef unsigned int uint32_t;	 // 32 bits

extern UART_HandleTypeDef Debug_UartHandle;

typedef union // 参数值
{
	uint8_t uc;	 // uchar
	uint16_t us; // ushort
	uint32_t ui; // uint
	int i;		 // int
	float f;	 // float
	double lf;	 // double
} OPT_arg_type;

/*函数声明*/
void Debug_init(void);
void Debug_IT_Handler(UART_HandleTypeDef *huart);
void Debug_Handler(void);

#endif

