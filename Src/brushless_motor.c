#include "brushless_motor.h"
#include "gpio.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "svpwm.h"

BrushlessMotor BLDC;

void init(BrushlessMotor *BLDC)
{
  BLDC->address = 0x01;
	read_code();
}

void motor_enable()
{
	HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, BRIDGE_A_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, BRIDGE_B_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, BRIDGE_C_EN_Pin, GPIO_PIN_SET);
}

void motor_disable()
{
	HAL_GPIO_WritePin(BRIDGE_A_EN_GPIO_Port, BRIDGE_A_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BRIDGE_B_EN_GPIO_Port, BRIDGE_B_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BRIDGE_C_EN_GPIO_Port, BRIDGE_C_EN_Pin, GPIO_PIN_RESET);
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

uint8_t read_code()
{
  BLDC.phase_a_state = HAL_GPIO_ReadPin(SENSOR_A_GPIO_Port, SENSOR_A_Pin);
  BLDC.phase_b_state = HAL_GPIO_ReadPin(SENSOR_B_GPIO_Port, SENSOR_B_Pin);
  BLDC.phase_c_state = HAL_GPIO_ReadPin(SENSOR_C_GPIO_Port, SENSOR_C_Pin);
  
  BLDC.position_code = BLDC.phase_a_state      | 
                              BLDC.phase_b_state << 1 |
                              BLDC.phase_c_state << 2;
  return BLDC.position_code;
}

void update_velocity(BrushlessMotor* BLDC, uint8_t velocity)
{
  if (velocity <= 128){
     BLDC->rotation_dir = clockwise;
     BLDC->pwm_duty = (128 - velocity)*23;
  }
  else {
     BLDC->rotation_dir = counterclockwise;
     BLDC->pwm_duty = (velocity - 128)*23;
  }
}

void set_angle(BrushlessMotor *BLDC, uint16_t angle, uint16_t amplitude, uint8_t dir)
{
	if (amplitude != 0) {
		uint16_t PWM[3];
		uvector_state(angle, amplitude, PWM, 2999, dir);
		update_pwm_duty(A, PWM[a]);
		update_pwm_duty(B, PWM[b]);
		update_pwm_duty(C, PWM[c]);	
	}
	else {
		motor_disable();
	}
}

void calculate_next_angle(BrushlessMotor *BLDC)
{
		BLDC->next_angle = (BLDC->base_vectors[BLDC->position_code] +	90 + BLDC->outrunning_angle) % 360;
}
