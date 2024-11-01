#ifndef __CRC16_H
#define __CRC16_H

#include "air001xx_hal.h"
#include "string.h"
#include <stdlib.h>

uint16_t CRC16_calculate(uint8_t *puchMsg, int usDataLen);

#endif
