
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

// keypad.h
#ifndef KEYPAD_H
#define KEYPAD_H

#include "ch32v003fun.h"
#include <stdint.h>
#include <stdbool.h> // Para o tipo bool, se não for incluído por ch32v003fun.h

#ifdef __cplusplus
extern "C" {
#endif

// Define o número de linhas e colunas do teclado
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// Estrutura para armazenar a configuração do teclado
typedef struct {
    uint32_t rowPins[KEYPAD_ROWS];
    uint32_t colPins[KEYPAD_COLS];
    char keys[KEYPAD_ROWS][KEYPAD_COLS];
} Keypad_Config_t;

// Protótipos das funções da biblioteca
void Keypad_Init(const Keypad_Config_t *config);
char Keypad_GetKey();
void Keypad_PrintConfig(const Keypad_Config_t *config);

#ifdef __cplusplus
}
#endif

#endif // KEYPAD_H