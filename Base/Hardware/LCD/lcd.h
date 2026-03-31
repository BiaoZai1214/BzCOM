// lcd.h
#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"                  // Device header
#include "lcd_font.h"
#include <stdlib.h>

// 颜色定义（根据你的屏调整）
#define COLOR_NONE 0xFEFE	// 透明色

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

/* ==================== 函数声明 ==================== */

/* 基础绘图 */
void LCD_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void LCD_SetPixel(uint16_t x, uint16_t y, uint16_t color);

/* 位图绘制 */
void LCD_SetBlock(int x, int y, const void *data, int w, int h, uint16_t fg_color, uint16_t bg_color);

/* 几何图形 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

/* 文本显示 */
uint8_t LCD_GetUTF8Len(char *string); // 判断 UTF-8 字符长度

// ASCII 专用
void LCD_PrintASCIIChar(uint8_t x, uint8_t y, char ch, const ASCIIFont *font, uint16_t color);
void LCD_PrintASCIIString(uint8_t x, uint8_t y, char *str, const ASCIIFont *font, uint16_t color);
void LCD_DrawImage(uint16_t x, uint16_t y, const Image *img);
// 通用 UTF-8 字符串（支持中英文混合）
void LCD_PrintString(uint8_t x, uint8_t y, char *str, const Font *font, uint16_t color);

#endif /* __LCD_H */
