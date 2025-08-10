
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#include "lib_74hc595.h"

static uint8_t sr_data_pin, sr_clock_pin, sr_latch_pin;

// Definimos um número máximo de 74HC595 que a biblioteca pode suportar.
// Se você precisar de mais que 4, basta aumentar este valor.
#define MAX_SR 4

// Variáveis estáticas para o estado interno da biblioteca
static uint8_t _currentOutputStates[MAX_SR];
static uint8_t _num_of_sr;

// NOVO: A função de inicialização agora recebe o número de CIs
void HC595_Init(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin, uint8_t num_of_sr) {
    sr_data_pin  = data_pin;
    sr_clock_pin = clock_pin;
    sr_latch_pin = latch_pin;
    _num_of_sr = num_of_sr; // Armazena o número de CIs na variável estática

    funPinMode(sr_data_pin,  GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(sr_clock_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(sr_latch_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);

    // Limpa o array de estado e envia para todos os CIs
    for (int i = 0; i < _num_of_sr; i++) {
        _currentOutputStates[i] = 0;
    }
    HC595_WriteCascadedBytes(_currentOutputStates, _num_of_sr);
}


// Implementação para múltiplos 74HC595 em cascata (permanece igual)
void HC595_WriteCascadedBytes(uint8_t *data_array, uint8_t num_of_sr) {
    funDigitalWrite(sr_latch_pin, FUN_LOW);

    for (int8_t sr_index = num_of_sr - 1; sr_index >= 0; sr_index--) {
        uint8_t current_byte = data_array[sr_index];
        for (int8_t bit_index = 7; bit_index >= 0; bit_index--) {
            funDigitalWrite(sr_clock_pin, FUN_LOW);
            funDigitalWrite(sr_data_pin, (current_byte >> bit_index) & 0x01);
            funDigitalWrite(sr_clock_pin, FUN_HIGH);
        }
    }
    funDigitalWrite(sr_latch_pin, FUN_HIGH);
}

// NOVO: Implementação da função para controlar um único bit
void HC595_DigitalWrite(uint8_t bitIndex, uint8_t state) {
    uint8_t sr_index = bitIndex / 8;
    uint8_t bit_within_sr = bitIndex % 8;

    if (sr_index < _num_of_sr) { // Garante que o índice esteja dentro do limite
        if (state == FUN_HIGH) {
            _currentOutputStates[sr_index] |= (1 << bit_within_sr);
        } else {
            _currentOutputStates[sr_index] &= ~(1 << bit_within_sr);
        }
        HC595_WriteCascadedBytes(_currentOutputStates, _num_of_sr);
    }
}


