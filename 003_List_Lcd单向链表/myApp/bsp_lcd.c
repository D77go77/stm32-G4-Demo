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

// 菜单结构体定义
typedef struct Menu {
    char *name;                // 菜单名称
    struct Menu *parent;       // 父菜单
    struct Menu *children;     // 指向子菜单链表的指针
    struct Menu *next;         // 下一个兄弟菜单指针
    uint8_t num_children;      // 子菜单数量
} Menu;
// 全局变量
Menu *current_menu;       // 当前菜单指针
uint8_t current_index;    // 当前选中的菜单索引
/**
 * @brief  格式化字符串并显示在指定的LCD行上。
 * 该函数接受一个行号和一个格式化字符串（类似于printf），
 * 格式化字符串后，将其显示在LCD的指定行上。
 * @param  Line    要显示字符串的LCD行号。
 * @param  format  格式化字符串，后跟要格式化的参数。
 * 该函数内部使用 `vsprintf` 来格式化字符串，然后
 * 调用 `LCD_DisplayStringLine` 在LCD上显示格式化后的字符串。
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
 * @brief  创建一个菜单节点
 * @param  name     菜单项名字
 * @param  parent   父菜单指针
 * @retval 动态分配的菜单节点
 */
Menu* create_menu_item(const char *name, Menu *parent) {
    Menu *menu = (Menu *)malloc(sizeof(Menu));
    if (!menu) {
//        printf("Error: Memory allocation failed!\n");
        return NULL;
    }
    menu->name = strdup(name);    // 创建菜单名称并分配新内存
    menu->parent = parent;        // 设置父菜单
    menu->children = NULL;        // 子菜单链表为空
    menu->next = NULL;            // 下一个兄弟菜单为空
    menu->num_children = 0;       // 未设置子菜单数量
    return menu;
}
/**
* @brief  向菜单中添加子菜单
* @param  parent     父菜单指针
* @param  child_name 子菜单名称
* @retval 新创建的子菜单
*/
Menu* add_submenu(Menu *parent, const char *child_name) {
    if (!parent) {
//        printf("Error: Parent menu is null!\n");
        return NULL;
    }
    // 创建子菜单
    Menu *new_child = create_menu_item(child_name, parent);
    if (!new_child) return NULL; // 创建失败
    // 将子菜单添加到链表
    if (!parent->children) {
        parent->children = new_child; // 第一个子菜单
    } else {
        Menu *temp = parent->children;
        while (temp->next) {
            temp = temp->next; // 遍历到链表末尾
        }
        temp->next = new_child;
    }
    parent->num_children++; // 更新父菜单的子菜单数量
    return new_child;
}
/**
 * @brief 递归释放给定菜单节点及其所有子节点
 * @param menu 要释放的菜单节点
 */
void delete_menu(Menu *menu) {
    if (!menu) return;
    // 删除子菜单
    Menu *child = menu->children;
    while (child) {
        Menu *next_child = child->next;
        delete_menu(child); // 递归删除子菜单
        child = next_child;
    }
    // 释放当前菜单
//    printf("Deleting menu: %s\n", menu->name);
    free(menu->name);
    free(menu);
}
/**
 * @brief 删除指定的子菜单节点（从链表中删除并释放内存）
 * @param parent   父菜单指针（从其子菜单链表中删除）
 * @param target   要删除的子菜单节点
 * @retval 1 成功，0 失败（未找到目标子菜单）
 */
int delete_submenu(Menu *parent, Menu *target) {
    if (!parent || !parent->children || !target) {
        return 0; // 无效的输入参数
    }
    Menu *child = parent->children;
    Menu *previous = NULL;
    // 遍历链表以找到目标
    while (child) {
        if (child == target) { // 找到目标节点
            // 如果目标是第一个子菜单
            if (previous == NULL) {
                parent->children = child->next; // 更新父菜单的 children 指针
            } else {
                previous->next = child->next; // 跳过目标节点
            }
            // 减少子菜单计数并释放目标节点（包括递归释放其子孙节点）
            parent->num_children--;
            delete_menu(child); // 递归释放子菜单内存
            return 1; // 删除成功
        }
        // 继续遍历下一个子菜单
        previous = child;
        child = child->next;
    }
    return 0; // 目标节点未找到
}
/**
 * @brief  显示菜单树（递归）
 * @param  menu   当前菜单
 * @param  level  菜单层级（用于缩进显示）
 */
void display_menu(Menu *menu, uint8_t index) {
    // 检查菜单是否为空
    if (menu == NULL || menu->num_children == 0) {
        LcdSprintf(1, "No menu found!");
        return;
    }
    LCD_Clear(Black); // 清除 LCD 显示

    // 顶部显示当前菜单名字
    if (menu->parent) {
        LcdSprintf(0, "Last: %s", menu->name);
    } else {
        LcdSprintf(0, "Last: None");
    }
    // 遍历当前菜单的子菜单项并显示
    Menu *child = menu->children; // 指向子菜单链表
    for (int i = 0; i < 9; i++) {
        if (!child) break; // 子链表问题的保护：避免空链表遍历
        if (i == index) {// 高亮当前选中的菜单项
            LCD_SetTextColor(Black);//反转颜色
            LCD_SetBackColor(White);
            LcdSprintf(i + 1, "> %s", child->name); // > 表示选中
        } else {

            LcdSprintf(i + 1, "  %s", child->name); // 普通显示
        }
        LCD_SetTextColor(White);
        LCD_SetBackColor(Black);
        child = child->next; // 继续遍历下一个子菜单
    }
}
/**
 * @brief  导航菜单
 * @param  direction 导航方向（0 上移，1 下移，2 进入子菜单，3 返回父菜单）
 */
void navigate_menu(uint8_t direction) {
    if (!current_menu) return;

    switch (direction) {
        case 0: { // 上移
            if (current_index > 0) {
                current_index--;
            }
            break;
        }
        case 1: { // 下移
            if (current_index < current_menu->num_children - 1) {
                current_index++;
            }
            break;
        }
        case 2: { // 进入子菜单
            Menu *child = current_menu->children;
            for (uint8_t i = 0; i < current_index && child; i++) {
                child = child->next;
            }
            if (child && child->num_children > 0) {
                current_menu = child; // 进入子菜单
                current_index = 0;   // 重置索引
            }
            break;
        }
        case 3: { // 返回父菜单
            if (current_menu->parent) {
                current_menu = current_menu->parent; // 返回父菜单
                current_index = 0;                  // 重置索引
            }
            break;
        }
        case 4: {
            Menu *menu1 = current_menu->children;
            Menu *sub_1_2 = menu1->children->next; // 指向 "Sub 1-2"
            delete_submenu(menu1, sub_1_2);
            break;
        }
        default:
            break;
    }
    // 刷新显示
    display_menu(current_menu, current_index);
}
/**
 * @brief  初始化菜单结构
 * @retval 根菜单
 */
Menu* init_menu(void) {
    Menu *root = create_menu_item("Start Menu", NULL);
    // 创建主菜单
    Menu *menu1 = add_submenu(root, "Menu 1");
    Menu *menu2 = add_submenu(root, "Menu 2");
    Menu *menu3 = add_submenu(root, "Menu 3");
    // 创建 Menu 1 的子菜单
    add_submenu(menu1, "Sub 1-1");
    add_submenu(menu1, "Sub 1-2");
    add_submenu(menu1, "Sub 1-3");
    // 创建 Menu 2 的子菜单
    add_submenu(menu2, "Sub 2-1");
    add_submenu(menu2, "Sub 2-2");
    // 创建 Menu 3 的子菜单
    add_submenu(menu3, "Sub 3-1");
    return root;
}
/**
  * @brief  task_lcd_proc led任务函数
  * @note   100ms
  * @param  None
  * @retval None
  */
void task_lcd_proc(void)
{

}
/**
  * @brief  led_init led初始化
  * @note   None
  * @param  None
  * @retval None
  */
void led_init(void)
{
    // 初始化菜单
    Menu *root_menu = init_menu();
    current_menu = root_menu;
    current_index = 0;
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetTextColor(White);
    LCD_SetBackColor(Black);
    // 显示菜单
    display_menu(current_menu, current_index);
}


