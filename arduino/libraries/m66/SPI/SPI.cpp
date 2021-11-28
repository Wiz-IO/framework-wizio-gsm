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

#include <Arduino.h>
#include "SPI.h"

#define DEBUG_SPI 
//Serial.printf

static uint8_t reverseByte(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

#define SPI_TYPE (_port == 1) // HW SPI

SPIClass SPI(1); // HW SPI

////////////////////////////////////////////////////////////////////////////////////////////

SPISettings::SPISettings(uint32_t clockFrequency, BitOrder bitOrder, SPIDataMode dataMode)
{
    clock = clockFrequency / 1000;
    order = bitOrder;
    mode = dataMode;
}

SPISettings::SPISettings()
{ /* default SPI setting */
    clock = 1000;
    order = MSBFIRST;
    mode = SPI_MODE0;
}

////////////////////////////////////////////////////////////////////////////////////////////

void SPIClass::setPins(int8_t sck, int8_t miso, int8_t mosi, int8_t cs)
{
    if (SPI_TYPE)
    {
        setPins();
    }
    else
    {
        //TODO
    }
}

void SPIClass::setPins() // HW pins
{
    _miso = PINNAME_PCM_SYNC;
    _mosi = PINNAME_PCM_OUT;
    _clk = PINNAME_PCM_IN;
    _cs = PINNAME_PCM_CLK;
}

SPIClass::SPIClass()
{
    _port = 1; // HW
    setPins();
}

SPIClass::SPIClass(uint32_t port)
{
    _port = 1;
    setPins();
}

void SPIClass::setBitOrder(BitOrder order)
{
    _order = order;
};

void SPIClass::setDataMode(uint8_t mode)
{
    _cpol = (bool)mode & 2;
    _cpha = (bool)mode & 1;
};

void SPIClass::setFrequency(uint32_t kHz)
{
    _clock = kHz;
}

void SPIClass::begin()
{
    int res;
    if ((res = Ql_SPI_Init(_port, _clk, _miso, _mosi, _cs, SPI_TYPE)))
    {
        DEBUG_SPI("[ERROR] Ql_SPI_Init( %d )\n", res);
    }
}

void SPIClass::end()
{
    Ql_SPI_Uninit(_port);
}

void SPIClass::beginTransaction(SPISettings s)
{
    int res;
    setFrequency(s.clock);
    setDataMode(s.mode);
    setBitOrder(s.order);
    if ((res = Ql_SPI_Config(_port, true, _cpol, _cpha, _clock /*kHz*/)))
    {
        DEBUG_SPI("[ERROR] Ql_SPI_Config( %d ) %d, %d, %d, %d\n", res, (int)_port, (int)_cpol, (int)_cpha, (int)_clock);
    }
}

uint8_t SPIClass::transfer(uint8_t tx)
{
    int res;
    uint8_t rx;

    if (_order == LSBFIRST)
        tx = __REV(__RBIT(tx));
    //if (_order == LSBFIRST) tx = reverseByte(tx);
    if (1 != (res = Ql_SPI_WriteRead(_port, &tx, 1, &rx, 1)))
    {
        DEBUG_SPI("[ERROR] Ql_SPI_WriteRead( %d )\n", res);
    }
    if (_order == LSBFIRST)
        rx = __REV(__RBIT(rx));
    //if (_order == LSBFIRST) rx = reverseByte(rx);
    return res == 1 ? rx : 0;
}

uint16_t SPIClass::transfer16(uint16_t _data)
{
    union
    {
        uint16_t val;
        struct
        {
            uint8_t lsb;
            uint8_t msb;
        };
    } t;
    t.val = _data;
    if (_order == LSBFIRST)
    {
        t.lsb = transfer(t.lsb);
        t.msb = transfer(t.msb);
    }
    else
    {
        t.msb = transfer(t.msb);
        t.lsb = transfer(t.lsb);
    }
    return t.val;
}

int SPIClass::transfer(uint8_t *tx, uint32_t wLen)
{

    if (tx && wLen)
    {
        if (_order == MSBFIRST)
        {
            return Ql_SPI_WriteRead(_port, tx, wLen, tx, wLen);
        }
        else
        {
            for (int i = wLen; i; i--)
                tx[i - 1] = transfer(tx[i - 1]);
        }
    }
    return -1;
}

int SPIClass::transfer(uint8_t *tx, uint32_t wLen, uint8_t *rx, uint32_t rLen)
{
    if (tx && rx && wLen && rLen)
    {
        if (_order == MSBFIRST)
        {
            return Ql_SPI_WriteRead(_port, tx, wLen, rx, rLen);
        }
        else
        {
            for (int i = wLen; i; i--)
                rx[i - 1] = transfer(tx[i - 1]);
        }
    }
    return -1;
}
