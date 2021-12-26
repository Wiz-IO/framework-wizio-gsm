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

#ifdef USE_API /* FIRMWARE DEPENDENT LIBRARY */

#ifndef HCISerial_h
#define HCISerial_h

#include <hal_btif.h>
#include <Stream.h>
#include <RingBuffer.h>

#ifdef __cplusplus

#define BTIF_OS_IRQ (0x26u) // OS IRQ number

typedef RingBufferN<512> BT_RingBuffer;

static BT_RingBuffer *pRX = NULL;

class BT_HCI_Serial : public Stream
{
private:
    BT_RingBuffer bt_rx;

    static void bt_irq_handler(void)
    {
        while (pRX && (BTIF_LSR & BTIF_LSR_DR_BIT))
        {
            if (pRX->availableForStore())
            {
                pRX->store_char(BTIF_RBR);
            }
            else
            {
                break;
            }
        }

        HAL->IRQ_Clear(BTIF_OS_IRQ);
    }

    static void btif_open(void)
    {
        if (HAL)
        {
            BTIF_IER = 0;
            BTIF_DMA_EN = 0;

            HAL->IRQ_Register(BTIF_OS_IRQ, bt_irq_handler, "BT-HCI");
            HAL->IRQ_Sensitivity(BTIF_OS_IRQ, 0);
            HAL->IRQ_Unmask(BTIF_OS_IRQ);

            BTIF_IER = 1; // enable Rx IRQ
        }
    }

public:
    BT_HCI_Serial()
    {
        pRX = &bt_rx;
        BTIF_Initialize();
    }

    ~BT_HCI_Serial() { end(); }

    void begin(unsigned long brg = 0)
    {
        bt_rx.clear();
        if (HAL)
        {
            BTIF_Open(btif_open);
        }
    }

    void end()
    {
        BTIF_IER = 0;
        BTIF_Close(NULL);
    }

    void flush() { bt_rx.clear(); }

    int available(void) { return bt_rx.available(); }

    int peek(void) { return bt_rx.peek(); }

    int read()
    {
        int res = -1;
        //uint32_t isr = DisableInterrupts();
        if (bt_rx.available() > 0)
        {
            res = bt_rx.read_char();
        }
        //RestoreInterrupts(isr);
        return res;
    }

    size_t write(uint8_t byte)
    {
        BTIF_Write(byte);
        return 1;
    }

    size_t write(uint8_t *buffer, size_t size)
    {
        if (buffer && size)
        {
            size_t cnt = size;
            while (cnt--)
                BTIF_Write(*buffer++);
        }
        return size;
    }

    operator bool() { return true; }

    using Print::write;
};

extern BT_HCI_Serial HCI;

#endif // CPP

#endif // HCISerial_h

#else
#error BT_HCI_Serial library use API support
#endif