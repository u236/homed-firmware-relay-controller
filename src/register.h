#ifndef REGISTER_H
#define REGISTER_H

#define REGISTER_UPTIME_MSB         0x10
#define REGISTER_UPTIME_LSB         0x11

#define REGISTER_OUTPUT_DATA        0x01
#define REGISTER_CONFIG_ADDRESS     0x20
#define REGISTER_CONFIG_BAUDRATE    0x21
#define REGISTER_CONFIG_INVERT      0x30

#define REGISTER_VALUES_BAUDRATE    {12, 24, 48, 96, 192, 384, 576, 1152}

#include "board.h"

uint8_t registerReadInput(uint16_t address, uint16_t *data, uint8_t *error);
uint8_t registerReadHolding(uint16_t address, uint16_t *data, uint8_t *error);
uint8_t registerWriteHolding(uint16_t address, uint16_t *data, uint8_t *error);
uint8_t registerWriteCoil(uint16_t address, uint16_t *data, uint8_t *error);

#endif
