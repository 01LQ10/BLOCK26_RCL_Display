#ifndef __DEBUG_LOG_H
#define __DEBUG_LOG_H

/*头文件*/
#include "air001xx_hal.h"

/** Debug print 分类 
 * Print	绝对输出：用于开机输出初始化信息
 * User		用户输出：进入系统后，由用户控制打开log输出
 * Test		调试输出：编译阶段控制是否输出，用于测试信息
 */
// 定义T_print宏，根据debug_enable的值决定是否输出
#define P_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)

extern uint8_t U_PRINT_ENABLE; // 用户打印使能
#define U_printf(fmt, ...) if(U_PRINT_ENABLE){printf(fmt, ##__VA_ARGS__);}

#define T_Debug_Enable 1
#if T_Debug_Enable
#define T_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define T_printf(fmt, ...)
#endif

#endif

