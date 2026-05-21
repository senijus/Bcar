#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>

/* 最大任务数量 */
#define SCHEDULER_MAX_TASKS  8

/* 任务结构体 */
typedef struct {
    void (*task_func)(void);   /* 任务函数指针 */
    uint16_t period_ms;        /* 执行周期（ms） */
    uint16_t elapsed_ms;       /* 已过时间（ms） */
    uint8_t  enabled;          /* 是否使能 */
} Scheduler_Task_t;

/* 函数声明 */
void Scheduler_Init(void);
void Scheduler_AddTask(void (*task_func)(void), uint16_t period_ms, uint8_t enabled);
void Scheduler_Run(void);

#endif /* __SCHEDULER_H */
