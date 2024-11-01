#ifndef __LED_H
#define __LED_H

#include "air001xx_hal.h"

/*  -    R0  R1  R2  R3 +
 *  L0  [ 0   1   2   3 ]
 *  L1  [ 4   5   6   7 ]
 *  L2  [ 8   9   10  11]
 *  L3  [ 12  13  14  15]
 *
 */
// GPIOA 0,1
// GPIOB 5,6

extern TIM_HandleTypeDef LED_TimHandle;

#define LED_PORT_R0 GPIOF
#define LED_PIN_R0 GPIO_PIN_0
#define LED_PORT_R1 GPIOF
#define LED_PIN_R1 GPIO_PIN_1
#define LED_PORT_R2 GPIOA
#define LED_PIN_R2 GPIO_PIN_5
#define LED_PORT_R3 GPIOA
#define LED_PIN_R3 GPIO_PIN_6

#define LED_PORT_L0 GPIOA
#define LED_PIN_L0 GPIO_PIN_7
#define LED_PORT_L1 GPIOA
#define LED_PIN_L1 GPIO_PIN_1
#define LED_PORT_L2 GPIOA
#define LED_PIN_L2 GPIO_PIN_0
#define LED_PORT_L3 GPIOA
#define LED_PIN_L3 GPIO_PIN_4

#define LED_SET(X, I, V) HAL_GPIO_WritePin(LED_PORT_##X[I], LED_PIN_##X[I], (GPIO_PinState)V)


void LED_Init(void);
void LED_SCAN(void);
void LED_SET_BUFF(uint8_t x);
void LED_Blink(void);


#endif
