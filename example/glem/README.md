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
