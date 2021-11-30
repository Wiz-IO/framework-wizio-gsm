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

#ifndef _HAL_G2D_H_
#define _HAL_G2D_H_
#ifdef __cplusplus
extern "C"
{
#endif
/* clang-format off */

#include "hal_pctl.h"

#define G2D_base					                          (0xA0440000) /* G2D - Graphic 2D */ 

#define REG_G2D_START                               *(volatile uint16_t *)(G2D_base + 0x0000)
#define REG_G2D_MODE_CTRL                           *(volatile uint32_t *)(G2D_base + 0x0004)
#define REG_G2D_RESET                               *(volatile uint16_t *)(G2D_base + 0x0008)
#define REG_G2D_STATUS                              *(volatile uint16_t *)(G2D_base + 0x000C)
 #define REG_G2D_SLOW_DOWN                          *(volatile uint32_t *)(G2D_base + 0x0014)

#define REG_G2D_ROI_CON                             *(volatile uint32_t *)(G2D_base + 0x0040)
#define REG_G2D_W2M_ADDR                            *(volatile uint32_t *)(G2D_base + 0x0044)
#define REG_G2D_W2M_PITCH                           *(volatile uint32_t *)(G2D_base + 0x0048)
#define REG_G2D_W2M_OFFSET                          *(volatile uint32_t *)(G2D_base + 0x0068)

#define REG_G2D_ROI_OFFSET                          *(volatile uint32_t *)(G2D_base + 0x004C)
#define REG_G2D_ROI_SIZE                            *(volatile uint32_t *)(G2D_base + 0x0050)
#define REG_G2D_ROI_BGCLR                           *(volatile uint32_t *)(G2D_base + 0x0054)

#define REG_G2D_CLP_MIN                             *(volatile uint32_t *)(G2D_base + 0x0058)
#define REG_G2D_CLP_MAX                             *(volatile uint32_t *)(G2D_base + 0x005C)
#define G2D_ROI_CON_CLIP_ENABLE_BIT                 0x10000

#define G2D_LAYER_ADDR_OFFSET                       0x40
#define REG_G2D_LAYER_CON(n)                        *((volatile uint32_t *) (G2D_base + 0x0080 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_ADDR(n)                       *((volatile uint32_t *) (G2D_base + 0x0084 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_PITCH(n)                      *((volatile uint16_t *) (G2D_base + 0x0088 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_OFFSET(n)                     *((volatile uint32_t *) (G2D_base + 0x008C + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_SIZE(n)                       *((volatile uint32_t *) (G2D_base + 0x0090 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_SRC_KEY(n)                    *((volatile uint32_t *) (G2D_base + 0x0094 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_RECTANGLE_FILL_COLOR(n)       *((volatile uint32_t *) (G2D_base + 0x0094 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_FONT_FOREGROUND_COLOR(n)      *((volatile uint32_t *) (G2D_base + 0x0094 + (n) * G2D_LAYER_ADDR_OFFSET))

#define REG_G2D_DI_CON                              *(volatile uint32_t *)(G2D_base + 0x0078)


#define G2D_RESET_WARM_RESET_BIT                    0x0001
#define G2D_RESET_HARD_RESET_BIT                    0x0002
#define G2D_START_BIT                               0x00000001
#define G2D_STATUS_BUSY_BIT                         0x00000001

#define G2D_MODE_CON_ENG_MODE_G2D_LT_BIT            0x0002
#define G2D_MODE_CON_ENG_MODE_G2D_BITBLT_BIT        0x0001
#define G2D_MODE_CON_ENABLE_SAD_BIT                 0x0100

#define G2D_ROI_CON_W2M_COLOR_GRAY                  0x00
#define G2D_ROI_CON_W2M_COLOR_RGB565                0x01
#define G2D_ROI_CON_W2M_COLOR_ARGB8888              0x08 
#define G2D_ROI_CON_W2M_COLOR_ARGB8565              0x09
#define G2D_ROI_CON_W2M_COLOR_ARGB6666              0x0A
#define G2D_ROI_CON_W2M_COLOR_RGB888                0x03
#define G2D_ROI_CON_W2M_COLOR_BGR888                0x13 
#define G2D_ROI_CON_W2M_COLOR_PARGB8888             0x0C 
#define G2D_ROI_CON_W2M_COLOR_PARGB8565             0x0D
#define G2D_ROI_CON_W2M_COLOR_PARGB6666             0x0E
#define G2D_ROI_CON_W2M_COLOR_MASK                  0x1F 


#define G2D_LX_CON_ENABLE_SRC_KEY_BIT               0x00800000
#define G2D_LX_CON_ENABLE_RECT_BIT                  0x00400000
#define G2D_LX_CON_ENABLE_FONT_BIT                  0x40000000
#define G2D_LX_CON_AA_FONT_BIT_MASK                 0x30000000


#define G2D_LX_CON_COLOR_GRAY                       0x00
#define G2D_LX_CON_COLOR_RGB565                     0x01
#define G2D_LX_CON_COLOR_UYVY                       0x02
#define G2D_LX_CON_COLOR_RGB888                     0x03
#define G2D_LX_CON_COLOR_ARGB8888                   0x08
#define G2D_LX_CON_COLOR_ARGB8565                   0x09
#define G2D_LX_CON_COLOR_ARGB6666                   0x0A
#define G2D_LX_CON_COLOR_PARGB8888                  0x0C
#define G2D_LX_CON_COLOR_PARGB8565                  0x0D
#define G2D_LX_CON_COLOR_PARGB6666                  0x0E
#define G2D_LX_CON_COLOR_BGR888                     0x13
#define G2D_LX_CON_COLOR_MASK                       0x1F


#define G2D_LX_CON_CCW_ROTATE_MASK                  0x70000
#define G2D_LX_CON_CCW_ROTATE_000                   0x00000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_090            0x10000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_000            0x20000
#define G2D_LX_CON_CCW_ROTATE_090                   0x30000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_180            0x40000
#define G2D_LX_CON_CCW_ROTATE_270                   0x50000
#define G2D_LX_CON_CCW_ROTATE_180                   0x60000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_270            0x70000


////////////////////////////////////////////////////////////////////////////


#define G2D_HARD_RESET()                            \
  do {                                              \
    REG_G2D_RESET = 0;                              \
    REG_G2D_RESET = G2D_RESET_HARD_RESET_BIT ;      \
    REG_G2D_RESET = 0;                              \
  } while(0)


#define G2D_SET_W2M_ADDR(addr)                      \
  do {                                              \
    REG_G2D_W2M_ADDR = addr;                        \
  } while(0)


#define G2D_SET_W2M_PITCH(pitch)                    \
  do {                                              \
    REG_G2D_W2M_PITCH = pitch;                      \
  } while(0)


#define G2D_SET_W2M_OFFSET(x, y)                                                        \
  do {                                                                                  \
    REG_G2D_W2M_OFFSET = (((uint16_t)(x) & 0xFFFF)<< 16) | ((uint16_t)(y) & 0xFFFF);    \
  } while(0)

#define G2D_SET_W2M_COLOR_FORMAT(format)                        \
  do {                                                          \
    REG_G2D_ROI_CON &= ~G2D_ROI_CON_W2M_COLOR_MASK;             \
    REG_G2D_ROI_CON |= (format & G2D_ROI_CON_W2M_COLOR_MASK);   \
  } while(0)

#define G2D_SET_ROI_CON_BG_COLOR(color)                                           \
  do {                                                                            \
    REG_G2D_ROI_BGCLR = color;                                                    \
  } while(0)

#define G2D_SET_ROI_OFFSET(x, y)                                                  \
  do {                                                                            \
    REG_G2D_ROI_OFFSET = (((x) << 16) | (0xFFFF & (y)));                          \
  } while(0)

#define G2D_SET_ROI_SIZE(w, h)                                                    \
  do {                                                                            \
    REG_G2D_ROI_SIZE = (((w) << 16) | (h));                                       \
  } while(0)


///// Layer

#define G2D_CLEAR_LAYER_CON(n)                                                    \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) = 0;                                                     \
  } while(0)

#define G2D_SET_LAYER_ADDR(n, addr)                                               \
  do {                                                                            \
    REG_G2D_LAYER_ADDR(n) = addr;                                                 \
  } while(0)

#define G2D_SET_LAYER_CON_COLOR_FORMAT(n, format)                                 \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_COLOR_MASK;                               \
    REG_G2D_LAYER_CON(n) |= (format & G2D_LX_CON_COLOR_MASK);                     \
  } while(0)

#define G2D_SET_LAYER_OFFSET(n, x, y)                                             \
  do {                                                                            \
    REG_G2D_LAYER_OFFSET(n) = (((x) << 16) | (0xFFFF & (y)));                     \
  } while(0)

#define G2D_SET_LAYER_SIZE(n, w, h)                                               \
  do {                                                                            \
    REG_G2D_LAYER_SIZE(n) = (((w) << 16) | (h));                                  \
  } while(0)

#define G2D_SET_LAYER_PITCH(n, pitch)                                             \
  do {                                                                            \
    REG_G2D_LAYER_PITCH(n) = pitch;                                               \
  } while(0)

#define G2D_ENABLE_ROI_LAYER(layer)                                               \
  do {                                                                            \
    REG_G2D_ROI_CON |= (1 << (31 - layer));                                       \
  } while(0)

#define G2D_SET_LAYER_FONT_FOREGROUND_COLOR(n, color)                             \
  do {                                                                            \
    REG_G2D_LAYER_FONT_FOREGROUND_COLOR(n) = color;                               \
  } while(0)

#define G2D_SET_LAYER_RECTANGLE_FILL_COLOR(n, color)                              \
  do {                                                                            \
    REG_G2D_LAYER_RECTANGLE_FILL_COLOR(n) = color;                                \
  } while(0)

#define G2D_ENABLE_LAYER_CON_FONT(n)                                              \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) |= G2D_LX_CON_ENABLE_FONT_BIT;                           \
  } while(0)

#define G2D_SET_LAYER_CON_ROTATE(n, rot)                                          \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_CCW_ROTATE_MASK;                          \
    REG_G2D_LAYER_CON(n) |= (rot);                                                \
  } while(0)



static inline void hal_g2d_deinit(void) { PCTL_PowerDown(PD_G2D); }
static inline void hal_g2d_init(void) { PCTL_PowerUp(PD_G2D); }

/* clang-format on */
#ifdef __cplusplus
}
#endif
#endif // _HAL_G2D_H_