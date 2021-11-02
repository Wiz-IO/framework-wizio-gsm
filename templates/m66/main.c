////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Georgi Angelov
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

#ifdef __cplusplus
extern "C"
{
#endif

#include "custom_feature_def.h"
#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_uart.h"
#include "ql_timer.h"
#include "ril.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
#define EXTERN extern "C" 
}
#else 
#define EXTERN
#endif //__cplusplus

char DBG_BUFFER[256];
#define DBG_PORT UART_PORT1
#define DBG(FORMAT, ...)                                                  \
    {                                                                     \
        Ql_memset(DBG_BUFFER, 0, sizeof(DBG_BUFFER));                     \
        Ql_sprintf(DBG_BUFFER, FORMAT, ##__VA_ARGS__);                    \
        Ql_UART_Write(DBG_PORT, (u8 *)DBG_BUFFER, Ql_strlen(DBG_BUFFER)); \
    }

void uart_callback(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void *customizedPara)
{
}

const Enum_PinName LED = PINNAME_NETLIGHT;
static u32 GP_timer = 0x101;
static u32 GPT_Interval = 1000;
static s32 timer_param = 0;
void timerCB(u32 timerId, void *param)
{
    static int val = 0;
    Ql_GPIO_SetLevel(LED, (Enum_PinLevel)val);
    val ^= 1;
}

EXTERN void proc_main_task(s32 taskId)
{
    s32 ret;
    ST_MSG msg;
    Ql_UART_Register(DBG_PORT, uart_callback, NULL);
    Ql_UART_Open(DBG_PORT, 115200, FC_NONE);
    Ql_GPIO_Init(LED, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_DISABLE);
    Ql_Timer_RegisterFast(GP_timer, timerCB, &timer_param);
    Ql_Timer_Start(GP_timer, GPT_Interval, TRUE);
    DBG("[APP] Hello World 2021 COMET M66\n");
    DBG("[APP] BEGIN\n");
    while (1)
    {
        //DBG("[APP] MSG %X, %X, %X\n", msg.message, msg.param1, msg.param2);
        Ql_OS_GetMessage(&msg);
        switch (msg.message)
        {
        case MSG_ID_RIL_READY:
            Ql_RIL_Initialize();
            DBG("[APP] Ril Ready\n");
            break;
        }
    }
}
