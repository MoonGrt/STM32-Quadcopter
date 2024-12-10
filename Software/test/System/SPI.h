#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

// #define SOFT_SPI
#define HARD_SPI SPI2
#define HARD_SPI_RCC RCC_APB1Periph_SPI2 // SPI2时钟

#define SPI_PIN_RCC RCC_APB2Periph_GPIOB // SPI使用的GPIO时钟
#define SPI_PIN_GPIO GPIOB               // SPI使用的GPIO端口
#define MOSI_PIN GPIO_Pin_13             // MOSI引脚
#define MISO_PIN GPIO_Pin_14             // MISO引脚
#define SCK_PIN GPIO_Pin_15              // SCK引脚

void SPI_INIT(void);
uint8_t SPI_SwapByte(uint8_t ByteSend);

#endif
