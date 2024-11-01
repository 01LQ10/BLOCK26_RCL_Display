// ..\tool\AirISP.exe -c air001 -p COM3 -b 115200 write_flash -e 0x08000000 .\Output\air001.hex
/**��һ����
 * ��ʱ�ػ�  ������⣺ȷ�� CNT clear ������ [1]���������ı仯[2]��������[3]debug����ͨ�� [v]
 * ���������ʼ��Ĭ��ֵ [X]
 * ����������÷��� [X]
 * ADC����[x]
 * ��β���û�ϵͳ���Ż��㷨
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
	/* ��ʼ���������裬Flash�ӿڣ�SysTick */
	HAL_Init();
	APP_SystemClockConfig(); // 24MHz
	HAL_Delay(1000);		 // ����������ʱ

	Power_init(); // ��Ƭ���ӹܵ�Դ���ƣ���������Դ

	Debug_init();
	flash_load();
	OS_TIM_init();
	LED_Init();
	// ������˸
	LED_Blink();

	BUTN_init();
	TOF050_init();

	// �ȴ������ɿ�����ֹ������
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

	/* ����HSI,HSE,LSE,LSI,PLL����ʱ�� */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;						  /* ʹ��HSI */
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;						  /* HSIԤ��Ƶ */
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz; /* ����HSI���ʱ��Ϊ24MHz,�������У׼ֵ */
	RCC_OscInitStruct.HSEState = RCC_HSE_OFF;						  /* ��ֹHSE */
	RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;					  /* ����HSEƵ�ʷ�Χ,û�ÿ��Բ����� */
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;						  /* ��ֹLSI */
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;						  /* ��ֹLSE */
	RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_LOW;					  /* ��Ҫ���ݾ����������������� */
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;					  /* ʹ��PLL */
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;			  /* PLL��ʱ��Դ��PLLʱ��Դֻ��24Mʱ, ��ʹ��PLL */

	HAL_RCC_OscConfig(&RCC_OscInitStruct); /* ����ʱ�� */

	/* ��ʼ��AHB,APB����ʱ�� */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; /* ����AHBʱ��Դ */
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;	   /* ����AHBԤ��Ƶ */
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;	   /* ����APB1Ԥ��Ƶ */

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1); /* �������� */
}
