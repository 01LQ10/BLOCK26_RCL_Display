#include "Filter.h"

#include <string.h>

low_pass_filter_t low_pass_filter_1 = {0}, low_pass_filter_2 = {0}; // 定义滤波器

uint8_t buffer[10] = {0};

// 源文件
/*******************************************************************************
 * @brief 初始化低通滤波器滤波系数
 * @param filter 滤波器
 * @param ts 采用周期 单位s
 * @return fc 截至频率 单位hz
 ******************************************************************************/
void Init_lowPass_alpha(low_pass_filter_t *const filter, const float ts, const float fc)
{
    float b = 2 * M_PI * fc * ts;
    filter->ts = ts;
    filter->fc = fc;
    filter->lastYn = 0;
    filter->alpha = b / (b + 1);
}

/*************===  平均值+数学期望  ===************** */
/*void filter_2(float *O_DISTANCE_FIL, uint8_t I_DISTANCE_RAW)
{
    static uint8_t BUFF[20] = {0}, BUFF_P = 0;
    float DISTANCE_AVG = 0, DISTANCE_VAR = 0, DISTANCE_FIL;

    if (I_DISTANCE_RAW > 255)
        I_DISTANCE_RAW = 255;

    BUFF[BUFF_P] = I_DISTANCE_RAW;
    BUFF_P++;
    if (BUFF_P >= 20)
        BUFF_P = 0;

    for (uint8_t i = 0; i < 20; i++)
    {
        DISTANCE_AVG += BUFF[i];
    }
    DISTANCE_AVG /= 20.0;

    for (uint8_t i = 0; i < 20; i++)
    {
        DISTANCE_VAR += pow((float)BUFF[i] - DISTANCE_AVG, 2) / 20.0; // 求方差
    }
}*/

/*************===  低通滤波  ===************** */
void Low_pass_filter(float data_in, float *data_out)
{
#define ts 0.01 // 采样周期(s)
#define fc 5.0  // 截至频率(hz)
#define b (2 * M_PI * fc * ts)
    static float lastYn;              // 上一次滤波值
    static float alpha = b / (b + 1); // 滤波系数
    float tem;
    tem = lastYn + (alpha * (data_in - lastYn));
    lastYn = tem;
    *data_out = tem;
}

/*************===  加权平均值滤波  ===************** */
void w_average_filter(uint8_t data_in, float *data_out)
{
#define FilterLen 5
    static uint8_t bArray_in[FilterLen] = {0}, bArray_p = 0;
    static float w[FilterLen] = {0.1, 0.2, 0.4, 0.2, 0.1};
    uint8_t i; // 循环变量
    float sum = 0;

    // 循环更新
    if (bArray_p == FilterLen)
        bArray_p = 0;
    bArray_in[bArray_p] = data_in;
    bArray_p++;

    for (i = 0; i < FilterLen; i++)
    {
        sum += (float)bArray_in[i] * w[i];
    }

    *data_out = sum;
}

/*************===  平均值滤波  ===************** */
void average_filter(uint8_t data_in, float *data_out)
{
#undef FilterLen
#define FilterLen 10
    static uint8_t bArray_in[FilterLen] = {0}, bArray_p = 0;
    uint8_t i; // 循环变量
    uint16_t sum = 0;
    // 循环更新
    if (bArray_p == FilterLen)
        bArray_p = 0;
    bArray_in[bArray_p] = data_in;
    bArray_p++;

    for (i = 0; i < FilterLen; i++)
    {
        sum += bArray_in[i];
    }

    *data_out = (float)sum / FilterLen;
}

/*************===  中值滤波  ===************** */
void median_filter(uint8_t data_in, uint8_t *data_out)
{
#undef FilterLen
#define FilterLen 5
    static uint8_t bArray_in[FilterLen] = {0}, bArray_p = 0;
    uint8_t bArray[FilterLen];
    uint8_t i, j; // 循环变量
    uint8_t bTemp, maxIndex, minIndex;

    // 循环更新
    if (bArray_p == FilterLen)
        bArray_p = 0;
    bArray_in[bArray_p] = data_in;
    bArray_p++;

    // copy，防止排序影响原数组
    memcpy(bArray, bArray_in, FilterLen);

    // 查找最大值和最小值下标，同时记录元素进行排序
    maxIndex = 0;
    minIndex = 0;
    for (i = 1; i < FilterLen; i++)
    {
        if (bArray[i] > bArray[maxIndex])
        {
            maxIndex = i;
        }
        if (bArray[i] < bArray[minIndex])
        {
            minIndex = i;
        }
    }

    // 从大到小排序
    for (i = 0; i < FilterLen - 1; i++)
    {
        for (j = i + 1; j < FilterLen; j++)
        {
            if (bArray[i] < bArray[j])
            {
                // 交换 arr[i] 和 arr[j]
                bTemp = bArray[i];
                bArray[i] = bArray[j];
                bArray[j] = bTemp;
            }
        }
    }

    /*计算中值*/
    if ((FilterLen & 1) > 0) // 数组有奇数个元素，返回中间一个元素
    {
        *data_out = bArray[FilterLen / 2];
    }
    else // 数组有偶数个元素，返回中间两个元素平均值
    {
        *data_out = (bArray[FilterLen / 2] + bArray[FilterLen / 2 + 1]) / 2;
    }

    /*将最大最小值替换为中值*/
    bArray_in[minIndex] = (*data_out+bArray_in[minIndex])/2;
    bArray_in[maxIndex] = (*data_out+bArray_in[maxIndex])/2;
}

/*************===  最终算法  ===************** */
void Filter(uint8_t data_in, float *data_out)
{
#define sampling_period 5
    static uint8_t median_filter_out;
    static float average_filter_out, Low_pass_filter_out;
    static uint8_t sampling_num = 0;
#define erro_period 5
    static uint8_t erro_num = 0;
    static float erro_last, erro;

    // if (sampling_num == sampling_period)
    // {
    median_filter(data_in, &median_filter_out);
    average_filter(median_filter_out, &average_filter_out);
    // w_average_filter(median_filter_out, &w_average_filter_out);
    // Low_pass_filter(average_filter_out, &Low_pass_filter_out);
    *data_out = average_filter_out;
    sampling_num = 0;

    if (erro_num == erro_period)
    {
        erro = average_filter_out - erro_last;
        erro_last = average_filter_out;
        erro_num = 0;
    }
    erro_num++;
    // }
    // sampling_num++;

    // printf("any:%u,%u,%.2f,%.2f", data_in, median_filter_out, average_filter_out, erro);
}
