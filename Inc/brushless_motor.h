#ifndef __brushless_motor_H
#define __brushless_motor_H

#include "stdint.h"
#include "stdbool.h"

#define MAX_CURRENT 4000
#define STARTED_FILTER 6

typedef enum { fan_mode, emf_mode, hall_mode } ControlMode;
typedef enum { clockwise, counterclockwise } RotationDir;
typedef enum { A, B, C} Phase;



typedef struct {
  ControlMode control_mode;
  RotationDir rotation_dir;
  
  bool started;
  
  uint8_t velocity;
  uint16_t pwm_duty;
  
  uint8_t state;
  uint8_t next_state;
  
  uint16_t current;
  
  uint16_t fan_mode_commutation_period;
  bool fan_mode_enable;
  uint8_t successful_predictions;
  
  uint8_t cur_gray_code;
  uint8_t cur_emf_code;
  
  uint16_t phase_a_tick;
  uint16_t phase_b_tick;
  uint16_t phase_c_tick;
  
  uint16_t ticks_for_next_commute;
  int32_t ticks_threshold;
  
} BrushlessMotor;

extern BrushlessMotor BLDC;

uint8_t read_gray_code(void);
uint8_t read_emf_code(void);

void commute(BrushlessMotor* BLDC, uint8_t state);
void update_pwm_duty(Phase phase, uint16_t duty);

void set_state(BrushlessMotor* BLDC, uint8_t state);
void set_next_state(BrushlessMotor* BLDC);

void update_state(BrushlessMotor* BLDC);
void update_velocity(BrushlessMotor* BLDC, uint8_t velocity);
void update_pwm_in_channel(BrushlessMotor* BLDC);

uint8_t convert_next_state(BrushlessMotor* BLDC, uint8_t code);
uint8_t get_next_state(BrushlessMotor* BLDC);

bool did_it_started(BrushlessMotor* BLDC);

#endif /*__brushless_motor_H */
