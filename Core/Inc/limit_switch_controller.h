#ifndef __limit_switch_controller_H
#define __limit_switch_controller_H

#include "stdint.h"
#include "stdbool.h"


typedef struct {
	uint8_t address;
	bool lim_switch1_state;
	bool lim_switch2_state;

} LimitSwitchController;


extern LimitSwitchController lim_sw_ctrl;
void update_lim_sw_states(LimitSwitchController *lim_sw_ctrl);

#endif /*__limit_switch_controller_H */
