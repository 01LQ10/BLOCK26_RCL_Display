#ifndef __TOF050_H
#define __TOF050_H

#include "Debug_log.h"
#include <math.h>
#include <string.h>

#define TOF_ticks 50 // ms

extern TIM_HandleTypeDef TOF_TimHandle;

extern float BLOCK_TOP, BLOCK_BOM;

void TOF050_Init(uint8_t *bullet_num);
void TOF050_TIM_Init(void);

void TOF050_IT_Handler(void);
void TOF050_Handler(void);

void TOF050_Read_range(void);
uint8_t TOF050_GET_BULLET(float distance);

void SET_TOP(float TOP);
void SET_BOM(float BOM);



#endif
