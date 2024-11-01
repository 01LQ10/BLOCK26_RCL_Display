/*---------------------------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include "OS_Task.h"

#define Tasks_MAX 8
uint8_t Tasks_Mum = 0; // sizeof(Task_Comps) / sizeof(Task_Comps[0]);

static TASK_COMPONENTS Task_Comps[Tasks_MAX];

TIM_HandleTypeDef OS_TimHandle;

void OS_Init(void)
{
	/*��ʱ�� TIM16 ��ʼ��*/
	__HAL_RCC_TIM16_CLK_ENABLE();
	OS_TimHandle.Instance = TIM16;										  /* ѡ��TIM16 */
	OS_TimHandle.Init.Period = 100 - 1;									  /* �Զ���װ��ֵ 0.1ms*/
	OS_TimHandle.Init.Prescaler = 24 - 1;								  /* ��Ԥ��Ƶ */
	OS_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;			  /* ʱ�Ӳ���Ƶ */
	OS_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;					  /* ���ϼ��� */
	OS_TimHandle.Init.RepetitionCounter = 1 - 1;						  /* ���ظ����� */
	OS_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* �Զ���װ�ؼĴ���û�л��� */
	HAL_TIM_Base_Init(&OS_TimHandle);
}

void OS_IT_Start(void)
{
	HAL_TIM_Base_Start_IT(&OS_TimHandle);

	HAL_NVIC_SetPriority(TIM16_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(TIM16_IRQn);
	P_printf("OS Start\n");
}

void OS_ADD_Task(uint16_t TIM, void (*FUN)(void))
{
	static uint8_t offset = 0;
	if (Tasks_Mum < Tasks_MAX)
	{
		Task_Comps[Tasks_Mum].Run = 0;
		Task_Comps[Tasks_Mum].TIMCount = TIM + offset; // ������
		Task_Comps[Tasks_Mum].TRITime = TIM;
		Task_Comps[Tasks_Mum].TaskHook = FUN;
		Tasks_Mum++;
		offset += 23;
	}
	else
		printf("erro:task number Exceed the maximum\n");
}

void OS_DEL_Task(void (*FUN)(void))
{
	for (uint8_t i = 0; i < Tasks_Mum; i++)
	{
		if (Task_Comps[i].TaskHook == FUN)
		{
			__disable_irq();
			Tasks_Mum--;
			Task_Comps[i].Run = Task_Comps[Tasks_Mum].Run;
			Task_Comps[i].TIMCount = Task_Comps[Tasks_Mum].TIMCount;
			Task_Comps[i].TRITime = Task_Comps[Tasks_Mum].TRITime;
			Task_Comps[i].TaskHook = Task_Comps[Tasks_Mum].TaskHook;
			__enable_irq();
			return;
		}
	}
}

// ����: �����ǻص�����.
void OS_Task_Marks_Handler(void)
{
	uint8_t i;
	for (i = 0; i < Tasks_Mum; i++)
	{
		if (Task_Comps[i].TIMCount) /* If the time is not 0 */
		{
			Task_Comps[i].TIMCount--;		 /* Time counter decrement */
			if (Task_Comps[i].TIMCount == 0) /* If time arrives */
			{
				/*Resume the timer value and try again */
				Task_Comps[i].TIMCount = Task_Comps[i].TRITime;
				Task_Comps[i].Run = 1; /* The task can be run */
			}
		}
	}
}

// ����: ������ص�����.
void OS_Task_Pro_Handler(void)
{
	uint8_t i;
	for (i = 0; i < Tasks_Mum; i++)
	{
		if (Task_Comps[i].Run) /* If task can be run */
		{
			Task_Comps[i].Run = 0;	  /* Flag clear 0 */
			Task_Comps[i].TaskHook(); /* Run task */
		}
	}
}
