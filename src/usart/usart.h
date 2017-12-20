#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>
//#include "main.h"
#include <stdbool.h>


#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"

typedef struct {
        GPIO_TypeDef*  gpioPort;
        USART_TypeDef* usartPort;
        uint16_t pinTx;
        uint16_t pinRx;
        uint8_t altFunc;
        IRQn_Type irq;
        UART_HandleTypeDef* USART_InitStruct;
} DrvUsartConf;

#define DRV_USART1 0
#define DRV_USART2 1

#define DRV_USART_BUFFER_SIZE 256

//------------------------------------------------------------------------------
void
DrvUsartInit(UART_HandleTypeDef* USART_InitStruct);

//------------------------------------------------------------------------------
void
DrvUsartMspInit(UART_HandleTypeDef* USART_InitStruct);

//------------------------------------------------------------------------------
uint16_t
DrvUsartRead(uint8_t com, uint8_t* buffer, uint16_t maxSize);

//------------------------------------------------------------------------------
uint16_t
DrvUsartWrite(uint8_t com, uint8_t* buffer, uint16_t size);

//------------------------------------------------------------------------------
void
DrvUsartReadBlocking(uint8_t com, uint8_t* buffer, uint16_t maxSize);

//------------------------------------------------------------------------------
void
DrvUsartWriteBlocking(uint8_t com, uint8_t* buffer, uint16_t size, bool flush);

//------------------------------------------------------------------------------
void
DrvUsartDeInit(uint8_t com);

//------------------------------------------------------------------------------

#endif
