#include "myAdc.h"
#include "adc.h"
// 回调函数声明
void rotate_right_channel_0(void);
void rotate_left_channel_0(void);
void rotate_right_channel_1(void);
void rotate_left_channel_1(void);
// 定义电位器通道实例
RotaryEncoderChannel_t rotary_channels[2] = {
        { .dma_buffer = {0},
                .adc_value = 0,
                .previous_adc_value = 0,
                .onRotateRight = rotate_right_channel_0,
                .onRotateLeft = rotate_left_channel_0 },

        { .dma_buffer = {0},
                .adc_value = 0,
                .previous_adc_value = 0,
                .onRotateRight = rotate_right_channel_1,
                .onRotateLeft = rotate_left_channel_1 }
};

/**
  * @brief  rotate_right_channel_0 `adc_value[0]` 的右旋回调函数
  **/
void rotate_right_channel_0(void) {
    buzzer_setup(50, 0.1f, 1); // 蜂鸣器
    auto_menu_handle_key(3);   // 回调菜单按键处理（右旋对应键值3）
    auto_menu_display();       // 刷新菜单显示
}

/**
  * @brief  rotate_left_channel_0 `adc_value[0]` 的左旋回调函数
  **/
void rotate_left_channel_0(void) {
    buzzer_setup(50, 0.1f, 1); // 蜂鸣器
    auto_menu_handle_key(4);   // 回调菜单按键处理（左旋对应键值4）
    auto_menu_display();       // 刷新菜单显示
}

/**
  * @brief  rotate_right_channel_1 `adc_value[1]` 的右旋回调函数
  **/
void rotate_right_channel_1(void) {
    buzzer_setup(50, 0.1f, 1); // 蜂鸣器
    auto_menu_handle_key(2);   // 回调菜单按键处理（右旋对应键值2）
    auto_menu_display();       // 刷新菜单显示
}

/**
  * @brief  rotate_left_channel_1 `adc_value[1]` 的左旋回调函数
  **/
void rotate_left_channel_1(void) {
    buzzer_setup(50, 0.1f, 1); // 蜂鸣器
    auto_menu_handle_key(1);   // 回调菜单按键处理（左旋对应键值1）
    auto_menu_display();       // 刷新菜单显示
}

/**
  * @brief  检测旋钮方向
  * @param  channel RotaryEncoderChannel_t类型的旋钮通道结构体
  * @note   检测并调用回调
  */
void process_rotary_encoder(RotaryEncoderChannel_t* channel) {
    if (abs(channel->adc_value - channel->previous_adc_value) > ROTARY_THRESHOLD) {
        if (channel->adc_value > channel->previous_adc_value) {
            channel->onRotateLeft(); // 左旋回调
        } else if (channel->adc_value < channel->previous_adc_value) {
            channel->onRotateRight(); // 右旋回调
        }
        // 更新上一轮的ADC值
        channel->previous_adc_value = channel->adc_value;
    }
}
/**
  * @brief  解析ADC采样值并更新对应通道的ADC值
  * @param  channel RotaryEncoderChannel_t类型的旋钮通道结构体
  */
void update_channel_adc_value(RotaryEncoderChannel_t* channel) {
    channel->adc_value = 0;
    for (uint8_t i = 0; i < 30; i++) {
        channel->adc_value += channel->dma_buffer[i];
    }
    channel->adc_value /= 30; // 求平均值
}

/**
  * @brief  task_adc_proc: 处理所有旋钮的采样解析
  * @note   周期性调用该函数，每隔20ms进行解析处理
  */
void task_adc_proc(void) {
    // 更新 channel 0 的 ADC 值
    update_channel_adc_value(&rotary_channels[0]);
    // 更新 channel 1 的 ADC 值
    update_channel_adc_value(&rotary_channels[1]);
    // 周期性调用该函数，用于检测旋钮变化并处理回调
    process_rotary_encoder(&rotary_channels[0]);
    process_rotary_encoder(&rotary_channels[1]);
}

/**
  * @brief  myadc_init: 初始化ADC和旋钮结构体
  * @note   在系统启动时调用
  */
void myadc_init(void) {
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)rotary_channels[0].dma_buffer, 30);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)rotary_channels[1].dma_buffer, 30);
    task_adc_proc(); // 启动时先进行一次解析，初始化 previous 值
    rotary_channels[0].previous_adc_value = rotary_channels[0].adc_value;
    rotary_channels[1].previous_adc_value = rotary_channels[1].adc_value;
}

