/**
 * @file app.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Приложение демо: одно меню настроек с прокруткой, три стиля подсветки,
 *        мигание, авто-демо. Реакция на кнопки - только состояние и флаг
 *        перерисовки, рисование идёт в app_task_1ms()
 *
 * @version 0.1
 * @date 2026-10-10
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */

#include "app.h"

#include "src/gmg12864lib.h"
#include "src/gmg12864menu.h"

/*--------------------------- Описание пунктов меню ---------------------------*/
static const gmg_menu_item_t kItems[] = {
    {"Volume", 10, true},   {"Brightness", 20, true}, {"Contrast", 30, true},
    {"Language", 40, true}, {"Time", 50, true},       {"Date", 60, true},
    {"Locked", 70, false},  /* недоступный пункт, выделение его пропускает */
    {"Reset", 80, true},
};

/* Короткие имена стилей подсветки для строки состояния */
static const char *const kStyleNames[] = {"SEL", "INV", "FRM"};
#define STYLE_COUNT (uint8_t)(sizeof(kStyleNames) / sizeof(kStyleNames[0]))
/*--------------------------- Описание пунктов меню ---------------------------*/

/*--------------------------- Геометрия и тайминги ----------------------------*/
#define MENU_X            (0U)
#define MENU_Y            (12U)
#define MENU_ROWS         (4U)  /* 4 пункта видны, остальные - прокрутка */
#define BLINK_PERIOD      (500U) /* Период мигания подсветки, мс */
#define AUTO_DEMO_STEP    (700U) /* Пауза авто-демо в вызовах app_task_1ms (1 мс) */
#define AUTO_DEMO_STYLES  (5U)   /* Каждый N-й шаг авто-демо меняет стиль */
/*--------------------------- Геометрия и тайминги ----------------------------*/

/*---------------------------------Состояние----------------------------------*/
static gmg_menu_t g_menu = {0};
static bool       g_blink_enable = true;
static bool       g_blink_phase = false;
static bool       g_dirty = true;      /* Нужна перерисовка */
    static bool       g_auto_demo = false;
static bool       g_exit = false;
static uint16_t   g_demo_ms = 0;
static uint8_t    g_demo_steps = 0;
/*---------------------------------Состояние----------------------------------*/

/*--------------------------- Перерисовка экрана ------------------------------*/
static void redraw(void)
{
    GMG12864_Clean_Frame_buffer();

    // Заголовок
    GMG12864_Decode_UTF8(2, 0, font5x7, inversion_off, "GMG12864 menu demo");

    // Меню
    GMG_Menu_Draw(&g_menu);

    // Строка состояния. Имя пункта обрезаем, чтоб влезало в экран
    GMG12864_Sprintf_inv(0, 48, inversion_off, "%.6s(%d) %s bl:%d",
                         GMG_Menu_Get_text(&g_menu), GMG_Menu_Get_value(&g_menu),
                         kStyleNames[g_menu.style], g_blink_enable ? 1 : 0);

    // Подсказка по управлению
    GMG12864_Decode_UTF8(0, 56, font3x5, inversion_off, "w/s:move a/d:style e:blink");

    GMG12864_Update();
}
/*--------------------------- Перерисовка экрана ------------------------------*/

/*------------------------- Циклическая смена стиля ----------------------------*/
static void style_shift(bool forward)
{
    int next = ((int)g_menu.style + (forward ? 1 : -1) + (int)STYLE_COUNT) %
               (int)STYLE_COUNT;
    GMG_Menu_Set_style(&g_menu, (gmg_menu_style_t)next);
}
/*------------------------- Циклическая смена стиля ----------------------------*/

/*--------------------------- Вкл/выкл мигание --------------------------------*/
static void blink_toggle(void)
{
    g_blink_enable = !g_blink_enable;
    GMG12864_Blink_Enable(g_blink_enable);
    GMG_Menu_Set_blink(&g_menu, g_blink_enable);
}
/*--------------------------- Вкл/выкл мигание --------------------------------*/

/*--------------------------- Инициализация приложения ------------------------*/
void app_init(void)
{
    GMG_Menu_Init(&g_menu, kItems, (uint8_t)(sizeof(kItems) / sizeof(kItems[0])), MENU_X,
                  MENU_Y, 0 /* ширина по авто */, MENU_ROWS, GMG_MENU_INVERSE,
                  g_blink_enable);

    GMG12864_Blink_Set_period(BLINK_PERIOD);
    GMG12864_Blink_Enable(g_blink_enable);
    g_blink_phase = GMG12864_Blink_State();

    redraw();
}
/*--------------------------- Инициализация приложения ------------------------*/

/*--------------------------- Реакция на кнопку -------------------------------*/
static void button_action(const btn_event_t *ev)
{
    bool act = (ev->type == BTN_EV_PRESS) || (ev->type == BTN_EV_REPEAT);

    switch (ev->id)
    {
    case BTN_UP:
        if (act)
        {
            GMG_Menu_Up(&g_menu);
            g_dirty = true;
        }
        break;
    case BTN_DOWN:
        if (act)
        {
            GMG_Menu_Down(&g_menu);
            g_dirty = true;
        }
        break;
    case BTN_LEFT:
        if (act)
        {
            style_shift(false);
            g_dirty = true;
        }
        break;
    case BTN_RIGHT:
        if (act)
        {
            style_shift(true);
            g_dirty = true;
        }
        break;
    case BTN_ENTER:
        if (ev->type == BTN_EV_PRESS)
        {
            blink_toggle();
            g_dirty = true;
        }
        else if (ev->type == BTN_EV_LONG)
        {
            // В десктопном демо - выход, на МК - возврат на родительский экран
            g_exit = true;
        }
        break;
    default:
        break;
    }
}

void app_on_button(const btn_event_t *ev)
{
    button_action(ev);

    // Живой ввод отменяет авто-демо
    if (ev->type != BTN_EV_NONE)
    {
        g_auto_demo = false;
    }
}
/*--------------------------- Реакция на кнопку -------------------------------*/

/*--------------------------- Задача приложения ------------------------------*/
void app_task_1ms(void)
{
    // Тик библиотеки - раз в 1 мс. В реальном проекте эту функцию зовёт
    // SysTick/таймер/задача RTOS, а app_task_1ms - только из суперцикла
    GMG12864_Tick();

    // Подсветка мигает - перерисовываем по смене фазы
    bool phase = GMG12864_Blink_State();
    if (phase != g_blink_phase)
    {
        g_blink_phase = phase;
        g_dirty = true;
    }

    // Авто-демо: генерируем события кнопок по таймеру
    if (g_auto_demo)
    {
        g_demo_ms++;
        if (g_demo_ms >= AUTO_DEMO_STEP)
        {
            g_demo_ms = 0;
            g_demo_steps++;

            btn_event_t ev = {0};
            ev.type = BTN_EV_PRESS;
            ev.id = (g_demo_steps % AUTO_DEMO_STYLES == 0) ? BTN_RIGHT : BTN_DOWN;
            button_action(&ev);
        }
    }

    if (g_dirty)
    {
        g_dirty = false;
        redraw();
    }
}
/*--------------------------- Задача приложения ------------------------------*/

void app_set_auto_demo(bool enable)
{
    g_auto_demo = enable;
    g_demo_ms = 0;
    g_demo_steps = 0;
}

bool app_exit_requested(void)
{
    return g_exit;
}
