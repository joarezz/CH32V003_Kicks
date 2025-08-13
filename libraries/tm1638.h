//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//05/08/2025
//https://www.youtube.com/@kickstech

#ifndef TM1638_H
#define TM1638_H

#include <stdint.h>

void TM1638_Init(uint8_t stbPin, uint8_t clkPin, uint8_t dioPin);
void TM1638_setLed(uint8_t ledIndex, uint8_t state);
void TM1638_displayNumber(uint32_t number);
void TM1638_displayString(const char* s);
void TM1638_displayTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void TM1638_clearDisplay(void);
uint8_t TM1638_readButtons(void);

#endif

