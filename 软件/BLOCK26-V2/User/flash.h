#ifndef __FLASH_H
#define __FLASH_H

/*Í·ÎÄ¼þ*/
#include "Debug_log.h"

#define FLASH_USER_START_ADDR 0x08007800
#define FLASH_DATA_SIZE 16 // uint32

#define BLOCK_TOP_DEFAULT 80
#define BLOCK_BOM_DEFAULT 2

union flash_DATA
{
    uint32_t W[FLASH_DATA_SIZE];
    struct DATA_STR
    {
        float BLOCK_TOP;
        float BLOCK_BOM;
    } R;
};

extern union flash_DATA FDATA;

void flash_load(void);
void flash_save(void);
void flash_show(void);

static void APP_FlashErase(void);
static void APP_FlashProgram(void);
static void APP_FlashBlank(void);
static void APP_FlashVerify(void);
void Error_Handler(void);

#endif
