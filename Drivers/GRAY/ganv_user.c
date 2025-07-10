#include "ganv_user.h"

No_MCU_Sensor g_ganv_sensor;
unsigned short g_calibrated_white[8] = { 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800 };
unsigned short g_calibrated_black[8] = { 300, 300, 300, 300, 300, 300, 300, 300 };
volatile unsigned short g_analog_value[8] = { 0 };
volatile uint8_t g_digital_value = 0;
