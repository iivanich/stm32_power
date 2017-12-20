#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include "cmd_loop.h"
#include "dbg.h"
#include "tokenizer.h"

#include "usart.h"

static const char* errorCodes[] = {
        "CMD_OK",
        "CMD_ERR_NOT_ALLOWED",
        "CMD_ERR_INVALID_ARG",
        "CMD_ERR_PARSE",
        "CMD_ERR_INVALID_CMD",
        "CMD_ERR_INVALID_STATE"
};

uint8_t
handleCommand(char* inCmd);

//-----------------------------------------------------------------------------
void
cleanBackspaces(uint8_t* inBuffer, uint16_t* inSize) {
        for (int iCnter = 0; iCnter < *inSize; iCnter ++) {
                if (inBuffer[iCnter] == 0x08) {
                        if (iCnter > 0) {
                                if ((iCnter + 1) < *inSize) {
                                        memmove(inBuffer + iCnter - 1,
                                                inBuffer + iCnter + 1,
                                                *inSize - iCnter - 1);
                                }

                                *inSize -= 2;
                                iCnter -= 2;
                        } else {
                                if ((iCnter + 1) < *inSize) {
                                        memmove(inBuffer + iCnter,
                                                inBuffer + iCnter + 1,
                                                *inSize - iCnter - 1);
                                }
                                *inSize -= 1;
                                iCnter --;
                        }
                }
        }
}

//-----------------------------------------------------------------------------
bool
assembleCommand(uint8_t* cmdBuffer, uint16_t* cmdSize,
                uint8_t* inBuffer, uint16_t inSize) {
        uint16_t offset = 0;

        if ((*cmdSize) == 0) {
                while(offset < inSize
                      && inBuffer[offset] != '$'
                      && inBuffer[offset] != 'm'
                      && inBuffer[offset] != 'h'
                      && inBuffer[offset] != 'p'
                      && inBuffer[offset] != '/'
                      && inBuffer[offset] != '-'
                      && inBuffer[offset] != '+')
                        offset ++;

                if (offset == inSize) {
                        return false; //no start char found!
                }

                //assemble single key commands
                if(inBuffer[offset] != '$') {
                        cmdBuffer[*cmdSize] = '$';
                        (*cmdSize) ++;
                        cmdBuffer[(*cmdSize)] = inBuffer[offset];
                        (*cmdSize) ++;
                        cmdBuffer[*cmdSize] = ':';
                        (*cmdSize) ++;
                        cmdBuffer[*cmdSize] = '#';
                        (*cmdSize) ++;
                        cmdBuffer[*cmdSize] = 0; //null terminated string
                        (*cmdSize) ++;
                        //$x:#
                        return true;
                }
        }

        for(; offset < inSize; offset++) {
                cmdBuffer[(*cmdSize)] = inBuffer[offset];
                if(cmdBuffer[*cmdSize] == '#') {
                        cmdBuffer[*cmdSize] = 0; //null terminated string

                        (*cmdSize) ++;
                        return true;
                }

                (*cmdSize) ++;

                if ((*cmdSize) >= DRV_USART_BUFFER_SIZE) {
                        (*cmdSize) = 0;
                        return false; //no end cmd sym reached, reset, ignore rcv packet
                }
        }

        return false;
}

//-----------------------------------------------------------------------------
void
assembleUartOutputCommand(uint8_t* inBuffer, uint16_t inSize) {
        DBG(5, "assembleUartOutputCommand\r\n");
        static uint16_t cmdOutBufferSize = 0;
        static uint8_t cmdOutBuffer[DRV_USART_BUFFER_SIZE];

        if (assembleCommand(cmdOutBuffer, &cmdOutBufferSize, inBuffer, inSize)) {
                cleanBackspaces(cmdOutBuffer, &cmdOutBufferSize);
                DBG(1, "cmd assembled: %s\r\n", cmdOutBuffer);
                DBG_hexdump(4, cmdOutBuffer, cmdOutBufferSize, 8);
                DBG(0, "\r\n");

                uint8_t result = handleCommand((char*)cmdOutBuffer);
                if (result != CMD_OK)
                        DBG(1, "\r\n --> handleCommand returned error: %s \r\n",
                            errorCodes[result]);
                else
                        DBG(1, "\r\n --> OK \r\n");

                cmdOutBufferSize = 0;
        }
}

//-----------------------------------------------------------------------------
uint8_t
handleCommand(char* inCmd) {
        const char* params = getToken(inCmd + 1, ':');

        DBG(4, "handleCommand: %s\r\n", tokenBuffer);

        for (int iCnter = 0;; iCnter ++) {
                if (cmdEntries[iCnter].cmdName == NULL)
                        break;

                if (strcmp(cmdEntries[iCnter].cmdName, (const char*)tokenBuffer) == 0) {
                        return cmdEntries[iCnter].handler(params);
                }
        }

        return CMD_ERR_INVALID_CMD;
}

//-----------------------------------------------------------------------------
void
readEepromState() {
        for (int iCnter = 0;; iCnter ++) {
                if (cmdEntries[iCnter].cmdName == NULL)
                        break;

                if (strcmp(cmdEntries[iCnter].cmdName, "flash_read") == 0) {
                        cmdEntries[iCnter].handler("$flash_read#");
                        break;
                }
        }
}

//-----------------------------------------------------------------------------
void
mainLoop() {
        readEepromState();

        DBG(1, "Starting PoWer ...\r\n");

        //find timer handler
        uint8_t (*handleTimerPtr)(const char* inCmd) = NULL;
        for (int iCnter = 0;; iCnter ++) {
                if (cmdEntries[iCnter].cmdName == NULL)
                        break;

                if (strcmp(cmdEntries[iCnter].cmdName, "timer") == 0) {
                        handleTimerPtr = cmdEntries[iCnter].handler;
                        break;
                }
        }

        DBG(1, "Entering CmdLoop ...\r\n");

        while (true) {
                __disable_irq();
                uint8_t buffer[32];
                uint16_t dataSize =
                        DrvUsartRead(dbgCom, buffer, 32);
                if (dataSize > 0) {
                        DBG(5, "%c", buffer[0]);
                        assembleUartOutputCommand(buffer, dataSize);
                }

                if (handleTimerPtr != NULL) {
                        static uint32_t lastTick = 0;
                        if(HAL_GetTick() != lastTick) {
                                lastTick = HAL_GetTick();
                                handleTimerPtr("$timer#");
                        }
                }
                __enable_irq();
                __WFI();
        }

}
//-----------------------------------------------------------------------------
