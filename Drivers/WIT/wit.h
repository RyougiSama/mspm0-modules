/*
 * SysConfig Configuration Steps:
 *   UART:
 *     1. Add an UART module.
 *     2. Name it as "UART_WIT".
 *     3. Set "Target Baud Rate" according to module configuration.
 *     4. Set "Communication Direction " to "RX only".
 *     5. Check the box "Enable FIFOs".
 *     6. Set "RX Timeout Interrupt Counts" to "1".
 *     7. Check the "RX timeout" box at "Enable Interrupts".
 *     8. Set "Configure DMA RX Trigger" to "UART RX interrupt".
 *     9. Set "DMA Channel RX Name" to "DMA_WIT".
 *     10. Set "Address Mode" to "Fixed addr. to Block addr.".
 *     11. Set "Source Length" and "Destination Length" to "Byte".
 *     12. Set the pin according to your needs.
 */

#ifndef __WIT_H
#define __WIT_H

#include "ti_msp_dl_config.h"

typedef struct {
    float pitch;
    float roll;
    float yaw;
    float temperature;
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
    int16_t version;
} WIT_Data_t;

typedef enum {
    CALIB_STATE_IDLE,               // 0: 空闲状态
    CALIB_STATE_SEND_UNLOCK,        // 1: 发送解锁指令
    CALIB_STATE_WAIT_UNLOCK,        // 2: 等待200ms
    CALIB_STATE_SEND_CALIBRATE,     // 3: 发送校准指令
    CALIB_STATE_WAIT_CALIBRATE,     // 4: 等待3000ms
    CALIB_STATE_SEND_SAVE           // 5: 发送保存指令
} GyroCalibState_t;

extern WIT_Data_t wit_data;

void WIT_Init(void);
void Gyro_Calibration_Start(void);
void Gyro_Calibration_Update(void);
bool Is_Gyro_Calib_Complete(void);

#endif /* #ifndef __WIT_H */
