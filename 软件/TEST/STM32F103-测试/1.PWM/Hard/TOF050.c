#include "TOF050.h"

/* UART2 Pin define
 * TX - A2(MB0) - 白
 * RX - A3(MB1) - 绿
 */

#define OUTPUT_T 0x0008		// ms
#define TIM_T (u16)(5 * 10) // ms

float BLOCK_TOP = 70, BLOCK_BOM = 5, BLOCK_DISTANCE = 0;

void TOF050_init(void)
{
	TOF050_Hard_init();

	TOF050_Write(BAUD, BAUD_112500);
	TOF050_Write(RANG, RANG_20cm);
	TOF050_Write(OPUT, OUTPUT_T);
	TOF050_Write(NIIC, NIIC_DISABLE);

	// TOF050_Write(SFR, SFR_RESET);
	// Delay_ms(10);
}

void TOF050_Hard_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_InternalClockConfig(TIM2);
	// 基本计数
	TIM_TimeBaseInitTypeDef TimeBaseInitStruct;
	TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 计数模式
	TimeBaseInitStruct.TIM_Period = 10000 - 1;				 // 重装寄存器
	TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;			 // 预分频
	TimeBaseInitStruct.TIM_RepetitionCounter = 0;			 // 重复计数
	TIM_TimeBaseInit(TIM2, &TimeBaseInitStruct);
	TIM_Cmd(TIM2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);

	USART_ClearFlag(USART2, USART_FLAG_TC | USART_FLAG_TXE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART2, ENABLE);
}

#define bityHI(x) (u8)((x) >> 8)
#define bityLO(x) (u8)((x) & 0x00FF)
u16 TOF_CRC;
u8 TOF_data[8];
void TOF050_Write(u16 reg_addr, u16 reg_data)
{
	u8 i;

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
		USART_SendData(USART2, TOF_data[i]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			;
	}
}

void TOF050_Read(u16 reg_addr, u16 reg_data)
{
	u8 i;

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
		USART_SendData(USART2, TOF_data[i]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			;
	}
}

u8 UART2_Buff_p = 0, UART2_Flag = 0;
u16 TIM_LAST = 0, TIM_NOW = 0;
u8 UART2_Buff[8] = {0}, UART2_Buff_copy[8] = {0};

void USART2_IRQHandler()
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // RX中断
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清除标志位

		// USART_SendData(USART1, USART_ReceiveData(USART2));

		TIM_LAST = TIM_NOW;
		TIM_NOW = TIM2->CNT;
		if (TIM_NOW - TIM_LAST > TIM_T) // 2.5ms
		{
			// memcpy(UART2_Buff_copy, UART2_Buff, 8);
			for (u8 i = 0; i < 8; i++)
			{
				UART2_Buff_copy[i] = UART2_Buff[i];
			}
			UART2_Flag = 1; // 启动处理函数
			UART2_Buff_p = 0;
			TIM2->CNT = 0;
		}
		if (UART2_Buff_p < 8)
		{
			UART2_Buff[UART2_Buff_p] = USART_ReceiveData(USART2);
			UART2_Buff_p++;
		}
	}
}

u8 GET_BULLET(float distance)
{
	u8 BULLET;
	if (distance < BLOCK_BOM + 1)
		BULLET = 16;
	else if (distance > BLOCK_TOP - 1)
		BULLET = 0;
	else // 距离->弹量
	{
		// if ((16 - (u8)((distance - BLOCK_BOM + 3.9 / 6) / 3.9)) == ((u8)((BLOCK_TOP - distance + 3.9 / 6) / 3.9)))
		// 	BULLET = 16 - (u8)((distance - BLOCK_BOM + 3.9 / 6) / 3.9);
		BULLET = 16 - (u8)((distance - BLOCK_BOM + 3.9 / 4) / 4.0);
	}
	if (BULLET > 16)
		BULLET = 16;
	return BULLET;
}

void TOF050_Handler(void)
{
	u16 CRC_recalculat, CRC_receive;
	u8 Distance_RAW;						  // 原始距离
	float Distance_FIL;						  // 滤波后距离
	static u8 BULLET_RAW = 0, BULLET_FIL = 0; // 弹量

	if (UART2_Flag == 1)
	{
		if (UART2_Buff_copy[0] == 0x01 && UART2_Buff_copy[1] == 0x03) // 读数据
		{
			// CRC校验
			CRC_recalculat = CRC16_calculate(UART2_Buff_copy, 5);
			CRC_receive = UART2_Buff_copy[6];
			CRC_receive = CRC_receive << 8;
			CRC_receive = CRC_receive | UART2_Buff_copy[5];
			// printf("CRC:%4x\n",CRC_recalculat);
			if (CRC_recalculat == CRC_receive) // CRC校验成功
			{
				// Distance_RAW = UART2_Buff_copy[3];
				// Distance_RAW = Distance_RAW << 8;
				// Distance_RAW = Distance_RAW | UART2_Buff_copy[4]; // 获取距离值
				Distance_RAW = UART2_Buff_copy[4];

				/*滤波*/
				Filter(Distance_RAW, &BLOCK_DISTANCE);
				BULLET_FIL = GET_BULLET(BLOCK_DISTANCE);
				printf(",%d\n", BULLET_FIL);
				// printf("any:%.2f,%u,%u,%.2f\n", BLOCK_DISTANCE, BULLET_FIL, Distance_RAW, filter_2(NULL, Distance_RAW));
			}
		}

		UART2_Flag = 0;
	}
}

void SET_TOP(float TOP)
{
	BLOCK_TOP = TOP;
	// printf("BOM IS %f\nTOP IS %f\n", BLOCK_BOM, BLOCK_TOP);
}

void SET_BOM(float BOM)
{
	BLOCK_BOM = BOM;
	// printf("BOM IS %f\nTOP IS %f\n", BLOCK_BOM, BLOCK_TOP);
}
