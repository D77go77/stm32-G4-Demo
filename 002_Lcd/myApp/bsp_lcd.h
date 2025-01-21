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


typedef struct Menu {
    const char *name;        // 菜单名字
    struct Menu *parent;     // 父菜单
    struct Menu *children;   // 子菜单列表
    uint8_t num_children;    // 子菜单个数
} Menu;

extern Menu *current_menu;      // 当前菜单
extern uint8_t current_index;   // 当前选中的菜单项索引



void task_lcd_proc(void);
void led_init(void);

#endif //INC_002_LCD_BSP_LCD_H
