#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* 函数声明 */
void Encoder_Init(TIM_HandleTypeDef *htim_left, TIM_HandleTypeDef *htim_right);
int16_t Encoder_ReadLeft(void);
int16_t Encoder_ReadRight(void);
float Encoder_GetSpeed(void);

#endif /* __ENCODER_H */
