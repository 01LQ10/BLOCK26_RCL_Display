#include "TOF050.h"

#include "VL6180X.h"
#include "Filter.h"
#include "Power.h"
#include "flash.h"
#include "LED.h"

TIM_HandleTypeDef TOF_TimHandle;

uint8_t TOF_range, TOF_range_status; // ԭʼ����
uint8_t TOF_Handler_Flag = 0;

uint8_t *BLOCK_BULLET_NUM; // ����
uint8_t BLOCK_BULLET_NUM_LSAT;
float BLOCK_TOP = 75, BLOCK_BOM = 12.5, BLOCK_DISTANCE = 0; // �˲������

void TOF050_Init(uint8_t *bullet_num)
{
	P_printf("TOF Init ...\n");
	if (NULL != bullet_num)
		BLOCK_BULLET_NUM = bullet_num;
	else
		T_printf("TOF data erro\n");

	if (false == VL6180X_Init())
	{
		P_printf("TOF_INIT false\n");
		return;
	}
	// �Ĵ�����ʼ��

	HAL_Delay(10);
	// TOF050_TIM_Init();

	P_printf("TOF_INIT OK\n");
}

void TOF050_TIM_Init(void)
{
	/*��ʱ�� TIM17 ��ʼ��*/
	__HAL_RCC_TIM17_CLK_ENABLE();
	TOF_TimHandle.Instance = TIM17;										   /* ѡ��TIM16 */
	TOF_TimHandle.Init.Period = 1000 - 1;								   /* �Զ���װ��ֵ 100ms*/
	TOF_TimHandle.Init.Prescaler = 2400 - 1;							   /* ��Ԥ��Ƶ */
	TOF_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;			   /* ʱ�Ӳ���Ƶ */
	TOF_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;				   /* ���ϼ��� */
	TOF_TimHandle.Init.RepetitionCounter = 1 - 1;						   /* ���ظ����� */
	TOF_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* �Զ���װ�ؼĴ���û�л��� */
	HAL_TIM_Base_Init(&TOF_TimHandle);
	HAL_TIM_Base_Start_IT(&TOF_TimHandle);

	HAL_NVIC_SetPriority(TIM17_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM17_IRQn);
}

void TOF050_Read_range(void)
{
	TOF_range = VL6180X_readRange();
	TOF_range_status = VL6180X_readRangeStatus();
	// if (status == 0)
	// 	T_printf("range:%u\n", TOF_range);
	// else
	// 	T_printf("vl6180 erro:%u\n", status);
}

void TOF050_IT_Handler(void)
{
	TOF050_Read_range();
	TOF_Handler_Flag = 1;
}

uint8_t TOF050_GET_BULLET(float distance)
{
	uint8_t BULLET;
	float distance_erro;
	if (distance < BLOCK_BOM + 1)
		BULLET = 16;
	else if (distance > BLOCK_TOP - 1)
		BULLET = 0;
	else // ����->����
	{
		// if ((16 - (uint8_t)((distance - FDATA.R.BLOCK_BOM + 3.9 / 6) / 3.9)) == ((uint8_t)((FDATA.R.BLOCK_TOP - distance + 3.9 / 6) / 3.9)))
		// 	BULLET = 16 - (uint8_t)((distance - FDATA.R.BLOCK_BOM + 3.9 / 6) / 3.9);
		distance_erro = distance - BLOCK_BOM;
		if (distance_erro < 0)
			distance_erro = 0;
		BULLET = 16 - (uint8_t)((distance_erro + 3.9 / 3) / 4);
	}
	if (BULLET > 16 & BULLET < 20)
		BULLET = 16;
	if (BULLET > 100)
		BULLET = 0;
	return BULLET;
}

void TOF050_Handler(void)
{
	/* �˲� -> ����
	 * ���㵯��
	 */

	/* �˲�
	 * �任
	 * ��ȡ����
	 * ���㵯��
	 */

	// ��ȡ����
	TOF050_Read_range();
	/*�˲�*/
	Filter(TOF_range, &BLOCK_DISTANCE);
	*BLOCK_BULLET_NUM = TOF050_GET_BULLET(BLOCK_DISTANCE);

	BLOCK_BULLET_NUM_LSAT = *BLOCK_BULLET_NUM;
	if (BLOCK_BULLET_NUM_LSAT != *BLOCK_BULLET_NUM) // �����仯ʱ��ճ�ʱ��¼
		Power_TimeOUT_clearCNT();

	// LED_Show_Num(*BLOCK_BULLET_NUM); 

	T_printf("distance:%u,%.2f,%u\n", TOF_range, BLOCK_DISTANCE, *BLOCK_BULLET_NUM);

	// if (1 == TOF_Handler_Flag)
	// {
	// 	T_printf("range:%u\n", TOF_range);
	// 	TOF_Handler_Flag = 0;
	// }
}

void SET_TOP(float TOP)
{
	if (TOP != NULL)
		FDATA.R.BLOCK_TOP = TOP;
	else
		FDATA.R.BLOCK_TOP = BLOCK_DISTANCE;
	printf("BOM IS %f\nTOP IS %f\n", FDATA.R.BLOCK_BOM, FDATA.R.BLOCK_TOP);
}

void SET_BOM(float BOM)
{
	if (BOM != NULL)
		FDATA.R.BLOCK_BOM = BOM;
	else
		BLOCK_BOM = BLOCK_DISTANCE - 64;
	printf("BOM IS %f\nTOP IS %f\n", FDATA.R.BLOCK_BOM, FDATA.R.BLOCK_TOP);
}
