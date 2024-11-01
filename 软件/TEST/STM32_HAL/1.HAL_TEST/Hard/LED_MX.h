/*
 * LED_MX.h
 *
 *  Created on: 2024Äê9ÔÂ30ÈÕ
 *      Author: 33088
 */

#ifndef USER_LED_MX_H_
#define USER_LED_MX_H_

#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SHOW_ROW_NUM 16
#define SHOW_COL_NUM 4 // x8bit

#define BUFF_ROW_NUM 16
#define BUFF_COL_NUM 12 // x8bit

#define A1_GPIO_PORT GPIOA
#define A1_PIN GPIO_PIN_0

#define A0_GPIO_PORT GPIOA
#define A0_PIN GPIO_PIN_1

#define A2_GPIO_PORT GPIOA
#define A2_PIN GPIO_PIN_2

#define A3_GPIO_PORT GPIOA
#define A3_PIN GPIO_PIN_3

#define LE_GPIO_PORT GPIOA
#define LE_PIN GPIO_PIN_4

#define CLK_GPIO_PORT GPIOA
#define CLK_PIN GPIO_PIN_5

#define SDI_GPIO_PORT GPIOA
#define SDI_PIN GPIO_PIN_6

#define OE_GPIO_PORT GPIOA
#define OE_PIN GPIO_PIN_7

#define LED_GPIO_PORT GPIOC
#define LED_PIN GPIO_PIN_13

#define SET(P, X) HAL_GPIO_WritePin(P##_GPIO_PORT, P##_PIN, (GPIO_PinState)X)

#define LED_MX_Show_Enable() SET(OE, 0)
#define LED_MX_Show_Disable() SET(OE, 1)

#define SHOW_BUFF_MODE 0
#define FONT_BUFF_MODE 1

void LED_MX_init(void);
void LED_MX_Send_ROW_addr(uint8_t addr);
void LED_MX_Send_COL_data(uint8_t data);
void LED_MX_Latch(void);
void LED_MX_Scan(uint8_t mode);

uint8_t Combine(uint8_t L, uint8_t H, uint8_t w /*Æ«ÒÆ*/);
uint8_t get_font_addr(uint8_t *str, uint32_t *offset_out);
void LED_MX_load_font_buff(uint8_t *STR);
void LED_MX_load_show_buff(uint16_t w /*Æ«ÒÆ*/);

#endif /* USER_LED_MX_H_ */
