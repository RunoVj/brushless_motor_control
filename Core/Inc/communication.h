#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "messages.h"
#include "brushless_motor.h"

bool parse_package(BrushlessMotor *BLDC, uint8_t *message, uint8_t length);
void normal_response(BrushlessMotor *BLDC);
void terminal_response(BrushlessMotor *BLDC);

#endif //__COMMUNICATION_H
