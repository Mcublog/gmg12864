/**
 * @file main.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Пример работы виджета меню (GMG12864_menu) на эмуляторе glem.
 *        Два меню: слева - основное с мигающей подсветкой и прокруткой,
 *        справа - выбор стиля подсветки левого меню.
 *
 *        Управление с клавиатуры (если запущено в терминале):
 *        w/s      - перемещение по активному меню
 *        a/d      - переключение активного меню
 *        b        - вкл/выкл мигание подсветки
 *        1/2/3    - стиль подсветки: SELECTOR/INVERSE/FRAME
 *        q        - выход
 *        Если запущено без терминала - авто-демо с движением по таймеру.
 *
 *        Запуск:
 *        glem -r 128x64 -s4 & ./example_glem
 *
 * @version 0.1
 * @date 2026-10-09
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */
/* usleep в strict c11 скрыт, включаем glibc-объявления */
#define _DEFAULT_SOURCE 1

#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "src/dev/glem_dev.h"
#include "src/gmg12864lib.h"
#include "src/gmg12864menu.h"

/*--------------------------- Описание пунктов меню ---------------------------*/
static const gmg_menu_item_t kMainItems[] = {
    {"Volume", 10, true},    {"Brightness", 20, true}, {"Contrast", 30, true},
    {"Language", 40, true},  {"Time", 50, true},       {"Date", 60, true},
    {"Locked", 70, false},   /* недоступный пункт, выделение его пропускает */
    {"Reset", 80, true},
};

static const gmg_menu_item_t kStyleItems[] = {
    {"SELECTOR", 0, true},
    {"INVERSE", 1, true},
    {"FRAME", 2, true},
};
/*--------------------------- Описание пунктов меню ---------------------------*/

/*--------------------------- Геометрия меню ---------------------------------*/
#define MAIN_X        (0U)
#define MAIN_Y        (12U)
#define MAIN_ROWS     (4U)  // 4 пункта видны, остальные - прокрутка
#define STYLE_X       (74U)
#define STYLE_Y       (12U)
/*--------------------------- Геометрия меню ---------------------------------*/

static gmg_menu_t g_main_menu = {0};
static gmg_menu_t g_style_menu = {0};
static gmg_menu_t *g_active = &g_main_menu; // Меню, которым управляем
static bool g_blink_enable = true;

/*--------------------------- Инициализация меню ------------------------------*/
static void menus_init(void)
{
    GMG_Menu_Init(&g_main_menu, kMainItems, sizeof(kMainItems) / sizeof(kMainItems[0]), MAIN_X,
                  MAIN_Y, 0 /* ширина по авто */, MAIN_ROWS, GMG_MENU_INVERSE, g_blink_enable);

    GMG_Menu_Init(&g_style_menu, kStyleItems, sizeof(kStyleItems) / sizeof(kStyleItems[0]),
                  STYLE_X, STYLE_Y, 0 /* ширина по авто */, 0 /* все пункты */,
                  GMG_MENU_SELECTOR, false);

    GMG12864_Blink_Set_period(500); // Период мигания 500 мс
    GMG12864_Blink_Enable(g_blink_enable);
}
/*--------------------------- Инициализация меню ------------------------------*/

/*--------------------------- Перерисовка экрана ------------------------------*/
static void redraw(void)
{
    char status[48] = {0};

    GMG12864_Clean_Frame_buffer();

    // Заголовок
    GMG12864_Decode_UTF8(2, 0, font5x7, inversion_off, "GMG12864 menu demo");

    // Подписи меню
    GMG12864_Decode_UTF8(MAIN_X, 8, font3x5, inversion_off, "main:");
    GMG12864_Decode_UTF8(STYLE_X, 8, font3x5, inversion_off, "style:");

    // Сами меню
    GMG_Menu_Draw(&g_main_menu);
    GMG_Menu_Draw(&g_style_menu);

    // Строка состояния. Имя пункта обрезаем, чтоб влезало в экран
    snprintf(status, sizeof(status), "%.6s(%d) act:%d st:%d",
             GMG_Menu_Get_text(&g_main_menu), GMG_Menu_Get_value(&g_main_menu),
             (g_active == &g_main_menu) ? 1 : 2, g_main_menu.style);
    GMG12864_Sprintf_inv(0, 48, inversion_off, status);

    // Подсказка по управлению
    GMG12864_Decode_UTF8(0, 56, font3x5, inversion_off, "w/s:move a/d:menu b:blink q:exit");

    GMG12864_Update();
}
/*--------------------------- Перерисовка экрана ------------------------------*/

/*--------------------------- Смена стиля подсветки ---------------------------*/
static void set_style(uint8_t index)
{
    if (index > 2)
    {
        index = 2;
    }
    GMG_Menu_Set_style(&g_main_menu, (gmg_menu_style_t)index);
    g_style_menu.selected = index; // Синхронизируем выбор в правом меню
}
/*--------------------------- Смена стиля подсветки ---------------------------*/

/*--------------------------- Настройка терминала -----------------------------*/
static struct termios g_old_tty = {0};
static bool g_raw_mode = false;

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

static void tty_raw_restore(void)
{
    if (g_raw_mode)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_old_tty);
        g_raw_mode = false;
    }
}
/*--------------------------- Настройка терминала -----------------------------*/

int main(void)
{
    const gmg_dev_t *dev = glem_dev();

    GMG12864_Init(dev);
    GMG12864_logo_demonstration();

    menus_init();

    if (!tty_raw_setup())
    {
        puts("No tty. Auto demo mode.");
    }

    redraw();

    uint32_t demo_ms = 0;
    bool blink_phase = GMG12864_Blink_State();
    bool running = true;

    while (running)
    {
        /*--------------------- Обработка клавиатуры ---------------------*/
        if (g_raw_mode)
        {
            struct pollfd fds = {.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};
            int rc = poll(&fds, 1, 1); // Ждем 1 мс
            if (rc > 0 && (fds.revents & POLLIN))
            {
                char key = 0;
                if (read(STDIN_FILENO, &key, 1) == 1)
                {
                    switch (key)
                    {
                    case 'w':
                        GMG_Menu_Up(g_active);
                        break;
                    case 's':
                        GMG_Menu_Down(g_active);
                        break;
                    case 'a':
                    case 'd':
                        g_active = (g_active == &g_main_menu) ? &g_style_menu : &g_main_menu;
                        break;
                    case 'b':
                        g_blink_enable = !g_blink_enable;
                        GMG12864_Blink_Enable(g_blink_enable);
                        GMG_Menu_Set_blink(&g_main_menu, g_blink_enable);
                        break;
                    case '1':
                        set_style(0);
                        break;
                    case '2':
                        set_style(1);
                        break;
                    case '3':
                        set_style(2);
                        break;
                    case 'q':
                        running = false;
                        break;
                    default:
                        break;
                    }

                    // Правое меню отвечает за стиль левого
                    if (g_active == &g_style_menu)
                    {
                        set_style(GMG_Menu_Get_selected(&g_style_menu));
                    }
                    redraw();
                }
            }
        }
        else
        {
            /*-------------- Авто-демо без терминала -------------------*/
            usleep(1000);
            demo_ms++;
            if (demo_ms >= 700)
            {
                demo_ms = 0;
                GMG_Menu_Down(&g_main_menu);
                redraw();
            }
        }

        /*------------------------- Мигание ---------------------------*/
        // Тик библиотеки - раз в 1 мс. В реальном проекте вызывает
        // прерывание таймера/SysTick
        GMG12864_Tick();
        bool phase = GMG12864_Blink_State();
        if (phase != blink_phase)
        {
            blink_phase = phase;
            redraw();
        }
    }

    tty_raw_restore();
    return 0;
}
