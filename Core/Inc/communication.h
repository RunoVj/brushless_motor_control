#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "messages.h"
#include "limit_switch_controller.h"

bool parse_package(LimitSwitchController *lim_sw_ctrl, uint8_t *message, uint8_t length);
void normal_response(LimitSwitchController *lim_sw_ctrl);
void terminal_response(LimitSwitchController *lim_sw_ctrl);
void device_response(LimitSwitchController *lim_sw_ctrl);

#endif //__COMMUNICATION_H
