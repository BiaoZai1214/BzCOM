/**
 * @file iap.h
 * @brief UART IAP 升级接口
 */
#ifndef __IAP_H__
#define __IAP_H__

#include <stdint.h>

/* IAP 超时配置 */
#define IAP_IDLE_TIMEOUT   2000    /* 空闲超时 2 秒 */
#define IAP_TOTAL_TIMEOUT  10000   /* 总超时 10 秒 */

/**
 * @brief 启动 UART IAP 接收
 * @param target_addr 目标写入地址
 */
void IAP_Start(uint32_t target_addr);

/**
 * @brief 处理 UART IAP 接收（轮询调用）
 * @retval 1: 传输完成  0: 继续接收
 */
uint8_t IAP_Process(void);

/**
 * @brief 检查 IAP 是否完成
 */
uint8_t IAP_IsDone(void);

/**
 * @brief 获取已接收的总字节数
 */
uint32_t IAP_GetReceivedLen(void);

#endif
