#include <string.h>
#include "tokenizer.h"
#include "dbg.h"

char tokenBuffer[TOKEN_BUFFER_SIZE];

//-----------------------------------------------------------------------------
char*
_ulltoa (unsigned long long value, char *string, int radix) {
        char *dst;
        char digits[64];
        int i, n;

        dst = string;
        if (radix < 2 || radix > 36) {
                *dst = 0;
                return (string);
        }

        i = 0;

        do {
                n = value % radix;
                digits[i++] = (n < 10 ? (char)n+'0' : (char)n-10+'a');
                value /= radix;
        } while (value != 0);

        while (i > 0)
          *dst++ = digits[--i];

        *dst = 0;
        return (string);
}

//-----------------------------------------------------------------------------
bool
isValidInt(char* str, int maxLen) {
        for(int iCnter = 0; iCnter <= maxLen; iCnter ++) {
                if (str[iCnter] == 0 && iCnter > 0)
                        return true;

                if (str[iCnter] < '0' || str[iCnter] > '9') {
                        DBG(0, "Invalid int: %s\r\n", str);
                        return false;
                }
        }

        DBG(0, "Invalid int: %s\r\n", str);

        return false;
}


//-----------------------------------------------------------------------------
const char*
getToken(const char* start, char delim) {
        uint16_t iCnter = 0;
        for (; start[iCnter] != 0; iCnter ++) {
                if (start[iCnter] == delim || start[iCnter] == '#') {
                        tokenBuffer[iCnter] = 0;
                        return (start + iCnter + 1);
                }

                if (iCnter >= (TOKEN_BUFFER_SIZE - 1)) {
                        DBG(0, "getToken out of bounds!");
                        while(1); //stop
                } else
                        tokenBuffer[iCnter] = start[iCnter];
        }

        tokenBuffer[iCnter] = 0;
        return (start + iCnter + 1);
}

//-----------------------------------------------------------------------------
uint16_t
asciiHexToBin(const char* start, uint8_t* buffer) {
        int iCnter = 0;
        DBG(4, "asciiHexToBin: %s ", start);

        for (;;iCnter +=3) {
                char symbol[3];
                getToken(start + iCnter, ',');
                if (strlen(tokenBuffer) != 2) break;

                strncpy(symbol, tokenBuffer, 3);

                *(buffer + iCnter / 3) = 0;

                if (symbol[0] >= '0' && symbol[0] <= '9')
                        *(buffer + iCnter / 3) += 16 * (symbol[0] - '0');
                else if (symbol[0] >= 'a' && symbol[0] <= 'f')
                        *(buffer + iCnter / 3) += 16 * (10 + (symbol[0] - 'a'));
                else if (symbol[0] >= 'A' && symbol[0] <= 'F')
                        *(buffer + iCnter / 3) += 16 * (10 + (symbol[0] - 'A'));


                if (symbol[1] >= '0' && symbol[1] <= '9')
                        *(buffer + iCnter / 3) += symbol[1] - '0';
                else if (symbol[1] >= 'a' && symbol[1] <= 'f')
                        *(buffer + iCnter / 3) += 10 + (symbol[1] - 'a');
                else if (symbol[1] >= 'A' && symbol[1] <= 'F')
                        *(buffer + iCnter / 3) += 10 + (symbol[1] - 'A');
        }

        DBG(4, ":  %d \r\n", iCnter / 3);
        return iCnter / 3;
}

//-----------------------------------------------------------------------------
