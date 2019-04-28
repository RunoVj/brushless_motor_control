/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"

/* USER CODE BEGIN 0 */
#include "brushless_motor.h"
#include "messages.h"
#include "communication.h"
#include "usart.h"

uint8_t rx_byte;
uint8_t rx_counter = 0;
bool uart1_package_received;
bool uart1_package_sended = true;

uint8_t uart_pack_size = 0;
uint8_t uart_receive_buf[20]; // max size of possible package

uint8_t package_timeout = RECEIVE_TIMEOUT;

bool package_started = true; //to start receiving

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	static uint16_t communication_counter;
  static uint16_t led_counter;
	// communication cycle
	// if first byte was received
	if (package_started) {
		++communication_counter;
		if (communication_counter == package_timeout) {
			communication_counter = 0;
			if (uart1_package_received){
				uart1_package_received = false;   
				bool parsing_is_ok = parse_package(&BLDC, uart_receive_buf, uart_pack_size);
				if (uart1_package_sended && parsing_is_ok) {
					if (uart_receive_buf[1] == NORMAL_REQUEST_TYPE) {
						normal_response(&BLDC); 
					}
					else /*if (uart_receive_buf[1] == TERMINAL_REQUEST_TYPE)*/ {
						terminal_response(&BLDC);
					}
				}          
			}
			rx_counter = 0;
			HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
			package_started = false;
		}
	}
		
	// blinking
	if (led_counter == 250) {
		led_counter = 0;
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
	++led_counter;
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line1 interrupt.
*/
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI1_IRQn 1 */
	read_code();
	BLDC.started = true;
	BLDC.timeout = 0;
	if (BLDC.update_base_vectors) {
		BLDC.base_vectors[BLDC.position_code] = BLDC.cur_angle;
	}
	else {
		BLDC.set_next_angle = true;
	}
	calculate_speed(&BLDC);
  /* USER CODE END EXTI1_IRQn 1 */
}

/**
* @brief This function handles EXTI line2 interrupt.
*/
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */

  /* USER CODE END EXTI2_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
  /* USER CODE BEGIN EXTI2_IRQn 1 */
	read_code();
	BLDC.started = true;
	BLDC.timeout = 0;
	if (BLDC.update_base_vectors) {
		BLDC.base_vectors[BLDC.position_code] = BLDC.cur_angle;
	}
	else {
		BLDC.set_next_angle = true;
	}
	calculate_speed(&BLDC);
  /* USER CODE END EXTI2_IRQn 1 */
}

/**
* @brief This function handles EXTI line3 interrupt.
*/
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI3_IRQn 1 */
	read_code();
	BLDC.started = true;
	BLDC.timeout = 0;
	if (BLDC.update_base_vectors) {
		BLDC.base_vectors[BLDC.position_code] = BLDC.cur_angle;
	}
	else {
		BLDC.set_next_angle = true;
	}

	calculate_speed(&BLDC);
	
  /* USER CODE END EXTI3_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel1 global interrupt.
*/
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */
	uint64_t sum = 0;
	for (uint8_t i = 0; i < ADC_BUF_SIZE; ++i) {
		sum += adc_buf[i];
	}
	BLDC.current = sum / ADC_BUF_SIZE;
  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel4 global interrupt.
*/
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */
  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
* @brief This function handles ADC1 and ADC2 global interrupts.
*/
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */
	BLDC.impulse_overcurrent = true;

  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
* @brief This function handles TIM1 update interrupt.
*/
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */
  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */
	if (BLDC.started) {
		BLDC.speed_counter[BLDC.cur_sector]++;
	}
	
	static uint8_t _3KHz_counter;
	if (_3KHz_counter == PWM_FREQUENCY/3000) {
		_3KHz_counter = 0;
		// start 3KHz
		// if update base vectors mode enabled
		if (BLDC.update_base_vectors) {
			BLDC.cur_angle++;
			if (BLDC.cur_angle >= 360) {
				BLDC.cur_angle = 0;
			}
			set_angle(BLDC.cur_angle, CORRECTION_PWM_DUTY, 1);
		}
		// if update base vectors mode disabled
		else if (BLDC.position_setting_enabled) {
			BLDC.cur_angle = BLDC.next_angle;
			set_angle(BLDC.next_angle, BLDC.pwm_duty, BLDC.rotation_dir);
		}
		// if BLDC started = the interruption of the sensors occurred during the timeout
		else if (BLDC.started) {
			//set_angle(&BLDC, BLDC.next_angle, BLDC.pwm_duty, BLDC.rotation_dir);
			BLDC.timeout++;
			if (BLDC.set_next_angle) {
				BLDC.set_next_angle = false;
				set_next_angle(&BLDC);
			}
			if (BLDC.timeout == COMMUTATION_TIMEOUT) {
				BLDC.started = false;
				BLDC.timeout = 0;
			}
		}
		// otherwise - synchronous mode
		else {
			if (BLDC.rotation_dir) {
				BLDC.next_angle = (BLDC.cur_angle + BLDC.fan_mode_commutation_period) % 360;
			}
			else {
				BLDC.next_angle = (BLDC.cur_angle - BLDC.fan_mode_commutation_period) % 360;
				if (BLDC.next_angle < 0) {
					BLDC.next_angle = (359 + BLDC.next_angle);
				}
			}
			set_angle(BLDC.next_angle, BLDC.pwm_duty, BLDC.rotation_dir);
			BLDC.cur_angle = BLDC.next_angle;
		}
	} // end 3KHz
	++_3KHz_counter;
  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
* @brief This function handles TIM3 global interrupt.
*/
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */
	
  /* USER CODE END TIM3_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (rx_counter == 0) {
    package_started = true;
	}
	else if (rx_counter == 1) {
		switch (rx_byte) {
			case NORMAL_REQUEST_TYPE:
				uart_pack_size = NORMAL_REQUEST_LENGTH;
			break;
			
			case TERMINAL_REQUEST_TYPE:
				uart_pack_size = TERMINAL_REQUEST_LENGTH;
			break;
			
			case CONFIG_REQUEST_TYPE:
				uart_pack_size = CONFIG_REQUEST_LENGTH;
			break;
		}
	}
  
	uart_receive_buf[rx_counter++] = rx_byte;
	
	if (rx_counter == uart_pack_size) {
		uart1_package_received = true;
		rx_counter = 0;
	}
  else {
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);    
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  uart1_package_sended = true;
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
