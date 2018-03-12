#include "brushless_motor.h"
#include "gpio.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"

BrushlessMotor BLDC;

void set_pwm_polarity(Phase phase, Polarity polarity)
{
  switch (phase){
    case A:
      if (polarity == direct){
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, 
        TIM_INPUTCHANNELPOLARITY_RISING);        
      }
      else if (polarity == reverse){
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, 
        TIM_INPUTCHANNELPOLARITY_FALLING);
      }   
    break;
    
    case B:
      if (polarity == direct){
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, 
        TIM_INPUTCHANNELPOLARITY_RISING);        
      }
      else if (polarity == reverse){
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, 
        TIM_INPUTCHANNELPOLARITY_FALLING);
      }      
    break;
    
    case C:
      if (polarity == direct){
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, 
        TIM_INPUTCHANNELPOLARITY_RISING);        
      }
      else if (polarity == reverse){
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, 
        TIM_INPUTCHANNELPOLARITY_FALLING);
      }      
    break;
    }      
}

void commute_old(BrushlessMotor* BLDC,uint8_t state)
{
  switch(state){
    case 4:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);
    
      set_pwm_polarity(A, reverse);
      set_pwm_polarity(C, direct);
    break;
  
    case 5:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_RESET);
    
      set_pwm_polarity(A, reverse);
      set_pwm_polarity(B, direct);  
    break;
  
    case 6:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);
    
      set_pwm_polarity(B, direct);
      set_pwm_polarity(C, reverse);
    break;
    
    case 1:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);
    
      set_pwm_polarity(A, direct);
      set_pwm_polarity(C, reverse);  
    break;			
    
    case 2:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_RESET);
    
      set_pwm_polarity(A, reverse);
      set_pwm_polarity(B, direct);		
    break;
    
    case 3:
      HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, 
        BRIDGE_A_EN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, 
        BRIDGE_B_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, 
        BRIDGE_C_EN_Pin, GPIO_PIN_SET);

      set_pwm_polarity(B, reverse);
      set_pwm_polarity(C, direct); 
    break;
  }
}

void commute(BrushlessMotor* BLDC,uint8_t state)
{
  switch(state){
    case 6:
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
  
    case 1:
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
  
    case 2:
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
    
    case 3:
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
    
    case 4:
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
    
    case 5:
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

uint8_t get_hall_state(uint8_t gray_code)
{
	switch (gray_code){
		// 2 6 4 5 1 3  <-
		// 3 1 5 4 6 2  ->
		case 1: return 1;
		case 5: return 2;
		case 4: return 3;
		case 6: return 4;
		case 2: return 5;
		case 3: return 6;	
		default : return 0;
	}	
}

uint8_t read_gray_code()
{
   return HAL_GPIO_ReadPin(EMF_A_GPIO_Port, EMF_A_Pin)      | 
          HAL_GPIO_ReadPin(EMF_B_GPIO_Port, EMF_B_Pin) << 1 |
          HAL_GPIO_ReadPin(EMF_C_GPIO_Port, EMF_C_Pin) << 2;
}


void set_emf_state(BrushlessMotor* BLDC, uint8_t gray_code)
{
  BLDC->emf_state = get_hall_state(gray_code);
//  uint8_t buf[2];
//  buf[0] = 0xAA;
//  buf[1] = state;
//  transmit_package_dma(buf, 2);
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

uint8_t get_next_state(BrushlessMotor* BLDC)
{
  if (BLDC->rotation_dir == clockwise){
    switch (BLDC->emf_state){
      case 1: return 2;
      case 2: return 3;
      case 3: return 4;
      case 4: return 5;
      case 5: return 6;
      case 6: return 1;	
      default : return 0;
    }	    
  }
  else {
    switch (BLDC->emf_state){
      case 1: return 6;
      case 2: return 1;
      case 3: return 2;
      case 4: return 3;
      case 5: return 4;
      case 6: return 5;	
      default : return 0;
    }    
  }
  
}
