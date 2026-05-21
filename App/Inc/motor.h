#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* PWM参数 */
#define MOTOR_PWM_MAX       999     /* ARR值 (10kHz @ 72MHz: PSC=72-1, ARR=1000-1) */
#define MOTOR_PWM_MIN       50      /* 死区补偿最小值 */

/* 电机编号 */
#define MOTOR_LEFT          0
#define MOTOR_RIGHT         1

/* 函数声明 */
void Motor_Init(TIM_HandleTypeDef *htim);
void Motor_SetPWM(int16_t left_pwm, int16_t right_pwm);
void Motor_Stop(void);
void Motor_SetDeadzone(int16_t deadzone);

#endif /* __MOTOR_H */
