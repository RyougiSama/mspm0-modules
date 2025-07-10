#ifndef __motor_H_
#define __motor_H_
#include "main.h"

void Motor_Init(void);
void Motor_On(void);
void MotorA_Duty(float duty);
void MotorB_Duty(float duty);
void Motor_Stop(void);
void Encoder_Init(void);

extern uint8_t g_motorA_dir;
extern uint8_t g_motorB_dir;
extern int g_encoder_A;
extern int g_encoder_B;

#endif
