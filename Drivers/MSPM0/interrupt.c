#include "ti_msp_dl_config.h"
#include "interrupt.h"
#include "clock.h"
#include "wit.h"
#include "motor.h"

void SysTick_Handler(void)
{
    tick_ms++;
}

#if defined UART_WIT_INST_IRQHandler
void UART_WIT_INST_IRQHandler(void)
{
    uint8_t checkSum, packCnt = 0;
    extern uint8_t wit_dmaBuffer[33];

    DL_DMA_disableChannel(DMA, DMA_WIT_CHAN_ID);
    uint8_t rxSize = 32 - DL_DMA_getTransferSize(DMA, DMA_WIT_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_WIT_INST) == false)
        wit_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_WIT_INST);

    while(rxSize >= 11)
    {
        checkSum=0;
        for(int i=packCnt*11; i<(packCnt+1)*11-1; i++)
            checkSum += wit_dmaBuffer[i];

        if((wit_dmaBuffer[packCnt*11] == 0x55) && (checkSum == wit_dmaBuffer[packCnt*11+10]))
        {
            if(wit_dmaBuffer[packCnt*11+1] == 0x51)
            {
                wit_data.ax = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 2.048; //mg
                wit_data.ay = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 2.048; //mg
                wit_data.az = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 2.048; //mg
                wit_data.temperature =  (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]) / 100.0; //°C
            }
            else if(wit_dmaBuffer[packCnt*11+1] == 0x52)
            {
                wit_data.gx = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 16.384; //°/S
                wit_data.gy = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 16.384; //°/S
                wit_data.gz = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 16.384; //°/S
            }
            else if(wit_dmaBuffer[packCnt*11+1] == 0x53)
            {
                wit_data.roll  = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 32768.0 * 180.0; //°
                wit_data.pitch = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 32768.0 * 180.0; //°
                wit_data.yaw   = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 32768.0 * 180.0; //°
                wit_data.version = (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]);
            }
        }

        rxSize -= 11;
        packCnt++;
    }
    
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_WIT_INST, dummy, 4);

    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &wit_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);
}
#endif

// Get encoder count
void GROUP1_IRQHandler(void)
{
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Right_E2A_PORT,GPIO_Encoder_PIN_Right_E2A_PIN); //预留四驱接口
    uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Left_E1A_PORT,GPIO_Encoder_PIN_Left_E1A_PIN);

    if((gpioB & GPIO_Encoder_PIN_Right_E2A_PIN))
    {   
        if(DL_GPIO_readPins(GPIO_Encoder_PIN_Right_E2B_PORT,GPIO_Encoder_PIN_Right_E2B_PIN))
            g_encoder_B++;
        else 
            g_encoder_B--;
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Right_E2A_PORT, GPIO_Encoder_PIN_Right_E2A_PIN);
    }
    if((gpioA & GPIO_Encoder_PIN_Left_E1A_PIN))
    {
        // DL_GPIO_togglePins(GPIOA, GPIO_LED_PIN_LED_PIN);
        if(DL_GPIO_readPins(GPIO_Encoder_PIN_Left_E1B_PORT,GPIO_Encoder_PIN_Left_E1B_PIN))
            g_encoder_A--;
        else 
            g_encoder_A++;
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Left_E1A_PORT, GPIO_Encoder_PIN_Left_E1A_PIN);
    }
}

void TIMER_speedget_INST_IRQHandler(void)       //编码器速度获取
{
    switch(DL_TimerA_getPendingInterrupt(TIMER_speedget_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            pid_control();
            break;
        default:
            break;
    }
}
