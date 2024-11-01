#include <stdio.h>
#include <stdlib.h>

// �ȽϺ���������
int compare(const void *a, const void *b) {
    return *(char *)a - *(char *)b;
}
// ��ֵ�˲�����
void median_filter(char data_in, char *data_out) {
    static char data[8];  // ����Ϊ 8 �ľ�̬����

    // ����������������
    for (int i = 0; i < 7; i++) {
        data[i] = data[i + 1];
    }
    data[7] = data_in;

    // ����������
    qsort(data, 8, sizeof(char), compare);

    // ȡ��λ��
    *data_out = data[3];
}


int main() {
    char data_in, data_out;

    // �����������ݲ�������ֵ�˲�
    while (1) {
        printf("��������[113 �˳�]: ");
        scanf("%d", &data_in);

        if (data_in == 'q') { //113
            break;
        }

        // ������ֵ�˲�
        median_filter(data_in, &data_out);

        // �����λ��
        printf("��λ����%d\n", data_out);
    }

    return 0;
}
