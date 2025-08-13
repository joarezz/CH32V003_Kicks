//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://www.youtube.com/@kickstech

#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SENSORS 10
#define MAX_FAILURES 3

typedef struct {
    uint8_t address[8];
    int16_t last_valid_temp;
    uint8_t failure_count;
} ds18b20_sensor_t;

int16_t ds18b20_read_temperature(uint8_t pin, uint8_t *addr);

void ds18b20_init(uint8_t pin);
void ds18b20_find_devices();
void ds18b20_set_all_sensors_resolution(); // <-- Nova função
void ds18b20_update_temperatures();

uint8_t ds18b20_get_sensor_count();
ds18b20_sensor_t ds18b20_get_sensor_status(uint8_t index);


#endif
