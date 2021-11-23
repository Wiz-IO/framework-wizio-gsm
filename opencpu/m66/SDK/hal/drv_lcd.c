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
//  Basic driver ( 16 bit Color ) for ILIxxxx & STxxxx LCD displays 
//  Tested with ST7789 & ILI9341
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
        uint16_t ms;
        uint8_t numArgs, numCommands = *p++;
        LCD_SET_TRANSFER_8();
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
    //lcd_fill_color(0);
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
}

void lcd_draw_imageRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *data)
{
    int size = (x2 - x1 + 1) * (y2 - y1 + 1);
    lcd_block_write(x1, y1, x2, y2);
    LCD_SET_TRANSFER_16();
    while (size--)
    {
        LCDIF_SDAT0 = *data++;
    }
}

void lcd_draw_pixel(int16_t x, int16_t y, uint16_t color)
{
    lcd_block_write(x, y, x + 1, y + 1);
    LCD_SET_TRANSFER_16();
    LCDIF_SDAT0 = color;
}
