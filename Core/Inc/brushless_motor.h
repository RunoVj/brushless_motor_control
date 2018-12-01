#ifndef __brushless_motor_H
#define __brushless_motor_H

#include "stdint.h"
#include "stdbool.h"

#define MAX_SERVO_DUTY 1900
#define MIN_SERVO_DUTY 1100
#define SERVO_STOP_DUTY 1500
#define SERVO_RANGE 400

#define PWM_FREQUENCY (72000000/(MAX_PWM_DUTY+1))
#define PWM_RESIZE_COEF (SERVO_RANGE/128)
#define MAX_BASE_VECTORS_NUMB 8


typedef enum { clockwise, counterclockwise } RotationDir;

typedef struct {
	uint8_t address;
	bool started;
	uint16_t speed_k[2];

	RotationDir rotation_dir;
	
	int8_t velocity;
	uint16_t pwm_duty;    
 
} BrushlessMotor;


extern BrushlessMotor BLDC;

void init(BrushlessMotor *BLDC);
void calculate_speed(BrushlessMotor *BLDC);
void update_velocity(BrushlessMotor *BLDC, int8_t velocity);

void motor_stop(BrushlessMotor *BLDC);
#endif /*__brushless_motor_H */
