#ifndef __KEY_H
#define __KEY_H

#include "air001xx_hal.h"
#include <string.h>
#include "LED.h"
#include "TOF050.h"

/*======================自定义========================*/
#define KEY_PORT GPIOB
#define KEY_PIN GPIO_PIN_3
#define KEY_CLOCK() __HAL_RCC_GPIOB_CLK_ENABLE()
/*======================自定义========================*/

// According to your need to modify the constants.
#define TICKS_INTERVAL 5 // ms
#define DEBOUNCE_TICKS 3 // MAX 7 (0 ~ 7) //去抖
#define SHORT_TICKS (300 / TICKS_INTERVAL)
#define LONG_TICKS (700 / TICKS_INTERVAL)

typedef void (*BtnCallback)(void *);

typedef enum
{
    PRESS_DOWN = 0,
    PRESS_UP,
    PRESS_REPEAT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS_START,
    LONG_PRESS_HOLD,
    number_of_event,
    NONE_PRESS
} PressEvent;

typedef struct Button
{
    uint16_t ticks;
    uint8_t repeat;
    uint8_t event;
    uint8_t state;
    uint8_t debounce_cnt;
    uint8_t active_level;
    uint8_t button_level;
    uint8_t button_id;
    uint8_t (*hal_button_Level)(uint8_t button_id_);
    BtnCallback cb[number_of_event];
    struct Button *next;
} Button;

void button_init(struct Button *handle, uint8_t (*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id);
void button_attach(struct Button *handle, PressEvent event, BtnCallback cb);
PressEvent get_button_event(struct Button *handle);
int button_start(struct Button *handle);
void button_stop(struct Button *handle);
void BUTN_tick(void);
void BUTN_GPIO_init(void);
void BUTN_init(void);

#endif
