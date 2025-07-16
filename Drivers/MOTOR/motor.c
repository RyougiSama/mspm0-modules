#include "motor.h"

#define PWM_MAX_DUTY (100)
#define PWM_FREQ (1000)
#define PWM_PERIOD_CNT (PWM_0_INST_CLK_FREQ / PWM_FREQ)

uint8_t g_motorA_dir = 1;
uint8_t g_motorB_dir = 1;
int g_encoder_A = 0;
int g_encoder_B = 0;

void Motor_Init(void)
{
    DL_TimerG_startCounter(PWM_0_INST);
}

void Motor_On(void)
{
    DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_STBY_PIN);
}

void Motor_Stop(void)
{
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_STBY_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_AIN1_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_AIN2_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_BIN1_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_BIN2_PIN);
    pid_reset(&g_motorA);
    pid_reset(&g_motorB);
    
}

static void Set_Duty(float duty, uint8_t channel)
{
    uint32_t CompareValue = (PWM_PERIOD_CNT - 1) - (PWM_PERIOD_CNT - 1) * duty / PWM_MAX_DUTY;
    if(channel==0)
    {
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,CompareValue,DL_TIMER_CC_0_INDEX);
    }
    else if(channel==1)
    {
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,CompareValue,DL_TIMER_CC_1_INDEX);
    }
}

void MotorA_Duty(float duty)
{
    Set_Duty(duty,0);
    if(g_motorA_dir==1)
    {
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_AIN1_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_AIN2_PIN);
    }
    else if(g_motorA_dir==0)
    {
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_AIN2_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_AIN1_PIN);
    }
}

void MotorB_Duty(float duty)
{
    Set_Duty(duty, 1);
    if(g_motorB_dir==1)
    {
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_BIN1_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_BIN2_PIN);
    }
    else if(g_motorB_dir==0)
    {
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_BIN2_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_BIN1_PIN);
    }
}

void Encoder_Init(void)
{
    NVIC_EnableIRQ(GPIO_Encoder_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_Encoder_GPIOA_INT_IRQN);
}