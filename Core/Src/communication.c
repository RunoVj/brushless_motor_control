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
#include "adc.h"

uint8_t msg_buf[TERMINAL_RESPONSE_LENGTH];

bool parse_normal_request(BrushlessMotor *BLDC, struct Request *req)
{
	if (req->address == BLDC->address) {
		BLDC->velocity = req->velocity;
		update_velocity(BLDC, BLDC->velocity);
		update_angles(BLDC);
		return true;
	}
	return false;
}

bool parse_terminal_request(BrushlessMotor *BLDC, struct TerminalRequest *req)
{
	if (req->address == BLDC->address) {
		// if vectors updating complited
		if (BLDC->update_base_vectors && !req->update_base_vector) {
			FLASH_WriteSettings(BLDC, false);
		}
		BLDC->update_base_vectors = req->update_base_vector;
		BLDC->position_setting_enabled = req->position_setting;
		if (req->position_setting) {
			BLDC->next_angle = req->angle;
		}
		BLDC->velocity = req->velocity;
		BLDC->fan_mode_commutation_period = req->frequency;
		update_velocity(BLDC, BLDC->velocity);
		BLDC->outrunning_angle = req->outrunning_angle;
		if (req->update_speed_k) {
			BLDC->speed_k[BLDC->rotation_dir] = req->speed_k;
		}
		// recalculate next angles
		update_angles(BLDC);
		return true;
	}	
	return false;	
};

bool parse_config_request(BrushlessMotor *BLDC, struct ConfigRequest *req)
{
	if (req->forse_setting || req->old_address == BLDC->address) {
		BLDC->high_impulse_current_threshold = req->high_threshold;
		BLDC->low_impulse_current_threshold = req->low_threshold;
		update_current_thresholds(&hadc1, req->high_threshold, req->low_threshold);
		BLDC->average_current_threshold = req->average_threshold;
		
		BLDC->address = req->new_address;
		BLDC->speed_k[clockwise] = req->clockwise_speed_k;
		BLDC->speed_k[counterclockwise] = req->counterclockwise_speed_k;
		
		FLASH_WriteSettings(BLDC, req->update_firmware);
		if (req->update_firmware) {
			motor_disable();
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
	resp.state = BLDC->state;
	resp.current = BLDC->current;
	resp.speed_period = BLDC->speed_period;

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
	resp.state = BLDC->state;
	resp.position_code = BLDC->position_code;
	resp.cur_angle = BLDC->cur_angle;
	resp.current = BLDC->current;
	resp.speed_period = BLDC->speed_period;
	resp.clockwise_speed_k = BLDC->speed_k[clockwise];
	resp.counterclockwise_speed_k = BLDC->speed_k[counterclockwise];
  
	memcpy((void*)msg_buf, (void*)&resp, TERMINAL_RESPONSE_LENGTH - 1);
	AddChecksumm8b(msg_buf, TERMINAL_RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	
  HAL_UART_Transmit_DMA(&huart1, msg_buf, TERMINAL_RESPONSE_LENGTH);
}
