
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef DS3231_RTC_H
#define DS3231_RTC_H

#include <stdint.h>
#include "ch32v003fun.h" // Para usar Delay_Ms e outras funções básicas do ch32v003fun
#include "lib_i2c.h"    // Inclui o cabeçalho da nova biblioteca I2C

// Endereço I2C do DS3231 (7-bit)
#define DS3231_ADDRESS 0x68

// Registradores do DS3231
#define DS3231_REG_SECONDS      0x00
#define DS3231_REG_MINUTES      0x01
#define DS3231_REG_HOURS        0x02
#define DS3231_REG_DAY_OF_WEEK  0x03
#define DS3231_REG_DAY          0x04
#define DS3231_REG_MONTH        0x05
#define DS3231_REG_YEAR         0x06

#define DS3231_REG_ALARM1_SECONDS 0x07
#define DS3231_REG_ALARM1_MINUTES 0x08
#define DS3231_REG_ALARM1_HOURS 0x09
#define DS3231_REG_ALARM1_DAY_DATE 0x0A

#define DS3231_REG_ALARM2_MINUTES 0x0B
#define DS3231_REG_ALARM2_HOURS 0x0C
#define DS3231_REG_ALARM2_DAY_DATE 0x0D

#define DS3231_REG_CONTROL      0x0E
#define DS3231_REG_STATUS       0x0F
#define DS3231_REG_AGING_OFFSET 0x10
#define DS3231_REG_TEMP_MSB     0x11 // MSB da Temperatura
#define DS3231_REG_TEMP_LSB     0x12 // LSB da Temperatura

// Estrutura para armazenar data e hora
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t weekDay; // 1 = Domingo, 2 = Segunda, ..., 7 = Sábado
    uint8_t date;
    uint8_t month;
    uint8_t year;    // Apenas os 2 últimos dígitos do ano (ex: 24 para 2024)
} DateTime_t;

// Funções públicas
void ds3231_init();
uint8_t ds3231_set_time(const DateTime_t *time);
uint8_t ds3231_get_time(DateTime_t *time);
float ds3231_get_temp();

#endif // DS3231_RTC_H
