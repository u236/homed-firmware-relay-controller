#include "config.h"
#include "crc.h"

struct configStruct config;
uint8_t configFlag = 0;

void configInit(void)
{
    memcpy(&config, (void*) CONFIG_ADDRESS, sizeof(config));

    if (!crcCheck((uint8_t*) &config, sizeof(config)))
    {
        config.baudrate = 1152;
        config.address = 1;
        config.invert = 0;
    }
}

void configWrite(void)
{
    crcAppend((uint8_t*) &config, sizeof(config));

    FLASH_Unlock();
    FLASH_ErasePage(CONFIG_ADDRESS);

    for (uint8_t i = 0; i < sizeof(config) / 2; i++)
        FLASH_ProgramHalfWord(CONFIG_ADDRESS + i * 2, *((uint16_t*) &config + i));

    FLASH_Lock();
}
