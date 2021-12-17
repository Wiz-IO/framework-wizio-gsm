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

#include "hal_api.h"

hal_api_t *HAL = NULL;

#ifdef USE_API

#define API_COUNT 1

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
/* FIRMWARE DEPENDENT FUNCTIONS */
static hal_api_t HAL_TABLE[API_COUNT] = {
    {
        "TODO",
    },
};
#pragma GCC diagnostic pop

void api_init(void)
{
    HAL = NULL;
    for (int i = 0; i < API_COUNT; i++)
    {
        if (0 == memcmp(HAL_TABLE[i].FW_VER_ADDR, HAL_TABLE[i].FW_VER, sizeof(HAL_TABLE[i].FW_VER)))
        {
            HAL = &HAL_TABLE[i];
            break;
        }
    }
}

#else
void api_init(void)
{
    HAL = NULL;
}
#endif

const char *api_getVersion(void)
{
    return HAL ? HAL->FW_VER : NULL;
}
