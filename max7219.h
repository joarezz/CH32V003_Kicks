// timers_utils.h
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>
#include "ch32fun.h" // Inclua ch32fun.h para os defines de pinos como PD0, PA1 etc.

// Defina os pinos SPI para o MAX7219 usando a nomenclatura da ch32fun
#define MAX7219_DIN_PIN   PC7 // Exemplo: Altere conforme seus pinos
#define MAX7219_CLK_PIN   PC6 // Exemplo: Altere conforme seus pinos
#define MAX7219_CS_PIN    PC5 // Exemplo: Altere conforme seus pinos

// !!! MUITO IMPORTANTE !!! Defina este valor de acordo com o NÚMERO DE MÓDULOS MAX7219 QUE VOCÊ TEM CONECTADOS.
// Se você tem 1 módulo, use 1. Se tem 4 módulos, use 4.
#define MAX7219_NUM_DISPLAYS 1 // Ajuste conforme a quantidade de módulos que você tem

// Constantes para direção de rolagem
#define SCROLL_LEFT   0
#define SCROLL_RIGHT  1

// Buffer de rolagem ajustado para ser a largura do display + uma margem para rolagem.
// O valor será limitado a 64 pixels para não estourar a RAM do CH32V003 (para texto longo).

// Funções de inicialização e controle do MAX7219
void MAX7219_init(void);
void MAX7219_shutdown(int mode);
void MAX7219_setIntensity(int intensity);
void MAX7219_clearDisplay(void);
void MAX7219_writeRow(int row, uint8_t value);
void MAX7219_displayImage(const uint8_t image[8]); // Para um único módulo
void MAX7219_displayImageMulti(const uint8_t image[8][MAX7219_NUM_DISPLAYS]); // Para múltiplos módulos

// Função de rolagem de texto
void MAX7219_scrollText(const char *text, int speed_ms, int direction);

#endif // MAX7219_H