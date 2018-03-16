#ifndef __brushless_motor_H
#define __brushless_motor_H

#include "stdint.h"
#include "stdbool.h"

#define MAX_CURRENT 4000

typedef enum { fan_mode, emf_mode } ControlMode;
typedef enum { clockwise, counterclockwise } RotationDir;
typedef enum { A, B, C} Phase;

typedef struct {
  ControlMode control_mode;
  RotationDir rotation_dir;
  
  uint8_t velocity;
  uint16_t pwm_duty;
  uint8_t emf_state;
  
  uint16_t current;
  
  uint16_t phase_a_tick;
  uint16_t phase_b_tick;
  uint16_t phase_c_tick;
  
  uint16_t ticks_for_next_commute;
  
} BrushlessMotor;

extern BrushlessMotor BLDC;

uint8_t read_gray_code(void);

void commute(BrushlessMotor* BLDC, uint8_t state);
void update_pwm_duty(Phase phase, uint16_t duty);

void set_emf_state(BrushlessMotor* BLDC, uint8_t state);
void set_next_emf_state(BrushlessMotor* BLDC);

void update_emf_state(BrushlessMotor* BLDC);
void update_velocity(BrushlessMotor* BLDC, uint8_t velocity);
uint8_t get_next_state(BrushlessMotor* BLDC, uint8_t gray_code);

#endif /*__brushless_motor_H */
