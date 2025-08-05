
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#include "lcd_i2c_pcf8574.h"
#include "lib_i2c.h"     // Inclua a nova biblioteca I2C aqui primeiro
#include <stdio.h>
#include <stdarg.h>    // ADICIONADO: Para suporte a funções variádicas (printf)

// Variável para armazenar o estado do backlight
static uint8_t _lcd_backlight_state = LCD_BACKLIGHT_PIN; // Backlight ligado por padrão
static uint8_t _lcd_i2c_address; // Variável para armazenar o endereço I2C do LCD

// Variáveis globais para controle de erros no LCD
static int lcd_consecutive_errors = 0;
const int LCD_ERROR_THRESHOLD = 3; // Definir um limite de erros consecutivos antes de tentar resetar

// Protótipos das funções estáticas (privadas) - Assinaturas atualizadas
static uint8_t pcf8574_i2c_write_byte(uint8_t data);
static uint8_t lcd_send_nibble(uint8_t nibble, uint8_t rs_bit); // Retorna uint8_t (0=ok, 1=erro)
static void lcd_command(uint8_t cmd); // ATUALIZADO: Retorno void
static void lcd_data(uint8_t data);   // ATUALIZADO: Retorno void

void lcd_put_char(char c) {
    lcd_data(c); // Chama a função interna lcd_data
}

// --- Nova função para obter o valor de lcd_consecutive_errors ---
int lcd_get_consecutive_errors() {
    return lcd_consecutive_errors;
}

// --- Funções I2C de baixo nível para o PCF8574 (adaptadas para lib_i2c) ---

// Escreve um byte para o PCF8574 via I2C
// Retorna 0 se sucesso, 1 se falha
static uint8_t pcf8574_i2c_write_byte(uint8_t data) {
    if (i2c_write_bytes(_lcd_i2c_address, &data, 1) != I2C_OK) {
        lcd_consecutive_errors++;
        if (lcd_consecutive_errors >= LCD_ERROR_THRESHOLD) {
            // Tentar re-inicializar o I2C ou o LCD se muitos erros
            // Serial_println("Too many LCD errors, attempting reset..."); // Descomente para depurar
            // i2c_init(I2C_PINOUT_DEFAULT); // Tenta reiniciar o I2C (use seu modo de pinagem)
            // lcd_init(_lcd_i2c_address, 16, 2); // Tenta reiniciar o LCD (ajuste cols/rows se necessário)
            // lcd_consecutive_errors = 0; // Reseta o contador de erros após a tentativa
        }
        return 1; // Falha na escrita I2C
    }
    lcd_consecutive_errors = 0; // Reseta o contador de erros se a operação foi bem-sucedida
    return 0; // Sucesso
}

// Envia um nibble (4 bits) para o LCD através do PCF8574
// rs_bit: LCD_RS_COMMAND ou LCD_RS_DATA
// Retorna 0 se sucesso, 1 se falha
static uint8_t lcd_send_nibble(uint8_t nibble, uint8_t rs_bit) {
    // Monta o byte para enviar ao PCF8574
    // O nibble de dados está nos bits D4-D7 do PCF8574 (shifted << 4)
    // rs_bit: RS (Register Select), LCD_RW_PIN_OFF: R/W (Read/Write) sempre 0 para escrita,
    // _lcd_backlight_state: estado do backlight
    uint8_t data_to_send = (nibble << 4) | rs_bit | LCD_RW_PIN_OFF | _lcd_backlight_state;

    // Pulso EN (Enable) - Alto
    if (pcf8574_i2c_write_byte(data_to_send | LCD_EN_PIN) != 0) {
        return 1; // Erro ao ligar EN
    }
    Delay_Us(1); // Pequeno atraso para o pulso EN

    // Pulso EN - Baixo
    if (pcf8574_i2c_write_byte(data_to_send & ~LCD_EN_PIN) != 0) {
        return 1; // Erro ao desligar EN
    }
    return 0; // Sucesso
}

// --- Funções de alto nível para controle do LCD ---

// Envia um comando para o LCD
static void lcd_command(uint8_t cmd) { // ATUALIZADO: Retorno void
    // Envia o nibble superior do comando
    if (lcd_send_nibble(cmd >> 4, LCD_RS_COMMAND) != 0) return; // Se houver erro, sai
    Delay_Us(40); // Atraso após o nibble superior

    // Envia o nibble inferior do comando
    if (lcd_send_nibble(cmd & 0x0F, LCD_RS_COMMAND) != 0) return; // Se houver erro, sai
    Delay_Us(40); // Pequeno atraso após o nibble inferior
}

// Envia um byte de dados para o LCD (caractere)
static void lcd_data(uint8_t data) { // ATUALIZADO: Retorno void
    // Envia o nibble superior do dado
    if (lcd_send_nibble(data >> 4, LCD_RS_DATA) != 0) return; // Se houver erro, sai
    Delay_Us(40); // Atraso após o nibble superior

    // Envia o nibble inferior do dado
    if (lcd_send_nibble(data & 0x0F, LCD_RS_DATA) != 0) return; // Se houver erro, sai
    Delay_Us(40); // Pequeno atraso após o nibble inferior
}


// Inicializa o módulo LCD I2C
bool lcd_init(uint8_t i2c_address, uint8_t cols, uint8_t rows) { // ATUALIZADO: com parâmetros e retorno bool
    _lcd_i2c_address = i2c_address; // Armazena o endereço I2C
    
    // Tenta pingar o endereço I2C para verificar se o LCD está conectado
    if (i2c_ping(_lcd_i2c_address) != I2C_OK) {
        // Serial_println("LCD I2C device not found at address."); // Descomente para depurar
        return false; // Retorna falha se o dispositivo não responder
    }

    // Sequência de inicialização para LCD HD44780 em modo 4 bits
    Delay_Ms(50); // Atraso de mais de 40ms após VCC para estabilização

    // 1. Resetar o LCD (enviar 0x03 três vezes)
    // Isso é feito em modo 8 bits, enviando apenas o nibble superior.
    // O modo 8 bits é o padrão após o reset.
    lcd_send_nibble(0x03, LCD_RS_COMMAND);
    Delay_Ms(5); // Espera >4.1ms

    lcd_send_nibble(0x03, LCD_RS_COMMAND);
    Delay_Us(150); // Espera >100us

    lcd_send_nibble(0x03, LCD_RS_COMMAND);
    Delay_Us(150); // Espera >100us

    // 2. Definir modo de 4 bits
    lcd_send_nibble(0x02, LCD_RS_COMMAND); // Entra no modo 4 bits
    Delay_Us(40); // Pequeno atraso

    // 3. Configurar função (4 bits, 2 linhas, 5x8 pontos)
    // LCD_FUNCTIONSET | LCD_4BITMODE (já implícito) | LCD_2LINE | LCD_5x8DOTS
    lcd_command(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS);
    Delay_Us(40);

    // 4. Ligar o display, desligar cursor, desligar blink
    lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    Delay_Us(40);

    // 5. Limpar o display
    lcd_command(LCD_CLEARDISPLAY);
    Delay_Ms(2); // Comando CLEAR exige um atraso maior (aprox. 1.52ms)

    // 6. Entry Mode Set (incrementa cursor, sem shift do display)
    // ADICIONADO: Usando flags corretas para Entry Mode
    lcd_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_AUTOSCROLL_OFF);
    Delay_Us(40);

    // Garante que o backlight esteja ligado por padrão
    lcd_backlight(true); // ATUALIZADO: Chamada com parâmetro `true`

    return true; // Retorna sucesso
}

// Limpa o display e retorna o cursor para a posição inicial (0,0)
void lcd_clear() {
    lcd_command(LCD_CLEARDISPLAY);
    Delay_Ms(2); // Atraso necessário após limpar o display
}

// Define a posição do cursor no LCD
void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t address = 0x00;
    // Mapeamento para LCD 20x4 (ou 16x2, que só usaria linhas 0 e 1)
    switch (row) {
        case 0:
            address = col;
            break;
        case 1:
            address = col + 0x40;
            break;
        case 2:
            address = col + 0x14; // Para LCDs 20x4, esta é a 3ª linha
            break;
        case 3:
            address = col + 0x54; // Para LCDs 20x4, esta é a 4ª linha
            break;
        default:
            address = col; // Padrão para linha 0 se linha inválida
            break;
    }
    lcd_command(LCD_SETDDRAMADDR | address);
}

// Exibe uma string de texto no LCD na posição atual do cursor
void lcd_print(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

// Exibe uma string formatada (similar a printf) no LCD
void lcd_printf(const char *format, ...) {
    char buffer[64]; // Buffer para a string formatada. Ajuste o tamanho conforme necessário.
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    lcd_print(buffer);
}

// Desliga o backlight do LCD
void lcd_no_backlight(void) {
    _lcd_backlight_state = 0x00; // Desliga o bit do backlight
    // Envia um comando dummy para atualizar o estado do PCF8574
    // Isso garante que o backlight seja atualizado imediatamente
    pcf8574_i2c_write_byte(LCD_RW_PIN_OFF | LCD_RS_COMMAND | _lcd_backlight_state);
}

// Liga o backlight do LCD
void lcd_backlight(bool on) { // ATUALIZADO: Com parâmetro `on`
    if (on) {
        _lcd_backlight_state = LCD_BACKLIGHT_PIN; // Liga o bit do backlight
    } else {
        _lcd_backlight_state = 0x00; // Desliga o bit do backlight
    }
    // Envia um comando dummy para atualizar o estado do PCF8574
    pcf8574_i2c_write_byte(LCD_RW_PIN_OFF | LCD_RS_COMMAND | _lcd_backlight_state);
}
