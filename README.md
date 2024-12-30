# Дисплеи ST7920 и GMG12864(ST7565r). Обновление граф. библиотек. Режим плоттера.

![GMG12864-06D](/doc/GMG12864.jpg)

## GMG12864-06D(ST7565r). Название библиотеки: GMG12864_lib

```bash
Перед использованием библиотеки, настройте назначение ног МК(файл GMG12864_lib.h)

Настройки spi:
Basic Parameters:
Frame Format Motorola
Data Size 8 Bits
First Bit MSB First
Clock Parameters:
Prescaler (for Baud Rate) 4 *
Baud Rate 18.0 MBits/s * (Можно еще быстрее)
Clock Polarity (CPOL) Low
Clock Phase (CPHA) 1 Edge
Advanced Parameters:
CRC Calculation Disabled
NSS Signal Type Software
```

```c
Функции:
/*================= Демонстрационное лого. Можно вырезать. =====================*/
void GMG12864_logo_demonstration(void);
/*================= Демонстрационное лого. Можно вырезать. =====================*/

void GMG12864_Init(void); //Функция инициализации дисплея
void GMG12864_Clean_Frame_buffer(void); //Функция очистки буфера кадра
void GMG12864_Update(void); //Функция вывода буфера кадра на дисплей
void GMG12864_DrawBitmap(const uint8_t *bitmap, int8_t x, int8_t y, int8_t w, int8_t h); //Функция вывода изображения с указанием его размера
void GMG12864_Draw_pixel(int16_t x, int16_t y, uint8_t color); //Функция рисования пикселя
void GMG12864_Print_symbol_5x7(uint8_t x, uint8_t y, uint16_t symbol,uint8_t inversion); //Функция вывода символа на дисплей(шрифт 5*7)
void GMG12864_Print_symbol_3x5(uint8_t x, uint8_t y, uint16_t symbol,uint8_t inversion); //Функция вывода символа на дисплей(шрифт 3*5)
void GMG12864_Decode_UTF8(uint8_t x, uint8_t y, uint8_t font, bool inversion, char *tx_buffer); //Функция декодирования UTF-8 в набор символов
void GMG12864_Inversion(uint16_t x_start, uint16_t x_end); //Функция инверсии любого места в буфере
uint8_t GMG12864_Value_for_Plot(int y_min, int y_max, float value); //Функция, формирующая точку значения на графике
void GMG12864_Fill_the_array_Plot(uint8_t *counter, uint8_t *array, uint8_t size_array, uint8_t value); //Функция, заполняющая массив значениями, чтоб отрисовывать график
void GMG12864_Generate_a_Graph(uint8_t *counter, uint8_t *array, uint8_t size_array, int y_min, int y_max, uint8_t x_grid_time, uint8_t time_interval, bool grid); //Функция, формирующая весь график, который будет отображаться на дисплее
void GMG12864_Draw_line(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color); //Функция, рисующая линию
void GMG12864_Draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color); //Функция, рисующая прямоугольник
void GMG12864_Draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color); //Функция, рисующая закрашенный прямоугольник
void GMG12864_Draw_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color); //Функция, рисующая окружность
void GMG12864_Draw_circle_filled(int16_t x, int16_t y, int16_t radius, uint8_t color); //Функция, рисующая закрашенную окружность
void GMG12864_Draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color); //Функция, рисующая треугольник
void GMG128640_Draw_triangle_filled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color); //Функция, рисующая закрашенный треугольник
```

## Полезные ссылки

* Материалы из видео **[[Скачать]](https://github.com/Solderingironspb/Lessons-Stm32/archive/Practice%2311.zip)**
* [[Главная страница]](https://github.com/Solderingironspb/Lessons-Stm32/blob/master/README.md)
