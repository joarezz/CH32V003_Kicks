// timers_utils.h
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef TIMERS_UTILS_H
#define TIMERS_UTILS_H

#include "ch32v003fun.h" // Inclua ch32v003fun.h para TIM1 e SystemCoreClock

#ifdef __cplusplus
extern "C" {
#endif

// --- Variáveis Globais (extern para serem acessíveis de outros arquivos) ---
// DECLARAÇÃO: 'extern' informa ao compilador que SystemCoreClock é definida em outro lugar.
extern volatile unsigned int SystemCoreClock; // Deve ser 'extern' aqui

extern volatile unsigned long millis_counter;
extern volatile int lcd_update_flag;
extern volatile int lcd_message_state;

// --- Funções Públicas ---
void setupTimer1();
unsigned long millis();
unsigned long micros();

#ifdef __cplusplus
}
#endif

#endif // TIMERS_UTILS_H
