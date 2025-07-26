#include "serial_protocol.h"
#include "app_command_handler.h" // 包含上层应用处理函数的头文件
#include "taojingchi.h"

// --- 硬件和缓冲区定义 ---
#define RX_BUFFER_SIZE 128
static uint8_t  g_rxBuffer[RX_BUFFER_SIZE];     // 环形缓冲区
static volatile uint16_t g_rxWriteIndex = 0;    // 写指针 (由中断修改)
static volatile uint16_t g_rxReadIndex = 0;     // 读指针 (由主循环修改)

// --- 协议状态机定义 ---
typedef enum {
    STATE_WAIT_FOR_HEADER,
    STATE_WAIT_FOR_PAGE_ID,
    STATE_WAIT_FOR_KEY_VALUE,
    STATE_WAIT_FOR_END
} ParserState_t;

static ParserState_t g_parserState = STATE_WAIT_FOR_HEADER;
static uint8_t g_pageId = 0;
static uint8_t g_keyValue = 0;

/**
 * @brief UART接收中断服务程序
 * @note  这个函数名 (TJC_UART_INST_IRQHandler) 必须与SysConfig中配置的UART实例名匹配
 */
void TJC_UART_INST_IRQHandler(void)
{
    // 我们只关心接收中断事件
    switch (DL_UART_getPendingInterrupt(TJC_UART_INST)) {
        case DL_UART_IIDX_RX: // 接收中断
            
            // ==================================================
            //  【调试代码】收到任何字节，都翻转一次LED
            //  请确保 GPIO_LEDS_PORT 和 GPIO_LEDS_USER_LED_1_PIN
            //  与您SysConfig中板载LED的定义一致
            // ==================================================
            // 1. 读取一个字节存入环形缓冲区
            g_rxBuffer[g_rxWriteIndex] = DL_UART_receiveData(TJC_UART_INST);
            // 2. 移动写指针
            g_rxWriteIndex = (g_rxWriteIndex + 1) % RX_BUFFER_SIZE;
            break;
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
            gDMADone = true;
            break;
        case DL_UART_MAIN_IIDX_EOT_DONE:
            gCheckUART = true;
            break;
        default:
            // 其他中断事件（如发送完成）由发送模块的逻辑处理，这里忽略
            break;
    }
}

/**
 * @brief 初始化串口协议处理模块
 */
void SerialProtocol_Init(void)
{
    // SysConfig已经完成了硬件配置，我们只需要使能接收中断
    NVIC_EnableIRQ(TJC_UART_INST_INT_IRQN);
}

/**
 * @brief 周期性地解析串口数据
 */
void SerialProtocol_Parse(void)
{
    // 只要读写指针不相等，就说明缓冲区里有新数据
    while (g_rxReadIndex != g_rxWriteIndex)
    {
        // 1. 从环形缓冲区取出一个字节
        uint8_t current_byte = g_rxBuffer[g_rxReadIndex];
        g_rxReadIndex = (g_rxReadIndex + 1) % RX_BUFFER_SIZE;

        // 2. 将字节送入状态机进行处理
        switch (g_parserState)
        {
            case STATE_WAIT_FOR_HEADER:
                if (current_byte == 0x33) {
                    g_parserState = STATE_WAIT_FOR_PAGE_ID;
                }
                break;

            case STATE_WAIT_FOR_PAGE_ID:
                g_pageId = current_byte;
                g_parserState = STATE_WAIT_FOR_KEY_VALUE;
                break;

            case STATE_WAIT_FOR_KEY_VALUE:
                g_keyValue = current_byte;
                g_parserState = STATE_WAIT_FOR_END;
                break;

            case STATE_WAIT_FOR_END:
                if (current_byte == 0xFF) {
                    // 一帧完整且有效的指令接收完毕！
                    // 调用上层应用函数来处理这个指令
                    App_ProcessSerialCommand(g_pageId, g_keyValue);
                }
                // 无论最后一个字节是否是0xFF，一帧处理都结束了，回到初始状态
                g_parserState = STATE_WAIT_FOR_HEADER;
                break;
        }
    }
}