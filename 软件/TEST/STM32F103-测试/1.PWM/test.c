#include "stdio.h"
#include <string.h>

typedef unsigned char u8;

#define iFilterLen 7
static u8 bArray_in[iFilterLen] = {0}, bArray_p = 0;
u8 bArray[iFilterLen];
/*************===  ��ֵ�˲�  ===************** */
void median_filter(u8 data_in, u8 *data_out)
{
    u8 i, j; // ѭ������
    unsigned char bTemp;

    // ѭ������
    if (bArray_p == iFilterLen)
        bArray_p = 0;
    bArray_in[bArray_p] = data_in;
    bArray_p++;

    // copy����ֹ����Ӱ��ԭ����
    memcpy(bArray, bArray_in, iFilterLen);
    // ��ð�ݷ��������������
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
    // ������ֵ
    if ((iFilterLen & 1) > 0) // ������������Ԫ�أ������м�һ��Ԫ��
    {
        *data_out = bArray[iFilterLen / 2];
    }
    else // ������ż����Ԫ�أ������м�����Ԫ��ƽ��ֵ
    {
        *data_out = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
    }
}

int main()
{
    char data_in, data_out;

    // �����������ݲ�������ֵ�˲�
    while (1)
    {
        printf("��������[113 �˳�]: ");
        scanf("%d", &data_in);

        if (data_in == 'q')
        { // 113
            break;
        }

        // ������ֵ�˲�
        median_filter(data_in, &data_out);

        // �����λ��
        printf("ԭʼ���飺");
        for (int i = 0; i < 7; i++)
        {
            printf("%d ", bArray_in[i]);
        }
        printf("\n");
        printf("�������飺");
        for (int i = 0; i < 7; i++)
        {
            printf("%d ", bArray[i]);
        }
        printf("\n");

        printf("��λ����%d       ��\n", data_out);
    }

    return 0;
}
