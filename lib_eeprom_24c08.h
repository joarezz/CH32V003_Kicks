
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef LIB_EEPROM_24C08_H
#define LIB_EEPROM_24C08_H

#include <stdint.h>
#include "lib_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EEPROM_24C08_ADDR  0x50

// Escreve 1 byte no endereço de memória
i2c_err_t eeprom24c08_write_byte(uint8_t mem_addr, uint8_t data);

// Lê 1 byte do endereço de memória
i2c_err_t eeprom24c08_read_byte(uint8_t mem_addr, uint8_t *data);

// Escreve até 8 bytes a partir de um endereço
i2c_err_t eeprom24c08_write_bytes(uint8_t mem_addr, const uint8_t *data, uint8_t len);

// Lê múltiplos bytes a partir de um endereço
i2c_err_t eeprom24c08_read_bytes(uint8_t mem_addr, uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif // LIB_EEPROM_24C08_H
