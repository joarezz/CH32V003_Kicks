
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

// keypad.c
#include "keypad.h"
#include "ch32v003fun.h"
#include "timers_utils.h" // Para millis()
#include <stdio.h>
#include <string.h>
#include <stdbool.h> // Para o tipo bool


static Keypad_Config_t _keypadConfig;

// --- Variáveis de estado para o debounce robusto (primeiro toque) ---
static char _lastReturnedKey = 0;           // Última tecla que foi *retornada* pela função getKey()
static unsigned long _keyPressStartTime = 0; // millis() quando a tecla foi detectada *pela primeira vez* como pressionada
static bool _wasKeyPressedLastScan = false; // true se alguma tecla estava pressionada na varredura anterior
static const unsigned long _DEBOUNCE_TIME_MS = 50; // Tempo de debounce em ms (ajuste conforme necessário)


void Keypad_Init(const Keypad_Config_t *config) {
    memcpy(&_keypadConfig, config, sizeof(Keypad_Config_t));

    for (uint8_t i = 0; i < KEYPAD_ROWS; i++) {
        funPinMode(_keypadConfig.rowPins[i], GPIO_CNF_IN_PUPD); 
        funDigitalWrite(_keypadConfig.rowPins[i], FUN_HIGH);
    }

    for (uint8_t i = 0; i < KEYPAD_COLS; i++) {
        funPinMode(_keypadConfig.colPins[i], GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
        funDigitalWrite(_keypadConfig.colPins[i], FUN_HIGH);
    }

    _lastReturnedKey = 0;
    _keyPressStartTime = 0;
    _wasKeyPressedLastScan = false;
}

char Keypad_GetKey() {
    char currentScannedKey = 0; // Tecla detectada nesta varredura (0 se nenhuma)

    // Desativa todas as colunas no início da varredura
    for (uint8_t c_idx = 0; c_idx < KEYPAD_COLS; c_idx++) {
        funDigitalWrite(_keypadConfig.colPins[c_idx], FUN_HIGH);
    }
    
    // Varre o teclado para encontrar a tecla atualmente pressionada
    for (uint8_t c = 0; c < KEYPAD_COLS; c++) {
        funDigitalWrite(_keypadConfig.colPins[c], FUN_LOW); // Ativa a coluna
        
        for (uint8_t r = 0; r < KEYPAD_ROWS; r++) {
            if (funDigitalRead(_keypadConfig.rowPins[r]) == FUN_LOW) {
                currentScannedKey = _keypadConfig.keys[r][c];
                break; // Encontrou uma tecla nesta coluna
            }
        }
        funDigitalWrite(_keypadConfig.colPins[c], FUN_HIGH); // Desativa a coluna
        if (currentScannedKey != 0) {
            break; // Encontrou uma tecla no teclado, pode parar de varrer
        }
    }

    // --- Lógica de Debounce (estado da máquina) ---

    // Caso 1: Nenhuma tecla está pressionada AGORA
    if (currentScannedKey == 0) {
        _wasKeyPressedLastScan = false; // Nenhuma tecla detectada na varredura atual
        _keyPressStartTime = 0;       // Reseta o tempo de início da pressão
        _lastReturnedKey = 0;         // Reseta a última tecla retornada
        return 0; // Nenhuma tecla para retornar
    } 
    // Caso 2: Uma tecla está pressionada AGORA (currentScannedKey != 0)
    else {
        // Se a tecla NÃO estava pressionada na varredura anterior, OU
        // se a tecla é diferente da última que foi retornada (evita repetição da mesma tecla mantida)
        // Isso marca o início de uma NOVA pressão de tecla
        if (!_wasKeyPressedLastScan) {
             _keyPressStartTime = millis(); // Guarda o tempo de início da NOVA pressão
             _wasKeyPressedLastScan = true;  // Marca que uma tecla está pressionada agora
             // Não retornamos a tecla aqui imediatamente, aguardamos o debounce
        }
        
        // Verifica se o tempo de debounce passou
        if (millis() - _keyPressStartTime >= _DEBOUNCE_TIME_MS) {
            // Se o tempo de debounce passou, e a tecla atual NÃO é a mesma que a última retornada
            // (Isso impede que a mesma tecla seja "re-digitada" enquanto é mantida pressionada)
            if (currentScannedKey != _lastReturnedKey) {
                _lastReturnedKey = currentScannedKey; // Armazena a tecla que será retornada
                return currentScannedKey;           // Retorna a tecla válida
            }
        }
    }
    
    return 0; // Nenhuma tecla válida para retornar neste momento
}

