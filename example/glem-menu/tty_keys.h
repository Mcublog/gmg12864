/**
 * @file tty_keys.h
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Платформенная часть слоя кнопок для десктопа: терминал в raw-режиме
 *        отдаёт битовую маску нажатых кнопок и время в мс, всё остальное
 *        (дребезг, повторы, долгое нажатие) делает btn.c.
 *
 *        Раскладка:
 *        w / стрелка вверх  - BTN_UP
 *        s / стрелка вниз   - BTN_DOWN
 *        a / стрелка влево  - BTN_LEFT
 *        d / стрелка вправо - BTN_RIGHT
 *        e / Enter          - BTN_ENTER
 *
 *        Терминал не умеет отдавать "уровень" кнопки, поэтому каждый принятый
 *        байт включает бит на TTy_HOLD_MS. Одиночный тап - один PRESS,
 *        удержание клавиши (автоповтор терминала) - удержание кнопки, то есть
 *        работают REPEAT и LONG.
 *
 * @version 0.1
 * @date 2026-10-10
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

#ifndef EXAMPLE_TTY_KEYS_H_
#define EXAMPLE_TTY_KEYS_H_

#include <stdbool.h>

#include "btn.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief true - есть терминал на stdin и его удалось перевести в raw-режим.
     *        false - ввода с клавиатуры нет, демо пойдёт по таймеру.
     *        Первый вызов пытается перевести терминал в raw-режим
     */
    bool tty_keys_available(void);

    /**
     * @brief Таблица ввода для btn_init()
     */
    const btn_io_t *tty_keys_io(void);

    /**
     * @brief Вернуть терминал в исходный режим
     */
    void tty_keys_restore(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_TTY_KEYS_H_ */
