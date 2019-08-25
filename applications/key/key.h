

#ifndef BEEP_KEY_H
#define BEEP_KEY_H

#include "button.h"
#include "player.h"

#define KEY_PLAY_PIN     66            //PE2
#define KEY_LAST_PIN     67            //PE3
#define KEY_NEXT_PIN     68            //PE4
#define KEY_PRESS_LEVEL  0             //PAO

int key_init(void);                     //按键初始化

#endif
