#include "flash_config.h"       

void FLASH_ReadSettings(BrushlessMotor *BLDC)
{
	BLDCConfig config;
	uint32_t *source_addr = (uint32_t *)CONFIG_PAGE_ADDR;
	uint32_t *dest_addr = (void *)&config;
	
	for (uint16_t i = 0; i < SETTINGS_WORDS; ++i) {
		*dest_addr = *(__IO uint32_t*)source_addr;
		source_addr++;
		dest_addr++;
	}
	
	BLDC->address = config.address;
	for (uint8_t i = 0; i < MAX_BASE_VECTORS_NUMB; ++i) {
		BLDC->base_vectors[i] = config.base_vectors[i];
	}
}

void FLASH_WriteSettings(BrushlessMotor *BLDC)
{
	BLDCConfig config;
	config.address = BLDC->address;
	for (uint8_t i = 0; i < MAX_BASE_VECTORS_NUMB; ++i) {
		config.base_vectors[i] = BLDC->base_vectors[i];
	}

	// Write settings
	HAL_FLASH_Unlock();

	// errase page
	FLASH_EraseInitTypeDef errase_conf;
	errase_conf.TypeErase = FLASH_TYPEERASE_PAGES; // errase only 1 page
	errase_conf.PageAddress = (uint32_t)CONFIG_PAGE_ADDR;
	errase_conf.NbPages = 1;
	
	uint32_t page_error;
	HAL_FLASHEx_Erase(&errase_conf, &page_error);
	
	// write page
	uint32_t *source_addr = (void *)&config;
	uint32_t *dest_addr = (uint32_t *)CONFIG_PAGE_ADDR;
	for (uint8_t i = 0; i < SETTINGS_WORDS; ++i) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)dest_addr, *source_addr);
		source_addr++;
		dest_addr++;
	}
	
	HAL_FLASH_Lock();
}
