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

void parse_package(BrushlessMotor *BLDC, uint8_t *message, uint8_t length)
{ 
	if (IsChecksumm8bCorrect(message, length)) {
		struct Request req;
		memcpy((void*)&req, (void*)message, length);
		BLDC->update_base_vectors = req.update_base_vector;
		BLDC->position_setting_enabled = req.position_setting;
		if (req.position_setting) {
			BLDC->next_angle = req.angle;
		}
		BLDC->velocity = req.velocity;
		BLDC->fan_mode_commutation_period = req.frequency;
		update_velocity(BLDC, BLDC->velocity);
		BLDC->outrunning_angle = req.outrunning_angle;
  }    
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

  AddChecksumm8b(msg_buf, REQUEST_LENGTH);
  
	memcpy((void*)&msg_buf, (void*)&resp, RESPONSE_LENGTH);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
  HAL_UART_Transmit_DMA(&huart1, msg_buf, REQUEST_LENGTH);
}
