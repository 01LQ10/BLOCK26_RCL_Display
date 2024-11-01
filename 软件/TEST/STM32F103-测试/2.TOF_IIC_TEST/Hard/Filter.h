#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"
#include "stdio.h"
#include <string.h>
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

void Filter(u8 data_in, float *data_out);

#endif
