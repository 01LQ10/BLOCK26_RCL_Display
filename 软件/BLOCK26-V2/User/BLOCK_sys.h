#ifndef __BLOCK_SYS_H
#define __BLOCK_SYS_H

#include "air001xx_hal.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>

#include "Debug.h"
#include "OS_Task.h"
#include "Power.h"
#include "LED.h"
#include "KEY.h"
#include "TOF050.h"
#include "Hall.h"

typedef struct BLOCK_sys
{
    uint8_t bullet_num;
    Hall_state box_state;

    uint16_t time_out;
    float top_distance;
    uint8_t start_led_effect;
} BLOCK_t;

void BLOCK_Sys_Init(void);
void BLOCK_Sys_Start(void);

#endif
