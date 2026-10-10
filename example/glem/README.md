# README

Минимальный пример: как подключить библиотеку к эмулятору [glem](https://github.com/Mcublog/glem)
и нарисовать кадр — текст и примитивы. Без ввода, без меню, без слоёв.

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
./build/example/glem/example_glem
```

## Что показывает пример

1. Демо-лого библиотеки (`GMG12864_logo_demonstration`).
2. Свой кадр: текст `Hello` шрифтом 5x7 (`GMG12864_Puts`), закрашенный
   прямоугольник, пиксели, линия и окружность.
3. Отправку кадра на дисплей (`GMG12864_Update`).

Более сложный пример (меню с кнопками, стили подсветки, авто-демо) —
в [`../glem-menu/`](../glem-menu/README.md).
