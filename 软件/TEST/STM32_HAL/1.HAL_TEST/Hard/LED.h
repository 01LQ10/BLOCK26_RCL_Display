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

#ifndef _LED_H
#define _LED_H

#include "main.h"

#define LED1_GPIO_PORT GPIOA
#define LED1_PIN GPIO_PIN_12
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOA

#define LED2_GPIO_PORT GPIOC
#define LED2_PIN GPIO_PIN_13
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOC

#define LED3_GPIO_PORT GPIOC
#define LED3_PIN GPIO_PIN_4
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOC

#define LED4_GPIO_PORT GPIOC
#define LED4_PIN GPIO_PIN_5
#define LED4_GPIO_CLK RCC_APB2Periph_GPIOC

void LED_Init(void);

#define LED_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, (GPIO_PinState)1)
#define LED_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, (GPIO_PinState)0)

#endif
