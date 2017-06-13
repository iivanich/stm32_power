#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <stdbool.h>

bool writeEepromBytes(uint32_t address, uint8_t* data, uint16_t length);
void readEepromBytes(uint32_t address, uint8_t* data, uint16_t length);

#endif
