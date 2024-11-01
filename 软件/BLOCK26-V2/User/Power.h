#ifndef __POWER_H
#define __POWER_H

#include "Debug_log.h"
#include "string.h"
#include <stdlib.h>

#define TIMEOUT_tick 100 // ms

void Power_Init(void);

void Power_CT_Init(void);
void Power_CT_ON(void);
void Power_CT_OFF(void);

void Power_ADC_Init(void);
float Power_ADC_get_Vbat(void);

void Power_TimeOUT_Handle(void);
void Power_TimeOUT_clearCNT(void);

#define POW_CT_PORT GPIOB
#define POW_CT_PIN GPIO_PIN_2

#define POW_ADC_PORT GPIOB
#define POW_ADC_PIN GPIO_PIN_1
#define POW_ADC_CHANNEL ADC_CHANNEL_9

#endif
