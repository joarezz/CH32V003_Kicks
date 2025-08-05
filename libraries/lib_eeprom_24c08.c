
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

// lib_eeprom_24c08.c
#include "lib_eeprom_24c08.h"
#include "timers_utils.h"

#define EEPROM_WRITE_DELAY_MS 10

i2c_err_t eeprom24c08_write_byte(uint8_t mem_addr, uint8_t data) {
    uint8_t buf[2] = {mem_addr, data};
    i2c_err_t err = i2c_write_bytes(EEPROM_24C08_ADDR, buf, 2);
    Delay_Ms(EEPROM_WRITE_DELAY_MS);
    return err;
}

i2c_err_t eeprom24c08_read_byte(uint8_t mem_addr, uint8_t *data) {
    i2c_err_t err;
    err = i2c_start();
    if (err != I2C_OK) return err;
    err = i2c_address_send(EEPROM_24C08_ADDR, I2C_TX);
    if (err != I2C_OK) return err;
    err = i2c_write_byte(mem_addr);
    if (err != I2C_OK) return err;
    err = i2c_start();
    if (err != I2C_OK) return err;
    err = i2c_address_send(EEPROM_24C08_ADDR, I2C_RX);
    if (err != I2C_OK) return err;
    return i2c_read_byte(I2C_NACK, data);
}

i2c_err_t eeprom24c08_write_bytes(uint8_t mem_addr, const uint8_t *data, uint8_t len) {
    if (len > 8) return I2C_ERR_TX_BYTE; // 24C08 page limit

    uint8_t buf[9];
    buf[0] = mem_addr;
    for (uint8_t i = 0; i < len; i++) {
        buf[i + 1] = data[i];
    }

    i2c_err_t err = i2c_write_bytes(EEPROM_24C08_ADDR, buf, len + 1);
    Delay_Ms(EEPROM_WRITE_DELAY_MS);
    return err;
}

i2c_err_t eeprom24c08_read_bytes(uint8_t mem_addr, uint8_t *data, uint8_t len) {
    i2c_err_t err;
    err = i2c_start();
    if (err != I2C_OK) return err;
    err = i2c_address_send(EEPROM_24C08_ADDR, I2C_TX);
    if (err != I2C_OK) return err;
    err = i2c_write_byte(mem_addr);
    if (err != I2C_OK) return err;
    err = i2c_start();
    if (err != I2C_OK) return err;
    err = i2c_address_send(EEPROM_24C08_ADDR, I2C_RX);
    if (err != I2C_OK) return err;
    for (uint8_t i = 0; i < len; i++) {
        uint8_t ack = (i == len - 1) ? I2C_NACK : I2C_ACK;
        err = i2c_read_byte(ack, &data[i]);
        if (err != I2C_OK) return err;
    }
    return I2C_OK;
}
