#include "ganv_user.h"
#include "clock.h"
#include "pid.h"

// 灰度传感器校准值宏定义
#define GRAY_WHITE_CALIBRATION_VALUE    810
#define GRAY_BLACK_CALIBRATION_VALUE    800

No_MCU_Sensor g_ganv_sensor;
unsigned short g_calibrated_white[8] = { GRAY_WHITE_CALIBRATION_VALUE, GRAY_WHITE_CALIBRATION_VALUE, GRAY_WHITE_CALIBRATION_VALUE, GRAY_WHITE_CALIBRATION_VALUE, GRAY_WHITE_CALIBRATION_VALUE, GRAY_WHITE_CALIBRATION_VALUE, GRAY_WHITE_CALIBRATION_VALUE, GRAY_WHITE_CALIBRATION_VALUE };
unsigned short g_calibrated_black[8] = { GRAY_BLACK_CALIBRATION_VALUE, GRAY_BLACK_CALIBRATION_VALUE, GRAY_BLACK_CALIBRATION_VALUE, GRAY_BLACK_CALIBRATION_VALUE, GRAY_BLACK_CALIBRATION_VALUE, GRAY_BLACK_CALIBRATION_VALUE, GRAY_BLACK_CALIBRATION_VALUE, GRAY_BLACK_CALIBRATION_VALUE };
volatile unsigned short g_analog_value[8] = { 0 };
volatile uint8_t g_digital_value = 0;

bool start_gray_sensor_task = false;

void Gray_Sensor_Update_Value(void)
{
    No_Mcu_Ganv_Sensor_Task_Without_tick(&g_ganv_sensor);
    Get_Analog_Value(&g_ganv_sensor, g_analog_value);
    g_digital_value = Get_Digtal_For_User(&g_ganv_sensor);
}

static void Gray_Digital2Array(bool *gray_arr)
{
    for (uint8_t i = 0; i < 8; ++i) {
        gray_arr[i] = (g_digital_value >> i) & 0x01;
    }
}

void Gray_Sensor_Test(void)
{
    if (!start_gray_sensor_task) return;

    static unsigned long task_tick = 0;
    if (tick_ms - task_tick < 10) return;
    task_tick = tick_ms;

    static bool is_white[8] = { false };
    Gray_Sensor_Update_Value();
    Gray_Digital2Array(is_white);

#if 0
    if (!is_white[3] && !is_white[4]) { // 11100111
        motor_target_set(20, 20);
    } else if (!is_white[3] && is_white[4]) { // 11101111
        motor_target_set(15, 20);
    } else if (is_white[3] && !is_white[4]) { // 11110111
        motor_target_set(20, 15);
    } else if (!is_white[2] || !is_white[3]) { // 11001111
        motor_target_set(10, 20);
    } else if (!is_white[4] || !is_white[5]) { // 11110011
        motor_target_set(20, 10);
    } else if (!is_white[0] || !is_white[1]) { // 00111111
        motor_target_set(5, 20);
    } else if (!is_white[6] || !is_white[7]) { // 11111100
        motor_target_set(20, 5);
    }
#endif

    switch (g_digital_value) {
        case 0b11100111:
            motor_target_set(10, 10);
            break;
        case 0b11101111:
        case 0b11001111:
            motor_target_set(10, 8);
            break;
        case 0b11110111:
        case 0b11110011:
            motor_target_set(8, 10);
            break;
        case 0b10111111:
        case 0b00111111:
            motor_target_set(10, 5);
            break;
        case 0b11111001:
        case 0b11111100:
            motor_target_set(5, 10);
            break;
        case 0b11111110:
            motor_target_set(2, 10);
            break;
        case 0b01111111:
            motor_target_set(10, 2);
            break;
        default:
            break;
    }
}
