#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "handlers.h"
#include "tokenizer.h"
#include "dbg.h"
#include "cmd_loop.h"
#include "usart.h"
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "pwm.h"
#include "iwdg.h"
//------------------------------------------------------------------------------


//commands
SCmdEntry cmdEntries[] = {
    {"adc_read", handleAdcRead},
    {"set_max_outv", handleSetMaxOutv},
    {"print_state", handlePrintState},
    {"help", handleHelp},
    {"set_dbg", handleSetDbg},
    {"set_fixed_pulse", handleSetFixedPulse},
    {"set_freq", handleSetFreq},
    {"timer", handleTimer},
    {"toggle_enable", handleToggleEnable},
#ifdef STM32F446xx
    {"set_dac", handleSetDac},
#endif
    {(char*) NULL, NULL}
};


//-----------------------------------------------------------------------------

uint8_t
handleAdcRead(const char* inCmd) {
    UNUSED(inCmd);
    DBG(0, "adcOutVValue: %d\r\n", ADC_getOutVValue());
    DBG(0, "adcSolarInVValue: %d\r\n", ADC_getSolarInVValue());
    DBG(0, "adcGateVValue: %d\r\n", ADC_getGateVValue());
    return CMD_OK;
}

//-----------------------------------------------------------------------------
#ifdef STM32F446xx

uint8_t
handleSetDac(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 4))
        return CMD_ERR_INVALID_ARG;

    uint16_t value = atoi(tokenBuffer);
    if (value > 4096)
        return CMD_ERR_INVALID_ARG;

    DAC_setValue(value);

    return CMD_OK;
}

#endif

//-----------------------------------------------------------------------------

uint8_t
handleSetMaxOutv(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 4))
        return CMD_ERR_INVALID_ARG;

    uint16_t value = atoi(tokenBuffer);
    if (value > 4096)
        return CMD_ERR_INVALID_ARG;

    ADC_setMaxOutVValue(value);

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleHelp(const char* inCmd) {
    UNUSED(inCmd);
    DBG(0, "\r\n PoWer command list: \r\n\r\n");


    DBG(0, "$help#\r\n");
    DBG(0, "   description: prints this text\r\n");

    DBG(0, "$adc_read#\r\n");
    DBG(0, "   reads the current adc values (A, B)\r\n");


    DBG(0, "$set_max_outv:[uint16]#\r\n");
    DBG(0, "   sets the upper limit of channel A,"
        " after that limit PWM DC will be shortened\r\n");
    DBG(0, "$print_state#\r\n");
    DBG(0, "   prints current controller state\r\n");

    DBG(0, "$set_dbg:[uint8_t]#\r\n");
    DBG(0, "   sets the debugging level\r\n");

    DBG(0, "$set_fixed_pulse:[uint32_t]#\r\n");
    DBG(0, "   sets the fixed PWM pulse\r\n");

    DBG(0, "$set_freq:[uint32_t]#\r\n");
    DBG(0, "   sets the PWM carrier frequency\r\n");

    DBG(0, "$toggle_enable#\r\n");
    DBG(0, "   toggles IR2110 enable pin PA6\r\n");

#ifdef STM32F446xx
    DBG(0, "$set_dac:[uint16_t]#\r\n");
    DBG(0, "   sets stm32f446 DAC output, pin PA4 \r\n");
#endif

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handlePrintState(const char* inCmd) {
    DBG(0, "max outV: %d\r\n", ADC_getMaxOutVValue());
    handleAdcRead(inCmd);

    DBG(0, "freq:   %d\r\n", PWM_getFreq());
    DBG(0, "pulse: %d\r\n", PWM_getFixedPulse());

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleSetDbg(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 1))
        return CMD_ERR_INVALID_ARG;

    int verbosityLevel = atoi(tokenBuffer);

    DBG_set_verbosity(verbosityLevel);
    DBG(2, "setting verbosity %d\r\n", verbosityLevel);

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleSetFreq(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 6))
        return CMD_ERR_INVALID_ARG;

    uint32_t freq = atoi(tokenBuffer);

    if(freq < MIN_FREQ || freq > MAX_FREQ)
        return CMD_ERR_INVALID_ARG;

    PWM_setFreq(freq);

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleSetFixedPulse(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 4))
        return CMD_ERR_INVALID_ARG;

    int pulse = atoi(tokenBuffer);

    if(pulse < MIN_DUTY_CYCLE || pulse > MAX_DUTY_CYCLE)
        return CMD_ERR_INVALID_ARG;

    PWM_setFixedPulse(pulse);

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleTimer(const char* inCmd) {
    UNUSED(inCmd);
    static uint32_t lastTick250 = 0;
    static uint32_t lastTick100 = 0;

    uint32_t currentTick = HAL_GetTick();
    uint32_t elapsed250 = currentTick - lastTick250;
    if (currentTick < lastTick250) { //overflow
        elapsed250 = 0xFFFFFFFF - lastTick250 + currentTick;
    }

    uint32_t elapsed100 = currentTick - lastTick100;
    if (currentTick < lastTick100) { //overflow
        elapsed100 = 0xFFFFFFFF - lastTick100 + currentTick;
    }


    if (elapsed250 > 250) {
        uint16_t adcA = ADC_getOutVValue();
        uint16_t adcB = ADC_getSolarInVValue();
        uint16_t adcC = ADC_getSolarInVValue();

        DBG(3, "adcOutV: %*d  adcSolarInV: %*d adcGateV: %*d\r\n",
            5, adcA,
            5, adcB,
            5, adcC);
        lastTick250 += 250;
    }

    if (elapsed100 > 100) {
        IWDG_refresh();
        lastTick100 += 100;
    }

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleToggleEnable(const char* inCmd) {
    UNUSED(inCmd);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    return CMD_OK;
}
//-----------------------------------------------------------------------------
