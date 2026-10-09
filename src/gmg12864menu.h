/**
 * @file gmg12864menu.h
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Виджет меню поверх графической библиотеки GMG12864.
 *        Подсветка выбранного пункта тремя способами:
 *        - GMG_MENU_SELECTOR - курсор ">" слева от пункта;
 *        - GMG_MENU_INVERSE  - инверсия (белая подложка, "вырезанный" текст);
 *        - GMG_MENU_FRAME    - рамка вокруг пункта.
 *        Для мигающей подсветки используется системный таймер
 *        (GMG12864_Tick() раз в 1 мс) и GMG12864_Blink_State().
 *
 * @version 0.1
 * @date 2026-10-09
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

#ifndef INC_GMG12864_MENU_H_
#define INC_GMG12864_MENU_H_

#include <stdbool.h>
#include <stdint.h>

#include "gmg12864lib.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Геометрия одной строки меню при шрифте 5x7 */
#define GMG_MENU_ROW_HEIGHT (8U)  /* высота строки в пикселях */
#define GMG_MENU_CHAR_WIDTH (6U)  /* ширина символа в пикселях */
#define GMG_MENU_FONT_HEIGHT (7U) /* высота глифа шрифта 5x7 в пикселях */

/* Пункт меню */
typedef struct
{
    const char *text; /* Текст пункта. UTF-8 */
    uint8_t     value;   /* Пользовательское значение. Отдается в switch/case
                            по результату выбора */
    bool        enabled; /* false - пункт недоступен, выделение его пропускает */
} gmg_menu_item_t;

/* Стиль подсветки выбранного пункта */
typedef enum
{
    GMG_MENU_SELECTOR = 0, /* Курсор ">" слева от пункта */
    GMG_MENU_INVERSE,      /* Инверсия: белая подложка, темный текст */
    GMG_MENU_FRAME         /* Рамка вокруг пункта */
} gmg_menu_style_t;

/* Меню */
typedef struct
{
    const gmg_menu_item_t *items;      /* Массив пунктов меню */
    uint8_t                count;      /* Количество пунктов */
    uint8_t                selected;   /* Индекс выбранного пункта */
    uint8_t                top;        /* Индекс первого видимого пункта */
    uint8_t                x;          /* Позиция меню по x, пиксели */
    uint8_t                y;          /* Позиция меню по y, пиксели */
    uint8_t                width;      /* Ширина области меню в пикселях.
                                         Используется для подложки/рамки и скроллбара.
                                         Если 0 - считается по самому длинному пункту */
    uint8_t                visible_rows; /* Сколько пунктов видно одновременно.
                                            Если 0 - все */
    bool                   blink;      /* true - подсветка мигает */
    gmg_menu_style_t       style;      /* Стиль подсветки */
} gmg_menu_t;

/**
 * @brief Инициализация меню
 *
 * @param menu - указатель на структуру меню
 * @param items - указатель на массив пунктов меню
 * @param count - количество пунктов
 * @param x, y - позиция меню в пикселях
 * @param width - ширина области меню в пикселях (0 - авто по длине текста)
 * @param visible_rows - сколько пунктов видно (0 - все)
 * @param style - стиль подсветки
 * @param blink - true - мигающая подсветка (GMG12864_Tick + Blink_Enable)
 */
void GMG_Menu_Init(gmg_menu_t *menu, const gmg_menu_item_t *items, uint8_t count, uint8_t x,
                   uint8_t y, uint8_t width, uint8_t visible_rows, gmg_menu_style_t style,
                   bool blink);

/**
 * @brief Отрисовка меню. Перед вызовом очистите кадр
 * (GMG12864_Clean_Frame_buffer()), затем вызовите GMG12864_Update()
 */
void GMG_Menu_Draw(const gmg_menu_t *menu);

/**
 * @brief Перемещение выделения на один пункт вверх. Зациклено.
 */
void GMG_Menu_Up(gmg_menu_t *menu);

/**
 * @brief Перемещение выделения на один пункт вниз. Зациклено.
 */
void GMG_Menu_Down(gmg_menu_t *menu);

/**
 * @brief Смена стиля подсветки. Ширина меню пересчитывается автоматически
 */
void GMG_Menu_Set_style(gmg_menu_t *menu, gmg_menu_style_t style);

/**
 * @brief Включение/выключение мигания подсветки выделенного пункта
 */
void GMG_Menu_Set_blink(gmg_menu_t *menu, bool blink);

/**
 * @brief Индекс выбранного пункта
 */
uint8_t GMG_Menu_Get_selected(const gmg_menu_t *menu);

/**
 * @brief Значение (value) выбранного пункта
 */
uint8_t GMG_Menu_Get_value(const gmg_menu_t *menu);

/**
 * @brief Текст выбранного пункта
 */
const char *GMG_Menu_Get_text(const gmg_menu_t *menu);

#ifdef __cplusplus
}
#endif

#endif /* INC_GMG12864_MENU_H_ */
