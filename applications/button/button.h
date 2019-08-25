

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
    BUTTON_EVENT_CLICK_DOWN, /* �������������¼� */
    BUTTON_EVENT_CLICK_UP,   /* �������������¼� */
    BUTTON_EVENT_HOLD,       /* ����������ʼ�¼� */
    BUTTON_EVENT_HOLD_CYC,   /* �������������Դ����¼� */
    BUTTON_EVENT_HOLD_UP,    /* �������������¼� */
    BUTTON_EVENT_NONE        /* �ް����¼� */
};

struct my_button
{
    rt_uint8_t  press_logic_level;  /* ��������ʱ�ĵ�ƽ */
    rt_uint16_t cnt;                /* ����ɨ�赽����״̬�Ĵ��� */
    rt_uint16_t hold_cyc_period;    /* �������ڻص������� */
    rt_uint16_t pin;                /* ������Ӧ�� pin ��� */

    enum my_button_event event;     /* �����Ĵ������¼� */

    my_button_callback cb;          /* ���������¼��ص����� */
};

int my_button_register(struct my_button *button);
int my_button_start(void);

#endif
