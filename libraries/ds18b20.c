#include "ds18b20.h"
#include "onewire.h"
#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define CMD_CONVERT_T 0x44
#define CMD_READ_SCRATCHPAD 0xBE
#define CMD_WRITE_SCRATCHPAD 0x4E // Comando para escrever no scratchpad
#define CMD_COPY_SCRATCHPAD 0x48 // Comando para copiar o scratchpad para a EEPROM
#define CMD_SELECT_ROM 0x55

static uint8_t ds18b20_pin;
static uint8_t num_sensors = 0;
static ds18b20_sensor_t sensors[MAX_SENSORS];

// Retorna a temperatura lida em 16ths de grau Celsius ou um valor de erro
static int16_t ds18b20_single_read_raw(uint8_t pin, uint8_t *addr) {
    uint8_t scratchpad[9];
    int16_t raw_temp = 9999; // Valor de erro

    if (OneWire_Reset(pin)) {
        OneWire_WriteByte(pin, CMD_SELECT_ROM);
        for (int i = 0; i < 8; i++) {
            OneWire_WriteByte(pin, addr[i]);
        }
        
        OneWire_WriteByte(pin, CMD_CONVERT_T);
        
        Delay_Ms(750);
        
        if (OneWire_Reset(pin)) {
            OneWire_WriteByte(pin, CMD_SELECT_ROM);
            for (int i = 0; i < 8; i++) {
                OneWire_WriteByte(pin, addr[i]);
            }
            OneWire_WriteByte(pin, CMD_READ_SCRATCHPAD);
            
            for (int i = 0; i < 9; i++) {
                scratchpad[i] = OneWire_ReadByte(pin);
            }
            
            if (OneWire_CRC8(scratchpad, 8) == scratchpad[8]) {
                raw_temp = (scratchpad[1] << 8) | scratchpad[0];
            }
        }
    }
    return raw_temp;
}

int16_t ds18b20_read_temperature(uint8_t pin, uint8_t *addr) {
    int16_t temp = 9999;
    uint8_t retries = 0;
    while(retries < 3) {
        temp = ds18b20_single_read_raw(pin, addr);
        if (temp != 9999) {
            return temp; // Leitura bem-sucedida
        }
        Delay_Ms(100);
        retries++;
    }
    return temp; // Retorna o erro após 3 tentativas
}

void ds18b20_init(uint8_t pin) {
    ds18b20_pin = pin;
    OneWire_Init(ds18b20_pin);
}

void ds18b20_find_devices() {
    uint8_t new_addrs[MAX_SENSORS][8];
    uint8_t new_num_found = 0;
    uint8_t addr[8];
    
    OneWire_Search_Start();
    while(OneWire_Search_Next(ds18b20_pin, addr) && new_num_found < MAX_SENSORS) {
        if (OneWire_CRC8(addr, 7) == addr[7]) {
            memcpy(new_addrs[new_num_found], addr, 8);
            new_num_found++;
        }
    }

    uint8_t temp_sensors_count = 0;
    ds18b20_sensor_t temp_sensors[MAX_SENSORS];
    
    for (uint8_t i = 0; i < new_num_found; i++) {
        bool found_existing = false;
        for (uint8_t j = 0; j < num_sensors; j++) {
            if (memcmp(sensors[j].address, new_addrs[i], 8) == 0) {
                memcpy(&temp_sensors[temp_sensors_count], &sensors[j], sizeof(ds18b20_sensor_t));
                temp_sensors[temp_sensors_count].failure_count = 0;
                found_existing = true;
                break;
            }
        }
        if (!found_existing) {
            memcpy(temp_sensors[temp_sensors_count].address, new_addrs[i], 8);
            temp_sensors[temp_sensors_count].last_valid_temp = 9999;
            temp_sensors[temp_sensors_count].failure_count = 0;
        }
        temp_sensors_count++;
    }

    for (uint8_t i = 0; i < num_sensors; i++) {
        bool still_present = false;
        for (uint8_t j = 0; j < new_num_found; j++) {
            if (memcmp(sensors[i].address, new_addrs[j], 8) == 0) {
                still_present = true;
                break;
            }
        }
        if (!still_present) {
            if (sensors[i].failure_count < MAX_FAILURES) {
                memcpy(&temp_sensors[temp_sensors_count], &sensors[i], sizeof(ds18b20_sensor_t));
                temp_sensors[temp_sensors_count].failure_count++;
                temp_sensors_count++;
            }
        }
    }

    num_sensors = temp_sensors_count;
    memcpy(sensors, temp_sensors, num_sensors * sizeof(ds18b20_sensor_t));
}

void ds18b20_set_all_sensors_resolution() {
    uint8_t resolution_byte = 0x7F; // 12-bit resolution

    for (uint8_t i = 0; i < num_sensors; i++) {
        if (OneWire_Reset(ds18b20_pin)) continue;
        
        // Seleciona o sensor pelo seu endereço
        OneWire_WriteByte(ds18b20_pin, CMD_SELECT_ROM);
        for (int j = 0; j < 8; j++) {
            OneWire_WriteByte(ds18b20_pin, sensors[i].address[j]);
        }
        
        // Escreve a configuração de resolução no scratchpad (bytes 2 e 3)
        OneWire_WriteByte(ds18b20_pin, CMD_WRITE_SCRATCHPAD);
        OneWire_WriteByte(ds18b20_pin, 0); // Th
        OneWire_WriteByte(ds18b20_pin, 0); // Tl
        OneWire_WriteByte(ds18b20_pin, resolution_byte); // Configura para 12 bits
        
        // Copia o scratchpad para a EEPROM para salvar permanentemente
        if (OneWire_Reset(ds18b20_pin)) continue;
        OneWire_WriteByte(ds18b20_pin, CMD_SELECT_ROM);
        for (int j = 0; j < 8; j++) {
            OneWire_WriteByte(ds18b20_pin, sensors[i].address[j]);
        }
        OneWire_WriteByte(ds18b20_pin, CMD_COPY_SCRATCHPAD);
        
        Delay_Ms(10); // A espera é necessária para a escrita na EEPROM
    }
}

void ds18b20_update_temperatures() {
    if (num_sensors == 0) {
        return;
    }
    
    for (uint8_t i = 0; i < num_sensors; i++) {
        if (sensors[i].failure_count >= MAX_FAILURES) continue;
        
        int16_t tempC = ds18b20_read_temperature(ds18b20_pin, sensors[i].address);
        
        if (tempC == 9999) {
            sensors[i].failure_count++;
        } else {
            sensors[i].last_valid_temp = tempC;
            sensors[i].failure_count = 0;
        }
    }
}

uint8_t ds18b20_get_sensor_count() {
    return num_sensors;
}

ds18b20_sensor_t ds18b20_get_sensor_status(uint8_t index) {
    if (index < num_sensors) {
        return sensors[index];
    }
    ds18b20_sensor_t invalid_sensor;
    memset(&invalid_sensor, 0, sizeof(ds18b20_sensor_t));
    invalid_sensor.failure_count = MAX_FAILURES; 
    return invalid_sensor;
}