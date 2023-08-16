/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "retarget.h"
#include "cmsis_os.h"
#include "Filter.h"
#include "PID.h"
#include "ADRC.h"
#include "my_PID.h"
#include "Motor.h"
#include "SG90.h"
#include "bluetooth.h"
#include "mpu6050.h"
#include "multi_button.h"
//#include "lcd.h"
//#include "lcd_init.h"
//#include "GT911.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern osMutexId printfMutex;
extern osMutexId SPI1Mutex;
extern SD_HandleTypeDef hsd1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim15;

typedef struct MPU_POSE{
    float pitch;
    float roll;
    float yaw;
}Pose;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Servo1_Pin GPIO_PIN_5
#define Servo1_GPIO_Port GPIOE
#define SD_cheater_Pin GPIO_PIN_0
#define SD_cheater_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define Servo2_Pin GPIO_PIN_2
#define Servo2_GPIO_Port GPIOA
#define AIN1_Pin GPIO_PIN_4
#define AIN1_GPIO_Port GPIOC
#define AIN2_Pin GPIO_PIN_5
#define AIN2_GPIO_Port GPIOC
#define BIN1_Pin GPIO_PIN_7
#define BIN1_GPIO_Port GPIOE
#define BIN2_Pin GPIO_PIN_8
#define BIN2_GPIO_Port GPIOE
#define VL53_SCL_Pin GPIO_PIN_10
#define VL53_SCL_GPIO_Port GPIOB
#define VL53_SDA_Pin GPIO_PIN_11
#define VL53_SDA_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOD
#define BEEP_Pin GPIO_PIN_4
#define BEEP_GPIO_Port GPIOD
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOD
#define KEY3_Pin GPIO_PIN_3
#define KEY3_GPIO_Port GPIOB
#define KEY5_Pin GPIO_PIN_5
#define KEY5_GPIO_Port GPIOB
#define IIC_SCL_Pin GPIO_PIN_6
#define IIC_SCL_GPIO_Port GPIOB
#define IIC_SDA_Pin GPIO_PIN_7
#define IIC_SDA_GPIO_Port GPIOB
#define KEY4_Pin GPIO_PIN_1
#define KEY4_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
extern QueueHandle_t SampleQueueHandle;
extern QueueHandle_t SpeedQueueHandle;

#define NORMALMODE 0
#define DEBUGMODE 1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
