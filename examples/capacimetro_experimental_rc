//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//12/08/2025
//https://www.youtube.com/@kickstech

#include "ch32v003fun.h"
#include "tm1638.h"
#include "timers_utils.h"
#include <stdio.h>
#include <stdbool.h>

// --- Configuração de pinos ---
#define TM_STB_PIN PD6
#define TM_CLK_PIN PD5
#define TM_DIO_PIN PD4

#define CAP_CHARGE_PIN PC0  
#define CAP_READ_PIN PA1
#define CAP_READ_ADC_CHANNEL 1 // PA1 -> canal 1 do ADC

#define BUTTON_PIN PC2

// --- Constantes ---
#define R_TEST 2200.0f
#define LN_3 1.09861228867f
#define ADC_THRESHOLD_HIGH 930
#define ADC_THRESHOLD_LOW  310

// --- Calibração ---
float calib_factor = 1.0f; // fator multiplicador

// --- Funções auxiliares ---
void charging_animation(int step) {
    char animation_frames[4][9] = {
        "        ",
        "       -",
        "      - ",
        "     -  "
    };
    TM1638_displayString(animation_frames[step % 4]);
}

void wait_button_release() {
    while(funDigitalRead(BUTTON_PIN) == FUN_LOW);
}

bool button_long_press_detected(unsigned long hold_time_ms) {
    if (funDigitalRead(BUTTON_PIN) == FUN_LOW) {
        unsigned long start = millis();
        while (funDigitalRead(BUTTON_PIN) == FUN_LOW) {
            if ((millis() - start) >= hold_time_ms) {
                wait_button_release();
                return true;
            }
        }
    }
    return false;
}

void wait_button_press_no_delay() {
    while(funDigitalRead(BUTTON_PIN) == FUN_HIGH);
    wait_button_release();
}

float measure_capacitance() {
    // Carrega capacitor
    
    funDigitalWrite(CAP_CHARGE_PIN, FUN_HIGH);
    
    uint16_t adc_value = 0;
    int animation_step = 0;
    unsigned long last_anim = millis();
    while (adc_value < ADC_THRESHOLD_HIGH) {
        adc_value = funAnalogRead(CAP_READ_ADC_CHANNEL);
        if (millis() - last_anim >= 100) {
            charging_animation(animation_step++);
            last_anim = millis();
        }
    }

    // Descarga e mede tempo
    
    funDigitalWrite(CAP_CHARGE_PIN, FUN_LOW);

    unsigned long start_time_discharge = micros();
    while (funAnalogRead(CAP_READ_ADC_CHANNEL) > ADC_THRESHOLD_LOW);
    unsigned long end_time_discharge = micros();

    unsigned long time_taken_us = end_time_discharge - start_time_discharge;
    if (time_taken_us == 0) return 0.0f;

    float time_s = (float)time_taken_us / 1000000.0f;
    float capacitance_f = time_s / (R_TEST * LN_3); // Formula resumida
    return capacitance_f * calib_factor;
}

void calibrate_capacitance() {
    TM1638_displayString("100uF CAP");
    wait_button_press_no_delay();

    float measured = measure_capacitance();
    if (measured > 0.0f) {
        calib_factor = (100e-6f) / measured;
        TM1638_displayString("OK");
        unsigned long start = millis();
        while (millis() - start < 1000);
    } else {
        TM1638_displayString("ERR");
        unsigned long start = millis();
        while (millis() - start < 1000);
    }
}

void display_capacitance(float capacitance_f) {
    char display_buffer[9];
    if (capacitance_f < 1e-9f) {
        uint32_t pf = (uint32_t)(capacitance_f * 1e12f);
        snprintf(display_buffer, sizeof(display_buffer), "%lu pF", pf);
    } else if (capacitance_f < 1e-6f) {
        uint32_t nf = (uint32_t)(capacitance_f * 1e9f);
        snprintf(display_buffer, sizeof(display_buffer), "%lu nF", nf);
    } else {
        uint32_t uf = (uint32_t)(capacitance_f * 1e6f);
        snprintf(display_buffer, sizeof(display_buffer), "%lu uF", uf);
    }
    TM1638_displayString(display_buffer);
}

void setup() {
    SystemInit();
    setupTimer1();

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD;
    RCC->APB2PCENR |= RCC_APB2Periph_ADC1;
    funPinMode(CAP_CHARGE_PIN, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);

    funPinMode(BUTTON_PIN, GPIO_CNF_IN_PUPD);
    funDigitalWrite(BUTTON_PIN, FUN_HIGH);
    funPinMode(CAP_READ_PIN, GPIO_CNF_IN_ANALOG);
    funAnalogInit();

    TM1638_Init(TM_STB_PIN, TM_CLK_PIN, TM_DIO_PIN);
    TM1638_clearDisplay();
}

int main(void) {
    setup();

    while (1) {
        TM1638_displayString("PRESS BT");

        // 1. Espera o botão ser pressionado (estado LOW)
        while (funDigitalRead(BUTTON_PIN) == FUN_HIGH);

        // 2. Atraso para ignorar repiques do botão (debounce)
        // Espera 20ms e verifica se o botão ainda está pressionado
        Delay_Ms(20);
        if (funDigitalRead(BUTTON_PIN) == FUN_HIGH) {
            // Se o botão não estiver mais pressionado, foi um repique.
            // Volta para o início do loop.
            continue;
        }

        // 3. Inicia a contagem de tempo
        unsigned long press_start = millis();

        // 4. Monitora o tempo enquanto o botão estiver pressionado
        while (funDigitalRead(BUTTON_PIN) == FUN_LOW);

        // 5. Calcula a duração do clique e faz um novo debounce na soltura
        unsigned long press_time = millis() - press_start;
        Delay_Ms(20);

        // 6. Avalia a duração do clique para decidir a ação
        if (press_time >= 1500) {
            // Long click (tempo de pressão >= 1500 ms) -> calibração
            calibrate_capacitance();
        } else {
            // Short click (tempo de pressão < 1500 ms) -> medição
            TM1638_displayString("MEDINDO");
            float cap = measure_capacitance();
            
            if (cap <= 0.0f) {
                TM1638_displayString("ERR");
            } else {
                display_capacitance(cap);
            }
            
            // Espera 5 segundos para exibir o resultado da medição
            unsigned long start = millis();
            while (millis() - start < 1000);
        }
    }

    return 0;
}
