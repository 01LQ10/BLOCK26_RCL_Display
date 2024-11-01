#include "Debug.h"

/*ȫ�ֱ���*/
#define BUFF_MAX 20				// ��󻺴�����
char Uart_Buff[BUFF_MAX];		// UART���ջ���
uint8_t Uart_Rx_Num = 0;		// UART����ָ��
uint8_t Flag_OPT_Handle = 0;	// ������־
uint8_t Flag_BUFF_OverFlow = 0; // ���������־
uint8_t arg_offset = 0;			// ����ƫ��
OPT_arg_type opt_arg;			// �����洢

UART_HandleTypeDef Debug_UartHandle;

/*��ݲ���*/
#define SET_Flag_OPT_Handle Flag_OPT_Handle = 1
#define CLEAR_Flag_OPT_Handle Flag_OPT_Handle = 0
#define Char_is(x) (Uart_Buff[Uart_Rx_Num] == x)
#define Char_isnt(x) (Uart_Buff[Uart_Rx_Num] != x)

int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����USARTx */
	HAL_UART_Transmit(&Debug_UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	// HAL_UART_Transmit_IT(&Debug_UartHandle, (uint8_t *)&ch, 1);
	return (ch);
}

/*****************************************************************
 * @brief Init debug module.
 * 	Complete the configuration of gpio, uart and other peripherals
 * 	��ʼ��Debugģ�飬���gpio��uart�����������
 * @param None
 * @return None
 *****************************************************************/
void Debug_init(void)
{
	/* USART��ʼ�� */
	Debug_UartHandle.Instance = USART1;
	Debug_UartHandle.Init.BaudRate = 115200;
	Debug_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	Debug_UartHandle.Init.StopBits = UART_STOPBITS_1;
	Debug_UartHandle.Init.Parity = UART_PARITY_NONE;
	Debug_UartHandle.Init.Mode = UART_MODE_TX_RX;
	Debug_UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	Debug_UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&Debug_UartHandle);

	/* NVIC��ʼ�� */
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)Uart_Buff, 1);
}

/*****************************************************************
 * @brief UART Interrupt request handler, put this function in the
 * 	�����жϴ����������봮���жϺ�����
 * @param None
 * @return None
 *****************************************************************/
void Debug_IT_Handler(void)
{
	if (Uart_Rx_Num >= BUFF_MAX - 1)
	{
		Uart_Rx_Num = 0;
		Flag_BUFF_OverFlow = 1; // ���𻺴������־
	}

	if (Char_is('\n') || Char_is('*')) // ����������
	{
		if (Uart_Rx_Num != 0)
		{
			SET_Flag_OPT_Handle;
			Uart_Buff[Uart_Rx_Num] = '\0';
			Uart_Rx_Num = 0;
		}
	}
	else if (Char_isnt(' ')) // �����ض��ַ�
		Uart_Rx_Num++;

	HAL_UART_Receive_IT(&Debug_UartHandle, (uint8_t *)(&Uart_Buff[Uart_Rx_Num]), 1);
}

/*****************************************************************
 * @brief Debug handler, put this function in the while loop
 * 	Debug������������whileѭ����
 * @param None
 * @return None
 *****************************************************************/
void Debug_Handler(void)
{
	uint8_t i;
	if (Flag_OPT_Handle == 1) // �ж�������־�Ƿ����
	{
		/*****  �����������  *****/
		if (Flag_BUFF_OverFlow)
		{
			Flag_BUFF_OverFlow = 0; // ������������־
			printf("[Error]: BUFF OVER FLOW\r\n");
			CLEAR_Flag_OPT_Handle; // ���������־
			return;				   // ����������
		}

		printf(">> %s\r\n", Uart_Buff);

		/*****  ����Ԥ����  *****/
		arg_offset = 0;			 // ����ƫ�ƹ���
		for (i = 0; i < 50; i++) // ������������
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

/*****  ����ƥ����ִ��  *****/
#define OPTION_IS(x) (strcmp(Uart_Buff, x) == 0)
#define OPTION_DATA &Uart_Buff[arg_offset]

		/** �û�����3 **
		 * @brief �û��Զ�������
		 * @example ����
		 */
		/*��1���޲�������*/
		if (OPTION_IS("TEST")) // �ж�����ƥ���ַ���
		{
			printf("TEST SECCUSS\r\n"); // ִ�в���
		}
		/*��2����������*/
		else if (OPTION_IS("SET_LED")) // �ж�����ƥ���ַ���
		{
			opt_arg.i = atoi(OPTION_DATA); // ת����������
		}

		else if (OPTION_IS("RESET"))
		{
			HAL_NVIC_SystemReset();
		}

		/*��ƥ������*/
		else
		{
			printf("[Error]: here is no this option\r\n");
		}

		CLEAR_Flag_OPT_Handle; // ���������־
	}
}
