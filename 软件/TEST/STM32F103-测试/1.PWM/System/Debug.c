#include "Debug.h"
#include <string.h>
#include <stdlib.h>
#include "TOF050.h"

char Uart1_Buffer[20] = {0};
char Uart1_Rx_Num = 0;
char debug_flag = 0;

#define SET_Debug_flag debug_flag = 1
#define CLEAR_Debug_flag debug_flag = 0

int fputc(int c, FILE *f)
{
	USART_SendData(USART1, (u8)c);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	return 1;
}

void Debug_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);

	USART_ClearFlag(USART1, USART_FLAG_TC | USART_FLAG_TXE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // NVIC锟斤拷锟斤拷
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler()
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // RX中断
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); // 清除标志位

		Uart1_Buffer[Uart1_Rx_Num] = USART_ReceiveData(USART1);
		if (Uart1_Buffer[Uart1_Rx_Num] == '\n')
		{
			Uart1_Buffer[Uart1_Rx_Num] = '\0';
			SET_Debug_flag;
			// printf("%s",Uart1_Buffer);
			Uart1_Rx_Num = 0;
		}
		else
			Uart1_Rx_Num++;
	}
}

char opt_offset = 0; // 命令偏移
union OPT_val		 // 参数值
{
	u8 uc; // uchar
	uint16_t us;
	uint32_t ui;
	int i;
	float f;
	double lf;
} opt_val;
void Debug_Handler(void)
{
	if (debug_flag == 1)
	{
		opt_offset = 0;
		for (u8 i = 0; i < 20; i++)
		{
			if (Uart1_Buffer[i] == '\0')
				break;

			if (Uart1_Buffer[i] == ':')
			{
				Uart1_Buffer[i] = '\0';
				opt_offset = i + 1;
				break;
			}
		}

		if (strcmp(Uart1_Buffer, "LED") == 0)
		{
			opt_val.i = atoi(&Uart1_Buffer[opt_offset]);
			if (opt_val.uc == 0)
				LED_ON(LED1);
			if (opt_val.uc == 1)
				LED_OFF(LED1);
		}
		if (strcmp(Uart1_Buffer, "SET_OPUT") == 0)
		{
			opt_val.i = atoi(&Uart1_Buffer[opt_offset]);
			if (opt_val.uc == 0)
				TOF050_Write(OPUT, OPUT_NAUTO);
			if (opt_val.uc == 1)
				TOF050_Write(OPUT, 5);
		}
		if (strcmp(Uart1_Buffer, "READ_DATA") == 0)
		{
			TOF050_Read(DATA, DATA_READ);
		}
		if (strcmp(Uart1_Buffer, "SFR_DEFAULT") == 0)
		{
			TOF050_Write(SFR, SFR_DEFAULT);
		}
		if (strcmp(Uart1_Buffer, "SET_TOP") == 0)
		{
			opt_val.f = atof(&Uart1_Buffer[opt_offset]);
			SET_TOP(opt_val.f);
		}
		if (strcmp(Uart1_Buffer, "SET_BOM") == 0)
		{
			opt_val.f = atof(&Uart1_Buffer[opt_offset]);
			SET_BOM(opt_val.f);
		}

		// LED_Turn(LED1);

		CLEAR_Debug_flag;
	}
}
