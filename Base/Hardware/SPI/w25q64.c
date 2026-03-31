#include "w25q64.h"

void W25Q64_Init(void)
{
    SoftSPI_Init();
}

// 读取ID
void W25Q64_ReadID(uint8_t * mid, uint16_t * did)
{
    SoftSPI_Start();
    
    // 1. 发送指令
    SoftSPI_SwapByte(W25Q64_JEDEC_ID);
    
    // 2. 获取制造商ID(随便发个数据,取返回值)
    * mid = SoftSPI_SwapByte(0xff);
    
    // 3. 获取设备ID(高8位 + 低8位)
    * did = 0;
    * did |= SoftSPI_SwapByte(0xff) << 8;
    * did |= SoftSPI_SwapByte(0xff);
    
    SoftSPI_Stop();
}

// 开启写使能
void W25Q64_WriteEnable(void)
{
    SoftSPI_Start();
    SoftSPI_SwapByte(W25Q64_WRITE_ENABLE);
    SoftSPI_Stop();
}

// 关闭写使能
void W25Q64_WriteDisable(void)
{
    SoftSPI_Start();
    SoftSPI_SwapByte(W25Q64_WRITE_DISABLE);
    SoftSPI_Stop();
}

// 等待->非busy
void W25Q64_WaitNotBusy(void)
{
    SoftSPI_Start();
    SoftSPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
    // 等待收到的数据末位(busy位)变成0
    while(SoftSPI_SwapByte(0xff) & 0x01);
    SoftSPI_Stop();
}

// 段擦除(sector erase),地址只需要 块号 & 段号:定位到页
void W25Q64_EraseSector(uint8_t block, uint8_t sector)
{
    
    W25Q64_WaitNotBusy();
    W25Q64_WriteEnable();
    
    // 计算要发送的地址(段首地址)
    uint32_t addr = (block << 16) | (sector << 12);
    
    SoftSPI_Start();
    
    SoftSPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
    
    //地址用24个字节表示 高8位:块地址 中8位:段地址 低16位:页地址
    SoftSPI_SwapByte((addr >> 16) & 0xff);      // 第一个字节: 块号
    SoftSPI_SwapByte((addr >> 8 ) & 0xff);      // 第二个字节: 段号
    SoftSPI_SwapByte((addr >> 0 ) & 0xff);      // 第三个字节: 页号
    
    SoftSPI_Stop();
    W25Q64_WriteDisable();
}

// 页写入
void W25Q64_PageWrite(uint8_t block, uint8_t sector, uint8_t page, uint8_t * data, uint16_t len)
{
    W25Q64_WaitNotBusy();
    W25Q64_WriteEnable();
    
    // 计算要发送的地址(页首地址)
    uint32_t addr = (block << 16) | (sector << 12) | (page << 8);
    
    SoftSPI_Start();

    SoftSPI_SwapByte(W25Q64_PAGE_PROGRAM);
    
    // 发送24位地址
    SoftSPI_SwapByte((addr >> 16) & 0xff);      // 第一个字节: 块号
    SoftSPI_SwapByte((addr >> 8 ) & 0xff);      // 第二个字节: 段号
    SoftSPI_SwapByte((addr >> 0 ) & 0xff);      // 第三个字节: 页号
    
    // 依次发送数据
    for (uint16_t i = 0; i < len; i++)
    {
        SoftSPI_SwapByte(data[i]);
    }
    
    SoftSPI_Stop();
    W25Q64_WriteDisable();
}

// 读取
void W25Q64_Read(uint8_t block, uint8_t sector,uint8_t page, uint8_t innerAddr, uint8_t * buffer, uint16_t len)
{
    W25Q64_WaitNotBusy();
    
    // 计算要发送的地址(页内地址)
    uint32_t addr = (block << 16) | (sector << 12) | (page << 8) | innerAddr;
    
    SoftSPI_Start();
  
    SoftSPI_SwapByte(W25Q64_READ_DATA);
        
    // 发送24位地址
    SoftSPI_SwapByte((addr >> 16) & 0xff);      // 第一个字节: 块号
    SoftSPI_SwapByte((addr >> 8 ) & 0xff);      // 第二个字节: 段号
    SoftSPI_SwapByte((addr >> 0 ) & 0xff);      // 第三个字节: 页号
    
    // 依次读取数据
    for (uint16_t i = 0; i < len; i++)
    {
        buffer[i] = SoftSPI_SwapByte(0xff);
    }
    
    SoftSPI_Stop();
}

  // 页写入
void W25Q64_RandomWrite(uint32_t addr, uint8_t * data, uint16_t len)
{
    W25Q64_WaitNotBusy();
    W25Q64_WriteEnable();
    
    SoftSPI_Start();

    SoftSPI_SwapByte(W25Q64_PAGE_PROGRAM);
    
    // 发送24位地址
    SoftSPI_SwapByte((addr >> 16) & 0xff);      // 第一个字节: 块号
    SoftSPI_SwapByte((addr >> 8 ) & 0xff);      // 第二个字节: 段号
    SoftSPI_SwapByte((addr >> 0 ) & 0xff);      // 第三个字节: 页号
    
    // 依次发送数据
    for (uint16_t i = 0; i < len; i++)
    {
        SoftSPI_SwapByte(data[i]);
    }
    
    SoftSPI_Stop();
    W25Q64_WriteDisable();
}


// 任意地址读取
void W25Q64_RandomRead(uint32_t addr, uint8_t * buffer, uint16_t len)
{
    W25Q64_WaitNotBusy();
    
    SoftSPI_Start();
  
    SoftSPI_SwapByte(W25Q64_READ_DATA);
        
    // 发送24位地址
    SoftSPI_SwapByte((addr >> 16) & 0xff);      // 第一个字节: 块号
    SoftSPI_SwapByte((addr >> 8 ) & 0xff);      // 第二个字节: 段号
    SoftSPI_SwapByte((addr >> 0 ) & 0xff);      // 第三个字节: 页号
    
    // 依次读取数据
    for (uint16_t i = 0; i < len; i++)
    {
        buffer[i] = SoftSPI_SwapByte(0xff);
    }
    
    SoftSPI_Stop();
}
