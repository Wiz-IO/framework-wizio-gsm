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
//Arduino   Quectel             Mediatek  eint_cb
    {0,     PINNAME_NETLIGHT,        3,   NULL }, // [16] LED_NET, PWM
    {1,     PINNAME_DTR,            30,   NULL }, // [19] EINT
    {2,     PINNAME_RI,             43,   NULL }, // [20] I2C_SCL
    {3,     PINNAME_DCD,            44,   NULL }, // [21] I2C_SDA
    {4,     PINNAME_CTS,            14,   NULL }, // [22] 
    {5,     PINNAME_RTS,            15,   NULL }, // [23]
    {6,     PINNAME_RXD_AUX,         0,   NULL }, // [28]
    {7,     PINNAME_TXD_AUX,         1,   NULL }, // [29]
    {8,     PINNAME_PCM_CLK,        26,   NULL }, // [30] SPI_CS        
    {9,     PINNAME_PCM_SYNC,       29,   NULL }, // [31] SPI_MISO      
    {10,    PINNAME_PCM_IN,         27,   NULL }, // [32] SPI_CLK       
    {11,    PINNAME_PCM_OUT,        28,   NULL }, // [33] SPI_MOSI      
    {12,    PINNAME_RFTXMON,        22,   NULL }, // [25] LED, AT+QCFG="RFTXburst",1/2
};

/* clang-format on */
