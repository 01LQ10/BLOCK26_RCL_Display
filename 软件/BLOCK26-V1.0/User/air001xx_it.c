#include "main.h"
#include "air001xx_it.h"
#include "Debug.h"
#include "TOF050.h"
#include "OS_Task.h"
#include "LED.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		Debug_IT_Handler(huart);
	}
	if (huart->Instance == USART1)
	{
		TOF_IT_Handler(huart);
	}
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&Debug_UartHandle);
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&TOF_UartHandle);
}

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
// 	if (htim->Instance == TIM14)
// 	{
// 		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
// 	}
// }

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM16)
	{
		OS_Task_Marks_Handler();
	}
	if (htim->Instance == TIM14)
	{
		LED_SCAN();
	}
}

void TIM16_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&OS_TimHandle);
}

void TIM14_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&LED_TimHandle);
}

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	while (1)
	{
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
}

/******************************************************************************/
/* AIR001xx Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/

/************************ (C) COPYRIGHT AirM2M *****END OF FILE******************/
