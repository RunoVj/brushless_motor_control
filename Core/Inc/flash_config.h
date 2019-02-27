#ifndef __flash_config_H
#define __flash_config_H

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "limit_switch_controller.h"
#include "bootloader.h"

#define MAX_BASE_VECTORS_NUMB 8

#pragma pack(push, 1)

// size of struct mast be a multiple of 4 byte (32 bits)
typedef struct { 
	uint8_t address;
	uint8_t update_firmware;
	
	// trash, used only in thrusters
	uint16_t base_vectors[MAX_BASE_VECTORS_NUMB]; // 8*2 byte
	uint16_t high_impulse_current_threshold;
	uint16_t low_impulse_current_threshold;
	uint16_t average_current_threshold;
	
} LimitSwitchControllerConfig;

#pragma pack(pop)

#define SETTINGS_WORDS sizeof(LimitSwitchControllerConfig)/4
	
void FLASH_ReadSettings(LimitSwitchController *lim_sw_ctrl);
void FLASH_WriteSettings(LimitSwitchController *lim_sw_ctrl, bool update_firmware);

#endif // __flash_config_H
