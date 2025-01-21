/**
  ******************************************************************************
  * @file           : myGpio.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */
#include "myGpio.h"
// 获取GPIO_PIN_8到GPIO_PIN_15的方式
#define GPIO_PIN(i) (0x0100 << (i - 8))  // i 从 8 到 15 之间
_gpio_control  leds[8];
_gpio_control  buzzer;
/*********************************************************************
 * @fn      nGpio_init
 *
 * @brief   实现gpio端口结构体初始化
 *
 * @return  none
 */
void nGpio_init(void)
{
    for (int i = 0; i < 8; i++)
    {
        leds[i].port = GPIOC;  // All LEDs connected to GPIOC
        leds[i].pin = GPIO_PIN(8+i);  // PC8 to PC15 (pins 8 to 15)
        leds[i].level = GPIO_LOW;//Low light the gpio
        leds[i].period = 40;   // 200 ms cycle, 40 * 5 ms
        leds[i].light_on_percent = 0.5f;  // 50% ON time for even LEDs
        leds[i].times = 5;
        leds[i].reset = 1;
    }

    buzzer.port = GPIOB;  // All LEDs connected to GPIOB
    buzzer.pin = GPIO_PIN_3;  // PB3
    buzzer.level = GPIO_HIGH;//HIGH light the gpio
    buzzer.period = 40;   // 200 ms cycle, 40 * 5 ms
    buzzer.light_on_percent = 0.5f;  // 50% ON time for even BUZZER
    buzzer.times = 2;
    buzzer.reset = 1;
}

/*********************************************************************
 * @fn      gpio_control_work(_gpio_control *light)
 * @time    5ms
 * @brief   gpio驱动状态机
 * @input   _gpio_control *light-gpio控制结构体
 * @return  none
 */
void gpio_control_work(_gpio_control *light)
{
    if(light->reset==1){                                    //重置gpio控制
        light->reset=0;
        light->cnt=0;
        light->times_cnt=0;
        light->end=0;
    }
    if(light->times_cnt>=light->times){                     //gpio周期控制
        light->end=1;
        if (light->level == GPIO_HIGH) {
            HAL_GPIO_WritePin(light->port, light->pin, GPIO_PIN_RESET);  // 高电平点亮
        } else {
            HAL_GPIO_WritePin(light->port, light->pin, GPIO_PIN_SET);  // 低电平点亮
        }
        return;
    }
    light->cnt++;                                           //5ms
    if(light->cnt<=light->period*light->light_on_percent)  // 在点亮时间区间内（根据 light_on_percent）
    {
        if (light->level == GPIO_HIGH) {
            HAL_GPIO_WritePin(light->port, light->pin, GPIO_PIN_SET);  // 高电平点亮
        } else {
            HAL_GPIO_WritePin(light->port, light->pin, GPIO_PIN_RESET);  // 低电平点亮
        }
    }else if(light->cnt<light->period)                     // 在熄灭时间区间内
    {
        if (light->level == GPIO_HIGH) {
            HAL_GPIO_WritePin(light->port, light->pin, GPIO_PIN_RESET);  // 高电平点亮
        } else {
            HAL_GPIO_WritePin(light->port, light->pin, GPIO_PIN_SET);  // 低电平点亮
        }
    }else//周期结束，重置计数并记录已完成次数
    {
        light->cnt=0;
        light->times_cnt++;
    }
}
/*********************************************************************
 * @fn      led_setup
 * @time
 * @brief   led及时控制 (Dynamically control LED configuration)
 * @input   uint32_t _period, float _light_on_percent, uint16_t _times
 * @return  none
 */
void led_setup(uint8_t led_index, uint32_t _period, float _light_on_percent, uint16_t _times)
{
    if (led_index < 8)
    {
        leds[led_index].period = _period;
        leds[led_index].light_on_percent = _light_on_percent;
        leds[led_index].reset = 1;
        leds[led_index].times = _times;
    }
}

void buzzer_setup(uint32_t _period, float _light_on_percent, uint16_t _times)
{
    buzzer.period = _period;
    buzzer.light_on_percent = _light_on_percent;
    buzzer.reset = 1;
    buzzer.times = _times;
}
/*********************************************************************
 * @fn      task_led_proc
 * @time    5ms
 * @brief   led任务函数 (Handle the LED control for all 8 LEDs)
 *
 * @return  none
 */
void task_led_proc(void)
{
    // Loop through all 8 LEDs
    for (int i = 0; i < 8; i++)
    {
        gpio_control_work(&leds[i]);

    }
    GPIOD->BSRR |= 0x01 << 2;   // 设置 GPIOD 第2位
    GPIOD->BRR |= 0x01 << 2;    // 重置 GPIOD 第2位
    // Loop through  1 BUZZER
    gpio_control_work(&buzzer);
}