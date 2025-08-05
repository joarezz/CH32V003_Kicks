//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://www.youtube.com/@kickstech


#include "tm1638.h"
#include "ch32v003fun.h"

static uint8_t STB, CLK, DIO;

#define delayTiny() for (volatile int i = 0; i < 10; i++) __NOP();

static void TM_send(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        funDigitalWrite(CLK, 0);
        delayTiny();
        funDigitalWrite(DIO, data & 0x01);
        delayTiny();
        funDigitalWrite(CLK, 1);
        delayTiny();
        data >>= 1;
    }
}

static uint8_t TM_read(void) {
    uint8_t data = 0;
    funPinMode(DIO, GPIO_CNF_IN_FLOATING);
    for (uint8_t i = 0; i < 8; i++) {
        data >>= 1;
        funDigitalWrite(CLK, 0);
        delayTiny();
        if (funDigitalRead(DIO)) data |= 0x80;
        delayTiny();
        funDigitalWrite(CLK, 1);
        delayTiny();
    }
    funPinMode(DIO, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    return data;
}

void TM1638_Init(uint8_t stbPin, uint8_t clkPin, uint8_t dioPin) {
    STB = stbPin;
    CLK = clkPin;
    DIO = dioPin;

    funPinMode(STB, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(CLK, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(DIO, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);

    funDigitalWrite(STB, 1);
    funDigitalWrite(CLK, 1);
    funDigitalWrite(DIO, 1);

    funDigitalWrite(STB, 0);
    TM_send(0x8F); // Display ON, brilho max
    funDigitalWrite(STB, 1);

    TM1638_clearDisplay();
}

void TM1638_clearDisplay(void) {
    funDigitalWrite(STB, 0);
    TM_send(0x40); // auto-increment
    funDigitalWrite(STB, 1);

    funDigitalWrite(STB, 0);
    TM_send(0xC0); // addr 0

    for (uint8_t i = 0; i < 16; i++) {
        TM_send(0x00);
    }

    funDigitalWrite(STB, 1);
}

void TM1638_setLed(uint8_t ledIndex, uint8_t state) {
    if (ledIndex < 1 || ledIndex > 8) return;

    funDigitalWrite(STB, 0);
    TM_send(0x44); // fixed addr
    funDigitalWrite(STB, 1);

    funDigitalWrite(STB, 0);
    TM_send(0xC1 + (ledIndex - 1) * 2); // LEDs nos endereços ímpares
    TM_send(state ? 0xFF : 0x00);
    funDigitalWrite(STB, 1);
}

// Mapa de segmentos para os dígitos e caracteres
static const uint8_t segmentMap[12] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x40, // Hífen (-)
    0x00  // Apagado
};

void TM1638_displayNumber(uint32_t number) {
    TM1638_clearDisplay();

    // Usa o modo de endereço fixo para preencher da direita para a esquerda
    uint32_t temp = number;

    // Loop para os 8 dígitos (da direita para a esquerda)
    for (int i = 7; i >= 0; i--) {
        uint8_t digit = temp % 10;

        funDigitalWrite(STB, 0);
        TM_send(0x44); // fixed addr
        funDigitalWrite(STB, 1);

        funDigitalWrite(STB, 0);
        TM_send(0xC0 + i * 2); // Endereço para o display na posição `i`

        // Exibe o dígito, ou se for zero, apenas se for a primeira posição (unidade)
        if (temp > 0 || (temp == 0 && i == 7)) {
            TM_send(segmentMap[digit]);
            temp /= 10;
        } else {
            TM_send(segmentMap[11]); // Apaga os dígitos restantes
        }

        funDigitalWrite(STB, 1);
    }
}

void TM1638_displayTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    // Escreve os dois hífens nos displays 0 e 7
    funDigitalWrite(STB, 0);
    TM_send(0x44); // fixed addr
    funDigitalWrite(STB, 1);
    funDigitalWrite(STB, 0);
    TM_send(0xC0); // addr do primeiro display
    TM_send(segmentMap[10]); // Hífen
    TM_send(0xC0 + 7 * 2); // addr do último display
    TM_send(segmentMap[10]); // Hífen
    funDigitalWrite(STB, 1);

    // Preenche o resto do display com zeros se não houverem dados
    funDigitalWrite(STB, 0);
    TM_send(0x40); // auto-increment
    funDigitalWrite(STB, 1);
    funDigitalWrite(STB, 0);
    TM_send(0xC0 + 1 * 2); // Começa a escrever no display 1 (HH)
    TM_send(segmentMap[hours / 10]);
    TM_send(segmentMap[hours % 10]);
    TM_send(segmentMap[minutes / 10] | 0x80); // Minutos com ponto
    TM_send(segmentMap[minutes % 10]);
    TM_send(segmentMap[seconds / 10] | 0x80); // Segundos com ponto
    TM_send(segmentMap[seconds % 10]);
    funDigitalWrite(STB, 1);
}

uint8_t TM1638_readButtons(void) {
    uint8_t buttons = 0;

    funDigitalWrite(STB, 0);
    TM_send(0x42); // read keys

    for (int i = 0; i < 4; i++) {
        uint8_t data = TM_read();
        buttons |= (data << i);
    }

    funDigitalWrite(STB, 1);
    return buttons;
}
