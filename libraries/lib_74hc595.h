
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef LIB_74HC595_H
#define LIB_74HC595_H

#include <stdint.h>
#include "ch32v003fun.h"

// Inicialização (agora recebe o número de CIs em cascata)
void HC595_Init(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin, uint8_t num_of_sr);

// Escreve um byte (8 saídas)
void HC595_WriteByte(uint8_t value);

// Escreve múltiplos bytes para 74HC595 em cascata
void HC595_WriteCascadedBytes(uint8_t *data_array, uint8_t num_of_sr);

// NOVO: Função para controlar um pino específico (0 a N*8 - 1)
void HC595_DigitalWrite(uint8_t bitIndex, uint8_t state);

#endif

