#ifndef __OLED_H
#define __OLED_H

#include "Debug_log.h"
#include <math.h>
//头文件
#ifndef M_PI
#define M_PI (3.141592f)
#endif
typedef struct
{
    float ts;       //采样周期(s)
    float fc;       //截至频率(hz)
    float lastYn;   //上一次滤波值
    float alpha;    //滤波系数
} low_pass_filter_t;

void Filter(uint8_t data_in, float *data_out);

#endif
