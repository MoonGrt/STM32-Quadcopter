#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"
#include <stdio.h>

extern void USART1_SendByte(const int8_t *Data, uint8_t len);
extern void USART3_SendByte(const int8_t *Data, uint8_t len);
extern void USART1_Config(u32 bound);
extern void USART3_Config(u32 bound);
extern int fputc(int ch, FILE *f);
extern void USART1_setBaudRate(uint32_t baudRate);

#endif /* __USART1_H */
