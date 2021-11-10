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

#include <variant.h>
#include "hal_ustimer.h"

inline unsigned int millis() { return Ql_GetMsSincePwrOn(); }

inline unsigned int seconds(void) { return millis() / 1000; }

inline unsigned int micros() { return USC_GetCurrentTicks(); }

inline void delay(unsigned int ms)
{
  //Ql_Sleep(ms);
  delayEx(ms);
}

inline void delayMicroseconds(unsigned int us) { delay_u(us); }

///////////////////////////////////////////////////////////

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))

PinDescription *getArduinoPin(uint8_t arduinoPin)
{
  for (int i = 0; i < ARRAYLEN(pinsMap); i++)
    if (pinsMap[i].arduino == arduinoPin)
      return &pinsMap[i];
  return NULL;
}

PinDescription *getDevicePin(uint8_t devicePin)
{
  for (int i = 0; i < ARRAYLEN(pinsMap); i++)
    if (pinsMap[i].quectel == devicePin)
      return &pinsMap[i];
  return NULL;
}

static void eint_callback(Enum_PinName eintPinName, Enum_PinLevel pinLevel, void *user)
{
  Enum_PinName pin = (Enum_PinName)user;
  PinDescription *n = getDevicePin(pin);
  if (n && n->eint)
  {
    Ql_EINT_Mask(pin);
    eint_callback_t cb = (eint_callback_t)n->eint;
    cb(Ql_EINT_GetLevel(n->quectel));
    Ql_EINT_Unmask(pin);
  }
}

void eintMode(uint8_t pin, Enum_EintType type, eint_callback_t cb, uint32_t hwDebounce, uint32_t swDebounce, bool automask)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
  {
    if (type == EINT_CLOSE)
    {
      Ql_EINT_Uninit(n->quectel);
      n->eint = NULL;
    }
    else if (cb)
    {
      n->eint = cb;
      Ql_EINT_RegisterFast(n->quectel, eint_callback, (void *)((int)n->quectel));
      Ql_EINT_Init(n->quectel, type, hwDebounce, swDebounce, automask);
    }
  }
}

///////////////////////////////////////////////////////////

static int _irq_;

void interrupts(void)
{
  //TODO
}

void noInterrupts(void)
{
  //TODO
}