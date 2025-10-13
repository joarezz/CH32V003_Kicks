//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//Codigo exemplo com keypad teclado matricial 
//12/10/2025
//https://youtu.be/-PVUfetwLzw



#include "ch32v003fun.h"
#include "lib_uart.h"
#include "timers_utils.h"
#include "lib_74hc595.h" 
#include <stdio.h>

// --- Configuração do Hardware (Versão Simplificada e Melhorada) ---
#define LATCH_PIN         PC5 // Controla RCK do 595 e PL# do 165
#define CLOCK_PIN         PC6 // Controla SCK do 595 e CP do 165
#define SR_DATA_OUT_PIN   PC7 // Data PARA o 74HC595 (SER)
#define SR_DATA_IN_PIN    PC4 // Data VINDO do 74HC165 (Q7)

const char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Adicionamos a função que faltava na biblioteca
void HC595_WriteByte(uint8_t value) {
    funDigitalWrite(LATCH_PIN, FUN_LOW); // Usa o nome simplificado
    for (int8_t i = 7; i >= 0; i--) {
        funDigitalWrite(CLOCK_PIN, FUN_LOW); // Usa o nome simplificado
        funDigitalWrite(SR_DATA_OUT_PIN, (value >> i) & 0x01);
        funDigitalWrite(CLOCK_PIN, FUN_HIGH); // Usa o nome simplificado
    }
    funDigitalWrite(LATCH_PIN, FUN_HIGH); // Usa o nome simplificado
}

uint8_t HC165_Read() {
    uint8_t data = 0;
    funDigitalWrite(LATCH_PIN, FUN_LOW); // Usa o nome simplificado
    Delay_Us(5);
    funDigitalWrite(LATCH_PIN, FUN_HIGH); // Usa o nome simplificado

    for (int i = 0; i < 8; i++) {
        if (i > 0) {
            funDigitalWrite(CLOCK_PIN, FUN_LOW); // Usa o nome simplificado
            Delay_Us(5);
            funDigitalWrite(CLOCK_PIN, FUN_HIGH); // Usa o nome simplificado
        }
        if (funDigitalRead(SR_DATA_IN_PIN)) {
            data |= (1 << (7 - i));
        }
    }
    return data;
}

char scanKeypad() {
    for (int row = 0; row < 4; row++) {
        uint8_t output_byte = ~(1 << row);
        HC595_WriteByte(output_byte);
        Delay_Us(10); 
        uint8_t col_data = HC165_Read();
        for (int col = 0; col < 4; col++) {
            if (!((col_data >> col) & 0x01)) {
                HC595_WriteByte(0xFF);
                return keys[row][col];
            }
        }
    }
    HC595_WriteByte(0xFF);
    return 0;
}

int main(void) {
    SystemInit();
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD;
    
    Serial_begin(115200);
    setupTimer1();

    Serial_println("Iniciando Teclado com 74HC595 e 74HC165...");

    // A inicialização agora usa os nomes simplificados e dedicados
    HC595_Init(SR_DATA_OUT_PIN, CLOCK_PIN, LATCH_PIN, 1);
    
    funPinMode(SR_DATA_IN_PIN, GPIO_CNF_IN_FLOATING);

    Serial_println("Sistema pronto. Pressione uma tecla.");

    char keyCharBuffer[2];
    static char ultimaTeclaLida = 0; 

    while (1) {
        char teclaAtual = scanKeypad();

        if (teclaAtual && teclaAtual != ultimaTeclaLida) {
            sprintf(keyCharBuffer, "%c", teclaAtual);
            Serial_println(keyCharBuffer);
        }
        
        ultimaTeclaLida = teclaAtual;
        Delay_Ms(10);
    }
}
