/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define BUFF_SIZE 256

typedef struct {
    uint8_t *rx_buff;
    uint8_t buff1[BUFF_SIZE];
    uint8_t buff2[BUFF_SIZE];
    uint16_t rx_len;
} UART_DoubleBuffTypeDef;

extern UART_DoubleBuffTypeDef uart_double_buff;
extern volatile uint8_t uart_recv_done;
extern volatile uint8_t uart_error_flag;
extern volatile uint32_t last_rec_time;
extern uint8_t uart_get_data(uint8_t **buff, uint16_t *len);
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void UART_SendByte(uint8_t data);
uint8_t UART_Available(void);
uint8_t UART_ReadByte(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

