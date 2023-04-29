#include "config.h"

uint32_t uptime = 0;

static void boardInitGPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;

    GPIO_InitStruct.GPIO_Pin = SR_CLK_PIN | SR_LATCH_PIN | SR_DATA_PIN;
    GPIO_Init(SR_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_Init(LED_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = DR_PIN;
    GPIO_Init(DR_GPIO, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;

    GPIO_InitStruct.GPIO_Pin = UART_TX_PIN | UART_RX_PIN;
    GPIO_Init(UART_GPIO, &GPIO_InitStruct);

    GPIO_PinAFConfig(UART_GPIO, UART_TX_SOURCE, UART_AF);
    GPIO_PinAFConfig(UART_GPIO, UART_RX_SOURCE, UART_AF);

    GPIO_ResetBits(DR_GPIO, DR_PIN);
}

static void boardInitTIM(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler = 47;
    TIM_TimeBaseInitStruct.TIM_Period = UART_TIMEOUT;
    TIM_TimeBaseInit(UART_TIMER, &TIM_TimeBaseInitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = UART_TIMER_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(UART_TIMER, ENABLE);
}

void boardInitUART(void)
{
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = config.baudrate * 100;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART_PORT, &USART_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(UART_PORT, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART_PORT, ENABLE);
}

void boardInit(void)
{
    SysTick_Config(SystemCoreClock / 1000);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    boardInitGPIO();
    boardInitTIM();
    boardInitUART();
}
