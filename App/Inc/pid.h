#ifndef __PID_H
#define __PID_H

#include <stdint.h>

/* PID控制器结构体 */
typedef struct {
    float Kp;               /* 比例系数 */
    float Ki;               /* 积分系数 */
    float Kd;               /* 微分系数 */
    float target;           /* 目标值 */
    float error;            /* 当前误差 */
    float last_error;       /* 上次误差 */
    float integral;         /* 积分累积 */
    float integral_limit;   /* 积分限幅 */
    float output_limit;     /* 输出限幅 */
    float output;           /* 控制输出 */
} PID_Controller_t;

/* 函数声明 */
void PID_Init(PID_Controller_t *pid, float kp, float ki, float kd);
void PID_SetTarget(PID_Controller_t *pid, float target);
void PID_SetParams(PID_Controller_t *pid, float kp, float ki, float kd);
void PID_SetLimits(PID_Controller_t *pid, float integral_limit, float output_limit);
float PID_Calculate(PID_Controller_t *pid, float measured);
void PID_Reset(PID_Controller_t *pid);

#endif /* __PID_H */
