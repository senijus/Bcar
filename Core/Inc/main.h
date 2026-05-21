/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
/* 电机方向控制引脚 */
#define LEFT_IN1_Pin        GPIO_PIN_0
#define LEFT_IN1_GPIO_Port  GPIOB
#define LEFT_IN2_Pin        GPIO_PIN_1
#define LEFT_IN2_GPIO_Port  GPIOB
#define RIGHT_IN1_Pin       GPIO_PIN_10
#define RIGHT_IN1_GPIO_Port GPIOB
#define RIGHT_IN2_Pin       GPIO_PIN_11
#define RIGHT_IN2_GPIO_Port GPIOB

/* 板载LED */
#define LED_Pin             GPIO_PIN_13
#define LED_GPIO_Port       GPIOC

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
