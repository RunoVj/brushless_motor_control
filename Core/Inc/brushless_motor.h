#ifndef __brushless_motor_H
#define __brushless_motor_H

#include "stdint.h"
#include "stdbool.h"

#define COMMUTATION_TIMEOUT 0x00FF

#define MAX_PWM_DUTY 1499
#define PWM_FREQUENCY (72000000/(MAX_PWM_DUTY+1))
#define PWM_RESIZE_COEF (MAX_PWM_DUTY/128)
#define MAX_BASE_VECTORS_NUMB 8

#define ADC_BUF_SIZE 100
#define SPEED_BUF_SIZE 42

#define CORRECTION_PWM_DUTY MAX_PWM_DUTY/6

typedef enum { clockwise, counterclockwise } RotationDir;
typedef enum {stopped, rotated, overcurrent} WorkingState;
typedef enum { A, B, C } Phase;

typedef struct {
	uint8_t address;
	bool started;
	bool set_next_angle;
	bool impulse_overcurrent;
	bool overcurrent;
	
	uint16_t high_impulse_current_threshold;
	uint16_t low_impulse_current_threshold;
	uint16_t average_current_threshold;
	
	uint8_t cur_sector;
	uint16_t speed_k[2];
	uint16_t speed_counter[SPEED_BUF_SIZE];
	uint16_t speed_period;
	
	bool update_base_vectors;
	
	// for iteration used position code (gray code)
	uint16_t base_vectors[MAX_BASE_VECTORS_NUMB];
	int16_t next_angles[MAX_BASE_VECTORS_NUMB];
	uint16_t pwm_duties[MAX_BASE_VECTORS_NUMB][3];
	
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
extern uint16_t adc_buf[ADC_BUF_SIZE];

void init(BrushlessMotor *BLDC);

uint8_t read_code(void);

void motor_enable(void);
void motor_disable(void);

void calculate_speed(BrushlessMotor *BLDC);

void update_angles(BrushlessMotor *BLDC);
void update_pwm_duty(Phase phase, uint16_t duty);
void update_velocity(BrushlessMotor *BLDC, int8_t velocity);

void set_next_angle(BrushlessMotor *BLDC);
void set_angles(BrushlessMotor *BLDC);
void set_angle(uint16_t angle, uint16_t amplitude, uint8_t dir);

void calculate_next_angles(BrushlessMotor *BLDC);


#endif /*__brushless_motor_H */
