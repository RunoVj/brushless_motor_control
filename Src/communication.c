#include <stdint.h>
#include <stdlib.h>
#include "usart.h"
#include "main.h"
#include "tim.h"

#include "communication.h"
#include "checksum.h"

void parse_package(BrushlessMotor *BLDC, uint8_t *message, uint8_t length)
{
  if(message[0] == 0x01){      
    update_velocity(BLDC, message[1]);          
  }
  
  else if(message[0] == 0x02){
    BLDC->control_param.fan_mode_enable = false;
    set_state(BLDC, message[1]);
    commute(BLDC, message[1]);
  }
  else if(message[0] == 0x03){
    BLDC->control_param.fan_mode_enable = true;
    BLDC->control_param.fan_mode_commutation_period = message[1];
  }  
}
