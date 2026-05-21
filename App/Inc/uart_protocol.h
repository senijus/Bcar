#ifndef __UART_PROTOCOL_H
#define __UART_PROTOCOL_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* 协议帧定义 */
#define FRAME_HEADER        0xAA
#define FRAME_TAIL          0x55
#define FRAME_MAX_LEN       32

/* 上位机下发命令 */
#define CMD_SET_ANGLE_KP    0x01
#define CMD_SET_ANGLE_KI    0x02
#define CMD_SET_ANGLE_KD    0x03
#define CMD_SET_SPEED_KP    0x04
#define CMD_SET_SPEED_KI    0x05
#define CMD_SET_SPEED_KD    0x06
#define CMD_SET_SPEED_TGT   0x10
#define CMD_READ_PARAMS     0x20
#define CMD_READ_STATUS     0x30

/* 下位机回传命令 */
#define CMD_REPORT_PARAMS   0x81
#define CMD_REPORT_STATUS   0x82
#define CMD_ACK             0x83

/* 接收状态机 */
typedef enum {
    STATE_IDLE,
    STATE_HEADER,
    STATE_CMD,
    STATE_LEN,
    STATE_DATA,
    STATE_CHECKSUM,
    STATE_TAIL
} UART_State_t;

/* 回调函数类型 */
typedef void (*ParamSetCallback_t)(uint8_t cmd, float value);
typedef void (*ParamReadCallback_t)(uint8_t cmd);

/* 函数声明 */
void UART_Proc_Init(UART_HandleTypeDef *huart);
void UART_Proc_Process(void);
void UART_Proc_SendStatus(float angle, float gyro, float speed, int16_t pwm_left, int16_t pwm_right);
void UART_Proc_SendParams(float angle_kp, float angle_ki, float angle_kd,
                          float speed_kp, float speed_ki, float speed_kd);
void UART_Proc_SetCallbacks(ParamSetCallback_t set_cb, ParamReadCallback_t read_cb);

/* 接收中断回调（需在stm32f1xx_it.c中调用） */
void UART_Proc_RxCallback(void);

#endif /* __UART_PROTOCOL_H */
