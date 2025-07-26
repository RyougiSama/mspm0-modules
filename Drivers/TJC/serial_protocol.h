#ifndef __SERIAL_PROTOCOL_H
#define __SERIAL_PROTOCOL_H

#include "ti_msp_dl_config.h"

/**
 * @brief 初始化串口协议处理模块
 * @note  需要在主函数中调用一次
 */
void SerialProtocol_Init(void);

/**
 * @brief 在主循环中周期性地解析串口数据
 * @note  这个函数需要被快速、反复地调用
 */
void SerialProtocol_Parse(void);

#endif /* __SERIAL_PROTOCOL_H */