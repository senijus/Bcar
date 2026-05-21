#include "encoder.h"

static TIM_HandleTypeDef *htim_left_enc;
static TIM_HandleTypeDef *htim_right_enc;

/**
 * @brief 初始化编码器
 * @param htim_left 左编码器定时器句柄指针
 * @param htim_right 右编码器定时器句柄指针
 */
void Encoder_Init(TIM_HandleTypeDef *htim_left, TIM_HandleTypeDef *htim_right)
{
    htim_left_enc = htim_left;
    htim_right_enc = htim_right;

    /* 启动编码器模式 */
    HAL_TIM_Encoder_Start(htim_left_enc, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(htim_right_enc, TIM_CHANNEL_ALL);
}

/**
 * @brief 读取左编码器计数值
 * @return 编码器计数值（读取后清零）
 */
int16_t Encoder_ReadLeft(void)
{
    int16_t count = (int16_t)__HAL_TIM_GET_COUNTER(htim_left_enc);
    __HAL_TIM_SET_COUNTER(htim_left_enc, 0);
    return count;
}

/**
 * @brief 读取右编码器计数值
 * @return 编码器计数值（读取后清零）
 */
int16_t Encoder_ReadRight(void)
{
    int16_t count = (int16_t)__HAL_TIM_GET_COUNTER(htim_right_enc);
    __HAL_TIM_SET_COUNTER(htim_right_enc, 0);
    return count;
}

/**
 * @brief 获取平均速度
 * @return 左右编码器平均值
 */
float Encoder_GetSpeed(void)
{
    int16_t left = Encoder_ReadLeft();
    int16_t right = Encoder_ReadRight();
    return (float)(left + right) * 0.5f;
}
