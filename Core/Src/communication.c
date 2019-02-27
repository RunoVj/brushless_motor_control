#include <stdint.h>
#include <stdlib.h>
#include "usart.h"
#include "main.h"

#include <string.h>
#include "limit_switch_controller.h"
#include "flash_config.h"
#include "bootloader.h"
#include "communication.h"
#include "checksum.h"

uint8_t msg_buf[TERMINAL_RESPONSE_LENGTH];

bool parse_normal_request(LimitSwitchController *lim_sw_ctrl, struct Request *req)
{
	if (req->address == lim_sw_ctrl->address) {
		return true;
	}
	return false;
}

bool parse_terminal_request(LimitSwitchController *lim_sw_ctrl, struct TerminalRequest *req)
{
	if (req->address == lim_sw_ctrl->address) {
		return true;
	}	
	return false;	
};

bool parse_config_request(LimitSwitchController *lim_sw_ctrl, struct ConfigRequest *req)
{
	if (req->forse_setting || req->old_address == lim_sw_ctrl->address) {
		
		lim_sw_ctrl->address = req->new_address;
		
		FLASH_WriteSettings(lim_sw_ctrl, req->update_firmware);
		if (req->update_firmware) {
			// go to bootloader
			HAL_NVIC_SystemReset(); 
//			jump(BOOTLOADER_ADDR);
		}
	}
	return false; // DO NOT ANSWER TO CONFIG PACKAGE!
}

bool parse_device_request(LimitSwitchController *lim_sw_ctrl, struct DevicesRequest *req)
{
	if (req->address == lim_sw_ctrl->address) {
		return true;
	}
	return false;
}

bool parse_package(LimitSwitchController *lim_sw_ctrl, uint8_t *message, uint8_t length)
{ 
	if (IsChecksumm8bCorrect(message, length)) {
		if (length == NORMAL_REQUEST_LENGTH) {
			struct Request req;
			memcpy((void*)&req, (void*)message, length);
			return parse_normal_request(lim_sw_ctrl, &req);
		}
		else if (length == TERMINAL_REQUEST_LENGTH) {
			struct TerminalRequest req;
			memcpy((void*)&req, (void*)message, length);
			return parse_terminal_request(lim_sw_ctrl, &req);
		}
		else if (length == CONFIG_REQUEST_LENGTH) {
			struct ConfigRequest req;
			memcpy((void*)&req, (void*)message, length);
			return parse_config_request(lim_sw_ctrl, &req);
		}
		else if (length == DEVICES_REQUEST_LENGTH) {
			struct DevicesRequest req;
			memcpy((void*)&req, (void*)message, length);
			return parse_device_request(lim_sw_ctrl, &req);
		}
  }
	return false;
}

void normal_response(LimitSwitchController *lim_sw_ctrl)
{
	struct Response resp;
	resp.AA = 0xAA;
	resp.type = NORMAL_REQUEST_TYPE;
	resp.address = lim_sw_ctrl->address;
	resp.state = lim_sw_ctrl->lim_switch1_state;
	resp.current = lim_sw_ctrl->lim_switch2_state;
	resp.speed_period = 0x55;

	memcpy((void*)msg_buf, (void*)&resp, NORMAL_RESPONSE_LENGTH - 1);
	AddChecksumm8b(msg_buf, NORMAL_RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	
  HAL_UART_Transmit_DMA(&huart1, msg_buf, NORMAL_RESPONSE_LENGTH);
}

void terminal_response(LimitSwitchController *lim_sw_ctrl)
{
	struct TerminalResponse resp;
	resp.AA = 0xAA;
	resp.type = TERMINAL_REQUEST_TYPE;
	resp.address = lim_sw_ctrl->address;
	resp.state = lim_sw_ctrl->lim_switch1_state;
	resp.position_code = lim_sw_ctrl->lim_switch2_state;
	resp.cur_angle = 0x55;
	resp.current = 0x55;
	resp.speed_period = 0x55;
	resp.clockwise_speed_k = 0x55;
	resp.counterclockwise_speed_k = 0x55;
  
	memcpy((void*)msg_buf, (void*)&resp, TERMINAL_RESPONSE_LENGTH - 1);
	AddChecksumm8b(msg_buf, TERMINAL_RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	
  HAL_UART_Transmit_DMA(&huart1, msg_buf, TERMINAL_RESPONSE_LENGTH);
}

void device_response(LimitSwitchController *lim_sw_ctrl)
{
	struct DevicesResponse resp;
	resp.AA = 0xAA;
	resp.address = lim_sw_ctrl->address;
	resp.errors = 0x00;
	resp.velocity1 = lim_sw_ctrl->lim_switch1_state;
	resp.velocity2 = lim_sw_ctrl->lim_switch2_state;
	resp.current1 = 0x00;
	resp.current2 = 0x00;
  
	memcpy((void*)msg_buf, (void*)&resp, DEVICES_RESPONSE_LENGTH - 1);
	AddChecksumm8b(msg_buf, DEVICES_RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	
  HAL_UART_Transmit_DMA(&huart1, msg_buf, DEVICES_RESPONSE_LENGTH);
}
