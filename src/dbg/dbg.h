#ifndef __USB_DEBUG_H__
#define __USB_DEBUG_H__

//#include "main.h"

void DBG_set_verbosity(int verbosity);
void DBG(int verbosity, const char *format, ...);
void DBG_verbose(const char *format, ...);
void DBG_hexdump(int verbosity, const uint8_t *data, int size, int lbreak);
void DBG_asciidump(int verbosity, const uint8_t *data, int size, int lbreak);

extern uint8_t dbgCom;
#endif