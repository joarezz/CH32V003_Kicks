
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#include "lib_74hc165.h"

static uint8_t sr_data_pin, sr_clock_pin, sr_latch_pin;
static uint8_t shift_reg_input = 0;

void HC165_Init(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin) {
    sr_data_pin  = data_pin;
    sr_clock_pin = clock_pin;
    sr_latch_pin = latch_pin;

    funPinMode(sr_data_pin,  GPIO_Speed_In | GPIO_CNF_IN_FLOATING);
    funPinMode(sr_clock_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(sr_latch_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
}

void HC165_Update() {
    funDigitalWrite(sr_latch_pin, FUN_LOW);
    Delay_Us(1);
    funDigitalWrite(sr_latch_pin, FUN_HIGH);

    shift_reg_input = 0;
    for (int8_t i = 7; i >= 0; i--) {
        funDigitalWrite(sr_clock_pin, FUN_LOW);
        Delay_Us(1);
        shift_reg_input |= (funDigitalRead(sr_data_pin) << i);
        funDigitalWrite(sr_clock_pin, FUN_HIGH);
        Delay_Us(1);
    }
}

int DigitalRead(uint8_t pin) {
    if (pin > 7) return 0;
    return (shift_reg_input >> pin) & 0x01;
}
