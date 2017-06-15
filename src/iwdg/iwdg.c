#include "iwdg.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"

static IWDG_HandleTypeDef IwdgHandle;


//------------------------------------------------------------------------------
void
IWDG_refresh() {
    /* Refresh IWDG: reload counter */
    if (HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK) {
        /* Refresh Error */
        Error_Handler();
    }
}

//------------------------------------------------------------------------------
void
IWDG_initAndStart(uint32_t periodMs) {
    /*##-1- Check if the system has resumed from IWDG reset ####################*/
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) {
        /* Clear reset flags */
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }

    /*##-2- Get the LSI frequency: TIM5 is used to measure the LSI frequency ###*/
    //uwLsiFreq = GetLSIFrequency();

    /*##-3- Configure the IWDG peripheral ######################################*/
    IwdgHandle.Instance = IWDG;

    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
    IwdgHandle.Init.Reload    = (periodMs * 40) / 32;

    if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK) {
        /* Initialization Error */
        Error_Handler();
    }

    /*##-4- Start the IWDG #####################################################*/
    if (HAL_IWDG_Start(&IwdgHandle) != HAL_OK) {
        Error_Handler();
    }
}
