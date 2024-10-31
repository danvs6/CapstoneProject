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
#define LCD_E_Pin GPIO_PIN_6
#define LCD_E_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_7
#define LCD_RS_GPIO_Port GPIOA
#define LCD_D4_Pin GPIO_PIN_4
#define LCD_D4_GPIO_Port GPIOC
#define LCD_D5_Pin GPIO_PIN_5
#define LCD_D5_GPIO_Port GPIOC
#define LCD_D6_Pin GPIO_PIN_0
#define LCD_D6_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_1
#define LCD_D7_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define GreenLED_Pin GPIO_PIN_8
#define GreenLED_GPIO_Port GPIOE
#define YellowLED_Pin GPIO_PIN_9
#define YellowLED_GPIO_Port GPIOE
#define S1_Pin GPIO_PIN_15
#define S1_GPIO_Port GPIOB
#define Power_Switch_Pin GPIO_PIN_8
#define Power_Switch_GPIO_Port GPIOD
#define Y0_Pin GPIO_PIN_9
#define Y0_GPIO_Port GPIOD
#define Y1_Pin GPIO_PIN_10
#define Y1_GPIO_Port GPIOD
#define Y2_Pin GPIO_PIN_11
#define Y2_GPIO_Port GPIOD
#define S0_Pin GPIO_PIN_6
#define S0_GPIO_Port GPIOC
#define S2_Pin GPIO_PIN_8
#define S2_GPIO_Port GPIOC
#define S3_Pin GPIO_PIN_9
#define S3_GPIO_Port GPIOC
#define COM_Pin GPIO_PIN_8
#define COM_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
