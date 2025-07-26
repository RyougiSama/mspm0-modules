#include "taojingchi.h"
#include <stdio.h>
#include <string.h>

// --- 内部状态变量和缓冲区 (对外部文件隐藏) ---
static uint8_t gTxPacket[128]; 
static volatile bool gCheckUART = true; // 初始化为true，允许第一次发送
static volatile bool gDMADone = true;   // 初始化为true，允许第一次发送

/**
 * @brief UART中断服务程序 (逻辑与您验证过的一致)
 * @note  这个函数名 (TJC_UART_INST_IRQHandler) 必须与SysConfig生成的名称一致
 */
void TJC_UART_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(TJC_UART_INST)) {
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
            gDMADone = true;
            break;
        case DL_UART_MAIN_IIDX_EOT_DONE:
            gCheckUART = true;
            break;
        default:
            break;
    }
}

/**
 * @brief 发送自定义指令的实现
 */
void TJC_SendCommand(const char* command)
{
    // 1. 等待上一次传输彻底完成
    while(gDMADone == false || gCheckUART == false);

    // 2. 构建数据包 (指令 + 3个结束符)
    int len = snprintf((char*)gTxPacket, sizeof(gTxPacket), "%s", command);
    gTxPacket[len++] = 0xFF;
    gTxPacket[len++] = 0xFF;
    gTxPacket[len++] = 0xFF;

    // 3. 清除完成标志位，为新传输做准备
    gDMADone   = false;
    gCheckUART = false;

    // 4. 设置并启动DMA通道 (使用您验证过的函数)
    DL_DMA_setSrcAddr(DMA, TJC_TX_DMA_CHAN_ID, (uint32_t)&gTxPacket[0]);
    DL_DMA_setDestAddr(DMA, TJC_TX_DMA_CHAN_ID, (uint32_t)(&TJC_UART_INST->TXDATA));
    DL_DMA_setTransferSize(DMA, TJC_TX_DMA_CHAN_ID, len);
    DL_DMA_enableChannel(DMA, TJC_TX_DMA_CHAN_ID);
}

/**
 * @brief 发送波形点的实现
 */
void TJC_SendWaveformPoint(uint8_t channel, int16_t value)
{
    // 1. 等待上一次传输彻底完成
    while(gDMADone == false || gCheckUART == false);

    // 2. 构建数据包 ("add s0,..." + 3个结束符)
    int len = snprintf((char*)gTxPacket, sizeof(gTxPacket), "add s0.id,%d,%d", channel, value);
    gTxPacket[len++] = 0xFF;
    gTxPacket[len++] = 0xFF;
    gTxPacket[len++] = 0xFF;

    // 3. 清除完成标志位
    gDMADone   = false;
    gCheckUART = false;

    // 4. 设置并启动DMA
    DL_DMA_setSrcAddr(DMA, TJC_TX_DMA_CHAN_ID, (uint32_t)&gTxPacket[0]);
    DL_DMA_setDestAddr(DMA, TJC_TX_DMA_CHAN_ID, (uint32_t)(&TJC_UART_INST->TXDATA));
    DL_DMA_setTransferSize(DMA, TJC_TX_DMA_CHAN_ID, len);
    DL_DMA_enableChannel(DMA, TJC_TX_DMA_CHAN_ID);
}
