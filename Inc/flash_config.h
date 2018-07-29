#ifndef __flash_config_H
#define __flash_config_H

#define CONFIG_PAGE_NUMB 127
#define CONFIG_PAGE_ADDR 0x0801FC00

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "brushless_motor.h"

#pragma pack(push, 1)

// size of struct mast be a multiple of 4 byte (32 bits)
typedef struct { 
	uint8_t address;
	uint8_t nothing1;
	uint16_t base_vectors[MAX_BASE_VECTORS_NUMB]; // 8*2 byte
	uint16_t nothing2;
	
} BLDCConfig;

#pragma pack(pop)

#define SETTINGS_WORDS sizeof(BLDCConfig)/4
	
void FLASH_ReadSettings(BrushlessMotor *BLDC);
void FLASH_WriteSettings(BrushlessMotor *BLDC);

#endif // __flash_config_H
