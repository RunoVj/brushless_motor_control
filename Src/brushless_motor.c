#include "brushless_motor.h"
#include "gpio.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "svpwm.h"

BrushlessMotor BLDC;
uint16_t adc_buf[ADC_BUF_SIZE];

void init(BrushlessMotor *BLDC)
{
	BLDC->position_setting_enabled = false;
	BLDC->speed_period = 0;
	BLDC->cur_sector = 0;
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

void update_velocity(BrushlessMotor* BLDC, int8_t velocity)
{
  if (velocity > 0) {
		BLDC->rotation_dir = clockwise;
  	BLDC->pwm_duty = velocity*PWM_RESIZE_COEF;
	}
  else {
    BLDC->rotation_dir = counterclockwise;
		BLDC->pwm_duty = (-velocity)*PWM_RESIZE_COEF;
  }

}

void set_angle(uint16_t angle, uint16_t amplitude, uint8_t dir)
{
	if (amplitude != 0) {
		uint16_t PWM[3];
		uvector_state(angle, amplitude, PWM, MAX_PWM_DUTY, dir);
		update_pwm_duty(A, PWM[a]);
		update_pwm_duty(B, PWM[b]);
		update_pwm_duty(C, PWM[c]);	
		motor_enable();
	}
	else {
		motor_disable();
	}
}

void set_next_angle(BrushlessMotor *BLDC)
{
	if (BLDC->pwm_duty != 0 && !BLDC->impulse_overcurrent) {
		update_pwm_duty(A, BLDC->pwm_duties[BLDC->position_code][a]);
		update_pwm_duty(B, BLDC->pwm_duties[BLDC->position_code][b]);
		update_pwm_duty(C, BLDC->pwm_duties[BLDC->position_code][c]);	
		motor_enable();		
	}
	else {
		BLDC->impulse_overcurrent = false;
		motor_disable();
	}	
}

void update_angles(BrushlessMotor *BLDC)
{
	calculate_next_angles(BLDC);
	if (BLDC->pwm_duty != 0) {
		for (uint8_t i = 0; i < MAX_BASE_VECTORS_NUMB; ++i) {
			uvector_state(BLDC->next_angles[i], BLDC->pwm_duty, BLDC->pwm_duties[i], MAX_PWM_DUTY, BLDC->rotation_dir);
		}
	}
}

int16_t calculate_next_angle(BrushlessMotor *BLDC, uint8_t position_code)
{
	int16_t next_angle = 0;
	static uint16_t speed_addition = 0;
	if (BLDC->rotation_dir) {
		next_angle = (BLDC->base_vectors[position_code] +	90 + BLDC->outrunning_angle);
		//
		if (BLDC->speed_period > 0 && BLDC->speed_period < 500) {
			speed_addition = BLDC->speed_k/BLDC->speed_period;
			next_angle += speed_addition;
		}
		//
		if ((BLDC->next_angle) > 359) {
			BLDC->next_angle = (BLDC->next_angle) - 359;
		}
	}
	else {
		next_angle = (BLDC->base_vectors[position_code] -	90 - BLDC->outrunning_angle);
		//
		if (BLDC->speed_period > 0 && BLDC->speed_period < 500) {
			speed_addition = BLDC->speed_k/BLDC->speed_period;
			next_angle -= speed_addition;
		}
		//
		if ((next_angle) < 0) {
				(next_angle) = 359 + (next_angle);
		}
	}
	return next_angle;
}

void calculate_next_angles(BrushlessMotor *BLDC)
{
	for (uint8_t i = 0; i < MAX_BASE_VECTORS_NUMB; ++i) {
		BLDC->next_angles[i] = calculate_next_angle(BLDC, i);
	}
}

void calculate_speed(BrushlessMotor *BLDC)
{
	BLDC->cur_sector = (BLDC->cur_sector + 1) % SPEED_BUF_SIZE;
	if (BLDC->cur_sector == 0) {
		uint64_t tmp = 0;
		for (int i = 0; i < SPEED_BUF_SIZE; ++i) {
			tmp += BLDC->speed_counter[i];
			BLDC->speed_counter[i] = 0;
		}
		BLDC->speed_period = tmp/SPEED_BUF_SIZE;
	}
}
