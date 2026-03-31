#include "at24c64.h"


/* ---- 读/写单个字节 ---- */

void AT24C64_W_Byte(uint16_t byte_addr, uint8_t data)
{
	// 1. 启动信号: 发送 读从设备地址
	// (1)操作句柄 (2)设备地址 (3)字节地址 (4)地址长度 (5)存放地址 (6)数据长度 (7)超时时间
	HAL_I2C_Mem_Write(&hi2c1, AT24C64_ADDR_W, byte_addr, 
					 I2C_MEMADD_SIZE_16BIT, &data, 1, 100);
}

uint8_t AT24C64_R_Byte(uint16_t byte_addr)
{
	uint8_t data;
	
	// 1. 启动信号: 发送 写从设备地址
	HAL_I2C_Mem_Read(&hi2c1, AT24C64_ADDR_R, byte_addr, 
					 I2C_MEMADD_SIZE_16BIT, &data, 1, 100);
	return data;
	
}


/* ---- 读/写多个字节 ---- */

// 写入多字节: 一次只能写一页 (0x0x -> 0x10) 
void AT24C64_W_Bytes(uint16_t byte_addr, uint8_t *data, uint16_t len)
{
    // 1. 边界检查
    if (byte_addr >= 8192 || (byte_addr + len) > 8192)
    {
        printf("boundary overflow!\r\n");
        return;
    }

    // 2. 循环跨页写入
    while (len > 0)
    {
        // 计算页内偏移 + 剩余空间
        uint8_t page_offset = byte_addr % 32;
        uint8_t page_remain = 32 - page_offset;

        // 本次能写多少
        uint16_t step = (len <= page_remain) ? len : page_remain;

        // 写
        HAL_I2C_Mem_Write(&hi2c1, AT24C64_ADDR_W, byte_addr,
                          I2C_MEMADD_SIZE_16BIT, data, step, 1000);

        // 必须等待写完成
        HAL_Delay(5);

        // 更新指针
        byte_addr += step;
        data += step;
        len -= step;
    }
}

// 读多字节：不需要分页！直接读！
void AT24C64_R_Bytes(uint16_t byte_addr, uint8_t *read_buf, uint16_t len)
{
	// 1. 边界检查
	if (byte_addr >= 8192 || (byte_addr + len) > 8192)
	{
		printf("boundary overflow!\r\n");
		return;
	}

	// 2. 直接读！不用分页！不用循环！
	HAL_I2C_Mem_Read(&hi2c1, AT24C64_ADDR_R, byte_addr,
					 I2C_MEMADD_SIZE_16BIT, read_buf, len, 1000);
}



