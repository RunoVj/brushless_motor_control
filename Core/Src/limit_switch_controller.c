#include "limit_switch_controller.h"
#include "gpio.h"

LimitSwitchController lim_sw_ctrl;

#define LIMIT_SWITCH1_Pin GPIO_PIN_6
#define LIMIT_SWITCH1_GPIO_Port GPIOA

void update_lim_sw_states(LimitSwitchController *lim_sw_ctrl)
{
	lim_sw_ctrl->lim_switch1_state = HAL_GPIO_ReadPin(LIMIT_SWITCH1_GPIO_Port, LIMIT_SWITCH1_Pin);
	lim_sw_ctrl->lim_switch2_state = HAL_GPIO_ReadPin(LIMIT_SWITCH2_GPIO_Port, LIMIT_SWITCH2_Pin);
}

