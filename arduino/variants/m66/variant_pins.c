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
    {0,     PINNAME_NETLIGHT,   0x03,   NULL}, //
    {1,     PINNAME_DTR,        0x30,   NULL}, //
    {2,     PINNAME_RI,         0x43,   NULL}, //
    {3,     PINNAME_DCD,        0x44,   NULL}, //
    {4,     PINNAME_CTS,        0x14,   NULL}, //
    {5,     PINNAME_RTS,        0x15,   NULL}, //
    {6,     PINNAME_RXD_AUX,    0x00,   NULL}, //
    {7,     PINNAME_TXD_AUX,    0x01,   NULL}, //
    {8,     PINNAME_PCM_CLK,    0x26,   NULL}, //
    {9,     PINNAME_PCM_SYNC,   0x29,   NULL}, //
    {10,    PINNAME_PCM_IN,     0x27,   NULL}, //
    {11,    PINNAME_PCM_OUT,    0x28,   NULL}, //
    {12,    PINNAME_RFTXMON,    0x22,   NULL}, //
};
/* clang-format on */