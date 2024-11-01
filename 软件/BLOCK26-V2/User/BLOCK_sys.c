#include "BLOCK_sys.h"

BLOCK_t BLOCK_26;

void HALL(void);
void BLOCK_Sys_Key_single_Handle(void);
void BLOCK_Sys_Key_double_Handle(void);
void BLOCK_Sys_Key_long_Handle(void);

void LED_SHOW_VBAT(void); // 100ms
void LED_SHOW_NUM(void);

void uprint_test(void)
{
    static uint8_t i;
    i++;
    U_printf("hello,%d\n", i);
}

void BLOCK_Sys_Init(void)
{
    /* 初始化所有外设，Flash接口，SysTick */
    HAL_Init();
    APP_SystemClockConfig(); // 24MHz
    // flash_load();

    HAL_Delay(1000); // 长按开机延时

    /*初始化开机检测所需外设*/
    Debug_Init(); // 依赖中断
    P_printf(">> BLOCK system Init ...\n");
    Power_Init(); // 初始化电源控制引脚，电池电压检测
    Power_CT_ON();
    LED_Init(); // 依赖中断
    LED_Scan_IT_Start();
    BUTN_GPIO_init();
    /*检测是否符合开机要求*/
    Power_ADC_get_Vbat(); // 电量检测
                          //

    // 判断开机状态

    // led指示
    LED_Blink();
    while (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == 0) // 等待按键松开
    	;

    TOF050_Init(&BLOCK_26.bullet_num); // 依赖中断
    OS_Init();                         // 依赖中断
    Hall_Init();                       // 依赖任务
    BUTN_Init(BLOCK_Sys_Key_single_Handle,
              BLOCK_Sys_Key_double_Handle,
              BLOCK_Sys_Key_long_Handle); // 依赖任务
}

void BLOCK_Sys_Start(void)
{
    P_printf(">> BLOCK system Start\n");

    // OS_ADD_TASK_MS(100, HALL);
    OS_ADD_TASK_MS(TICKS_INTERVAL, BUTN_tick);
    OS_ADD_TASK_MS(50, TOF050_Handler);
    OS_ADD_TASK_MS(50, LED_SHOW_NUM);
    OS_ADD_TASK_MS(TIMEOUT_tick, Power_TimeOUT_Handle);
    // OS_ADD_TASK_MS(100, LED_Flow);

    Debug_RX_IT_Start(); // 命令控制启动
    OS_IT_Start();       // 任务调度启动

    printf("\n>> Welcome to BLOCK-26\r\n");
    printf("SystemCoreClock:%u\n", SystemCoreClock);
    // 输出系统状态数据
}

void BLOCK_Sys_Close(void)
{
}

void BLOCK_Sys_Key_long_Handle(void)
{
    LED_Blink();
    LED_Show_Num(0);
    Power_CT_OFF();
    while (1)
        ;
}

void BLOCK_Sys_Key_double_Handle(void)
{
    Power_TimeOUT_clearCNT();
}

void BLOCK_Sys_Key_single_Handle(void)
{
    Power_TimeOUT_clearCNT();
    OS_DEL_Task(LED_SHOW_NUM);
    OS_ADD_TASK_MS(100,LED_SHOW_VBAT);
    // static uint8_t i = 1;
    // if (i == 0)
    // {
    //     OS_ADD_TASK_MS(100, LED_Flow);
    //     printf("add led task\n");
    //     i = 1;
    // }
    // else if (i == 1)
    // {
    //     OS_DEL_Task(LED_Flow);
    //     printf("del led task\n");
    //     i = 0;
    // }
}
void HALL(void)
{
    if (BOX_IN == Hall_get_state())
        LED_Show_Num(16);
    else
        LED_Show_Num(0);
}

void LED_SHOW_VBAT(void) // 100ms
{
    static uint8_t t = 0;
    static uint16_t buff = 0xF000;
    if(t==0)
    {
        float Vbat = Power_ADC_get_Vbat();
        if (Vbat < 3.85)
            buff = 0xF000;
        else if (Vbat < 3.95)
            buff = 0x0F00;
        else if (Vbat < 4.1)
            buff = 0x00F0;
        else
            buff = 0x000F;
    }
    if(t%10 == 0)
        LED_Set_Buff(buff);
    else if(t%5 == 0)
        LED_Show_Clear();

    t++;

    if(t>= 20)
    {
        t = 0;
        buff = 0xF000;
        OS_DEL_Task(LED_SHOW_VBAT);
        OS_ADD_TASK_MS(50, LED_SHOW_NUM);
    }
}

void LED_SHOW_NUM(void) // 50ms
{
    LED_Show_Num(BLOCK_26.bullet_num); 
}

