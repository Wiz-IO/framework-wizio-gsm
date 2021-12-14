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
////////////////////////////////////////////////////////////////////////////////////////

#include "variant.h"

/* clang-format off */

PinDescription pinsMap[MAX_PINS] = {
// Arduino   Quectel            Mediatek    EINT_CB // [pin]    
    {0,     PINNAME_NETLIGHT,       2,      NULL }, // [47] LED_NET
    {1,     PINNAME_DTR,           30,      NULL }, // [37] 
    {2,     PINNAME_RI,            43,      NULL }, // [35] SCL
    {3,     PINNAME_DCD,           44,      NULL }, // [36] SDA
    {4,     PINNAME_CTS,           14,      NULL }, // [38] 
    {5,     PINNAME_RTS,           15,      NULL }, // [39] 
    {6,     PINNAME_PCM_CLK,       26,      NULL }, // [59] SPI_CS      LCD_SCK
    {7,     PINNAME_PCM_SYNC,      29,      NULL }, // [61] SPI_MISO    LCD_DC
    {8,     PINNAME_PCM_IN,        27,      NULL }, // [62] SPI_SCK     LCD_CS
    {9,     PINNAME_PCM_OUT,       28,      NULL }, // [60] SPI_MOSI    LCD_DAT
    {10,    PINNAME_SD_CMD,        31,      NULL }, // [07] 
    {11,    PINNAME_SD_CLK,        32,      NULL }, // [08] 
    {12,    PINNAME_SD_DATA,       33,      NULL }, // [09] 
    {13,    PINNAME_SIM2_DATA,     40,      NULL }, // [11] 
    {14,    PINNAME_SIM2_RST,      41,      NULL }, // [12] 
    {15,    PINNAME_SIM2_CLK,      42,      NULL }, // [10] 
    {16,    PINNAME_GPIO0,         34,      NULL }, // [57] 
    {17,    PINNAME_GPIO1,          8,      NULL }, // [58] SCL
    {18,    PINNAME_GPIO2,          9,      NULL }, // [63] SDA
    {19,    PINNAME_GPIO3,         20,      NULL }, // [64] 
    {20,    PINNAME_GPIO4,         21,      NULL }, // [65] 

// EXTENDED -----------------------------------------------------------------------   

    {-1,    -1,                    -1,      NULL }, // [33] TX1-MAIN
    {-1,    -1,                    -1,      NULL }, // [34] RX1-MAIN
    {-1,    -1,                    -1,      NULL }, // [29] TX2-DBG
    {-1,    -1,                    -1,      NULL }, // [30] RX2-DBG 
    {-1,    -1,                    -1,      NULL }, // [29] TX3-AUX
    {-1,    -1,                    -1,      NULL }, // [25] RX3-AUX     
    {-1,    -1,                    -1,      NULL }, // [22] TX4-GPS
    {-1,    -1,                    -1,      NULL }, // [23] RX4-GPS         

    {-1,    -1,                    -1,      NULL }, // [55] RESERVED 
    {-1,    -1,                    -1,      NULL }, // [56] RESERVED 
    {-1,    -1,                    -1,      NULL }, // [66] RESERVED 
    {-1,    -1,                    -1,      NULL }, // [67] RESERVED 
    {-1,    -1,                    -1,      NULL }, // [68] RESERVED 

  //{-1,    -1,                    -1,      NULL }, // [28] GNSS_VCC_EN    
};

/* clang-format on */