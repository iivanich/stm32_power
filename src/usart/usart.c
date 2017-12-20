//#include "main.h"
#include "usart.h"
#include "fifo.h"

static DrvUsartConf usartConfs[] = {
        {GPIOA, USART1,
         GPIO_PIN_9, GPIO_PIN_10, 0x7,
         USART1_IRQn, NULL},
        {GPIOA, USART2,
         GPIO_PIN_2, GPIO_PIN_3, 0x7,
         USART2_IRQn, NULL}
};

#define TX 0
#define RX 1

static uint8_t usartBuffers[2][2][DRV_USART_BUFFER_SIZE + sizeof(Fifo_t)];
static __IO bool txIrqDisabled[2] = {true, true};

static uint8_t rxChar = 0;

//------------------------------------------------------------------------------
uint16_t
DrvUsartRead(uint8_t com, uint8_t* buffer, uint16_t maxBufSize) {

        __disable_irq();

        int iCnter = 0;
        for (; !IsFifoEmpty((Fifo_t*)usartBuffers[com][RX])
                     && iCnter < maxBufSize; iCnter ++) {
                buffer[iCnter] = FifoPop((Fifo_t*)usartBuffers[com][RX]);
        }

        __enable_irq();

        return iCnter;
}

//------------------------------------------------------------------------------
void
DrvUsartReadBlocking(uint8_t com, uint8_t* buffer, uint16_t maxBufSize) {
        uint16_t bytesRead = 0;
        while(bytesRead < maxBufSize) {
                bytesRead += DrvUsartRead(com, buffer + bytesRead,
                                          maxBufSize - bytesRead);
                __WFI();
        }
}

//------------------------------------------------------------------------------
void
DrvUsartWriteBlocking(uint8_t com, uint8_t* buffer, uint16_t size, bool flush) {
        uint16_t bytesWritten = 0;
        while(bytesWritten < size) {
                bytesWritten += DrvUsartWrite(com, buffer + bytesWritten,
                                                 size - bytesWritten);
                __WFI();
        }

        if (flush)
                while(!IsFifoEmpty((Fifo_t*)usartBuffers[com][TX]))
                        __WFI();
}

//------------------------------------------------------------------------------
uint16_t
DrvUsartWrite(uint8_t com, uint8_t* buffer, uint16_t size) {
        __disable_irq();

        int iCnter = 0;
        for (; !IsFifoFull((Fifo_t*)usartBuffers[com][TX])
                     && iCnter < size; iCnter ++) {
                FifoPush((Fifo_t*)usartBuffers[com][TX], buffer[iCnter]);
        }

        if (txIrqDisabled[com]) {
                HAL_UART_TxCpltCallback(usartConfs[com].USART_InitStruct);
                txIrqDisabled[com] = false;
        }

        __enable_irq();

        return iCnter;
}

//------------------------------------------------------------------------------
void
DrvUsartInit(UART_HandleTypeDef* USART_InitStruct) {
        if(HAL_UART_DeInit(USART_InitStruct) != HAL_OK) {
                Error_Handler();
        }

        if(HAL_UART_Init(USART_InitStruct) != HAL_OK) {
                Error_Handler();
        }

        if(HAL_UART_Receive_IT(USART_InitStruct, &rxChar, 1) != HAL_OK) {
                // Transfer error in reception process
                Error_Handler();
        }
}


//------------------------------------------------------------------------------
void
DrvGpioClkEnable(uint8_t com) {
        switch ((uint32_t)usartConfs[com].gpioPort) {
        case (uint32_t)GPIOA:
                __HAL_RCC_GPIOA_CLK_ENABLE();
                break;

        case (uint32_t)GPIOB:
                __HAL_RCC_GPIOB_CLK_ENABLE();
                break;

        case (uint32_t)GPIOC:
                __HAL_RCC_GPIOC_CLK_ENABLE();
                break;

        default: // not supported
                Error_Handler();
        }
}

//------------------------------------------------------------------------------
void
DrvUsartClkEnable(uint8_t com) {
        switch ((uint32_t)usartConfs[com].usartPort) {
        case (uint32_t)USART1:
                __HAL_RCC_USART1_CLK_ENABLE();
                break;

        case (uint32_t)USART2:
                __HAL_RCC_USART2_CLK_ENABLE();
                break;

        default: // not supported
                Error_Handler();
        }
}

//------------------------------------------------------------------------------
void
DrvUsartMspInit(UART_HandleTypeDef* USART_InitStruct) {
        uint8_t com = 0;
        while (usartConfs[com].usartPort != USART_InitStruct->Instance)
                com ++;

        usartConfs[com].USART_InitStruct = USART_InitStruct;

        DrvGpioClkEnable(com);
        DrvUsartClkEnable(com);

        GPIO_InitTypeDef  GPIO_InitStruct;

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = usartConfs[com].pinTx;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

        HAL_GPIO_Init(usartConfs[com].gpioPort, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = usartConfs[com].pinRx;
        GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;

        HAL_GPIO_Init(usartConfs[com].gpioPort, &GPIO_InitStruct);

        /* init rx buffer */
        FifoInit((Fifo_t*)usartBuffers[com][RX], DRV_USART_BUFFER_SIZE);

        /* init tx buffer */
        FifoInit((Fifo_t*)usartBuffers[com][TX], DRV_USART_BUFFER_SIZE);

        HAL_NVIC_SetPriority(usartConfs[com].irq, 1, 0);
        HAL_NVIC_EnableIRQ(usartConfs[com].irq);
}

//------------------------------------------------------------------------------
//fixme: modify to match com!
void
DrvUsartMspDeInit(UART_HandleTypeDef* USART_InitStruct) {
        uint8_t com = 0;
        while (usartConfs[com].usartPort != USART_InitStruct->Instance)
                com ++;

        /*##-1- Reset peripherals ##################################################*/
        switch ((uint32_t)usartConfs[com].usartPort) {
        case (uint32_t)USART1:
                __HAL_RCC_USART1_FORCE_RESET();
                __HAL_RCC_USART1_RELEASE_RESET();
                break;
        case (uint32_t)USART2:
                __HAL_RCC_USART2_FORCE_RESET();
                __HAL_RCC_USART2_RELEASE_RESET();
                break;
        default: // not supported
                Error_Handler();
        }

        /*##-2- Disable peripherals and GPIO Clocks #################################*/
        HAL_GPIO_DeInit(usartConfs[com].gpioPort, usartConfs[com].pinTx);
        HAL_GPIO_DeInit(usartConfs[com].gpioPort, usartConfs[com].pinRx);

        /*##-3- Disable the NVIC for UART ##########################################*/
        HAL_NVIC_DisableIRQ(usartConfs[com].irq);
}

//------------------------------------------------------------------------------

/**
 * @brief  This function handles USART1 global interrupt request.
 * @param  None
 * @retval None
 */

void
USART1_IRQHandler(void) {
        HAL_UART_IRQHandler(usartConfs[0].USART_InitStruct);
}

//------------------------------------------------------------------------------

/**
 * @brief  This function handles USART2 global interrupt request.
 * @param  None
 * @retval None
 */

void
USART2_IRQHandler(void) {
        HAL_UART_IRQHandler(usartConfs[1].USART_InitStruct);
}


//------------------------------------------------------------------------------

/**
 * @brief  Tx Transfer completed callback
 * @param  UartHandle: UART handle.
 * @note   This example shows a simple way to report end of IT Tx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void
HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
        /* Turn LED1 on: Transfer in transmission process is correct */
        //BSP_LED_On(LED1);
        static uint8_t txChar = 0;

        int com = 0;
        while (usartConfs[com].USART_InitStruct != UartHandle)
                com ++;

        if (!IsFifoEmpty((Fifo_t*)usartBuffers[com][TX])) {
                txChar = FifoPop((Fifo_t*)usartBuffers[com][TX]);

                if(HAL_UART_Transmit_IT(UartHandle, &txChar, 1)!= HAL_OK) {
                        /* Transfer error in transmission process */
                        Error_Handler();
                }
        } else {
                txIrqDisabled[com] = true;
        }
}

//------------------------------------------------------------------------------

/**
 * @brief  Rx Transfer completed callback
 * @param  UartHandle: UART handle
 * @note   This example shows a simple way to report end of IT Rx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void
HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
        /* Turn LED1 on: Transfer in reception process is correct */
        //BSP_LED_On(LED1);
        int com = 0;
        while (usartConfs[com].USART_InitStruct != UartHandle)
                com ++;

        if (!IsFifoFull((Fifo_t*)usartBuffers[com][RX])) {
                /* Read one byte from the receive data register */
                FifoPush((Fifo_t*)usartBuffers[com][RX], rxChar);
        }

        if(HAL_UART_Receive_IT(UartHandle, &rxChar, 1) != HAL_OK) {
                /* Transfer error in reception process */
                Error_Handler();
        }
}

//------------------------------------------------------------------------------
/**
 * @brief UART MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 *           - DMA configuration for transmission request by peripheral
 *           - NVIC configuration for DMA interrupt request enable
 * @param huart: UART handle pointer
 * @retval None
 */
void
HAL_UART_MspInit(UART_HandleTypeDef *huart) {
        DrvUsartMspInit(huart);
}

//------------------------------------------------------------------------------
void
HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
        DrvUsartMspDeInit(huart);
}

//------------------------------------------------------------------------------
