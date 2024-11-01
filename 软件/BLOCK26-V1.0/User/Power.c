#include "Power.h"
// CNT clear ������ [1]���������ı仯[2]��������[3]debug����ͨ��
uint16_t TimeOUT_CNT = 0;

void Power_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* ʹ��GPIOBʱ�� */
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP; /* ������� */
    GPIO_Struct.Pull = GPIO_PULLDOWN;
    GPIO_Struct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO�ٶ� */
    GPIO_Struct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOB, &GPIO_Struct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (GPIO_PinState)1);
}

void Power_TimeOUT_Handle(void)
{
    TimeOUT_CNT++;
    if (TimeOUT_CNT > 60 /*s*/ * 1000 / TIMEOUT_tick)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (GPIO_PinState)0); // �ػ�
    }
}

void Power_TimeOUT_clearCNT(void)
{
    TimeOUT_CNT = 0;
}
