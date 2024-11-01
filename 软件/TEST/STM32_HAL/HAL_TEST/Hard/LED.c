/**
  ******************************************************************************
  * @file    LED.h
  * @author  LQ
  * @version V2.0
  * @date    2023-11-28
  * @brief   GPIOÇý¶¯LED
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "LED.h"


void LED_Init(void)
{   
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
