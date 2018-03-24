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

uint8_t rx_byte;
uint8_t rx_counter;
bool uart1_package_received;
uint8_t uart_pack_size = 2;
uint8_t uart_buf[2];

uint8_t package_timeout = 2;

bool package_started = true; //to start receiving

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
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
  static uint16_t communication_counter = 0;
  static uint16_t led_counter = 0;
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  
  if (package_started){
    ++communication_counter;
    if (communication_counter >= package_timeout){
      communication_counter = 0;
      if (uart1_package_received){
        uart1_package_received = false;

        if(uart_buf[0] == 0x01){      
          update_velocity(&BLDC, uart_buf[1]);          
        }
        else if(uart_buf[0] == 0x02){
          BLDC.fan_mode_enable = false;
          set_state(&BLDC, uart_buf[1]);
          commute(&BLDC, uart_buf[1]);
        }
        else if(uart_buf[0] == 0x03){
          BLDC.fan_mode_enable = true;
          BLDC.fan_mode_commutation_period = uart_buf[1];
        }
      }
      rx_counter = 0;
      HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
      package_started = false;
     }
  }
  //  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);
  
  if (led_counter == 250){
    HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
    led_counter = 0;
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
* @brief This function handles DMA1 channel1 global interrupt.
*/
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

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

  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
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
	if(rx_counter == 0){
    package_started = true;
	}
  
	uart_buf[rx_counter++] = rx_byte;
	
	if (rx_counter == uart_pack_size) {
		uart1_package_received = true;
		rx_counter = 0;
	} 
  else {
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);    
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  static bool a_rising_edge, b_rising_edge, c_rising_edge;
  if (htim == &htim2){
    switch (htim->Channel){
      case HAL_TIM_ACTIVE_CHANNEL_1:
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_2:
        if (a_rising_edge){
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, 
            TIM_INPUTCHANNELPOLARITY_FALLING);
          a_rising_edge = false;
        }
        else{
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, 
            TIM_INPUTCHANNELPOLARITY_RISING);
          a_rising_edge = true;
        }
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_3:
        if (b_rising_edge){
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, 
            TIM_INPUTCHANNELPOLARITY_FALLING);
          b_rising_edge = false;
        }
        else{
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, 
            TIM_INPUTCHANNELPOLARITY_RISING);
          b_rising_edge = true;
        }
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_4:
        if (c_rising_edge){
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, 
            TIM_INPUTCHANNELPOLARITY_FALLING);
          c_rising_edge = false;
        }
        else{
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, 
            TIM_INPUTCHANNELPOLARITY_RISING);
          c_rising_edge = true;
        }
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_CLEARED:
      break;      
    }
    
    BLDC.phase_a_tick = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_2);
    BLDC.phase_b_tick = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_3);
    BLDC.phase_c_tick = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_4);
    
    __HAL_TIM_SET_COUNTER(htim, 0);
    

    BLDC.cur_emf_code = read_emf_code();
    BLDC.cur_gray_code = read_gray_code();
    
    
    if (did_it_started(&BLDC)){
      BLDC.control_mode = emf_mode;
      update_state(&BLDC);
      set_next_state(&BLDC);
      commute(&BLDC, BLDC.state);
    }
    
    switch (htim->Channel){
      case HAL_TIM_ACTIVE_CHANNEL_1:
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_2:
        BLDC.ticks_for_next_commute = (uint16_t)(BLDC.phase_a_tick/2);
        BLDC.ticks_threshold = BLDC.phase_a_tick;
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_3:
        BLDC.ticks_for_next_commute = (uint16_t)(BLDC.phase_b_tick/2);
        BLDC.ticks_threshold = BLDC.phase_b_tick;
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_4:
        BLDC.ticks_for_next_commute = (uint16_t)(BLDC.phase_c_tick/2);
        BLDC.ticks_threshold = BLDC.phase_c_tick;
      break;
      
      case HAL_TIM_ACTIVE_CHANNEL_CLEARED:
      break;      
    }
    BLDC.ticks_threshold *= 10;
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1){
		HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim3){
    
    if (BLDC.control_mode == fan_mode && BLDC.fan_mode_enable){
      static uint16_t fan_mode_counter = 0;
      ++fan_mode_counter;
     
      if (fan_mode_counter >= BLDC.fan_mode_commutation_period){
        fan_mode_counter = 0;
        set_next_state(&BLDC);
        commute(&BLDC, BLDC.state);
      }
    }
    
    else if (BLDC.control_mode == emf_mode){
      BLDC.ticks_for_next_commute--;
      BLDC.ticks_threshold--;
      
//      if (BLDC.ticks_for_next_commute == 0){
//        update_state(&BLDC);
//        set_next_state(&BLDC);
//        commute(&BLDC, BLDC.state);
//      }
//      else 
        if (BLDC.ticks_threshold <= 0){
        BLDC.control_mode = fan_mode;
        BLDC.started = false;
      }
    }   
  }
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
