#include "advanced_line_follower.h"
#include "ganv_user.h"
#include "pid.h"
#include "motor.h"
#include "clock.h"
// #include "wit.h" // 不再需要陀螺仪
// #include "math.h"  // 不再需要fabs()

// ================== 任务可配置参数 ==================
#define TARGET_LAPS              20      // 目标圈数
#define CORNER_FORWARD_DURATION  120    // 到达拐角后，向前直行的毫秒数 (需调试)
#define PIVOT_TURN_SPEED         8     // 原地旋转时的电机目标速度 (需调试)
#define TASK_EXECUTION_INTERVAL  10     // 任务执行间隔 (ms)
// ====================================================


// 模块内部静态变量
CarState g_car_state = STATE_IDLE;
static uint32_t g_task_timer = 0;
static uint32_t g_state_timer = 0;
uint8_t g_lap_count = 1;
uint8_t g_lap_target = 1;
//static bool g_is_first_corner = true;

// 启动任务函数
void ALF_Start(void) {
    if (g_car_state == STATE_IDLE || g_car_state == STATE_FINISHED) {
        //g_lap_count = 1;
        //g_is_first_corner = true;
        g_car_state = STATE_TRACKING;
        Motor_On();
    }
}

// 停止任务函数
void ALF_Stop(void) {
    Motor_Stop();
    g_car_state = STATE_IDLE;
}

// 查询任务是否完成
bool ALF_IsFinished(void) {
    return (g_car_state == STATE_FINISHED);
}

// 循迹任务主循环
void ALF_Task(void) {
    if (tick_ms - g_task_timer < TASK_EXECUTION_INTERVAL) return;
    g_task_timer = tick_ms;

    if (g_car_state == STATE_IDLE || g_car_state == STATE_FINISHED) return;

    // 【关键修改】: 在旋转状态下也需要更新传感器
    // 因此将更新函数提前
    if (g_car_state == STATE_TRACKING || g_car_state == STATE_CORNER_TURNING) {
        Gray_Sensor_Update_Value();
    }

    // 状态机核心逻辑
    switch (g_car_state) {
        case STATE_TRACKING:
            switch (g_digital_value) {
        case 0b11111111:
        // case 0b00011111:
        // case 0b00001111:
        // case 0b00000111:
        // case 0b00000011:
        // case 0b00000001:
             g_car_state = STATE_CORNER_STOP;
             break;
         case 0b11100111:
            motor_target_set(25, 25);
            break;
        case 0b11101111:
        case 0b11001111:
            motor_target_set(25, 23);
            break;
        case 0b11110111:
        case 0b11110011:
            motor_target_set(23, 25);
            break;
        case 0b10011111:
        case 0b11011111:
        case 0b10111111:
            motor_target_set(25, 19);
            break;
        case 0b11111001:
        case 0b11111101:
        case 0b11111011:
            motor_target_set(19, 25);
            break;
        case 0b11111100:
        case 0b11111110:
            motor_target_set(13, 25);
            break;
        case 0b01111111:
        case 0b00111111:
            motor_target_set(25, 13);
            break;
        default:
            break;
            }
            break;

        case STATE_CORNER_STOP:
            Motor_Stop();
           // if (g_is_first_corner) g_is_first_corner = false;
            g_lap_count++;

            if (g_lap_count > g_lap_target) {
                g_car_state = STATE_FINISHED;
                Motor_Stop();
            } else {
                g_state_timer = tick_ms;
                g_car_state = STATE_CORNER_TURNING;
            }
            break;

        case STATE_CORNER_FORWARD:
            Motor_On(); 
            motor_target_set(10, 10);
            if (tick_ms - g_state_timer > CORNER_FORWARD_DURATION) {
                g_car_state = STATE_CORNER_TURNING;
            }
            break;

        case STATE_CORNER_TURNING:
            Motor_On();
        {
            Motor_On();
            // 1. 设置差速，让小车开始逆时针原地旋转
            motor_target_set(-PIVOT_TURN_SPEED, PIVOT_TURN_SPEED); 

            // 2. 判断是否找到了新的黑线
            // 只要不是全白(0xFF)，就认为传感器阵列已经接触到新的黑线
<<<<<<< HEAD
            if ((g_digital_value != 0b11111111)&&(g_digital_value != 0b01111111)&&(g_digital_value != 0b00111111))
=======
            if ((g_digital_value != 0b11111111)&&(g_digital_value != 0b01111111))
>>>>>>> 651e7c13d059d2f906a2e29b69fb292cc29ef956
            {
                // 找到了！立即切换回正常循迹状态
                // 下一个循环，STATE_TRACKING的逻辑会根据当前g_digital_value的值来接管控制
                g_car_state = STATE_TRACKING;
            }
            break; 
        }
        
        default:
            ALF_Stop();
            break;
    }
}
