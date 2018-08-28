#include "svpwm.h"
#include "table_functions.h"

int table_value_1, table_value_2 = 0;
extern const int FULL_SIN[360];
unsigned int T[3];
unsigned int angle_in_sector;
char sector;

void uvector_state(uint16_t angle, uint16_t  amplitude, uint16_t* PWM, uint16_t pwm_limit, uint8_t direction)
{			
		sector = angle/60;
		angle_in_sector = angle - ( 60*sector ) ;
		table_value_1 = FULL_SIN[60 - angle_in_sector]; 
		table_value_2 = FULL_SIN[angle_in_sector];
		table_value_1 = (amplitude*table_value_1) >> 8; // equals multiply by 2 (aproximatley 1.713) 
		table_value_2 = (amplitude*table_value_2) >> 8;
		T[1] = table_value_1;
		T[2] = table_value_2;
		if (T[1] > pwm_limit) 
		{
			T[1] =  pwm_limit;
		}
		if (T[2] > pwm_limit)  
		{
			T[2] =  pwm_limit;
		}
		T[0] = (pwm_limit - T[1] - T[2]) >> 1;
		T[1] = T[1]; 													
		T[2] = T[2];
		
		
		
		if (direction) {
			switch (sector) {
			case 0: PWM[a] = T[0]; 
							PWM[b] = PWM[a] + T[1];
							PWM[c] = PWM[b] + T[2];
			break;
			case 1: PWM[b] = T[0]; 
							PWM[a] = PWM[b] + T[2];
							PWM[c]=  PWM[a] + T[1];
			break;
			case 2: PWM[b] = T[0]; 
							PWM[c] = PWM[b] + T[1];
							PWM[a] = PWM[c] + T[2];
			break;
			case 3: PWM[c] = T[0]; 
							PWM[b] = PWM[c] + T[2];
							PWM[a] = PWM[b] + T[1];
			break;
			case 4: PWM[c] = T[0];
							PWM[a] = PWM[c] + T[1];
							PWM[b] = PWM[a] + T[2];
			break;
			case 5: PWM[a] = T[0]; 
							PWM[c] = PWM[a] + T[2];
							PWM[b] = PWM[c] + T[1];
			break;
			default: break;
			}
		}
		else {
			switch (sector) {
			case 0: PWM[a] = T[0]; 
							PWM[b] = PWM[a] + T[2];
							PWM[c] = PWM[b] + T[1];
			break;
			case 1: PWM[b] = T[0]; 
							PWM[a] = PWM[b] + T[1];
							PWM[c] = PWM[a] + T[2];
			break;
			case 2: PWM[b] = T[0]; 
							PWM[c] = PWM[b] + T[2];
							PWM[a] = PWM[c] + T[1];
			break;
			case 3: PWM[c] = T[0];
							PWM[b] = PWM[c] + T[1];
							PWM[a] = PWM[b] + T[2];
			break;
			case 4:	PWM[c] = T[0];
							PWM[a] = PWM[c] + T[2];
							PWM[b] = PWM[a] + T[1];
			break;
			case 5: PWM[a] = T[0]; 
							PWM[c] = PWM[a] + T[1];
							PWM[b] = PWM[c] + T[2];
			break;
			default: break;
			}
		}
}
