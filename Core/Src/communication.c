#include <stdint.h>
#include <stdlib.h>
#include "usart.h"
#include "main.h"
#include "tim.h"

#include <string.h>
#include "brushless_motor.h"
#include "flash_config.h"
#include "bootloader.h"
#include "communication.h"
#include "checksum.h"

uint8_t msg_buf[TERMINAL_RESPONSE_LENGTH];

bool parse_normal_request(BrushlessMotor *BLDC, struct Request *req)
{
	if (req->address == BLDC->address) {
		BLDC->velocity = req->velocity;
		update_velocity(BLDC, BLDC->velocity);
		return true;
	}
	return false;
}

bool parse_terminal_request(BrushlessMotor *BLDC, struct TerminalRequest *req)
{
	if (req->address == BLDC->address) {
		// if vectors updating complited
		BLDC->velocity = req->velocity;
		update_velocity(BLDC, BLDC->velocity);
		if (req->update_speed_k) {
			BLDC->speed_k[BLDC->rotation_dir] = req->speed_k;
		}
		return true;
	}	
	return false;	
};

bool parse_config_request(BrushlessMotor *BLDC, struct ConfigRequest *req)
{
	if (req->forse_setting || req->old_address == BLDC->address) {
		
		BLDC->address = req->new_address;
		BLDC->speed_k[clockwise] = req->clockwise_speed_k;
		BLDC->speed_k[counterclockwise] = req->counterclockwise_speed_k;
		
		FLASH_WriteSettings(BLDC, req->update_firmware);
		if (req->update_firmware) {
			motor_stop(BLDC);
			// go to bootloader
			HAL_NVIC_SystemReset(); 
//			jump(BOOTLOADER_ADDR);
		}
	}
	return false; // DO NOT ANSWER TO CONFIG PACKAGE!
}

bool parse_package(BrushlessMotor *BLDC, uint8_t *message, uint8_t length)
{ 
	if (IsChecksumm8bCorrect(message, length)) {
		if (length == NORMAL_REQUEST_LENGTH) {
			struct Request req;
			memcpy((void*)&req, (void*)message, length);
			return parse_normal_request(BLDC, &req);
		}
		else if (length == TERMINAL_REQUEST_LENGTH) {
			struct TerminalRequest req;
			memcpy((void*)&req, (void*)message, length);
			return parse_terminal_request(BLDC, &req);
		}
		else if (length == CONFIG_REQUEST_LENGTH) {
			struct ConfigRequest req;
			memcpy((void*)&req, (void*)message, length);
			return parse_config_request(BLDC, &req);
		}
  }
	return false;
}

void normal_response(BrushlessMotor *BLDC)
{
	struct Response resp;
	resp.AA = 0xAA;
	resp.type = 0x01;
	resp.address = BLDC->address;
	resp.state = 0x55;
	resp.current = 0x55;
	resp.speed_period = 0x55;

	memcpy((void*)msg_buf, (void*)&resp, NORMAL_RESPONSE_LENGTH - 1);
	AddChecksumm8b(msg_buf, NORMAL_RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	
  HAL_UART_Transmit_DMA(&huart1, msg_buf, NORMAL_RESPONSE_LENGTH);
}

void terminal_response(BrushlessMotor *BLDC)
{
	struct TerminalResponse resp;
	resp.AA = 0xAA;
	resp.type = 0x01;
	resp.address = BLDC->address;
	resp.state = 0x55;
	resp.position_code = 0x55;
	resp.cur_angle = 0x55;
	resp.current = 0x55;
	resp.speed_period = 0x55;
	resp.clockwise_speed_k = BLDC->speed_k[clockwise];
	resp.counterclockwise_speed_k = BLDC->speed_k[counterclockwise];
  
	memcpy((void*)msg_buf, (void*)&resp, TERMINAL_RESPONSE_LENGTH - 1);
	AddChecksumm8b(msg_buf, TERMINAL_RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	
  HAL_UART_Transmit_DMA(&huart1, msg_buf, TERMINAL_RESPONSE_LENGTH);
}
