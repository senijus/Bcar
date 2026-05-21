#include "imu.h"

/**
 * @brief 初始化IMU
 * @param imu IMU数据结构指针
 * @param dt 采样周期（秒）
 */
void IMU_Init(IMU_Data_t *imu, float dt)
{
    imu->angle = 0.0f;
    imu->gyro = 0.0f;
    imu->accel_angle = 0.0f;
    imu->dt = dt;
    imu->alpha = 0.98f;  /* 默认互补滤波系数 */
}

/**
 * @brief 更新IMU数据（互补滤波）
 * @param imu IMU数据结构指针
 * @param accel_angle 加速度计计算的角度（度）
 * @param gyro_rate 陀螺仪角速度（°/s）
 *
 * 互补滤波公式：
 * angle = alpha * (angle + gyro_rate * dt) + (1 - alpha) * accel_angle
 *
 * 说明：
 * - alpha越大，越信任陀螺仪（动态响应好，但有漂移）
 * - alpha越小，越信任加速度计（静态准确，但有噪声）
 * - 推荐值：0.95~0.99
 */
void IMU_Update(IMU_Data_t *imu, float accel_angle, float gyro_rate)
{
    /* 保存加速度计角度 */
    imu->accel_angle = accel_angle;

    /* 保存陀螺仪角速度 */
    imu->gyro = gyro_rate;

    /* 互补滤波融合 */
    /* 陀螺仪积分得到角度变化量 */
    float gyro_angle = imu->angle + gyro_rate * imu->dt;

    /* 互补滤波：加权融合陀螺仪和加速度计 */
    imu->angle = imu->alpha * gyro_angle + (1.0f - imu->alpha) * accel_angle;
}

/**
 * @brief 设置互补滤波系数
 * @param imu IMU数据结构指针
 * @param alpha 滤波系数（0~1）
 */
void IMU_SetAlpha(IMU_Data_t *imu, float alpha)
{
    /* 限制范围 */
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    imu->alpha = alpha;
}
