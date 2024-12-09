#ifndef __spi_H
#define __spi_H

void spi_Init(void);
void spi_Start(void);
void spi_Stop(void);
uint8_t spi_SwapByte(uint8_t ByteSend);

#endif
