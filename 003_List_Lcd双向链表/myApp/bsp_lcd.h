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

// 菜单项类型定义
typedef enum
{
    MENU_ITEM_ACTION,  // 执行动作的菜单项
    MENU_ITEM_SUBMENU, // 子菜单项
    MENU_ITEM_BOOLEAN, // 布尔类型菜单项（开关）
    MENU_ITEM_NUMERIC  // 数值调节菜单项
} MenuItemType;

// 菜单项结构体
typedef struct MenuItem
{
    char *name;        // 菜单项名称
    MenuItemType type; // 菜单项类型
    union
    {
        void (*action)(void);     // 菜单动作
        struct MenuItem *submenu; // 子菜单指针
        struct
        { // 布尔类型菜单项数据
            uint8_t *value;
            void (*toggle_action)();
        } boolean_item;
        struct
        { // 数值调节菜单项数据
            int *value;
            int min;
            int max;
            void (*adjust_action)(int);
        } numeric_item;
    } menu_item;
    struct MenuItem *next; // 下一个菜单项
    struct MenuItem *prev; // 上一个菜单项
} MenuItem;

// 父菜单堆栈的大小定义
#define MENU_STACK_SIZE 10

// 最大行数定义
#define MAX_LINES 10

// 初始化菜单系统
void auto_menu_init(MenuItem *root);

// 处理按键输入
void auto_menu_handle_key(uint8_t key);

// 显示菜单（通常在每次按键处理后调用）
void auto_menu_display(void);

// 创建一个菜单项
MenuItem *create_menu_item(const char *name, MenuItemType type, void (*action)(void), MenuItem *submenu);

// 将菜单项添加到链表中
void add_menu_item(MenuItem **head, MenuItem *item);

// 删除菜单项
void remove_menu_item(MenuItem **head, const char *name);

// 更新菜单项
void update_menu_item(MenuItem *item, const char *new_name, void (*new_action)(void));

// 释放菜单结构的内存
void free_menu(MenuItem *menu);

void menu_init(void);

void mylcd_init(void);

#endif //INC_002_LCD_BSP_LCD_H

