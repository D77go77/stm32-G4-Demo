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
  * @brief  button_callback 按键控制回调函数
  * @note   _btn->event 按键状态 _btn->buttun_id 按键编号
  * @param  void* btn 回调句柄
  * @retval None
  */
void button_callback(void* btn) {
    struct Button *_btn = btn;

    switch (_btn->event) {
        case PRESS_DOWN:
//			 LOG_INFO("---> key%d press down! <---\r\n",btn->button_id);
            switch (_btn->button_id) {
                case btn1_id:
                    led_setup(0, 40, 0.5f, 3);
                    buzzer_setup(50, 0.1f, 1);
                    navigate_menu(0);
                    break;
                case btn2_id:
                    led_setup(1, 40, 0.5f, 3);
                    buzzer_setup(50, 0.1f, 1);
                    navigate_menu(1);
                    break;
                case btn3_id:
                    led_setup(2, 40, 0.5f, 3);
                    buzzer_setup(50, 0.1f, 1);
                    navigate_menu(2);
                    break;
                case btn4_id:
                    led_setup(3, 40, 0.5f, 3);
                    buzzer_setup(50, 0.1f, 1);
                    navigate_menu(3);
                    break;
                default:
                    // 处理无效按键情况，例如记录日志或返回
                    break;
            }
            break;
        case PRESS_UP:
//			 LOG_INFO("---> key%d press up! <---\r\n",btn->button_id);
            break;
        case PRESS_REPEAT:
//			 LOG_INFO("---> key%d press repeat! <---\r\n",btn->button_id);
            break;
        case SINGLE_CLICK:
//			 LOG_INFO("---> key%d single click! <---\r\n",btn->button_id);
            break;
        case DOUBLE_CLICK:
//			 LOG_INFO("---> key%d double click! <***\r\n",btn->button_id);
            break;
        case LONG_PRESS_START:
//			 LOG_INFO("---> key%d long press start! <---\r\n",btn->button_id);
            switch (_btn->button_id) {
                case btn4_id:
                    buzzer_setup(50, 0.5f, 1);
                    navigate_menu(4);
                    break;
            }
            break;
        case LONG_PRESS_HOLD:
//			 LOG_INFO("---> key%d long press hold! <***\r\n",btn->button_id);
            break;
    }
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

    button_attach(&btn1, PRESS_DOWN,       button_callback);
    button_attach(&btn2, PRESS_DOWN,       button_callback);
    button_attach(&btn3, PRESS_DOWN,       button_callback);
    button_attach(&btn4, PRESS_DOWN,       button_callback);
    button_attach(&btn4, LONG_PRESS_START, button_callback);

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