#ifndef __TAOJINGCHI_H
#define __TAOJINGCHI_H

#include "ti_msp_dl_config.h"

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

#endif /* __TAOJINGCHI_H */