/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Motor_debugMod 0
#define MPU_debugMod 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
#define USARTBUFFER 30
uint8_t uartBuffer[USARTBUFFER];

float xyControl[2];

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
__weak void vApplicationTickHook(void) {
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int ScanOutsideFlag, ScanA4Flag;
void ServoControlTask(void const *argument) {
    float servo_pitch_output = 0, servo_yaw_output = 0;
    static float last_servo_pitch_output = 0, last_servo_yaw_output = 0;

    SG90_init();
    setAngle_270(&Servo_yaw, 0);
    setAngle_180(&Servo_pitch, 0);

    osDelay(100);
//
//    setAngle_270(&Servo_yaw, 130);
//    setAngle_180(&Servo_pitch, 130);
//
//    osDelay(100);
//
//    setAngle_270(&Servo_yaw, 0);
//    setAngle_180(&Servo_pitch, 0);
//
//    osDelay(100);
    SG90_scan();
    float t;

    for (;;) {

        if(ScanOutsideFlag == 1){
            SG90_scan();
        }
        ScanOutsideFlag = 0;

        if(ScanA4Flag == 1){
            servo_yaw.Kp = 0.08f;
            servo_yaw.Kd = 1.5f;
            servo_pitch.Kp = -0.11f;
            servo_pitch.Kd = -0.9f;
            servo_pitch.Actual = xyControl[1] / 10;
            servo_yaw.Actual = xyControl[0] / 10;

            servo_pitch.Error = Low_Pass_Filter(&lpf_pitch_error,servo_pitch.Error,0.7f);
            servo_yaw.Error = Low_Pass_Filter(&lpf_yaw_error,servo_yaw.Error,0.7f);

            servo_pitch.Error = Window_Slide_Filter(&slide_pitch_error,servo_pitch.Error);
            servo_yaw.Error = Window_Slide_Filter(&slide_yaw_error,servo_yaw.Error);

            servo_pitch_output = Position_Pid_Calculate(&servo_pitch);
            servo_yaw_output = Position_Pid_Calculate(&servo_yaw);

            servo_pitch_output = Low_Pass_Filter(&lpf_pitch_out,servo_pitch_output,0.7f);
            servo_yaw_output = Low_Pass_Filter(&lpf_yaw_out,servo_yaw_output,0.7f);

            servo_pitch_output = Window_Slide_Filter(&slide_pitch_out,servo_pitch_output);
            servo_yaw_output = Window_Slide_Filter(&slide_yaw_out,servo_yaw_output);

            servo_pitch_output += last_servo_pitch_output;
            servo_yaw_output += last_servo_yaw_output;

            setAngle_180(&Servo_pitch, servo_pitch_output);
            setAngle_270(&Servo_yaw, servo_yaw_output);
            last_servo_pitch_output = servo_pitch_output;
            last_servo_yaw_output = servo_yaw_output;
        }

        osDelay(10);
    }
}

//Global variable, use as Mode Choose flag.
int gMode = NORMALMODE;
void ButtonTask(void const *argument) {
    button_init(&BUTTON_RST, read_KEY1_GPIO, 0);
    button_init(&BUTTON_START, read_KEY2_GPIO, 0);
    button_init(&BUTTON_PAUSE, read_KEY3_GPIO, 0);
    button_attach(&BUTTON_RST, PRESS_DOWN, KEY1_PRESS_DOWN_Handler);
    button_attach(&BUTTON_START, PRESS_DOWN, KEY2_PRESS_DOWN_Handler);
    button_attach(&BUTTON_PAUSE, PRESS_DOWN, KEY3_PRESS_DOWN_Handler);
    button_start(&BUTTON_RST);
    button_start(&BUTTON_START);
    button_start(&BUTTON_PAUSE);

    for (;;) {
        if(HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == 0) gMode = NORMALMODE;
        if(HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == 1) gMode = DEBUGMODE;
        button_ticks();
        osDelay(pdMS_TO_TICKS(5));
    }
}

void BEEPLEDTask(void const *argument) {
    for (;;) {
        for (int i = 0; i < 6; i++) {
            HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            osDelay(100);
        }
        osThreadSuspend(NULL);
    }
}

void ReceiveTask(void const *argument) {
    int i;
    int array_empty_flag1 = 1;

    for (;;) {
        HAL_UART_Receive_DMA(&huart1, &uartBuffer[0], USARTBUFFER);
        for (i = 0; i < USARTBUFFER; i++) {
            if (uartBuffer[i] != 0) {
                array_empty_flag1 = 0;
                break;
            }
        }
        if (array_empty_flag1 == 0) {
            array_empty_flag1 = 1;

            ReformatBuffer(uartBuffer, xyControl);

            memset(uartBuffer, 0, USARTBUFFER);
        }

        osDelay(5);
    }
}
/* USER CODE END Application */
