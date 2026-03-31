#ifndef __W25Q64_H
#define __W25Q64_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "softSPI.h"
#include "hardSPI.h"
#include "w25q64_ins.h"
/* 宏定义 */

/* 数据结构 */

/* 函数声明 */
void W25Q64_Init(void);

// 读取ID
void W25Q64_ReadID(uint8_t * mid, uint16_t * did);

// 开启写使能
void W25Q64_WriteEnable(void);

// 关闭写使能
void W25Q64_WriteDisable(void);

// 等待状态非busy
void W25Q64_WaitNotBusy(void);

// 擦除段(sector erase),地址只需要 块号 和 段号
void W25Q64_EraseSector(uint8_t block, uint8_t sector);

// 页写入
void W25Q64_PageWrite(uint8_t block, uint8_t sector, uint8_t page, uint8_t * data, uint16_t len);
// 读取
void W25Q64_Read(uint8_t block, uint8_t sector,uint8_t page, uint8_t innerAddr, uint8_t * buffer, uint16_t len); 

// 随机地址写入
void W25Q64_RandomWrite(uint32_t addr, uint8_t * data, uint16_t len);
// 随机地址读取
void W25Q64_RandomRead(uint32_t addr, uint8_t * buffer, uint16_t len);

#endif
