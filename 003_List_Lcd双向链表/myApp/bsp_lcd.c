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
// 定义全局变量
static MenuItem *current_menu = NULL;                // 当前菜单指针
static MenuItem *parent_menu_stack[MENU_STACK_SIZE]; // 父菜单堆栈
static int parent_menu_stack_index = -1;             // 堆栈索引
static int current_level = 0;                        // 当前菜单层级

/**
 * 创建一个菜单项
 * @param name 菜单项的名称
 * @param type 菜单项的类型，决定菜单项的行为
 * @param action 当菜单项被激活时执行的回调函数，仅适用于动作类型
 * @param submenu 子菜单指针，仅适用于子菜单类型
 * @return 返回新创建的菜单项指针，如果内存分配失败则返回NULL
 */
MenuItem *create_menu_item(const char *name, MenuItemType type, void (*action)(void), MenuItem *submenu){
    MenuItem *item = (MenuItem *)malloc(sizeof(MenuItem));
    if (!item) return NULL; // 内存分配失败时返回 NULL
    item->name = strdup(name); // 复制菜单项名称
    item->type = type;
    if (type == MENU_ITEM_ACTION){
        item->menu_item.action = action; // 为动作类型设置回调函数
    }
    else if (type == MENU_ITEM_SUBMENU){
        item->menu_item.submenu = submenu; // 为子菜单类型设置子菜单指针
    }
    else if (type == MENU_ITEM_BOOLEAN){
        item->menu_item.boolean_item.value = NULL;         // 初始设置为空，需在外部设置
        item->menu_item.boolean_item.toggle_action = NULL; // 初始设置为空，需在外部设置
    }
    else if (type == MENU_ITEM_NUMERIC){
        item->menu_item.numeric_item.value = NULL;         // 初始设置为空，需在外部设置
        item->menu_item.numeric_item.min = 0;              // 初始最小值为0
        item->menu_item.numeric_item.max = 100;            // 初始最大值为100
        item->menu_item.numeric_item.adjust_action = NULL; // 初始设置为空，需在外部设置
    }
    item->next = NULL;
    item->prev = NULL;
    return item;
}
/**
 * @param head 链表头指针的指针，用于可能更新链表头
 * @param item 要添加到链表的菜单项
 *
 * 此函数将一个菜单项添加到链表的末尾如果链表为空，直接将新项设为头节点
 * 否则，遍历链表到末尾，然后将新项添加为最后一个节点的下一个节点，并更新相应指针
 */
void add_menu_item(MenuItem **head, MenuItem *item){
    if (*head == NULL) *head = item; // 如果链表为空，将新项设为头节点
    else{
        MenuItem *temp = *head;
        while (temp->next != NULL){
            temp = temp->next; // 遍历到链表末尾
        }
        temp->next = item; // 将新项添加到链表末尾
        item->prev = temp; // 设置前一项的指针
    }
}
// 删除菜单项
/**
 * @param head 链表头指针的指针，用于可能更新链表头
 * @param name 要删除的菜单项的名称
 * 此函数根据菜单项的名称从链表中删除一个菜单项
 * 它首先遍历链表找到匹配名称的节点，然后更新周围节点的指针以移除该节点，
 * 并释放其占用的内存如果链表为空或参数无效，则函数不执行任何操作
 */
void remove_menu_item(MenuItem **head, const char *name){
    if (!head || !*head || !name) return;
    MenuItem *temp = *head;
    while (temp != NULL){
        if (strcmp(temp->name, name) == 0){
            if (temp->prev)
                temp->prev->next = temp->next;
            if (temp->next)
                temp->next->prev = temp->prev;
            if (temp == *head)
                *head = temp->next;
            free(temp->name);
            free(temp);
            return;
        }
        temp = temp->next;
    }
}
/**
 * 更新菜单项的信息
 * 此函数用于更新菜单项的名称和关联的操作
 * @param item 指向菜单项的指针，如果为NULL则函数不执行任何操作
 * @param new_name 新的菜单项名称，如果为NULL则函数不执行任何操作
 * @param new_action 新的菜单项操作，一个函数指针，如果菜单项类型为ACTION，则更新此函数
 */
void update_menu_item(MenuItem *item, const char *new_name, void (*new_action)(void)){
    if (!item || !new_name) return;
    free(item->name);
    item->name = strdup(new_name);
    if (item->type == MENU_ITEM_ACTION){
        item->menu_item.action = new_action;
    }
}
// 释放菜单结构的内存
void free_menu(MenuItem *menu)
{
    MenuItem *temp;
    while (menu){
        temp = menu;
        menu = menu->next;
        free(temp->name); // 释放菜单项名称的内存
        free(temp);       // 释放菜单项的内存
    }
}
// 初始化菜单
void auto_menu_init(MenuItem *root)
{
    current_menu = root;          // 设置当前菜单为根菜单
    parent_menu_stack_index = -1; // 重置父菜单堆栈索引
    current_level = 0;            // 重置菜单层级
    auto_menu_display();          // 初始化后立即显示菜单
}

/**
 * @brief 处理菜单按键操作，根据按键值更新当前菜单项并执行相应操作。
 *
 * 该函数用于处理用户输入的按键事件，根据不同的按键值进行菜单导航、进入子菜单或退出子菜单等操作。
 * 具体按键功能如下：
 * - 按键1：移动到前一项，如果当前项没有前一项，则移动到最后一项。
 * - 按键2：移动到后一项，如果当前项没有后一项，则移动到第一项。
 * - 按键3：如果是动作项则执行动作，如果是子菜单项则进入子菜单。
 * - 按键4：退出当前子菜单，返回上级菜单。
 *
 * @param key 按键值，范围为1-4，表示不同的按键操作。
 */
void auto_menu_handle_key(uint8_t key)
{
    // 处理非设置模式下的菜单导航逻辑
    switch (key){
        case 1:
            if (current_menu && current_menu->prev != NULL){
                current_menu = current_menu->prev; // 移动到前一项
            }else{
                while (current_menu && current_menu->next != NULL){
                    current_menu = current_menu->next;
                }
            }
            break;
        case 2:
            if (current_menu && current_menu->next != NULL){
                current_menu = current_menu->next; // 移动到后一项
            }else{
                while (current_menu && current_menu->prev != NULL){
                    current_menu = current_menu->prev;
                }
            }
            break;
        case 3://进入
            if (current_menu && current_menu->type == MENU_ITEM_ACTION){
                if (current_menu->menu_item.action){
                    current_menu->menu_item.action(); // 执行动作
                }
            }else if (current_menu && current_menu->type == MENU_ITEM_SUBMENU){
                if (parent_menu_stack_index < MENU_STACK_SIZE - 1){
                    parent_menu_stack[++parent_menu_stack_index] = current_menu; // 压入堆栈
                    current_menu = current_menu->menu_item.submenu;              // 进入子菜单
                    current_level++;                                             // 增加层级
                }
            }else if (current_menu && current_menu->type == MENU_ITEM_BOOLEAN){
                if (current_menu->menu_item.boolean_item.toggle_action){
                    current_menu->menu_item.boolean_item.toggle_action(); // 执行动作
                }
            }
            break;
        case 4://退出
            if (parent_menu_stack_index >= 0){
                current_menu = parent_menu_stack[parent_menu_stack_index--]; // 弹出堆栈
                current_level--;                                             // 减少层级
            }
            break;
    }
    auto_menu_display(); // 刷新菜单显示
}

#define LCD_LINE_WIDTH 20  // 假设每行最多显示 20 个字符
// 显示菜单
void auto_menu_display(void)
{
    if (current_menu == NULL){
        menu_init(); // 如果当前菜单为空，重新初始化菜单
        return;
    }
    LCD_Clear(Black); // 清除 LCD 显示
    LCD_SetTextColor(White); // 设置默认的字体颜色为白色
    LCD_SetBackColor(Black); // 设置默认的背景颜色为黑色
    MenuItem *temp = current_menu;
    uint8_t line = 0;
    // 显示当前层级
    char level_str[20];
    snprintf(level_str, sizeof(level_str), "Level: %d", current_level);
    LCD_DisplayStringLine(line++ * 24, level_str);
    // 查找当前选中项的第一个可显示的菜单项
    while (temp->prev != NULL && line < 4){
        temp = temp->prev;
        line++;
    }
    // 重置行计数器
    line = 1;
    // 遍历从当前选中项开始的菜单项并显示
    while (temp != NULL && line < MAX_LINES){
        if (temp->name == NULL || temp->name[0] == '\0'){
            break;
        }
        if (temp == current_menu){// 如果当前菜单项是选中项，使用反转颜色
            LCD_SetTextColor(Black);
            LCD_SetBackColor(White);
        }else{
            LCD_SetTextColor(White);
            LCD_SetBackColor(Black);
        }
        char buffer[LCD_LINE_WIDTH + 1] = {0};  // +1 保留 NULL 终止符
        size_t name_length = strlen(temp->name);

        // 如果名字长度小于行宽，复制名字并在末尾补空格
        if (name_length < LCD_LINE_WIDTH) {
            memcpy(buffer, temp->name, name_length);             // 拷贝名字
            memset(buffer + name_length, ' ', LCD_LINE_WIDTH - name_length);  // 补空格
        } else {
            // 如果名字太长，截断到行宽
            memcpy(buffer, temp->name, LCD_LINE_WIDTH);
        }

        buffer[LCD_LINE_WIDTH] = '\0';  // 确保以 NULL 终止
        LCD_DisplayStringLine(line * 24, buffer); // 显示菜单项
        temp = temp->next; // 遍历兄弟项
        line++;
    }
}
// 动作回调函数示例
void action_1_Hander(void){
//    printf("Action 1 triggered!\n");
}

void action_2_Hander(void){
//    printf("Action 2 triggered!\n");
}

// 布尔开关的回调函数
void toggle_bool_Hander(void){
    // 获取当前布尔值
    uint8_t *bool_value = current_menu->menu_item.boolean_item.value;
    *bool_value = !(*bool_value); // 切换布尔值
    // 动态修改菜单名称，附加 ON/OFF
    snprintf(current_menu->name, 22, "Toggle Bool %s", *bool_value ? "ON" : "OFF");
}
// 数值调整的回调函数
void adjust_value_Hander(int adjustment){
    static int value = 50; // 初始值为50
    value += adjustment;
    if (value < 0)
        value = 0;
    if (value > 100)
        value = 100;
//    printf("Value adjusted: %d\n", value);
}
uint8_t bool_test = 0;
int num_test = 0;
void menu_init(void){
    // 初始化菜单结构，这里可以加载保存的配置或者生成默认菜单

    // 创建菜单结构
    MenuItem *root_menu = create_menu_item("Main Menu", MENU_ITEM_SUBMENU, NULL, NULL);
    MenuItem *submenu1 = create_menu_item("SubMenu 1", MENU_ITEM_SUBMENU, NULL, NULL);
    MenuItem *submenu2 = create_menu_item("SubMenu 2", MENU_ITEM_SUBMENU, NULL, NULL);
    MenuItem *action_item1 = create_menu_item("Action 1", MENU_ITEM_ACTION, action_1_Hander, NULL);
    MenuItem *action_item2 = create_menu_item("Action 2", MENU_ITEM_ACTION, action_2_Hander, NULL);
    MenuItem *submenu3 = create_menu_item("SubMenu 3", MENU_ITEM_SUBMENU, NULL, NULL);

    // 创建布尔类型菜单项
    MenuItem *boolean_item = create_menu_item("Toggle Bool", MENU_ITEM_BOOLEAN, NULL, NULL);
    boolean_item->menu_item.boolean_item.value = &bool_test;//bool value test
    boolean_item->menu_item.boolean_item.toggle_action = toggle_bool_Hander;

    // 创建数值调节菜单项
    MenuItem *numeric_item = create_menu_item("Adjust Value", MENU_ITEM_NUMERIC, NULL, NULL);
    numeric_item->menu_item.numeric_item.value = &num_test;//num value test
    numeric_item->menu_item.numeric_item.min = 0;
    numeric_item->menu_item.numeric_item.max = 100;
    numeric_item->menu_item.numeric_item.adjust_action = adjust_value_Hander;

    // 构建菜单层次结构
    add_menu_item(&root_menu->menu_item.submenu, action_item1);
    add_menu_item(&root_menu->menu_item.submenu, submenu1);
    add_menu_item(&root_menu->menu_item.submenu, submenu2);
    add_menu_item(&submenu1->menu_item.submenu, action_item2);
    add_menu_item(&submenu1->menu_item.submenu, boolean_item);
    add_menu_item(&submenu1->menu_item.submenu, numeric_item);
    add_menu_item(&submenu2->menu_item.submenu, submenu3);

    // 初始化菜单系统
    auto_menu_init(root_menu);
}
/**
  * @brief  mylcd_init lcd初始化
  * @note   None
  * @param  None
  * @retval None
  */
void mylcd_init(void)
{
    // 初始化菜单
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetTextColor(White);
    LCD_SetBackColor(Black);
    menu_init();
}