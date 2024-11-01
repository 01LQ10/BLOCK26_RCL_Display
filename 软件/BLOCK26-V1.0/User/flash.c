#include "flash.h"

#define DATA FDATA.W

union flash_DATA FDATA;                                                // flash数据变量
union flash_DATA *FDATA_R = (union flash_DATA *)FLASH_USER_START_ADDR; // 读指针

#define FLASH_SHOW(X, T) printf(#X "->" #T "\n", FDATA_R->R.X)
void flash_show(void)
{
    FLASH_SHOW(BLOCK_BOM, % f);
    FLASH_SHOW(BLOCK_TOP, % f);
}

#define FLASH_LOAD(X) FDATA.R.X = FDATA_R->R.X
void flash_load(void)
{
    uint8_t i;
    for (i = 0; i < FLASH_DATA_SIZE; i++)
    {
        FDATA.W[i] = 0X55;
    }

    // 载入flash数据到变量
    FLASH_LOAD(BLOCK_BOM);
    // FLASH_LOAD(BLOCK_TOP);
    FDATA.R.BLOCK_TOP = 200;
}

// 存储数据到flash
void flash_save(void)
{
    // 写入数据
    // __disable_irq();
    HAL_FLASH_Unlock(); /* 解锁FLASH */

    APP_FlashErase();   /* 擦除FLASH */
    APP_FlashBlank();   /* 查空FLASH */
    APP_FlashProgram(); /* 写FLASH */

    HAL_FLASH_Lock(); /* 锁定FLASH */

    APP_FlashVerify(); /* 校验FLASH */
    // __enable_irq();
}
/**
 * @brief  擦除FLASH
 * @param  无
 * @retval 无
 */
static void APP_FlashErase(void)
{
    uint32_t SECTORError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORERASE;         /* 擦写类型FLASH_TYPEERASE_PAGEERASE=Page擦, FLASH_TYPEERASE_SECTORERASE=Sector擦 */
    EraseInitStruct.SectorAddress = FLASH_USER_START_ADDR;           /* 擦写起始地址  */
    EraseInitStruct.NbSectors = 1;                                   /* 需要擦写的扇区数量  */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) /* 执行sector擦除,SECTORError返回擦写错误的sector,返回0xFFFFFFFF,表示擦写成功  */
    {
        printf("FlashErase error\n");
        Error_Handler();
    }
    printf("FlashErase OK\n");
}

/**
 * @brief  写入FLASH
 * @param  无
 * @retval 无
 */
static void APP_FlashProgram(void)
{
    uint32_t flash_program_start = FLASH_USER_START_ADDR;                /* flash写起始地址 */
    uint32_t flash_program_end = (FLASH_USER_START_ADDR + sizeof(DATA)); /* flash写结束地址 */
    uint32_t *src = (uint32_t *)DATA;                                    /* 数组指针 */

    while (flash_program_start < flash_program_end)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK) /* 执行Program */
        {
            flash_program_start += FLASH_PAGE_SIZE; /* flash起始指针指向第一个page */
            src += FLASH_PAGE_SIZE / 4;             /* 更新数据指针 */
        }
    }
    printf("FlashProgram OK\n");
}

/**
 * @brief  查空FLASH
 * @param  无
 * @retval 无
 */
static void APP_FlashBlank(void)
{
    uint32_t addr = 0;

    while (addr < sizeof(DATA))
    {
        if (0xFFFFFFFF != HW32_REG(FLASH_USER_START_ADDR + addr))
        {
            printf("FlashBlank error\n");
            Error_Handler();
        }
        addr += 4;
    }
    
    printf("FlashBlank OK\n");
}

/**
 * @brief  校验FLASH
 * @param  无
 * @retval 无
 */
static void APP_FlashVerify(void)
{
    uint32_t addr = 0;

    while (addr < sizeof(DATA))
    {
        if (DATA[addr / 4] != HW32_REG(FLASH_USER_START_ADDR + addr))
        {
            printf("FlashVerify error\n");
            Error_Handler();
        }
        addr += 4;
    }
    printf("FlashVerify OK\n");
}

/**
 * @brief  错误执行函数
 * @param  无
 * @retval 无
 */
void Error_Handler(void)
{
    while (1)
    {
    }
}
