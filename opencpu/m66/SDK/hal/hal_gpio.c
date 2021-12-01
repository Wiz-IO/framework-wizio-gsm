// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
* This file is part of the DZ09 project.
*
* Copyright (C) 2020, 2019 AJScorp
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "hal_gpio.h"

void GPIO_Setup(uint32_t Pin, uint32_t Flags)
{
    if (Pin <= GPIOMAX)
    {
        if (Flags & GPDO)
            GPIO_SETDIROUT(Pin);
        else
            GPIO_SETDIRIN(Pin);

        if (Flags & GPDIEN)
            GPIO_SETINPUTEN(Pin);
        else
            GPIO_SETINPUTDIS(Pin);

        if (Flags & GPPDN)
            GPIO_SETPULLDOWN(Pin);
        else
            GPIO_SETPULLUP(Pin);

        if (Flags & GPPULLEN)
            GPIO_PULLENABLE(Pin);
        else
            GPIO_PULLDISABLE(Pin);

        if (Flags & GPSMT)
            GPIO_SMTENABLE(Pin);
        else
            GPIO_SMTDISABLE(Pin);

        GPIO_SETMODE(Pin, (Flags >> 8));
    }
}