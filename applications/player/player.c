/************************************************************
  * @brief   player Windowns
	* @param   NULL
  * @return  NULL
  * @author  mountxing
  * @date    2018-08-xx
  * @version v1.0.0
  * @note    player.c
  ***********************************************************/

#include <rthw.h>

#include "player.h"

int player_play(player_t player)
{
    rt_uint32_t level;

    if (player->status != PLAYER_RUNNING)
    {
        /* �ر�ȫ���ж� */
        level = rt_hw_interrupt_disable();

        /* �趨����״̬Ϊ����״̬ */
        player->status = PLAYER_RUNNING;

        /* ��ȫ���ж� */
        rt_hw_interrupt_enable(level);

        /*�ͷ��ź���֪ͨ�����߳̿�ʼ����*/
        rt_sem_release(player->sem_play);
    }

    return 0;
}

int player_stop(player_t player)
{
    rt_uint32_t level;

    if (player->status == PLAYER_RUNNING)
    {
        /* �ر�ȫ���ж� */
        level = rt_hw_interrupt_disable();

        /* �趨����״̬Ϊ����״̬ */
        player->status = PLAYER_STOP;

        /* ��ȫ���ж� */
        rt_hw_interrupt_enable(level);
    }

    return 0;
}

int player_last(player_t player)
{
    uint16_t len;
    rt_uint32_t level;

    /* �ر�ȫ���ж� */
    level = rt_hw_interrupt_disable();

    /* ����ǰ���Ÿ�������� ��һ */
    if (player->song_current > 1)
    {
        player->song_current --;
    }
    else
    {
        player->song_current = player->song_num;
    }

    /* ���²������ĵ�ǰ�����Ĳ���ʱ���Լ���ʱ�� */
    player->song_time_pass = 0;

    /* ��ȫ���ж� */
    rt_hw_interrupt_enable(level);

    player->decode->control(player->song_sheet[player->song_current - 1], DECODE_OPS_CMD_GET_LEN, &len);

    /* �ر�ȫ���ж� */
    level = rt_hw_interrupt_disable();

    player->song_time_all = len;

    /* ��ȫ���ж� */
    rt_hw_interrupt_enable(level);

    if (player->status != PLAYER_RUNNING)
    {
        player_play(player);
    }

    return 0;
}

int player_next(player_t player)
{
    uint16_t len;
    rt_uint32_t level;

    /* �ر�ȫ���ж� */
    level = rt_hw_interrupt_disable();

    /* ����ǰ���Ÿ�������� ��һ */
    if (player->song_current < player->song_num)
    {
        player->song_current ++;
    }
    else
    {
        player->song_current = 1;
    }

    /* ���²������ĵ�ǰ�����Ĳ���ʱ���Լ���ʱ�� */
    player->song_time_pass = 0;

    /* ��ȫ���ж� */
    rt_hw_interrupt_enable(level);

    player->decode->control(player->song_sheet[player->song_current - 1], DECODE_OPS_CMD_GET_LEN, &len);

    /* �ر�ȫ���ж� */
    level = rt_hw_interrupt_disable();

    player->song_time_all = len;

    /* ��ȫ���ж� */
    rt_hw_interrupt_enable(level);

    if (player->status != PLAYER_RUNNING)
    {
        player_play(player);
    }

    return 0;
}
int player_control(player_t player, int cmd, void *arg)
{
    rt_uint32_t level;

    switch (cmd)
    {
    case PLAYER_CMD_PLAY:
        player_play(player);
        break;
    case PLAYER_CMD_STOP:
        player_stop(player);
        break;
    case PLAYER_CMD_LAST:
        player_last(player);
        break;
    case PLAYER_CMD_NEXT:
        player_next(player);
        break;
    case PLAYER_CMD_SET_VOL:
        /* �ر�ȫ���ж� */
        level = rt_hw_interrupt_disable();
        player->volume = *(uint8_t *)arg;
        /* ��ȫ���ж� */
        rt_hw_interrupt_enable(level);
        player->audio->control(AUDIO_OPS_CMD_SET_VOL, &player->volume);
        break;
    case PLAYER_CMD_GET_VOL:
        *(uint8_t *)arg = player->volume;
        break;
    case PLAYER_CMD_GET_STATUS:
        *(uint8_t *)arg = player->status;
        break;
    }
    return 0;
}
int player_add_song(player_t player, void *song)
{
    rt_uint32_t level;

    if (player->song_num == PLAYER_SONG_NUM_MAX)
    {
        return -1;
    }
    /* �ر�ȫ���ж� */
    level = rt_hw_interrupt_disable();

    player->song_sheet[player->song_num] = song;
    player->song_num++;

    /* ��ȫ���ж� */
    rt_hw_interrupt_enable(level);

    return 0;
}

int player_show(player_t player)
{
    char name[PLAYER_SONG_NAME_LEN_MAX + 1];
    uint8_t i;
    uint16_t percent;

    rt_kprintf("*********** Beep Player ***********\n");

    /* ��ӡ�赥 */
    for (i = 0; i < player->song_num; i++)
    {
        player->decode->control(player->song_sheet[i], DECODE_OPS_CMD_GET_NAME, name);
        rt_kprintf("%02d. %s\n", i + 1, name);
    }

    /* ��ӡ��ǰ����״̬ */
    if (PLAYER_RUNNING == player->status)
    {
        rt_kprintf("<---  ���ڲ��ţ�");
    }
    else
    {
        rt_kprintf("<---  ��ͣ���ţ�");
    }

    /* ��ӡ��ǰ���� */
    player->decode->control(player->song_sheet[player->song_current - 1], DECODE_OPS_CMD_GET_NAME, name);
    rt_kprintf("%s", name);
    rt_kprintf("--->\n");

    /* ��ӡ���Ž��� */
    percent = player->song_time_pass * 100 / player->song_time_all;
    rt_kprintf("���Ž��ȣ�%02d%%  ������С��%02d%%\n", percent, player->volume);

    rt_kprintf("***********************************\n");

    return 0;
}

static void player_entry(void *parameter)
{
    player_t player = (player_t)parameter;
    uint8_t buffer[PLAYER_BUFFER_SIZE], size;

    while (1)
    {
        if (player->status == PLAYER_RUNNING)
        {
            size = player->song_time_all - player->song_time_pass;
            if (size > PLAYER_BUFFER_SIZE) size = PLAYER_BUFFER_SIZE;
            size = player->decode->read(player->song_sheet[player->song_current - 1], player->song_time_pass, buffer, size);
            if (size > 0)
            {
                player->audio->write(buffer, size);
                player->song_time_pass += size;
            }
            /* �������ʱ�䵽�ˣ��л�����һ�� */
            if (player->song_time_pass >= player->song_time_all)
            {
                player_next(player);
                player_show(player);
            }
        }
        else
        {
            /* ��ͣ����ʱ�ر���Ƶ�豸*/
            player->audio->close();

            /* �ȴ����ŵ��ź��� */
            rt_sem_take(player->sem_play, RT_WAITING_FOREVER);

            /* ��ʼ����ʱ����Ƶ�豸*/
            player->audio->open();
        }
    }
}

int player_start(player_t player)
{
    uint16_t len;
    static rt_uint8_t inited = 0;

    /* ��ⲥ�����Ƿ��Ѿ������� */
    if (inited == 1)
    {
        return -RT_ERROR;
    }

    if (player->song_num == 0)
    {
        return -1;
    }
    /* ���ýӿڳ�ʼ�������� */
    player->decode->init();

    player->status = PLAYER_STOP;
    player->volume = PLAYER_SOUND_SIZE_DEFAULT;
    player->song_current = 1;
    player->song_time_pass = 0;
    player->decode->control(player->song_sheet[player->song_current - 1], DECODE_OPS_CMD_GET_LEN, &len);
    player->song_time_all = len;

    /* ���ýӿڳ�ʼ����Ƶ�豸 */
    player->audio->init();
    player->audio->control(AUDIO_OPS_CMD_SET_VOL, &player->volume);

    /* ��ʼ����̬�ź��� */
    player->sem_play = rt_sem_create("sem_play", 0, RT_IPC_FLAG_FIFO);
    if (player->sem_play == RT_NULL)
    {
        return -RT_ERROR;
    }

    /* ������̬�߳� */
    player->play_thread = rt_thread_create("player",
                                           player_entry, player,
                                           512, 20, 20);
    if (player->play_thread != RT_NULL)
    {
        rt_thread_startup(player->play_thread);
    }
    else
    {
        rt_sem_delete(player->sem_play);
        return -RT_ERROR;
    }
    inited = 1;

    return 0;
}
