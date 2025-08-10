
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef LIB_74HC165_H
#define LIB_74HC165_H

#include <stdint.h>
#include "ch32v003fun.h"

// Inicialização
void HC165_Init(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin);

// Atualiza o registrador interno com novo valor
void HC165_Update();

// Interface tipo funDigitalRead (bit por bit)
int DigitalRead(uint8_t pin);

#endif



