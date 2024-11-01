#include "KEY.h"

#include "Power.h"
#include "LED.h"
#include "TOF050.h"
/*======================自定义========================*/
struct Button btn1;

uint8_t BUTN_GPIO_read(uint8_t id)
{
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == 1)
        return 1;
    else
        return 0;
}

void BTN1_SINGLE_Handler(void *bnt)
{
    Power_TimeOUT_clearCNT();
    // SET_BOM(NULL);
    // flash_save();
    // static uint8_t i = 0;
    // LED_Show_Num(i++);
    // if (i == 17)
    // 	i = 0;
}
void BTN1_DOUBLE_Handler(void *bnt)
{
    Power_TimeOUT_clearCNT();
    // SET_BOM(NULL);
    flash_save();
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (GPIO_PinState)1);
    // printf("SET POW-CT 1\n");
}
void BTN1_LONG_PRESS_Handler(void *bnt)
{
    LED_Blink();
    LED_Show_Num(0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (GPIO_PinState)0);
    while (1)
        ;
    // printf("SET POW-CT 0\n");
    // flash_save();
}
void BUTN_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    KEY_CLOCK();
    /* GPIO初始化 */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;           /* 上拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度 */
    GPIO_InitStruct.Pin = KEY_PIN;
    HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);
}
void BUTN_Init(void (*butn_single_handle)(void), void (*butn_double_handle)(void), void (*butn_long_handle)(void))
{
    printf("BUTN Init ...\n");
    BUTN_GPIO_init();
    button_init(&btn1, BUTN_GPIO_read, 0, 0);
    button_attach(&btn1, SINGLE_CLICK, butn_single_handle);
    button_attach(&btn1, DOUBLE_CLICK, butn_double_handle);
    button_attach(&btn1, LONG_PRESS_START, butn_long_handle);
    button_start(&btn1);
    printf("BUTN Init OK\n");
}

/*======================自定义========================*/

#define EVENT_CB(ev)    \
    if (handle->cb[ev]) \
    handle->cb[ev]((void *)handle)
#define PRESS_REPEAT_MAX_NUM 15 /*!< The maximum value of the repeat counter */

// button handle list head.
static struct Button *head_handle = NULL;

static void button_handler(struct Button *handle);

/**
 * @brief  Initializes the button struct handle.
 * @param  handle: the button handle struct.
 * @param  pin_level: read the HAL GPIO of the connected button level.
 * @param  active_level: pressed GPIO level.
 * @param  button_id: the button id.
 * @retval None
 */
void button_init(struct Button *handle, uint8_t (*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id)
{
    memset(handle, 0, sizeof(struct Button));
    handle->event = (uint8_t)NONE_PRESS;
    handle->hal_button_Level = pin_level;
    handle->button_level = handle->hal_button_Level(button_id);
    handle->active_level = active_level;
    handle->button_id = button_id;
}

/**
 * @brief  Attach the button event callback function.
 * @param  handle: the button handle struct.
 * @param  event: trigger event type.
 * @param  cb: callback function.
 * @retval None
 */
void button_attach(struct Button *handle, PressEvent event, BtnCallback cb)
{
    handle->cb[event] = cb;
}

/**
 * @brief  Inquire the button event happen.
 * @param  handle: the button handle struct.
 * @retval button event.
 */
PressEvent get_button_event(struct Button *handle)
{
    return (PressEvent)(handle->event);
}

/**
 * @brief  Button driver core function, driver state machine.
 * @param  handle: the button handle struct.
 * @retval None
 */
static void button_handler(struct Button *handle)
{
    uint8_t read_gpio_level = handle->hal_button_Level(handle->button_id);

    // ticks counter working..
    if ((handle->state) > 0)
        handle->ticks++;

    /*------------button debounce handle---------------*/
    if (read_gpio_level != handle->button_level)
    { // not equal to prev one
        // continue read 3 times same new level change
        if (++(handle->debounce_cnt) >= DEBOUNCE_TICKS)
        {
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    }
    else
    { // level not change ,counter reset.
        handle->debounce_cnt = 0;
    }

    /*-----------------State machine-------------------*/
    switch (handle->state)
    {
    case 0:
        if (handle->button_level == handle->active_level)
        { // start press down
            handle->event = (uint8_t)PRESS_DOWN;
            EVENT_CB(PRESS_DOWN);
            handle->ticks = 0;
            handle->repeat = 1;
            handle->state = 1;
        }
        else
        {
            handle->event = (uint8_t)NONE_PRESS;
        }
        break;

    case 1:
        if (handle->button_level != handle->active_level)
        { // released press up
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            handle->ticks = 0;
            handle->state = 2;
        }
        else if (handle->ticks > LONG_TICKS)
        {
            handle->event = (uint8_t)LONG_PRESS_START;
            EVENT_CB(LONG_PRESS_START);
            handle->state = 5;
        }
        break;

    case 2:
        if (handle->button_level == handle->active_level)
        { // press down again
            handle->event = (uint8_t)PRESS_DOWN;
            EVENT_CB(PRESS_DOWN);
            if (handle->repeat != PRESS_REPEAT_MAX_NUM)
            {
                handle->repeat++;
            }
            EVENT_CB(PRESS_REPEAT); // repeat hit
            handle->ticks = 0;
            handle->state = 3;
        }
        else if (handle->ticks > SHORT_TICKS)
        { // released timeout
            if (handle->repeat == 1)
            {
                handle->event = (uint8_t)SINGLE_CLICK;
                EVENT_CB(SINGLE_CLICK);
            }
            else if (handle->repeat == 2)
            {
                handle->event = (uint8_t)DOUBLE_CLICK;
                EVENT_CB(DOUBLE_CLICK); // repeat hit
            }
            handle->state = 0;
        }
        break;

    case 3:
        if (handle->button_level != handle->active_level)
        { // released press up
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            if (handle->ticks < SHORT_TICKS)
            {
                handle->ticks = 0;
                handle->state = 2; // repeat press
            }
            else
            {
                handle->state = 0;
            }
        }
        else if (handle->ticks > SHORT_TICKS)
        { // SHORT_TICKS < press down hold time < LONG_TICKS
            handle->state = 1;
        }
        break;

    case 5:
        if (handle->button_level == handle->active_level)
        {
            // continue hold trigger
            handle->event = (uint8_t)LONG_PRESS_HOLD;
            EVENT_CB(LONG_PRESS_HOLD);
        }
        else
        { // released
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            handle->state = 0; // reset
        }
        break;
    default:
        handle->state = 0; // reset
        break;
    }
}

/**
 * @brief  Start the button work, add the handle into work list.
 * @param  handle: target handle struct.
 * @retval 0: succeed. -1: already exist.
 */
int button_start(struct Button *handle)
{
    struct Button *target = head_handle;
    while (target)
    {
        if (target == handle)
            return -1; // already exist.
        target = target->next;
    }
    handle->next = head_handle;
    head_handle = handle;
    return 0;
}

/**
 * @brief  Stop the button work, remove the handle off work list.
 * @param  handle: target handle struct.
 * @retval None
 */
void button_stop(struct Button *handle)
{
    struct Button **curr;
    for (curr = &head_handle; *curr;)
    {
        struct Button *entry = *curr;
        if (entry == handle)
        {
            *curr = entry->next;
            //			free(entry);
            return; // glacier add 2021-8-18
        }
        else
        {
            curr = &entry->next;
        }
    }
}

/**
 * @brief  background ticks, timer repeat invoking interval 5ms.
 * @param  None.
 * @retval None
 */
void BUTN_tick(void)
{
    struct Button *target;
    for (target = head_handle; target; target = target->next)
    {
        button_handler(target);
    }
}
