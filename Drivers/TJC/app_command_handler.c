#include "app_command_handler.h"

// --- 包含您项目中所有需要的模块头文件 ---
#include "motor.h"
#include "pid.h"
#include "wit.h" // 假设 gyro/wit 功能在此
// #include "gray_sensor.h" // 假设有灰度传感器模块
#include "ti_msp_dl_config.h" // 包含硬件定义

// --- 【新增】定义全局波形发送标志位 ---
volatile bool g_isSineWaveActive = false;


/**
 * @brief 处理从串口解析出的有效指令
 * @note  此函数完全替代了您原来的 Key_PID_MDF() 函数
 */
void App_ProcessSerialCommand(uint8_t page_id, uint8_t key_value)
{
    // 第一层：根据页面ID，进入不同的功能模块
    switch (page_id)
    {
        // ===============================================================
        //  【新增】系统级指令处理
        // ===============================================================
        case PAGE_ID_SYSTEM_CMD:
        {
            switch (key_value)
            {
                case CMD_START_SINE_WAVE: // 0x00: 启动波形发送
                    g_isSineWaveActive = true;
                    break;
                case CMD_STOP_SINE_WAVE: // 0x01: 停止波形发送
                    g_isSineWaveActive = false;
                    break;
                default:
                    break;
            }
            break;
        }

        // ===============================================================
        //  主控制页面 (对应您原来的 key_mode == 0)
        // ===============================================================
        case PAGE_ID_MAIN_CONTROL:
        {
            switch (key_value)
            {
                case 1: // 启动电机，目标速度1000
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    // motor_status = 1;
                    // Motor_On();
                    // pid_init(&g_motorA, DELTA_PID, 0, 0.19, 0);
                    // pid_init(&g_motorB, DELTA_PID, 0, 0.19, 0);
                    // motor_target_set(1000, 1000);
                    // motor_start = tick_ms;
                    break;
                case 2: // 启动灰度传感器任务
                    start_gray_sensor_task = true;
                    Motor_On();
                    pid_init(&g_motorA, DELTA_PID, 0, 0.19, 0);
                    pid_init(&g_motorB, DELTA_PID, 0, 0.19, 0);
                    break;
                case 3: // 停止电机
                    Motor_Stop();
                    break;
                case 5: // 开始陀螺仪校准
                    Gyro_Calibration_Start(); // 假设函数名为这个
                    break;
                default:
                    break;
            }
            break;
        }

        // ===============================================================
        //  电机A PID调参页面 (对应您原来的 key_mode == 1)
        // ===============================================================
        case PAGE_ID_PID_TUNE_A:
        {
            switch (key_value)
            {
                // P参数调节
                case 1: g_motorA.p += 0.5f; break;  // P++ (大步进)
                case 2: g_motorA.p -= 0.5f; break;  // P-- (大步进)
                case 3: g_motorA.p += 0.01f; break; // P+ (小步进)
                case 4: g_motorA.p -= 0.01f; break; // P- (小步进)
                // I参数调节
                case 5: g_motorA.i += 0.5f; break;  // I++
                case 6: g_motorA.i -= 0.5f; break;  // I--
                case 7: g_motorA.i += 0.01f; break; // I+
                case 8: g_motorA.i -= 0.01f; break; // I-
                // D参数调节
                case 9:  g_motorA.d += 0.5f; break;  // D++
                case 10: g_motorA.d -= 0.5f; break;  // D--
                case 11: g_motorA.d += 0.01f; break; // D+
                case 12: g_motorA.d -= 0.01f; break; // D-
                // 测试电机
                case 13: // 以设定好的PID参数，启动电机A
                    motor_target_set(100, 0);
                    pid_init(&g_motorA, DELTA_PID, g_motorA.p, g_motorA.i, g_motorA.d);
                    Motor_On();
                    break;
                case 14: // 停止测试
                    Motor_Stop();
                    break;
                default:
                    break;
            }
            // 可以在这里通过调试串口打印更新后的PID值
            // printf("Motor A PID: P=%.2f, I=%.2f, D=%.2f\r\n", g_motorA.p, g_motorA.i, g_motorA.d);
            break;
        }

        // ===============================================================
        //  电机B PID调参页面 (对应您原来的 key_mode == 2)
        // ===============================================================
        case PAGE_ID_PID_TUNE_B:
        {
            // 逻辑与电机A完全相同
            switch (key_value)
            {
                case 1: g_motorB.p += 0.5f; break;
                case 2: g_motorB.p -= 0.5f; break;
                // ... 以此类推 ...
                case 13: // 以设定好的PID参数，启动电机B
                    motor_target_set(0, 100);
                    pid_init(&g_motorB, DELTA_PID, g_motorB.p, g_motorB.i, g_motorB.d);
                    Motor_On();
                    break;
                case 14: // 停止测试
                    Motor_Stop();
                    break;
                default:
                    break;
            }
            break;
        }

        // ===============================================================
        //  角度环 PID调参页面 (对应您原来的 key_mode == 3)
        // ===============================================================
        case PAGE_ID_PID_TUNE_YAW:
        {
            switch (key_value)
            {
                case 1: g_angle.p += 0.5f; break;
                case 2: g_angle.p -= 0.5f; break;
                // ... 以此类推 ...
                case 13: // 以默认参数启动角度环测试
                    // motor_status = 1;
                    // motor_start = tick_ms;
                    Motor_On();
                    pid_init(&g_motorA, DELTA_PID, 0, 0.19, 0);
                    pid_init(&g_motorB, DELTA_PID, 0, 0.19, 0);
                    pid_init(&g_angle, POSITION_PID, 0.22, 0 ,1.37);
                    break;
                case 14: // 停止测试
                    // motor_status = 0;
                    Motor_Stop();
                    break;
                case 15: // 以当前调好的参数启动角度环测试
                    // motor_status = 1;
                    // motor_start = tick_ms;
                    Motor_On();
                    pid_init(&g_motorA, DELTA_PID, 0, 0.19, 0);
                    pid_init(&g_motorB, DELTA_PID, 0, 0.19, 0);
                    pid_init(&g_angle, POSITION_PID, g_angle.p, g_angle.i, g_angle.d);
                    break;
                default:
                    break;
            }
            break;
        }

        default:
            // 未知的页面ID
            break;
    }
}
