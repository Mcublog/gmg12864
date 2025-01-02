
#ifndef GMG12864_CONFIG_TEMPLATE_H
#define GMG12864_CONFIG_TEMPLATE_H

#define cs_set()    do{HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);}while(0)   //CS_притягиваем к земле
#define cs_reset()  do{HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);}while(0)     //CS притягиваем к 3.3v
#define RST_set()   do{HAL_GPIO_WritePin(RST_GPIO_Port, RST__Pin, GPIO_PIN_RESET);}while(0) //RST притягиваем к земле
#define RST_reset() do{HAL_GPIO_WritePin(RST_GPIO_Port, RST__Pin, GPIO_PIN_SET);}while(0)   //RST притягиваем к 3.3v
#define DC_set()    do{HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);}while(0)    //DC притягиваем к земле
#define DC_reset()  do{HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);}while(0)      //DC притягиваем к 3.3v

#define DELAY_MS(x) do{HAL_Delay(x);}while (0)

#define SPI_HANDLE (hspi2)
extern SPI_HandleTypeDef SPI_HANDLE; //Используемая шина spi

#endif // GMG12864_CONFIG_TEMPLATE_H