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
#include "stm32h7xx_hal.h"

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
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
int horizontalaxis(int i);
void hc_bz();
void pid_mv();
void motor_qd();
void uart_ever();
void detection_yw();
void detection_hw();
void detection_gm();
void yy_bz();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DATA_Pin GPIO_PIN_1
#define DATA_GPIO_Port GPIOC
#define Trig_Pin GPIO_PIN_4
#define Trig_GPIO_Port GPIOA
#define PWMA_Pin GPIO_PIN_7
#define PWMA_GPIO_Port GPIOA
#define BIN1_Pin GPIO_PIN_5
#define BIN1_GPIO_Port GPIOC
#define BIN2_Pin GPIO_PIN_0
#define BIN2_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_7
#define LCD_DC_GPIO_Port GPIOE
#define LCD_BLK_Pin GPIO_PIN_8
#define LCD_BLK_GPIO_Port GPIOE
#define PWMB_Pin GPIO_PIN_9
#define PWMB_GPIO_Port GPIOE
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define LCD_SCLK_Pin GPIO_PIN_13
#define LCD_SCLK_GPIO_Port GPIOB
#define LCD_RES_Pin GPIO_PIN_14
#define LCD_RES_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_15
#define LCD_MOSI_GPIO_Port GPIOB
#define LED_RIGHT_Pin GPIO_PIN_14
#define LED_RIGHT_GPIO_Port GPIOD
#define LED_LEFT_Pin GPIO_PIN_8
#define LED_LEFT_GPIO_Port GPIOA
#define AIN2_Pin GPIO_PIN_12
#define AIN2_GPIO_Port GPIOC
#define AIN1_Pin GPIO_PIN_0
#define AIN1_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
