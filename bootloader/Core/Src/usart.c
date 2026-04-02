/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* UART IAP 相关变量 */
UART_DoubleBuffTypeDef uart_double_buff = {0};
volatile uint8_t uart_recv_done = 0;
volatile uint8_t uart_error_flag = 0;
volatile uint32_t last_rec_time = 0;


/* 重定向printf到串口 */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 100);
    return ch;
}

/**
 * @brief 串口发送一个字节
 */
void UART_SendByte(uint8_t data)
{
    HAL_UART_Transmit(&huart1, &data, 1, 100);
}

/**
 * @brief 串口接收字节数（非阻塞查询）
 * @note 轮询方式：返回1表示有字节可读，返回0表示无
 */
uint8_t UART_Available(void)
{
    uint32_t temcnt = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE);
    return (temcnt != RESET) ? 1 : 0;
}

/**
 * @brief 串口读取一个字节（阻塞）
 */
uint8_t UART_ReadByte(void)
{
    uint8_t data;
    HAL_UART_Receive(&huart1, &data, 1, 100);
    return data;
}

/**
 * @brief UART错误回调
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART1) return;
    uart_error_flag = 1;
    __HAL_UART_CLEAR_OREFLAG(huart);
    __HAL_UART_CLEAR_FEFLAG(huart);
    __HAL_UART_CLEAR_NEFLAG(huart);
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    HAL_UARTEx_ReceiveToIdle_IT(huart, uart_double_buff.buff1, BUFF_SIZE);
}

/**
 * @brief UART接收事件回调（空闲中断）- 双缓冲
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance != USART1) return;

    /* 如果上一帧还没被读取，丢弃新数据 */
    if (uart_double_buff.rx_len != 0) {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UARTEx_ReceiveToIdle_IT(huart, uart_double_buff.rx_buff, BUFF_SIZE);
        return;
    }

    /* 保存数据 */
    uart_double_buff.rx_len = Size;
    last_rec_time = HAL_GetTick();

    /* 切换到另一个缓冲区 */
    if (uart_double_buff.rx_buff == uart_double_buff.buff1)
        uart_double_buff.rx_buff = uart_double_buff.buff2;
    else
        uart_double_buff.rx_buff = uart_double_buff.buff1;

    __HAL_UART_CLEAR_IDLEFLAG(huart);
    HAL_UARTEx_ReceiveToIdle_IT(huart, uart_double_buff.rx_buff, BUFF_SIZE);
}

/**
 * @brief 获取接收到的数据
 * @retval 1:有数据 0:无数据
 */
uint8_t uart_get_data(uint8_t **buff, uint16_t *len)
{
    if (uart_double_buff.rx_len == 0) return 0;

    /* 返回当前已接收数据的缓冲区 */
    if (uart_double_buff.rx_buff == uart_double_buff.buff1)
        *buff = uart_double_buff.buff2;
    else
        *buff = uart_double_buff.buff1;

    *len = uart_double_buff.rx_len;
    uart_double_buff.rx_len = 0;

    return 1;
}

/* USER CODE END 1 */
