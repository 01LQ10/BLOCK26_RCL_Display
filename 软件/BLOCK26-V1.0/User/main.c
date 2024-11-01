// ..\tool\AirISP.exe -c air001 -p COM3 -b 115200 write_flash -e 0x08000000 .\Output\air001.hex
/**下一步：
 * 超时关机  解决问题：确定 CNT clear 的条件 [1]弹量参数的变化[2]按键操作[3]debug串口通信 [v]
 * 距离参数初始化默认值 [X]
 * 距离参数设置防误触 [X]
 * ADC测试[x]
 * 结尾：用户系统，优化算法
 */
#include "air001xx_hal.h"
#include "Debug.h"
#include "TOF050.h"
#include "LED.h"
#include "OS_Task.h"
#include "KEY.h"
#include "Power.h"

void APP_SystemClockConfig(void);

void led(void)
{
	static uint8_t i = 0;
	LED_SET_BUFF(i++);
	if (i == 17)
		i = 0;
}

int main(void)
{
	/* 初始化所有外设，Flash接口，SysTick */
	HAL_Init();
	APP_SystemClockConfig(); // 24MHz
	HAL_Delay(1000);		 // 长按开机延时

	Power_init(); // 单片机接管电源控制，并开启电源

	Debug_init();
	flash_load();
	OS_TIM_init();
	LED_Init();
	// 开机闪烁
	LED_Blink();

	BUTN_init();
	TOF050_init();

	// 等待按键松开，防止开机误触
	while (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == 0)
		;

	printf(">> Welcome to BLOCK-26\r\n");
	printf("SystemCoreClock:%u\n", SystemCoreClock);

	// OS_ADD_TASK_US(500, LED_SCAN);
	// OS_ADD_TASK_MS(100, led); // TEST
	OS_ADD_TASK_MS(TICKS_INTERVAL, BUTN_tick);
	OS_ADD_TASK_MS(TOF_ticks, TOF050_Handler);
	OS_ADD_TASK_MS(TIMEOUT_tick, Power_TimeOUT_Handle);

	while (1)
	{
		Debug_Handler();
		OS_Task_Pro_Handler();
	}
}

static void APP_SystemClockConfig(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/* 配置HSI,HSE,LSE,LSI,PLL所有时钟 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;						  /* 使能HSI */
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;						  /* HSI预分频 */
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz; /* 设置HSI输出时钟为24MHz,库会设置校准值 */
	RCC_OscInitStruct.HSEState = RCC_HSE_OFF;						  /* 禁止HSE */
	RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;					  /* 设置HSE频率范围,没用可以不设置 */
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;						  /* 禁止LSI */
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;						  /* 禁止LSE */
	RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_LOW;					  /* 需要根据晶振规格设置驱动能力 */
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;					  /* 使能PLL */
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;			  /* PLL的时钟源，PLL时钟源只有24M时, 再使能PLL */

	HAL_RCC_OscConfig(&RCC_OscInitStruct); /* 配置时钟 */

	/* 初始化AHB,APB总线时钟 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; /* 配置AHB时钟源 */
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;	   /* 设置AHB预分频 */
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;	   /* 设置APB1预分频 */

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1); /* 配置总线 */
}
