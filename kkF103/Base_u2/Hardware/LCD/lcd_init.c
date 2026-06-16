#include "lcd_init.h"
#include "Delay.h"
#include "hardSPI.h"

void LCD_GPIO_Init(void)
{
	hardSPI_Init();
	// 1. 使能时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    // 2. GPIO_MODE 
	
    // PB0:  RST 通用推挽输出 CNF: 00 MODE: 11
    GPIOB->CRL |=  GPIO_CRL_MODE0; 
    GPIOB->CRL &= ~GPIO_CRL_CNF0; 
	
	// PB1:  DC  通用推挽输出 CNF: 00 MODE: 11
	GPIOB->CRL |=  GPIO_CRL_MODE1; 
    GPIOB->CRL &= ~GPIO_CRL_CNF1;

	// PB10: CS  通用推挽输出 CNF: 00 MODE: 11
	GPIOB->CRH |=  GPIO_CRH_MODE10; 
    GPIOB->CRH &= ~GPIO_CRH_CNF10;
	
	// 默认拉高
	GPIOB->BSRR = GPIO_BSRR_BS0 | GPIO_BSRR_BS1 | GPIO_BSRR_BS10;
}


/******************************************************************************
      函数说明：LCD写入1字节
      入口数据：数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(uint8_t byteSend) 
{
    LCD_CS_LOW;
    
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = byteSend;
    
	while (!(SPI1->SR & SPI_SR_RXNE)); 
    (void)SPI1->DR;  // 清 RX 缓冲区
    
    while (SPI1->SR & SPI_SR_BSY); // 现在 BSY 才会正常清零
    
    LCD_CS_HIGH;
}



/******************************************************************************
      函数说明：LCD写入2字节
      入口数据：数据
      返回值：  无
******************************************************************************/
void LCD_WR_DOUBLE(uint16_t data)
{
    LCD_CS_LOW;
    
    while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = data >> 8;	// 发送高字节
    
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;	// 发送低字节
	
	SPI1->DR; // 清 RX
    while (SPI1->SR & SPI_SR_BSY);
    
    LCD_CS_HIGH;
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_CMD(uint8_t data)
{
	LCD_DC_LOW;//写命令
	LCD_WR_DATA(data);
	LCD_DC_HIGH;//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
	  入口数据: x1,x2 设置列的起始和结束地址
				y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    // 根据旋转方向调整坐标偏移
    switch (USE_HORIZONTAL) {
        case 1:
            y1 += 80;
            y2 += 80;
            break;
        case 3:
            x1 += 80;
            x2 += 80;
            break;

        default:
            break;
    }

    // 统一发送地址设置命令
    LCD_WR_CMD(0x2a); // 列地址
    LCD_WR_DOUBLE(x1);
    LCD_WR_DOUBLE(x2);

    LCD_WR_CMD(0x2b); // 行地址
    LCD_WR_DOUBLE(y1);
    LCD_WR_DOUBLE(y2);

    LCD_WR_CMD(0x2c); // 写显存
}

void LCD_Init(void)
{
	LCD_GPIO_Init();//初始化GPIO
	
	LCD_RST_LOW;//复位
	Delay_ms(10);
	LCD_RST_HIGH;
	Delay_ms(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_CMD(0x11); //Sleep out 
	Delay_ms(120);              //Delay 120ms 
	//************* Start Initial Sequence **********// 
	LCD_WR_CMD(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA(0x70);
	else LCD_WR_DATA(0xA0);

	LCD_WR_CMD(0x3A);			
	LCD_WR_DATA(0x05);

	LCD_WR_CMD(0xB2);			
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x33); 
	LCD_WR_DATA(0x33); 			

	LCD_WR_CMD(0xB7);			
	LCD_WR_DATA(0x35);

	LCD_WR_CMD(0xBB);			
	LCD_WR_DATA(0x36); //Vcom=1.35V
					
	LCD_WR_CMD(0xC2);
	LCD_WR_DATA(0x01);

	LCD_WR_CMD(0xC3);			
	LCD_WR_DATA(0x15); //GVDD=4.8V  颜色深度
				
	LCD_WR_CMD(0xC4);			
	LCD_WR_DATA(0x20); //VDV, 0x20:0v

	LCD_WR_CMD(0xC6);			
	LCD_WR_DATA(0x0F); //0x0F:60Hz       	

	LCD_WR_CMD(0xD0);			
	LCD_WR_DATA(0xA4);
	LCD_WR_DATA(0xA1); 

	LCD_WR_CMD(0xE0);
	LCD_WR_DATA(0xD0);   
	LCD_WR_DATA(0x08);   
	LCD_WR_DATA(0x0E);   
	LCD_WR_DATA(0x09);   
	LCD_WR_DATA(0x09);   
	LCD_WR_DATA(0x05);   
	LCD_WR_DATA(0x31);   
	LCD_WR_DATA(0x33);   
	LCD_WR_DATA(0x48);   
	LCD_WR_DATA(0x17);   
	LCD_WR_DATA(0x14);   
	LCD_WR_DATA(0x15);   
	LCD_WR_DATA(0x31);   
	LCD_WR_DATA(0x34);   

	LCD_WR_CMD(0xE1);     
	LCD_WR_DATA(0xD0);   
	LCD_WR_DATA(0x08);   
	LCD_WR_DATA(0x0E);   
	LCD_WR_DATA(0x09);   
	LCD_WR_DATA(0x09);   
	LCD_WR_DATA(0x15);   
	LCD_WR_DATA(0x31);   
	LCD_WR_DATA(0x33);   
	LCD_WR_DATA(0x48);   
	LCD_WR_DATA(0x17);   
	LCD_WR_DATA(0x14);   
	LCD_WR_DATA(0x15);   
	LCD_WR_DATA(0x31);   
	LCD_WR_DATA(0x34);
	LCD_WR_CMD(0x21); 

	LCD_WR_CMD(0x29);
	Delay_ms(50); 
} 
