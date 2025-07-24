#include "led_buzzer.h"


void Buzzer_On(void)
{
    DL_GPIO_clearPins(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN_BUZZER_PIN);
}

// 关闭蜂鸣器
void Buzzer_Off(void)
{
    DL_GPIO_setPins(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN_BUZZER_PIN);
}

// 蜂鸣器响一声
void Buzzer_Beep(uint32_t duration_ms)
{
    Buzzer_On();
    mspm0_delay_ms(300);
    Buzzer_Off();
}
