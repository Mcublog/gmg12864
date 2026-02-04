/**
 * @file ssd1306spi_dev.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Аппаратно зависамая часть работы с контректным контроллером дисплея
 * @version 0.1
 * @date 2026-02-01
 *
 * @copyright Viacheslav mcublog (c) 2026
 *
 */
#include <stdint.h>

#include "ssd1306spi_dev.h"

#include "gmg_macros.h"
#include "gmg_types.h"
#include "ssd1306spi_config.h"
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

static void ssd1306_SetDisplayOn(const uint8_t on) {
    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
    } else {
        value = 0xAE;   // Display off
    }
    send_command(value);
}

static void delay_ms(uint32_t ms)
{
    DELAY_MS(ms);
}

static void ssd1306_Reset(void)
{
    RST_set();
    delay_ms(1);
    RST_reset();
    delay_ms(10);
}

/**
 * @brief
 *
 * @return uint8_t
 */
static uint8_t init(void)
{
    // Reset OLED
    ssd1306_Reset();

    // Wait for the screen to boot
    delay_ms(100);

    // Init OLED
    ssd1306_SetDisplayOn(0); //display off

    send_command(0x20); //Set Memory Addressing Mode
    send_command(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    send_command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    send_command(0xC0); // Mirror vertically
#else
    send_command(0xC8); //Set COM Output Scan Direction
#endif

    send_command(0x00); //---set low column address
    send_command(0x10); //---set high column address

    send_command(0x40); //--set start line address - CHECK

    set_contrast(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    send_command(0xA0); // Mirror horizontally
#else
    send_command(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    send_command(0xA7); //--set inverse color
#else
    send_command(0xA6); //--set normal color
#endif

// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    send_command(0xFF);
#else
    send_command(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
    send_command(0x1F); //
#elif (SSD1306_HEIGHT == 64)
    send_command(0x3F); //
#elif (SSD1306_HEIGHT == 128)
    send_command(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    send_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    send_command(0xD3); //-set display offset - CHECK
    send_command(0x00); //-not offset

    send_command(0xD5); //--set display clock divide ratio/oscillator frequency
    send_command(0xF0); //--set divide ratio

    send_command(0xD9); //--set pre-charge period
    send_command(0x22); //

    send_command(0xDA); //--set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
    send_command(0x02);
#elif (SSD1306_HEIGHT == 64)
    send_command(0x12);
#elif (SSD1306_HEIGHT == 128)
    send_command(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    send_command(0xDB); //--set vcomh
    send_command(0x20); //0x20,0.77xVcc

    send_command(0x8D); //--set DC-DC enable
    send_command(0x14); //
    ssd1306_SetDisplayOn(1); //--turn on SSD1306 panel

    return 0;
}
//<<----------------------

//>>----------------------  Public
const gmg_dev_t *ssd1306spi_dev(void)
{
    static const gmg_dev_t kSsd1306 =
    {
        .init = init,
        .set_contrast = set_contrast,
        .send_command = send_command,
        .send_data = send_data,
        .delay_ms = delay_ms
    };

    return &kSsd1306;
}
//<<----------------------