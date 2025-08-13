#include "ch32v003fun.h"

#define LED PD0
#define BOTAO PD2

int main(void)
{
    SystemInit();
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD; // HABILITA O CLOCK PARA O PORT D

    funPinMode(LED, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funPinMode(BOTAO, GPIO_CNF_IN_PUPD);
    funDigitalWrite(BOTAO, FUN_HIGH);

    while (1)
    {
        if (funDigitalRead(BOTAO) == FUN_LOW)
        {
            funDigitalWrite(LED, FUN_HIGH);
        }
        else
        {

            funDigitalWrite(LED, FUN_LOW);
        }
    }
}