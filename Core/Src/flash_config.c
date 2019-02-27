#include "flash_config.h"       

void FLASH_ReadSettings(LimitSwitchController *lim_sw_ctrl)
{
	LimitSwitchControllerConfig config;
	uint32_t *source_addr = (uint32_t *)CONFIG_PAGE_ADDR;
	uint32_t *dest_addr = (void *)&config;
	
	for (uint16_t i = 0; i < SETTINGS_WORDS; ++i) {
		*dest_addr = *(__IO uint32_t*)source_addr;
		source_addr++;
		dest_addr++;
	}
	
	lim_sw_ctrl->address = config.address;
}

void FLASH_WriteSettings(LimitSwitchController *lim_sw_ctrl, bool update_firmware)
{
	LimitSwitchControllerConfig config;
	config.address = lim_sw_ctrl->address;
	config.update_firmware = update_firmware;
	

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
