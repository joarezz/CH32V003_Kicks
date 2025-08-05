// timers_utils.h
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA


#include "max7219.h"
#include "ch32fun.h"
#include <string.h>  // Necessário para strlen(), memset()
#include <stdbool.h> // Necessário para tipo bool
#include <stdio.h>    // Adicione esta linha para usar sprintf (manter a inclusão, mas remover chamadas)

// Definição da fonte padrão (ASCII 32 a 127) - 8x8 pixels por caractere
// Cada caractere é uma coluna de 8 bytes.
// Adaptei esta fonte de https://github.com/nickgammon/MAX7219_Dot_Matrix/blob/master/MAX7219_Dot_Matrix.ino
const uint8_t default_font[][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 32: espaço
    {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00}, // 33: !
    {0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00}, // 34: "
    {0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, 0x00, 0x00}, // 35: #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00, 0x00, 0x00}, // 36: $
    {0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00, 0x00}, // 37: %
    {0x36, 0x49, 0x56, 0x20, 0x50, 0x00, 0x00, 0x00}, // 38: &
    {0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00}, // 39: '
    {0x00, 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x00}, // 40: (
    {0x00, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00}, // 41: )
    {0x14, 0x08, 0x3E, 0x08, 0x14, 0x00, 0x00, 0x00}, // 42: *
    {0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00, 0x00}, // 43: +
    {0x00, 0x50, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00}, // 44: ,
    {0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00}, // 45: -
    {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}, // 46: .
    {0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00, 0x00}, // 47: /
    {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00, 0x00}, // 48: 0
    {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00}, // 49: 1
    {0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00, 0x00}, // 50: 2
    {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00, 0x00, 0x00}, // 51: 3
    {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00, 0x00}, // 52: 4
    {0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00, 0x00}, // 53: 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00, 0x00}, // 54: 6
    {0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00, 0x00}, // 55: 7
    {0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00}, // 56: 8
    {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00, 0x00}, // 57: 9
    {0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00}, // 58: :
    {0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00}, // 59: ;
    {0x14, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00}, // 60: <
    {0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00}, // 61: =
    {0x41, 0x22, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00}, // 62: >
    {0x02, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00, 0x00}, // 63: ?
    {0x3E, 0x41, 0x59, 0x55, 0x5E, 0x00, 0x00, 0x00}, // 64: @
    {0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00, 0x00, 0x00}, // 65: A
    {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00}, // 66: B
    {0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00, 0x00}, // 67: C
    {0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00}, // 68: D
    {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00, 0x00}, // 69: E
    {0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00, 0x00}, // 70: F
    {0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00, 0x00, 0x00}, // 71: G
    {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00, 0x00}, // 72: H
    {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x00}, // 73: I
    {0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, 0x00, 0x00}, // 74: J
    {0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00}, // 75: K
    {0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00}, // 76: L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, 0x00, 0x00}, // 77: M
    {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, 0x00, 0x00}, // 78: N
    {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00, 0x00}, // 79: O
    {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00}, // 80: P
    {0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00, 0x00, 0x00}, // 81: Q
    {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00, 0x00}, // 82: R
    {0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00, 0x00}, // 83: S
    {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x00}, // 84: T
    {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00}, // 85: U
    {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, 0x00, 0x00}, // 86: V
    {0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00, 0x00, 0x00}, // 87: W (pode ser ajustado)
    {0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00, 0x00}, // 88: X
    {0x07, 0x08, 0x70, 0x08, 0x07, 0x00, 0x00, 0x00}, // 89: Y
    {0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00, 0x00}, // 90: Z
    {0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00}, // 91: [
    {0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00, 0x00}, // 92: '\'
    {0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, 0x00}, // 93: ]
    {0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00}, // 94: ^
    {0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00}, // 95: _
    {0x00, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00}, // 96: `
    {0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00, 0x00}, // 97: a
    {0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00}, // 98: b
    {0x38, 0x44, 0x44, 0x44, 0x20, 0x00, 0x00, 0x00}, // 99: c
    {0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, 0x00, 0x00}, // 100: d
    {0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00, 0x00}, // 101: e
    {0x08, 0x7E, 0x09, 0x01, 0x02, 0x00, 0x00, 0x00}, // 102: f
    {0x08, 0x14, 0x34, 0x24, 0x7C, 0x00, 0x00, 0x00}, // 103: g
    {0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00, 0x00}, // 104: h
    {0x00, 0x44, 0x7D, 0x40, 0x00, 0x00, 0x00, 0x00}, // 105: i
    {0x20, 0x40, 0x44, 0x3D, 0x00, 0x00, 0x00, 0x00}, // 106: j
    {0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00}, // 107: k
    {0x00, 0x7F, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00}, // 108: l
    {0x7C, 0x04, 0x18, 0x04, 0x78, 0x00, 0x00, 0x00}, // 109: m
    {0x7C, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00, 0x00}, // 110: n
    {0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00}, // 111: o
    {0x7C, 0x14, 0x14, 0x14, 0x08, 0x00, 0x00, 0x00}, // 112: p
    {0x08, 0x14, 0x14, 0x18, 0x7C, 0x00, 0x00, 0x00}, // 113: q
    {0x7C, 0x08, 0x04, 0x04, 0x08, 0x00, 0x00, 0x00}, // 114: r
    {0x48, 0x54, 0x54, 0x54, 0x20, 0x00, 0x00, 0x00}, // 115: s
    {0x04, 0x3F, 0x44, 0x40, 0x20, 0x00, 0x00, 0x00}, // 116: t
    {0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00, 0x00, 0x00}, // 117: u
    {0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, 0x00, 0x00}, // 118: v
    {0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00, 0x00, 0x00}, // 119: w (pode ser ajustado)
    {0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00, 0x00}, // 120: x
    {0x07, 0x08, 0x70, 0x08, 0x07, 0x00, 0x00, 0x00}, // 121: y
    {0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x00, 0x00}, // 122: z
    {0x00, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x00}, // 123: {
    {0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00}, // 124: |
    {0x00, 0x41, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00}, // 125: }
    {0x08, 0x08, 0x2A, 0x1C, 0x08, 0x00, 0x00, 0x00}, // 126: ~
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 127: DEL (ou char final, pode ser preenchido)
};

// Funções auxiliares para SPI bit-banging
void _MAX7219_writeData(uint8_t data) {
    for (int i = 8; i > 0; i--) {
        // Põe o CLK em LOW
        funDigitalWrite(MAX7219_CLK_PIN, FUN_LOW);
        // Coloca o bit de dados
        if (data & 0x80) { // MSB first
            funDigitalWrite(MAX7219_DIN_PIN, FUN_HIGH);
        } else {
            funDigitalWrite(MAX7219_DIN_PIN, FUN_LOW);
        }
        data <<= 1;
        // Põe o CLK em HIGH
        funDigitalWrite(MAX7219_CLK_PIN, FUN_HIGH);
    }
}

void _MAX7219_writeRegister(uint8_t reg, uint8_t data) {
    // CS LOW para iniciar a transmissão
    funDigitalWrite(MAX7219_CS_PIN, FUN_LOW);
    _MAX7219_writeData(reg);
    _MAX7219_writeData(data);
    // CS HIGH para finalizar a transmissão
    funDigitalWrite(MAX7219_CS_PIN, FUN_HIGH);
}

// Inicializa o módulo MAX7219
void MAX7219_init(void) {
    // Configura os pinos como Output Push-Pull
    funPinMode(MAX7219_DIN_PIN, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(MAX7219_CLK_PIN, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(MAX7219_CS_PIN, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);

    // Definir o estado inicial dos pinos (CS e CLK em HIGH, DIN em LOW)
    funDigitalWrite(MAX7219_CS_PIN, FUN_HIGH);
    funDigitalWrite(MAX7219_CLK_PIN, FUN_LOW); // O CLK geralmente começa baixo
    funDigitalWrite(MAX7219_DIN_PIN, FUN_LOW);

    // Inicialização do MAX7219
    _MAX7219_writeRegister(0x09, 0x00); // Decode mode: no decode
    _MAX7219_writeRegister(0x0A, 0x0F); // Intensity: max
    _MAX7219_writeRegister(0x0B, 0x07); // Scan limit: display all 8 digits
    _MAX7219_writeRegister(0x0C, 0x01); // Shutdown mode: normal operation
    _MAX7219_writeRegister(0x0F, 0x00); // Display test: normal operation
    MAX7219_clearDisplay();
}

// Desliga/Liga o display
void MAX7219_shutdown(int mode) {
    _MAX7219_writeRegister(0x0C, mode & 0x01); // 0x00 para shutdown, 0x01 para normal
}

// Define a intensidade (brilho) do display
void MAX7219_setIntensity(int intensity) {
    if (intensity >= 0 && intensity <= 15) {
        _MAX7219_writeRegister(0x0A, intensity);
    }
}

// Limpa todos os LEDs do display
void MAX7219_clearDisplay(void) {
    for (int i = 1; i <= 8; i++) {
        _MAX7219_writeRegister(i, 0x00);
    }
}

// Escreve um byte em uma linha específica (row) do display
void MAX7219_writeRow(int row, uint8_t value) {
    if (row >= 1 && row <= 8) {
        _MAX7219_writeRegister(row, value);
    }
}

// Exibe uma imagem 8x8 em um único display MAX7219
void MAX7219_displayImage(const uint8_t image[8]) {
    for (int i = 0; i < 8; i++) {
        _MAX7219_writeRegister(i + 1, image[i]);
    }
}

// Exibe uma imagem 8xN onde N é MAX7219_NUM_DISPLAYS
void MAX7219_displayImageMulti(const uint8_t image[8][MAX7219_NUM_DISPLAYS]) {
    for (int r = 0; r < 8; r++) { // Itera através das linhas (0-7)
        funDigitalWrite(MAX7219_CS_PIN, FUN_LOW); // CS LOW para iniciar a transmissão
        // Envia dados para cada display na ordem inversa (cadeia daisy chain)
        for (int d = MAX7219_NUM_DISPLAYS - 1; d >= 0; d--) {
            _MAX7219_writeData(r + 1);       // Registrador é o número da linha (1-8)
            _MAX7219_writeData(image[r][d]); // Dados para essa linha
        }
        funDigitalWrite(MAX7219_CS_PIN, FUN_HIGH); // CS HIGH para finalizar a transmissão
    }
}

// Constantes para a fonte
#define FONT_CHAR_WIDTH     8   // Largura de um caractere em pixels
#define FONT_SPACE_WIDTH    1   // Largura do espaço entre caracteres em pixels
#define FONT_START_CHAR     32  // Caractere ASCII inicial da fonte (' ')
#define FONT_NUM_CHARS      96  // Número total de caracteres na fonte (de ' ' a '~')

// Função para exibir um único caractere no display (a que você já testou)
void MAX7219_displayChar(char c) {
    uint8_t frame_buffer[8][MAX7219_NUM_DISPLAYS];
    memset(frame_buffer, 0, sizeof(frame_buffer)); // Limpa o buffer

    if (c >= FONT_START_CHAR && c < (FONT_START_CHAR + FONT_NUM_CHARS)) {
        const uint8_t *char_data = default_font[c - FONT_START_CHAR];
        for (int r = 0; r < 8; r++) { // Para cada linha (row)
            uint8_t row_data = 0;
            for (int p = 0; p < FONT_CHAR_WIDTH; p++) { // Para cada pixel na coluna do caractere
                if (char_data[p] & (1 << r)) { // Verifica o bit da linha 'r' na coluna 'p'
                    row_data |= (1 << (7 - p)); // Mapeia o pixel para a coluna correta do display
                }
            }
            frame_buffer[r][0] = row_data; // Assume display 0 (o primeiro, se houver múltiplos)
        }
    }
    MAX7219_displayImageMulti(frame_buffer);
}


// Função para rolar texto
void MAX7219_scrollText(const char *text, int speed_ms, int direction) {
    int text_len = strlen(text);
    int text_pixel_width = 0;
    for (int i = 0; i < text_len; i++) {
        text_pixel_width += FONT_CHAR_WIDTH;
        if (i < text_len - 1) { // Adiciona espaço entre caracteres, exceto o último
            text_pixel_width += FONT_SPACE_WIDTH;
        }
    }

    int display_width_pixels = MAX7219_NUM_DISPLAYS * 8; // Largura total em pixels de todos os displays

    int start_offset;
    int end_offset;
    int step_direction;

    if (direction == SCROLL_LEFT) {
        start_offset = display_width_pixels;      // Começa com o texto à direita do display
        end_offset = -(text_pixel_width);         // Termina quando o texto sair pela esquerda
        step_direction = -1;                      // Move um pixel para a esquerda por vez
    } else { // SCROLL_RIGHT
        start_offset = -(text_pixel_width);       // Começa com o texto à esquerda do display, fora
        end_offset = display_width_pixels;        // Termina quando o texto sair pela direita
        step_direction = 1;                       // Move um pixel para a direita por vez
    }

    // Loop principal de rolagem
    for (int current_scroll_pos = start_offset; ; current_scroll_pos += step_direction) {
        uint8_t current_display_frame[8][MAX7219_NUM_DISPLAYS];
        memset(current_display_frame, 0, sizeof(current_display_frame)); // Limpa o frame buffer

        for (int r = 0; r < 8; r++) { // Itera sobre as 8 linhas do display
            for (int d = 0; d < MAX7219_NUM_DISPLAYS; d++) { // Itera sobre cada módulo de display
                uint8_t module_data = 0; // CORREÇÃO: Declarado aqui, no escopo correto do display (d)
                for (int p = 0; p < 8; p++) { // Itera sobre os 8 pixels de cada display
                    int global_pixel_x = (d * 8) + p; // Posição X do pixel no total de displays
                    // Calcula a posição do pixel dentro da largura total do texto
                    int pixel_in_text_x = global_pixel_x - current_scroll_pos;

                    bool pixel_is_on = false;

                    // Se o pixel atual está dentro da área do texto
                    if (pixel_in_text_x >= 0 && pixel_in_text_x < text_pixel_width) {
                        int current_char_x_pos = 0; // Posição inicial do caractere atual dentro do texto
                        for (int char_idx = 0; char_idx < text_len; char_idx++) {
                            int char_total_width = FONT_CHAR_WIDTH + (char_idx < text_len - 1 ? FONT_SPACE_WIDTH : 0);

                            // Verifica se o pixel_in_text_x está dentro do caractere atual
                            if (pixel_in_text_x >= current_char_x_pos &&
                                pixel_in_text_x < (current_char_x_pos + FONT_CHAR_WIDTH)) {
                                int pixel_offset_within_char = pixel_in_text_x - current_char_x_pos;

                                char current_char = text[char_idx];
                                // Verifica se o caractere está no range da fonte
                                if (current_char >= FONT_START_CHAR && current_char < (FONT_START_CHAR + FONT_NUM_CHARS)) {
                                    const uint8_t *char_data = default_font[current_char - FONT_START_CHAR];
                                    // Os dados da fonte são colunas de 8 bytes, onde cada byte é uma linha.
                                    // Então, char_data[coluna_do_caractere] dá os 8 bits para essa coluna.
                                    // Para o bit da linha 'r', verificamos o (1 << r) nesse byte.
                                    if (char_data[pixel_offset_within_char] & (1 << r)) {
                                        pixel_is_on = true;
                                    }
                                }
                                // Se o pixel está no espaço entre caracteres, pixel_is_on permanece false (desligado)
                                break; // Caractere encontrado, parar de procurar
                            }
                            current_char_x_pos += char_total_width; // Mover para a posição do próximo caractere
                        }
                    }

                    // Empacotar o pixel no byte module_data para envio ao MAX7219
                    if (pixel_is_on) {
                        module_data |= (1 << (7 - p)); // Mapeamento padrão (MSB para a coluna mais à esquerda)
                    }
                }
                current_display_frame[r][d] = module_data; // Atribui o dado acumulado para o display e linha
            }
        }

        MAX7219_displayImageMulti(current_display_frame); // Exibe o quadro renderizado
        Delay_Ms(speed_ms);                               // Pequeno atraso para a rolagem

        // Condições de parada da rolagem
        if (step_direction > 0 && current_scroll_pos >= end_offset) { // Rolando para a direita
            break;
        }
        if (step_direction < 0 && current_scroll_pos <= end_offset) { // Rolando para a esquerda
            break;
        }
    }
    MAX7219_clearDisplay(); // Limpa o display após a rolagem
}
