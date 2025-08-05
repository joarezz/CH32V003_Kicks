
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#include "ds3231_rtc.h"
#include "lib_i2c.h"     // Inclua a nova biblioteca I2C aqui
#include "ch32v003fun.h" // Para Delay_Ms
#include <stdlib.h>      // Para strtol, se necessário, mas não diretamente neste arquivo
#include <stdint.h>      // Para int8_t


//#include "serial_uart.h" // Para Serial_print (mantenha se estiver usando para debug)

// A instância ds3231_dev baseada em i2c_device_t foi removida
// pois o novo lib_i2c.h não usa mais essa estrutura para o endereço.
// O endereço DS3231_ADDRESS agora é passado diretamente para as funções I2C.

// Helper para converter BCD para Decimal
static uint8_t bcdToDec(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

// Helper para converter Decimal para BCD
static uint8_t decToBcd(uint8_t dec) {
    return ((dec / 10) << 4) + (dec % 10);
}

// Protótipos das funções I2C de baixo nível (mantidas estáticas e internas)
// Adaptadas para a nova API da lib_i2c

// Escreve um byte para um registrador específico do DS3231
static uint8_t ds3231_write_reg(uint8_t reg_addr, uint8_t data) {
    uint8_t write_buffer[2];
    write_buffer[0] = reg_addr;
    write_buffer[1] = data;
    // Usa i2c_write_bytes para enviar o endereço do registrador e os dados
    if (i2c_write_bytes(DS3231_ADDRESS, write_buffer, 2) == I2C_OK) {
        return 0; // Sucesso
    }
    return 1; // Erro
}

// Lê um byte de um registrador específico do DS3231
static uint8_t ds3231_read_reg(uint8_t reg_addr, uint8_t *data) {
    // Primeiro, envia o endereço do registrador para o qual se deseja ler
    if (i2c_write_bytes(DS3231_ADDRESS, &reg_addr, 1) != I2C_OK) {
        return 1; // Erro ao enviar o endereço do registrador
    }

    // Em seguida, lê os dados do registrador
    if (i2c_read_bytes(DS3231_ADDRESS, data, 1) != I2C_OK) {
        return 1; // Erro ao ler os dados
    }
    return 0; // Sucesso
}

// Inicializa o DS3231 (nenhuma inicialização de hardware I2C aqui, apenas lógica do chip)
void ds3231_init() {
    // Não é necessário inicializar o I2C aqui, pois o main.c já faz isso.
    // Pode-se adicionar aqui qualquer configuração específica do DS3231, se necessário.
}

// Define a hora e data no DS3231
uint8_t ds3231_set_time(const DateTime_t *time) {
    uint8_t status = 0;
    status |= ds3231_write_reg(DS3231_REG_SECONDS, decToBcd(time->seconds));
    status |= ds3231_write_reg(DS3231_REG_MINUTES, decToBcd(time->minutes));
    // As horas precisam de tratamento especial para 12/24h e bit AM/PM se usado.
    // Aqui assumimos formato 24h (bit 6 = 0).
    status |= ds3231_write_reg(DS3231_REG_HOURS, decToBcd(time->hours));
    status |= ds3231_write_reg(DS3231_REG_DAY_OF_WEEK, time->weekDay); // Weekday não é BCD
    status |= ds3231_write_reg(DS3231_REG_DAY, decToBcd(time->date));
    // O bit 7 do mês é o bit de século (Century Enable).
    // Se o ano for 20xx, o bit é 0. Se for 21xx, o bit é 1.
    // Por simplicidade, assumimos 20xx (bit 7 = 0).
    status |= ds3231_write_reg(DS3231_REG_MONTH, decToBcd(time->month));
    status |= ds3231_write_reg(DS3231_REG_YEAR, decToBcd(time->year)); // Apenas os 2 últimos dígitos

    return status; // Retorna 0 se todas as escritas foram bem-sucedidas
}

// Obtém a hora e data do DS3231 lendo registrador por registrador
// Esta é a função correta para a sua lib_i2c
uint8_t ds3231_get_time(DateTime_t *time) {
    uint8_t status = 0;
    uint8_t data;

    status |= ds3231_read_reg(DS3231_REG_SECONDS, &data);
    time->seconds = bcdToDec(data & 0x7F);

    status |= ds3231_read_reg(DS3231_REG_MINUTES, &data);
    time->minutes = bcdToDec(data & 0x7F);

    status |= ds3231_read_reg(DS3231_REG_HOURS, &data);
    time->hours   = bcdToDec(data & 0x3F); // Ignora bits de formato AM/PM e 12/24

    status |= ds3231_read_reg(DS3231_REG_DAY_OF_WEEK, &data);
    time->weekDay = data & 0x07;

    status |= ds3231_read_reg(DS3231_REG_DAY, &data);
    time->date    = bcdToDec(data & 0x3F);

    status |= ds3231_read_reg(DS3231_REG_MONTH, &data);
    time->month   = bcdToDec(data & 0x1F); // Ignora bit Century

    status |= ds3231_read_reg(DS3231_REG_YEAR, &data);
    time->year    = bcdToDec(data);

    return status;
}

// Obtém a temperatura do DS3231
float ds3231_get_temp() {
    uint8_t temp_msb_raw, temp_lsb_raw;
    float temperature = 0.0;

    // Lê o MSB da temperatura
    if (ds3231_read_reg(DS3231_REG_TEMP_MSB, &temp_msb_raw) != 0) {
        //Serial_print("Erro ao ler MSB da temperatura do DS3231.\r\n");
        return -999.0; // Retorna um valor de erro
    }

    // Lê o LSB da temperatura
    if (ds3231_read_reg(DS3231_REG_TEMP_LSB, &temp_lsb_raw) != 0) {
        //Serial_print("Erro ao ler LSB da temperatura do DS3231.\r\n");
        return -999.0; // Retorna um valor de erro
    }

    // O MSB contém a parte inteira (em complemento de dois)
    // Os dois bits mais significativos do LSB (bits 7 e 6) representam 0.25, 0.50, 0.75
    // temp_lsb_raw: BITS 7-6 | BITS 5-0 (sempre 0 para o DS3231 na temperatura)
    // Ex: 0x00 = 0.00, 0x40 = 0.25, 0x80 = 0.50, 0xC0 = 0.75

    // Converte o MSB para float, tratando como complemento de dois
    // Se o bit mais significativo (MSB) do temp_msb_raw for 1, é um número negativo.
    // Usamos int8_t para garantir a interpretação correta de números negativos em complemento de dois.
    temperature = (float)((int8_t)temp_msb_raw);

    // Adiciona a parte fracionária do LSB
    // Os bits 7 e 6 do LSB representam:
    // 00 -> 0.00
    // 01 -> 0.25
    // 10 -> 0.50
    // 11 -> 0.75
    uint8_t fractional_bits = (temp_lsb_raw >> 6); // Pega apenas os bits 7 e 6

    if (fractional_bits == 1) { // 01
        temperature += 0.25f;
    } else if (fractional_bits == 2) { // 10
        temperature += 0.50f;
    } else if (fractional_bits == 3) { // 11
        temperature += 0.75f;
    }
    // Se fractional_bits == 0 (00), não adiciona nada (temperatura += 0.00)

    return temperature;
}
