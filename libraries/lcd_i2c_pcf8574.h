
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef _LCD_I2C_PCF8574_H_
#define _LCD_I2C_PCF8574_H_

#include <stdint.h>
#include <stdbool.h>   // Para o tipo bool
#include "lib_i2c.h"     // Nova biblioteca I2C
#include "ch32v003fun.h" // Para usar Delay_Us/Ms

// Endereço I2C do módulo LCD (PCF8574)
// O mais comum é 0x27 ou 0x3F. Verifique o seu módulo.
// #define LCD_I2C_ADDRESS 0x27 // Agora passado como parâmetro em lcd_init

// --- Pinos do PCF8574 para o LCD ---
// Mapeamento dos bits do byte enviado ao PCF8574 para os pinos do LCD
#define LCD_RS_PIN          0x01 // P0 do PCF8574 -> RS do LCD (Bit 0)
#define LCD_RW_PIN          0x02 // P1 do PCF8574 -> R/W do LCD (Bit 1)
#define LCD_RW_PIN_OFF      0x00 // Define R/W como OFF (0) para escrita
#define LCD_EN_PIN          0x04 // P2 do PCF8574 -> Enable do LCD (Bit 2)
#define LCD_BACKLIGHT_PIN   0x08 // P3 do PCF8574 -> Backlight (Bit 3)
// P4-P7 do PCF8574 são para dados (D4-D7)

// --- Comandos do LCD (HD44780) ---
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

// Flags para Entry Mode Set
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_AUTOSCROLL_ON       0x01
#define LCD_AUTOSCROLL_OFF      0x00

// Flags para Display Control
#define LCD_DISPLAYON       0x04 // Liga o display
#define LCD_DISPLAYOFF      0x00 // Desliga o display
#define LCD_CURSORON        0x02 // Liga o cursor
#define LCD_CURSOROFF       0x00 // Desliga o cursor
#define LCD_BLINKON         0x01 // Liga o blink do cursor
#define LCD_BLINKOFF        0x00 // Desliga o blink do cursor

// Cursor or Display Shift
#define LCD_DISPLAYMOVE     0x08
#define LCD_CURSORMOVE      0x00
#define LCD_MOVERIGHT       0x04
#define LCD_MOVELEFT        0x00

// Function Set
#define LCD_8BITMODE        0x10 // Interface de dados de 8 bits
#define LCD_4BITMODE        0x00 // Interface de dados de 4 bits
#define LCD_2LINE           0x08 // Display de 2 linhas
#define LCD_1LINE           0x00 // Display de 1 linha
#define LCD_5x10DOTS        0x04 // Matriz de caracteres 5x10
#define LCD_5x8DOTS         0x00 // Matriz de caracteres 5x8

// Flags para controle interno
#define LCD_RS_COMMAND      0x00
#define LCD_RS_DATA         0x01 // Usado com LCD_RS_PIN


// Funções públicas (assinaturas atualizadas)
bool lcd_init(uint8_t i2c_address, uint8_t cols, uint8_t rows);
// REMOVIDAS: void lcd_command(uint8_t cmd);
// REMOVIDAS: void lcd_data(uint8_t data);
void lcd_clear(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_printf(const char *format, ...);
void lcd_no_backlight(void);
void lcd_backlight(bool on);
int lcd_get_consecutive_errors(void); // Adicionada para obter o contador de erros
void lcd_put_char(char c); // ADICIONADO: Nova função para escrever um único caractere


#endif // _LCD_I2C_PCF8574_H_
