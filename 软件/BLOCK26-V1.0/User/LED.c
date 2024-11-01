#include "LED.h"

GPIO_TypeDef *LED_PORT_R[4] = {LED_PORT_R0, LED_PORT_R1, LED_PORT_R2, LED_PORT_R3};
uint16_t LED_PIN_R[4] = {LED_PIN_R0, LED_PIN_R1, LED_PIN_R2, LED_PIN_R3};
GPIO_TypeDef *LED_PORT_L[4] = {LED_PORT_L0, LED_PORT_L1, LED_PORT_L2, LED_PORT_L3};
uint16_t LED_PIN_L[4] = {LED_PIN_L0, LED_PIN_L1, LED_PIN_L2, LED_PIN_L3};

uint16_t LED_BUFF = 0;

TIM_HandleTypeDef LED_TimHandle;

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOA_CLK_ENABLE(); /* ʹ��GPIOAʱ�� */
	__HAL_RCC_GPIOF_CLK_ENABLE(); /* ʹ��GPIOBʱ�� */

	/* GPIO��ʼ�� */
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	  /* ������� */
	GPIO_InitStruct.Pull = GPIO_NOPULL;			  /* ���� */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO�ٶ� */

	for (uint8_t i = 0; i < 4; i++)
	{
		GPIO_InitStruct.Pin = LED_PIN_R[i];
		HAL_GPIO_Init(LED_PORT_R[i], &GPIO_InitStruct);
		GPIO_InitStruct.Pin = LED_PIN_L[i];
		HAL_GPIO_Init(LED_PORT_L[i], &GPIO_InitStruct);

		HAL_GPIO_WritePin(LED_PORT_R[i], LED_PIN_R[i], (GPIO_PinState)0);
		HAL_GPIO_WritePin(LED_PORT_L[i], LED_PIN_L[i], (GPIO_PinState)1);
	}

	/*��ʱ�� TIM16 ��ʼ��*/
	__HAL_RCC_TIM14_CLK_ENABLE();
	LED_TimHandle.Instance = TIM14;										   /* ѡ��TIM16 */
	LED_TimHandle.Init.Period = 500 - 1;								   /* �Զ���װ��ֵ 0.5ms*/
	LED_TimHandle.Init.Prescaler = 24 - 1;								   /* ��Ԥ��Ƶ */
	LED_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;			   /* ʱ�Ӳ���Ƶ */
	LED_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;				   /* ���ϼ��� */
	LED_TimHandle.Init.RepetitionCounter = 1 - 1;						   /* ���ظ����� */
	LED_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* �Զ���װ�ؼĴ���û�л��� */
	HAL_TIM_Base_Init(&LED_TimHandle);
	HAL_TIM_Base_Start_IT(&LED_TimHandle);

	HAL_NVIC_SetPriority(TIM14_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(TIM14_IRQn);
}



void LED_SCAN(void)
{
	static uint8_t i = 0;
	static uint16_t mask = 0x0001;

	if (i == 0)
	{
		LED_SET(R, 15 % 4, 0);
		LED_SET(L, 15 / 4, 1);
	}
	else
	{
		LED_SET(R, (i - 1) % 4, 0);
		LED_SET(L, (i - 1) / 4, 1);
	}

	if ((LED_BUFF & mask) != 0)
	{
		LED_SET(R, i % 4, 1);
		LED_SET(L, i / 4, 0);
	}

	i++;
	mask <<= 1;
	if (i == 16)
	{
		i = 0;
		mask = 0x0001;
	}
}

void LED_Blink(void)
{
	HAL_NVIC_DisableIRQ(TIM14_IRQn);
	LED_SET(R, 0, 1);
	LED_SET(R, 1, 0);
	LED_SET(R, 2, 0);
	LED_SET(R, 3, 0);
	LED_SET(L, 0, 0);
	LED_SET(L, 1, 1);
	LED_SET(L, 2, 1);
	LED_SET(L, 3, 1);
	HAL_Delay(200);
	LED_SET(R, 0, 0); // LED_SET(R, 1, 0);LED_SET(R, 2, 0);LED_SET(R, 3, 0);
	LED_SET(L, 0, 1); // LED_SET(L, 1, 1);LED_SET(L, 2, 1);LED_SET(L, 3, 1);
	HAL_Delay(200);
	LED_SET(R, 0, 1);
	LED_SET(R, 1, 0);
	LED_SET(R, 2, 0);
	LED_SET(R, 3, 0);
	LED_SET(L, 0, 0);
	LED_SET(L, 1, 1);
	LED_SET(L, 2, 1);
	LED_SET(L, 3, 1);
	HAL_Delay(200);
	LED_SET(R, 0, 0); // LED_SET(R, 1, 0);LED_SET(R, 2, 0);LED_SET(R, 3, 0);
	LED_SET(L, 0, 1); // LED_SET(L, 1, 1);LED_SET(L, 2, 1);LED_SET(L, 3, 1);
	HAL_NVIC_EnableIRQ(TIM14_IRQn);
}

void LED_SET_BUFF(uint8_t x)
{
	if (x <= 16)
	{
		LED_BUFF = ~(0xFFFF >> x);
	}
}
