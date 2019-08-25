/************************************************************
  * @brief   LED
	* @param   NULL
  * @return  NULL
  * @author  mountxing
  * @date    2018-08-xx
  * @version v1.0.0
  * @note    LED.c
  ***********************************************************/
#include <rtdevice.h>

#include "led.h"

int led_init(void)
{
    /* 设定 LED 引脚为推挽输出模式 */
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);

    return 0;
}

int led_on(void)
{
    /* 调用 API 输出低电平 */
    rt_pin_write(LED_PIN, PIN_LOW);

    return 0;
}

int led_off(void)
{
    /* 调用 API 输出高电平 */
    rt_pin_write(LED_PIN, PIN_HIGH);

    return 0;
}

int led_toggle(void)
{
    /* 调用 API 读出当前电平 然后输出相反电平 */
    rt_pin_write(LED_PIN, !rt_pin_read(LED_PIN));

    return 0;
}
