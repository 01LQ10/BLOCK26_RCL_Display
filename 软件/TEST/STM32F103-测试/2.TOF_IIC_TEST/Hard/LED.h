#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h" 

#define GPIO_LED GPIOC
#define GPIO_Pin_LED GPIO_Pin_13
#define LED1 GPIO_Pin_13
#define LED2 GPIO_Pin_1
#define LED3 GPIO_Pin_2

void LED_Init(void);
void LED_ON(u16 LEDx);
void LED_OFF(u16 LEDx);
void LED_Turn(u16 LEDx);

#endif
