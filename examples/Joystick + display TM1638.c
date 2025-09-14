//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//12/08/2025
//https://www.youtube.com/@kickstech

Este programa conta com a demonstração de uma leitura analogica com Joystick e apresentação de dados no display TM1638


#include "ch32v003fun.h" // Inclui o arquivo de cabeçalho da biblioteca "ch32v003fun", que contém funções para interagir com o microcontrolador.
#include "tm1638.h"       // Inclui a biblioteca para o display e teclado TM1638.
#include "timers_utils.h"   // Inclui funções utilitárias para temporização, como Delay_Ms().
#include <stdio.h>          // Biblioteca padrão para entrada/saída, usada para formatar strings com sprintf().
#include <stdbool.h>        // Permite o uso do tipo de dado booleano (true/false).
#include <stdlib.h>         // Inclui funções utilitárias, como abs() para valor absoluto.

// --- Configuração de pinos ---
#define TM_STB_PIN PD6       // Define o pino de strobe do TM1638 como PD6.
#define TM_CLK_PIN PD5       // Define o pino de clock do TM1638 como PD5.
#define TM_DIO_PIN PD4       // Define o pino de dados do TM1638 como PD4.

// --- Pinos do Joystick ---
#define JOY_X_PIN PA1        // Define o pino do eixo X do joystick como PA1.
#define JOY_Y_PIN PA2        // Define o pino do eixo Y do joystick como PA2.
#define JOY_BTN_PIN PC0      // Define o pino do botão do joystick como PC0.

// --- Canais ADC ---
#define ADC_X_CHANNEL 1      // Mapeia o pino JOY_X_PIN (PA1) para o canal 1 do ADC.
#define ADC_Y_CHANNEL 0      // Mapeia o pino JOY_Y_PIN (PA2) para o canal 0 do ADC.

// --- Limites ---
#define THRESHOLD_CENTER 100 // Define a "zona morta" do joystick. Leituras dentro deste valor do centro são consideradas "centro".

void setup()
{
    SystemInit();            // Inicializa o sistema do microcontrolador.
    setupTimer1();           // Configura o Timer 1, usado para as funções de tempo (Delay_Ms, etc.).

    // Habilita os relógios para as portas GPIO e o ADC, que são necessários para os pinos usados.
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD;
    RCC->APB2PCENR |= RCC_APB2Periph_ADC1;

    // Configura o joystick
    funPinMode(JOY_BTN_PIN, GPIO_CNF_IN_PUPD); // Configura o pino do botão como entrada com pull-up.
    funDigitalWrite(JOY_BTN_PIN, FUN_HIGH);    // Ativa o pull-up interno, tornando o estado padrão HIGH.
    funPinMode(JOY_X_PIN, GPIO_CNF_IN_ANALOG); // Configura o pino do eixo X como entrada analógica.
    funPinMode(JOY_Y_PIN, GPIO_CNF_IN_ANALOG); // Configura o pino do eixo Y como entrada analógica.
    funAnalogInit();           // Inicializa o módulo ADC do microcontrolador.
    ADC1->RSQR3 = ADC_Y_CHANNEL; // Configuração inicial do registrador ADC.

    // Inicializa o display TM1638
    TM1638_Init(TM_STB_PIN, TM_CLK_PIN, TM_DIO_PIN); // Inicializa a comunicação com o display.
    TM1638_clearDisplay();   // Limpa o display para começar.
}

int main(void)
{
    setup();                 // Chama a função de configuração uma vez.

    while (1)                // Inicia o loop infinito do programa.
    {
        // Lê os valores dos canais analógicos e armazena nas variáveis.
        uint16_t x_value = funAnalogRead(ADC_X_CHANNEL);
        uint16_t y_value = funAnalogRead(ADC_Y_CHANNEL);
        bool button_pressed = (funDigitalRead(JOY_BTN_PIN) == FUN_LOW); // Lê o botão (estado é LOW quando pressionado).

        if (button_pressed)  // Verifica se o botão está pressionado.
        {
            TM1638_displayString("BOTAO"); // Se sim, exibe "BOTAO".
        }
        else                 // Se o botão não estiver pressionado, executa a lógica de direção.
        {
            int16_t dx = (int16_t)x_value - 512; // Calcula a diferença do valor X em relação ao centro (512).
            int16_t dy = (int16_t)y_value - 512; // Calcula a diferença do valor Y em relação ao centro (512).

            // Centro
            // Usa abs() para pegar o valor absoluto, verificando se os eixos X e Y estão dentro da zona morta.
            if (abs(dx) < THRESHOLD_CENTER && abs(dy) < THRESHOLD_CENTER)
            {
                TM1638_displayString("CENTRO"); // Se ambos estiverem, exibe "CENTRO".
            }
            else // Se não estiver no centro, determina a direção.
            {
                int angle = 0; // Variável para armazenar o ângulo.

                if (dy <= -THRESHOLD_CENTER) // Verifica o movimento para Cima
                {
                    if (dx >= THRESHOLD_CENTER)
                        angle = 45; // Cima-direita
                    else if (dx <= -THRESHOLD_CENTER)
                        angle = 315; // Cima-esquerda
                    else
                        angle = 0; // Cima puro
                }
                else if (dy >= THRESHOLD_CENTER) // Verifica o movimento para Baixo
                {
                    if (dx >= THRESHOLD_CENTER)
                        angle = 135; // Baixo-direita
                    else if (dx <= -THRESHOLD_CENTER)
                        angle = 225; // Baixo-esquerda
                    else
                        angle = 180; // Baixo puro
                }
                else // Verifica se o joystick está na horizontal.
                {
                    if (dx >= THRESHOLD_CENTER)
                        angle = 90; // Direita pura
                    else if (dx <= -THRESHOLD_CENTER)
                        angle = 270; // Esquerda pura
                    else
                        angle = 0; // Centro aproximado (caso a lógica falhe em cima ou embaixo)
                }

                char buf[6];
                sprintf(buf, "%3d", angle);     // Formata o ângulo como uma string de 3 dígitos (ex: " 90").
                TM1638_displayString(buf);      // Exibe a string com o ângulo no display.
            }
        }

        Delay_Ms(100); // Aguarda 100 milissegundos antes de repetir o loop.
    }

    return 0; // Retorna 0 ao terminar o programa (nunca é alcançado em um loop infinito).
}