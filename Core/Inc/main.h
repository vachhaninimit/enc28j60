/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __main_h
#define __main_h

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

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
#define LC_3_CLK_Pin GPIO_PIN_9
#define LC_3_CLK_GPIO_Port GPIOA

#define LC_3_DAT_Pin GPIO_PIN_10
#define LC_3_DAT_GPIO_Port GPIOA

#define LC_2_DAT_Pin GPIO_PIN_15
#define LC_2_DAT_GPIO_Port GPIOA
#define LC_2_CLK_Pin GPIO_PIN_10
#define LC_2_CLK_GPIO_Port GPIOC

#define LC_1_DAT_Pin GPIO_PIN_11
#define LC_1_DAT_GPIO_Port GPIOC
#define LC_1_CLK_Pin GPIO_PIN_12
#define LC_1_CLK_GPIO_Port GPIOC

#define ENC28J60_CS_Pin GPIO_PIN_2
#define ENC28J60_CS_GPIO_Port GPIOD
#define ENC28J60_SCK_Pin GPIO_PIN_3
#define ENC28J60_SCK_GPIO_Port GPIOB
#define ENC28J60_SO_Pin GPIO_PIN_4
#define ENC28J60_SO_GPIO_Port GPIOB
#define ENC28J60_ST_Pin GPIO_PIN_5
#define ENC28J60_ST_GPIO_Port GPIOB
#define RS_485_TX_Pin GPIO_PIN_6
#define RS_485_TX_GPIO_Port GPIOB
#define RS_485_RX_Pin GPIO_PIN_7
#define RS_485_RX_GPIO_Port GPIOB
#define RS_485_EN_Pin GPIO_PIN_8
#define RS_485_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
extern uint32_t    serial_number;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
