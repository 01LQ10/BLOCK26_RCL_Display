#ifndef __LED_H
#define __LED_H

#include "Debug_log.h"

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

#define LED_SET(X, I, V) HAL_GPIO_WritePin(LED_PORT_##X[I], LED_PIN_##X[I], (GPIO_PinState)V)

void LED_Init(void);
void LED_Scan_IT_Start(void);
void LED_SCAN(void);

void LED_Set_Buff(uint16_t buff);

void LED_Show_Single(uint8_t n);
void LED_Show_Add(uint8_t n);
void LED_Show_Remove(uint8_t n);
void LED_Show_Num(uint8_t n);
void LED_Show_Clear(void);

void LED_Blink(void);
void LED_Flow(void);

#define LED_PORT_R0 GPIOA
#define LED_PIN_R0 GPIO_PIN_13
#define LED_PORT_R1 GPIOA
#define LED_PIN_R1 GPIO_PIN_5
#define LED_PORT_R2 GPIOA
#define LED_PIN_R2 GPIO_PIN_6
#define LED_PORT_R3 GPIOA
#define LED_PIN_R3 GPIO_PIN_14

#define LED_PORT_L0 GPIOA
#define LED_PIN_L0 GPIO_PIN_0
#define LED_PORT_L1 GPIOA
#define LED_PIN_L1 GPIO_PIN_1
#define LED_PORT_L2 GPIOA
#define LED_PIN_L2 GPIO_PIN_4
#define LED_PORT_L3 GPIOA
#define LED_PIN_L3 GPIO_PIN_7

#endif
