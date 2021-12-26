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

#ifndef _BTIF_H_
#define _BTIF_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <hal.h>

//#define BTIF_base             (0xA00B0000)
#define BTIF_RBR                (*(volatile uint16_t *)(BTIF_base + 0x0000)) ///* RX Buffer Register: read only */
#define BTIF_THR                (*(volatile uint16_t *)(BTIF_base + 0x0000)) ///* TX Holding Register: write only */
#define BTIF_IER                (*(volatile uint16_t *)(BTIF_base + 0x0004)) ///* Interrupt Enable Register: read/write */
#define BTIF_IIR                (*(volatile uint16_t *)(BTIF_base + 0x0008))
#define BTIF_FIFOCTRL           (*(volatile uint16_t *)(BTIF_base + 0x0008))
#define BTIF_FAKELCR            (*(volatile uint16_t *)(BTIF_base + 0x000C))
#define BTIF_LSR                (*(volatile uint16_t *)(BTIF_base + 0x0014)) ///* Line Status Register: read only */
#define BTIF_SLEEP_EN           (*(volatile uint16_t *)(BTIF_base + 0x0048)) ///* Sleep Enable Register: read/write */
#define BTIF_DMA_EN             (*(volatile uint16_t *)(BTIF_base + 0x004C)) ///* DMA Enable Register: read/write */ 
#define BTIF_RTOCNT             (*(volatile uint16_t *)(BTIF_base + 0x0054))
#define BTIF_TRI_LVL            (*(volatile uint16_t *)(BTIF_base + 0x0060))
#define BTIF_WAK                (*(volatile uint16_t *)(BTIF_base + 0x0064))
#define BTIF_WAT_TIME           (*(volatile uint16_t *)(BTIF_base + 0x0068))
#define BTIF_HANDSHAKE          (*(volatile uint16_t *)(BTIF_base + 0x006C))

/*BTIF_LSR bits*/
#define BTIF_LSR_DR_BIT         (1 << 0)    /* 1: Rx full */
#define BTIF_LSR_THRE_BIT       (1 << 5)
#define BTIF_LSR_TEMT_BIT       (1 << 6)    /* 1: Tx is empty */

/*BTIF_IER bits*/
#define BTIF_IER_TXEEN          (1 << 1)	/* 1: Tx holding register is empty */
#define BTIF_IER_RXFEN          (1 << 0)	/* 1: Rx buffer contains data */

/*BTIF_IIR bits*/
#define BTIF_IIR_NINT           (1 << 0)	/* No INT Pending */
#define BTIF_IIR_TX_EMPTY       (1 << 1)	/* Tx Holding Register empty */
#define BTIF_IIR_RX             (1 << 2)	/* Rx data received */
#define BTIF_IIR_RX_TIMEOUT     (0x11 << 2)	/* Rx data received */

/*BTIF_SLEEP_EN bits*/
#define BTIF_SLEEP_EN_BIT       (1 << 0)	/* enable Sleep mode  */
#define BTIF_SLEEP_DIS_BIT      (0)	        /* disable sleep mode */

#define BTIF_TX_FIFO_SIZE       (16)
#define BTIF_RX_FIFO_SIZE       (8)


#define TOPSM_RM_PWR_CON1       (*(volatile uint32_t *)(TOPSM_base + 0x0804))
#define TOPSM_PWR_ON            (0x00000004)     
#define TOPSM_PWR_REQ_EN        (0x00000040) 
#define TOPSM_PWR_RESET         (0x00002000) 


void BTIF_Initialize(void);
void BTIF_Power(bool on_off, bool init);
void BTIF_Write(char byte);
void BTIF_Open(void (*BT_OPEN)(void));
void BTIF_Close(void (*BT_CLOSE)(void));

#ifdef __cplusplus
}
#endif
#endif