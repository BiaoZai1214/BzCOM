#include "lcd.h"
#include "lcd_init.h"
#include "lcd_font.h"
#include "delay.h"
#include "hardSPI.h"
#include "dma.h"


/******************************************************************************
 * 功能：填充背景颜色
 ******************************************************************************/
void LCD_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    if (x0 >= x1 || y0 >= y1) return;
    uint32_t num = (uint32_t)(x1 - x0) * (y1 - y0);
    LCD_Address_Set(x0, y0, x1 - 1, y1 - 1);
    LCD_DC_HIGH;
    LCD_CS_LOW;

    const uint8_t H = color >> 8;
    const uint8_t L = color & 0xFF;

    for (uint32_t i = 0; i < num; i++) {
        while (!(SPI1->SR & SPI_SR_TXE)); SPI1->DR = H;
        while (!(SPI1->SR & SPI_SR_TXE)); SPI1->DR = L;
    }

    while (SPI1->SR & SPI_SR_BSY);
    LCD_CS_HIGH;
}

/******************************************************************************
 * 功能：设置单个像素点
 ******************************************************************************/
void LCD_SetPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_W || y >= LCD_H) return;
    LCD_Address_Set(x, y, x, y);
    LCD_CS_LOW;
    while (!(SPI1->SR & SPI_SR_TXE)); SPI1->DR = (uint8_t)(color >> 8);
    while (!(SPI1->SR & SPI_SR_TXE)); SPI1->DR = (uint8_t)color;
    while (SPI1->SR & SPI_SR_BSY);
    LCD_CS_HIGH;
}

/******************************************************************************
 * 功能：显示列优先位图（适用于波特律动生成的字模）
 * data: 字模数据（列主序，每列按 bit 从上到下）
 * w, h: 宽高（像素）
 * fg_color: 前景色
 * bg_color: 背景色；若为 COLOR_TRANSPARENT，则背景不绘制（透明）
 ******************************************************************************/
void LCD_SetBlock(int x, int y, const void *data, int w, int h, uint16_t fg_color, uint16_t bg_color)
{
    if (!data || w <= 0 || h <= 0) return;
    if (x >= LCD_W || y >= LCD_H) return;

    const uint8_t *dot_data = (const uint8_t *)data;
    // 每列所需字节数（32行=4字节，和计算结果一致）
    uint8_t bytes_per_col = (h + 7) / 8;

    // 行优先遍历：先遍历行，再遍历列
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            // 计算当前像素对应的字节索引和位索引
            uint8_t byte_idx = col * bytes_per_col + (row >> 3);  // row/8
            uint8_t bit_in_byte = row & 0x07;                     // row%8
            // 读取位值（注意：字模的位序是“高位在前/低位在前”需匹配，这里按常规“最低位对应第一行”）
            uint8_t bit_val = (dot_data[byte_idx] >> bit_in_byte) & 0x01;
            
            int px_x = x + col;
            int px_y = y + row;

            // 确保像素在LCD可视区域内
            if (px_x < LCD_W && px_y < LCD_H) {
                if (bit_val) {
                    // 位值为1，绘制前景色
                    LCD_SetPixel(px_x, px_y, fg_color);
                } else if (bg_color != COLOR_NONE) {
                    // 位值为0且不透明，绘制背景色
                    LCD_SetPixel(px_x, px_y, bg_color);
                }
            }
        }
    }
}

/******************************************************************************
 * 功能：画线（Bresenham 算法）
 ******************************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int16_t dx = x2 - x1;
    int16_t dy = y2 - y1;
    int16_t ux = (dx > 0) ? 1 : -1;
    int16_t uy = (dy > 0) ? 1 : -1;
    dx = abs(dx);
    dy = abs(dy);

    int16_t x = x1, y = y1;
    int16_t eps = 0;

    if (dx > dy) {
        for (int16_t i = 0; i <= dx; i++) {
            LCD_SetPixel(x, y, color);
            x += ux;
            eps += dy;
            if ((eps << 1) >= dx) {
                y += uy;
                eps -= dx;
            }
        }
    } else {
        for (int16_t i = 0; i <= dy; i++) {
            LCD_SetPixel(x, y, color);
            y += uy;
            eps += dx;
            if ((eps << 1) >= dy) {
                x += ux;
                eps -= dy;
            }
        }
    }
}

/******************************************************************************
 * 功能：画矩形（空心）
 ******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    if (x1 > x2) { uint16_t t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { uint16_t t = y1; y1 = y2; y2 = t; }
    LCD_DrawLine(x1, y1, x2, y1, color); // top
    LCD_DrawLine(x1, y2, x2, y2, color); // bottom
    LCD_DrawLine(x1, y1, x1, y2, color); // left
    LCD_DrawLine(x2, y1, x2, y2, color); // right
}

/******************************************************************************
 * 功能：画圆（中点圆算法）
 ******************************************************************************/
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    if (r == 0) return;
    int16_t x = 0, y = r;
    int16_t d = 3 - 2 * r;

    while (x <= y) {
        LCD_SetPixel(x0 + x, y0 + y, color);
        LCD_SetPixel(x0 - x, y0 + y, color);
        LCD_SetPixel(x0 + x, y0 - y, color);
        LCD_SetPixel(x0 - x, y0 - y, color);
        LCD_SetPixel(x0 + y, y0 + x, color);
        LCD_SetPixel(x0 - y, y0 + x, color);
        LCD_SetPixel(x0 + y, y0 - x, color);
        LCD_SetPixel(x0 - y, y0 - x, color);

        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/******************************************************************************
 * UTF-8 字符长度判断
 ******************************************************************************/
uint8_t LCD_GetUTF8Len(char *string)
{
    if ((string[0] & 0x80) == 0x00) return 1;
    if ((string[0] & 0xE0) == 0xC0) return 2;
    if ((string[0] & 0xF0) == 0xE0) return 3;
    if ((string[0] & 0xF8) == 0xF0) return 4;
    return 1; // fallback
}

/**
 * @brief 绘制一个ASCII字符（逐列、LSB=top）
 */
void LCD_PrintASCIIChar(uint8_t x, uint8_t y, char ch, const ASCIIFont *font, uint16_t color)
{
    if (!font || ch < ' ' || ch > '~') return;

	// 取font 宽高数据
    uint8_t h = font->h; // 16
    uint8_t w = font->w; // 24

    uint8_t bytes_per_col = (h + 7) / 8; // 2 for 16px height
    uint16_t bytes_per_char = bytes_per_col * w; // 48

    uint16_t index = ch - ' ';
    const uint8_t *data = font->chars + index * bytes_per_char;

    for (int col = 0; col < w; col++) {
        for (int row = 0; row < h; row++) {
            uint8_t byte_idx = col * bytes_per_col + (row >> 3); // row/8
            uint8_t bit_in_byte = row & 0x07;                   // row%8

            // LSB = top row → bit0 = row0
            if ((data[byte_idx] >> bit_in_byte) & 1) {
                if ((x + col) < LCD_W && (y + row) < LCD_H) {
                    LCD_SetPixel(x + col, y + row, color);
                }
            }
        }
    }
}
/**
 * @brief 绘制一个ASCII字符串
 */
void LCD_PrintASCIIString(uint8_t x, uint8_t y, char *str, const ASCIIFont *font, uint16_t color)
{
    uint8_t x0 = x;
    while (*str) {
        LCD_PrintASCIIChar(x0, y, *str, font, color);
        x0 += font->w;
        str++;
    }
}

/**
 * @brief 绘制UTF-8字符串（支持中英文混合）
 */
void LCD_PrintString(uint8_t x, uint8_t y, char *str, const Font *font, uint16_t color)
{
    if (!str || !font) return;

    uint16_t i = 0;
    // 注意：波特律动字模头4字节是UTF-8字符本身
    uint8_t bytes_per_glyph = ((font->h + 7) / 8) * font->w + 4; // +4 是 UTF-8 头

    while (str[i]) {
        uint8_t utf8Len = LCD_GetUTF8Len(str + i);
        if (utf8Len == 0 || utf8Len > 4) break;

        uint8_t found = 0;
        // 遍历字体库查找匹配的UTF-8字符
        for (uint16_t j = 0; j < font->len; j++) {
            uint8_t *glyph_head = (uint8_t*)(font->chars) + j * bytes_per_glyph;
            if (memcmp(str + i, glyph_head, utf8Len) == 0) {
                // 字模数据在头4字节之后
                LCD_SetBlock(x, y, glyph_head + 4, font->w, font->h, color, COLOR_NONE);
                x += font->w;
                i += utf8Len;
                found = 1;
                break;
            }
        }

        if (!found) {
            if (utf8Len == 1) {
                // 尝试用 ASCII 子字体
                if (font->ascii) {
                    LCD_PrintASCIIChar(x, y, str[i], font->ascii, color);
                    x += font->ascii->w;
                } else {
                    LCD_PrintASCIIChar(x, y, '?', NULL, color); // 或跳过
                    x += 8; // 默认宽度
                }
                i++;
            } else {
                // 非ASCII且未找到，显示空格或问号
                if (font->ascii) {
                    LCD_PrintASCIIChar(x, y, '?', font->ascii, color);
                    x += font->ascii->w;
                } else {
                    x += font->w;
                }
                i += utf8Len;
            }
        }
    }
}



void LCD_DrawImage(uint16_t x, uint16_t y, const Image *img)
{
    if (!img || !img->data) return;

    uint16_t w = img->w;
    uint16_t h = img->h;
    const uint8_t *data = img->data;

    // 设置显示窗口
    LCD_Address_Set(x, y, x + w - 1, y + h - 1);

    // 计算总像素数
    uint32_t pixel_count = (uint32_t)w * h;

    // 逐个像素写入（每个像素2字节，组合为uint16_t）
    for (uint32_t i = 0; i < pixel_count; i++) {
        uint16_t color = ((uint16_t)data[i * 2] << 8) | data[i * 2 + 1];
        LCD_WR_DOUBLE(color);
    }
}
