#include "scheduler.h"
#include <stddef.h>  /* for NULL */

/* 任务列表 */
static Scheduler_Task_t tasks[SCHEDULER_MAX_TASKS];
static uint8_t task_count = 0;

/**
 * @brief 初始化调度器
 */
void Scheduler_Init(void)
{
    task_count = 0;
    for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        tasks[i].task_func = NULL;
        tasks[i].period_ms = 0;
        tasks[i].elapsed_ms = 0;
        tasks[i].enabled = 0;
    }
}

/**
 * @brief 添加任务
 * @param task_func 任务函数指针
 * @param period_ms 执行周期（ms）
 * @param enabled 是否使能
 */
void Scheduler_AddTask(void (*task_func)(void), uint16_t period_ms, uint8_t enabled)
{
    if (task_count >= SCHEDULER_MAX_TASKS) return;

    tasks[task_count].task_func = task_func;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].elapsed_ms = 0;
    tasks[task_count].enabled = enabled;
    task_count++;
}

/**
 * @brief 调度器运行（需在定时器中断中调用，周期5ms）
 *
 * 调度逻辑：
 * 1. 遍历所有任务
 * 2. 累加已过时间
 * 3. 如果已过时间 >= 执行周期，且任务已使能，则执行任务
 * 4. 重置已过时间
 */
void Scheduler_Run(void)
{
    for (uint8_t i = 0; i < task_count; i++) {
        if (tasks[i].enabled && tasks[i].task_func != NULL) {
            tasks[i].elapsed_ms += 5;  /* 每次调用增加5ms */

            if (tasks[i].elapsed_ms >= tasks[i].period_ms) {
                tasks[i].elapsed_ms = 0;
                tasks[i].task_func();
            }
        }
    }
}
