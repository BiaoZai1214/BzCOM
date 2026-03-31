#ifndef __W25Q64_H
#define __W25Q64_H

#include "spi.h"
#include "w25q64_ins.h"

/* W25Q64 容量参数 */
#define W25Q64_PAGE_SIZE      256     // 一页 256 字节
#define W25Q64_SECTOR_SIZE    4096    // 一扇区 4KB
#define W25Q64_BLOCK_SIZE     65536   // 一块 64KB

/* W25Q64 片选引脚 */
#define W25Q64_CS_HIGH   HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port, W25Q64_CS_Pin, GPIO_PIN_SET)
#define W25Q64_CS_LOW    HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port, W25Q64_CS_Pin, GPIO_PIN_RESET)

/* 函数声明 */
void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *mid, uint16_t *did);

void W25Q64_WaitNotBusy(void);
void W25Q64_WriteEnable(void);
void W25Q64_WriteDisable(void);

void W25Q64_EraseSector(uint32_t addr);        // 扇区擦除(4KB)
//void W25Q64_EraseBlock(uint32_t addr);         // 块擦除(64KB)
//void W25Q64_EraseChip(void);                   // 整片擦除

void W25Q64_WritePage(uint32_t addr, uint8_t *data, uint16_t len);  // 页写入
void W25Q64_WriteData(uint32_t addr, uint8_t *data, uint16_t len);  // 任意地址写入（自动跨页）
void W25Q64_ReadData (uint32_t addr, uint8_t *data, uint16_t len);   // 任意地址读取

#endif
