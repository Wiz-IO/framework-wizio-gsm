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
//       Basic mode: RGB565 [ 240/320 x 240 ] 20/13 frames per sec
//    Extended mode: RGB565 [ 240/320 x 240 ] 55/45 frames per sec
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_LCD_H_
#define _DRV_LCD_H_
#ifdef __cplusplus
extern "C"
{
#endif
  /* clang-format off */

#if defined(ILI9341) || defined(ST7789) 

#include <lcd_config.h>

#else

    #define LCD_X_RESOLUTION 0
    #define LCD_Y_RESOLUTION 0
    #define LCD_ARGUMENTS 0
    
#endif

#include <hal_lcd.h>

// RGB565 basic colors 
#define	BLACK                   0x0000
#define	BLUE                    0x001F
#define	RED                     0xF800
#define	GREEN                   0x07E0
#define CYAN                    0x07FF
#define MAGENTA                 0xF81F
#define YELLOW                  0xFFE0
#define WHITE                   0xFFFF

// LCD basic commands 
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
/* BASIC MODE */

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

//////////////////////////////////////////////////////////////////////////////////////////////
/* EXTENDED MODE */

void LCD_EX_INIT();
void LCD_RUN(void);

void lcd_ex_block_write(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);

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

#define LCD_INIT                lcd_init
#define LCD_RESET               lcd_reset
#define LCD_WRITE_COMMAND       lcd_command_write
#define LCD_WRITE_DATA          lcd_data_write
#define LCD_COMMAND             lcd_command

#ifdef LCD_BASIC

#define LCD_BLOCK_WRITE         lcd_block_write
#define LCD_FILL_RECT           lcd_fill_rect
#define LCD_FILL                lcd_fill
#define LCD_DRAW_IMAGE_RECT     lcd_draw_image_rect
#define LCD_DRAW_IMAGE          lcd_draw_image

#else

#define LCD_BLOCK_WRITE         lcd_ex_block_write
#define LCD_FILL_RECT           lcd_ex_fill_rect
#define LCD_FILL                lcd_ex_fill
#define LCD_DRAW_IMAGE_RECT     lcd_ex_draw_image_rect
#define LCD_DRAW_IMAGE          lcd_ex_draw_image

#endif

//////////////////////////////////////////////////////////////////////////////////////////////

static inline void LCD_DRAW_PIXEL(int16_t x, int16_t y, uint16_t color)
{
  LCD_FILL_RECT(x, y, x + 1, y + 1, color);
}

static inline void LCD_DRAW_VLINE(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  LCD_FILL_RECT(x, y, x, y + h - 1, color);
}

static inline void LCD_DRAW_HLINE(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  LCD_FILL_RECT(x, y, x + w - 1, y, color);
}

//////////////////////////////////////////////////////////////////////////////////////////////

/* clang-format on */
#ifdef __cplusplus
}
#endif
#endif /* _DRV_LCD_H_ */