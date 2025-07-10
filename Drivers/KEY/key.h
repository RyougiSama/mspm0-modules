#ifndef __key_H_
#define __key_H_
#include "main.h"

uint8_t Matrix_Key_Scan(void);
void Key_PID_MDF(void);

extern uint32_t motor_start;
extern uint8_t motor_status;
#endif