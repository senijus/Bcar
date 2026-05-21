#include "uart_protocol.h"
#include <string.h>

static UART_HandleTypeDef *huart_proc;

/* 接收缓冲区 */
static uint8_t rx_buf[FRAME_MAX_LEN];
static uint8_t rx_byte;
static uint8_t rx_index;
static uint8_t rx_len;
static uint8_t rx_cmd;
static uint8_t rx_checksum;
static UART_State_t rx_state;

/* 回调函数 */
static ParamSetCallback_t param_set_callback = NULL;
static ParamReadCallback_t param_read_callback = NULL;

/**
 * @brief 初始化串口协议
 * @param huart 串口句柄指针
 */
void UART_Proc_Init(UART_HandleTypeDef *huart)
{
    huart_proc = huart;
    rx_state = STATE_IDLE;
    rx_index = 0;

    /* 启动接收中断 */
    HAL_UART_Receive_IT(huart_proc, &rx_byte, 1);
}

/**
 * @brief 设置回调函数
 * @param set_cb 参数设置回调
 * @param read_cb 参数读取回调
 */
void UART_Proc_SetCallbacks(ParamSetCallback_t set_cb, ParamReadCallback_t read_cb)
{
    param_set_callback = set_cb;
    param_read_callback = read_cb;
}

/**
 * @brief 发送一个字节
 * @param data 字节数据
 */
static void UART_SendByte(uint8_t data)
{
    HAL_UART_Transmit(huart_proc, &data, 1, HAL_MAX_DELAY);
}

/**
 * @brief 发送缓冲区
 * @param data 数据指针
 * @param len 数据长度
 */
static void UART_SendBuffer(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(huart_proc, data, len, HAL_MAX_DELAY);
}

/**
 * @brief 计算校验和
 * @param data 数据指针
 * @param len 数据长度
 * @return 校验和
 */
static uint8_t CalcChecksum(uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

/**
 * @brief 处理接收到的命令
 */
static void ProcessCommand(void)
{
    if (rx_cmd >= CMD_SET_ANGLE_KP && rx_cmd <= CMD_SET_SPEED_TGT) {
        /* 参数设置命令 */
        if (rx_len == 4 && param_set_callback != NULL) {
            float value;
            memcpy(&value, rx_buf, 4);
            param_set_callback(rx_cmd, value);
        }
        /* 发送ACK */
        uint8_t ack[5] = {FRAME_HEADER, CMD_ACK, 1, 0x00, FRAME_TAIL};
        ack[4] = CalcChecksum(ack, 4);
        UART_SendBuffer(ack, 5);
    } else if (rx_cmd == CMD_READ_PARAMS || rx_cmd == CMD_READ_STATUS) {
        /* 参数读取命令 */
        if (param_read_callback != NULL) {
            param_read_callback(rx_cmd);
        }
    }
}

/**
 * @brief 串口接收中断回调
 * @note  需在HAL_UART_RxCpltCallback中调用
 */
void UART_Proc_RxCallback(void)
{
    switch (rx_state) {
    case STATE_IDLE:
        if (rx_byte == FRAME_HEADER) {
            rx_state = STATE_HEADER;
            rx_index = 0;
        }
        break;

    case STATE_HEADER:
        rx_cmd = rx_byte;
        rx_state = STATE_CMD;
        break;

    case STATE_CMD:
        rx_len = rx_byte;
        if (rx_len > FRAME_MAX_LEN - 6) {
            rx_state = STATE_IDLE;  /* 长度超限，丢弃 */
        } else {
            rx_state = STATE_LEN;
            rx_index = 0;
        }
        break;

    case STATE_LEN:
        if (rx_len > 0) {
            rx_buf[rx_index++] = rx_byte;
            if (rx_index >= rx_len) {
                rx_state = STATE_DATA;
            }
        } else {
            rx_state = STATE_DATA;
        }
        break;

    case STATE_DATA:
        rx_checksum = rx_byte;
        rx_state = STATE_CHECKSUM;
        break;

    case STATE_CHECKSUM:
        if (rx_byte == FRAME_TAIL) {
            /* 验证校验和 */
            uint8_t calc_sum = FRAME_HEADER + rx_cmd + rx_len;
            for (uint8_t i = 0; i < rx_len; i++) {
                calc_sum += rx_buf[i];
            }
            if (calc_sum == rx_checksum) {
                ProcessCommand();
            }
        }
        rx_state = STATE_IDLE;
        break;

    default:
        rx_state = STATE_IDLE;
        break;
    }

    /* 继续接收下一个字节 */
    HAL_UART_Receive_IT(huart_proc, &rx_byte, 1);
}

/**
 * @brief 处理串口数据（主循环调用）
 */
void UART_Proc_Process(void)
{
    /* 目前使用中断接收，此函数预留 */
}

/**
 * @brief 发送状态数据
 * @param angle 倾角
 * @param gyro 角速度
 * @param speed 速度
 * @param pwm_left 左电机PWM
 * @param pwm_right 右电机PWM
 */
void UART_Proc_SendStatus(float angle, float gyro, float speed, int16_t pwm_left, int16_t pwm_right)
{
    uint8_t buf[20];
    buf[0] = FRAME_HEADER;
    buf[1] = CMD_REPORT_STATUS;
    buf[2] = 16;  /* 数据长度 */

    /* 填充数据 */
    memcpy(&buf[3], &angle, 4);
    memcpy(&buf[7], &gyro, 4);
    memcpy(&buf[11], &speed, 4);
    buf[15] = (uint8_t)(pwm_left & 0xFF);
    buf[16] = (uint8_t)((pwm_left >> 8) & 0xFF);
    buf[17] = (uint8_t)(pwm_right & 0xFF);

    /* 校验和 */
    buf[18] = CalcChecksum(buf, 18);
    buf[19] = FRAME_TAIL;

    UART_SendBuffer(buf, 20);
}

/**
 * @brief 发送参数数据
 * @param angle_kp 角度环Kp
 * @param angle_ki 角度环Ki
 * @param angle_kd 角度环Kd
 * @param speed_kp 速度环Kp
 * @param speed_ki 速度环Ki
 * @param speed_kd 速度环Kd
 */
void UART_Proc_SendParams(float angle_kp, float angle_ki, float angle_kd,
                          float speed_kp, float speed_ki, float speed_kd)
{
    uint8_t buf[29];
    buf[0] = FRAME_HEADER;
    buf[1] = CMD_REPORT_PARAMS;
    buf[2] = 24;  /* 数据长度 */

    /* 填充数据 */
    memcpy(&buf[3], &angle_kp, 4);
    memcpy(&buf[7], &angle_ki, 4);
    memcpy(&buf[11], &angle_kd, 4);
    memcpy(&buf[15], &speed_kp, 4);
    memcpy(&buf[19], &speed_ki, 4);
    memcpy(&buf[23], &speed_kd, 4);

    /* 校验和 */
    buf[27] = CalcChecksum(buf, 27);
    buf[28] = FRAME_TAIL;

    UART_SendBuffer(buf, 29);
}
