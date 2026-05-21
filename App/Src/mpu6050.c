#include "mpu6050.h"
#include <math.h>

static I2C_HandleTypeDef *hi2c_mpu6050;

/**
 * @brief 初始化MPU6050
 * @param hi2c I2C句柄指针
 */
void MPU6050_Init(I2C_HandleTypeDef *hi2c)
{
    hi2c_mpu6050 = hi2c;

    /* 复位MPU6050 */
    MPU6050_WriteByte(MPU6050_REG_PWR_MGMT_1, 0x80);
    HAL_Delay(100);

    /* 唤醒MPU6050，使用X轴陀螺仪作为时钟源 */
    MPU6050_WriteByte(MPU6050_REG_PWR_MGMT_1, 0x01);
    HAL_Delay(10);

    /* 设置采样率：1kHz / (7+1) = 125Hz */
    MPU6050_WriteByte(MPU6050_REG_SMPLRT_DIV, 0x07);

    /* 设置低通滤波：带宽5Hz */
    MPU6050_WriteByte(MPU6050_REG_CONFIG, 0x06);

    /* 设置陀螺仪量程：±2000°/s */
    MPU6050_WriteByte(MPU6050_REG_GYRO_CONFIG, 0x18);

    /* 设置加速度计量程：±4g */
    MPU6050_WriteByte(MPU6050_REG_ACCEL_CONFIG, 0x01);
}

/**
 * @brief 测试MPU6050连接
 * @return 1: 连接成功, 0: 连接失败
 */
uint8_t MPU6050_TestConnection(void)
{
    uint8_t who_am_i = MPU6050_ReadByte(MPU6050_REG_WHO_AM_I);
    return (who_am_i == 0x68) ? 1 : 0;
}

/**
 * @brief 读取原始数据
 * @param data 数据结构指针
 */
void MPU6050_ReadRaw(MPU6050_Data_t *data)
{
    uint8_t buf[14];

    /* 读取14字节数据：加速度(6) + 温度(2) + 陀螺仪(6) */
    MPU6050_ReadBytes(MPU6050_REG_ACCEL_XOUT_H, buf, 14);

    /* 解析加速度数据 */
    data->accel_x = (int16_t)((buf[0] << 8) | buf[1]);
    data->accel_y = (int16_t)((buf[2] << 8) | buf[3]);
    data->accel_z = (int16_t)((buf[4] << 8) | buf[5]);

    /* 解析陀螺仪数据 */
    data->gyro_x = (int16_t)((buf[8] << 8) | buf[9]);
    data->gyro_y = (int16_t)((buf[10] << 8) | buf[11]);
    data->gyro_z = (int16_t)((buf[12] << 8) | buf[13]);
}

/**
 * @brief 读取校准后的数据
 * @param data 数据结构指针
 */
void MPU6050_ReadCalibrated(MPU6050_Data_t *data)
{
    /* 先读取原始数据 */
    MPU6050_ReadRaw(data);

    /* 减去偏移量，转换为物理单位 */
    /* 加速度：±4g量程，灵敏度8192 LSB/g */
    data->accel_x_cal = (data->accel_x - data->accel_x_offset) / 8192.0f;
    data->accel_y_cal = (data->accel_y - data->accel_y_offset) / 8192.0f;
    data->accel_z_cal = (data->accel_z - data->accel_z_offset) / 8192.0f;

    /* 陀螺仪：±2000°/s量程，灵敏度16.4 LSB/(°/s) */
    data->gyro_x_cal = (data->gyro_x - data->gyro_x_offset) / 16.4f;
    data->gyro_y_cal = (data->gyro_y - data->gyro_y_offset) / 16.4f;
    data->gyro_z_cal = (data->gyro_z - data->gyro_z_offset) / 16.4f;
}

/**
 * @brief 校准MPU6050（需静止状态调用）
 * @param data 数据结构指针
 * @param samples 采样次数
 */
void MPU6050_Calibrate(MPU6050_Data_t *data, uint16_t samples)
{
    float sum_ax = 0, sum_ay = 0, sum_az = 0;
    float sum_gx = 0, sum_gy = 0, sum_gz = 0;

    /* 采集多组数据求平均 */
    for (uint16_t i = 0; i < samples; i++) {
        MPU6050_ReadRaw(data);
        sum_ax += data->accel_x;
        sum_ay += data->accel_y;
        sum_az += data->accel_z;
        sum_gx += data->gyro_x;
        sum_gy += data->gyro_y;
        sum_gz += data->gyro_z;
        HAL_Delay(2);
    }

    /* 计算偏移量 */
    data->accel_x_offset = sum_ax / samples;
    data->accel_y_offset = sum_ay / samples;
    data->accel_z_offset = (sum_az / samples) - 8192.0f;  /* 静止时Z轴应为1g */

    data->gyro_x_offset = sum_gx / samples;
    data->gyro_y_offset = sum_gy / samples;
    data->gyro_z_offset = sum_gz / samples;
}

/**
 * @brief 读取一个字节
 * @param reg 寄存器地址
 * @return 读取的字节
 */
uint8_t MPU6050_ReadByte(uint8_t reg)
{
    uint8_t value;
    HAL_I2C_Mem_Read(hi2c_mpu6050, MPU6050_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
    return value;
}

/**
 * @brief 写入一个字节
 * @param reg 寄存器地址
 * @param value 要写入的字节
 */
void MPU6050_WriteByte(uint8_t reg, uint8_t value)
{
    HAL_I2C_Mem_Write(hi2c_mpu6050, MPU6050_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
}

/**
 * @brief 读取多个字节
 * @param reg 起始寄存器地址
 * @param buf 数据缓冲区
 * @param len 读取长度
 */
void MPU6050_ReadBytes(uint8_t reg, uint8_t *buf, uint16_t len)
{
    HAL_I2C_Mem_Read(hi2c_mpu6050, MPU6050_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY);
}
