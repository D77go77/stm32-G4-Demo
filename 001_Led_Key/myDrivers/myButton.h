/**
  ******************************************************************************
  * @file           : myButton.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */

#ifndef INC_002_G_MYBUTTON_H
#define INC_002_G_MYBUTTON_H

#include <stdint.h>
#include <string.h>

//According to your need to modify the constants.
#define TICKS_INTERVAL    5	//ms
#define DEBOUNCE_TICKS    3	//MAX 7 (0 ~ 7)
#define SHORT_TICKS       (300 /TICKS_INTERVAL)
#define LONG_TICKS        (1000 /TICKS_INTERVAL)


typedef void (*BtnCallback)(void*);

typedef enum {
    PRESS_DOWN = 0,          // 按键按下事件
    PRESS_UP,                // 按键释放事件
    PRESS_REPEAT,            // 按键重复按下事件
    SINGLE_CLICK,            // 单击事件
    DOUBLE_CLICK,            // 双击事件
    LONG_PRESS_START,        // 长按开始事件
    LONG_PRESS_HOLD,         // 长按持续事件
    number_of_event,         // 事件数量，用于计数
    NONE_PRESS               // 无按键事件
}PressEvent;

typedef struct Button {
    uint16_t ticks;
    uint8_t  repeat : 4;
    uint8_t  event : 4;
    uint8_t  state : 3;
    uint8_t  debounce_cnt : 3;
    uint8_t  active_level : 1;
    uint8_t  button_level : 1;
    uint8_t  button_id;
    uint8_t  (*hal_button_Level)(uint8_t button_id_);
    BtnCallback  cb[number_of_event];
    struct Button* next;
}Button;

#ifdef __cplusplus
extern "C" {
#endif

void button_init(struct Button* handle, uint8_t(*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id);
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);
PressEvent get_button_event(struct Button* handle);
int  button_start(struct Button* handle);
void button_stop(struct Button* handle);
void button_ticks(void);

#ifdef __cplusplus
}
#endif



#endif //INC_002_G_MYBUTTON_H
