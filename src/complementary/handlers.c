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
#include "eeprom.h"
//------------------------------------------------------------------------------

#define CHANGE_DEBUG 0
#define CHANGE_FREQ 1
#define CHANGE_PULSE 2

static uint16_t change_step = 1;
static uint8_t  change_mode = CHANGE_DEBUG;

//commands
SCmdEntry cmdEntries[] = {
    {"adc_read", handleAdcRead},
    {"set_max_outv", handleSetMaxOutv},
    {"set_min_inv", handleSetMinInv},
    {"print_state", handlePrintState},
    {"help", handleHelp},
    {"set_dbg", handleSetDbg},
    {"set_fixed_pulse", handleSetFixedPulse},
    {"set_freq", handleSetFreq},
    {"timer", handleTimer},
    {"toggle_enable", handleToggleEnable},
    {"toggle_manual", handleToggleManual},
    {"flash_read", handleFlashRead},
    {"flash_write", handleFlashWrite},
    {"+", handlePlus},
    {"-", handleMinus},
//    {"/", handleToggleEnable},
    {"p", handlePrintState},
    {"h", handleHelp},
//    {"m", handleToggleManual},
    {"change_mode", handleChangeMode},
    {"set_step", handleSetStep},
#ifdef STM32F446xx
    {"set_dac", handleSetDac},
#endif
    {(char*) NULL, NULL}
};


//-----------------------------------------------------------------------------
uint8_t
handleFlashRead(const char* inCmd) {
    UNUSED(inCmd);
    DBG(1, "Reading FLASH...\r\n");

    uint32_t data = 0;
    readEepromBytes(0, &data, 1);
    if (data != FLASH_MAGICK) {
        DBG(1, "flash magick not valid, skip reading...: %d\r\n", data);
        return CMD_ERR_NOT_ALLOWED;
    }

    readEepromBytes(1, &data, 1);
    DBG(1, "frequency: %d\r\n", data);
    PWM_setFreq(data);

    readEepromBytes(2, &data, 1);
    DBG(1, "maxOutV: %d\r\n", data);
    ADC_setMaxOutVValue(data);

    readEepromBytes(3, &data, 1);
    DBG(1, "minInvV: %d\r\n", data);
    ADC_setMinSolarInVValue(data);

    readEepromBytes(4, &data, 1);
    DBG(1, "manualMode: %d\r\n", data);
    ADC_setManualMode(data);

    readEepromBytes(5, &data, 1);
    DBG(1, "changeMode: %d\r\n", data);
    if (data <= 2)
        change_mode = data;

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleFlashWrite(const char* inCmd) {
    UNUSED(inCmd);
    DBG(1, "Writing FLASH...\r\n");
    eraseEepromArea();

    uint32_t data = FLASH_MAGICK;
    writeEepromBytes(0, &data, 1);
    DBG(1, "magick: %d\r\n", data);

    data = PWM_getFreq();
    writeEepromBytes(1, &data, 1);
    DBG(1, "frequency: %d\r\n", data);

    data = ADC_getMaxOutVValue();
    writeEepromBytes(2, &data, 1);
    DBG(1, "maxOutV: %d\r\n", data);

    data = ADC_getMinSolarInVValue();
    writeEepromBytes(3, &data, 1);
    DBG(1, "minInV: %d\r\n", data);

    data = ADC_getManualMode();
    writeEepromBytes(4, &data, 1);
    DBG(1, "manualMode: %d\r\n", data);

    data = change_mode;
    writeEepromBytes(5, &data, 1);
    DBG(1, "changeMode: %d\r\n", data);

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleAdcRead(const char* inCmd) {
    UNUSED(inCmd);
    DBG(0, "adcOutVValue:     %d\r\n", ADC_getOutVAvg());
    DBG(0, "adcSolarInVValue: %d\r\n", ADC_getSolarInVAvg());
    DBG(0, "adcGateVValue:    %d\r\n", ADC_getGateVAvg());
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
handleSetMinInv(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 4))
        return CMD_ERR_INVALID_ARG;

    uint16_t value = atoi(tokenBuffer);
    if (value > 4096)
        return CMD_ERR_INVALID_ARG;

    ADC_setMinSolarInVValue(value);

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

    DBG(0, "$toggle_manual#\r\n");
    DBG(0, "   toggles manual mode\r\n");

    DBG(0, "$set_step:[uint16_t]#\r\n");
    DBG(0, "   set the +- step of change\r\n");

    DBG(0, "$change_mode:[uint8_t]#\r\n");
    DBG(0, "   set the +- mode of change: 0 - DEBUG, 1 - FREQ, 2 - PULSE\r\n");

    DBG(0, "single key commands: +, -, /, p, h, m\r\n");
    DBG(0, "   + : increase value depending on change mode\r\n");
    DBG(0, "   - : decrease value depending on change mode\r\n");
    DBG(0, "   / : toggle enable\r\n");
    DBG(0, "   p : print state\r\n");
    DBG(0, "   h : help\r\n");
    DBG(0, "   m : toggle manual mode\r\n");

#ifdef STM32F446xx
    DBG(0, "$set_dac:[uint16_t]#\r\n");
    DBG(0, "   sets stm32f446 DAC output, pin PA4 \r\n");
#endif

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handlePrintState(const char* inCmd) {
    DBG(0, "max outV:    %d\r\n", ADC_getMaxOutVValue());
    DBG(0, "--------------------------------------------------------------------------------\r\n");
    DBG(0, "min inV:     %d\r\n", ADC_getMinSolarInVValue());
    DBG(0, "--------------------------------------------------------------------------------\r\n");

    handleAdcRead(inCmd);

    DBG(0, "--------------------------------------------------------------------------------\r\n");
    DBG(0, "freq:        %d\r\n", PWM_getFreq());
    DBG(0, "--------------------------------------------------------------------------------\r\n");
    DBG(0, "pulse:       %d\r\n", PWM_getFixedPulse());
    DBG(0, "--------------------------------------------------------------------------------\r\n");
    DBG(0, "manual:      %d\r\n", (int)ADC_getManualMode());
    DBG(0, "--------------------------------------------------------------------------------\r\n");
    DBG(0, "pwm enabled: %d\r\n", (int) (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET));
    DBG(0, "--------------------------------------------------------------------------------\r\n");

    switch (change_mode) {
    case CHANGE_DEBUG:
        DBG(0, "change mode: DEBUG\r\n");
        break;

    case CHANGE_FREQ:
        DBG(0, "change mode: FREQUENCY\r\n");
        break;

    case CHANGE_PULSE:
        DBG(0, "change mode: PULSE\r\n");
        break;
    }

    DBG(0, "--------------------------------------------------------------------------------\r\n");
    DBG(0, "change step: %d\r\n", change_step);
    DBG(0, "--------------------------------------------------------------------------------\r\n");

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleSetDbg(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 1))
        return CMD_ERR_INVALID_ARG;

    int verbosityLevel = atoi(tokenBuffer);

    DBG_setVerbosity(verbosityLevel);
    DBG(2, "setting verbosity %d\r\n", verbosityLevel);

    return CMD_OK;
    }

//-----------------------------------------------------------------------------
uint8_t
handlePlus(const char* inCmd) {
    UNUSED(inCmd);
    switch (change_mode) {
    case CHANGE_DEBUG: {
        if (DBG_setVerbosity(DBG_getVerbosity() + 1)) {
            DBG(1, "setting verbosity %d\r\n", DBG_getVerbosity());
            return CMD_OK;
        }
        break;
    }

    case CHANGE_FREQ: {
        if (PWM_setFreq(PWM_getFreq() + change_step)) {
            DBG(1, "setting freq to %d\r\n", PWM_getFreq());
            return CMD_OK;
        }
        break;
    }

    case CHANGE_PULSE: {
        if (PWM_setFixedPulse(PWM_getFixedPulse() + change_step)) {
            DBG(1, "setting pulse to %d\r\n", PWM_getFixedPulse());
            return CMD_OK;
        }
        break;
    }
    }

    return CMD_ERR_INVALID_ARG;
}

//-----------------------------------------------------------------------------
uint8_t
handleMinus(const char* inCmd) {
    UNUSED(inCmd);
    switch (change_mode) {
    case CHANGE_DEBUG: {
        if (DBG_setVerbosity(DBG_getVerbosity() - 1)) {
            DBG(1, "setting verbosity %d\r\n", DBG_getVerbosity());
            return CMD_OK;
        }
        break;
    }

    case CHANGE_FREQ: {
        if (PWM_setFreq(PWM_getFreq() - change_step)) {
            DBG(1, "setting freq to %d\r\n", PWM_getFreq());
            return CMD_OK;
        }
        break;
    }

    case CHANGE_PULSE: {
        if (PWM_setFixedPulse(PWM_getFixedPulse() - change_step)) {
            DBG(1, "setting pulse to %d\r\n", PWM_getFixedPulse());
            return CMD_OK;
        }
        break;
    }
    }

    return CMD_ERR_INVALID_ARG;
}

//-----------------------------------------------------------------------------
uint8_t
handleSetFreq(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 6))
        return CMD_ERR_INVALID_ARG;

    uint32_t freq = atoi(tokenBuffer);

    if (!PWM_setFreq(freq))
        return CMD_ERR_INVALID_ARG;

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleSetFixedPulse(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 4))
        return CMD_ERR_INVALID_ARG;

    int pulse = atoi(tokenBuffer);

    if(!PWM_setFixedPulse(pulse))
        return CMD_ERR_INVALID_ARG;

    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleTimer(const char* inCmd) {
    UNUSED(inCmd);
    static uint32_t lastTick250 = 0;
    static uint32_t lastTick100 = 0;
    static uint32_t lastTick2000 = 0;

    uint32_t currentTick = HAL_GetTick();

    uint32_t elapsed250 = currentTick - lastTick250;
    if (currentTick < lastTick250) { //overflow
        elapsed250 = 0xFFFFFFFF - lastTick250 + currentTick;
    }

    uint32_t elapsed100 = currentTick - lastTick100;
    if (currentTick < lastTick100) { //overflow
        elapsed100 = 0xFFFFFFFF - lastTick100 + currentTick;
    }

    uint32_t elapsed2000 = currentTick - lastTick2000;
    if (currentTick < lastTick2000) { //overflow
        elapsed2000 = 0xFFFFFFFF - lastTick2000 + currentTick;
    }

    if (elapsed250 > 250) {
        uint16_t adcA = ADC_getOutVAvg();
        uint16_t adcB = ADC_getSolarInVAvg();
        uint16_t adcC = ADC_getGateVAvg();

        DBG(3, "adcOutV: %*d  adcSolarInV: %*d adcGateV: %*d pulse: %*d\r\n",
            5, adcA,
            5, adcB,
            5, adcC,
            5, PWM_getFixedPulse());
        lastTick250 += 250;
    }

    if (elapsed100 > 100) {
        IWDG_refresh();
        lastTick100 += 100;
    }

    static bool ledOn = false;
    if (elapsed2000 > 2000) {
        /* Turn LED2 on: Transfer process is correct */
        if (!ledOn) {
            //BSP_LED_On(LED2);
            ledOn = true;
        }
        lastTick2000 += 2000;
    } else if (ledOn && elapsed2000 / 4 > PWM_getFixedPulse()) {
        //BSP_LED_Off(LED2);
        ledOn = false;
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
uint8_t
handleToggleManual(const char* inCmd) {
    UNUSED(inCmd);
    ADC_setManualMode(!ADC_getManualMode());
    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleChangeMode(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 1))
        return CMD_ERR_INVALID_ARG;

    uint8_t mode = atoi(tokenBuffer);
    if (mode > 2)
        return CMD_ERR_INVALID_ARG;

    change_mode = mode;
    change_step = 1; //safety
    return CMD_OK;
}

//-----------------------------------------------------------------------------
uint8_t
handleSetStep(const char* inCmd) {
    getToken(inCmd, ':');

    if(!isValidInt(tokenBuffer, 5))
        return CMD_ERR_INVALID_ARG;

    change_step = atoi(tokenBuffer);
    return CMD_OK;
}

//-----------------------------------------------------------------------------
