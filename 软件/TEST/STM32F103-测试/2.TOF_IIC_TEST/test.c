#include "stdio.h"
#include <string.h>

typedef unsigned char u8;

#define iFilterLen 7
static u8 bArray_in[iFilterLen] = {0}, bArray_p = 0;
u8 bArray[iFilterLen];
/*************===  中值滤波  ===************** */
void median_filter(u8 data_in, u8 *data_out)
{
    u8 i, j; // 循环变量
    unsigned char bTemp;

    // 循环更新
    if (bArray_p == iFilterLen)
        bArray_p = 0;
    bArray_in[bArray_p] = data_in;
    bArray_p++;

    // copy，防止排序影响原数组
    memcpy(bArray, bArray_in, iFilterLen);
    // 用冒泡法对数组进行排序
    for (j = 0; j < iFilterLen - 1; j++)
    {
        for (i = 0; i < iFilterLen - j - 1; i++)
        {
            if (bArray[i] > bArray[i + 1])
            {
                bTemp = bArray[i];
                bArray[i] = bArray[i + 1];
                bArray[i + 1] = bTemp;
            }
        }
    }
    // 计算中值
    if ((iFilterLen & 1) > 0) // 数组有奇数个元素，返回中间一个元素
    {
        *data_out = bArray[iFilterLen / 2];
    }
    else // 数组有偶数个元素，返回中间两个元素平均值
    {
        *data_out = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
    }
}

int main()
{
    char data_in, data_out;

    // 不断输入数据并进行中值滤波
    while (1)
    {
        printf("输入数据[113 退出]: ");
        scanf("%d", &data_in);

        if (data_in == 'q')
        { // 113
            break;
        }

        // 进行中值滤波
        median_filter(data_in, &data_out);

        // 输出中位数
        printf("原始数组：");
        for (int i = 0; i < 7; i++)
        {
            printf("%d ", bArray_in[i]);
        }
        printf("\n");
        printf("排序数组：");
        for (int i = 0; i < 7; i++)
        {
            printf("%d ", bArray[i]);
        }
        printf("\n");

        printf("中位数：%d       ↑\n", data_out);
    }

    return 0;
}
