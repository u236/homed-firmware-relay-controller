#include "config.h"
#include "output.h"

uint16_t outputData = 0;

void outputUpdate(void)
{
    uint16_t status = config.invert ? ~(outputData >> 8 | outputData << 8) : outputData >> 8 | outputData << 8;

    for (uint8_t i = 0; i < 16; i++)
    {
        GPIO_ResetBits(SR_GPIO, SR_CLK_PIN | SR_DATA_PIN);

        if (status & (1 << i))
            GPIO_SetBits(SR_GPIO, SR_DATA_PIN);

        GPIO_SetBits(SR_GPIO, SR_CLK_PIN);
    }

    GPIO_SetBits(SR_GPIO, SR_LATCH_PIN);
    GPIO_ResetBits(SR_GPIO, SR_LATCH_PIN);
}
