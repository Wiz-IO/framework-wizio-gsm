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

#include <hal_btif.h>
#include <hal_pctl.h>
#include <hal_ustimer.h>

static int btif_power_on = 0;

void BTIF_Initialize(void) // brt_drv_init.c
{
    //ARM7_Initialize()
    RegAddr32(0xA0010310) |= (1 << 15);
    delay_u(5);
    RegAddr32(0xA0010310) |= ((1 << 13) | (1 << 9));
    RegAddr32(0xA0180804) &= ~(TOPSM_PWR_ON | TOPSM_PWR_REQ_EN | TOPSM_PWR_RESET);
    delay_u(150);

    //ARM7_PowerOn_Digital()
    RegAddr32(0xA0010320) |= 0xA200;
    RegAddr32(0xA0180804) |= TOPSM_PWR_ON | TOPSM_PWR_REQ_EN;
    delay_u(50);
}

void BTIF_Power(bool on_off, bool init)
{
    if (init)
        BTIF_Initialize();

    if (on_off)
    {
        PCTL_PowerUp(PD_BTIF);
    }
    else
    {
        PCTL_PowerDown(PD_BTIF);
    }

    btif_power_on = on_off;

    delay_u(50);
}

void BTIF_Write(char byte)
{
    if (btif_power_on)
    {
        while (!(BTIF_LSR & BTIF_LSR_TEMT_BIT))
            ;
        BTIF_THR = byte;
    }
}

void BTIF_Open(void (*BT_OPEN)(void))
{
    BTIF_Power(true, false);
    BTIF_SLEEP_EN = 1;
    if (BT_OPEN) // register IRQ
        BT_OPEN();
}

void BTIF_Close(void (*BT_CLOSE)(void))
{
    if (BT_CLOSE)
        BT_CLOSE();
    BTIF_Power(false, false);
}