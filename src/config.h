#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_ADDRESS      0x08003C00

#include "board.h"

struct configStruct
{
    uint8_t  address;
    uint16_t baudrate;
    uint8_t  invert;
    uint16_t crc;
};

extern struct configStruct config;
extern uint8_t configFlag;

void configInit(void);
void configWrite(void);

#endif
