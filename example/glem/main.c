/**
 * @file main.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Минимальный пример: инициализация библиотеки на бэкенде эмулятора
 *        glem, демо-лого, текст и пара примитивов. Никакого ввода и меню -
 *        только рисование кадра и отправка его на "дисплей".
 *        Пример с меню и кнопками - в ../glem-menu/
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

#include <unistd.h>

#include "src/dev/glem_dev.h"
#include "src/gmg12864lib.h"

int main(void)
{
    /* Бэкенд дисплея - эмулятор glem (окно, принимающее пиксели по сокету) */
    const gmg_dev_t *kDev = glem_dev();
    GMG12864_Init(kDev);

    /* Демо-лого рисует и обновляет кадр само, подожмём, чтоб успеть увидеть */
    GMG12864_logo_demonstration();
    usleep(500 * 1000);

    /* Свой кадр: текст шрифтом 5x7 */
    GMG12864_Clean_Frame_buffer();
    GMG12864_Puts(0, 0, "Hello");

    /* Пара примитивов: закрашенный прямоугольник и пиксели */
    GMG12864_Draw_rectangle_filled(0, 16, 32, 8, inversion_on);
    GMG12864_Draw_pixel(40, 20, 1);
    GMG12864_Draw_pixel(41, 20, 1);
    GMG12864_Draw_pixel(42, 20, 1);

    /* Линия и окружность */
    GMG12864_Draw_line(0, 32, 63, 32, 1);
    GMG12864_Draw_circle(96, 40, 12, 1);

    /* Отправляем кадр на "дисплей" */
    GMG12864_Update();

    return 0;
}
