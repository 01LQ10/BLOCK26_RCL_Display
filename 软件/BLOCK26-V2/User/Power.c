#include "Power.h"
// CNT clear 的条件 [1]弹量参数的变化[2]按键操作[3]debug串口通信
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
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* 使能GPIOB时钟 */
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP; /* 推挽输出 */
    GPIO_Struct.Pull = GPIO_PULLDOWN;
    GPIO_Struct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度 */
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
    __HAL_RCC_ADC_RELEASE_RESET(); /* 复位ADC */
    __HAL_RCC_ADC_CLK_ENABLE();    /* ADC时钟使能 */

    AdcHandle.Instance = ADC1;

    /* ADC校准 */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
    {
        printf("ADC calibration erro\n");
    }

    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;            /* 设置ADC时钟 */
    AdcHandle.Init.Resolution = ADC_RESOLUTION_10B;                      /* 转换分辨率12bit */
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* 数据右对齐 */
    AdcHandle.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;            /* 扫描序列方向：向上 */
    AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* 单次采样 */
    AdcHandle.Init.LowPowerAutoWait = ENABLE;                            /* 等待转换模式开启*/
    AdcHandle.Init.ContinuousConvMode = DISABLE;                         /* 单次转换模式 */
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;                      /* 不使能非连续模式 */
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 触发边沿无 */
    AdcHandle.Init.DMAContinuousRequests = DISABLE;                      /* DMA不使能 */
    AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* 当过载发生时，覆盖上一个值  */
    AdcHandle.Init.SamplingTimeCommon = ADC_SAMPLETIME_41CYCLES_5;       /* 设置采样周期41.5个ADC时钟 */
    /* ADC初始化 */
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        printf("ADC Init erro\n");
    }

    sConfig.Channel = POW_ADC_CHANNEL;      /* 设置采样通道 */
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER; /* 设置加入规则组通道 */
    /* 配置ADC通道 */
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
    /* 启动ADC */
    HAL_ADC_Start(&AdcHandle);

    /* 等待ADC转换完成 */
    HAL_ADC_PollForConversion(&AdcHandle, 0xFFFF);

    /* 获取ADC值 */
    adc_value = (uint16_t)HAL_ADC_GetValue(&AdcHandle);

    /* 计算VCC电压 */
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
        Power_CT_OFF(); // 关机
    }
}

void Power_TimeOUT_clearCNT(void)
{
    TimeOUT_CNT = 0;
}
