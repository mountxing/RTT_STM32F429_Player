

#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include <rtthread.h>

#define MY_BUTTON_DOWN_MS        50
#define MY_BUTTON_HOLD_MS        700

#define MY_BUTTON_SCAN_SPACE_MS  20
#define MY_BUTTON_LIST_MAX       10

typedef void (*my_button_callback)(void*);

enum my_button_event
{
    BUTTON_EVENT_CLICK_DOWN, /* 按键单击按下事件 */
    BUTTON_EVENT_CLICK_UP,   /* 按键单击结束事件 */
    BUTTON_EVENT_HOLD,       /* 按键长按开始事件 */
    BUTTON_EVENT_HOLD_CYC,   /* 按键长按周期性触发事件 */
    BUTTON_EVENT_HOLD_UP,    /* 按键长按结束事件 */
    BUTTON_EVENT_NONE        /* 无按键事件 */
};

struct my_button
{
    rt_uint8_t  press_logic_level;  /* 按键按下时的电平 */
    rt_uint16_t cnt;                /* 连续扫描到按下状态的次数 */
    rt_uint16_t hold_cyc_period;    /* 长按周期回调的周期 */
    rt_uint16_t pin;                /* 按键对应的 pin 编号 */

    enum my_button_event event;     /* 按键的触发的事件 */

    my_button_callback cb;          /* 按键触发事件回调函数 */
};

int my_button_register(struct my_button *button);
int my_button_start(void);

#endif
