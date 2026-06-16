/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   UART 双缓冲接收 —— 指针交换式
  *
  * ISR 写 rx_buff，应用读 data_buff。空闲回调交换两个指针。
  * uart_get_data() 永远返回 data_buff，无需判断缓冲区归属。
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdio.h>

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define BUFF_SIZE 256

typedef struct {
    uint8_t  buff1[BUFF_SIZE];
    uint8_t  buff2[BUFF_SIZE];
    uint8_t *rx_buff;      /* ISR 写入 */
    uint8_t *data_buff;    /* 应用读取 */
    uint16_t rx_len;
} UART_RxBuffTypeDef;

extern UART_RxBuffTypeDef uart_rx_buff;
extern volatile uint8_t  uart_recv_done;
extern volatile uint8_t  uart_error_flag;
extern volatile uint32_t last_rec_time;

uint8_t uart_get_data(uint8_t **buff, uint16_t *len);
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
