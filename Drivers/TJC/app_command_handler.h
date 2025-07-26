#ifndef __APP_COMMAND_HANDLER_H
#define __APP_COMMAND_HANDLER_H

#include <stdint.h>
#include <stdbool.h> // 【新增】包含bool类型定义

// --- 【新增】定义页面ID和系统指令 ---
#define PAGE_ID_SYSTEM_CMD      0x00
#define CMD_START_SINE_WAVE     0x00
#define CMD_STOP_SINE_WAVE      0x01

#define PAGE_ID_MAIN_CONTROL    0x01
#define PAGE_ID_PID_TUNE_A      0x02
#define PAGE_ID_PID_TUNE_B      0x03
#define PAGE_ID_PID_TUNE_YAW    0x04

// --- 【新增】声明全局波形发送标志位 ---
extern volatile bool g_isSineWaveActive;

/**
 * @brief 处理从串口解析出的有效指令
 * @param page_id   指令所属的界面ID
 * @param key_value 指令的按键值
 */
void App_ProcessSerialCommand(uint8_t page_id, uint8_t key_value);

#endif /* __APP_COMMAND_HANDLER_H */