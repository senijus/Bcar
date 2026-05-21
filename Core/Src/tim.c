/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration of the TIM instances.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/**
  * @brief TIM1 Initialization Function - 右编码器
  */
void MX_TIM1_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 0xFFFF;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 0x06;
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 0x06;

    if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief TIM2 Initialization Function - PWM输出（电机）
  */
void MX_TIM2_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 72 - 1;       /* 1MHz */
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;        /* 10kHz */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief TIM3 Initialization Function - 左编码器
  */
void MX_TIM3_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0xFFFF;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 0x06;
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 0x06;

    if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief TIM4 Initialization Function - 调度定时器（200Hz，5ms）
  */
void MX_TIM4_Init(void)
{
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 72 - 1;       /* 1MHz */
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 5000 - 1;        /* 5ms -> 200Hz */
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief TIM_PWM MSP Initialization
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
}

/**
  * @brief TIM_Encoder MSP Initialization
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *tim_encoderHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (tim_encoderHandle->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* TIM1 GPIO Configuration: PA8 -> TIM1_CH1, PA9 -> TIM1_CH2 */
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (tim_encoderHandle->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* TIM3 GPIO Configuration: PA6 -> TIM3_CH1, PA7 -> TIM3_CH2 */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
  * @brief TIM_Base MSP Initialization
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{
    if (tim_baseHandle->Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();

        /* TIM4 中断优先级配置 */
        HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM4_IRQn);
    }
}

/**
  * @brief TIM PWM Post Init
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (timHandle->Instance == TIM2) {
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* TIM2 GPIO Configuration: PA0 -> TIM2_CH1, PA1 -> TIM2_CH2 */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
  * @brief TIM_PWM MSP De-Initialization
  */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
}

/**
  * @brief TIM_Encoder MSP De-Initialization
  */
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *tim_encoderHandle)
{
    if (tim_encoderHandle->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8 | GPIO_PIN_9);
    }
    else if (tim_encoderHandle->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6 | GPIO_PIN_7);
    }
}

/**
  * @brief TIM_Base MSP De-Initialization
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle)
{
    if (tim_baseHandle->Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM4_IRQn);
    }
}
