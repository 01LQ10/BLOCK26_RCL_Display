#ifndef __DEBUG_H
#define __DEBUG_H

#include "stm32f10x.h"
#include "stdio.h"
#include "LED.h"
#include "DELAY.h"
#include "Filter.h"

void Debug_init(void);
void Debug_Handler(void);

#endif
