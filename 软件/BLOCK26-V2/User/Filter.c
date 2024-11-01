#include "Filter.h"

#include <string.h>

low_pass_filter_t low_pass_filter_1 = {0}, low_pass_filter_2 = {0}; // �����˲���

uint8_t buffer[10] = {0};

// Դ�ļ�
/*******************************************************************************
 * @brief ��ʼ����ͨ�˲����˲�ϵ��
 * @param filter �˲���
 * @param ts �������� ��λs
 * @return fc ����Ƶ�� ��λhz
 ******************************************************************************/
void Init_lowPass_alpha(low_pass_filter_t *const filter, const float ts, const float fc)
{
    float b = 2 * M_PI * fc * ts;
    filter->ts = ts;
    filter->fc = fc;
    filter->lastYn = 0;
    filter->alpha = b / (b + 1);
}

/*************===  ƽ��ֵ+��ѧ����  ===************** */
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
        DISTANCE_VAR += pow((float)BUFF[i] - DISTANCE_AVG, 2) / 20.0; // �󷽲�
    }
}*/

/*************===  ��ͨ�˲�  ===************** */
void Low_pass_filter(float data_in, float *data_out)
{
#define ts 0.01 // ��������(s)
#define fc 5.0  // ����Ƶ��(hz)
#define b (2 * M_PI * fc * ts)
    static float lastYn;              // ��һ���˲�ֵ
    static float alpha = b / (b + 1); // �˲�ϵ��
    float tem;
    tem = lastYn + (alpha * (data_in - lastYn));
    lastYn = tem;
    *data_out = tem;
}

/*************===  ��Ȩƽ��ֵ�˲�  ===************** */
void w_average_filter(uint8_t data_in, float *data_out)
{
#define FilterLen 5
    static uint8_t bArray_in[FilterLen] = {0}, bArray_p = 0;
    static float w[FilterLen] = {0.1, 0.2, 0.4, 0.2, 0.1};
    uint8_t i; // ѭ������
    float sum = 0;

    // ѭ������
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

/*************===  ƽ��ֵ�˲�  ===************** */
void average_filter(uint8_t data_in, float *data_out)
{
#undef FilterLen
#define FilterLen 10
    static uint8_t bArray_in[FilterLen] = {0}, bArray_p = 0;
    uint8_t i; // ѭ������
    uint16_t sum = 0;
    // ѭ������
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

/*************===  ��ֵ�˲�  ===************** */
void median_filter(uint8_t data_in, uint8_t *data_out)
{
#undef FilterLen
#define FilterLen 5
    static uint8_t bArray_in[FilterLen] = {0}, bArray_p = 0;
    uint8_t bArray[FilterLen];
    uint8_t i, j; // ѭ������
    uint8_t bTemp, maxIndex, minIndex;

    // ѭ������
    if (bArray_p == FilterLen)
        bArray_p = 0;
    bArray_in[bArray_p] = data_in;
    bArray_p++;

    // copy����ֹ����Ӱ��ԭ����
    memcpy(bArray, bArray_in, FilterLen);

    // �������ֵ����Сֵ�±꣬ͬʱ��¼Ԫ�ؽ�������
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

    // �Ӵ�С����
    for (i = 0; i < FilterLen - 1; i++)
    {
        for (j = i + 1; j < FilterLen; j++)
        {
            if (bArray[i] < bArray[j])
            {
                // ���� arr[i] �� arr[j]
                bTemp = bArray[i];
                bArray[i] = bArray[j];
                bArray[j] = bTemp;
            }
        }
    }

    /*������ֵ*/
    if ((FilterLen & 1) > 0) // ������������Ԫ�أ������м�һ��Ԫ��
    {
        *data_out = bArray[FilterLen / 2];
    }
    else // ������ż����Ԫ�أ������м�����Ԫ��ƽ��ֵ
    {
        *data_out = (bArray[FilterLen / 2] + bArray[FilterLen / 2 + 1]) / 2;
    }

    /*�������Сֵ�滻Ϊ��ֵ*/
    bArray_in[minIndex] = (*data_out+bArray_in[minIndex])/2;
    bArray_in[maxIndex] = (*data_out+bArray_in[maxIndex])/2;
}

/*************===  �����㷨  ===************** */
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
