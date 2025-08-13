#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "ch32v003fun.h"
#include <stdint.h>
#include <stdbool.h>

void OneWire_Init(uint8_t pin);
uint8_t OneWire_Reset(uint8_t pin);
void OneWire_WriteByte(uint8_t pin, uint8_t byte);
uint8_t OneWire_ReadByte(uint8_t pin);
uint8_t OneWire_CRC8(const uint8_t *data, uint8_t len);

void OneWire_Search_Start();
bool OneWire_Search_Next(uint8_t pin, uint8_t *newAddr);

#endif