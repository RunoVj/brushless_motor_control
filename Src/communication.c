#include <stdint.h>
#include <stdlib.h>
#include "usart.h"
#include "main.h"
#include "tim.h"

#include <string.h>
#include "brushless_motor.h"
#include "communication.h"
#include "checksum.h"

uint8_t msg_buf[RESPONSE_LENGTH];

bool parse_normal_request(BrushlessMotor *BLDC, struct Request *req)
{
	if (req->address == BLDC->address) {
		BLDC->update_base_vectors = req->update_base_vector;
		BLDC->position_setting_enabled = req->position_setting;
		if (req->position_setting) {
			BLDC->next_angle = req->angle;
		}
		BLDC->velocity = req->velocity;
		BLDC->fan_mode_commutation_period = req->frequency;
		update_velocity(BLDC, BLDC->velocity);
		BLDC->outrunning_angle = req->outrunning_angle;
		return true;
	}	
	return false;
}

//    uint8_t AA;
//    uint8_t type; // 0x02
//    uint8_t update_firmware; // (bool) set new address or update firmware even if old address doesn't equal BLDC address
//    uint8_t forse_setting; // (bool) set new address even if old address doesn't equal BLDC address
//    uint8_t old_address;
//    uint8_t new_address;
//    uint8_t crc;

bool parse_config_request(BrushlessMotor *BLDC, struct ConfigRequest *req)
{
	if (req->forse_setting || req->old_address == BLDC->address) {
		if (req->update_firmware) {
			// TODO -> write to flash update flag
		}
		if (req->new_address != BLDC->address) {
			// write to FLASH new address
			BLDC->address = req->new_address;
		}
	}
	return false; // DO NOT ANSWER TO CONFIG PACKAGE!
}

bool parse_package(BrushlessMotor *BLDC, uint8_t *message, uint8_t length)
{ 
	if (IsChecksumm8bCorrect(message, length)) {
		if (length == REQUEST_LENGTH) {
			struct Request req;
			memcpy((void*)&req, (void*)message, length);
			return parse_normal_request(BLDC, &req);
		}
		else if (length == CONFIG_REQUEST_LENGTH) {
			struct ConfigRequest req;
			memcpy((void*)&req, (void*)message, length);
			return parse_config_request(BLDC, &req);
		}
  }
	return false;
}

void send_package(BrushlessMotor *BLDC)
{
	struct Response resp;
	resp.AA = 0xAA;
	resp.address = BLDC->address;
	resp.type = 0x01;
	resp.position_code = BLDC->position_code;
	resp.current = BLDC->current;
	resp.cur_angle = BLDC->cur_angle;

  AddChecksumm8b(msg_buf, RESPONSE_LENGTH);
  
	memcpy((void*)msg_buf, (void*)&resp, RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	
  HAL_UART_Transmit_IT(&huart1, msg_buf, RESPONSE_LENGTH);
}
