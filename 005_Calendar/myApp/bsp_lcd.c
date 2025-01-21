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
#include "myRtc.h"
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
void free_menu(MenuItem *menu){
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

void calc_week(void);
// 显示菜单
void auto_menu_display(void)
{
    if (current_menu == NULL){
        menu_init(); // 如果当前菜单为空，重新初始化菜单
        return;
    }
    LCD_Clear(Black);
    LCD_SetTextColor(White);
    LCD_SetBackColor(Black);
    MenuItem *temp = current_menu;
    uint8_t line = 0;
    // 显示菜单外显示图层
    switch(current_level)
    {
        case 0:
            LcdSprintf(0,"  State : ToBeStart");
            break;
        case 1:
            LcdSprintf(0,"  State : Running");
            break;
        case 2:
            LcdSprintf(0,"  State : Setting");
            break;
    }
    LcdSprintf(9,"  Writen by @Z1R343L");

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
        // 显示菜单项
        switch(current_level)
        {
            case 0:
                LcdSprintf(4, "      %s      ",current_menu->name);
                break;
            case 1:
                LcdSprintf(2, "     20%02d-%02d-%02d      ",display_date.Year,display_date.Month,display_date.Date);//data
                LcdSprintf(4, "      %02d:%02d:%02d      ",display_time.Hours,display_time.Minutes,display_time.Seconds);//time
                break;
            case 2:
                LcdSprintf(2, "     20%02d-%02d-%02d      ",modify_date.Year,modify_date.Month,modify_date.Date);//data
                LcdSprintf(4, "      %02d:%02d:%02d      ",modify_time.Hours,modify_time.Minutes,modify_time.Seconds);//time
                break;
        }
        temp = temp->next; // 遍历兄弟项
        line++;
    }
}
/**
 * 回调函数：修改日期字段
 */
void adjust_date_time(uint8_t dir) {
    uint8_t *value = current_menu->menu_item.numeric_item.value;
    if(dir) {
        (*value)++;
        if (*value > current_menu->menu_item.numeric_item.max)
            *value = current_menu->menu_item.numeric_item.min;
        if (*value < current_menu->menu_item.numeric_item.min)
            *value = current_menu->menu_item.numeric_item.max;
    }else {
        (*value)--;
        if (*value > current_menu->menu_item.numeric_item.max)
            *value = current_menu->menu_item.numeric_item.min;
        if (*value < current_menu->menu_item.numeric_item.min)
            *value = current_menu->menu_item.numeric_item.max;
    }
    // 更新 LCD 显示
}
/**
 * 回调函数：保存修改的日期和时间
 */
int CaculateWeekDay(int y,int m, int d)
{
    if(m==1||m==2) {
        m+=12;
        y--;
    }
    int week=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7+1;
    return week;
}
void save_calendar(void) {
//    W= (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) mod 7
    modify_date.WeekDay = CaculateWeekDay(modify_date.Year,modify_date.Month,modify_date.Date);
    HAL_RTC_SetDate(&hrtc, &modify_date, RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc, &modify_time, RTC_FORMAT_BIN);
}
void get_calendar_time(void) {
    modify_time = display_time;
    modify_date = display_date;
}
void menu_init(void){
    // 创建主菜单
    MenuItem *calendar_menu = create_menu_item("Calendar",          MENU_ITEM_SUBMENU, NULL, NULL);
    // 创建子菜单：万年历逻辑
    // 万年历功能：显示当前日期和时间
    // 万年历功能：修改日期（包含子菜单）// 创建数值调节菜单项
    MenuItem *modify_date_menu = create_menu_item("Modify Date",    MENU_ITEM_SUBMENU, NULL, NULL);
    MenuItem *modify_year = create_menu_item("Year",                MENU_ITEM_NUMERIC, NULL, NULL);
    modify_year->menu_item.numeric_item.value = &modify_date.Year;
    modify_year->menu_item.numeric_item.min = 0;
    modify_year->menu_item.numeric_item.max = 99;
    modify_year->menu_item.numeric_item.adjust_action = adjust_date_time;
    MenuItem *modify_month = create_menu_item("Month",              MENU_ITEM_NUMERIC, NULL, NULL);
    modify_month->menu_item.numeric_item.value = &modify_date.Month;
    modify_month->menu_item.numeric_item.min = 1;
    modify_month->menu_item.numeric_item.max = 12;
    modify_month->menu_item.numeric_item.adjust_action = adjust_date_time;
    MenuItem *modify_day = create_menu_item("Day",                  MENU_ITEM_NUMERIC, NULL, NULL);
    modify_day->menu_item.numeric_item.value = &modify_date.Date;
    modify_day->menu_item.numeric_item.min = 1;
    modify_day->menu_item.numeric_item.max = 30;
    modify_day->menu_item.numeric_item.adjust_action = adjust_date_time;
    // 绑定父链表
    add_menu_item(&modify_date_menu->menu_item.submenu, modify_year);
    add_menu_item(&modify_date_menu->menu_item.submenu, modify_month);
    add_menu_item(&modify_date_menu->menu_item.submenu, modify_day);
    // 万年历功能：修改时间（包含子菜单）
    MenuItem *modify_hour = create_menu_item("Hour",                MENU_ITEM_NUMERIC, NULL, NULL);
    modify_hour->menu_item.numeric_item.value = &modify_time.Hours;
    modify_hour->menu_item.numeric_item.min = 0;
    modify_hour->menu_item.numeric_item.max = 23;
    modify_hour->menu_item.numeric_item.adjust_action = adjust_date_time;
    MenuItem *modify_minute = create_menu_item("Minute",            MENU_ITEM_NUMERIC, NULL, NULL);
    modify_minute->menu_item.numeric_item.value = &modify_time.Minutes;
    modify_minute->menu_item.numeric_item.min = 0;
    modify_minute->menu_item.numeric_item.max = 59;
    modify_minute->menu_item.numeric_item.adjust_action = adjust_date_time;
    MenuItem *modify_second = create_menu_item("Second",            MENU_ITEM_NUMERIC, NULL, NULL);
    modify_second->menu_item.numeric_item.value = &modify_time.Seconds;
    modify_second->menu_item.numeric_item.min = 0;
    modify_second->menu_item.numeric_item.max = 59;
    modify_second->menu_item.numeric_item.adjust_action = adjust_date_time;
    add_menu_item(&modify_date_menu->menu_item.submenu, modify_hour);
    add_menu_item(&modify_date_menu->menu_item.submenu, modify_minute);
    add_menu_item(&modify_date_menu->menu_item.submenu, modify_second);
    // 将日期修改和时间修改功能添加到万年历子菜单
    add_menu_item(&calendar_menu->menu_item.submenu, modify_date_menu);
    // 初始化菜单
    auto_menu_init(calendar_menu);
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
/**
  * @brief  task_lcd_proc
  * @note   100
  * @param  None
  * @retval None
  */
void task_lcd_proc(void)
{
    // 显示菜单项
    switch(current_level){
        case 1:
            LcdSprintf(2, "     20%02d-%02d-%02d      ",display_date.Year,display_date.Month,display_date.Date);//data
            LcdSprintf(4, "      %02d:%02d:%02d      ",display_time.Hours,display_time.Minutes,display_time.Seconds);//time
            calc_week();
            break;
        case 2:
            LcdSprintf(2, "     20%02d-%02d-%02d      ",modify_date.Year,modify_date.Month,modify_date.Date);//data
            LcdSprintf(4, "      %02d:%02d:%02d      ",modify_time.Hours,modify_time.Minutes,modify_time.Seconds);//time
            if (strcmp(current_menu->name, "Year") == 0) { // 比较字符串是否是 "Year"
                LcdSprintf(3, "     ^^^^     ");
                LcdSprintf(5, "              ");
            } else if (strcmp(current_menu->name, "Month") == 0) { // 比较字符串是否是 "Month"
                LcdSprintf(3, "          ^^     ");
            } else if (strcmp(current_menu->name, "Day") == 0) { // 比较字符串是否是 "Day"
                LcdSprintf(3, "             ^^ ");
                LcdSprintf(5, "              ");
            } else if (strcmp(current_menu->name, "Hour") == 0) { // 比较字符串是否是 "Hours"
                LcdSprintf(3, "               ");
                LcdSprintf(5, "      ^^       ");
            } else if (strcmp(current_menu->name, "Minute") == 0) { // 比较字符串是否是 "Minutes"
                LcdSprintf(5, "         ^^     ");
            } else if (strcmp(current_menu->name, "Second") == 0) { // 比较字符串是否是 "Seconds"
                LcdSprintf(3, "               ");
                LcdSprintf(5, "            ^^     ");
            }
            calc_week();
            break;
    }
}

void calc_week(void){
    switch(display_date.WeekDay){
        case 7:LcdSprintf(6, "      Sunday      ");break;
        case 1:LcdSprintf(6, "      Monday      ");break;
        case 2:LcdSprintf(6, "      Tuesday      ");break;
        case 3:LcdSprintf(6, "      Wednesday      ");break;
        case 4:LcdSprintf(6, "      Thursday      ");break;
        case 5:LcdSprintf(6, "      Friday      ");break;
        case 6:LcdSprintf(6, "      Saturday     ");break;
    }
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
                get_calendar_time();
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
                if(current_level == 1)save_calendar();
            }
            break;
        case 5://++
            if(current_level != 2) return;
                if (current_menu && current_menu->type == MENU_ITEM_NUMERIC){
                    if (current_menu->menu_item.numeric_item.adjust_action){
                        current_menu->menu_item.numeric_item.adjust_action(1); // 执行动作
                    }
                }
            break;
        case 6://--
            if(current_level != 2) return;
                if (current_menu && current_menu->type == MENU_ITEM_NUMERIC){
                    if (current_menu->menu_item.numeric_item.adjust_action){
                        current_menu->menu_item.numeric_item.adjust_action(0); // 执行动作
                    }
                }
            break;
    }
    if(current_level == 2) return;//调参界面不刷屏
    auto_menu_display(); // 刷新菜单显示
}