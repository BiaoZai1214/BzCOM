/**
 * @file ota.h
 * @brief OTA 升级接口
 */
#ifndef __OTA_H__
#define __OTA_H__

#include <stdint.h>

/* OTA 超时配置（毫秒） */
#define OTA_IDLE_TIMEOUT_MS   2000   /* 空闲超时 2 秒 */
#define OTA_TOTAL_TIMEOUT_MS  10000  /* 总超时 10 秒 */

/**
 * @brief OTA 接收处理（串口 → W25Q64 → Flash 分区）
 * @note 流程: 等待 '1' 启动 → 接收 bin → 存 W25Q64 → 烧录目标分区 → 校验 → 切换
 */
void OTA_Receive(void);

#endif
