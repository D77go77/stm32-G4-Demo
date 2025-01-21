/**
  ******************************************************************************
  * @file           : bsp_system.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */

#ifndef INC_002_G_BSP_SYSTEM_H
#define INC_002_G_BSP_SYSTEM_H

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include "math.h"
#include "stm32g4xx_hal.h"


//Core
#include "rtc.h"
//myUser
#include "scheduler.h"
#include "bsp_key.h"
#include "bsp_lcd.h"
//myDrivers
#include "myGpio.h"
#include "myButton.h"
#include "myLcd.h"
#include "myAdc.h"
#include "myRtc.h"


void loop(void);
#endif //INC_002_G_BSP_SYSTEM_H
