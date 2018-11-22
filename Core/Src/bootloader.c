#include "bootloader.h"

void jump(const uint32_t program_start_addr)
{
	// make functor
	typedef void(*func_ptr)(void); 
	func_ptr program_func;
	
	__disable_irq();
	
	uint32_t reset_addr = *( uint32_t*) (program_start_addr + 4);  // RESET address
	program_func = (func_ptr)reset_addr; 
	
	__set_MSP(*(__IO uint32_t*) program_start_addr); // set main stack pointer  
	
	// go to new program
	shift_interrupt_vectors(program_start_addr);
	program_func();		      	
}

void update_firmware()
{
	
}

void shift_interrupt_vectors(uint32_t program_start_addr)
{
	__set_PRIMASK(1);

	SCB->VTOR = program_start_addr;

	__set_PRIMASK(0);
}
