#ifndef __GANV_USER_H
#define __GANV_USER_H

#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"

extern No_MCU_Sensor g_ganv_sensor;
extern unsigned short g_calibrated_white[8];        // 白校准基准值
extern unsigned short g_calibrated_black[8];        // 黑校准基准值
extern volatile unsigned short g_analog_value[8];   // 当前模拟值
extern volatile uint8_t g_digital_value;            // 当前数字值

#endif
