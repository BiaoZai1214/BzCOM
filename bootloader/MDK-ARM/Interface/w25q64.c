#include "w25q64.h"

/**
 * @brief 初始化W25Q64
 * @note SPI1和GPIO已由CubeMX初始化，此处只需取消片选(拉高CS)
 */
void W25Q64_Init(void)
{
    W25Q64_CS_HIGH;  /* 片选无效，芯片处于待机状态 */
}

/**
  * @brief  SPI 全双工读写 1 字节
  */
static uint8_t W25Q64_SwapByte(uint8_t data)
{
	uint8_t rx_data;
	HAL_SPI_TransmitReceive(&hspi1, &data, &rx_data, 1, 100);
	return rx_data;
}

/**
  * @brief  读取 ID
  */
void W25Q64_ReadID(uint8_t *mid, uint16_t *did)
{
	W25Q64_CS_LOW;
	
	W25Q64_SwapByte(W25Q64_JEDEC_ID);
	
	*mid  = W25Q64_SwapByte(0xFF);
	*did  = W25Q64_SwapByte(0xFF) << 8;
	*did |= W25Q64_SwapByte(0xFF);
	
	W25Q64_CS_HIGH;
}

/**
  * @brief  等待不忙
  */
void W25Q64_WaitNotBusy(void)
{
	W25Q64_CS_LOW;
    W25Q64_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
    // 等待收到的数据末位(busy位)
    while(W25Q64_SwapByte(0xff) & 0x01);
    W25Q64_CS_HIGH;
}

/**
  * @brief  写使/失能
  */
void W25Q64_WriteEnable(void)
{
	W25Q64_CS_LOW;
	W25Q64_SwapByte(W25Q64_WRITE_ENABLE);
	W25Q64_CS_HIGH;
}

void W25Q64_WriteDisable(void)
{
	W25Q64_CS_LOW;
	W25Q64_SwapByte(W25Q64_WRITE_DISABLE);
	W25Q64_CS_HIGH;
}

/**
  * @brief  扇区擦除 4KB
  */
void W25Q64_EraseSector(uint32_t addr)
{
	addr &= 0x00FFFFFF;
	
	W25Q64_WaitNotBusy();
	W25Q64_WriteEnable();
	
	W25Q64_CS_LOW;
	W25Q64_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	
	// 24位地址格式：高8位(bit23-16) → 中8位(bit15-8) → 低8位(bit7-0)
	W25Q64_SwapByte((addr >> 16) & 0xFF);	 // 高8位：块号
	W25Q64_SwapByte((addr >> 8)  & 0xFF);    // 中8位：段号
	W25Q64_SwapByte(addr & 0xFF);            // 低8位：页号 + 页内地址
	
	W25Q64_CS_HIGH;
	W25Q64_WaitNotBusy();
}

/**
  * @brief  页写入（最大 256 字节）
  */
void W25Q64_WritePage(uint32_t addr, uint8_t *data, uint16_t len)
{
	if(len > W25Q64_PAGE_SIZE) len = W25Q64_PAGE_SIZE;
	
	W25Q64_WaitNotBusy();
	W25Q64_WriteEnable();
	
	W25Q64_CS_LOW;
	W25Q64_SwapByte(W25Q64_PAGE_PROGRAM);

	W25Q64_SwapByte((addr >> 16) & 0xFF);
	W25Q64_SwapByte((addr >> 8)  & 0xFF);
	W25Q64_SwapByte(addr & 0xFF);
	
	for(uint16_t i=0; i<len; i++)
	{
		W25Q64_SwapByte(data[i]);
	}
	
	W25Q64_CS_HIGH;
	W25Q64_WaitNotBusy();
}

/**
  * @brief  任意地址写入（自动跨页）
  */
void W25Q64_WriteData(uint32_t addr, uint8_t *data, uint16_t len)
{
	uint16_t page_remain;
	page_remain = W25Q64_PAGE_SIZE - (addr % W25Q64_PAGE_SIZE);
	
	if(len <= page_remain) page_remain = len;
	
	while(1)
	{
		W25Q64_WritePage(addr, data, page_remain);
		
		if(len == page_remain) break;
		
		data += page_remain;
		addr += page_remain;
		len  -= page_remain;
		
		if(len > W25Q64_PAGE_SIZE) page_remain = W25Q64_PAGE_SIZE;
		else page_remain = len;
	}
}

/**
  * @brief  任意地址读取
  */
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint16_t len)
{
	W25Q64_WaitNotBusy();
	
	W25Q64_CS_LOW;
	W25Q64_SwapByte(W25Q64_READ_DATA);

	W25Q64_SwapByte((addr >> 16) & 0xFF);
	W25Q64_SwapByte((addr >> 8)  & 0xFF);
	W25Q64_SwapByte(addr & 0xFF);
	
	for(uint16_t i=0; i<len; i++)
	{
		data[i] = W25Q64_SwapByte(0xFF);
	}
	
	W25Q64_CS_HIGH;
}
