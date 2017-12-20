/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H

#define  PERIOD_VALUE       (uint32_t)(100)     /* Period Value  */
#define  SIN_FREQ           (uint32_t)(1)
#define  DEAD_TIME          (uint16_t)(60)
#define  MAX_DUTY_CYCLE     75
#define  MIN_DUTY_CYCLE     2

#define  MAX_FREQ     200000
#define  MIN_FREQ     50000

//------------------------------------------------------------------------------
void PWM_initAndStart();

//------------------------------------------------------------------------------
bool PWM_setFixedPulse(uint16_t pulse);

//------------------------------------------------------------------------------
uint16_t PWM_getFixedPulse();

//------------------------------------------------------------------------------
bool PWM_setFreq(uint32_t freq);

//------------------------------------------------------------------------------
uint32_t PWM_getFreq();

//------------------------------------------------------------------------------

#endif
