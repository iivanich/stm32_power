#include "stm32l1xx_flash.h"
#include "eeprom.h"
#include "dbg.h"


bool writeEepromBytes(uint32_t address, uint8_t* data, uint16_t length) {
        FLASH_Status status;
        address = address + 0x08080000;

        DATA_EEPROM_Unlock(); //Unprotect the EEPROM to allow writing

        for (int iCnter = 0; iCnter < length; iCnter ++) {
                status = DATA_EEPROM_ProgramByte(address + iCnter, data[iCnter]);
                if (status != FLASH_COMPLETE) {
                        DBG(0, "FLASH_ERROR: %d\r\n", status);
                        break;
                }
        }

        DATA_EEPROM_Lock();   // Reprotect the EEPROM

        return (status == FLASH_COMPLETE);
}

void readEepromBytes(uint32_t address, uint8_t* data, uint16_t length) {
        address = address + 0x08080000;
        for (int iCnter = 0; iCnter < length; iCnter ++) {
                data[iCnter] = *((__IO uint8_t*)(address + iCnter));
        }
}
