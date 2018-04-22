#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "messages.h"
#include "brushless_motor.h"

void parse_package(BrushlessMotor *BLDC, uint8_t *message, uint8_t length);
void send_package(BrushlessMotor *BLDC);

#endif //__COMMUNICATION_H
