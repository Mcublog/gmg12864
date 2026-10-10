/**
 * @file tty_keys.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Десктопная реализация btn_io_t: терминал в raw-режиме. Единственное
 *        место примера, где есть POSIX (termios, poll, read). На bare metal
 *        этот файл заменяется на опрос GPIO - btn.c и app.c не меняются.
 *
 * @version 0.1
 * @date 2026-10-10
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

/* termios/poll/read в strict c11 скрыты, включаем объявления */
#define _DEFAULT_SOURCE 1

#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "tty_keys.h"

/*-----------------------------------Настройки----------------------------------*/
/* Сколько держим бит кнопки после полученного байта. Должно быть больше
 * интервала автоповтора терминала (~30 мс), иначе удержание клавиши будет
 * рвать состояние, и меньше паузы между отдельными нажатиями человека, иначе
 * два тапа склеятся в одно удержание */
#define TTy_HOLD_MS (150U)
#define TTy_READ_MAX (8U)  /* Сколько байт вычитываем за один вызов */
/*-----------------------------------Настройки----------------------------------*/

/*--------------------------- Escape-последовательности стрелок -----------------*/
#define TTY_ESC      (0x1B)
#define TTY_CSI_UP    'A'
#define TTY_CSI_DOWN  'B'
#define TTY_CSI_RIGHT 'C'
#define TTY_CSI_LEFT  'D'
/*--------------------------- Escape-последовательности стрелок -----------------*/

/*---------------------------Настройка терминала-------------------------------*/
static struct termios g_old_tty = {0};
static bool           g_raw_mode = false;

static bool tty_raw_setup(void)
{
    struct termios tty = {0};

    if (!isatty(STDIN_FILENO))
    {
        return false;
    }
    if (tcgetattr(STDIN_FILENO, &tty) != 0)
    {
        return false;
    }

    g_old_tty = tty;
    tty.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0)
    {
        return false;
    }

    g_raw_mode = true;
    return true;
}
/*---------------------------Настройка терминала-------------------------------*/

bool tty_keys_available(void)
{
    // Пробуем перевести терминал в raw-режим один раз, дальше - только читаем
    if (!g_raw_mode)
    {
        tty_raw_setup();
    }
    return g_raw_mode;
}

void tty_keys_restore(void)
{
    if (g_raw_mode)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_old_tty);
        g_raw_mode = false;
    }
}

/*---------------------------Эмуляция нажатых кнопок----------------------------*/
static uint32_t g_hold_mask = 0;               /* Какие биты "нажаты" сейчас */
static uint32_t g_hold_since[BTN_COUNT] = {0}; /* Время последнего нажатия, мс */

static uint32_t tty_now_ms(void)
{
    struct timespec ts = {0};

#if defined(CLOCK_MONOTONIC)
    clock_gettime(CLOCK_MONOTONIC, &ts);
#else
    clock_gettime(CLOCK_REALTIME, &ts);
#endif

    return (uint32_t)((uint64_t)ts.tv_sec * 1000U + (uint64_t)ts.tv_nsec / 1000000U);
}

static void tty_key_press(btn_id_t id)
{
    if (id == BTN_NONE || id >= BTN_COUNT)
    {
        return;
    }

    uint32_t now = tty_now_ms();
    g_hold_mask |= BTN_BIT(id);
    g_hold_since[id] = now;
}

/* Стрелки приходят как ESC [ A/B/C/D. Кладём байты в буфер и разбираем.
 * Возвращаем true, если распознали последовательность */
static btn_id_t tty_escape_button(uint8_t c)
{
    static uint8_t tail[2] = {0};

    if (c == TTY_ESC)
    {
        tail[0] = 0;
        tail[1] = 0;
        return BTN_NONE;
    }
    if (tail[0] == 0)
    {
        tail[0] = c;
        return BTN_NONE;
    }
    if (tail[0] != '[')
    {
        // Не CSI - сбрасываем разбор
        tail[0] = 0;
        return BTN_NONE;
    }

    tail[0] = 0;
    switch (c)
    {
    case TTY_CSI_UP:
        return BTN_UP;
    case TTY_CSI_DOWN:
        return BTN_DOWN;
    case TTY_CSI_RIGHT:
        return BTN_RIGHT;
    case TTY_CSI_LEFT:
        return BTN_LEFT;
    default:
        return BTN_NONE;
    }
}

static btn_id_t tty_char_button(uint8_t c)
{
    switch (c)
    {
    case 'w':
        return BTN_UP;
    case 's':
        return BTN_DOWN;
    case 'a':
        return BTN_LEFT;
    case 'd':
        return BTN_RIGHT;
    case 'e':
    case '\r':
    case '\n':
        return BTN_ENTER;
    default:
        return BTN_NONE;
    }
}

static uint32_t tty_raw_buttons(void)
{
    uint32_t now = tty_now_ms();

    // Читаем всё, что накопилось в stdin
    struct pollfd fds = {.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};
    for (uint8_t i = 0; i < TTy_READ_MAX; i++)
    {
        int rc = poll(&fds, 1, 0);
        if (rc <= 0 || !(fds.revents & POLLIN))
        {
            break;
        }

        char key = 0;
        if (read(STDIN_FILENO, &key, 1) != 1)
        {
            break;
        }

        uint8_t c = (uint8_t)key;
        btn_id_t id = (c == TTY_ESC) ? BTN_NONE : tty_char_button(c);
        if (id == BTN_NONE)
        {
            id = tty_escape_button(c);
        }
        tty_key_press(id);
    }

    // Снимаем биты, время удержания которых вышло
    for (btn_id_t id = BTN_UP; id < BTN_COUNT; id = (btn_id_t)(id + 1))
    {
        if ((g_hold_mask & BTN_BIT(id)) && (now - g_hold_since[id]) >= TTy_HOLD_MS)
        {
            g_hold_mask &= ~BTN_BIT(id);
        }
    }

    return g_hold_mask;
}
/*---------------------------Эмуляция нажатых кнопок----------------------------*/

static const btn_io_t g_tty_io = {
    .raw = tty_raw_buttons,
    .now_ms = tty_now_ms,
};

const btn_io_t *tty_keys_io(void)
{
    return &g_tty_io;
}
