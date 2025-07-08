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

uint32_t motor_ms, oled_ms;
uint16_t test_duty = 10000;

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
    // WIT_Init();

    OLED_ShowString(0, 0, (uint8_t *)"Initializing...", 16);
    delay_ms(2000);
    OLED_Clear();

    Motor_On();

    while (1)
    {
        if (tick_ms - motor_ms >= 5000)
        {
            motor_ms = tick_ms;
            if (test_duty >= 35000)
            {
                test_duty = 10000;
            }
            else
            {
                test_duty += 5000;
            }
            motorA_duty(test_duty);
            motorB_duty(test_duty);
        }

        if (tick_ms - oled_ms >= 500)
        {
            oled_ms = tick_ms;
            OLED_Clear();
            OLED_ShowString(8, 2, (uint8_t *)"Encoder1:", 16);
            OLED_ShowString(8, 4, (uint8_t *)"Encoder2:", 16);
            OLED_ShowNum(80, 2, (uint32_t)motorA.now, 3, 16);
            OLED_ShowNum(80, 4, (uint32_t)motorB.now, 3, 16);
        }
        if (tick_ms - motor_ms >= 5000)
        {
            motor_ms = tick_ms;
            if (test_duty >= 35000)
            {
                test_duty = 10000;
            }
            else
            {
                test_duty += 5000;
            }
            motorA_duty(test_duty);
            motorB_duty(test_duty);
        }

        if (tick_ms - oled_ms >= 500)
        {
            oled_ms = tick_ms;
            OLED_Clear();
            OLED_ShowString(8, 2, (uint8_t *)"Encoder1:", 16);
            OLED_ShowString(8, 4, (uint8_t *)"Encoder2:", 16);
            OLED_ShowNum(80, 2, (uint32_t)motorA.now, 3, 16);
            OLED_ShowNum(80, 4, (uint32_t)motorB.now, 3, 16);
        }
    }
}
