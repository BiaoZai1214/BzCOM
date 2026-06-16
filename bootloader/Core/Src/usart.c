/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   UART 双缓冲接收 —— 指针交换式
  *
  * ISR 写 rx_buff，应用读 data_buff。
  * 空闲回调交换两个指针，ISR 立即重启到新的 rx_buff。
  * 双方永远操作不同缓冲区，无需 memcpy，无需互斥。
  ******************************************************************************
  */
/* USER CODE END Header */
#include "usart.h"

UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void)
{
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
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
}

/* USER CODE BEGIN 1 */

UART_RxBuffTypeDef uart_rx_buff = {0};
volatile uint8_t  uart_recv_done  = 0;
volatile uint8_t  uart_error_flag = 0;
volatile uint32_t last_rec_time   = 0;

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 100);
    return ch;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART1) return;
    uart_error_flag = 1;
    __HAL_UART_CLEAR_OREFLAG(huart);
    __HAL_UART_CLEAR_FEFLAG(huart);
    __HAL_UART_CLEAR_NEFLAG(huart);
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    HAL_UARTEx_ReceiveToIdle_IT(huart, uart_rx_buff.rx_buff, BUFF_SIZE);
}

/**
 * @brief 空闲回调 —— 交换 rx_buff ↔ data_buff，立即重启接收
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance != USART1) return;

    {
        uint8_t *tmp = uart_rx_buff.rx_buff;
        uart_rx_buff.rx_buff  = uart_rx_buff.data_buff;
        uart_rx_buff.data_buff = tmp;
    }

    uart_rx_buff.rx_len = Size;
    last_rec_time = HAL_GetTick();

    __HAL_UART_CLEAR_IDLEFLAG(huart);
    HAL_UARTEx_ReceiveToIdle_IT(huart, uart_rx_buff.rx_buff, BUFF_SIZE);
}

/**
 * @brief 获取已就绪的数据，永远返回 data_buff
 */
uint8_t uart_get_data(uint8_t **buff, uint16_t *len)
{
    if (uart_rx_buff.rx_len == 0) return 0;
    *buff = uart_rx_buff.data_buff;
    *len  = uart_rx_buff.rx_len;
    uart_rx_buff.rx_len = 0;
    return 1;
}

/* USER CODE END 1 */
