#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"

#include "eeprom.h"
#include "dbg.h"

//-----------------------------------------------------------------------------
bool eraseEepromArea() {
        bool bRet = true;

        /* Unlock the Flash to enable the flash control register access *************/
        if (HAL_FLASH_Unlock() != HAL_OK) {
                DBG(0, "FLASH UNLOCK ERROR!\r\n");
                bRet = false;
        } else {
                /* Erase the user Flash area
                   (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

                FLASH_EraseInitTypeDef EraseInitStruct;
                uint32_t pageError = 0;
                /* Fill EraseInit structure*/
                EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
                EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
                EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)
                        / FLASH_PAGE_SIZE;

                if (HAL_FLASHEx_Erase(&EraseInitStruct, &pageError) != HAL_OK) {
                        DBG(0, "FLASH ERASE ERROR: %d!\r\n", pageError);
                        bRet = false;
                }
        }

        /* Lock the Flash to disable the flash control register access (recommended
           to protect the FLASH memory against possible unwanted operation) *********/
        if(HAL_FLASH_Lock() != HAL_OK) {
                DBG(0, "FLASH LOCK ERROR!\r\n");
                bRet = false;
        }

        return bRet;
}

//-----------------------------------------------------------------------------
bool writeEepromBytes(uint32_t address, uint32_t* data, uint16_t length) {
        bool bRet = true;
        address *= 4;

        if (address >= FLASH_USER_END_ADDR - FLASH_USER_START_ADDR
            || address + length * 4 >= FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)
                return false;

        /* Unlock the Flash to enable the flash control register access *************/
        if (HAL_FLASH_Unlock() != HAL_OK) {
                DBG(0, "FLASH UNLOCK ERROR!\r\n");
                bRet = false;
        } else {
                for (int iCnter = 0; iCnter < length; iCnter ++) {
                        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                              FLASH_USER_START_ADDR + address + iCnter * 4,
                                              data[iCnter]) != HAL_OK) {
                                DBG(0, "FLASH WRITE ERROR!\r\n");
                                bRet = false;
                                break;
                        }
                }
        }

        /* Lock the Flash to disable the flash control register access (recommended
           to protect the FLASH memory against possible unwanted operation) *********/
        if(HAL_FLASH_Lock() != HAL_OK) {
                DBG(0, "FLASH LOCK ERROR!\r\n");
                bRet = false;
        }

        return bRet;
}

//-----------------------------------------------------------------------------
bool readEepromBytes(uint32_t address, uint32_t* data, uint16_t length) {
        address *= 4;

        if (address >= FLASH_USER_END_ADDR - FLASH_USER_START_ADDR
            || address + length * 4 >= FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)
                return false;

        for (int iCnter = 0; iCnter < length; iCnter ++) {
                data[iCnter] = *((__IO uint32_t*)(FLASH_USER_START_ADDR + address + iCnter * 4));
        }

        return true;
}

//-----------------------------------------------------------------------------
