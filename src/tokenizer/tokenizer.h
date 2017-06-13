#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdint.h>
#include <stdbool.h>

#define TOKEN_BUFFER_SIZE 256

extern char tokenBuffer[TOKEN_BUFFER_SIZE];

//-----------------------------------------------------------------------------
char
*_ulltoa (unsigned long long value, char *string, int radix);

//-----------------------------------------------------------------------------
bool
isValidInt(char* str, int maxLen);

//-----------------------------------------------------------------------------
const char*
getToken(const char* start, char delim);

//-----------------------------------------------------------------------------
uint16_t
asciiHexToBin(const char* start, uint8_t* buffer);

//-----------------------------------------------------------------------------

#endif
