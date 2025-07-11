#include "uart_pid.h"
#include "pid.h"
#include "stdio.h"
#include "string.h"

int fputc(int c, FILE *stream)
{
    DL_UART_Main_transmitDataBlocking(UART_PID_INST, c);
    return c;
}

int fputs(const char *restrict s, FILE *restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for (i = 0; i < len; i++)
    {
        DL_UART_Main_transmitDataBlocking(UART_PID_INST, s[i]);
    }
    return len;
}

int puts(const char *_ptr)
{
    int count = fputs(_ptr, stdout);
    count += fputs("\n", stdout);
    return count;
}

void Serial_PID(void)
{
    printf("%.2f,%.2f,%.2f,%.2f\n", g_motorA.target, g_motorA.now,g_motorB.target,g_motorB.now);
}