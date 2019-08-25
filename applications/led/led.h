

#ifndef LED_H
#define LED_H

#define LED_PIN     123                 //PH10 -->LED1

int led_init(void);                     //LED 灯初始化
int led_on(void);                       //LED 灯亮
int led_off(void);                      //LED 灯灭
int led_toggle(void);                   //LED 灯亮灭状态翻转

#endif
