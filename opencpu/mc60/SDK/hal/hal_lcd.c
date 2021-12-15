////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
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
////////////////////////////////////////////////////////////////////////////

#include "hal_lcd.h"

void LCDIF_DisableInterface(void)
{
    PCTL_PowerDown(PD_LCD);  // Power down LCD controller
    PCTL_PowerDown(PD_SLCD); // Power down serial interface
}

void LCDIF_InitInterface(uint32_t config_clock, uint32_t config_time, uint32_t config_format)
{
    PCTL_PowerUp(PD_SLCD); // Power up LCD serial interface
    PCTL_PowerUp(PD_LCD);  // Power up LCD controller

    LCD_SERIAL_CLOCK_REG = (LCD_SERIAL_CLOCK_REG & ~LCD_SERIAL_CLOCK_MASK) | LCD_SERIAL_CLOCK(config_clock);

    LCDIF_START = LCDIF_INT_RESET; // Assert LCD controller internal Reset
    LCDIF_START = 0;               // Release LCD controller internal Reset

    LCDIF_SIF0_TIMING = config_time; // Setup SIF timing
    LCDIF_SIF_CON = config_format;   // Setup interface configuration
}
