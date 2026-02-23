/**
 * @file glem_dev.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Используется для эмуляции на компе с пощью
 * библиотекеи glem
 * https://github.com/Mcublog/glem
 *
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */
#include <stdbool.h>

#include <glem.h>

#include "glem_dev.h"

#include "../gmg_types.h"
/// >>----------------------  Private
#define DISP_WIDTH  (128U)
#define DISP_HEIGHT (64U)

enum
{
    X_GETTING = 0xFF,
    Y_GETTING = 0xFE,
};

typedef struct
{
    uint8_t x;
    uint8_t y;
} coord_t;

static coord_t m_coord = {0};

uint8_t m_buffer[DISP_WIDTH / 8 * DISP_HEIGHT] = {0};

void set_bit(uint8_t x, uint8_t y, bool bit)
{
    if (x >= DISP_WIDTH || y >= DISP_HEIGHT)
    {
        return;
    }

    uint16_t byte_index = (y * DISP_WIDTH + x) / 8;
    uint8_t bit_pos = 7 - (x % 8); // Инвертируем позицию бита для MSB first

    if (bit)
    {
        m_buffer[byte_index] |= (1 << bit_pos);
    }
    else
    {
        m_buffer[byte_index] &= ~(1 << bit_pos);
    }
}

/**
 * @brief
 *
 * @param cmd
 * @return uint8_t
 */
static uint8_t send_command(uint8_t cmd)
{
    if (cmd == 0x10)
    {
        m_coord.x = 0;
        m_coord.y = 0;
        // puts("first x cmd, reset coord");
    }
    else if (cmd < 0x0F)
    {
        // puts("second x cmd, wait y");
        m_coord.x = X_GETTING;
    }
    else if (m_coord.x == X_GETTING && (cmd & 0xB0))
    {
        m_coord.x = Y_GETTING;
        m_coord.y = cmd & 0x07;
        // printf("y: %d\n", m_coord.y);
    }

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
    if (m_coord.x == Y_GETTING && m_coord.y <= 0x07)
    {
        // puts("fill line");
        // memcpy(&m_buffer[size * m_coord.y], data, size);
        if (m_coord.y == 7)
        {
            glem_write(m_buffer, sizeof(m_buffer));
        }
        m_coord.x = 0;
        m_coord.y = 0;
    }

    return 0;
}

static uint8_t set_contrast(uint8_t contrast)
{
    return 0;
}

static void delay_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

static uint8_t put_pixel(int16_t x, int16_t y, uint8_t color)
{
    set_bit(x, y, color);
}

static uint8_t init(void)
{
    glem_init(DISP_WIDTH, DISP_HEIGHT, 0);
    return 0;
}
//<<----------------------

//>>----------------------  Public
const gmg_dev_t *glem_dev(void)
{
    static const gmg_dev_t kGlemDev =
    {
        .init = init,
        .set_contrast = set_contrast,
        .send_command = send_command,
        .put_pixel = put_pixel,
        .send_data = send_data,
        .delay_ms = delay_ms
    };

    return &kGlemDev;
}
//<<----------------------
