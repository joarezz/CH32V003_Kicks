// timers_utils.c
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA


#include "timers_utils.h" // Inclua o próprio cabeçalho
#include "ch32v003fun.h"   // Para RCC, TIM1, NVIC_EnableIRQ etc.

// --- Definição e Inicialização da Variável SystemCoreClock ---
// DEFINIÇÃO: A variável é criada e inicializada neste arquivo.
// ESTA É A ÚNICA VEZ QUE ELA É INICIALIZADA NO PROJETO.
volatile unsigned int SystemCoreClock = 48000000; // 48 MHz para CH32V003

// --- Variáveis Globais (definidas aqui para uso interno e 'extern' em .h) ---
volatile unsigned long millis_counter = 0;
volatile int lcd_update_flag = 0;
volatile int lcd_message_state = 0;

// ... (restante das funções setupTimer1, TIM1_UP_IRQHandler, millis, micros) ...

void setupTimer1() {
    // Habilita o clock do Timer1
    RCC->APB2PCENR |= RCC_APB2Periph_TIM1;

    // Configura Timer1 para 1 milissegundo de interrupção
    // Clock do APB2 (TIM1) é SystemCoreClock (48MHz por padrão no CH32V003fun)
    
    // Prescaler para que o timer conte a cada 1 microssegundo: PSC = (SystemCoreClock / 1000000) - 1
    // Se SystemCoreClock = 48MHz, então PSC = (48000000 / 1000000) - 1 = 48 - 1 = 47
    TIM1->PSC = (SystemCoreClock / 1000000) - 1; // Contador do timer incrementa a cada microssegundo

    // O Auto-Reload Register (ATRLR) define o valor máximo que o contador atingirá antes de resetar e gerar uma interrupção.
    // Para uma interrupção a cada 1ms, o contador deve atingir 1000 (ticks de microssegundos).
    TIM1->ATRLR = 1000 - 1; // Interrupção a cada 1 milissegundo

    // Clear UEV flag
    TIM1->INTFR = 0;
    // Habilita a interrupção de update (UEV)
    TIM1->CTLR1 |= TIM_CEN;     // Habilita o contador
    TIM1->DMAINTENR |= TIM_UIE; // Habilita a interrupção de atualização

    // Habilita a interrupção no NVIC
    NVIC_EnableIRQ(TIM1_UP_IRQn);
}

// ISR para Timer1 (Update Event)
void TIM1_UP_IRQHandler(void) __attribute__((interrupt));
void TIM1_UP_IRQHandler(void) {
    if (TIM1->INTFR & TIM_UIF) { // Verifica se a interrupção de atualização ocorreu
        TIM1->INTFR = (uint16_t)~TIM_UIF; // Limpa a flag de interrupção

        millis_counter++; // Incrementa o contador de milissegundos
        
        // Define uma flag para atualização do LCD a cada X milissegundos (ex: 100ms)
        if (millis_counter % 100 == 0) { // A cada 100ms
            lcd_update_flag = 1;
        }
        
        // Lógica para alternar mensagens do LCD a cada Y milissegundos (ex: 5 segundos)
        if (millis_counter % 5000 == 0) { // A cada 5 segundos
            lcd_message_state++;
            if (lcd_message_state > 1) { // Temos 2 estados para as linhas 0 e 1 (0 e 1)
                lcd_message_state = 0;
            }
            lcd_update_flag = 1; // Força atualização do LCD quando a mensagem muda
        }
    }
}

// Função similar ao millis() do Arduino
unsigned long millis() {
    unsigned long m;
    // Desabilita interrupções brevemente para garantir leitura atômica da variável volátil
    __disable_irq(); // Desabilita interrupções
    m = millis_counter;
    __enable_irq();  // Habilita interrupções
    return m;
}

// Função similar ao micros() do Arduino (aproximada)
unsigned long micros() {
    unsigned long m;
    unsigned long t;
    // Desabilita interrupções brevemente
    __disable_irq(); // Desabilita interrupções
    m = millis_counter;
    t = TIM1->CNT; // Lê o valor atual do contador do timer
    __enable_irq();  // Habilita interrupções

    // Esta é uma aproximação. Para alta precisão em microssegundos,
    // seria necessário considerar o transbordo do timer entre a leitura de m e t.
    // Para a maioria das aplicações, essa aproximação é suficiente.
    return (m * 1000) + t; // Combina milissegundos com os microssegundos parciais do ciclo atual
}
