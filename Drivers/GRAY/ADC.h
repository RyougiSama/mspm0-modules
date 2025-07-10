#ifndef __ADC_H
#define __ADC_H

#include "ti_msp_dl_config.h"

void Adc_Init(void);
unsigned int adc_getValue(void);//读取ADC的数据

#endif
