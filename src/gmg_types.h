#pragma once

#include <stdint.h>

typedef struct gmg_dev_t
{
    uint8_t (*init)(void);
    uint8_t (*set_contrast)(uint8_t contrast);
    uint8_t (*send_command)(uint8_t cmd);
    uint8_t (*put_pixel)(int16_t x, int16_t y, uint8_t clr);
    uint8_t (*send_data)(const uint8_t *data, uint16_t size);
    void    (*delay_ms)(uint32_t ms);
} gmg_dev_t;