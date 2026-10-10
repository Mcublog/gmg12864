/**
 * @file app.h
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Приложение демо: одно меню настроек, отрисовка, реакция на события
 *        кнопок. Слой не знает о железе и о платформе - его можно скопировать
 *        в проект на bare metal целиком, заменив только источник событий
 *        (см. example/glem/README.md, раздел "Перенос на bare metal").
 *
 *        Кнопки демо (4 направления джойстика + центральная кнопка):
 *        UP/DOWN     - перемещение по меню;
 *        LEFT/RIGHT  - смена стиля подсветки SELECTOR/INVERSE/FRAME;
 *        ENTER       - короткое: вкл/выкл мигание подсветки;
 *                      длинное: запрос выхода (на МК - возврат на родительский
 *                      экран, в десктопном демо - завершение программы).
 *
 * @version 0.1
 * @date 2026-10-10
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

#ifndef EXAMPLE_APP_H_
#define EXAMPLE_APP_H_

#include <stdbool.h>

#include "btn.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Инициализация меню, мигания и первая отрисовка.
     *        GMG12864_Init() должен быть вызван заранее
     */
    void app_init(void);

    /**
     * @brief Реакция на событие кнопки. Только состояние и флаг перерисовки:
     *        рисовать из обработчика кнопки (или из прерывания) нельзя,
     *        отрисовка происходит в app_task_1ms()
     */
    void app_on_button(const btn_event_t *ev);

    /**
     * @brief Задача приложения: тик библиотеки (1 мс), авто-демо,
     *        перерисовка по изменениям. Зовите из суперцикла или задачи RTOS
     */
    void app_task_1ms(void);

    /**
     * @brief Авто-демо: события кнопок генерируются по таймеру.
     *        Удобно, когда физических кнопок нет (как в десктопном режиме
     *        без терминала)
     */
    void app_set_auto_demo(bool enable);

    /**
     * @brief true - приложению предложили выйти (длинное нажатие ENTER)
     */
    bool app_exit_requested(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_APP_H_ */
