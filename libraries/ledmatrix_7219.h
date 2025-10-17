// ARQUIVO: ledmatrix_7219.h
// JOAREZ DE OLIVEIRA DA FONSECA
// KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
// FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
// DATA: 13/10/2025

#ifndef LEDMATRIX_7219_H
#define LEDMATRIX_7219_H

#include <stdint.h>
#include "ch32fun.h"

// --- Constantes para direção de rolagem ---
#define SCROLL_LEFT   0
#define SCROLL_RIGHT  1

// --- Funções Públicas da Biblioteca ---

/**
 * @brief Inicializa a biblioteca da matriz de LED.
 * @param din_pin Pino de dados (DIN) do MAX7219.
 * @param clk_pin Pino de clock (CLK) do MAX7219.
 * @param cs_pin Pino de Chip Select (CS) do MAX7219.
 * @param num_displays O número de módulos 8x8 em cascata.
 */
void LedMatrix_init(uint8_t din_pin, uint8_t clk_pin, uint8_t cs_pin, uint8_t num_displays);

/**
 * @brief Desliga ou liga os displays.
 * @param mode 0 para desligar (shutdown), 1 para ligar (operação normal).
 */
void LedMatrix_shutdown(int mode);

/**
 * @brief Define a intensidade do brilho de todos os displays.
 * @param intensity Valor de 0 (mínimo) a 15 (máximo).
 */
void LedMatrix_setIntensity(int intensity);

/**
 * @brief Apaga todos os LEDs de todos os displays.
 */
void LedMatrix_clearAll(void);

/**
 * @brief Exibe uma imagem completa nos displays.
 * O buffer da imagem deve ter um tamanho de (8 * num_displays) bytes.
 * @param image_buffer Ponteiro para o buffer da imagem.
 */
void LedMatrix_displayImage(const uint8_t *image_buffer);

/**
 * @brief Exibe um texto estático no display.
 * O texto será truncado se for maior que a largura dos displays.
 * @param text A string de texto a ser exibida.
 */
void LedMatrix_displayText(const char *text);

/**
 * @brief Rola um texto através dos displays.
 * @param text A string de texto a ser rolada.
 * @param speed_ms Atraso em milissegundos entre cada passo da rolagem.
 * @param direction Direção da rolagem (SCROLL_LEFT ou SCROLL_RIGHT).
 */
void LedMatrix_scrollText(const char *text, int speed_ms, int direction);

#endif // LEDMATRIX_7219_H