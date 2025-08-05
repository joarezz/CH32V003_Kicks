
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#include "lib_74hc595.h"

static uint8_t sr_data_pin, sr_clock_pin, sr_latch_pin;
// static uint8_t shift_reg_value = 0; // Esta variável não é mais estritamente necessária aqui

void HC595_Init(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin) {
    sr_data_pin  = data_pin;
    sr_clock_pin = clock_pin;
    sr_latch_pin = latch_pin;

    funPinMode(sr_data_pin,  GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(sr_clock_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(sr_latch_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);

    // Agora, para a limpeza inicial, usamos a nova função para múltiplos SRs
    // Assume 2 SRs por padrão na inicialização para limpar.
    uint8_t initial_clear_data[2] = {0, 0};
    HC595_WriteCascadedBytes(initial_clear_data, 2); 
    // Ou HC595_WriteByte(0); duas vezes se não quiser passar a qtd aqui.
    // Para ser mais genérico, a chamada original era HC595_WriteByte(0);
    // Vamos manter a chamada original para HC595_WriteByte, mas ela só afeta 1 SR.
    // Para limpar múltiplos SRs de forma genérica, o ideal seria HC595_Init_Multi(..., num_sr_at_init)
    // Por enquanto, mantenha o HC595_WriteByte(0); original se ele funciona, ou faça um loop de 0's se preciso.
    // Para simplificar, vou manter como estava e a nova função tratará a limpeza via main ou chamada.
    // HC595_WriteByte(0); // limpa inicial para um SR
}


// Escreve um byte (8 saídas) - Função original
void HC595_WriteByte(uint8_t value) {
    funDigitalWrite(sr_latch_pin, FUN_LOW);
    for (int8_t i = 7; i >= 0; i--) {
        funDigitalWrite(sr_clock_pin, FUN_LOW);
        funDigitalWrite(sr_data_pin, (value >> i) & 0x01);
        funDigitalWrite(sr_clock_pin, FUN_HIGH);
    }
    funDigitalWrite(sr_latch_pin, FUN_HIGH);
}


// NOVO: Implementação para múltiplos 74HC595 em cascata
void HC595_WriteCascadedBytes(uint8_t *data_array, uint8_t num_of_sr) {
    funDigitalWrite(sr_latch_pin, FUN_LOW); // Latch em LOW para toda a transmissão

    // Iterar do ÚLTIMO 74HC595 na array (que é o PRIMEIRO a receber os bits)
    // até o PRIMEIRO 74HC595 na array (que é o ÚLTIMO a receber os bits)
    // Isso é crucial para a ordem da cascata.
    // Ex: Se data_array[0] = unidades, data_array[1] = dezenas
    // Para que dezenas vá para o SR da esquerda e unidades para o SR da direita
    // Precisamos enviar dezenas (data_array[1]) primeiro, depois unidades (data_array[0])
    // Então, o loop deve ir de (num_of_sr - 1) até 0.
    for (int8_t sr_index = num_of_sr - 1; sr_index >= 0; sr_index--) {
        uint8_t current_byte = data_array[sr_index];
        for (int8_t bit_index = 7; bit_index >= 0; bit_index--) {
            funDigitalWrite(sr_clock_pin, FUN_LOW);
            funDigitalWrite(sr_data_pin, (current_byte >> bit_index) & 0x01);
            funDigitalWrite(sr_clock_pin, FUN_HIGH);
        }
    }
    funDigitalWrite(sr_latch_pin, FUN_HIGH); // Latch em HIGH para travar todos os dados
}

// void DigitalWrite(uint8_t pin, uint8_t state) { } // Esta função não é relevante para a lib do 595
