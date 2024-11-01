#include "Power.h"
// CNT clear ������ [1]���������ı仯[2]��������[3]debug����ͨ��
uint16_t TimeOUT_CNT = 0;

/*PB1 ADC_IN9*/
ADC_HandleTypeDef AdcHandle;
ADC_ChannelConfTypeDef sConfig;
float VBat_Value;

void Power_Init(void)
{
    Power_CT_Init();
    Power_ADC_Init();
}

void Power_CT_Init(void)
{
    printf("Power_CT Init ...\n");
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* ʹ��GPIOBʱ�� */
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP; /* ������� */
    GPIO_Struct.Pull = GPIO_PULLDOWN;
    GPIO_Struct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO�ٶ� */
    GPIO_Struct.Pin = POW_CT_PIN;
    HAL_GPIO_Init(POW_CT_PORT, &GPIO_Struct);
    printf("Power_CT Init OK\n");
    // Power_CT_ON();
}

void Power_CT_ON(void)
{
    HAL_GPIO_WritePin(POW_CT_PORT, POW_CT_PIN, (GPIO_PinState)1);
}
void Power_CT_OFF(void)
{
    HAL_GPIO_WritePin(POW_CT_PORT, POW_CT_PIN, (GPIO_PinState)0);
}

void Power_ADC_Init(void)
{
    printf("ADC Init...\n");
    __HAL_RCC_ADC_FORCE_RESET();
    __HAL_RCC_ADC_RELEASE_RESET(); /* ��λADC */
    __HAL_RCC_ADC_CLK_ENABLE();    /* ADCʱ��ʹ�� */

    AdcHandle.Instance = ADC1;

    /* ADCУ׼ */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
    {
        printf("ADC calibration erro\n");
    }

    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;            /* ����ADCʱ�� */
    AdcHandle.Init.Resolution = ADC_RESOLUTION_10B;                      /* ת���ֱ���12bit */
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* �����Ҷ��� */
    AdcHandle.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;            /* ɨ�����з������� */
    AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* ���β��� */
    AdcHandle.Init.LowPowerAutoWait = ENABLE;                            /* �ȴ�ת��ģʽ����*/
    AdcHandle.Init.ContinuousConvMode = DISABLE;                         /* ����ת��ģʽ */
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;                      /* ��ʹ�ܷ�����ģʽ */
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* ������� */
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* ���������� */
    AdcHandle.Init.DMAContinuousRequests = DISABLE;                      /* DMA��ʹ�� */
    AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* �����ط���ʱ��������һ��ֵ  */
    AdcHandle.Init.SamplingTimeCommon = ADC_SAMPLETIME_41CYCLES_5;       /* ���ò�������41.5��ADCʱ�� */
    /* ADC��ʼ�� */
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        printf("ADC Init erro\n");
    }

    sConfig.Channel = POW_ADC_CHANNEL;      /* ���ò���ͨ�� */
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER; /* ���ü��������ͨ�� */
    /* ����ADCͨ�� */
    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        printf("ADC channel erro\n");
    }
    else
        printf("ADC Init OK\n");
}
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_ADC_CLK_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = POW_ADC_PIN;
    HAL_GPIO_Init(POW_ADC_PORT, &GPIO_InitStruct);
}

float Power_ADC_get_Vbat(void)
{
    uint16_t adc_value;
    /* ����ADC */
    HAL_ADC_Start(&AdcHandle);

    /* �ȴ�ADCת����� */
    HAL_ADC_PollForConversion(&AdcHandle, 0xFFFF);

    /* ��ȡADCֵ */
    adc_value = (uint16_t)HAL_ADC_GetValue(&AdcHandle);

    /* ����VCC��ѹ */
    VBat_Value = (float)adc_value / 1024 * 3.3 * 2;
    T_printf("ADC: %u\n", adc_value);
    T_printf("VCC: %f V\r\n", VBat_Value);

    return VBat_Value;
}

void Power_TimeOUT_Handle(void)
{
    TimeOUT_CNT++;
    if (TimeOUT_CNT > 3/*min*/ * 60 /*s*/ * 1000 / TIMEOUT_tick)
    {
        Power_CT_OFF(); // �ػ�
    }
}

void Power_TimeOUT_clearCNT(void)
{
    TimeOUT_CNT = 0;
}
