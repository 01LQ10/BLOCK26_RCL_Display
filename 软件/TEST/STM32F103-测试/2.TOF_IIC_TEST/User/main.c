#include "stm32f10x.h" // Device header
#include "stdio.h"
#include "LED.h"
#include "DELAY.h"
#include "Debug.h"
#include "TOF050.h"

int main(void)
{

	LED_Init();
	Debug_init();

	TOF050_init();

	// printf("%u",TIM2->CNT);

	while (1)
	{
		Debug_Handler();
		TOF050_Handler();
	}
}
