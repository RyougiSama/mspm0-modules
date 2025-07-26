#ifndef __TAOJINGCHI_H
#define __TAOJINGCHI_H

#include "ti_msp_dl_config.h"

//配置串口步骤：
//新增一个串口，命名为TJC_UART
//波特率115200，其余默认
//配置串口引脚（布套使用PA25/PA26）
//勾选使能FIFO
//RX>=1 entry,TX=1 entry
//勾选中断使能：DMA done on transmit / End of transmission / Receive
//勾选DMA TX Interrupt，通道命名为TJC_TX_DMA
//选择block to fixed
//字长为BYTE

/**
 * @brief 向串口屏的波形控件(s0)发送一个数据点
 * @param channel: 波形通道号 (0, 1, 2...)
 * @param value: 要发送的Y轴数值
 */
void TJC_SendWaveformPoint(uint8_t channel, int16_t value);

/**
 * @brief 向串口屏发送任意自定义指令
 * @param command: 指令字符串，例如 "t0.txt=\"Hello\""
 */
void TJC_SendCommand(const char* command);

// --- 发送相关 ---
extern volatile bool gCheckUART;
extern volatile bool gDMADone;


#endif /* __TAOJINGCHI_H */