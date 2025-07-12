#ifndef _MAIN_H_
#define _MAIN_H_

#include "clock.h"

#include "oled_hardware_i2c.h"
#include "wit.h"
#include "motor.h"
#include "pid.h"
#include "key.h"
#include "interrupt.h"

void OLED_Task(void);
void Wit_Proc(void);
#endif  /* #ifndef _MAIN_H_ */
