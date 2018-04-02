#include "brushless_motor.h"
#include "gpio.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"

BrushlessMotor BLDC;

void commute(BrushlessMotor* BLDC,uint8_t state)
{
  switch(state){
    case 1:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_RESET);   
    break;
  
    case 2:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);  
    break;
  
    case 3:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);
    break;
    
    case 4:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_RESET);
    break;			
    
    case 5:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);
    break;
    
    case 6:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);    
    break;
  }
  update_pwm_in_channel(BLDC);
}

void update_pwm_duty(Phase phase, uint16_t duty)
{
  switch (phase){
    case A:
      htim1.Instance->CCR1 = duty;
    break;
    
    case B:
      htim3.Instance->CCR2 = duty;      
    break;
    
    case C:
      htim3.Instance->CCR1 = duty;      
    break;
  }
}

uint8_t read_gray_code()
{
   return HAL_GPIO_ReadPin(EMF_A_GPIO_Port, EMF_A_Pin)      | 
          HAL_GPIO_ReadPin(EMF_B_GPIO_Port, EMF_B_Pin) << 1 |
          HAL_GPIO_ReadPin(EMF_C_GPIO_Port, EMF_C_Pin) << 2;
}


void set_state(BrushlessMotor* BLDC, uint8_t new_state)
{
  BLDC->state = new_state;
}

void update_state(BrushlessMotor* BLDC)
{
  if (BLDC->control_mode == hall_mode){
    set_state(BLDC, convert_next_state(BLDC, read_gray_code()));
  }
  else{
    set_state(BLDC, convert_next_state(BLDC, read_emf_code()));    
  }
}

void set_next_state(BrushlessMotor* BLDC)
{
  set_state(BLDC, get_next_state(BLDC));
}

void update_velocity(BrushlessMotor* BLDC, uint8_t velocity)
{
  if (velocity <= 128){
     BLDC->rotation_dir = clockwise;
     BLDC->pwm_duty = (128 - velocity)*47;
  }
  else {
     BLDC->rotation_dir = counterclockwise;
     BLDC->pwm_duty = (velocity - 128)*47;
  }
  update_pwm_in_channel(BLDC);
}

void update_pwm_in_channel(BrushlessMotor* BLDC)
{
  switch(BLDC->state){
    case 1:
      update_pwm_duty(A, BLDC->pwm_duty);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, 0);    
    break;
  
    case 2:
      update_pwm_duty(A, BLDC->pwm_duty);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, 0);     
    break;
  
    case 3:
      update_pwm_duty(A, 0);
      update_pwm_duty(B, BLDC->pwm_duty);
      update_pwm_duty(C, 0); 
    break;
    
    case 4:
      update_pwm_duty(A, 0);
      update_pwm_duty(B, BLDC->pwm_duty);
      update_pwm_duty(C, 0);     
    break;			
    
    case 5:
      update_pwm_duty(A, 0);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, BLDC->pwm_duty); 		
    break;
    
    case 6:
      update_pwm_duty(A, 0);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, BLDC->pwm_duty);     
    break;
  }  
}

uint8_t convert_next_state(BrushlessMotor* BLDC, uint8_t code)
{
  if (BLDC->control_mode == hall_mode){
    if (BLDC->rotation_dir == clockwise){
      switch (code){
        // 2 6 4 5 1 3  <-
        // 3 1 5 4 6 2  ->
        case 1: return 1;
        case 5: return 2;
        case 4: return 3;
        case 6: return 4;
        case 2: return 5;
        case 3: return 6;
        default: return 0;      
      }	    
    }
    else {
      switch (code){
        case 1: return 6;
        case 5: return 1;
        case 4: return 2;
        case 6: return 3;
        case 2: return 4;
        case 3: return 5;
        default: return 0;      
      }    
    }    
  }
  else {
    if (BLDC->rotation_dir == clockwise){
      switch (code){
        // 4 6 7 3 1 0  <-
        // 0 1 3 7 6 4  ->
        case 0: return 1;
        case 4: return 2;
        case 6: return 3;
        case 7: return 4;
        case 3: return 5;
        case 1: return 6;
        default: return 0;      
      }	    
    }
    else {
      switch (code){
        case 0: return 6;
        case 4: return 1;
        case 6: return 2;
        case 7: return 3;
        case 3: return 4;
        case 1: return 5;
        default: return 0;       
      }    
    }        
  } 
}

uint8_t get_next_state(BrushlessMotor* BLDC)
{
  uint8_t state = BLDC->state;
  if (BLDC->rotation_dir == clockwise){
    switch(state){
      case 0: return 1;
      case 1: return 2;
      case 2: return 3;
      case 3: return 4;
      case 4: return 5;
      case 5: return 6;
      case 6: return 1; 
      default: return 0;
    }
  }
  else{
    switch(state){
      case 0: return 1;
      case 1: return 6;
      case 2: return 1;
      case 3: return 2;
      case 4: return 3;
      case 5: return 4;
      case 6: return 5; 
      default: return 0;      
    }
  }
}

uint8_t read_emf_code()
{
  return HAL_GPIO_ReadPin(EMF_A_GPIO_Port, EMF_A_Pin) << 1 | 
         HAL_GPIO_ReadPin(EMF_B_GPIO_Port, EMF_B_Pin) << 0 |
         HAL_GPIO_ReadPin(EMF_C_GPIO_Port, EMF_C_Pin) << 2;
}

bool did_it_started(BrushlessMotor* BLDC)
{
  if (BLDC->started){
    return true;
  }
  else if (BLDC->state == BLDC->next_state){
    BLDC->successful_predictions++;
    BLDC->next_state = get_next_state(BLDC);
    
    if (BLDC->successful_predictions == STARTED_FILTER){
      BLDC->successful_predictions = 0;
      BLDC->started = true;
      return true;
    }
    return false;
  }
  else {
    BLDC->successful_predictions = 0;
    BLDC->next_state = get_next_state(BLDC);
    return false;
  }
}



