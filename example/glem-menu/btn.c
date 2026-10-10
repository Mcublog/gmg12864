/**
 * @file btn.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Реализация слоя кнопок: антидребезг, фронты нажатия/отпускания,
 *        авто-повтор при удержании, долгое нажатие, очередь событий.
 *        Железо спрятано за btn_io_t (см. btn.h), здесь его нет.
 *
 * @version 0.1
 * @date 2026-10-10
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

#include <stddef.h>

#include "btn.h"

/*-----------------------------------Настройки----------------------------------*/
#define BTN_QUEUE_SIZE    (8U)  /* Глубина очереди событий */
#define BTN_ALL_BITS_MASK ((uint32_t)(BTN_BIT(BTN_COUNT) - 1U)) /* Битовая маска всех кнопок */
/*-----------------------------------Настройки----------------------------------*/

/*---------------------------Кольцевая очередь событий---------------------------*/
static btn_event_t g_queue[BTN_QUEUE_SIZE] = {0};
static uint8_t     g_head = 0; /* Индекс первого события */
static uint8_t     g_count = 0; /* Сколько событий в очереди */

static void btn_push(btn_id_t id, btn_ev_t type)
{
    if (g_count == BTN_QUEUE_SIZE)
    {
        // Очередь полна - теряем самое старое событие
        g_head = (uint8_t)((g_head + 1U) % BTN_QUEUE_SIZE);
        g_count--;
    }

    uint8_t tail = (uint8_t)((g_head + g_count) % BTN_QUEUE_SIZE);
    g_queue[tail].id = id;
    g_queue[tail].type = type;
    g_count++;
}
/*---------------------------Кольцевая очередь событий---------------------------*/

/*-----------------------------------Состояние----------------------------------*/
static const btn_io_t *g_io = NULL;
static btn_cfg_t       g_cfg = {0};

static uint32_t g_raw_last = 0;  /* Предыдущий сэмпл платформы */
static uint32_t g_deb_start = 0; /* Когда сэмпл изменился, мс */
static uint32_t g_state = 0;     /* Стабильное состояние кнопок */
static uint32_t g_press_ms[BTN_COUNT] = {0};  /* Время нажатия, мс */
static uint32_t g_repeat_ms[BTN_COUNT] = {0}; /* Время последнего повтора, мс */
static bool     g_long_done[BTN_COUNT] = {0}; /* Долгое нажатие отработано */
/*-----------------------------------Состояние----------------------------------*/

/*---------------------------Инициализация слоя кнопок--------------------------*/
void btn_init(const btn_io_t *io, const btn_cfg_t *cfg)
{
    g_io = io;
    g_cfg = *cfg;

    g_head = 0;
    g_count = 0;
    g_raw_last = io->raw();
    g_state = g_raw_last & BTN_ALL_BITS_MASK;
    g_deb_start = io->now_ms();

    for (btn_id_t id = BTN_NONE; id < BTN_COUNT; id = (btn_id_t)(id + 1))
    {
        g_press_ms[id] = 0;
        g_repeat_ms[id] = 0;
        g_long_done[id] = false;
    }
}
/*---------------------------Инициализация слоя кнопок--------------------------*/

/*-----------------------------Обработка одной кнопки----------------------------*/
static void btn_hold_proc(btn_id_t id, uint32_t now)
{
    if (g_press_ms[id] == 0)
    {
        return; // Кнопка не нажата
    }

    uint32_t held = now - g_press_ms[id];

    // Долгое нажатие - один раз
    if (g_cfg.long_ms && !g_long_done[id] && held >= g_cfg.long_ms)
    {
        g_long_done[id] = true;
        btn_push(id, BTN_EV_LONG);
    }

    // Авто-повтор при удержании
    if (g_cfg.repeat_delay_ms && held >= g_cfg.repeat_delay_ms)
    {
        if (now - g_repeat_ms[id] >= g_cfg.repeat_period_ms)
        {
            g_repeat_ms[id] = now;
            btn_push(id, BTN_EV_REPEAT);
        }
    }
}
/*-----------------------------Обработка одной кнопки----------------------------*/

/*--------------------------------Опрос кнопок---------------------------------*/
void btn_task(void)
{
    if (g_io == NULL)
    {
        return;
    }

    uint32_t now = g_io->now_ms();
    uint32_t raw = g_io->raw() & BTN_ALL_BITS_MASK;

    // Антидребезг: уровень принимается, если держится debounce_ms
    if (raw != g_raw_last)
    {
        g_raw_last = raw;
        g_deb_start = now;
    }
    bool settled = (g_cfg.debounce_ms == 0) || ((now - g_deb_start) >= g_cfg.debounce_ms);

    if (settled && raw != g_state)
    {
        uint32_t changed = raw ^ g_state;
        g_state = raw;

        for (btn_id_t id = BTN_UP; id < BTN_COUNT; id = (btn_id_t)(id + 1))
        {
            uint32_t bit = BTN_BIT(id);
            if ((changed & bit) == 0)
            {
                continue;
            }

            if (raw & bit)
            {
                // Нажатие
                g_press_ms[id] = now;
                g_repeat_ms[id] = now;
                g_long_done[id] = false;
                btn_push(id, BTN_EV_PRESS);
            }
            else
            {
                // Отпускание
                g_press_ms[id] = 0;
                btn_push(id, BTN_EV_RELEASE);
            }
        }
    }

    // Удержание: долгое нажатие и авто-повтор
    for (btn_id_t id = BTN_UP; id < BTN_COUNT; id = (btn_id_t)(id + 1))
    {
        if (g_state & BTN_BIT(id))
        {
            btn_hold_proc(id, now);
        }
    }
}
/*--------------------------------Опрос кнопок---------------------------------*/

/*-----------------------------Забор события из очереди--------------------------*/
bool btn_get_event(btn_event_t *ev)
{
    if (g_count == 0)
    {
        return false;
    }

    *ev = g_queue[g_head];
    g_head = (uint8_t)((g_head + 1U) % BTN_QUEUE_SIZE);
    g_count--;
    return true;
}
/*-----------------------------Забор события из очереди--------------------------*/

/*-----------------------------Состояние кнопки после дребезга------------------*/
bool btn_is_down(btn_id_t id)
{
    if (id >= BTN_COUNT)
    {
        return false;
    }
    return (g_state & BTN_BIT(id)) != 0;
}
/*-----------------------------Состояние кнопки после дребезга------------------*/
