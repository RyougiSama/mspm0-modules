#include "wit.h"
#include "clock.h"
#include <stdbool.h> // 引入布尔类型

uint8_t wit_dmaBuffer[33];

WIT_Data_t wit_data;

void WIT_Init(void)
{
    
    DL_DMA_setSrcAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t)(&UART_WIT_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &wit_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);

    NVIC_EnableIRQ(UART_WIT_INST_INT_IRQN);
}


// 2. 定义状态机需要的静态变量
static GyroCalibState_t g_calib_state = CALIB_STATE_IDLE;
static uint32_t g_calib_timestamp = 0;
static volatile bool g_is_calib_complete = false; // <--- 新增：校准完成标志位 (volatile 可选，用于防止编译器过度优化)

// /**
//  * @brief 启动陀螺仪校准流程
//  * @note  调用此函数会开始整个校准流程，但函数会立即返回，不会造成程序阻塞。
//  */
void Gyro_Calibration_Start(void)
{
    // 为防止重复执行，仅当状态机空闲时才启动
    if (g_calib_state == CALIB_STATE_IDLE) {
        g_is_calib_complete = false; 
        g_calib_state = CALIB_STATE_SEND_UNLOCK;
    }
}

/**
 * @brief 陀螺仪校准状态机更新函数
 * @note  此函数必须在主循环(while(1))中被【不断地、快速地】调用。
 */
void Gyro_Calibration_Update(void)
{
    // 假设您的UART发送函数是 DL_UART_Main_transmitDataBlocking
    // 假设您的计时函数是 
    
    switch (g_calib_state) {
        case CALIB_STATE_IDLE:
            // 空闲状态，不执行任何操作
            break;

        case CALIB_STATE_SEND_UNLOCK:
            // 步骤1: 发送解锁指令
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0XFF);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0XAA);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X69);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X88);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0XB5);
            
            // 记录当前时间戳
            g_calib_timestamp = tick_ms;
            // 切换到下一个状态：等待200ms
            g_calib_state = CALIB_STATE_WAIT_UNLOCK;
            break;

        case CALIB_STATE_WAIT_UNLOCK:
            // 步骤1.1: 等待200ms
            if (tick_ms - g_calib_timestamp >= 200) {
                // 等待时间到，切换到下一个状态
                g_calib_state = CALIB_STATE_SEND_CALIBRATE;
            }
            break;

        case CALIB_STATE_SEND_CALIBRATE:
            // 步骤2: 发送校准指令
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0XFF);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0XAA);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X01);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X04);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X00);

            // 再次记录当前时间戳
            g_calib_timestamp = tick_ms;
            // 切换到下一个状态：等待3秒
            g_calib_state = CALIB_STATE_WAIT_CALIBRATE;
            break;
            
        case CALIB_STATE_WAIT_CALIBRATE:
            // 步骤2.1: 等待3秒 (3000毫秒)
            if (tick_ms - g_calib_timestamp >= 3000) {
                // 等待时间到，切换到下一个状态
                g_calib_state = CALIB_STATE_SEND_SAVE;
            }
            break;

        case CALIB_STATE_SEND_SAVE:
            // 步骤3: 发送保存指令
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0XFF);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0XAA);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X00);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X00);
            DL_UART_Main_transmitDataBlocking(UART_WIT_INST, 0X00);

            g_is_calib_complete = true;

            // 所有操作完成，返回空闲状态，等待下一次启动
            g_calib_state = CALIB_STATE_IDLE;
            break;
            
        default:
            // 容错处理：如果出现意外状态，则返回空闲
            g_calib_state = CALIB_STATE_IDLE;
            break;
    }
}

bool Is_Gyro_Calib_Complete(void) // <--- 新增：用于外部读取状态的函数
{
    return g_is_calib_complete;
}