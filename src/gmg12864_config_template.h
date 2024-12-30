
#ifndef GMG12864_CONFIG_TEMPLATE_H
#define GMG12864_CONFIG_TEMPLATE_H

/*Внимание! Для совместимости с контроллерами STM32F4хх и другими,
для выключения ножки, используем регистр BSRR, т.к. регистр BRR в них уже отсутствует */
// Собственно, ниже, так и сделано:

#define cs_set()    do{CS_GPIO_Port->BSRR = (uint32_t) CS_Pin << 16u;}while(0)   //CS_притягиваем к земле
#define cs_reset()  do{CS_GPIO_Port-> BSRR = CS_Pin;}while(0)                    //CS притягиваем к 3.3v
#define RST_set()   do{RST_GPIO_Port->BSRR = (uint32_t) RST_Pin << 16u;}while(0) //RST притягиваем к земле
#define RST_reset() do{RST_GPIO_Port-> BSRR = RST_Pin;}while(0)                  //RST притягиваем к 3.3v
#define DC_set()    do{DC_GPIO_Port->BSRR = (uint32_t) DC_Pin << 16u;}while(0)   //DC притягиваем к земле
#define DC_reset()  do{DC_GPIO_Port-> BSRR = DC_Pin;}while(0)                    //DC притягиваем к 3.3v

#define SPI_HANDLE (hspi2)
extern SPI_HandleTypeDef SPI_HANDLE; //Используемая шина spi


#endif // GMG12864_CONFIG_TEMPLATE_H