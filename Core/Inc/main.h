/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "lcd_i2c.h"
#include "ds3231.h"
#include "menu.h"
#include "button.h"
#include "schedule.h"
#include "flash.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BT_BK_Pin GPIO_PIN_2
#define BT_BK_GPIO_Port GPIOA
#define BUZZ_Pin GPIO_PIN_10
#define BUZZ_GPIO_Port GPIOB
#define TYPEB_Pin GPIO_PIN_12
#define TYPEB_GPIO_Port GPIOB
#define TYPEA_Pin GPIO_PIN_13
#define TYPEA_GPIO_Port GPIOB
#define TYPEA_CNT_INTR_Pin GPIO_PIN_14
#define TYPEA_CNT_INTR_GPIO_Port GPIOB
#define TYPEA_CNT_INTR_EXTI_IRQn EXTI15_10_IRQn
#define TYPEB_CNT_INTR_Pin GPIO_PIN_15
#define TYPEB_CNT_INTR_GPIO_Port GPIOB
#define TYPEB_CNT_INTR_EXTI_IRQn EXTI15_10_IRQn
#define BT_SL_Pin GPIO_PIN_10
#define BT_SL_GPIO_Port GPIOD
#define BT_DN_Pin GPIO_PIN_11
#define BT_DN_GPIO_Port GPIOD
#define BT_UP_Pin GPIO_PIN_12
#define BT_UP_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define TIME_UNIT	5
#define RTC_EMUL	0
#define BTN_EMUL	1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
