/**
 * @file gmg12864menu.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Виджет меню поверх графической библиотеки GMG12864
 *
 * @version 0.1
 * @date 2026-10-09
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

#include <string.h>

#include "gmg12864menu.h"

/*-----------------------------------Настройки----------------------------------*/
#define DISP_WIDTH             (128U) // Ширина дисплея в пикселях
#define DISP_HEIGHT            (64U)  // Высота дисплея в пикселях
#define MENU_SCROLLBAR_WIDTH   (2U)   // Ширина скроллбара в пикселях
#define MENU_SCROLLBAR_GAP     (3U)   // Зазор между меню и скроллбаром
/*-----------------------------------Настройки----------------------------------*/

/*----------------------Подсчет длины строки в символах UTF-8---------------------*/
static uint8_t utf8_strlen(const char *str)
{
    uint8_t len = 0;
    for (size_t i = 0; str[i]; i++)
    {
        // Байт < 0x80 или ведущий байт многобайтовой последовательности >= 0xC0
        if ((uint8_t)str[i] < 0x80 || (uint8_t)str[i] >= 0xC0)
        {
            len++;
        }
    }
    return len;
}
/*----------------------Подсчет длины строки в символах UTF-8---------------------*/

/*----------------------Ширина меню по самому длинному пункту---------------------*/
static uint8_t menu_auto_width(const gmg_menu_t *menu)
{
    uint8_t max_chars = 0;
    for (uint8_t i = 0; i < menu->count; i++)
    {
        uint8_t len = utf8_strlen(menu->items[i].text);
        if (len > max_chars)
        {
            max_chars = len;
        }
    }

    // Отступ 1 пиксель слева и 1 пиксель справа
    uint8_t width = (uint8_t)(max_chars * GMG_MENU_CHAR_WIDTH + 2U);

    if (menu->style == GMG_MENU_SELECTOR)
    {
        // + место под курсор ">" и межсимвольный интервал
        width = (uint8_t)(width + GMG_MENU_CHAR_WIDTH);
    }

    if ((menu->count > menu->visible_rows) &&
        ((uint16_t)menu->x + width + MENU_SCROLLBAR_GAP + MENU_SCROLLBAR_WIDTH < DISP_WIDTH))
    {
        // + место под скроллбар
        width = (uint8_t)(width + MENU_SCROLLBAR_GAP + MENU_SCROLLBAR_WIDTH);
    }

    // Не выходим за пределы экрана
    if ((uint16_t)menu->x + width > DISP_WIDTH)
    {
        width = (uint8_t)(DISP_WIDTH - menu->x);
    }

    return width;
}
/*----------------------Ширина меню по самому длинному пункту---------------------*/

/*---------------------------Корректировка прокрутки----------------------------*/
static void menu_fix_scroll(gmg_menu_t *menu)
{
    if (menu->selected < menu->top)
    {
        menu->top = menu->selected;
    }
    if (menu->selected >= menu->top + menu->visible_rows)
    {
        menu->top = (uint8_t)(menu->selected - menu->visible_rows + 1U);
    }

    if (menu->top > menu->count - menu->visible_rows)
    {
        menu->top = (uint8_t)(menu->count - menu->visible_rows);
    }
}
/*---------------------------Корректировка прокрутки----------------------------*/

/*------------------------Функция инициализации меню---------------------------*/
void GMG_Menu_Init(gmg_menu_t *menu, const gmg_menu_item_t *items, uint8_t count, uint8_t x,
                   uint8_t y, uint8_t width, uint8_t visible_rows, gmg_menu_style_t style,
                   bool blink)
{
    menu->items = items;
    menu->count = count;
    menu->x = x;
    menu->y = y;
    menu->style = style;
    menu->blink = blink;
    menu->visible_rows = visible_rows;

    if (menu->visible_rows == 0 || menu->visible_rows > menu->count)
    {
        menu->visible_rows = menu->count;
    }
    if (menu->visible_rows == 0)
    {
        menu->visible_rows = 1;
    }

    // Выделяем первый доступный пункт
    menu->selected = 0;
    for (uint8_t i = 0; i < menu->count; i++)
    {
        if (menu->items[i].enabled)
        {
            menu->selected = i;
            break;
        }
    }

    // Показываем выделенный пункт
    menu->top = menu->selected;
    menu_fix_scroll(menu);

    menu->width = width ? width : menu_auto_width(menu);
}
/*------------------------Функция инициализации меню---------------------------*/

/*---------------------------Отрисовка скроллбара------------------------------*/
static void menu_draw_scrollbar(const gmg_menu_t *menu)
{
    if (menu->count <= menu->visible_rows)
    {
        return;
    }

    uint8_t sx = (uint8_t)(menu->x + menu->width + MENU_SCROLLBAR_GAP);
    if (sx >= DISP_WIDTH || sx + MENU_SCROLLBAR_WIDTH > DISP_WIDTH)
    {
        return;
    }

    uint16_t track = (uint16_t)menu->visible_rows * GMG_MENU_ROW_HEIGHT;
    uint16_t thumb = track * menu->visible_rows / menu->count;
    if (thumb < 2U)
    {
        thumb = 2U;
    }
    uint16_t max_top = (uint16_t)(menu->count - menu->visible_rows);
    uint16_t thumb_pos = (uint16_t)((uint32_t)(track - thumb) * menu->top / max_top);

    // Дорожка скроллбара
    GMG12864_Draw_rectangle(sx, menu->y, MENU_SCROLLBAR_WIDTH - 1U, (uint16_t)track - 1U, 1);
    // Ползунок
    GMG12864_Draw_rectangle_filled(sx, (uint8_t)(menu->y + thumb_pos), MENU_SCROLLBAR_WIDTH,
                                   thumb, 1);
}
/*---------------------------Отрисовка скроллбара------------------------------*/

/*-----------------------------Отрисовка строки--------------------------------*/
static void menu_draw_row(const gmg_menu_t *menu, uint8_t index, uint8_t row_y,
                          bool highlight)
{
    const gmg_menu_item_t *item = &menu->items[index];
    uint8_t text_x = menu->x;

    if (menu->style == GMG_MENU_SELECTOR)
    {
        if (highlight)
        {
            GMG12864_Decode_UTF8(menu->x, row_y, font5x7, inversion_off, ">");
        }
        text_x = (uint8_t)(menu->x + GMG_MENU_CHAR_WIDTH);
    }
    else if (menu->style == GMG_MENU_INVERSE && highlight)
    {
        // Белая подложка под строку, текст поверх рисуем "вырезанным"
        GMG12864_Draw_rectangle_filled(menu->x, row_y, (uint16_t)menu->width,
                                       GMG_MENU_FONT_HEIGHT, inversion_on);
        text_x = (uint8_t)(menu->x + 1U);
    }
    else if (menu->style == GMG_MENU_FRAME && highlight)
    {
        uint8_t fx = menu->x ? (uint8_t)(menu->x - 1U) : 0U;
        uint8_t fy = row_y ? (uint8_t)(row_y - 1U) : 0U;
        GMG12864_Draw_rectangle(fx, fy, (uint16_t)(menu->width + 1U),
                                (uint16_t)(GMG_MENU_FONT_HEIGHT + 1U), inversion_on);
    }

    bool inversion = (menu->style == GMG_MENU_INVERSE && highlight) ? inversion_on
                                                                   : inversion_off;
    GMG12864_Decode_UTF8(text_x, row_y, font5x7, inversion, item->text);
}
/*-----------------------------Отрисовка строки--------------------------------*/

/*---------------------------Функция отрисовки меню----------------------------*/
void GMG_Menu_Draw(const gmg_menu_t *menu)
{
    // Подсветка мигает только во включенной фазе
    bool blink_on = menu->blink ? GMG12864_Blink_State() : true;

    for (uint8_t row = 0; row < menu->visible_rows; row++)
    {
        uint8_t index = (uint8_t)(menu->top + row);
        if (index >= menu->count)
        {
            break;
        }

        uint8_t row_y = (uint8_t)(menu->y + row * GMG_MENU_ROW_HEIGHT);
        bool highlight = (index == menu->selected) && blink_on;

        menu_draw_row(menu, index, row_y, highlight);
    }

    menu_draw_scrollbar(menu);
}
/*---------------------------Функция отрисовки меню----------------------------*/

/*----------------------Перемещение выделения вверх/вниз----------------------*/
void GMG_Menu_Up(gmg_menu_t *menu)
{
    if (menu->count < 2U)
    {
        return;
    }

    uint8_t i = menu->selected;
    for (uint8_t attempt = 0; attempt < menu->count; attempt++)
    {
        i = i ? (uint8_t)(i - 1U) : (uint8_t)(menu->count - 1U);
        if (menu->items[i].enabled)
        {
            menu->selected = i;
            break;
        }
    }
    menu_fix_scroll(menu);
}

void GMG_Menu_Down(gmg_menu_t *menu)
{
    if (menu->count < 2U)
    {
        return;
    }

    uint8_t i = menu->selected;
    for (uint8_t attempt = 0; attempt < menu->count; attempt++)
    {
        i = (uint8_t)((i + 1U) % menu->count);
        if (menu->items[i].enabled)
        {
            menu->selected = i;
            break;
        }
    }
    menu_fix_scroll(menu);
}
/*----------------------Перемещение выделения вверх/вниз----------------------*/

/*-------------------------Геттеры выделенного пункта--------------------------*/
uint8_t GMG_Menu_Get_selected(const gmg_menu_t *menu)
{
    return menu->selected;
}

uint8_t GMG_Menu_Get_value(const gmg_menu_t *menu)
{
    return menu->items[menu->selected].value;
}

const char *GMG_Menu_Get_text(const gmg_menu_t *menu)
{
    return menu->items[menu->selected].text;
}
/*-------------------------Геттеры выделенного пункта--------------------------*/

/*------------------------Смена стиля подсветки и мигания----------------------*/
void GMG_Menu_Set_style(gmg_menu_t *menu, gmg_menu_style_t style)
{
    menu->style = style;
    menu->width = menu_auto_width(menu);
}

void GMG_Menu_Set_blink(gmg_menu_t *menu, bool blink)
{
    menu->blink = blink;
    if (blink)
    {
        GMG12864_Blink_Enable(blink);
    }
}
/*------------------------Смена стиля подсветки и мигания----------------------*/
