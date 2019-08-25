/************************************************************
  * @brief   BUTTON Control
  * @author  mountxing
  * @date    2018-08-xx
  * @version v1.0.0
  * @note    button.c
  ***********************************************************/

#include <rtdevice.h>
#include "button.h"

/****    Debug     ****/
#define DBG_ENABLE
#define DBG_SECTION_NAME "button"
#define DBG_LEVEL        DBG_INFO
#define DBG_COLOR

#include <rtdbg.h>

#define MY_BUTTON_CALL(func, argv) \
    do { if ((func) != RT_NULL) func argv; } while (0)

struct my_button_manage
{
    rt_uint8_t num;                 /* 已注册的按键的数目 */
    rt_timer_t timer;               /* 按键扫描用到的定时器 */
    struct my_button *button_list[MY_BUTTON_LIST_MAX];  /* 存储按键指针的数组 */
};
static struct my_button_manage button_manage;


int my_button_register(struct my_button *button)
{
    /* 初始化按键对应的 pin 模式 */
    if (button->press_logic_level == 0)
    {
        rt_pin_mode(button->pin, PIN_MODE_INPUT_PULLUP);
    }
    else
    {
        rt_pin_mode(button->pin, PIN_MODE_INPUT_PULLDOWN);
    }

    /* 初始化按键结构体 */
    button->cnt = 0;
    button->event = BUTTON_EVENT_NONE;

    /* 添加按键到管理列表 */
    button_manage.button_list[button_manage.num++] = button;

    return 0;
}

static void my_button_scan(void *param)
{
    rt_uint8_t i;
    rt_uint16_t cnt_old;

    for (i = 0; i < button_manage.num; i++)
    {
        cnt_old = button_manage.button_list[i]->cnt;

        /* 检测按键的电平状态为按下状态 */
        if (rt_pin_read(button_manage.button_list[i]->pin) == button_manage.button_list[i]->press_logic_level)
        {
            /* 按键扫描的计数值加一 */
            button_manage.button_list[i]->cnt ++;

            /* 连续按下的时间达到单击按下事件触发的阈值 */
            if (button_manage.button_list[i]->cnt == MY_BUTTON_DOWN_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_DOWN */
            {
                LOG_D("BUTTON_DOWN");
                button_manage.button_list[i]->event = BUTTON_EVENT_CLICK_DOWN;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            /* 连续按下的时间达到长按开始事件触发的阈值 */
            else if (button_manage.button_list[i]->cnt == MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD */
            {
                LOG_D("BUTTON_HOLD");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            /* 连续按下的时间达到长按周期回调事件触发的阈值 */
            else if (button_manage.button_list[i]->cnt > MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD_CYC */
            {
                LOG_D("BUTTON_HOLD_CYC");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD_CYC;
                if (button_manage.button_list[i]->hold_cyc_period && button_manage.button_list[i]->cnt % (button_manage.button_list[i]->hold_cyc_period / MY_BUTTON_SCAN_SPACE_MS) == 0)
                    MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
        }
        /* 检测按键的电平状态为抬起状态 */
        else
        {
            /* 清除按键的计数值 */
            button_manage.button_list[i]->cnt = 0;
            /* 连续按下的时间达到单击结束事件触发的阈值 */
            if (cnt_old >= MY_BUTTON_DOWN_MS / MY_BUTTON_SCAN_SPACE_MS && cnt_old < MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_CLICK_UP */
            {
                LOG_D("BUTTON_CLICK_UP");
                button_manage.button_list[i]->event = BUTTON_EVENT_CLICK_UP;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            /* 连续按下的时间达到长按结束事件触发的阈值 */
            else if (cnt_old >= MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD_UP */
            {
                LOG_D("BUTTON_HOLD_UP");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD_UP; 
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
        }
    }
}


int my_button_start()
{
    if (button_manage.timer != RT_NULL)
        return -1;

    /* 创建定时器1 */
    button_manage.timer = rt_timer_create("timer1",      /* 定时器名字是 timer1 */
                               my_button_scan,           /* 超时时回调的处理函数 */
                               RT_NULL,                  /* 超时函数的入口参数 */
                               RT_TICK_PER_SECOND * MY_BUTTON_SCAN_SPACE_MS / 1000, /* 定时长度，以OS Tick为单位，即10个OS Tick */
                               RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER); /* 周期性定时器 */
    /* 启动定时器 */
    if (button_manage.timer != RT_NULL)
        rt_timer_start(button_manage.timer);

    return 0;
}



