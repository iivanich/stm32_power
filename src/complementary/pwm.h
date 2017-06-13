/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H

#define  PERIOD_VALUE       (uint32_t)(500)     /* Period Value  */
#define  SIN_FREQ           (uint32_t)(1)
#define  DEAD_TIME          (uint16_t)(50)
#define  MAX_DUTY_CYCLE     400
#define  MIN_DUTY_CYCLE     10

#define  MAX_FREQ     200000
#define  MIN_FREQ     1

//------------------------------------------------------------------------------
void PWM_initAndStart();

//------------------------------------------------------------------------------
void PWM_setFixedPulse(uint16_t pulse);

//------------------------------------------------------------------------------
uint16_t PWM_getFixedPulse();

//------------------------------------------------------------------------------
void PWM_setFreq(uint32_t freq);

//------------------------------------------------------------------------------
uint16_t PWM_getFreq();

//------------------------------------------------------------------------------

#endif
