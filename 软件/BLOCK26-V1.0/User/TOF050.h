#ifndef __TOF050_H
#define __TOF050_H

#include "air001xx_hal.h"
#include <math.h>
#include <string.h>
#include "LED.h"
#include "CRC16.h"
#include "flash.h"
#include "Filter.h"
#include "Power.h"

#define TOF_ticks 1 // ms

extern UART_HandleTypeDef TOF_UartHandle;
extern TIM_HandleTypeDef TOF_TimHandle;

extern float BLOCK_TOP, BLOCK_BOM;

void TOF050_init(void);
void TOF050_Hard_init(void);
void TOF050_Write(uint16_t reg_addr, uint16_t reg_data);
void TOF050_Read(uint16_t reg_addr, uint16_t reg_data);
void TOF050_Handler(void);
void TOF_IT_Handler(UART_HandleTypeDef *huart);

void SET_TOP(float TOP);
void SET_BOM(float BOM);

void TOF_UART_print_test(uint8_t *str);

/*寄存器*/

#define SFR 0x0001         // 特殊寄存器
#define SFR_TEST 0x0000    // 测试通讯
#define SFR_RESET 0x1000   // 重启
#define SFR_DEFAULT 0xAA55 // 恢复默认参数

#define ADDR 0x0002 // 设备地址

#define BAUD 0x0003 // 波特率
#define BAUD_38400 0x0001
#define BAUD_9600 0x0002
#define BAUD_112500 0x0003

#define RANG 0x0004 // 量程 measuring range
#define RANG_20cm 0x0001
#define RANG_40cm 0x0002
#define RANG_50cm 0x0003

#define OPUT 0x0005 // 连续输出控制
#define OPUT_NAUTO 0x0000
#define OPUT_AUTO10ms 10
#define OPUT_AUTO50ms 50

#define ADJS 0x0006           // 加载校准
#define ADJS_ENABLE 0x0000    // 加载
#define ADJS_DISENABLE 0x0001 // 不加载

#define CORR 0x0007 // 偏移修正值
#define XTKC 0x0008 // xtalk修正值

#define NIIC 0x0009         // 禁止iic使能
#define NIIC_ALLOW 0x0000   // 不禁止（默认）
#define NIIC_DISABLE 0x0001 // 禁止(MCU释放io)

#define DATA 0x0010 // 测量结果
#define DATA_READ 0x0001

#define OFFS 0x0020 // offset校准
#define XTKA 0x0021 // xtalk校准

#endif
