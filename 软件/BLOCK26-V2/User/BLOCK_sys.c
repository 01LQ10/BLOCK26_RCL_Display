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
    /* ��ʼ���������裬Flash�ӿڣ�SysTick */
    HAL_Init();
    APP_SystemClockConfig(); // 24MHz
    // flash_load();

    HAL_Delay(1000); // ����������ʱ

    /*��ʼ�����������������*/
    Debug_Init(); // �����ж�
    P_printf(">> BLOCK system Init ...\n");
    Power_Init(); // ��ʼ����Դ�������ţ���ص�ѹ���
    Power_CT_ON();
    LED_Init(); // �����ж�
    LED_Scan_IT_Start();
    BUTN_GPIO_init();
    /*����Ƿ���Ͽ���Ҫ��*/
    Power_ADC_get_Vbat(); // �������
                          //

    // �жϿ���״̬

    // ledָʾ
    LED_Blink();
    while (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == 0) // �ȴ������ɿ�
    	;

    TOF050_Init(&BLOCK_26.bullet_num); // �����ж�
    OS_Init();                         // �����ж�
    Hall_Init();                       // ��������
    BUTN_Init(BLOCK_Sys_Key_single_Handle,
              BLOCK_Sys_Key_double_Handle,
              BLOCK_Sys_Key_long_Handle); // ��������
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

    Debug_RX_IT_Start(); // �����������
    OS_IT_Start();       // �����������

    printf("\n>> Welcome to BLOCK-26\r\n");
    printf("SystemCoreClock:%u\n", SystemCoreClock);
    // ���ϵͳ״̬����
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

