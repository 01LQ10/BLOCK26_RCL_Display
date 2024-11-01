#include "flash.h"

#define DATA FDATA.W

union flash_DATA FDATA;                                                // flash���ݱ���
union flash_DATA *FDATA_R = (union flash_DATA *)FLASH_USER_START_ADDR; // ��ָ��

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

    // ����flash���ݵ�����
    FLASH_LOAD(BLOCK_BOM);
    // FLASH_LOAD(BLOCK_TOP);
    FDATA.R.BLOCK_TOP = 200;
}

// �洢���ݵ�flash
void flash_save(void)
{
    // д������
    // __disable_irq();
    HAL_FLASH_Unlock(); /* ����FLASH */

    APP_FlashErase();   /* ����FLASH */
    APP_FlashBlank();   /* ���FLASH */
    APP_FlashProgram(); /* дFLASH */

    HAL_FLASH_Lock(); /* ����FLASH */

    APP_FlashVerify(); /* У��FLASH */
    // __enable_irq();
}
/**
 * @brief  ����FLASH
 * @param  ��
 * @retval ��
 */
static void APP_FlashErase(void)
{
    uint32_t SECTORError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORERASE;         /* ��д����FLASH_TYPEERASE_PAGEERASE=Page��, FLASH_TYPEERASE_SECTORERASE=Sector�� */
    EraseInitStruct.SectorAddress = FLASH_USER_START_ADDR;           /* ��д��ʼ��ַ  */
    EraseInitStruct.NbSectors = 1;                                   /* ��Ҫ��д����������  */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) /* ִ��sector����,SECTORError���ز�д�����sector,����0xFFFFFFFF,��ʾ��д�ɹ�  */
    {
        printf("FlashErase error\n");
        Error_Handler();
    }
    printf("FlashErase OK\n");
}

/**
 * @brief  д��FLASH
 * @param  ��
 * @retval ��
 */
static void APP_FlashProgram(void)
{
    uint32_t flash_program_start = FLASH_USER_START_ADDR;                /* flashд��ʼ��ַ */
    uint32_t flash_program_end = (FLASH_USER_START_ADDR + sizeof(DATA)); /* flashд������ַ */
    uint32_t *src = (uint32_t *)DATA;                                    /* ����ָ�� */

    while (flash_program_start < flash_program_end)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK) /* ִ��Program */
        {
            flash_program_start += FLASH_PAGE_SIZE; /* flash��ʼָ��ָ���һ��page */
            src += FLASH_PAGE_SIZE / 4;             /* ��������ָ�� */
        }
    }
    printf("FlashProgram OK\n");
}

/**
 * @brief  ���FLASH
 * @param  ��
 * @retval ��
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
 * @brief  У��FLASH
 * @param  ��
 * @retval ��
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
 * @brief  ����ִ�к���
 * @param  ��
 * @retval ��
 */
void Error_Handler(void)
{
    while (1)
    {
    }
}
