/**
  ******************************************************************************
  * @file           : myRtc.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/22
  ******************************************************************************
  */

#ifndef INC_005_CALENDAR_MYRTC_H
#define INC_005_CALENDAR_MYRTC_H

#include "bsp_system.h"

extern RTC_TimeTypeDef display_time;
extern RTC_DateTypeDef display_date;
extern RTC_TimeTypeDef modify_time;
extern RTC_DateTypeDef modify_date;

void task_rtc_proc(void);
void myrtc_init(void);
void update_rtc_set_Hander(void);

#endif //INC_005_CALENDAR_MYRTC_H
