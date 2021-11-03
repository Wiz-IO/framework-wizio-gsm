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

PinDescription pinsMap[13] = {
    {0, PINNAME_NETLIGHT, NULL},
    {1, PINNAME_DTR, NULL},
    {2, PINNAME_RI, NULL},
    {3, PINNAME_DCD, NULL},
    {4, PINNAME_CTS, NULL},
    {5, PINNAME_RTS, NULL},
    {6, PINNAME_RXD_AUX, NULL},
    {7, PINNAME_TXD_AUX, NULL},
    {8, PINNAME_PCM_CLK, NULL},
    {9, PINNAME_PCM_SYNC, NULL},
    {10, PINNAME_PCM_IN, NULL},
    {11, PINNAME_PCM_OUT, NULL},
    {12, PINNAME_RFTXMON, NULL},
};
