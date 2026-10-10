# README

Нужно собрать библиотеку [glem](https://github.com/Mcublog/glem)
И установить её, я предочитаю ставить ~/.local/lib/
Чтобы библитека и загловки потом нашлиcь можно прописать в .bashrc

```bash
# Extend path to headers and libs
export CPATH=~/.local/include/experments:$CPATH
export LIBRARY_PATH=~/.local/lib/experments:$LIBRARY_PATH
export LD_LIBRARY_PATH=~/.local/lib/experments:$LD_LIBRARY_PATH
```

Перед запуском нужно запустить glem серве, который тоже можно устнановить в
переменное окружение.

## Сборка и запуск

```sh
cmake -B build && cmake --build build

glem -r 128x64 -s4 &
./build/example/glem-menu/example_glem_menu
```

## Как устроен пример

Код разбит на слои, чтобы демо можно было перенести на железо почти без правок:

| Файл | Что внутри | Куда переносить |
| --- | --- | --- |
| `main.c` | Суперцикл (~1 мс) и инициализация | заменить своим |
| `tty_keys.c/h` | Единственное место с POSIX (termios, poll, read): терминал отдаёт маску нажатых кнопок | заменить опросом GPIO/EXTI |
| `btn.c/h` | Антидребезг, PRESS/RELEASE, авто-повтор, долгое нажатие, очередь событий. Железа не знает | копировать как есть |
| `app.c/h` | Меню, отрисовка, реакция на кнопки. Платформы не знает | копировать как есть |

Управление (4 направления джойстика + центральная кнопка):

```txt
UP / DOWN  - перемещение по меню (с автоповтором при удержании)
LEFT/RIGHT - стиль подсветки: SELECTOR / INVERSE / FRAME
ENTER      - короткое: вкл/выкл мигание подсветки
ENTER      - удержание 1 с: выход (на МК - возврат на родительский экран)
```

Без терминала (`./example_glem_menu < /dev/null`) включается авто-демо: события
кнопок генерируются по таймеру, меню и стили меняются сами.

## Перенос на bare metal

1. Скопировать в проект: `src/gmg12864lib.c`, `src/gmg12864menu.c`, бэкенд дисплея
   (`src/ssd1306dev.c`, `src/gmg12964dev.c` или свой) и файлы `example/glem-menu/btn.c`,
   `app.c` с заголовками.
2. Реализовать `btn_io_t` вместо `tty_keys.c` — две функции:

```c
static const btn_io_t kMyIo = {
    .raw    = my_buttons,  /* битовая маска: BTN_BIT(BTN_UP) | ... */
    .now_ms = my_ticks,    /* HAL_GetTick(), счётчик SysTick и т.п. */
};

static uint32_t my_buttons(void)
{
    uint32_t mask = 0;
    if (HAL_GPIO_ReadPin(UP_GPIO, UP_PIN) == GPIO_PIN_RESET)    { mask |= BTN_BIT(BTN_UP); }
    if (HAL_GPIO_ReadPin(DOWN_GPIO, DOWN_PIN) == GPIO_PIN_RESET){ mask |= BTN_BIT(BTN_DOWN); }
    ...
    return mask;
}
```

1. Вызвать из кода:

```c
GMG12864_Init(&my_dev);   // свой бэкенд дисплея
app_init();
btn_init(&kMyIo, &kBtnCfg);

while (1)
{
    btn_task();            // опрос кнопок, можно и из задачи RTOS
    btn_event_t ev;
    while (btn_get_event(&ev))
    {
        app_on_button(&ev);
    }
    app_task_1ms();         // тик библиотеки раз в 1 мс + перерисовка
}
```

`GMG12864_Tick()` внутри `app_task_1ms()` рассчитан на период 1 мс: на МК удобнее
вызывать его прямо из SysTick, а `app_task_1ms()` оставить в суперцикле.

1. Заменить действие `ENTER` в `app_on_button()` на своё: `GMG_Menu_Get_value()`
   вернёт `value` выделенного пункта, под него и пишется `switch/case`.
   Само короткое `ENTER` освобождается под выбор пункта, а долгое — под
   «назад/выход».

Терминальная эмуляция ввода (`tty_keys.c`) не умеет различать скорость отпускания:
каждый принятый байт «зажимает» кнопку на 150 мс. Поэтому тапы чаще 150 мс могут
склеиться в удержание — на реальных GPIO такого нет, там источник истины - уровень.
