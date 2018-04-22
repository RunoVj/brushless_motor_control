#include <stdint.h>
#include <stdlib.h>
#include "usart.h"
#include "main.h"
#include "tim.h"

#include "communication.h"
#include "checksum.h"
#include "brushless_motor.h"

void parse_package(BrushlessMotor *BLDC, uint8_t *message, uint8_t length)
{ 
  BLDC->control_param.pwm_duty = message[VMA_DEV_REQUEST_VELOCITY1];
  update_velocity(BLDC, BLDC->control_param.pwm_duty );          
  
  BLDC->control_param.position_setting_enabled = message[VMA_DEV_REQUEST_ADDRESS];
  if (BLDC->control_param.position_setting_enabled){
    commute(BLDC, message[VMA_DEV_REQUEST_SETTING]);
  }
  
  BLDC->control_param.fan_mode_commutation_period = message[VMA_DEV_REQUEST_VELOCITY2];
    
}

void send_package(BrushlessMotor *BLDC)
{
  uint8_t msg_buf[VMA_DEV_RESPONSE_LENGTH];
  msg_buf[VMA_DEV_RESPONSE_AA] = 0xAA;
  msg_buf[VMA_DEV_RESPONSE_ADDRESS] = BLDC->settings.address;
  msg_buf[VMA_DEV_RESPONSE_ERRORS] = 0xFF;
  msg_buf[VMA_DEV_RESPONSE_CURRENT_1H] = (uint8_t)(BLDC->state_param.current >> 8);
  msg_buf[VMA_DEV_RESPONSE_CURRENT_1L] = (uint8_t)(BLDC->state_param.current);
  msg_buf[VMA_DEV_RESPONSE_CURRENT_2H] = BLDC->state_param.position_code;
  msg_buf[VMA_DEV_RESPONSE_CURRENT_2L] = BLDC->state_param.state;
  msg_buf[VMA_DEV_RESPONSE_VELOCITY1] = (uint8_t)(BLDC->state_param.ticks_for_next_commute >> 8);
  msg_buf[VMA_DEV_RESPONSE_VELOCITY2] = (uint8_t)(BLDC->state_param.ticks_for_next_commute);
  AddChecksumm8b(msg_buf, VMA_DEV_RESPONSE_LENGTH);
  
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
  HAL_UART_Transmit_DMA(&huart1, msg_buf, VMA_DEV_RESPONSE_LENGTH);
}
