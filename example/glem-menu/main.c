/**
 * @file main.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Пример работы виджета меню (GMG12864_menu) на эмуляторе glem.
 *        Одно меню настроек слева: прокрутка, скроллбар, три стиля подсветки
 *        с миганием. Вся логика - в app.c, абстракция кнопок - в btn.c,
 *        десктопный ввод с клавиатуры - в tty_keys.c.
 *
 *        Управление с клавиатуры (если запущено в терминале):
 *        w/s      - перемещение по меню
 *        a/d      - стиль подсветки: SELECTOR/INVERSE/FRAME
 *        e        - вкл/выкл мигание подсветки
 *        e (дольше 1 с) - выход
 *        Стрелки работают так же. Если запущено без терминала - авто-демо
 *        с движением по таймеру.
 *
 *        Запуск:
 *        glem -r 128x64 -s4 & ./example_glem
 *
 * @version 0.1
 * @date 2026-10-10
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

/* usleep в strict c11 скрыт, включаем glibc-объявления */
#define _DEFAULT_SOURCE 1

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "app.h"
#include "src/dev/glem_dev.h"
#include "src/gmg12864lib.h"
#include "tty_keys.h"

/*--------------------------- Настройки кнопок --------------------------------*/
static const btn_cfg_t kBtnCfg = {
    .debounce_ms = 20,       /* Антидребезг */
    .repeat_delay_ms = 400,  /* Задержка до авто-повтора */
    .repeat_period_ms = 120, /* Период авто-повтора */
    .long_ms = 1000,         /* Долгое нажатие ENTER - выход */
};
/*--------------------------- Настройки кнопок --------------------------------*/

int main(void)
{
    const gmg_dev_t *dev = glem_dev();

    GMG12864_Init(dev);
    GMG12864_logo_demonstration();

    app_init();

    // Без терминала ввода нет - включаем авто-демо
    const btn_io_t *io = tty_keys_available() ? tty_keys_io() : NULL;
    if (io == NULL)
    {
        puts("No tty. Auto demo mode.");
        app_set_auto_demo(true);
    }
    else
    {
        btn_init(io, &kBtnCfg);
    }

    while (!app_exit_requested())
    {
        if (io != NULL)
        {
            btn_task();

            // Разбираем все накопившиеся события
            btn_event_t ev = {0};
            while (btn_get_event(&ev))
            {
                app_on_button(&ev);
            }
        }

        // Тик библиотеки раз в 1 мс, перерисовка по изменениям
        app_task_1ms();
        usleep(1000);
    }

    tty_keys_restore();
    return 0;
}
