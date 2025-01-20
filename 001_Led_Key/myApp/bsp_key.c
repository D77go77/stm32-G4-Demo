/**
  ******************************************************************************
  * @file           : bsp_key.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */
#include "bsp_key.h"

enum Button_IDs {
    btn1_id,
    btn2_id,
    btn3_id,
    btn4_id,
};

struct Button btn1;
struct Button btn2;
struct Button btn3;
struct Button btn4;
/**
  * @brief  read_button_GPIO 按键读取函数
  * @note   None
  * @param  button_id 
  * @retval None
  */
uint8_t read_button_GPIO(uint8_t button_id)
{
    // you can share the GPIO read function with multiple Buttons
    switch(button_id)
    {
        case btn1_id:
            return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
        case btn2_id:
            return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
        case btn3_id:
            return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
        case btn4_id:
            return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
        default:
            return 0;
    }
}


/**
  * @brief  BTN_Hander 按键控制回调函数
  * @note   None
  * @param  void* btn 回调句柄
  * @retval None
  */
void BTN1_PRESS_DOWN_Handler(void* btn)
{
    //do something...
    led_setup(0, 40, 0.5f, 3);
}
void BTN2_PRESS_DOWN_Handler(void* btn)
{
    //do something...
    led_setup(2, 40, 0.5f, 3);
}
void BTN3_PRESS_DOWN_Handler(void* btn)
{
    //do something...
    led_setup(4, 40, 0.5f, 3);
}
void BTN4_PRESS_DOWN_Handler(void* btn)
{
    //do something...
    led_setup(6, 40, 0.5f, 3);
}

/**
  * @brief  button_init_all 按键句柄初始化集合
  * @note   None
  * @param  None
  * @retval None
  */
void button_init_all(void)
{
    button_init(&btn1, read_button_GPIO, 0, btn1_id);
    button_init(&btn2, read_button_GPIO, 0, btn2_id);
    button_init(&btn3, read_button_GPIO, 0, btn3_id);
    button_init(&btn4, read_button_GPIO, 0, btn4_id);

    button_attach(&btn1, PRESS_DOWN,       BTN1_PRESS_DOWN_Handler);
    button_attach(&btn2, PRESS_DOWN,       BTN2_PRESS_DOWN_Handler);
    button_attach(&btn3, PRESS_DOWN,       BTN3_PRESS_DOWN_Handler);
    button_attach(&btn4, PRESS_DOWN,       BTN4_PRESS_DOWN_Handler);

//    //for example
//    button_attach(&btn2, PRESS_DOWN,       BTN2_PRESS_DOWN_Handler);
//    button_attach(&btn2, PRESS_UP,         BTN2_PRESS_UP_Handler);
//    button_attach(&btn2, PRESS_REPEAT,     BTN2_PRESS_REPEAT_Handler);
//    button_attach(&btn2, SINGLE_CLICK,     BTN2_SINGLE_Click_Handler);
//    button_attach(&btn2, DOUBLE_CLICK,     BTN2_DOUBLE_Click_Handler);
//    button_attach(&btn2, LONG_PRESS_START, BTN2_LONG_PRESS_START_Handler);
//    button_attach(&btn2, LONG_PRESS_HOLD,  BTN2_LONG_PRESS_HOLD_Handler);

    button_start(&btn1);
    button_start(&btn2);
    button_start(&btn3);
    button_start(&btn4);
}