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
    
      update_pwm_duty(A, BLDC->pwm_duty);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, 0);    
    break;
  
    case 2:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);

      update_pwm_duty(A, BLDC->pwm_duty);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, 0);     
    break;
  
    case 3:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);

      update_pwm_duty(A, 0);
      update_pwm_duty(B, BLDC->pwm_duty);
      update_pwm_duty(C, 0); 
    break;
    
    case 4:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_RESET);

      update_pwm_duty(A, 0);
      update_pwm_duty(B, BLDC->pwm_duty);
      update_pwm_duty(C, 0);     
    break;			
    
    case 5:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);

      update_pwm_duty(A, 0);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, BLDC->pwm_duty); 		
    break;
    
    case 6:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);

      update_pwm_duty(A, 0);
      update_pwm_duty(B, 0);
      update_pwm_duty(C, BLDC->pwm_duty);     
    break;
  }
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


void set_emf_state(BrushlessMotor* BLDC, uint8_t state)
{
  BLDC->emf_state = state;
}

void update_emf_state(BrushlessMotor* BLDC)
{
  set_emf_state(BLDC, get_next_state(BLDC, read_gray_code()));
}

void set_next_emf_state(BrushlessMotor* BLDC)
{
  uint8_t state = BLDC->emf_state;
  if (BLDC->rotation_dir == clockwise){
    switch(state){
      case 0:
        state = 1;
      break;
      
      case 1:
        state = 2;
      break;
 
      case 2:
        state = 3;
      break;
      
      case 3:
        state = 4;
      break;
 
      case 4:
        state = 5;
      break;
      
      case 5:
        state = 6;
      break;
 
      case 6:
        state = 1;
      break;    
    }
  }
  else{
    switch(state){
      case 0:
        state = 6;
      break;
      
      case 1:
        state = 6;
      break;
 
      case 2:
        state = 1;
      break;
      
      case 3:
        state = 2;
      break;
 
      case 4:
        state = 3;
      break;
      
      case 5:
        state = 4;
      break;
 
      case 6:
        state = 5;
      break;    
    }
  }

  set_emf_state(BLDC, state);
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
  
  update_pwm_duty(A, BLDC->pwm_duty);
  update_pwm_duty(B, BLDC->pwm_duty);
  update_pwm_duty(C, BLDC->pwm_duty);
}

uint8_t get_next_state(BrushlessMotor* BLDC, uint8_t gray_code)
{
  if (BLDC->rotation_dir == clockwise){
    switch (gray_code){
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
    switch (gray_code){
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

uint8_t get_next_gray_code(BrushlessMotor* BLDC, uint8_t state)
{
  if (BLDC->rotation_dir == clockwise){
    switch (state){
      // 2 6 4 5 1 3  <-
      // 3 1 5 4 6 2  ->
      case 1: return 1;
      case 2: return 5;
      case 3: return 4;
      case 4: return 6;
      case 5: return 2;
      case 6: return 3;
      default: return 0;      
    }	    
  }
  else {
    switch (state){
      case 6: return 1;
      case 1: return 5;
      case 2: return 4;
      case 3: return 6;
      case 4: return 2;
      case 5: return 3;
      default: return 0;
    }    
  } 
}

bool did_it_started(BrushlessMotor* BLDC)
{
  BLDC->cur_gray_code = read_gray_code();
  if (BLDC->cur_gray_code == get_next_gray_code(BLDC, BLDC->next_emf_state)){
    BLDC->successful_predictions++;
    BLDC->next_emf_state = get_next_state(BLDC, read_gray_code());
    if (BLDC->successful_predictions == STARTED_FILTER){
      BLDC->successful_predictions = 0;
      BLDC->started = true;
      return true;
    }
    return false;
  }
  else {
    BLDC->successful_predictions = 0;
    BLDC->next_emf_state = get_next_state(BLDC, read_gray_code());
    return false;
  }
}
