/*
 * GMG12864_lib.h
 *
 *  Версия 1.1.(Общая графическая библиотека Волкова Олега)
 *  Переделан вывод символов на дисплей. В версии 1.1. имеется возможность вывода текста в
 * любой точке по x и y.
 *
 *  Created on: Aug 16, 2021
 *      Author: Oleg Volkov
 *
 *  YouTube: https://www.youtube.com/channel/UCzZKTNVpcMSALU57G1THoVw
 *  GitHub: https://github.com/Solderingironspb/Lessons-Stm32/blob/master/README.md
 *  Группа ВК: https://vk.com/solderingiron.stm32
 *
 *  Кодировка UTF-8 Basic Latin: https://www.utf8-chartable.de/unicode-utf8-table.pl
 *  Кодировка UTF-8 Cyrillic:
 * https://www.utf8-chartable.de/unicode-utf8-table.pl?start=1024&names=-&unicodeinhtml=hex
 *  Программа для конвертации изображения.bmp в bitmap:
 * http://en.radzio.dxp.pl/bitmap_converter/
 */

#ifndef INC_GMG12864_LIB_H_
#define INC_GMG12864_LIB_H_

#include <stdint.h>

#define font3x5 (0)
#define font5x7 (1)
#define inversion_off (0)
#define inversion_on (1)

#ifdef __cplusplus
extern "C"
{
#endif

    /*================= Демонстрационное лого. Можно вырезать. =====================*/
    void GMG12864_logo_demonstration(void);
    /*================= Демонстрационное лого. Можно вырезать. =====================*/

    void GMG12864_Init(void);
    void GMG12864_Clean_Frame_buffer(void);
    void GMG12864_Update(void);
    void GMG12864_DrawBitmap(const uint8_t *bitmap, int8_t x, int8_t y, int8_t w,
                             int8_t h);
    void GMG12864_Draw_pixel(int16_t x, int16_t y, uint8_t color);
    void GMG12864_Print_symbol_5x7(uint8_t x, uint8_t y, uint16_t symbol,
                                   uint8_t inversion);
    void GMG12864_Print_symbol_3x5(uint8_t x, uint8_t y, uint16_t symbol,
                                   uint8_t inversion);
    void GMG12864_Decode_UTF8(uint8_t x, uint8_t y, uint8_t font, bool inversion,
                              const char *tx_buffer);
    void GMG12864_Inversion(uint16_t x_start, uint16_t x_end);
    uint8_t GMG12864_Value_for_Plot(int y_min, int y_max, float value);
    void GMG12864_Fill_the_array_Plot(uint8_t *counter, uint8_t *array,
                                      uint8_t size_array, uint8_t value);
    void GMG12864_Generate_a_Graph(uint8_t *counter, uint8_t *array, uint8_t size_array,
                                   int y_min, int y_max, uint8_t x_grid_time,
                                   uint8_t time_interval, bool grid);
    void GMG12864_Draw_line(uint8_t x_start, uint8_t y_start, uint8_t x_end,
                            uint8_t y_end, uint8_t color);
    void GMG12864_Draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                 uint8_t color);
    void GMG12864_Draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t width,
                                        uint16_t height, uint8_t color);
    void GMG12864_Draw_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color);
    void GMG12864_Draw_circle_filled(int16_t x, int16_t y, int16_t radius, uint8_t color);
    void GMG12864_Draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                                uint16_t x3, uint16_t y3, uint8_t color);
    void GMG12864_Draw_triangle_filled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                                       uint16_t x3, uint16_t y3, uint8_t color);

    int GMG12864_Sprintf(uint8_t px, uint8_t py, const char *fmt, ...);
    int GMG12864_Puts(uint8_t px, uint8_t py, const char *text);

    void GMG12864_Set_contrast(uint8_t contrast);

#ifdef __cplusplus
}
#endif

#endif /* INC_GMG12864_LIB_H_ */
