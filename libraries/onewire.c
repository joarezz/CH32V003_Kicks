//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://www.youtube.com/@kickstech

#include "onewire.h"
#include "ch32v003fun.h"
#include <stdint.h>
#include <stdbool.h>

static uint8_t search_last_discrepancy;
static uint8_t search_last_device_flag;
static uint8_t ROM_NO[8];

static void ow_drive_low(uint8_t pin) {
    funPinMode(pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP);
    funDigitalWrite(pin, 0);
}

static void ow_release(uint8_t pin) {
    funPinMode(pin, GPIO_CNF_IN_PUPD);
}

void OneWire_Init(uint8_t pin) {
    ow_release(pin);
}

uint8_t OneWire_Reset(uint8_t pin) {
    uint8_t presence = 0;
    
    ow_drive_low(pin);
    Delay_Us(480);
    
    ow_release(pin);
    
    Delay_Us(70);
    presence = !funDigitalRead(pin);
    
    Delay_Us(410);
    
    return presence;
}

static void OneWire_WriteBit(uint8_t pin, uint8_t bit) {
    if (bit & 0x01) {
        ow_drive_low(pin);
        Delay_Us(6);
        ow_release(pin);
        Delay_Us(64);
    } else {
        ow_drive_low(pin);
        Delay_Us(60);
        ow_release(pin);
        Delay_Us(10);
    }
}

static uint8_t OneWire_ReadBit(uint8_t pin) {
    uint8_t bit = 0;
    
    ow_drive_low(pin);
    Delay_Us(2);
    
    ow_release(pin);
    
    Delay_Us(13);
    bit = funDigitalRead(pin);
    
    Delay_Us(45);
    
    return bit;
}

void OneWire_WriteByte(uint8_t pin, uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        OneWire_WriteBit(pin, byte & 0x01);
        byte >>= 1;
    }
}

uint8_t OneWire_ReadByte(uint8_t pin) {
    uint8_t val = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (OneWire_ReadBit(pin)) val |= (1 << i);
    }
    return val;
}

uint8_t OneWire_CRC8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        uint8_t inbyte = data[i];
        for (uint8_t j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

void OneWire_Search_Start() {
    search_last_discrepancy = 0;
    search_last_device_flag = 0;
    for(uint8_t i = 0; i < 8; i++) {
        ROM_NO[i] = 0;
    }
}

bool OneWire_Search_Next(uint8_t pin, uint8_t *newAddr) {
    uint8_t i;
    uint8_t search_direction;
    uint8_t discrepancy_marker = 0;
    
    if (search_last_device_flag) {
        return false;
    }
    
    if (OneWire_Reset(pin) == 0) {
        OneWire_Search_Start();
        return false;
    }
    
    OneWire_WriteByte(pin, 0xF0); // Comando de busca de ROM
    
    for (i = 0; i < 64; i++) {
        uint8_t a = OneWire_ReadBit(pin);
        uint8_t b = OneWire_ReadBit(pin);
        
        if (a == 1 && b == 1) {
            OneWire_Search_Start();
            return false;
        } else {
            if (a == 0 && b == 0) {
                if (i < search_last_discrepancy) {
                    search_direction = (ROM_NO[i / 8] >> (i % 8)) & 0x01;
                } else {
                    search_direction = (i == search_last_discrepancy);
                }
                if (search_direction == 0) {
                    discrepancy_marker = i;
                }
            } else {
                search_direction = a;
            }
            if (search_direction == 1) {
                ROM_NO[i / 8] |= (1 << (i % 8));
            } else {
                ROM_NO[i / 8] &= ~(1 << (i % 8));
            }
            OneWire_WriteBit(pin, search_direction);
        }
    }
    
    search_last_discrepancy = discrepancy_marker;
    if (search_last_discrepancy == 0) {
        search_last_device_flag = 1;
    }
    
    for (i = 0; i < 8; i++) {
        newAddr[i] = ROM_NO[i];
    }
    
    return true;

}
