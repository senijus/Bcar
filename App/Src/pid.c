#include "pid.h"

/**
 * @brief 初始化PID控制器
 * @param pid PID结构体指针
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 */
void PID_Init(PID_Controller_t *pid, float kp, float ki, float kd)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->target = 0.0f;
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->integral_limit = 5000.0f;
    pid->output_limit = 999.0f;
    pid->output = 0.0f;
}

/**
 * @brief 设置目标值
 * @param pid PID结构体指针
 * @param target 目标值
 */
void PID_SetTarget(PID_Controller_t *pid, float target)
{
    pid->target = target;
}

/**
 * @brief 设置PID参数
 * @param pid PID结构体指针
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 */
void PID_SetParams(PID_Controller_t *pid, float kp, float ki, float kd)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
}

/**
 * @brief 设置限幅值
 * @param pid PID结构体指针
 * @param integral_limit 积分限幅
 * @param output_limit 输出限幅
 */
void PID_SetLimits(PID_Controller_t *pid, float integral_limit, float output_limit)
{
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
}

/**
 * @brief PID计算
 * @param pid PID结构体指针
 * @param measured 测量值
 * @return 控制输出
 *
 * 位置式PID公式：
 * output = Kp * error + Ki * integral + Kd * derivative
 */
float PID_Calculate(PID_Controller_t *pid, float measured)
{
    /* 计算误差 */
    pid->error = pid->target - measured;

    /* 积分累加 */
    pid->integral += pid->error;

    /* 积分限幅（抗饱和） */
    if (pid->integral > pid->integral_limit)
        pid->integral = pid->integral_limit;
    else if (pid->integral < -pid->integral_limit)
        pid->integral = -pid->integral_limit;

    /* 位置式PID计算 */
    float derivative = pid->error - pid->last_error;
    pid->output = pid->Kp * pid->error
                + pid->Ki * pid->integral
                + pid->Kd * derivative;

    /* 保存本次误差 */
    pid->last_error = pid->error;

    /* 输出限幅 */
    if (pid->output > pid->output_limit)
        pid->output = pid->output_limit;
    else if (pid->output < -pid->output_limit)
        pid->output = -pid->output_limit;

    return pid->output;
}

/**
 * @brief 重置PID控制器
 * @param pid PID结构体指针
 */
void PID_Reset(PID_Controller_t *pid)
{
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
}
