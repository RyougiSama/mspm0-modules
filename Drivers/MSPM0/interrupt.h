#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

extern volatile bool g_isGyroDataReady; 
extern uint8_t g_rawGyroData[9]; // 用于存放从中断中接收的原始数据
extern volatile uint8_t receivedData[9];

#endif  /* #ifndef _INTERRUPT_H_ */