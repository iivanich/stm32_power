/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "dbg.h"
#include "cmd_loop.h"
#include "adc.h"
#include "dac.h"
#include "pwm.h"
#include "iwdg.h"
//------------------------------------------------------------------------------


/** @addtogroup STM32F4xx_HAL_Examples
 * @{
 */

/** @addtogroup ADC_RegularConversion_DMA
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

static UART_HandleTypeDef UartHandle;
//------------------------------------------------------------------------------
static void
initAndStartDbgUsart() {
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
     - Word Length = 8 Bits
     - Stop Bit = One Stop bit
     - Parity = None
     - BaudRate = 115200 baud
     - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USART2;
  dbgCom = DRV_USART2;

  UartHandle.Init.BaudRate     = 115200;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  //UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  DrvUsartInit(&UartHandle);
  DBG_setVerbosity(1);
}

//------------------------------------------------------------------------------
/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {

  HAL_Init();

  /* Configure the system clock to 180 MHz */
  SystemClock_Config();

  IWDG_initAndStart(250);

  initAndStartDbgUsart();

  DBG(1, "\r\nStarting...\r\n");

  /* Configure LED2 */
  BSP_LED_Init(LED2);

#ifdef STM32F446xx
  DAC_initAndStart();
#endif

  PWM_initAndStart();
  ADC_initAndStart();

  mainLoop();
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 180000000
 *            HCLK(Hz)                       = 180000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 360
 *            PLL_P                          = 2
 *            PLL_Q                          = 7
 *            PLL_R                          = 2
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 5
 * @param  None
 * @retval None
 */

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};

  /* Configure PLL ------------------------------------------------------*/
  /* PLL configuration: PLLCLK = (HSI / 2) * PLLMUL = (8 / 2) * 16 = 64 MHz */
  /* PREDIV1 configuration: PREDIV1CLK = PLLCLK / HSEPredivValue = 64 / 1 = 64 MHz */
  /* Enable HSI and activate PLL with HSi_DIV2 as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
  oscinitstruct.HSEState        = RCC_HSE_OFF;
  oscinitstruct.LSEState        = RCC_LSE_OFF;
  oscinitstruct.HSIState        = RCC_HSI_ON;
  oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  oscinitstruct.HSEPredivValue    = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSI_DIV2;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void
Error_Handler(void) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  while (true) {
    BSP_LED_Toggle(LED2);
    HAL_Delay(500);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief  UART error callbacks
 * @param  UartHandle: UART handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
void
HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle) {
  UNUSED(UartHandle);
}



#ifdef  USE_FULL_ASSERT

//------------------------------------------------------------------------------
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void
assert_failed(uint8_t *file, uint32_t line) {
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /* Infinite loop */
  while (true) {
    BSP_LED_Toggle(LED2);

    HAL_Delay(250);
  }
}

#endif


/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
