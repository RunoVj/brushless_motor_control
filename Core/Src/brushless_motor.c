#include "brushless_motor.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "messages.h"

BrushlessMotor BLDC;

void init(BrushlessMotor *BLDC)
{
	motor_stop(BLDC);
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

void motor_stop(BrushlessMotor *BLDC)
{
	htim3.Instance->CCR1 = SERVO_STOP_DUTY;
}
