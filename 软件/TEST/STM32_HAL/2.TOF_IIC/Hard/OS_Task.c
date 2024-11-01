/*---------------------------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include "OS_Task.h"

#define Tasks_MAX 8
uint8_t Tasks_Mum = 0; // sizeof(Task_Comps) / sizeof(Task_Comps[0]);

static TASK_COMPONENTS Task_Comps[Tasks_MAX];

TIM_HandleTypeDef OS_TimHandle;

void OS_TIM_init(void)
{
	/*��ʱ�� TIM3 ��ʼ��*/
	__HAL_RCC_TIM3_CLK_ENABLE();
	OS_TimHandle.Instance = TIM3;										  /* ѡ��TIM3 */
	OS_TimHandle.Init.Period = 100 - 1;								  /* �Զ���װ��ֵ 0.1ms*/
	OS_TimHandle.Init.Prescaler = 8 - 1;								  /* ��Ԥ��Ƶ */
	OS_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;			  /* ʱ�Ӳ���Ƶ */
	OS_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;					  /* ���ϼ��� */
	OS_TimHandle.Init.RepetitionCounter = 0;						  /* ���ظ����� */
	OS_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* �Զ���װ�ؼĴ���û�л��� */
	HAL_TIM_Base_Init(&OS_TimHandle);
	HAL_TIM_Base_Start_IT(&OS_TimHandle);

	HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void OS_ADD_Task(uint16_t TIM, void (*FUN)(void))
{
	if (Tasks_Mum < Tasks_MAX)
	{
		Task_Comps[Tasks_Mum].Run = 0;
		Task_Comps[Tasks_Mum].TIMCount = TIM;
		Task_Comps[Tasks_Mum].TRITime = TIM;
		Task_Comps[Tasks_Mum].TaskHook = FUN;
		Tasks_Mum++;
	}
	else
		printf("erro:task number Exceed the maximum\n");
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
