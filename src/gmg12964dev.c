/**
 * @file gmg12964dev.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Аппаратно зависамая часть работы с контректным контроллером дисплея
 * @version 0.1
 * @date 2026-02-01
 *
 * @copyright Viacheslav mcublog (c) 2026
 *
 */
#include "gmg12864dev.h"

#include "gmg12864_config.h"
#include "gmg_macros.h"
#include "gmg_types.h"

//>>----------------------  Private
/**
 * @brief
 *
 * @param cmd
 * @return uint8_t
 */
static uint8_t send_command(uint8_t cmd)
{
    DC_set();
    cs_set();
    HAL_SPI_Transmit(&SPI_HANDLE, &cmd, 1, HAL_MAX_DELAY);
    while (HAL_SPI_GetState(&SPI_HANDLE) != HAL_SPI_STATE_READY)
        ;
    cs_reset();
    DC_reset();
    return 0;
}

/**
 * @brief
 *
 * @param data
 * @param size
 * @return uint8_t
 */
static uint8_t send_data(const uint8_t *data, uint16_t size)
{
    DC_reset();
    cs_set();

    HAL_SPI_Transmit(&SPI_HANDLE, (uint8_t*)data, size, HAL_MAX_DELAY);
    while (HAL_SPI_GetState(&SPI_HANDLE) != HAL_SPI_STATE_READY)
        ;
    cs_reset();
    DC_set();
    return 0;
}

static uint8_t set_contrast(uint8_t contrast)
{
    // войти в режим динамического контраста
    send_command(0x81); // Electronic Volume
    send_command(contrast);
    return 0;
}

/**
 * @brief
 *
 * @return uint8_t
 */
static uint8_t init(void)
{
    /// Функция инициализации дисплея
    cs_set();
    RST_set();
    DELAY_MS(10);
    RST_reset();
    DELAY_MS(10);
    cs_reset();
    // Установите рабочий цикл ( 1/7 или 1/9 ) в зависимости от физического ЖК-дисплея
    send_command(0xA2);

    // Установите горизонтальную и вертикальную ориентацию в известное состояние
#ifdef GMG12864_OLED
    send_command(0xA1); // ADC selection(SEG0->SEG128)
#else
    send_command(0xA0);
#endif
    send_command(0xC8); // SHL selection(COM0->COM64)

    // делитель внутреннего резистора установлен на 7 (от 0..7)
    send_command(0x20 | 0x7); // Regulator Resistor Selection

    // управление питанием, все внутренние блоки включены	(от 0..7)
    send_command(0x28 | 0x7);

    // войти в режим динамического контраста
    set_contrast(7);

    send_command(0x40);

    // CMD_DISPLAY_ON  CMD_DISPLAY_OFF
    send_command(0xAF); // Display on
    // Инвертирование экрана
    send_command(0xA6); // 0xA6 - nomal, 0xA7 - revers
    return 0;
}
//<<----------------------

//>>----------------------  Public
const gmg_dev_t *gmg12864_dev(void)
{
    static const gmg_dev_t kGmg12864 =
    {
        .init = init,
        .set_contrast = set_contrast,
        .send_command = send_command,
        .send_data = send_data,
    };

    return &kGmg12864;
}
//<<----------------------