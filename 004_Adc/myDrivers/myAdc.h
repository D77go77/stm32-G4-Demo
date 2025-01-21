/**
  ******************************************************************************
  * @file           : myAdc.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/21
  ******************************************************************************
  */

#ifndef INC_004_ADC_MYADC_H
#define INC_004_ADC_MYADC_H

#include "bsp_system.h"
// 设置旋转阈值（灵敏度）//后续可调
#define ROTARY_THRESHOLD 200
// 封装电位器相关数据结构
typedef struct {
    uint32_t dma_buffer[30];    // DMA采样缓存
    float adc_value;         // 当前ADC采样值
    float previous_adc_value;// 上一次ADC采样值
    void (*onRotateRight)(void); // 右旋回调函数
    void (*onRotateLeft)(void);  // 左旋回调函数
} RotaryEncoderChannel_t;

void myadc_init(void);
void task_adc_proc(void);

#endif //INC_004_ADC_MYADC_H
