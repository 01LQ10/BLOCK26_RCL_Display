/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) AirM2M.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef UartHandle;
uint8_t aTxBuffer[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
uint8_t aRxBuffer[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
  /* systick初始化 */
  HAL_Init();
  
  /* USART初始化 */
  UartHandle.Instance          = USART1;
  UartHandle.Init.BaudRate     = 115200;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  HAL_UART_Init(&UartHandle);
  
  /*通过中断方式发送数据*/
  if (HAL_UART_Transmit_IT(&UartHandle, (uint8_t *)aTxBuffer, 12) != HAL_OK)
  {
    Error_Handler();
  }

  while (1)
  {
  }
}

/**
  * @brief  USART错误回调执行函数，输出错误代码
  * @param  huart：USART句柄
  * @retval 无
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  printf("Uart Error, ErrorCode = %d\r\n", huart->ErrorCode);
}

/**
  * @brief  USART发送回调执行函数
  * @param  huart：USART句柄
  * @retval 无
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if (HAL_UART_Receive_IT(UartHandle, (uint8_t *)aRxBuffer, 12) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  USART接收回调执行函数
  * @param  huart：USART句柄
  * @retval 无
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /*通过中断方式接收数据*/
  if (HAL_UART_Transmit_IT(UartHandle, (uint8_t *)aRxBuffer, 12) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void Error_Handler(void)
{
  /* 无限循环 */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  输出产生断言错误的源文件名及行号
  * @param  file：源文件名指针
  * @param  line：发生断言错误的行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* 用户可以根据需要添加自己的打印信息,
     例如: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* 无限循环 */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT AirM2M *****END OF FILE******************/
