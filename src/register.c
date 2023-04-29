#include "config.h"
#include "modbus.h"
#include "output.h"
#include "register.h"

static uint8_t registerCheckBaudrate(uint16_t value)
{
    uint16_t values[] = REGISTER_VALUES_BAUDRATE;

    for (uint8_t i = 0; i < sizeof(values) / sizeof(value); i++)
        if (value == values[i])
            return 1;

    return 0;
}

uint8_t registerReadInput(uint16_t address, uint16_t *data, uint8_t *error)
{
    switch (address)
    {
        case REGISTER_UPTIME_MSB:
            *data = (uint16_t) (uptime >> 16);
            return 1;

        case REGISTER_UPTIME_LSB:
            *data = (uint16_t) (uptime);
            return 1;

        default:
            *error = MODBUS_ERROR_ADDRESS;
            return 0;
    }
}

uint8_t registerReadHolding(uint16_t address, uint16_t *data, uint8_t *error)
{
    switch (address)
    {
        case REGISTER_OUTPUT_DATA:
            *data = outputData;
            return 1;

        case REGISTER_CONFIG_ADDRESS:
            *data = config.address;
            return 1;

        case REGISTER_CONFIG_BAUDRATE:
            *data = config.baudrate;
            return 1;

        case REGISTER_CONFIG_INVERT:
            *data = config.invert;
            return 1;

        default:
            *error = MODBUS_ERROR_ADDRESS;
            return 0;
    }
}

uint8_t registerWriteHolding(uint16_t address, uint16_t *data, uint8_t *error)
{
    switch (address)
    {
        case REGISTER_OUTPUT_DATA:
            outputData = *data;
            return 1;

        case REGISTER_CONFIG_ADDRESS:

            if (*data < 1 || *data > MODBUS_MAX_ADDRESS)
            {
                *error = MODBUS_ERROR_VALUE;
                return 0;
            }

            if (config.address != *data)
            {
                config.address = (uint8_t) *data;
                configFlag = 1;
            }

            return 1;

        case REGISTER_CONFIG_BAUDRATE:

            if (!registerCheckBaudrate(*data))
            {
                *error = MODBUS_ERROR_VALUE;
                return 0;
            }

            if (config.baudrate != *data)
            {
                config.baudrate = *data;
                configFlag = 1;
            }

            return 1;

        case REGISTER_CONFIG_INVERT:

            if (*data > 1)
            {
                *error = MODBUS_ERROR_VALUE;
                return 0;
            }

            if (config.invert != *data)
            {
                config.invert = (uint8_t) *data;
                configFlag = 1;
            }

            return 1;

        default:
            *error = MODBUS_ERROR_ADDRESS;
            return 0;
    }
}

uint8_t registerWriteCoil(uint16_t address, uint16_t *data, uint8_t *error)
{
    (void) address;
    (void) *data;
    *error = MODBUS_ERROR_ADDRESS;
    return 0;
}
