#ifndef __svpwm_H
#define __svpwm_H

#include "stdint.h"

#define a 0
#define b 1
#define c 2

void uvector_state(uint16_t angle, uint16_t  amplitude, uint16_t* PWM, uint16_t pwm_limit, uint8_t direction);

#endif // __svpwm_H
