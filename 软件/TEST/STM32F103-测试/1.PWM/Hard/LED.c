#include "stm32f10x.h"
#include "LED.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能GPIO时钟
	
	GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO初�?�化结构�?
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_LED;
	GPIO_Init(GPIO_LED, &GPIO_InitStruct);
}

void LED_ON(u16 LEDx)
{
	GPIO_ResetBits(GPIO_LED,LEDx);//�?1
}

void LED_OFF(u16 LEDx)
{
	GPIO_SetBits(GPIO_LED,LEDx);//�?0
}

void LED_Turn(u16 LEDx)
{
	if(GPIO_ReadOutputDataBit(GPIO_LED,LEDx) == Bit_SET)
		GPIO_ResetBits(GPIO_LED,LEDx);
	else
		GPIO_SetBits(GPIO_LED,LEDx);
}
