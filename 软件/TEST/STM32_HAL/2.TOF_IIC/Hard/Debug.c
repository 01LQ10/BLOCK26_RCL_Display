#include "Debug.h"

/*全局变量*/
#define BUFF_MAX 20				// 最大缓存数量
char Uart_Buff[BUFF_MAX];		// UART接收缓存
uint8_t Uart_Rx_Num = 0;		// UART接收指针
uint8_t Flag_OPT_Handle = 0;	// 命令处理标志
uint8_t Flag_BUFF_OverFlow = 0; // 缓存溢出标志
uint8_t arg_offset = 0;			// 参数偏移
OPT_arg_type opt_arg;			// 参数存储

UART_HandleTypeDef Debug_UartHandle;

/*便捷操作*/
#define SET_Flag_OPT_Handle Flag_OPT_Handle = 1
#define CLEAR_Flag_OPT_Handle Flag_OPT_Handle = 0
#define Char_is(x) (Uart_Buff[Uart_Rx_Num] == x)
#define Char_isnt(x) (Uart_Buff[Uart_Rx_Num] != x)

int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口USARTx */
	HAL_UART_Transmit(&Debug_UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	// HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)&ch, 1);
	return (ch);
}

/*****************************************************************
 * @brief Init debug module.
 * 	Complete the configuration of gpio, uart and other peripherals
 * 	初始化Debug模块，完成gpio、uart等外设的配置
 * @param None
 * @return None
 *****************************************************************/
void Debug_init(void)
{
	/* USART初始化 */
	Debug_UartHandle.Instance = USART1;
	Debug_UartHandle.Init.BaudRate = 115200;
	Debug_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	Debug_UartHandle.Init.StopBits = UART_STOPBITS_1;
	Debug_UartHandle.Init.Parity = UART_PARITY_NONE;
	Debug_UartHandle.Init.Mode = UART_MODE_TX_RX;
	Debug_UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	Debug_UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&Debug_UartHandle);

	/* NVIC初始化 */
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)Uart_Buff, 1);
}

/*****************************************************************
 * @brief UART Interrupt request handler, put this function in the
 * 	串口中断处理函数，放入串口中断函数里
 * @param None
 * @return None
 *****************************************************************/
void Debug_IT_Handler(void)
{
	if (Uart_Rx_Num >= BUFF_MAX - 1)
	{
		Uart_Rx_Num = 0;
		Flag_BUFF_OverFlow = 1; // 挂起缓存溢出标志
	}

	if (Char_is('\n') || Char_is('*')) // 检测结束符号
	{
		if (Uart_Rx_Num != 0)
		{
			SET_Flag_OPT_Handle;
			Uart_Buff[Uart_Rx_Num] = '\0';
			Uart_Rx_Num = 0;
		}
	}
	else if (Char_isnt(' ')) // 屏蔽特定字符
		Uart_Rx_Num++;

	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)(&Uart_Buff[Uart_Rx_Num]), 1);
}

/*****************************************************************
 * @brief Debug handler, put this function in the while loop
 * 	Debug处理函数，放入while循环里
 * @param None
 * @return None
 *****************************************************************/
void Debug_Handler(void)
{
	uint8_t i;
	if (Flag_OPT_Handle == 1) // 判断命令处理标志是否挂起
	{
		/*****  缓存溢出处理  *****/
		if (Flag_BUFF_OverFlow)
		{
			Flag_BUFF_OverFlow = 0; // 清除缓存溢出标志
			printf("[Error]: BUFF OVER FLOW\r\n");
			CLEAR_Flag_OPT_Handle; // 清除命令处理标志
			return;				   // 跳出处理函数
		}

		printf(">> %s\r\n", Uart_Buff);

		/*****  命令预处理  *****/
		arg_offset = 0;			 // 参数偏移归零
		for (i = 0; i < 50; i++) // 拆分命令和数据
		{
			if (Uart_Buff[i] == '\0')
				break;
			if (Uart_Buff[i] == ':')
			{
				Uart_Buff[i] = '\0';
				arg_offset = i + 1;
				break;
			}
		}

/*****  命令匹配与执行  *****/
#define OPTION_IS(x) (strcmp(Uart_Buff, x) == 0)
#define OPTION_DATA &Uart_Buff[arg_offset]

		/** 用户设置3 **
		 * @brief 用户自定义命令
		 * @example 如下
		 */
		/*例1：无参数命令*/
		if (OPTION_IS("TEST")) // 判断命令匹配字符串
		{
			printf("TEST SECCUSS\r\n"); // 执行操作
		}
		/*例2：整数参数*/
		else if (OPTION_IS("SET_LED")) // 判断命令匹配字符串
		{
			opt_arg.i = atoi(OPTION_DATA); // 转换整数参数
		}

		else if (OPTION_IS("RESET"))
		{
			HAL_NVIC_SystemReset();
		}

		/*无匹配命令*/
		else
		{
			printf("[Error]: here is no this option\r\n");
		}

		CLEAR_Flag_OPT_Handle; // 清除命令处理标志
	}
}
