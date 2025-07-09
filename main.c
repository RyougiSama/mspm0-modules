/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "main.h"
#include "stdio.h"
#include "ganv_user.h"

uint32_t motor_ms, oled_ms, key_ms;
uint16_t test_duty = 10000;
float val = 0;
char oled_buffer[20];

void Oled_Motor_Test()
{
    OLED_Clear();
    OLED_ShowString(4, 0, (uint8_t *)"En1:", 16);
    OLED_ShowString(4, 2, (uint8_t *)"En2:", 16);

    OLED_ShowNum(40, 0, (uint32_t)g_motorA.now, 3, 16);
    OLED_ShowNum(40, 2, (uint32_t)g_motorB.now, 3, 16);
    // --- 显示I值 ---
    sprintf((char *)oled_buffer, "AP:%-4.2f", g_motorA.p);
    OLED_ShowString(0, 4, (uint8_t *)oled_buffer, 16);

    // --- 显示I值 ---
    sprintf((char *)oled_buffer, "AI:%-4.2f", g_motorA.i);
    OLED_ShowString(65, 4, (uint8_t *)oled_buffer, 16);

    // --- 显示D值 ---
    sprintf((char *)oled_buffer, "AD:%-4.2f", g_motorA.d);
    OLED_ShowString(65, 6, (uint8_t *)oled_buffer, 16);
}

void Oled_Ganv_Test()
{
    static unsigned short analog_value[8];
    No_Mcu_Ganv_Sensor_Task_Without_tick(&g_ganv_sensor); // 执行传感器任务
    OLED_Clear();
    if (Get_Analog_Value(&g_ganv_sensor, analog_value)) {
        for (int i = 0; i < 8; i++) {
            OLED_ShowNum(0, i, analog_value[i], 4, 8);
        }
    }
    // OLED_ShowString(0, 4, (uint8_t *)"Digital:", 16);
}

int main(void)
{
    // sys initialization
    SYSCFG_DL_init();
    SysTick_Init();
    NVIC_EnableIRQ(TIMER_speedget_INST_INT_IRQN);
    DL_TimerA_startCounter(TIMER_speedget_INST);

    // peripheral initialization
    OLED_Init();
    Motor_Init();
    Encoder_Init();
    // WIT_Init();
    No_MCU_Ganv_Sensor_Init_Frist(&g_ganv_sensor);

    // Motor_On();
    // pid_init(&g_motorA, DELTA_PID, 10, 5, 3);
    // pid_init(&g_motorB, DELTA_PID, 10, 5, 3);
    // motor_target_set(30, -30);

    while (1)
    {

        Key_PID_MDF();

        if (tick_ms - oled_ms >= 500)
        {
            oled_ms = tick_ms;
            // Oled_Motor_Test();
            Oled_Ganv_Test();
        }
    }
}
