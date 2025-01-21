/**
  ******************************************************************************
  * @file           : myRtc.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/22
  ******************************************************************************
  */
#include "myRtc.h"

RTC_TimeTypeDef display_time;
RTC_DateTypeDef display_date;
RTC_TimeTypeDef modify_time;
RTC_DateTypeDef modify_date;

void set_rtc_time_Hander(void){
    HAL_RTC_SetTime(&hrtc, &modify_time, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &modify_date, RTC_FORMAT_BIN);
}

void update_rtc_set_Hander(void){
    modify_time = display_time;
    modify_date = display_date;
}

void task_rtc_proc(void){
    HAL_RTC_GetTime(&hrtc, &display_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &display_date, RTC_FORMAT_BIN);
}

void myrtc_init(void){
    HAL_RTC_GetTime(&hrtc, &display_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &display_date, RTC_FORMAT_BIN);
    update_rtc_set_Hander();// 更新时间设定值
}

