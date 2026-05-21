#include "motor.h"

static TIM_HandleTypeDef *htim_motor;
static int16_t deadzone = MOTOR_PWM_MIN;

/* GPIO引脚定义 - 根据实际硬件修改 */
#define LEFT_IN1_PORT   GPIOB
#define LEFT_IN1_PIN    GPIO_PIN_0
#define LEFT_IN2_PORT   GPIOB
#define LEFT_IN2_PIN    GPIO_PIN_1
#define RIGHT_IN1_PORT  GPIOB
#define RIGHT_IN1_PIN   GPIO_PIN_10
#define RIGHT_IN2_PORT  GPIOB
#define RIGHT_IN2_PIN   GPIO_PIN_11

/**
 * @brief 初始化电机驱动
 * @param htim PWM定时器句柄指针
 */
void Motor_Init(TIM_HandleTypeDef *htim)
{
    htim_motor = htim;

    /* 启动PWM输出 */
    HAL_TIM_PWM_Start(htim_motor, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(htim_motor, TIM_CHANNEL_2);

    /* 初始停止 */
    Motor_Stop();
}

/**
 * @brief 设置死区补偿值
 * @param dz 死区值
 */
void Motor_SetDeadzone(int16_t dz)
{
    deadzone = dz;
}

/**
 * @brief 应用死区补偿
 * @param pwm 原始PWM值
 * @return 补偿后的PWM值
 */
static int16_t ApplyDeadzone(int16_t pwm)
{
    if (pwm == 0) return 0;

    int16_t sign = (pwm > 0) ? 1 : -1;
    int16_t abs_pwm = (pwm > 0) ? pwm : -pwm;

    if (abs_pwm < deadzone)
        abs_pwm = deadzone;
    if (abs_pwm > MOTOR_PWM_MAX)
        abs_pwm = MOTOR_PWM_MAX;

    return sign * abs_pwm;
}

/**
 * @brief 设置单个电机PWM
 * @param pwm PWM值（正负控制方向）
 * @param in1_port IN1端口
 * @param in1_pin IN1引脚
 * @param in2_port IN2端口
 * @param in2_pin IN2引脚
 * @param channel PWM通道
 */
static void Motor_SetSingle(int16_t pwm, GPIO_TypeDef *in1_port, uint16_t in1_pin,
                             GPIO_TypeDef *in2_port, uint16_t in2_pin, uint32_t channel)
{
    pwm = ApplyDeadzone(pwm);

    if (pwm > 0) {
        /* 正转 */
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(htim_motor, channel, pwm);
    } else if (pwm < 0) {
        /* 反转 */
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(htim_motor, channel, -pwm);
    } else {
        /* 停止 */
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(htim_motor, channel, 0);
    }
}

/**
 * @brief 设置左右电机PWM
 * @param left_pwm 左电机PWM（-999 ~ +999）
 * @param right_pwm 右电机PWM（-999 ~ +999）
 */
void Motor_SetPWM(int16_t left_pwm, int16_t right_pwm)
{
    Motor_SetSingle(left_pwm, LEFT_IN1_PORT, LEFT_IN1_PIN, LEFT_IN2_PORT, LEFT_IN2_PIN, TIM_CHANNEL_1);
    Motor_SetSingle(right_pwm, RIGHT_IN1_PORT, RIGHT_IN1_PIN, RIGHT_IN2_PORT, RIGHT_IN2_PIN, TIM_CHANNEL_2);
}

/**
 * @brief 停止电机
 */
void Motor_Stop(void)
{
    HAL_GPIO_WritePin(LEFT_IN1_PORT, LEFT_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_IN2_PORT, LEFT_IN2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_IN1_PORT, RIGHT_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_IN2_PORT, RIGHT_IN2_PIN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(htim_motor, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(htim_motor, TIM_CHANNEL_2, 0);
}
