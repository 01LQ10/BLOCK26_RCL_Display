#include "Hall.h"

void Hall_Init(void)
{
    P_printf("Hall Init...\n");
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* 使能GPIOB时钟 */
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.Mode = GPIO_MODE_INPUT; /* 推挽输出 */
    GPIO_Struct.Pull = GPIO_PULLDOWN;
    GPIO_Struct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度 */
    GPIO_Struct.Pin = Hall_PIN;
    HAL_GPIO_Init(Hall_PORT, &GPIO_Struct);
    P_printf("Hall Init OK\n");
}

Hall_state Hall_get_state(void)
{
    return (Hall_state)HAL_GPIO_ReadPin(Hall_PORT, Hall_PIN);
}
