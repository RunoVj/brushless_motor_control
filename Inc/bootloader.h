#ifndef __bootloader_H
#define __bootloader_H

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "flash_config.h"

#define CONFIG_PAGE_NUMB 31
#define CONFIG_PAGE_ADDR 0x08007C00
#define BOOTLOADER_ADDR  0x08000000
#define APPLICATION_ADDR 0x08002800  // 10 kB for bootloader
#define FLASH_END_ADDR   0x08008000


void jump(uint32_t program_start_addr);
void update_firmware(void);
void shift_interrupt_vectors(uint32_t program_start_addr);

#endif // __bootloader_H
