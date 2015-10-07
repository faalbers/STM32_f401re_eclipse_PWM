//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

TIM_HandleTypeDef TIM_TimeBaseStructure;


/**************************************************************************************/

void RCC_Configuration(void)
{
  // Enable GPIOC Peripheral clock
  __GPIOC_CLK_ENABLE();

  /* GPIOA clock enable */
  __GPIOA_CLK_ENABLE();


  /* TIM2 clock enable */
  __TIM2_CLK_ENABLE();
}

/**************************************************************************************/

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in input push/pull mode
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  // TIM output
  GPIO_InitStructure.Pin = GPIO_PIN_5;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);



}

/**************************************************************************************/

void TIM2_Configuration(void)
{
  TIM_OC_InitTypeDef TIM_OCInitStructure;

  TIM_TimeBaseStructure.Instance = TIM2;
  // Timer cycles speed = 400 kHz = 84,000,000 / 84
  TIM_TimeBaseStructure.Init.Prescaler = 84-1;
  // Period = 100 Hz = 100,000 / 1000
  TIM_TimeBaseStructure.Init.Period = 2500-1;
  TIM_TimeBaseStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
  HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);

  TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
  TIM_OCInitStructure.Pulse = 1000; // 2000
  TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
  TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
  TIM_OCInitStructure.OCNIdleState= TIM_OCNIDLESTATE_RESET;
  TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;

  HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,TIM_CHANNEL_1);

  HAL_TIM_PWM_Start(&TIM_TimeBaseStructure, TIM_CHANNEL_1);

}

GPIO_PinState ButtonPressed()
{
  GPIO_PinState pin = 1;
  return pin-HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
}

int
main(int argc, char* argv[])
{
  GPIO_PinState   trigger = 0;
  uint16_t  pwm = 1000;
  int16_t   pwmstep;

  // At this stage the system clock should have already been configured
  // at high speed.

  RCC_Configuration();

  GPIO_Configuration();

  TIM2_Configuration();


  // Infinite loop
  while (1)
    {
      if(!trigger && ButtonPressed()) {
        if (pwm > 1800) pwmstep = -50;
        if (pwm <= 1000) pwmstep = 50;
        pwm += pwmstep;
        __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, TIM_CHANNEL_1, pwm);
        trigger = 1;
      } else if (trigger && !ButtonPressed()) {
        trigger = 0;
      }
    }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
