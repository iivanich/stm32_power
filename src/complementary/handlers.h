#ifndef __HANDLERS_H__
#define __HANDLERS_H__

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
handleAdcRead(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleSetDac(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleSetMaxOutv(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleSetMinInv(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleHelp(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handlePrintState(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleSetDbg(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleSetFixedPulse(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleSetFreq(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleTimer(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleToggleEnable(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handlePlus(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleMinus(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleToggleManual(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleSetStep(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleChangeMode(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleFlashRead(const char* inCmd);

//-----------------------------------------------------------------------------
uint8_t
handleFlashWrite(const char* inCmd);

//-----------------------------------------------------------------------------

#endif
