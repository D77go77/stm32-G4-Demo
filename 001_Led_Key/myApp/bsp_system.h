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
#include "math.h"
#include "stm32g4xx_hal.h"


//myUser
#include "scheduler.h"
#include "bsp_key.h"
//myDrivers
#include "myGpio.h"
#include "myButton.h"



void loop(void);
#endif //INC_002_G_BSP_SYSTEM_H
