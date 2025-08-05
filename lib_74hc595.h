
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef LIB_74HC595_H
#define LIB_74HC595_H

#include <stdint.h>
#include "ch32v003fun.h"

// Inicialização (define os pinos de controle)
void HC595_Init(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin);

// Escreve um byte (8 saídas) - Função original, para uso interno ou único SR
void HC595_WriteByte(uint8_t value);

// NOVO: Escreve múltiplos bytes para 74HC595 em cascata
// data_array: Array de bytes com os dados para cada SR. data_array[0] para o primeiro SR, data_array[1] para o segundo, etc.
// num_of_sr: Quantidade de 74HC595 em cascata
void HC595_WriteCascadedBytes(uint8_t *data_array, uint8_t num_of_sr);

// Interface tipo funDigitalWrite (bit por bit) - se ainda for usada
// void DigitalWrite(uint8_t pin, uint8_t state); // Esta não é relevante para a lib do 595

#endif