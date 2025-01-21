/**
  ******************************************************************************
  * @file           : bsp_lcd.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */
#include "bsp_lcd.h"

Menu subMenu1[] = {
        {" Sub 1-1   ", NULL, NULL, 0},  // 子菜单项 1-1
        {" Sub 1-2   ", NULL, NULL, 0},  // 子菜单项 1-2
        {" Sub 1-3   ", NULL, NULL, 0}
};

Menu subMenu2[] = {
        {" Sub 2-1   ", NULL, NULL, 0},  // 子菜单项 2-1
        {" Sub 2-2   ", NULL, NULL, 0}  // 子菜单项 2-2
};

Menu mainMenu[] = {
        {" Menu 1   ", NULL, subMenu1, 3}, // 主菜单项 1，有 2 子菜单
        {" Menu 2   ", NULL, subMenu2, 2}, // 主菜单项 2，有 2 子菜单
        {" Menu 3   ", NULL, NULL,     0}, // 主菜单项 3，无子菜单
        {" Menu 4   ", NULL, NULL,     0}
};

Menu startMenu[]={//此界面是显示主界面-父系
        {"Start", NULL, mainMenu, 4}
};
Menu *current_menu;      // 当前菜单
uint8_t current_index;   // 当前选中的菜单项索引

/**
 * @brief  格式化字符串并显示在指定的LCD行上。
 *
 * 该函数接受一个行号和一个格式化字符串（类似于printf），
 * 格式化字符串后，将其显示在LCD的指定行上。
 *
 * @param  Line    要显示字符串的LCD行号。
 * @param  format  格式化字符串，后跟要格式化的参数。
 *
 * 该函数内部使用 `vsprintf` 来格式化字符串，然后
 * 调用 `LCD_DisplayStringLine` 在LCD上显示格式化后的字符串。
 *
 * 示例用法:
 * @code
 * LcdSprintf(0, "Temperature: %d C", temperature);
 * @endcode
 */
void LcdSprintf(uint8_t Line, char *format,...)
{
    char String[21];  // 缓冲区用于存储格式化后的字符串
    va_list arg;      // 参数列表用于存储可变参数
    va_start(arg, format);  // 使用格式化字符串初始化参数列表
    vsprintf(String, format, arg);  // 格式化字符串并存储在缓冲区中
    va_end(arg);  // 清理参数列表
    LCD_DisplayStringLine(Line*24,String);  // 在LCD的指定行显示格式化后的字符串
}

/**
  * @brief  display_menu
  * @note   None
  * @param  Menu 菜单子列表 index 菜单索引
  * @retval None
  */
void display_menu(Menu *menu, uint8_t index)
{
    // 检查菜单对象是否为空或没有子元素
    if (menu == NULL || menu->num_children == 0) {
        LcdSprintf(1, "No Menu");
        return;
    }
    if (menu->parent != NULL) {
        LcdSprintf(0, "Parent: %s", menu->parent->name); // 显示父菜单的名称
    } else {
        LcdSprintf(0, "Parent: None"); // 如果没有父菜单，显示 None
    }
    // 遍历菜单项，最多显示10行
    for (uint8_t i = 0; i < 9; i++) {
        if (i < menu->num_children) { // 如果当前索引小于菜单的子项数量
            if (i == index) {
                LcdSprintf(i + 1, "> %s", menu->children[i].name); // 高亮显示
            } else {
                LcdSprintf(i + 1, "  %s", menu->children[i].name);
            }
        } else {
            LcdSprintf(i + 1, "           "); // 清空未用的行
        }
    }
}
/**
  * @brief  setup_menus 菜单父系界面绑定
  * @note   None
  * @param  None
  * @retval None
  */
void setup_menus(void)
{
    // 设置子菜单 1 的父指针
    for (int i = 0; i < 3; i++) {
        subMenu1[i].parent = &mainMenu[0]; // SubMenu1 的父菜单是 MainMenu 的第一项
    }
    // 设置子菜单 2 的父指针
    for (int i = 0; i < 2; i++) {
        subMenu2[i].parent = &mainMenu[1]; // SubMenu2 的父菜单是 MainMenu 的第二项
    }
    // 设置主菜单 1 的父指针
    for (int i = 0; i < 2; i++) {
        mainMenu[i].parent = &startMenu[0]; // SubMenu2 的父菜单是 MainMenu 的第二项
    }
}
/**
  * @brief  task_lcd_proc led任务函数
  * @note   100ms
  * @param  None
  * @retval None
  */
void task_lcd_proc(void)
{
    display_menu(current_menu, current_index);
}

/**
  * @brief  led_init led初始化
  * @note   None
  * @param  None
  * @retval None
  */
void led_init(void)
{
    setup_menus();                      // 初始化菜单父子关系
    current_menu = startMenu;            // 启动时显示主菜单
    current_index = 0;                  // 默认高亮主菜单的第一个条目
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetTextColor(White);
    LCD_SetBackColor(Black);
    display_menu(current_menu, current_index); // 初始化显示主菜单
}


