#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FLASH_USER_START_ADDR 0x08007800
#define FLASH_DATA_SIZE 16 // uint32

#define BLOCK_TOP_DEFAULT 80
#define BLOCK_BOM_DEFAULT 2

#define FLASH_LOAD(X) FDATA.R.X = FDATA_R->R.X

union flash_DATA
{
    __UINT32_TYPE__ W[FLASH_DATA_SIZE];
    struct DATA_STR
    {
        int BLOCK_TOP;
        float BLOCK_BOM;
    } R;
};
union flash_DATA FDATA;                                                // flash数据变量
union flash_DATA *FDATA_R = (union flash_DATA *)FLASH_USER_START_ADDR; // 读指针

int main()
{
    // FDATA.R.BLOCK_BOM = 1.333;
    // FDATA.W[1] = 0x3faa9fbe;

    // printf("%x\n",(__UINT32_TYPE__)FDATA.R.BLOCK_TOP);
    // printf("%f\n",FDATA.R.BLOCK_BOM);

    float test;
    test = 23.2-25.0;
    printf("%u\n",(__UINT8_TYPE__)test);
    return 0;
}