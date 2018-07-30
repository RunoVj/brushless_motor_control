#ifndef __brushless_motor_H
#define __brushless_motor_H

#include "stdint.h"
#include "stdbool.h"

#define COMMUTATION_TIMEOUT 0x0FFF
#define MAX_PWM_DUTY 2999
#define MAX_CURRENT 4000
#define STARTED_FILTER 6
#define MAX_BASE_VECTORS_NUMB 8

#define CORRECTION_PWM_DUTY 250

typedef enum { clockwise, counterclockwise } RotationDir;
typedef enum {stopped, rotated, overcurrent} WorkingState;
typedef enum { A, B, C } Phase;

typedef struct {
	uint8_t address;   
	bool started;
	
	bool update_base_vectors;
	uint16_t base_vectors[MAX_BASE_VECTORS_NUMB];
	
	int16_t cur_angle;
	int16_t outrunning_angle;
	int16_t next_angle;
	
	uint8_t phase_a_state;
	uint8_t phase_b_state;
	uint8_t phase_c_state;

	uint8_t state;
	uint8_t position_code;
	
	uint16_t current;
	
	uint16_t timeout;

	bool position_setting_enabled;
	RotationDir rotation_dir;
	
	int8_t velocity;
	uint16_t pwm_duty;    
	uint16_t fan_mode_commutation_period;
 
} BrushlessMotor;


extern BrushlessMotor BLDC;

void init(BrushlessMotor *BLDC);

uint8_t read_code(void);

void motor_enable(void);
void motor_disable(void);

void update_pwm_duty(Phase phase, uint16_t duty);

void update_velocity(BrushlessMotor *BLDC, int8_t velocity);

void set_angle(BrushlessMotor *BLDC, uint16_t angle, uint16_t amplitude, uint8_t dir);
void calculate_next_angle(BrushlessMotor *BLDC);


#endif /*__brushless_motor_H */
