#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"

#include "adc.h"
#include "pwm.h"
#include "main.h"

/* ADC handler declaration */
static ADC_HandleTypeDef    AdcHandle;

/* Variable used to get converted value */
static __IO uint16_t adcGateVValue = 0;
static __IO uint16_t adcOutVValue = 0;
static __IO uint16_t adcSolarInVValue = 0;
static __IO uint32_t adcGateVAvg = 0;
static __IO uint32_t adcOutVAvg = 0;
static __IO uint32_t adcSolarInVAvg = 0;
static __IO uint32_t adcAvgCounter = 0;

static __IO uint16_t maxAdcOutVValue = 1700; // max output voltage
static __IO uint16_t minAdcSolarInVValue = 600; // min input voltage
static __IO uint16_t minAdcGateVValue = 1000; // min gate input voltage

static __IO uint16_t uhADCxConvertedValue[24];

static __IO bool adcManualMode = false;

//------------------------------------------------------------------------------
void
ADC_initAndStart() {

  ADC_ChannelConfTypeDef sAdcConfig;
  /* STM32F4xx HAL library initialization:
     - Configure the Flash prefetch
     - Systick timer is configured by default as source of time base, but user
     can eventually implement his proper time base source (a general purpose
     timer for example or other time source), keeping in mind that Time base
     duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
     handled in milliseconds basis.
     - Set NVIC Group Priority to 4
     - Low Level Initialization
  */

  /*##-1- Configure the ADC peripheral #######################################*/
  AdcHandle.Instance                   = ADC1;
  AdcHandle.Init.ScanConvMode          = ENABLE;                        /* Sequencer enabled (ADC conversion on 2 channels) */
  AdcHandle.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 3; //number of channels

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
    /* ADC initialization Error */
    Error_Handler();
  }

  sAdcConfig.Channel      = ADC_CHANNEL_7;
  sAdcConfig.Rank         = ADC_REGULAR_RANK_1;
  sAdcConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;

  if (HAL_ADC_ConfigChannel(&AdcHandle, &sAdcConfig) != HAL_OK) {
    /* Channel Configuration Error */
    Error_Handler();
  }

  /*##-2- Configure ADC regular channel 1 ######################################*/
  sAdcConfig.Channel      = ADC_CHANNEL_8;
  sAdcConfig.Rank         = ADC_REGULAR_RANK_2;
  sAdcConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;

  if (HAL_ADC_ConfigChannel(&AdcHandle, &sAdcConfig) != HAL_OK) {
    /* Channel Configuration Error */
    Error_Handler();
  }

  /*##--- Configure ADC regular channel 2 ######################################*/
  sAdcConfig.Channel      = ADC_CHANNEL_9;
  sAdcConfig.Rank         = ADC_REGULAR_RANK_3;
  sAdcConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;

  if (HAL_ADC_ConfigChannel(&AdcHandle, &sAdcConfig) != HAL_OK) {
    /* Channel Configuration Error */
    Error_Handler();
  }

  /*##-3- Start the conversion process #######################################*/
  /* Note: Considering IT occurring after each number of ADC conversions      */
  /*       (IT by DMA end of transfer), select sampling time and ADC clock    */
  /*       with sufficient duration to not create an overhead situation in    */
  /*        IRQHandler. */
  if(HAL_ADC_Start_DMA(&AdcHandle, (uint32_t*)&uhADCxConvertedValue, 24) != HAL_OK) {
    /* Start Conversation Error */
    Error_Handler();
  }
}


//------------------------------------------------------------------------------
void
ADC_getAvgs(uint16_t* avgGateV, uint16_t* avgOutV, uint16_t* avgSolarInV) {
  __disable_irq();
  *avgGateV = adcGateVAvg / adcAvgCounter;
  *avgOutV = adcOutVAvg / adcAvgCounter;
  *avgSolarInV = adcSolarInVAvg / adcAvgCounter;
  adcOutVAvg = adcSolarInVAvg = adcGateVAvg = 0;
  adcAvgCounter = 0;
  __enable_irq();
  return;
}

//------------------------------------------------------------------------------
void
ADC_resetAvgs() {
  __disable_irq();
  adcOutVAvg = adcSolarInVAvg = adcGateVAvg = 0;
  adcAvgCounter = 0;
  __enable_irq();
  return;
}

//------------------------------------------------------------------------------
uint32_t
ADC_getAvgCounter() {
  uint32_t result = 0;
  __disable_irq();
  result = adcAvgCounter;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getGateVAvg() {
  uint16_t result = 0;
  __disable_irq();
  result = adcGateVAvg / adcAvgCounter;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getOutVAvg() {
  uint16_t result = 0;
  __disable_irq();
  result = adcOutVAvg / adcAvgCounter;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getSolarInVAvg() {
  uint16_t result = 0;
  __disable_irq();
  result = adcSolarInVAvg / adcAvgCounter;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getOutVValue() {
    uint16_t result = 0;
    __disable_irq();
    result = adcOutVValue;
    __enable_irq();
    return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getSolarInVValue() {
    uint16_t result = 0;
    __disable_irq();
    result = adcSolarInVValue;
    __enable_irq();
    return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getGateVValue() {
  uint16_t result = 0;
  __disable_irq();
  result = adcGateVValue;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getMaxOutVValue() {
    uint16_t result = 0;
    __disable_irq();
    result = maxAdcOutVValue;
    __enable_irq();
    return result;
}

//------------------------------------------------------------------------------
void
ADC_setMaxOutVValue(uint16_t value) {
    __disable_irq();
    maxAdcOutVValue = value;
    __enable_irq();
}

//------------------------------------------------------------------------------
uint16_t
ADC_getMinSolarInVValue() {
  uint16_t result = 0;
  __disable_irq();
  result = minAdcSolarInVValue;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
uint16_t
ADC_getMinGateVValue() {
  uint16_t result = 0;
  __disable_irq();
  result = minAdcGateVValue;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
bool
ADC_getManualMode() {
  bool result = 0;
  __disable_irq();
  result = adcManualMode;
  __enable_irq();
  return result;
}

//------------------------------------------------------------------------------
void
ADC_setMinSolarInVValue(uint16_t value) {
  __disable_irq();
  minAdcSolarInVValue = value;
  __enable_irq();
}

//------------------------------------------------------------------------------
void
ADC_setManualMode(bool value) {
  __disable_irq();
  adcManualMode = value;
  __enable_irq();
}

//------------------------------------------------------------------------------
void
ADC_setMinGateVValue(uint16_t value) {
  __disable_irq();
  minAdcGateVValue = value;
  __enable_irq();
}

//------------------------------------------------------------------------------
void
ADC_adjustPWM() {
  static uint16_t lastAvgOutV = 0;

  if (ADC_getOutVAvg() > ADC_getMaxOutVValue() && ADC_getAvgCounter() > 200) {
    PWM_setFixedPulse(PWM_getFixedPulse() - 1);
    ADC_resetAvgs();
  } else if (ADC_getSolarInVAvg() < ADC_getMinSolarInVValue() && ADC_getAvgCounter() > 100) {
    PWM_setFixedPulse(PWM_getFixedPulse() - 1); //decrease pulse,
                                                //reset possible otherwise!
    ADC_resetAvgs();
  } else if (ADC_getAvgCounter() > 1000) {
    uint16_t avgOutV, avgSolarInV, avgGate;
    static int8_t pulseInc = 1;

    ADC_getAvgs(&avgGate, &avgOutV, &avgSolarInV);

    if (avgOutV < lastAvgOutV) {
      pulseInc = - pulseInc;
    } else if (PWM_getFixedPulse() == MIN_DUTY_CYCLE
               || PWM_getFixedPulse() == MAX_DUTY_CYCLE) {
      pulseInc = - pulseInc;
    }

    lastAvgOutV = avgOutV;

    //if (lastAvgA > 0)
    PWM_setFixedPulse(PWM_getFixedPulse() + pulseInc);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief  Conversion complete callback in non blocking mode
 * @param  AdcHandle : AdcHandle handle
 * @note   This example shows a simple way to report end of conversion, and
 *         you can add your own implementation.
 * @retval None
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  UNUSED(hadc);

  uint16_t sum = uhADCxConvertedValue[12]
    + uhADCxConvertedValue[15]
    + uhADCxConvertedValue[18]
    + uhADCxConvertedValue[21];

  adcGateVValue = adcGateVValue / 2 + sum / 8;

  sum = uhADCxConvertedValue[13]
    + uhADCxConvertedValue[16]
    + uhADCxConvertedValue[19]
    + uhADCxConvertedValue[22];

  adcSolarInVValue = adcSolarInVValue / 2 + sum / 8;

  sum = uhADCxConvertedValue[14]
    + uhADCxConvertedValue[17]
    + uhADCxConvertedValue[20]
    + uhADCxConvertedValue[23];

  adcOutVValue = adcOutVValue / 2 + sum / 8;

  adcGateVAvg += adcGateVValue;
  adcOutVAvg += adcOutVValue;
  adcSolarInVAvg += adcSolarInVValue;
  adcAvgCounter ++;

  if (!ADC_getManualMode())
    ADC_adjustPWM();
}

//------------------------------------------------------------------------------
/**
 * @brief  Regular conversion half DMA transfer callback in non blocking mode
 * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
 *         the configuration information for the specified ADC.
 * @retval None
 */

void
HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
  UNUSED(hadc);

  uint16_t sum = uhADCxConvertedValue[0]
    + uhADCxConvertedValue[3]
    + uhADCxConvertedValue[6]
    + uhADCxConvertedValue[9];

  adcGateVValue = adcGateVValue / 2 + sum / 8;

  sum = uhADCxConvertedValue[1]
    + uhADCxConvertedValue[4]
    + uhADCxConvertedValue[7]
    + uhADCxConvertedValue[10];

  adcSolarInVValue = adcSolarInVValue / 2 + sum / 8;

  sum = uhADCxConvertedValue[2]
    + uhADCxConvertedValue[5]
    + uhADCxConvertedValue[8]
    + uhADCxConvertedValue[11];

  adcOutVValue = adcOutVValue / 2 + sum / 8;
}

//------------------------------------------------------------------------------
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;
  static DMA_HandleTypeDef  hdma_adc;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* ADC1 Periph clock enable */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /* Enable GPIO clock ****************************************/
  //__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Enable DMA1 clock */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* ADC Channel 8 GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* ADC Channel 8 GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* ADC Channel 9 GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*##-3- Configure the DMA streams ##########################################*/
  /* Set the parameters to be configured */
  hdma_adc.Instance = DMA1_Channel1;
  hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc.Init.Mode = DMA_CIRCULAR;
  hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&hdma_adc);

  /* Associate the initialized DMA handle to the ADC handle */
  __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt */
  //HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

//------------------------------------------------------------------------------

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles DMA interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}
