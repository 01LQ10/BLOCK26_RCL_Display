#include "main.h"
#include "LED.h"
#include "TOF_IIC.h"
#include "Debug.h"
#include "OS_Task.h"
#include "TOF_IIC.h"
#include "VL6180X.h"

// I2C_HandleTypeDef hi2c1;
// // I2C1的初始化函数
// void I2C1_Init(void)
// {
// 	hi2c1.Instance = I2C1;		   // 使用I2C1
// 	hi2c1.Init.ClockSpeed = 10000; // 通信速率100K
// 	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
// 	hi2c1.Init.OwnAddress1 = 0;
// 	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
// 	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
// 	hi2c1.Init.OwnAddress2 = 0;
// 	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
// 	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
// 	HAL_I2C_Init(&hi2c1); // 初始化I2C1
// }

// // I2C的底层初始化函数
// void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
// {
// 	GPIO_InitTypeDef GPIO_InitType; // IO口配置的结构体

// 	if (hi2c->Instance == I2C1)
// 	{
// 		__HAL_RCC_I2C1_CLK_ENABLE();  // 打开I2C1的时钟
// 		__HAL_RCC_GPIOB_CLK_ENABLE(); // 打开B口时钟

// 		GPIO_InitType.Pin = GPIO_PIN_6 | GPIO_PIN_7; // 配置PIN6 7
// 		GPIO_InitType.Mode = GPIO_MODE_AF_OD;		 // 复用开漏功能
// 		GPIO_InitType.Speed = GPIO_SPEED_FREQ_LOW;	 // 速度低速
// 		HAL_GPIO_Init(GPIOB, &GPIO_InitType);		 // 配置PB0 1
// 	}
// }

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void Error_Handler(void);

void LED_TASK(void)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	MX_GPIO_Init();
	Debug_init();
	OS_TIM_init();
	LED_Init();

	printf("SystemCoreClock:%u\n", SystemCoreClock);

	OS_ADD_TASK_MS(100, LED_TASK);

	/*IIC TEST*/
	I2C1_Init();

	if (VL6180X_Init(&hi2c1))
	{
		printf("TOF_INIT OK\n");
	}
	// printf("addr:%u\n", VL6180X_getAddress());
	printf("ID:%X\r\n", read8(VL6180X_REG_IDENTIFICATION_MODEL_ID));
	printf("STA:%02X\r\n", read8(VL6180X_REG_RESULT_RANGE_STATUS));
	write8(VL6180X_REG_SYSRANGE_START, 0x01);
	printf("STA_G:%02X\r\n", read8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO));
	printf("range:%u\r\n", VL6180X_readRange());
	
	// VL6180X_init();

	// printf("ID:%X\r\n", VL6180X_ReadByte(IDENTIFICATION__MODEL_ID));
	// printf("STATE:%X\r\n", VL6180X_ReadByte(0X004D));

	// VL6180X_WR_CMD(0X18, 0X01);
	// printf("RANGE:%X\r\n", VL6180X_ReadByte(0X0062));
	// // printf("range:%u\r\n", RangePollingRead());

	// VL6180X_ConfigureDefault();
	// VL6180X_SetTimeout(2);
	// VL6180X_SetScaling(1);

	printf("Init OK.\n");
	// uint8_t ID;
	// ID = VL6180X_ReadByte(0x00);

	// printf("ID:%u\n",ID);

	while (1)
	{
		Debug_Handler();
		// VL6180X_ReadByte(0x00);
		// HAL_Delay(100);
		// OS_Task_Pro_Handler();

		// uint16_t distance = VL6180X_ReadRangeSingleMillimeters();
		// printf("range:%u\r\n", read8(VL6180X_REG_RESULT_RANGE_VAL));
		// HAL_Delay(1000);
	}
}

static void MX_GPIO_Init(void)
{
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef clkinitstruct = {0};
	RCC_OscInitTypeDef oscinitstruct = {0};

	/* Configure PLL ------------------------------------------------------*/
	/* PLL configuration: PLLCLK = (HSI / 2) * PLLMUL = (8 / 2) * 16 = 64 MHz */
	/* PREDIV1 configuration: PREDIV1CLK = PLLCLK / HSEPredivValue = 64 / 1 = 64 MHz */
	/* Enable HSI and activate PLL with HSi_DIV2 as source */
	oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	oscinitstruct.HSEState = RCC_HSE_OFF;
	oscinitstruct.LSEState = RCC_LSE_OFF;
	oscinitstruct.HSIState = RCC_HSI_ON;
	oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	oscinitstruct.PLL.PLLState = RCC_PLL_ON;
	oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL16;
	if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK)
	{
		/* Initialization Error */
		while (1)
			;
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	   clocks dividers */
	clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
	clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK)
	{
		/* Initialization Error */
		while (1)
			;
	}
}

// void SystemClock_Config(void)
// {
// 	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
// 	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

// 	/** Initializes the RCC Oscillators according to the specified parameters
// 	 * in the RCC_OscInitTypeDef structure.
// 	 */
// 	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
// 	// RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
// 	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
// 	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
// 	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
// 	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
// 	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
// 	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
// 	{
// 		// Error_Handler();
// 	}

// 	/** Initializes the CPU, AHB and APB buses clocks
// 	 */
// 	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
// 	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
// 	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
// 	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
// 	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

// 	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
// 	{
// 		// Error_Handler();
// 	}
// }

void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
