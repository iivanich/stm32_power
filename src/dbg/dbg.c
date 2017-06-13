#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "dbg.h"

#include "usart.h"

static int verbosityLevel;

uint8_t dbgCom = -1;

#define DBG_SEND_DATA(buf,len) DrvUsartWriteBlocking(dbgCom, buf, len);

//-----------------------------------------------------------------------------
void
DBG_set_verbosity(int verbosity) {
        verbosityLevel = verbosity;
}

//-----------------------------------------------------------------------------
void
DBG(int verbosity, const char *format, ...) {
        if(verbosity <= verbosityLevel) {
                char formatted[256];
                va_list args;
                va_start(args, format);
                vsprintf(formatted, format, args);
                va_end(args);
                        DBG_SEND_DATA((uint8_t*)formatted, strlen(formatted));
        }
}

//-----------------------------------------------------------------------------
void
DBG_verbose(const char *format, ...) {
        char formatted[256];
        va_list args;
        va_start(args, format);
        vsprintf(formatted, format, args);
        va_end(args);

        DBG_SEND_DATA((uint8_t*)formatted, strlen(formatted));
        DBG_SEND_DATA((uint8_t*)"\r\n", strlen("\r\n"));
}

//-----------------------------------------------------------------------------

bool
isReadableASCIIChar(char c) {
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
                return true;

        return false;
}

//-----------------------------------------------------------------------------
void
DBG_asciidump(int verbosity, const uint8_t *data, int size, int lbreak) {
        uint16_t iCnter = 0;

        for (; iCnter < size; iCnter ++) {
                if(iCnter > 0 && iCnter % lbreak == 0)
                        DBG(verbosity, "\r\n");

                if (isReadableASCIIChar(data[iCnter]))
                        DBG(verbosity, "%c", data[iCnter]);
                else
                        DBG(verbosity, ".");
        }

        if((iCnter - 1) % lbreak != 0)
                DBG(verbosity, "\r\n");
}

//-----------------------------------------------------------------------------
void
DBG_hexdump(int verbosity, const uint8_t *data, int size, int lbreak) {
        uint16_t iCnter = 0;
        for (; iCnter < size; iCnter ++) {
                if(iCnter > 0 && iCnter % lbreak == 0)
                        DBG(verbosity, "\r\n");

                DBG(verbosity, "%02x,", data[iCnter]);
        }

        if((iCnter - 1) % lbreak != 0)
                DBG(verbosity, "\r\n");
}

//-----------------------------------------------------------------------------
