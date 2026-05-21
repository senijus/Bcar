#ifndef __IMU_H
#define __IMU_H

#include <stdint.h>

/* 弧度转角度 */
#define RAD_TO_DEG  57.295779513082320876798154814105f

/* 角度转弧度 */
#define DEG_TO_RAD  0.01745329251994329576923690768489f

/* IMU数据结构 */
typedef struct {
    float angle;        /* 融合后的倾角（度） */
    float gyro;         /* 角速度（°/s） */
    float accel_angle;  /* 加速度计计算的角度（度） */
    float dt;           /* 采样周期（秒） */
    float alpha;        /* 互补滤波系数 */
} IMU_Data_t;

/* 函数声明 */
void IMU_Init(IMU_Data_t *imu, float dt);
void IMU_Update(IMU_Data_t *imu, float accel_angle, float gyro_rate);
void IMU_SetAlpha(IMU_Data_t *imu, float alpha);

#endif /* __IMU_H */
