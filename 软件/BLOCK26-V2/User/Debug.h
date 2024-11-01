#ifndef __DEBUG_H
#define __DEBUG_H

/*头文件*/
#include "air001xx_hal.h"
#include "Debug_log.h"
#include <stdlib.h>

/** Debug print 分类
 * Print	绝对输出：用于开机输出初始化信息
 * User		用户输出：进入系统后，由用户控制打开log输出
 * Test		调试输出：编译阶段控制是否输出，用于测试信息
 */
// 定义T_print宏，根据debug_enable的值决定是否输出
// #define P_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)

// extern uint8_t U_PRINT_ENABLE; // 用户打印使能
// #define U_printf(fmt, ...) if(U_PRINT_ENABLE){printf(fmt, ##__VA_ARGS__);}

// #define T_Debug_Enable 0
// #if T_Debug_Enable
// #define T_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
// #else
// #define T_printf(fmt, ...)
// #endif

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
void Debug_Init(void);
void Debug_RX_IT_Start(void);
void Debug_IT_Handler(void);
void Debug_Handler(void);

#endif
