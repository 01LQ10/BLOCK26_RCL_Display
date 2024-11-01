#ifndef __HALL_H
#define __HALL_H

#include "Debug_log.h"

void Hall_Init(void);
uint8_t Hall_get_state(void);

typedef enum
{
    /*HALL:无磁性时输出1，有磁性时输出0*/
    BOX_IN = 0,
    Box_OUT = 1
}Hall_state;

#define Hall_PORT GPIOB
#define Hall_PIN GPIO_PIN_0

#endif
