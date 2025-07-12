#include "wit.h"

uint8_t wit_dmaBuffer[33];

WIT_Data_t wit_data;

// void WIT_Init(void)
// {
//     DL_DMA_setSrcAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t)(&UART_WIT_INST->RXDATA));
//     DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &wit_dmaBuffer[0]);
//     DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);
//     DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);

//     NVIC_EnableIRQ(UART_WIT_INST_INT_IRQN);
// }

void Serial_JY61P_Zero_Yaw(void){
    DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0XFF);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0XAA);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X69);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X88);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0XB5);
	delay_ms(100);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0XFF);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0XAA);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X01);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X04);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X00);
	delay_ms(100);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0XFF);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0XAA);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X00);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X00);
	DL_UART_Main_transmitDataBlocking(UART_WIT_INST,0X00);
	
}