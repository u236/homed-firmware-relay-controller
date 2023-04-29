#ifndef BOARD_H
#define BOARD_H

#define SLAVE_ID                "HOMEd Relay Controller"

#define SR_GPIO                 GPIOA
#define SR_DATA_PIN             GPIO_Pin_0
#define SR_LATCH_PIN            GPIO_Pin_1
#define SR_CLK_PIN              GPIO_Pin_2

#define LED_GPIO                GPIOA
#define LED_PIN                 GPIO_Pin_3

#define DR_GPIO                 GPIOB
#define DR_PIN                  GPIO_Pin_1

#define UART_GPIO               GPIOA
#define UART_AF                 GPIO_AF_1
#define UART_TX_PIN             GPIO_Pin_9
#define UART_TX_SOURCE          GPIO_PinSource9
#define UART_RX_PIN             GPIO_Pin_10
#define UART_RX_SOURCE          GPIO_PinSource10

#define UART_PORT               USART1
#define UART_IRQ                USART1_IRQn
#define UART_HANDLER            USART1_IRQHandler
#define UART_TIMEOUT            5000

#define UART_TIMER              TIM3
#define UART_TIMER_IRQ          TIM3_IRQn
#define UART_TIMER_HANDLER      TIM3_IRQHandler

#define TICK_HANDLER            SysTick_Handler

#include <string.h>
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"

extern uint32_t uptime;

void boardInitUART(void);
void boardInit(void);

#endif
