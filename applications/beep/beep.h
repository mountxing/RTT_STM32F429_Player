
#ifndef BEEP_H
#define BEEP_H

#include <rtthread.h>

#define BEEP_PWM_DEVICE  "pwm5"     //TIM5_PWM5_CH1 -->PH10
#define BEEP_PWM_CH      1

int beep_init(void);                         //蜂鸣器初始化
int beep_on(void);                           //蜂鸣器开
int beep_off(void);                          //蜂鸣器关
int beep_set(uint16_t freq, uint8_t volume); //蜂鸣器设定

#endif
