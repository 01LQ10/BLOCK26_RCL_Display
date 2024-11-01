
u8 UART2_Buff[8], UART2_Buff_copy[8];

void TOF050_init(void)
{
	TOF050_Hard_init();

	TOF050_Write(BAUD, BAUD_112500);
	TOF050_Write(RANG, RANG_20cm);
	TOF050_Write(OPUT, OPUT_NAUTO);
	TOF050_Write(NIIC, NIIC_DISABLE);
	TOF050_Write(SFR, SFR_RESET);
	Delay_ms(100);
}

void TOF050_Hard_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_InternalClockConfig(TIM2);
	// 基本计数
	TIM_TimeBaseInitTypeDef TimeBaseInitStruct;
	TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 计数模式
	TimeBaseInitStruct.TIM_Period = 1000 - 1;				 // 重装寄存器
	TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;			 // 预分频
	TimeBaseInitStruct.TIM_RepetitionCounter = 0;			 // 重复计数
	TIM_TimeBaseInit(TIM2, &TimeBaseInitStruct);
	TIM_Cmd(TIM2, ENABLE);

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
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1位停止位
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

	TOF_data[0] = 0;				// 广播地址
	TOF_data[1] = 0x06;				// 写
	TOF_data[2] = bityHI(reg_addr); // 寄存器地址高位
	TOF_data[3] = bityLO(reg_addr); // 寄存器地址低位
	TOF_data[4] = bityHI(reg_data); // 数据高位
	TOF_data[5] = bityLO(reg_data); // 数据低位
	TOF_CRC = crc16tablefast(TOF_data, 6);
	TOF_data[6] = bityLO(TOF_CRC); // CRC低位
	TOF_data[7] = bityHI(TOF_CRC); // CRC高位

	for (i = 0; i < 8; i++)
	{
		USART_SendData(USART2, TOF_data[i]);
	}
}

void TOF050_Read(u16 reg_addr)
{
	u8 i;

	TOF_data[0] = 0;				// 广播地址
	TOF_data[1] = 0x03;				// 读
	TOF_data[2] = bityHI(reg_addr); // 寄存器地址高位
	TOF_data[3] = bityLO(reg_addr); // 寄存器地址低位
	TOF_data[4] = 0;				// 数据高位
	TOF_data[5] = 0;				// 数据低位
	TOF_CRC = crc16tablefast(TOF_data, 6);
	TOF_data[6] = bityLO(TOF_CRC); // CRC低位
	TOF_data[7] = bityHI(TOF_CRC); // CRC高位
	for (i = 0; i < 8; i++)
	{
		USART_SendData(USART2, TOF_data[i]);
	}
}

u8 UART2_Buff_p = 0, UART2_Flag = 0;
u16 TIM_LAST = 0, TIM_NOW = 0;
void USART2_IRQHandler()
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // RX中断
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清除标志位

		TIM_LAST = TIM_NOW;
		TIM_NOW = TIM2->CNT;

		if (TIM_NOW - TIM_LAST > 50) // 5ms
		{
			memcpy(UART2_Buff_copy, UART2_Buff, 8);
			UART2_Flag = 1; // 启动处理函数
			UART2_Buff_p = 0;
		}

		UART2_Buff[UART2_Buff_p] = USART_ReceiveData(USART2);
		UART2_Buff_p++;
		if (UART2_Buff_p == 7)
		{
			UART2_Buff_p = 0;
			UART2_Flag = 1; // 启动处理函数
		}
	}
}

void TOF050_Handler(void)
{
	u16 CRC_recalculat, CRC_receive, Distance;
	if (UART2_Flag == 1)
	{
		for (u8 i = 0; i < 7; i++)
		{
			USART_SendData(USART1, UART2_Buff[i]);
		}

		// if (UART2_Buff_copy[0] == 0 && UART2_Buff_copy[1] == 0x03) // 读数据
		// {
		// 	CRC_recalculat = crc16tablefast(TOF_data, 6);
		// 	CRC_receive = UART2_Buff_copy[6];
		// 	CRC_receive = CRC_receive << 8;
		// 	CRC_receive = CRC_receive | UART2_Buff_copy[5];
		// 	if (CRC_recalculat == CRC_receive)
		// 	{
		// 		Distance = UART2_Buff_copy[3];
		// 		Distance = Distance << 8;
		// 		Distance = Distance | UART2_Buff_copy[4];
		// 		printf("distance:%u", Distance);
		// 	}
		// }

		UART2_Flag = 0;
	}
}

void TOF050_TEST(void)
{
}
