#include <stdio.h>
#include <stdlib.h>

// 比较函数（升序）
int compare(const void *a, const void *b) {
    return *(char *)a - *(char *)b;
}
// 中值滤波函数
void median_filter(char data_in, char *data_out) {
    static char data[8];  // 长度为 8 的静态数组

    // 将新数据移入数组
    for (int i = 0; i < 7; i++) {
        data[i] = data[i + 1];
    }
    data[7] = data_in;

    // 对数组排序
    qsort(data, 8, sizeof(char), compare);

    // 取中位数
    *data_out = data[3];
}


int main() {
    char data_in, data_out;

    // 不断输入数据并进行中值滤波
    while (1) {
        printf("输入数据[113 退出]: ");
        scanf("%d", &data_in);

        if (data_in == 'q') { //113
            break;
        }

        // 进行中值滤波
        median_filter(data_in, &data_out);

        // 输出中位数
        printf("中位数：%d\n", data_out);
    }

    return 0;
}
