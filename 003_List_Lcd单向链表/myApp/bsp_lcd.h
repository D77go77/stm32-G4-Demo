/**
  ******************************************************************************
  * @file           : bsp_lcd.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */

#ifndef INC_002_LCD_BSP_LCD_H
#define INC_002_LCD_BSP_LCD_H

#include "bsp_system.h"


void task_lcd_proc(void);
void led_init(void);
void navigate_menu(uint8_t direction);//导航方向（0 上移，1 下移，2 进入子菜单，3 返回父菜单）

#endif //INC_002_LCD_BSP_LCD_H
