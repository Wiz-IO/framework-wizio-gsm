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

#include <drv_lcd.h>
#include <stdarg.h>

#ifndef LCD_RST
#define LCD_RST GPIO27 // select gpio
#endif

#define LCD_DC GPIO29  // LCDIF LSA0DA1[29]
#define LCD_SCK GPIO26 // LCDIF LSCK1[26]
#define LCD_SDA GPIO28 // LCDIF LSDA1[28]

#pragma GCC push_options
#pragma GCC optimize("O0")
void lcd_command_write(uint8_t command)
{
    LCDIF_SCMD0 = command;
}

void lcd_data_write(uint8_t data)
{
    LCDIF_SDAT0 = data;
}
#pragma GCC pop_options

static void lcd_load_settings(const uint8_t *p)
{
    if (p)
    {
        LCD_SET_TRANSFER_8();
        uint16_t ms;
        uint8_t numArgs, numCommands = *p++;
        while (numCommands--)
        {
            LCDIF_SCMD0 = *p++;
            numArgs = *p++;
            ms = numArgs & LCD_ARGUMENTS;
            numArgs &= ~LCD_ARGUMENTS;
            while (numArgs--)
            {
                LCDIF_SDAT0 = *p++;
            }
            if (ms)
            {
                ms = *p++;
                if (ms == 255)
                    ms = 500;
                delay_m(ms);
            }
        }
    }
}

void lcd_reset(const uint8_t *settings)
{
    GPIO_DATAOUT(LCD_RST, 0);
    delay_m(200);
    GPIO_DATAOUT(LCD_RST, 1);
    lcd_load_settings(settings);
}

void lcd_init(const uint8_t *settings)
{
    LCDIF_InitInterface(LCD_CLOCK_MPLL_DIV4, 0, 0);
    GPIO_Setup(LCD_SCK, GPMODE(2));
    GPIO_Setup(LCD_SDA, GPMODE(2));
    GPIO_Setup(LCD_DC, GPMODE(2));
    GPIO_Setup(LCD_RST, GPMODE(0));
    GPIO_SETDIROUT(LCD_RST);
    lcd_reset(settings);
    lcd_fill(0);
    delay_m(1000);
#ifndef LCD_BASIC
    LCD_EX_INIT();
#endif
}

void lcd_command(uint8_t cmd, uint8_t cnt, ...)
{
    LCD_SET_TRANSFER_8();
    LCDIF_SCMD0 = cmd;
    va_list args;
    va_start(args, cnt);
    uint8_t data;
    while (cnt--)
    {
        data = va_arg(args, int);
        LCDIF_SDAT0 = data;
    }
    va_end(args);
}

void lcd_block_write_slow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
    LCD_SET_TRANSFER_8();
    lcd_command_write(CASET);
    lcd_data_write(__builtin_bswap16(sx));
    lcd_data_write(sx);
    lcd_data_write(__builtin_bswap16(ex));
    lcd_data_write(ex);
    lcd_command_write(RASET);
    lcd_data_write(__builtin_bswap16(sy));
    lcd_data_write(sy);
    lcd_data_write(__builtin_bswap16(ey));
    lcd_data_write(ey);
    lcd_command_write(RAMWR);
}

void lcd_block_write(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
    LCD_SET_TRANSFER_8();
    LCDIF_SCMD0 = CASET;

    LCD_SET_TRANSFER_16();
    LCDIF_SDAT0 = sx;
    LCDIF_SDAT0 = ex;

    LCD_SET_TRANSFER_8();
    LCDIF_SCMD0 = RASET;

    LCD_SET_TRANSFER_16();
    LCDIF_SDAT0 = sy;
    LCDIF_SDAT0 = ey;

    LCD_SET_TRANSFER_8();
    LCDIF_SCMD0 = RAMWR;
}

void lcd_fill_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int size = (x2 - x1 + 1) * (y2 - y1 + 1);
    lcd_block_write(x1, y1, x2, y2);
    LCD_SET_TRANSFER_16();
    while (size--)
    {
        LCDIF_SDAT0 = color;
    }
    LCD_SET_TRANSFER_8();
}

void lcd_draw_image_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *image)
{
    if (image)
    {
        int size = (x2 - x1 + 1) * (y2 - y1 + 1);
        lcd_block_write(x1, y1, x2, y2);
        LCD_SET_TRANSFER_16();
        while (size--)
        {
            LCDIF_SDAT0 = *image++;
        }
    }
    LCD_SET_TRANSFER_8();
}

void lcd_draw_pixel(int16_t x, int16_t y, uint16_t color)
{
    lcd_block_write(x, y, x + 1, y + 1);
    LCD_SET_TRANSFER_16();
    LCDIF_SDAT0 = color;
}

//////////////////////////////////////////////////////////////////////////////////////////////

#define LAYER_ENABLE_MASK LCDIF_L0EN;
pLAYER LAYER = (volatile pLAYER) & LCDIF_LAYER0BASE;

extern void Ql_Sleep(uint32_t msec);
void lcd_ex_run(void)
{
    LCDIF_START = 0;
    LCDIF_START = LCDIF_RUN;
    while (LCDIF_STA & LCDIF_RUNNING)
        ; // max 22ms
    LCDIF_START = 0;
}

void LCD_EX_INIT(void)
{
    LAYER->LCDIF_LWINCON = 0;
    LAYER->LCDIF_LWINKEY = 0;
    LAYER->LCDIF_LWINOFFS = 0;
    LAYER->LCDIF_LWINADD = 0;
    LAYER->LCDIF_LWINSIZE = 0;
    LAYER->LCDIF_LWINSCRL = 0;
    LAYER->LCDIF_LWINMOFS = 0;
    LAYER->LCDIF_LWINPITCH = 0;
    LCDIF_WROICON = LCDIF_F_ITF_8B | LCDIF_F_RGB565 | LCDIF_F_BGR;
    LCDIF_WROICADD = LCDIF_CSIF0;
    LCDIF_WROIDADD = LCDIF_DSIF0;
    LCDIF_WROIOFS = 0;
    LCDIF_WROISIZE = LCDIF_WROICOL(LCD_X_RESOLUTION) | LCDIF_WROIROW(LCD_Y_RESOLUTION);
    LCDIF_WROI_BGCLR = 0;
    lcd_ex_run();
}

uint32_t lcd_ex_block_write(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
#define DATA_LEN 11
#define H16(v) (((v)&0xFF00) >> 8)
#define L16(v) ((v)&0xFF)
    uint32_t data[DATA_LEN];
    uint32_t *p = data;
    *p++ = LCDIF_COMM(CASET) | LCDIF_CMD;
    *p++ = LCDIF_COMM(H16(sx)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(L16(sx)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(H16(ex)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(L16(ex)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(RASET) | LCDIF_CMD;
    *p++ = LCDIF_COMM(H16(sy)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(L16(sy)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(H16(ey)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(L16(ey)) | LCDIF_DATA;
    *p++ = LCDIF_COMM(RAMWR) | LCDIF_CMD;
    uint32_t w = ex - sx + 1;
    uint32_t h = ey - sy + 1;
    LCDIF_WROIOFS = LCDIF_WROIOFX(sx) | LCDIF_WROIOFY(sy);
    LCDIF_WROISIZE = LCDIF_WROICOL(w) | LCDIF_WROIROW(h);
    p = data;
    for (int i = 0; i < DATA_LEN; i++)
        LCDIF_COMD(i) = *p++;

    LCDIF_WROICON &= ~LCDIF_COMMAND_MASK;
    LCDIF_WROICON |= LCDIF_COMMAND(DATA_LEN - 1) | LCDIF_ENC; // enable command transfer + data
    return w * h;
}

void lcd_ex_fill_rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    LCDIF_WROI_BGCLR = ((color & RED) << 8) | ((color & GREEN) << 5) | ((color & BLUE) << 3);
    LCDIF_WROICON &= ~LAYER_ENABLE_MASK;
    lcd_ex_block_write(sx, sy, ex, ey);
    lcd_ex_run();
}

void lcd_ex_draw_image_rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *image)
{
    if (image)
    {
        uint32_t w = ex - sx + 1;
        uint32_t h = ey - sy + 1;
        LAYER->LCDIF_LWINADD = (uint32_t)image;
        LAYER->LCDIF_LWINCON = LCDIF_LCF(LCDIF_LCF_RGB565) | LCDIF_LROTATE(LCDIF_LR_NO); // | LCDIF_LRGB_SWP;
        LAYER->LCDIF_LWINOFFS = LCDIF_LWINOF_X(sx) | LCDIF_LWINOF_Y(sy);
        LAYER->LCDIF_LWINSIZE = LCDIF_LCOLS(w) | LCDIF_LROWS(h);
        LAYER->LCDIF_LWINPITCH = w * 2;
        LAYER->LCDIF_LWINSCRL = 0;
        LAYER->LCDIF_LWINMOFS = 0;
        LAYER->LCDIF_LWINKEY = 0;
        LCDIF_WROICON |= LAYER_ENABLE_MASK;
        lcd_ex_block_write(sx, sy, ex, ey);
        lcd_ex_run();
        //LCDIF_WROICON &= ~LAYER_ENABLE_MASK; // cleared in fill()
    }
}
