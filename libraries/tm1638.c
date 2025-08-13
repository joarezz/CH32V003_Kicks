//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://www.youtube.com/@kickstech


#include "tm1638.h"
#include "ch32v003fun.h"
#include <string.h>
#include <stdio.h> 

static uint8_t STB, CLK, DIO;

#define delayTiny() for (volatile int i = 0; i < 10; i++) __NOP();

// Envia um byte de dados
static void TM_send(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        funDigitalWrite(CLK, 0);
        delayTiny();
        if (data & 0x01) {
            funDigitalWrite(DIO, 1);
        } else {
            funDigitalWrite(DIO, 0);
        }
        delayTiny();
        funDigitalWrite(CLK, 1);
        delayTiny();
        data >>= 1;
    }
}

// Lê um byte de dados
static uint8_t TM_read(void) {
    uint8_t data = 0;
    funPinMode(DIO, GPIO_CNF_IN_FLOATING);
    for (uint8_t i = 0; i < 8; i++) {
        funDigitalWrite(CLK, 0);
        delayTiny();
        data >>= 1;
        if (funDigitalRead(DIO)) {
            data |= 0x80;
        }
        delayTiny();
        funDigitalWrite(CLK, 1);
        delayTiny();
    }
    funPinMode(DIO, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    return data;
}

// Inicializa o módulo TM1638
void TM1638_Init(uint8_t stbPin, uint8_t clkPin, uint8_t dioPin) {
    STB = stbPin;
    CLK = clkPin;
    DIO = dioPin;

    funPinMode(STB, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(CLK, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(DIO, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);

    funDigitalWrite(STB, 1);
    funDigitalWrite(CLK, 1);

    // Comando de configuração do display (Display ON, brilho 7 de 8)
    funDigitalWrite(STB, 0);
    TM_send(0x8F); 
    funDigitalWrite(STB, 1);

    // Comando de modo de dados (auto-increment)
    funDigitalWrite(STB, 0);
    TM_send(0x40); 
    funDigitalWrite(STB, 1);

    TM1638_clearDisplay();
}

// Limpa todos os segmentos e LEDs
void TM1638_clearDisplay(void) {
    funDigitalWrite(STB, 0);
    TM_send(0x40); // modo de endereço fixo
    funDigitalWrite(STB, 1);

    funDigitalWrite(STB, 0);
    TM_send(0xC0); // endereço 0

    for (uint8_t i = 0; i < 16; i++) {
        TM_send(0x00);
    }

    funDigitalWrite(STB, 1);
}

// Liga ou desliga um LED (1 a 8)
void TM1638_setLed(uint8_t ledIndex, uint8_t state) {
    if (ledIndex < 1 || ledIndex > 8) return;

    funDigitalWrite(STB, 0);
    TM_send(0x44); // modo de endereço fixo
    funDigitalWrite(STB, 1);

    funDigitalWrite(STB, 0);
    TM_send(0xC0 + (ledIndex - 1) * 2); 
    TM_send(state ? 0x01 : 0x00);
    funDigitalWrite(STB, 1);
}


static const uint8_t charSegmentMap[] = {
    0x3F, // '0'
    0x06, // '1'
    0x5B, // '2'
    0x4F, // '3'
    0x66, // '4'
    0x6D, // '5'
    0x7D, // '6'
    0x07, // '7'
    0x7F, // '8'
    0x6F, // '9'
    0x77, // 'A'
    0x7C, // 'B'
    0x39, // 'C'
    0x5E, // 'D'
    0x79, // 'E'
    0x71, // 'F'
    0x3D, // 'G'
    0x76, // 'H'
    0x04, // 'I'
    0x0E, // 'J'
    0x75, // 'K'
    0x38, // 'L'
    0x15, // 'M'
    0x54, // 'N'
    0x5C, // 'O'
    0x73, // 'P'
    0x6B, // 'Q'
    0x50, // 'R'
    0x6D, // 'S'
    0x78, // 'T'
    0x3E, // 'U'
    0x3E, // 'V'
    0x6A, // 'W'
    0x76, // 'X'
    0x6E, // 'Y'
    0x5B, // 'Z'
    0x40, // '-'
    0x00  // ' ' (space)
};

static uint8_t getSegment(char c) {
    if (c >= '0' && c <= '9') {
        return charSegmentMap[c - '0'];
    }
    if (c >= 'A' && c <= 'Z') {
        return charSegmentMap[c - 'A' + 10];
    }
    if (c >= 'a' && c <= 'z') {
        return charSegmentMap[c - 'a' + 10];
    }
    if (c == '-') {
        return charSegmentMap[36];
    }
    return charSegmentMap[37];
}

void TM1638_displayNumber(uint32_t number) {
    TM1638_clearDisplay();
    char buffer[9];
    sprintf(buffer, "%8lu", number);

    funDigitalWrite(STB, 0);
    TM_send(0x40);
    funDigitalWrite(STB, 1);

    funDigitalWrite(STB, 0);
    TM_send(0xC0);
    for (int i = 0; i < 8; i++) {
        TM_send(getSegment(buffer[i]));
        TM_send(0x00);
    }
    funDigitalWrite(STB, 1);
}

void TM1638_displayString(const char* s) {
    TM1638_clearDisplay();
    size_t len = strlen(s);
    if (len > 8) len = 8;

    funDigitalWrite(STB, 0);
    TM_send(0x40);
    funDigitalWrite(STB, 1);

    funDigitalWrite(STB, 0);
    TM_send(0xC0);

    for (size_t i = 0; i < len; i++) {
        TM_send(getSegment(s[i]));
        TM_send(0x00);
    }
    for (size_t i = len; i < 8; i++) {
        TM_send(0x00);
        TM_send(0x00);
    }
    funDigitalWrite(STB, 1);
}

void TM1638_displayTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    char timeStr[9];
    sprintf(timeStr, "%02d%02d%02d", hours, minutes, seconds);

    funDigitalWrite(STB, 0);
    TM_send(0x40);
    funDigitalWrite(STB, 1);

    funDigitalWrite(STB, 0);
    TM_send(0xC0);

    TM_send(getSegment(timeStr[0])); // H
    TM_send(0x00);
    TM_send(getSegment(timeStr[1])); // H
    TM_send(0x00);
    TM_send(getSegment(timeStr[2]) | 0x80); // M (com ponto)
    TM_send(0x00);
    TM_send(getSegment(timeStr[3])); // M
    TM_send(0x00);
    TM_send(getSegment(timeStr[4]) | 0x80); // S (com ponto)
    TM_send(0x00);
    TM_send(getSegment(timeStr[5])); // S
    TM_send(0x00);
    TM_send(0x00);
    TM_send(0x00);
    TM_send(0x00);
    TM_send(0x00);

    funDigitalWrite(STB, 1);
}

uint8_t TM1638_readButtons(void) {
    uint8_t buttons = 0;
    funDigitalWrite(STB, 0);
    TM_send(0x42); // read key scan data
    buttons = TM_read();
    funDigitalWrite(STB, 1);
    return buttons;
}
