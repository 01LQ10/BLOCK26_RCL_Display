/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#ifndef __OS_TASK_H
#define __OS_TASK_H

#include "main.h"
#include "LED.h"

#define OS_ticks 100													 // us
#define OS_ADD_TASK_US(TIM, FUN) OS_ADD_Task(TIM / OS_ticks, FUN)		 // 微秒
#define OS_ADD_TASK_MS(TIM, FUN) OS_ADD_Task(TIM * 1000 / OS_ticks, FUN) // 毫秒

extern TIM_HandleTypeDef OS_TimHandle;

typedef struct
{
	uint8_t Run;			// 任务状态：Run/Stop
	uint16_t TIMCount;		// 定时计数器
	uint16_t TRITime;		// 重载计数器
	void (*TaskHook)(void); // 任务函数
} TASK_COMPONENTS;

void OS_TIM_init(void);
void OS_ADD_Task(uint16_t TIM, void (*FUN)(void));
void OS_Task_Marks_Handler(void);
void OS_Task_Pro_Handler(void);

#endif
