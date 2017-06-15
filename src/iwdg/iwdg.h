#ifndef __IWDG_H__
#define __IWDG_H__

#include <stdint.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
void
IWDG_refresh();

//------------------------------------------------------------------------------
void
IWDG_initAndStart(uint32_t periodMs);

#endif
