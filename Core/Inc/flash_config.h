#ifndef __flash_config_H
#define __flash_config_H

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "brushless_motor.h"
#include "bootloader.h"

#pragma pack(push, 1)

// size of struct mast be a multiple of 4 byte (32 bits)
typedef struct { 
	uint8_t address;
	uint8_t update_firmware;
	uint16_t base_vectors[MAX_BASE_VECTORS_NUMB]; // 8*2 byte
	uint16_t high_impulse_current_threshold;
	uint16_t low_impulse_current_threshold;
	uint16_t average_current_threshold;
	
} BLDCConfig;

typedef struct {
	uint16_t speed_k[2];
} BLDCAdditionalConfig;

#pragma pack(pop)

#define SETTINGS_WORDS sizeof(BLDCConfig)/4
#define ADDITIONAL_SETTINGS_WORDS sizeof(BLDCAdditionalConfig)/4
	
void FLASH_ReadSettings(BrushlessMotor *BLDC);
void FLASH_WriteSettings(BrushlessMotor *BLDC, bool update_firmware);

#endif // __flash_config_H
