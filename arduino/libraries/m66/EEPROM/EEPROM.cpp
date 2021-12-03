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

#include "EEPROM.h"

#ifdef __OCPU_RIL_SUPPORT__

#define DEBUG_EEP
//Serial.printf

EEPROMClass EEPROM;

// Quectel cell from 1 to 13
static void ql_eeprom_get_index(u32 address, u8 *cell, u32 *size, u32 *offset)
{
    if (address < 400)
    {
        *cell = 1 + (address / 50);
        *size = 50;
        *offset = 0;
        return;
    }
    else if (address < 800)
    {
        address -= 400;
        *cell = 9 + (address / 100);
        *size = 100;
        *offset = 400;
        return;
    }
    else
    {
        *cell = 13;
        *size = 500;
        *offset = 800;
    }
}

uint8_t EERef::operator*() const
{
    if (index < _EEPROM_SIZE) // index is eeprom address
    {
        u8 cell;
        u32 size, offset;
        ql_eeprom_get_index(index, &cell, &size, &offset);
        uint8_t data[size];
        int res = Ql_SecureData_Read(cell, (u8 *)data, size);
        DEBUG_EEP("[EEPROM] Read Cell = %d [ %d ] RES = %d\n", cell, size, res);
        if (0 == res)
        {
            DEBUG_EEP("[EEPROM] Read empty cell. Return 0 as value\n");
        }
        else if (size == res)
        {
            return data[index - offset]; // eeprom_address - cell_offset ( 0 .. 400 .. 800 )
        }
        else
        {
            DEBUG_EEP("[ERROR] EEPROM Read failed. Return 0 as value\n");
        }
    }
    return 0;
}

EERef &EERef::operator=(uint8_t value)
{
    if (index < _EEPROM_SIZE) // index is eeprom address
    {
        u8 cell;
        u32 size, offset;
        ql_eeprom_get_index(index, &cell, &size, &offset);
        uint8_t data[size];
        int res = Ql_SecureData_Read(cell, (u8 *)data, size); // read cell data[]
        DEBUG_EEP("[EEPROM] Read(W) Cell = %d [ %d ] RES = %d\n", cell, size, res);
        if (res > -1)
        {
            if (0 == res) // the cell is empty
            {
                memset(data, 0, size);
            }
            if (size == res) // must be
            {
                data[index - offset] = value; // eeprom_address - cell_offset ( 0 .. 400 .. 800 ), change data[x] = value
                if ((res = Ql_SecureData_Store(cell, (u8 *)data, size))) // save cell data[]
                {
                    DEBUG_EEP("[ERROR] EEPROM Write failed ( %d )\n", res);
                }
            }
        }
    }
    return *this;
}

#endif // __OCPU_RIL_SUPPORT__
