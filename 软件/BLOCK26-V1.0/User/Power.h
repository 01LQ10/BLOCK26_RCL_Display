#ifndef __POWER_H
#define __POWER_H

#include "air001xx_hal.h"
#include "string.h"
#include <stdlib.h>

#define TIMEOUT_tick 100 // ms

void Power_init(void);
void Power_TimeOUT_Handle(void);
void Power_TimeOUT_clearCNT(void);

#endif
