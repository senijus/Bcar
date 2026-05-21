/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : STM32 两轮自平衡小车 - 主程序
  ******************************************************************************
  * 控制架构：双闭环级联 PID
  *   外环（速度环，50Hz）→ 输出角度补偿值
  *   内环（角度环，200Hz）→ 输出 PWM 占空比
  *
  * 硬件平台：STM32F103C8T6
  * 开发环境：Keil MDK
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "mpu6050.h"
#include "imu.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"
#include "uart_protocol.h"
#include "scheduler.h"
#include <math.h>

/* Private variables ---------------------------------------------------------*/
static MPU6050_Data_t  mpu_data;
static IMU_Data_t      imu;
static PID_Controller_t pid_angle;
static PID_Controller_t pid_speed;

static float angle_target = 0.0f;   /* 角度目标值（由速度环修正） */
static float speed_target = 0.0f;   /* 速度目标值 */
static int16_t pwm_output = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Task functions */
static void Task_ImuUpdate(void);
static void Task_AngleControl(void);
static void Task_SpeedControl(void);
static void Task_UartReport(void);

/* Callback functions */
static void OnParamSet(uint8_t cmd, float value);
static void OnParamRead(uint8_t cmd);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    HAL_Init();
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_USART2_UART_Init();

    /* MPU6050 初始化与校准 */
    MPU6050_Init(&hi2c1);
    if (!MPU6050_TestConnection()) {
        /* 连接失败，LED 快闪报警 */
        while (1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            HAL_Delay(100);
        }
    }
    MPU6050_Calibrate(&mpu_data, 500);  /* 静止 1 秒校准 */

    /* 姿态解算初始化（5ms 周期） */
    IMU_Init(&imu, 0.005f);

    /* PID 初始化 */
    PID_Init(&pid_angle, 30.0f, 0.0f, 5.0f);
    PID_SetLimits(&pid_angle, 3000.0f, 999.0f);

    PID_Init(&pid_speed, 0.5f, 0.01f, 0.0f);
    PID_SetLimits(&pid_speed, 500.0f, 8.0f);  /* 速度环输出限幅为角度补偿（±8°） */

    /* 电机与编码器 */
    Motor_Init(&htim2);
    Motor_SetDeadzone(50);
    Encoder_Init(&htim3, &htim1);

    /* 串口协议 */
    UART_Proc_Init(&huart2);
    UART_Proc_SetCallbacks(OnParamSet, OnParamRead);

    /* 任务调度 */
    Scheduler_Init();
    Scheduler_AddTask(Task_ImuUpdate,     5, 1);   /* 5ms  - 姿态解算 */
    Scheduler_AddTask(Task_AngleControl,  5, 1);   /* 5ms  - 角度环 */
    Scheduler_AddTask(Task_SpeedControl, 20, 1);   /* 20ms - 速度环 */
    Scheduler_AddTask(Task_UartReport,   50, 1);   /* 50ms - 串口回传 */

    /* 启动 TIM4 中断（5ms 周期） */
    HAL_TIM_Base_Start_IT(&htim4);

    /* 主循环 */
    while (1) {
        UART_Proc_Process();
        __WFI();  /* 等待中断，降低功耗 */
    }
}

/**
  * @brief  姿态解算任务（5ms周期）
  */
static void Task_ImuUpdate(void)
{
    MPU6050_ReadCalibrated(&mpu_data);

    /* 加速度计计算倾角（绕 X 轴旋转） */
    float accel_angle = atan2f(mpu_data.accel_y, mpu_data.accel_z) * RAD_TO_DEG;

    /* 互补滤波融合 */
    IMU_Update(&imu, accel_angle, mpu_data.gyro_x);
}

/**
  * @brief  角度环控制任务（5ms周期）
  */
static void Task_AngleControl(void)
{
    /* 角度环：目标角度 → PWM */
    PID_SetTarget(&pid_angle, angle_target);
    pwm_output = (int16_t)PID_Calculate(&pid_angle, imu.angle);

    Motor_SetPWM(pwm_output, pwm_output);
}

/**
  * @brief  速度环控制任务（20ms周期）
  */
static void Task_SpeedControl(void)
{
    /* 读取编码器速度 */
    float speed = Encoder_GetSpeed();

    /* 速度环：目标速度 → 角度补偿 */
    PID_SetTarget(&pid_speed, speed_target);
    float angle_comp = PID_Calculate(&pid_speed, speed);

    /* 修正角度目标 */
    angle_target = 0.0f + angle_comp;
}

/**
  * @brief  串口回传任务（50ms周期）
  */
static void Task_UartReport(void)
{
    float speed = Encoder_GetSpeed();
    UART_Proc_SendStatus(imu.angle, imu.gyro, speed, pwm_output, pwm_output);
}

/**
  * @brief  参数设置回调
  */
static void OnParamSet(uint8_t cmd, float value)
{
    switch (cmd) {
    case CMD_SET_ANGLE_KP: pid_angle.Kp = value; break;
    case CMD_SET_ANGLE_KI: pid_angle.Ki = value; break;
    case CMD_SET_ANGLE_KD: pid_angle.Kd = value; break;
    case CMD_SET_SPEED_KP: pid_speed.Kp = value; break;
    case CMD_SET_SPEED_KI: pid_speed.Ki = value; break;
    case CMD_SET_SPEED_KD: pid_speed.Kd = value; break;
    case CMD_SET_SPEED_TGT: speed_target = value; break;
    default: break;
    }
}

/**
  * @brief  参数读取回调
  */
static void OnParamRead(uint8_t cmd)
{
    switch (cmd) {
    case CMD_READ_PARAMS:
        UART_Proc_SendParams(pid_angle.Kp, pid_angle.Ki, pid_angle.Kd,
                             pid_speed.Kp, pid_speed.Ki, pid_speed.Kd);
        break;
    case CMD_READ_STATUS: {
        float speed = Encoder_GetSpeed();
        UART_Proc_SendStatus(imu.angle, imu.gyro, speed, pwm_output, pwm_output);
        break;
    }
    default: break;
    }
}

/**
  * @brief  定时器中断回调
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4) {
        Scheduler_Run();
    }
}

/**
  * @brief  串口接收完成回调
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        UART_Proc_RxCallback();
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
    }
}
