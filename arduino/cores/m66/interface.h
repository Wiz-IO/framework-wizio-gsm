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

#ifndef INTERFACE_H_
#define INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <_ansi.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "ql_common.h"
#include "ql_type.h"
#include "ql_system.h"
#include "ql_memory.h"
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_uart.h"
#include "ql_gpio.h"
#include "ql_adc.h"
#include "ql_pwm.h"
#include "ql_spi.h"
#include "ql_iic.h"
#include "ql_eint.h"
#include "ql_power.h"
#include "ql_time.h"
#include "ql_timer.h"
#include "ql_clock.h"
#include "ql_fota.h"
#include "ql_fs.h"
#include "ql_gprs.h"
#include "ql_socket.h"
#include "ql_wtd.h"
#include "nema_pro.h"

#include "ril.h"
#include "ril_system.h"
#include "ril_util.h"
#include "ril_sim.h"
#include "ril_telephony.h"
#include "ril_sms.h"
#include "lib_ril_sms.h"
#include "ril_network.h"
#include "ril_location.h"
#include "ril_http.h"
#include "ril_ftp.h"
#include "ril_dtmf.h"
#include "ril_bluetooth.h"
#include "ril_audio.h"
#include "ril_alarm.h"

#define atoi Ql_atoi
#define atof Ql_atof
#define memcmp Ql_memcmp
#define memmove Ql_memmove
#define strcpy Ql_strcpy
#define strncpy Ql_strncpy
#define strcat Ql_strcat
#define strncat Ql_strncat
#define strcmp Ql_strcmp
#define strncmp Ql_strncmp
#define strchr Ql_strchr
#define strlen Ql_strlen
#define strstr Ql_strstr
#define toupper Ql_toupper
#define tolower Ql_tolower
#define sprintf Ql_sprintf
#define snprintf Ql_snprintf
#define sscanf Ql_sscanf
#define malloc Ql_MEM_Alloc
#define free Ql_MEM_Free

#include "hal_ustimer.h"

#define strtol api_strtol
#define api_strtoul strtoul
    ////#define vsnprintf wiz__vsnprintf
    ////#define rand api_rand
    ////#define srand api_srand

#define ARDUINO_TASK_ID 3 /* arduino_task_id */
#define MSG_PROCESS_MESSAGES 0x100
    void arduinoProcessMessages(unsigned int wait);

    void __libc_init_array(void);
    void __libc_fini_array(void);

#define CLOSE (1 << 7)

    unsigned int seconds(void);
    unsigned int millis(void);
    unsigned int micros(void);
    void delay(unsigned int);
    void delayMicroseconds(unsigned int us);

    extern char *utoa(unsigned int value, char *buffer, int radix);
    static inline char *ltoa(long value, char *result, int base) { return utoa(value, result, base); }
    static inline char *ultoa(unsigned long value, char *result, int base) { return utoa(value, result, base); }

    uint32_t clockCyclesPerMicrosecond(void);
    uint32_t clockCyclesToMicroseconds(uint32_t a);
    uint32_t microsecondsToClockCycles(uint32_t a);

    unsigned int *Ql_convertIP(unsigned int ip);
    int Ql_inet_aton(const char *cp, uint32_t *ip);

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 256
#endif

#define digitalPinToPort(p) -1
#define digitalPinToBitMask(p) -1
#define digitalPinToClkid(p) -1
#define digitalPinSPIAvailiable(p) -1
#define digitalPinToSPIDevice(p) -1
#define digitalPinToSPIClockId(p) -1
#define portOutputRegister(port) -1
#define portInputRegister(port) -1

    // for SPI
    __attribute__((always_inline)) static inline uint32_t __RBIT(uint32_t value)
    {
        uint32_t result;
        int32_t s = 4 /*sizeof(v)*/ * 8 - 1; /* extra shift needed at end */
        result = value;                      /* r will be reversed bits of v; first get LSB of v */
        for (value >>= 1U; value; value >>= 1U)
        {
            result <<= 1U;
            result |= value & 1U;
            s--;
        }
        result <<= s; /* shift when v's highest bits are zero */
        return (result);
    }

    __attribute__((always_inline)) static inline uint32_t __REV(uint32_t x)
    {
        return __builtin_bswap32(x);
    }

    __attribute__((always_inline)) static inline uint16_t __REV16(uint16_t x)
    {
        return __builtin_bswap16(x);
    }

#ifdef __cplusplus
} // extern "C"

void arduinoSetWait(u32 wait);
void delayEx(unsigned int ms);

#endif //__cplusplus

#endif /* INTERFACE_H_ */