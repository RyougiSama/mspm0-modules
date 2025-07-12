#include "ti_msp_dl_config.h"
#include "interrupt.h"
#include "clock.h"
#include "wit.h"
#include "motor.h"

#define WAIT_HEADER1 0
#define WAIT_HEADER2 1
#define RECEIVE_DATA 2
volatile uint8_t RxState = WAIT_HEADER1;
volatile uint8_t receivedData[9];
uint8_t dataIndex = 0;
// 使用volatile关键字，防止编译器过度优化
volatile bool g_isGyroDataReady = false; 
volatile uint8_t g_rawGyroData[9]; // 用于存放从中断中接收的原始数据

void SysTick_Handler(void)
{
    tick_ms++;
}

#if defined UART_WIT_INST_IRQHandler
void UART_WIT_INST_IRQHandler(void)
{
    // uint8_t checkSum, packCnt = 0;
    // extern uint8_t wit_dmaBuffer[33];

    // DL_DMA_disableChannel(DMA, DMA_WIT_CHAN_ID);
    // uint8_t rxSize = 32 - DL_DMA_getTransferSize(DMA, DMA_WIT_CHAN_ID);

    // if(DL_UART_isRXFIFOEmpty(UART_WIT_INST) == false)
    //     wit_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_WIT_INST);

    // while(rxSize >= 11)
    // {
    //     checkSum=0;
    //     for(int i=packCnt*11; i<(packCnt+1)*11-1; i++)
    //         checkSum += wit_dmaBuffer[i];

    //     if((wit_dmaBuffer[packCnt*11] == 0x55) && (checkSum == wit_dmaBuffer[packCnt*11+10]))
    //     {
    //         if(wit_dmaBuffer[packCnt*11+1] == 0x51)
    //         {
    //             wit_data.ax = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 2.048; //mg
    //             wit_data.ay = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 2.048; //mg
    //             wit_data.az = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 2.048; //mg
    //             wit_data.temperature =  (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]) / 100.0; //°C
    //         }
    //         else if(wit_dmaBuffer[packCnt*11+1] == 0x52)
    //         {
    //             wit_data.gx = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 16.384; //°/S
    //             wit_data.gy = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 16.384; //°/S
    //             wit_data.gz = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 16.384; //°/S
    //         }
    //         else if(wit_dmaBuffer[packCnt*11+1] == 0x53)
    //         {
    //             wit_data.roll  = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 32768.0 * 180.0; //°
    //             wit_data.pitch = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 32768.0 * 180.0; //°
    //             wit_data.yaw   = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 32768.0 * 180.0; //°
    //             wit_data.version = (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]);
    //         }
    //     }

    //     rxSize -= 11;
    //     packCnt++;
    // }
    
    // uint8_t dummy[4];
    // DL_UART_drainRXFIFO(UART_WIT_INST, dummy, 4);

    // DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &wit_dmaBuffer[0]);
    // DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);
    // DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);

    uint8_t uartdata = DL_UART_Main_receiveData(UART_WIT_INST); // 接收一个uint8_t数据

    switch (RxState) {
    case WAIT_HEADER1:
        if (uartdata == 0x55) {
            RxState = WAIT_HEADER2;
        }
        break;
    case WAIT_HEADER2:
        if (uartdata == 0x53) {
            RxState = RECEIVE_DATA;
            dataIndex = 0;
        } else {
            RxState = WAIT_HEADER1; // 如果不是期望的第二个头，重置状态
        }
        break;
    case RECEIVE_DATA:
        receivedData[dataIndex++] = uartdata;
        if (dataIndex == 9) {
            // 数据接收完毕，分配给具体的变量
            // RollL = receivedData[0];
            // RollH = receivedData[1];
            // PitchL = receivedData[2];
            // PitchH = receivedData[3];
            // YawL = receivedData[4];
            // YawH = receivedData[5];
            // VL = receivedData[6];
            // VH = receivedData[7];
            // SUM = receivedData[8];

            // 校验SUM是否正确
            uint8_t calculatedSum = 0x55 + 0x53;
             for(uint8_t i=0; i<8; i++) {
                calculatedSum += receivedData[i];
            }
            if (calculatedSum == receivedData[8]) {
                // 校验成功！将原始数据复制到全局缓冲区
                for(int i=0; i<9; i++) {
                    g_rawGyroData[i] = receivedData[i];
                }
                // 设置数据就绪标志
                g_isGyroDataReady = true;

                // // 校验成功，可以进行后续处理
                // if((float)(((uint16_t)RollH << 8) | RollL)/32768*180>180){
                //     Roll = (float)(((uint16_t)RollH << 8) | RollL)/32768*180 - 360;
                // }else{
                //     Roll = (float)(((uint16_t)RollH << 8) | RollL)/32768*180;
                // }

                // if((float)(((uint16_t)PitchH << 8) | PitchL)/32768*180>180){
                //     Pitch = (float)(((uint16_t)PitchH << 8) | PitchL)/32768*180 - 360;
                // }else{
                //     Pitch = (float)(((uint16_t)PitchH << 8) | PitchL)/32768*180;
                // }

                // if((float)(((uint16_t)YawH << 8) | YawL)/32768*180 >180){
                //     Yaw = (float)(((uint16_t)YawH << 8) | YawL)/32768*180 - 360;
                // }else{
                //     Yaw = (float)(((uint16_t)YawH << 8) | YawL)/32768*180;
                // }
                
                
            } 
            RxState = WAIT_HEADER1; // 重置状态以等待下一个数据包
        }
        break;
    }

}
#endif

// Get encoder count
void GROUP1_IRQHandler(void)
{
    // uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Right_E2A_PORT,GPIO_Encoder_PIN_Right_E2A_PIN); //预留四驱接口
    // uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Left_E1A_PORT,GPIO_Encoder_PIN_Left_E1A_PIN);

    // if((gpioB & GPIO_Encoder_PIN_Right_E2A_PIN))
    // {   
    //     if(DL_GPIO_readPins(GPIO_Encoder_PIN_Right_E2B_PORT,GPIO_Encoder_PIN_Right_E2B_PIN))
    //         g_encoder_B++;
    //     else 
    //         g_encoder_B--;
    //     DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Right_E2A_PORT, GPIO_Encoder_PIN_Right_E2A_PIN);
    // }
    // if((gpioA & GPIO_Encoder_PIN_Left_E1A_PIN))
    // {
    //     // DL_GPIO_togglePins(GPIOA, GPIO_LED_PIN_LED_PIN);
    //     if(DL_GPIO_readPins(GPIO_Encoder_PIN_Left_E1B_PORT,GPIO_Encoder_PIN_Left_E1B_PIN))
    //         g_encoder_A--;
    //     else 
    //         g_encoder_A++;
    //     DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Left_E1A_PORT, GPIO_Encoder_PIN_Left_E1A_PIN);
    // }


    // 获取A、B两相的中断状态
    uint32_t gpioA_left  = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Left_E1A_PORT, GPIO_Encoder_PIN_Left_E1A_PIN);
    uint32_t gpioB_left  = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Left_E1B_PORT, GPIO_Encoder_PIN_Left_E1B_PIN);
    uint32_t gpioA_right = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Right_E2A_PORT, GPIO_Encoder_PIN_Right_E2A_PIN);
    uint32_t gpioB_right = DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PIN_Right_E2B_PORT, GPIO_Encoder_PIN_Right_E2B_PIN);

    // --- 处理左侧编码器 ---
    // 注意：如果电机实际转动方向与计数方向相反，只需将下面的 g_encoder_A++ 和 g_encoder_A-- 对调即可。
    
    // A相引脚触发了中断
    if (gpioA_left & GPIO_Encoder_PIN_Left_E1A_PIN)
    {
        // 通过读取B相电平来判断方向
        if (DL_GPIO_readPins(GPIO_Encoder_PIN_Left_E1B_PORT, GPIO_Encoder_PIN_Left_E1B_PIN) != 
            DL_GPIO_readPins(GPIO_Encoder_PIN_Left_E1A_PORT, GPIO_Encoder_PIN_Left_E1A_PIN)) {
            g_encoder_A--; // 正转
        } else {
            g_encoder_A++; // 反转
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Left_E1A_PORT, GPIO_Encoder_PIN_Left_E1A_PIN);
    }
    
    // B相引脚触发了中断
    if (gpioB_left & GPIO_Encoder_PIN_Left_E1B_PIN)
    {
        // 通过读取A相电平来判断方向
        if (DL_GPIO_readPins(GPIO_Encoder_PIN_Left_E1A_PORT, GPIO_Encoder_PIN_Left_E1A_PIN) == 
            DL_GPIO_readPins(GPIO_Encoder_PIN_Left_E1B_PORT, GPIO_Encoder_PIN_Left_E1B_PIN)) {
            g_encoder_A--; // 正转
        } else {
            g_encoder_A++; // 反转
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Left_E1B_PORT, GPIO_Encoder_PIN_Left_E1B_PIN);
    }


    // --- 处理右侧编码器 ---
    // 注意：如果电机实际转动方向与计数方向相反，只需将下面的 g_encoder_B++ 和 g_encoder_B-- 对调即可。

    // A相引脚触发了中断
    if (gpioA_right & GPIO_Encoder_PIN_Right_E2A_PIN)
    {
        if (DL_GPIO_readPins(GPIO_Encoder_PIN_Right_E2B_PORT, GPIO_Encoder_PIN_Right_E2B_PIN) == 
            DL_GPIO_readPins(GPIO_Encoder_PIN_Right_E2A_PORT, GPIO_Encoder_PIN_Right_E2A_PIN)) {
            g_encoder_B--; // 正转
        } else {
            g_encoder_B++; // 反转
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Right_E2A_PORT, GPIO_Encoder_PIN_Right_E2A_PIN);
    }
    
    // B相引脚触发了中断
    if (gpioB_right & GPIO_Encoder_PIN_Right_E2B_PIN)
    {
        if (DL_GPIO_readPins(GPIO_Encoder_PIN_Right_E2A_PORT, GPIO_Encoder_PIN_Right_E2A_PIN) != 
            DL_GPIO_readPins(GPIO_Encoder_PIN_Right_E2B_PORT, GPIO_Encoder_PIN_Right_E2B_PIN)) {
            g_encoder_B--; // 正转
        } else {
            g_encoder_B++; // 反转
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PIN_Right_E2B_PORT, GPIO_Encoder_PIN_Right_E2B_PIN);
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


void HardFault_Handler(void)
{
    // 在这里让一个LED灯快速闪烁
    while(1)
    {
        DL_GPIO_togglePins(GPIOA, GPIO_LED_PIN_LED_PIN);
    }
}