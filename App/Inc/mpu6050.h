#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* MPU6050 I2C地址 */
#define MPU6050_ADDRESS         0xD0    // AD0 = 0: 0x68 << 1 = 0xD0

/* MPU6050寄存器地址 */
#define MPU6050_REG_SMPLRT_DIV  0x19
#define MPU6050_REG_CONFIG      0x1A
#define MPU6050_REG_GYRO_CONFIG 0x1B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_TEMP_OUT_H  0x41
#define MPU6050_REG_GYRO_XOUT_H 0x43
#define MPU6050_REG_PWR_MGMT_1  0x6B
#define MPU6050_REG_WHO_AM_I    0x75

/* MPU6050数据结构 */
typedef struct {
    /* 原始数据 */
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;

    /* 校准偏移量 */
    float accel_x_offset, accel_y_offset, accel_z_offset;
    float gyro_x_offset, gyro_y_offset, gyro_z_offset;

    /* 校准后的数据（单位：g和deg/s） */
    float accel_x_cal, accel_y_cal, accel_z_cal;
    float gyro_x_cal, gyro_y_cal, gyro_z_cal;
} MPU6050_Data_t;

/* 函数声明 */
void MPU6050_Init(I2C_HandleTypeDef *hi2c);
uint8_t MPU6050_TestConnection(void);
void MPU6050_ReadRaw(MPU6050_Data_t *data);
void MPU6050_ReadCalibrated(MPU6050_Data_t *data);
void MPU6050_Calibrate(MPU6050_Data_t *data, uint16_t samples);

/* 寄存器读写 */
uint8_t MPU6050_ReadByte(uint8_t reg);
void MPU6050_WriteByte(uint8_t reg, uint8_t value);
void MPU6050_ReadBytes(uint8_t reg, uint8_t *buf, uint16_t len);

#endif /* __MPU6050_H */
