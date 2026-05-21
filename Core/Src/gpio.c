/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration of all used GPIO pins.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/**
  * @brief GPIO Initialization Function
  */
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* 电机方向引脚默认低电平 */
    HAL_GPIO_WritePin(GPIOB, LEFT_IN1_Pin | LEFT_IN2_Pin | RIGHT_IN1_Pin | RIGHT_IN2_Pin, GPIO_PIN_RESET);

    /* LED默认高电平（灭） */
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

    /* 电机方向引脚配置：PB0, PB1, PB10, PB11 */
    GPIO_InitStruct.Pin = LEFT_IN1_Pin | LEFT_IN2_Pin | RIGHT_IN1_Pin | RIGHT_IN2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* LED引脚配置：PC13 */
    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}
