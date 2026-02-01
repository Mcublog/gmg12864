#pragma once

#define SSD1306_HEIGHT (64U)

#define DELAY_MS(x) do{HAL_Delay(x);}while (0)

#define SSD1306_I2C_ADDR        (0x3C << 1)

#define I2C_HANDLE (hi2c1)
extern I2C_HandleTypeDef I2C_HANDLE; //Используемая шина i2x
