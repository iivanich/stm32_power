#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"

#include "main.h"
#include "dac.h"

#ifdef STM32F446xx

static DAC_HandleTypeDef DacHandle;
static DAC_ChannelConfTypeDef sConfig;

//------------------------------------------------------------------------------
void
DAC_initAndStart() {
  /*---1- Configure the DAC peripheral ---------------------------------------*/
  DacHandle.Instance = DAC;

  if (HAL_DAC_Init(&DacHandle) != HAL_OK) {
    /* Initialization Error */
    Error_Handler();
  }

  /*---2- Configure DAC channel1 ---------------------------------------------*/
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

  if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
    /* Channel configuration Error */
    Error_Handler();
  }

  /*---3- Set DAC Channel1 DHR register --------------------------------------*/
  if (HAL_DAC_SetValue(&DacHandle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0xFFF) != HAL_OK) {
    /* Setting value Error */
    Error_Handler();
  }

  /*---4- Enable DAC Channel1 ------------------------------------------------*/
  if (HAL_DAC_Start(&DacHandle, DAC_CHANNEL_1) != HAL_OK) {
    /* Start Error */
    Error_Handler();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief DAC MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 * @param hdac: DAC handle pointer
 * @retval None
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
  UNUSED(hdac);
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*---1- Enable peripherals and GPIO Clocks ---------------------------------*/
  /* Enable GPIO clock ---------------------------------------*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* DAC Periph clock enable */
  __HAL_RCC_DAC_CLK_ENABLE();

  /*---2- Configure peripheral GPIO ------------------------------------------*/
  /* DAC Channel1 GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//------------------------------------------------------------------------------
void DAC_setValue(uint16_t value) {
  if (HAL_DAC_SetValue(&DacHandle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value) != HAL_OK) {
    /* Setting value Error */
    Error_Handler();
  }
}

//------------------------------------------------------------------------------

#endif
