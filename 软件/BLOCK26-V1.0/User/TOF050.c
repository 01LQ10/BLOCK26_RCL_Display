#include "TOF050.h"

/* UART1 Pin define
 * TX - A14(MB0) - 白
 * RX - A13(MB1) - 绿
 */

#define OUTPUT_T 20				  // ms
#define TIM_T (uint16_t)(10 * 10) // ms

UART_HandleTypeDef TOF_UartHandle;
TIM_HandleTypeDef TOF_TimHandle;

float BLOCK_TOP = 78, BLOCK_BOM = 8, BLOCK_DISTANCE = 0;

uint8_t TOFUart_Buff_p = 0, TOFUart_Flag = 0;
uint16_t TIM_LAST = 0, TIM_NOW = 0;
uint8_t TOFUart_Buff[8] = {0}, TOFUart_Buff_copy[8] = {0}, TOFUart_Buff_temp;

void TOF050_init(void)
{
	TOF050_Hard_init();
	TOF050_Write(BAUD, BAUD_112500);
	TOF050_Write(RANG, RANG_20cm);
	TOF050_Write(OPUT, OUTPUT_T);
	TOF050_Write(NIIC, NIIC_DISABLE);
	TOF050_Write(SFR, SFR_RESET);
	HAL_Delay(1000);

	if (FDATA.R.BLOCK_BOM == 0XFFFFFFFF || FDATA.R.BLOCK_TOP == 0XFFFFFFFF)
	{
		FDATA.R.BLOCK_BOM = BLOCK_BOM;
		FDATA.R.BLOCK_TOP = BLOCK_TOP;
	}
}

void TOF050_Hard_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_USART1_CLK_ENABLE(); /* 使能USART1时钟 */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Alternate = GPIO_AF8_USART1;
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USART初始化 */
	TOF_UartHandle.Instance = USART1;
	TOF_UartHandle.Init.BaudRate = 115200;
	TOF_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	TOF_UartHandle.Init.StopBits = UART_STOPBITS_1;
	TOF_UartHandle.Init.Parity = UART_PARITY_NONE;
	TOF_UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	TOF_UartHandle.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&TOF_UartHandle);

	HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	HAL_UART_Receive_IT(&TOF_UartHandle, (uint8_t *)(&TOFUart_Buff[TOFUart_Buff_p]), 1);

	/*定时器初始化*/
	// __HAL_RCC_TIM14_CLK_ENABLE();
	// TOF_TimHandle.Instance = TIM14;										   /* 选择TIM14 */
	// TOF_TimHandle.Init.Period = 0xFFFF - 1;								   /* 自动重装载值 0.1ms*/
	// TOF_TimHandle.Init.Prescaler = 800 - 1;								   /* 不预分频 */
	// TOF_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;			   /* 时钟不分频 */
	// TOF_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;				   /* 向上计数 */
	// TOF_TimHandle.Init.RepetitionCounter = 1 - 1;						   /* 不重复计数 */
	// TOF_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* 自动重装载寄存器没有缓冲 */
	// HAL_TIM_Base_Init(&TOF_TimHandle);
	// HAL_TIM_Base_Start(&TOF_TimHandle);
}

void TOF_UART_print_test(uint8_t *str)
{
	uint8_t i = 0;
	while (str[i] != '\0')
	{
		HAL_UART_Transmit(&TOF_UartHandle, &(str[i]), 1, 0xff);
		i++;
	}
}

#define bityHI(x) (uint8_t)((x) >> 8)
#define bityLO(x) (uint8_t)((x) & 0x00FF)

void TOF050_Write(uint16_t reg_addr, uint16_t reg_data)
{
	uint16_t TOF_CRC;
	uint8_t TOF_data[8];
	uint8_t i;

	TOF_data[0] = 0x00;				// 广播地址
	TOF_data[1] = 0x06;				// 写
	TOF_data[2] = bityHI(reg_addr); // 寄存器地址高位
	TOF_data[3] = bityLO(reg_addr); // 寄存器地址低位
	TOF_data[4] = bityHI(reg_data); // 数据高位
	TOF_data[5] = bityLO(reg_data); // 数据低位
	TOF_CRC = CRC16_calculate(TOF_data, 6);
	TOF_data[6] = bityLO(TOF_CRC); // CRC低位
	TOF_data[7] = bityHI(TOF_CRC); // CRC高位

	for (i = 0; i < 8; i++)
	{
		HAL_UART_Transmit(&TOF_UartHandle, &(TOF_data[i]), 1, 0xff);
	}
}

void TOF050_Read(uint16_t reg_addr, uint16_t reg_data)
{
	uint16_t TOF_CRC;
	uint8_t TOF_data[8];
	uint8_t i;

	TOF_data[0] = 0x01;				// 从机地址
	TOF_data[1] = 0x03;				// 读
	TOF_data[2] = bityHI(reg_addr); // 寄存器地址高位
	TOF_data[3] = bityLO(reg_addr); // 寄存器地址低位
	TOF_data[4] = bityHI(reg_data); // 数据高位
	TOF_data[5] = bityLO(reg_data); // 数据低位
	TOF_CRC = CRC16_calculate(TOF_data, 6);
	TOF_data[6] = bityLO(TOF_CRC); // CRC低位
	TOF_data[7] = bityHI(TOF_CRC); // CRC高位
	for (i = 0; i < 8; i++)
	{
		HAL_UART_Transmit(&TOF_UartHandle, &(TOF_data[i]), 1, 0xff);
	}
}

uint16_t TOF_timeCNT = 0;
void TOF_IT_Handler(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		// TIM_LAST = TIM_NOW;
		// TIM_NOW = TIM14->CNT;
		// if (TIM_NOW - TIM_LAST > TIM_T) // 2.5ms
		// {
		// 	for (uint8_t i = 0; i < 8; i++)
		// 	{
		// 		TOFUart_Buff_copy[i] = TOFUart_Buff[i];
		// 	}
		// 	TOFUart_Flag = 1; // 启动处理函数
		// 	TOFUart_Buff_p = 0;
		// 	TIM14->CNT = 0;
		// }

		if (TOF_timeCNT > (OUTPUT_T - 5) / TOF_ticks)
		{
			for (uint8_t i = 0; i < 8; i++)
			{
				TOFUart_Buff_copy[i] = TOFUart_Buff[i];
			}
			TOFUart_Flag = 1; // 启动处理函数
			TOFUart_Buff_p = 0;
		}
		TOF_timeCNT = 0;

		if (TOFUart_Buff_p < 8)
		{
			TOFUart_Buff[TOFUart_Buff_p] = TOFUart_Buff_temp;
			TOFUart_Buff_p++;
		}

		HAL_UART_Receive_IT(&TOF_UartHandle, (uint8_t *)(&TOFUart_Buff_temp), 1);
	}
}

uint8_t GET_BULLET(float distance)
{
	uint8_t BULLET;
	float distance_erro;
	if (distance < FDATA.R.BLOCK_BOM + 1)
		BULLET = 16;
	else if (distance > FDATA.R.BLOCK_TOP - 1)
		BULLET = 0;
	else // 距离->弹量
	{
		// if ((16 - (uint8_t)((distance - FDATA.R.BLOCK_BOM + 3.9 / 6) / 3.9)) == ((uint8_t)((FDATA.R.BLOCK_TOP - distance + 3.9 / 6) / 3.9)))
		// 	BULLET = 16 - (uint8_t)((distance - FDATA.R.BLOCK_BOM + 3.9 / 6) / 3.9);
		distance_erro = distance - FDATA.R.BLOCK_BOM;
		if (distance_erro < 0)
			distance_erro = 0;
		BULLET = 16 - (uint8_t)((distance_erro + 3.9 / 3) / 4.0);
	}
	if (BULLET > 16 & BULLET < 20)
		BULLET = 16;
	if (BULLET > 100)
		BULLET = 0;
	return BULLET;
}

void TOF050_Handler(void)
{
	uint16_t CRC_recalculat, CRC_receive;
	uint8_t Distance_RAW;							// 原始距离
	float Distance_FIL;								// 滤波后距离
	static uint8_t BULLET_LAST = 0, BULLET_FIL = 0; // 弹量

	TOF_timeCNT++;
	if (TOFUart_Flag == 1)
	{
		// HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
		// printf("RX: %x %x %x %x %x %x %x\n", TOFUart_Buff_copy[0], TOFUart_Buff_copy[1], TOFUart_Buff_copy[2], TOFUart_Buff_copy[3], TOFUart_Buff_copy[4], TOFUart_Buff_copy[5], TOFUart_Buff_copy[6]);

		if (TOFUart_Buff_copy[0] == 0x01 && TOFUart_Buff_copy[1] == 0x03) // 读数据
		{
			// CRC校验
			CRC_recalculat = CRC16_calculate(TOFUart_Buff_copy, 5);
			CRC_receive = TOFUart_Buff_copy[6];
			CRC_receive = CRC_receive << 8;
			CRC_receive = CRC_receive | TOFUart_Buff_copy[5];
			// printf("CRC:%4x\n",CRC_recalculat);
			if (CRC_recalculat == CRC_receive) // CRC校验成功
			{
				// Distance_RAW = TOFUart_Buff_copy[3];
				// Distance_RAW = Distance_RAW << 8;
				// Distance_RAW = Distance_RAW | TOFUart_Buff_copy[4]; // 获取距离值
				Distance_RAW = TOFUart_Buff_copy[4];

				/*滤波*/
				Filter(Distance_RAW, &BLOCK_DISTANCE);
				BULLET_LAST = BULLET_FIL;
				BULLET_FIL = GET_BULLET(BLOCK_DISTANCE);
				if (BULLET_LAST != BULLET_FIL) // 弹量变化时清空超时记录
					Power_TimeOUT_clearCNT();
				// printf("%u,%u\n", Distance_RAW, BULLET_FIL);
				// BULLET_FIL = GET_BULLET(Distance_RAW);
				// printf("distance:%u,%u\n", Distance_RAW, BULLET_FIL);
				LED_SET_BUFF((uint8_t)BULLET_FIL);
			}
		}

		TOFUart_Flag = 0;
	}
}

void SET_TOP(float TOP)
{
	if (TOP != NULL)
		FDATA.R.BLOCK_TOP = TOP;
	else
		FDATA.R.BLOCK_TOP = BLOCK_DISTANCE;
	printf("BOM IS %f\nTOP IS %f\n", FDATA.R.BLOCK_BOM, FDATA.R.BLOCK_TOP);
}

void SET_BOM(float BOM)
{
	if (BOM != NULL)
		FDATA.R.BLOCK_BOM = BOM;
	else
		FDATA.R.BLOCK_BOM = BLOCK_DISTANCE - 64;
	printf("BOM IS %f\nTOP IS %f\n", FDATA.R.BLOCK_BOM, FDATA.R.BLOCK_TOP);
}
