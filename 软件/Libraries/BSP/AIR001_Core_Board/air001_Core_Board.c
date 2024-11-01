#include "air001_Core_Board.h"

#define __AIR001_CORE_BOARD_BSP_VERSION_MAIN (0x01U) /*!< [31:24] main version */
#define __AIR001_CORE_BOARD_BSP_VERSION_SUB1 (0x00U) /*!< [23:16] sub1 version */
#define __AIR001_CORE_BOARD_BSP_VERSION_SUB2 (0x00U) /*!< [15:8]  sub2 version */
#define __AIR001_CORE_BOARD_BSP_VERSION_RC (0x00U)   /*!< [7:0]  release candidate */
#define __AIR001_CORE_BOARD_BSP_VERSION ((__AIR001_CORE_BOARD_BSP_VERSION_MAIN << 24) | (__AIR001_CORE_BOARD_BSP_VERSION_SUB1 << 16) | (__AIR001_CORE_BOARD_BSP_VERSION_SUB2 << 8) | (__AIR001_CORE_BOARD_BSP_VERSION_RC))

#ifdef HAL_UART_MODULE_ENABLED
UART_HandleTypeDef DebugUartHandle;
#endif
GPIO_TypeDef *LED_PORT[LEDn] = {LED1_GPIO_PORT};
const uint16_t LED_PIN[LEDn] = {LED1_PIN};

GPIO_TypeDef *BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};
const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};
const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

/** @addtogroup AIR001_CORE_BOARD_Exported_Functions
 * @{
 */

/**
 * @brief 璇ユ柟娉曡繑鍥濧IR001 CORE BOARD BSP Driver鐨勭増鏈�銆�
 * @retval version : 0xXYZR (8bits for each decimal, R for RC)
 */
uint32_t BSP_GetVersion(void)
{
  return __AIR001_CORE_BOARD_BSP_VERSION;
}

/** @addtogroup LED_Functions
 * @{
 */

/**
 * @brief 閰嶇疆LED鐨凣PIO銆�
 * @param Led 鎸囧畾瑕侀厤缃�鐨凩ED銆�
 * 杩欎釜鍙傛暟鍙�浠ユ槸浠ヤ笅鐨勫弬鏁颁箣涓€锛�
 * @arg LED1
 * @retval 鏃�
 */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(Led);

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/**
 * @brief DeInitialize LED GPIO.
 * @param Led 鎸囧畾瑕佸幓閰嶇疆鐨凩ED銆�
 * 杩欎釜鍙傛暟鍙�浠ユ槸浠ヤ笅鍊间箣涓€锛�
 * @arg LED1
 * @娉ㄦ剰 BSP_LED_DeInit()涓嶄細绂佺敤GPIO鏃堕挓銆�
 * @retval 鏃�
 */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
  /* DeInit the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  HAL_GPIO_DeInit(LED_PORT[Led], GPIO_InitStruct.Pin);
}

/**
 * @brief 灏嗛€夊畾鐨凩ED鎵撳紑銆�
 * @param Led 鎸囧畾瑕佽�剧疆鐨凩ED銆�
 * 杩欎釜鍙傛暟鍙�浠ユ槸浠ヤ笅鐨勫弬鏁颁箣涓€锛�
 * @arg LED1
 * @retval 鏃�
 */
void BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief 灏嗛€夊畾鐨凩ED鍏抽棴銆�
 * @param Led 鎸囧畾瑕佸叧闂�鐨凩ED銆�
 * 杩欎釜鍙傛暟鍙�浠ユ槸涓嬪垪鍙傛暟涔嬩竴锛�
 * @arg LED1
 * @retval 鏃�
 */
void BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/**
 * @brief 鍒囨崲閫夊畾鐨凩ED鐢靛钩銆�
 * @param Led 鎸囧畾瑕佸垏鎹㈢殑LED銆�
 * 杩欎釜鍙傛暟鍙�浠ユ槸涓嬪垪鍙傛暟涔嬩竴锛�
 * @arg LED1
 * @retval 鏃�
 */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}

/**
 * @brief 閰嶇疆鎸夐挳鐨凣PIO鍜孍XTI绾裤€�
 * @param Button锛� 鎸囧畾瑕侀厤缃�鐨勬寜閽�銆�
 *杩欎釜鍙傛暟搴旇�ユ槸锛� BUTTON_USER
 * @param ButtonMode锛� 鎸囧畾鎸夐挳妯″紡銆�
 * 杩欎釜鍙傛暟鍙�浠ユ槸浠ヤ笅鐨勫弬鏁颁箣涓€锛�
 * @arg BUTTON_MODE_GPIO: 鎸夐挳灏嗚��鐢ㄤ綔鏅�閫欼O銆�
 * @arg BUTTON_MODE_EXTI: 鎸夐挳灏嗚��杩炴帴鍒板甫鏈変腑鏂�鐨凟XTI绾裤€�
 * @retval 鏃�
 */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpioinitstruct;

  /* Enable the BUTTON Clock */
  BUTTONx_GPIO_CLK_ENABLE(Button);

  gpioinitstruct.Pin = BUTTON_PIN[Button];
  gpioinitstruct.Pull = GPIO_NOPULL;
  gpioinitstruct.Speed = GPIO_SPEED_FREQ_MEDIUM;

  if (ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpioinitstruct.Mode = GPIO_MODE_INPUT;

    HAL_GPIO_Init(BUTTON_PORT[Button], &gpioinitstruct);
  }

  if (ButtonMode == BUTTON_MODE_EXTI)
  {
    /* 閰嶇疆涓轰笂鍗囨部涓�鏂� */
    gpioinitstruct.Mode = GPIO_MODE_IT_RISING;
    HAL_GPIO_Init(BUTTON_PORT[Button], &gpioinitstruct);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0);
    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  }
}

/**
 * @brief Push Button DeInit.
 *@param Button锛� 瑕侀厤缃�鐨勬寜閽�
 *杩欎釜鍙傛暟搴旇�ユ槸锛� BUTTON_USER
 * @娉ㄦ剰 PB DeInit涓嶄細绂佺敤GPIO鏃堕挓
 * @retval 鏃�
 */
void BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
}

/**
 * @brief 杩斿洖閫夊畾鐨凚utton鐘舵€併€�
 *@param Button锛� 鎸囧畾瑕佹�€鏌ョ殑Button銆�
 *杩欎釜鍙傛暟搴旇�ユ槸锛� BUTTON_USER
 * @retval Button state.
 */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

#ifdef HAL_UART_MODULE_ENABLED
/**
 * @brief DEBUG_USART GPIO閰嶇疆,妯″紡閰嶇疆,115200 8-N-1
 * @param None
 * @retval 鏃�
 */
void BSP_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  DEBUG_USART_CLK_ENABLE();

  DebugUartHandle.Instance = DEBUG_USART;

  DebugUartHandle.Init.BaudRate = DEBUG_USART_BAUDRATE;
  DebugUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  DebugUartHandle.Init.StopBits = UART_STOPBITS_1;
  DebugUartHandle.Init.Parity = UART_PARITY_NONE;
  DebugUartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  DebugUartHandle.Init.Mode = UART_MODE_TX_RX;

  HAL_UART_Init(&DebugUartHandle);

  DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();

  /**USART GPIO Configuration
    PA2     ------> USART1_TX
    PA3     ------> USART1_RX
    */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;

  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);

  /* ENABLE NVIC */
  HAL_NVIC_SetPriority(DEBUG_USART_IRQ, 0, 1);
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ);
}

#if (defined(__CC_ARM)) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/**
 * @brief 鍐欎竴涓�瀛楃�﹀埌usart涓�銆�
 *@param ch
 * *f
 *@retval 璇ュ瓧绗�
 */
int fputc(int ch, FILE *f)
{
  /* Send a byte to USART */
  HAL_UART_Transmit(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);

  return (ch);
}

/**
 *@brief 浠巙sart涓�鑾峰彇涓€涓�瀛楃��
 *@param *f
 *@retval 涓€涓�瀛楃��
 */
int fgetc(FILE *f)
{
  int ch;
  HAL_UART_Receive(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);
  return (ch);
}

#elif defined(__ICCARM__)
/**
 * @brief 鍐欎竴涓�瀛楃�﹀埌usart涓�銆�
 *@param ch
 * *f
 *@retval 璇ュ瓧绗�
 */
int putchar(int ch)
{
  /* Send a byte to USART */
  HAL_UART_Transmit(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);

  return (ch);
}
#endif

#endif
