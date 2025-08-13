#include "ch32v003fun.h"

// Definir o pino do LED
#define LED PD0

int main(void)
{
    // Inicialização do sistema
    SystemInit();
    
    // Habilita o clock para o Port D
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD; 

    // Configura o pino do LED como saída push-pull de 50MHz
    funPinMode(LED, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);

    // Loop principal
    while (1)
    {
        // Liga o LED
        funDigitalWrite(LED, FUN_HIGH);
        
        // Espera por um tempo (aproximadamente 500ms)
        Delay_Ms(500); 

        // Desliga o LED
        funDigitalWrite(LED, FUN_LOW);
        
        // Espera por um tempo (aproximadamente 500ms)
        Delay_Ms(500); 
    }
}
