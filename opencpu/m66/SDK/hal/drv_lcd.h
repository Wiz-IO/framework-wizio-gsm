////////////////////////////////////////////////////////////////////////////////////////
//
//      2021 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  Basic driver ( 16 bit Color RGB565 ) for ILIxxxx & STxxxx LCD displays
//  Tested with ST7789 & ILI9341
//
//       Basic mode: RGB565 [ 320 x 240 ] 13 frames per sec
//    Extended mode: RGB565 [ 320 x 240 ] 45 frames per sec
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_LCD_H_
#define _DRV_LCD_H_
#ifdef __cplusplus
extern "C"
{
#endif
  /* clang-format off */

#include <lcd_config.h>
#include <hal_lcd.h>

#define CASET                   0x2A
#define RASET                   0x2B
#define RAMWR                   0x2C

#define LCD_SET_TRANSFER_8()    LCDIF_SIF_CON = 0
#define LCD_SET_TRANSFER_16()   LCDIF_SIF_CON = 2

void lcd_command_write(uint8_t command);
void lcd_data_write(uint8_t data);
void lcd_init(const uint8_t *settings);
void lcd_reset(const uint8_t *settings);
void lcd_command(uint8_t cmd, uint8_t data_len, ...);

//////////////////////////////////////////////////////////////////////////////////////////////
/* BASIC */

void lcd_block_write_slow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
void lcd_block_write(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);

void lcd_fill_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
static inline void lcd_fill(uint16_t color)
{
    lcd_fill_rect(0, 0, LCD_X_RESOLUTION - 1, LCD_Y_RESOLUTION - 1, color);
}

void lcd_draw_image_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *image);
static inline void lcd_draw_image(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *image)
{
    lcd_draw_image_rect(x, y, x + w - 1, y + h - 1, image);
}

void lcd_draw_pixel(int16_t x, int16_t y, uint16_t color);

static inline void lcd_drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  lcd_fill_rect(x, y, x, y + h - 1, color);
}

static inline void lcd_drawtHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  lcd_fill_rect(x, y, x + w - 1, y, color);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/* EXTENDED */

void LCD_EX_INIT(void);

void lcd_ex_fill_rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
static inline void lcd_ex_fill(uint16_t color)
{
    lcd_ex_fill_rect(0, 0, LCD_X_RESOLUTION - 1, LCD_Y_RESOLUTION - 1, color);
}

void lcd_ex_draw_image_rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *image);
static inline void lcd_ex_draw_image(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *image)
{
    lcd_ex_draw_image_rect(x, y, x + w - 1, y + h - 1, image);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/* INTERFACE */

#define LCD_WRITE_COMMAND       lcd_command_write
#define LCD_WRITE_DATA          lcd_data_write
#define LCD_INIT                lcd_init
#define LCD_RESET               lcd_reset
#define LCD_COMMAND             lcd_command

#ifdef LCD_BASIC

#define LCD_BLOCK_WRITE         lcd_block_write
#define LCD_FILL_RECT           lcd_fill_rect
#define LCD_FILL                lcd_fill
#define LCD_DRAW_IMAGE_RECT     lcd_draw_image_rect
#define LCD_DRAW_IMAGE          lcd_draw_image
#define LCD_DRAW_PIXEL          lcd_draw_pixel
#define LCD_DRAW_VLINE          lcd_drawVLine
#define LCD_DRAW_HLINE          lcd_drawtHLine

#else

#define LCD_BLOCK_WRITE         lcd_ex_block_write
#define LCD_FILL_RECT           lcd_ex_fill_rect
#define LCD_FILL                lcd_ex_fill
#define LCD_DRAW_IMAGE_RECT     lcd_ex_draw_image_rect
#define LCD_DRAW_IMAGE          lcd_ex_draw_image
#define LCD_DRAW_PIXEL          lcd_ex_draw_pixel
#define LCD_DRAW_VLINE          lcd_ex_drawVLine
#define LCD_DRAW_HLINE          lcd_ex_drawtHLine

#endif



//////////////////////////////////////////////////////////////////////////////////////////////

/* clang-format on */
#ifdef __cplusplus
}
#endif
#endif /* _DRV_LCD_H_ */