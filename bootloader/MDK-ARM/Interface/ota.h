/**
 * @file ota.h
 * @brief OTA 升级接口
 * @note 超时常量已迁移到 bootloader.h (IAP_IDLE_TIMEOUT, IAP_TOTAL_TIMEOUT)
 */
#ifndef __OTA_H__
#define __OTA_H__

#include <stdint.h>

/**
 * @brief OTA 接收处理（串口 → W25Q64 → Flash 分区）
 * @note 流程: 等待 IAP_START_CHAR 启动 → 接收 bin → 存 W25Q64 → 烧录目标分区 → 校验 → 切换
 */
void OTA_Receive(void);

#endif
