#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"
#include "stdio.h"
#include <string.h>
#include <math.h>
//ͷ�ļ�
#ifndef M_PI
#define M_PI (3.141592f)
#endif
typedef struct
{
    float ts;       //��������(s)
    float fc;       //����Ƶ��(hz)
    float lastYn;   //��һ���˲�ֵ
    float alpha;    //�˲�ϵ��
} low_pass_filter_t;

void Filter(u8 data_in, float *data_out);

#endif
