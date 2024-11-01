#ifndef __OLED_H
#define __OLED_H

#include "Debug_log.h"
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

void Filter(uint8_t data_in, float *data_out);

#endif
