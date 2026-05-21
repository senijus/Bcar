/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for the tim.c file
  ******************************************************************************
  */

#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;   /* 右编码器 */
extern TIM_HandleTypeDef htim2;   /* PWM输出 */
extern TIM_HandleTypeDef htim3;   /* 左编码器 */
extern TIM_HandleTypeDef htim4;   /* 调度定时器 */

/* Function prototypes -------------------------------------------------------*/
void MX_TIM1_Init(void);   /* 右编码器初始化 */
void MX_TIM2_Init(void);   /* PWM输出初始化 */
void MX_TIM3_Init(void);   /* 左编码器初始化 */
void MX_TIM4_Init(void);   /* 调度定时器初始化 */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
