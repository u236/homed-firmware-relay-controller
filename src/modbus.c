#include "config.h"
#include "crc.h"
#include "modbus.h"
#include "register.h"

static void modbusTransmit(uint8_t *data, uint8_t length)
{
    crcAppend(data, length);
    GPIO_SetBits(DR_GPIO, DR_PIN);

    for (uint8_t i = 0; i < length; i++)
    {
        while (USART_GetFlagStatus(UART_PORT, USART_FLAG_TXE) == RESET);
        USART_SendData(UART_PORT, data[i]);
    }

    while (USART_GetFlagStatus(UART_PORT, USART_FLAG_TC) == RESET);
    GPIO_ResetBits(DR_GPIO, DR_PIN);
}

static void modbusError(struct request_struct *request, uint8_t error)
{
    uint8_t buffer[5] = {request->address, request->function | 0x80, error};
    modbusTransmit(buffer, sizeof(buffer));
}

static void modbusReply(struct request_struct *request, uint8_t *data, uint8_t length)
{
    uint8_t buffer[MODBUS_MAX_LENGTH] = {request->address, request->function};

    switch (request->function)
    {
        case MODBUS_READ_HOLDING_REGISERS:
        case MODBUS_READ_INPUT_REGISERS:
            buffer[2] = length;
            memcpy(buffer + 3, data, length);
            length += 5;
            break;

        case MODBUS_WRITE_SINGLE_COIL:
        case MODBUS_WRITE_SINGLE_REGISER:
        case MODBUS_WRITE_MULTIPLE_REGISERS:
            memcpy(buffer + 2, data, length);
            length += 4;
            break;

        case MODBUS_REPORT_SLAVE_ID:
            buffer[2] = length + 1;
            buffer[length + 3] = 0xFF;
            memcpy(buffer + 3, data, length);
            length += 6;
            break;
    }

    modbusTransmit(buffer, length);
}

static void modbusReadRegisters(struct request_struct *request)
{
    uint16_t address = (uint16_t) (request->data[0] << 8 | request->data[1]), count = (uint16_t) (request->data[2] << 8 | request->data[3]);
    uint8_t buffer[MODBUS_MAX_COUNT * 2], length = 0;

    if (!count || count > MODBUS_MAX_COUNT)
    {
        modbusError(request, MODBUS_ERROR_VALUE);
        return;
    }

    for (uint8_t i = 0; i < count; i++)
    {
        uint16_t data = 0;
        uint8_t result = 0, error = MODBUS_ERROR_FUNCTION;

        switch (request->function)
        {
            case MODBUS_READ_HOLDING_REGISERS:
                result = registerReadHolding(address + i, &data, &error);
                break;

            case MODBUS_READ_INPUT_REGISERS:
                result = registerReadInput(address + i, &data, &error);
                break;
        }

        if (!result)
        {
            modbusError(request, error);
            return;
        }

        buffer[length++] = (uint8_t) (data >> 8);
        buffer[length++] = (uint8_t) (data);
    }

    modbusReply(request, buffer, length);
}

static void modbusWriteSingleRegister(struct request_struct *request)
{
    uint16_t address = (uint16_t) (request->data[0] << 8 | request->data[1]), data = (uint16_t) (request->data[2] << 8 | request->data[3]);
    uint8_t error;

    if (!registerWriteHolding(address, &data, &error))
    {
        modbusError(request, error);
        return;
    }

    modbusReply(request, request->data, 4);
}

static void modbusWriteMultipleRegisters(struct request_struct *request)
{
    uint16_t address = (uint16_t) (request->data[0] << 8 | request->data[1]), count = (uint16_t) (request->data[2] << 8 | request->data[3]);

    if (!count || count > MODBUS_MAX_COUNT || count * 2 != request->data[4])
    {
        modbusError(request, MODBUS_ERROR_VALUE);
        return;
    }

    for (uint8_t i = 0; i < count; i++)
    {
        uint16_t data = (uint16_t) (request->data[5 + i * 2] << 8 | request->data[6 + i * 2]);
        uint8_t error;

        if (!registerWriteHolding(address + i, &data, &error))
        {
            modbusError(request, error);
            return;
        }
    }

    modbusReply(request, request->data, 4);
}

static void modbusWriteSingleCoil(struct request_struct *request)
{
    uint16_t address = (uint16_t) (request->data[0] << 8 | request->data[1]), data = (uint16_t) (request->data[2] << 8 | request->data[3]);
    uint8_t error;

    if (data && data != 0xFF00)
    {
        modbusError(request, MODBUS_ERROR_VALUE);
        return;
    }

    if (!registerWriteCoil(address, &data, &error))
    {
        modbusError(request, error);
        return;
    }

    modbusReply(request, request->data, 4);
}

static void modbusReportSlaveID(struct request_struct *request)
{
    char *id = SLAVE_ID;
    modbusReply(request, (uint8_t*) id, (uint8_t) strlen(id));
}

void modbusHandler(uint8_t *data, uint8_t length)
{
    struct request_struct request = {data[0], data[1], data + 2};

    if (length < 8 || request.address != config.address || !crcCheck(data, length))
        return;

    blink = 1;

    switch (request.function)
    {
        case MODBUS_READ_HOLDING_REGISERS:
        case MODBUS_READ_INPUT_REGISERS:
            modbusReadRegisters(&request);
            break;

        case MODBUS_WRITE_SINGLE_COIL:
            modbusWriteSingleCoil(&request);
            break;

        case MODBUS_WRITE_SINGLE_REGISER:
            modbusWriteSingleRegister(&request);
            break;

        case MODBUS_WRITE_MULTIPLE_REGISERS:
            modbusWriteMultipleRegisters(&request);
            break;

        case MODBUS_REPORT_SLAVE_ID:
            modbusReportSlaveID(&request);
            break;

        default:
            modbusError(&request, MODBUS_ERROR_FUNCTION);
            break;
    }
}
