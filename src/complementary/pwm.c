#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"

#include "main.h"
#include "pwm.h"

/* Timer handler declaration */
static TIM_HandleTypeDef    TimHandle;
static uint32_t CARRIER_FREQ = (uint32_t)(150000);  // hz, carrier frequency
static uint32_t PULSE_VALUE  = (uint32_t)(MIN_DUTY_CYCLE);

/* Timer Break Configuration Structure declaration */
static TIM_BreakDeadTimeConfigTypeDef sBreakConfig;

//------------------------------------------------------------------------------
void
PWM_initAndStart() {
  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef sTimConfig;

  /* Counter Prescaler value */
  uint32_t uhPrescalerValue = 0;

  /* Compute the prescaler value to have TIM1 counter clock equal to 50 kHz */
  uhPrescalerValue = (uint32_t)(SystemCoreClock / (CARRIER_FREQ * PERIOD_VALUE) - 1);

  /*##-1- Configure the TIM peripheral #######################################*/
  TimHandle.Instance = TIM1;

  TimHandle.Init.Prescaler         = uhPrescalerValue;
  TimHandle.Init.Period            = PERIOD_VALUE - 1;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;

  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK) {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  //  sTimConfig.OCMode       = TIM_OCMODE_PWM1;
  sTimConfig.OCMode       = TIM_OCMODE_PWM1;
  sTimConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sTimConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sTimConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sTimConfig.OCIdleState  = TIM_OCIDLESTATE_SET;
  sTimConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;


  /* Set the pulse value for channel 1 */
  sTimConfig.Pulse = PULSE_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sTimConfig, TIM_CHANNEL_1) != HAL_OK) {
    /* Configuration Error */
    Error_Handler();
  }

  /* Set the Break feature & Dead time */
  sBreakConfig.BreakState       = TIM_BREAK_DISABLE;
  sBreakConfig.DeadTime         = DEAD_TIME;
  sBreakConfig.OffStateRunMode  = TIM_OSSR_ENABLE;
  sBreakConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
  sBreakConfig.LockLevel        = TIM_LOCKLEVEL_1;
  sBreakConfig.BreakPolarity    = TIM_BREAKPOLARITY_LOW;
  sBreakConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;

  if(HAL_TIMEx_ConfigBreakDeadTime(&TimHandle, &sBreakConfig) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  //__HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_CC1);
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK) {
    /* PWM Generation Error */
    Error_Handler();
  }

  /* Start channel 1N */
  if(HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK) {
    /* Starting Error */
    Error_Handler();
  }

  /*##-2- Set PA06 IO, enable IR2110 */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

}

//------------------------------------------------------------------------------
bool
PWM_setFixedPulse(uint16_t pulse) {
    if(pulse < MIN_DUTY_CYCLE || pulse > MAX_DUTY_CYCLE)
        return false;

    if (PULSE_VALUE != pulse) {
      PULSE_VALUE = pulse;
      __HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_1, pulse);
    }

    return true;
}

//------------------------------------------------------------------------------
uint16_t
PWM_getFixedPulse() {
    return PULSE_VALUE;
}

//------------------------------------------------------------------------------
bool
PWM_setFreq(uint32_t freq) {
    if(freq < MIN_FREQ || freq > MAX_FREQ)
        return false;

    CARRIER_FREQ = freq;

    /* Set the Prescaler value */
    TimHandle.Instance->PSC = (uint32_t)(SystemCoreClock /
                                         (CARRIER_FREQ * PERIOD_VALUE) - 1);
    return true;
}

//------------------------------------------------------------------------------
uint32_t
PWM_getFreq() {
    return CARRIER_FREQ;
}

//------------------------------------------------------------------------------
/**
 * @brief  This function handles TIMx_INSTANCE Interrupt.
 * @param  None
 * @retval None
 */

void TIM1_CC_IRQHandler()
{
    HAL_TIM_IRQHandler(&TimHandle);
}

//------------------------------------------------------------------------------
/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void
HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
  UNUSED(htim);
  GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */
  __HAL_RCC_TIM1_CLK_ENABLE();

  /* Enable GPIO Channels Clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure (PA.08, TIM1_CH1), (PB.13, TIM1_CH1N), (PA.09, TIM1_CH2),
               (PB.14, TIM1_CH2N), (PA.10, TIM1_CH3), (PB.15, TIM1_CH3N),
               (PB.12, TIM1_BKIN) in push-pull, alternate function mode  */

  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  /* GPIO TIM1_Channel1 configuration */
//  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* GPIO TIM1_Channel1N configuration */
//  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


  /*##-1- Configure PA05 IO in output push-pull mode ###*/
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
