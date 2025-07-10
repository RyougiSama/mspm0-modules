#include "ADC.h"

#if 0
unsigned int adc_getValue(void)
{
    unsigned int gAdcResult = 0;

    //使能ADC转换
    DL_ADC12_enableConversions(ADC1_INST);
    //软件触发ADC开始转换
    DL_ADC12_startConversion(ADC1_INST);

    //如果当前状态 不是 空闲状态
    while (DL_ADC12_getStatus(ADC1_INST) != DL_ADC12_STATUS_CONVERSION_IDLE );

    //清除触发转换状态
    DL_ADC12_stopConversion(ADC1_INST);
    //失能ADC转换
    DL_ADC12_disableConversions(ADC1_INST);

    //获取数据
    gAdcResult = DL_ADC12_getMemResult(ADC1_INST, ADC1_ADCMEM_ADC_Channel0);

    return gAdcResult;
}
#endif

#if 1
static bool g_adc_check;

void Adc_Init(void)
{
    NVIC_EnableIRQ(ADC1_INST_INT_IRQN);
    g_adc_check = false;
}

unsigned int adc_getValue(void)
{
    unsigned int adc_val = 0;
    // 软件触发ADC开始转换
    DL_ADC12_startConversion(ADC1_INST);
    // 等待转换完成
    while (g_adc_check == false) {}
    // 获取ADC值
    adc_val = DL_ADC12_getMemResult(ADC1_INST, DL_ADC12_MEM_IDX_0);
    g_adc_check = false;
    // 使能ADC转换
    DL_ADC12_enableConversions(ADC1_INST);
    return adc_val;
}

void ADC1_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC1_INST)) {
    case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
        g_adc_check = true;
        break;
    default:
        break;
    }
}

#endif
