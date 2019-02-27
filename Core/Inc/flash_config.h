#ifndef __flash_config_H
#define __flash_config_H

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "limit_switch_controller.h"
#include "bootloader.h"

#pragma pack(push, 1)

// size of struct mast be a multiple of 4 byte (32 bits)
typedef struct { 
	uint8_t address;
	uint8_t update_firmware;
} LimitSwitchControllerConfig;

#pragma pack(pop)

#define SETTINGS_WORDS sizeof(LimitSwitchControllerConfig)/4
	
void FLASH_ReadSettings(LimitSwitchController *lim_sw_ctrl);
void FLASH_WriteSettings(LimitSwitchController *lim_sw_ctrl, bool update_firmware);

#endif // __flash_config_H
