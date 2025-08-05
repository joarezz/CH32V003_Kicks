//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//05/08/2025
//https://www.youtube.com/@kickstech

#ifndef TM1638_H
#define TM1638_H

#include <stdint.h>

// Inicializa o TM1638 com os pinos conectados
void TM1638_Init(uint8_t stbPin, uint8_t clkPin, uint8_t dioPin);

// Liga ou desliga um LED (1 a 8)
void TM1638_setLed(uint8_t ledIndex, uint8_t state);

// Escreve até 8 dígitos no display (usa mapa básico de segmentos)
void TM1638_displayNumber(uint32_t number);

// Exibe a hora no formato "-HHMMSS-"
void TM1638_displayTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

// Limpa todos os dígitos
void TM1638_clearDisplay(void);

// Lê o estado dos 8 botões (1 = pressionado)
uint8_t TM1638_readButtons(void);

#endif
