#ifndef __ADVANCED_LINE_FOLLOWER_H
#define __ADVANCED_LINE_FOLLOWER_H

#include <stdbool.h>
#include <stdint.h>

// 定义小车运行状态
typedef enum {
    STATE_IDLE,             // 0: 空闲状态
    STATE_TRACKING,         // 1: 正常循迹
    STATE_CORNER_STOP,      // 2: 检测到弯道，停车
    STATE_CORNER_FORWARD,   // 3: 拐弯前，前进一小步
    STATE_CORNER_TURNING,   // 4: 原地旋转寻找黑线
    STATE_FINISHED          // 5: 任务完成
} CarState;

// --- 对外接口函数 ---

void ALF_Init(void);
void ALF_Start(void);
void ALF_Stop(void);
void ALF_Task(void);
bool ALF_IsFinished(void);

extern CarState g_car_state;

extern uint8_t g_lap_count;
extern uint8_t g_lap_target;

#endif