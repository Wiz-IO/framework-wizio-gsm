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
#include <variant.h>

void pinMode(uint8_t pin, uint8_t mode)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
  {
    if (mode == CLOSE)
    {
      Ql_GPIO_Uninit(n->quectel);
      n->quectel = PINNAME_END;
    }
    else
    {
      Enum_PinPullSel pull = PINPULLSEL_DISABLE;
      Enum_PinDirection dir = PINDIRECTION_IN;
      Enum_PinLevel level = PINLEVEL_LOW;
      if (mode & INPUT_PULLUP)
        pull = PINPULLSEL_PULLUP;
      if (mode & INPUT_PULLDOWN)
        pull = PINPULLSEL_PULLDOWN;
      if ((mode & OUTPUT) || (mode & OUTPUT_LO) || (mode & OUTPUT_HI))
        dir = PINDIRECTION_OUT;
      if (mode & OUTPUT_HI)
        level = PINLEVEL_HIGH;
      Ql_GPIO_Init(n->quectel, dir, level, pull);
    }
  }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
    Ql_GPIO_SetLevel(n->quectel, (Enum_PinLevel)val & 1);
}

int digitalRead(uint8_t pin)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
    return Ql_GPIO_GetLevel(n->quectel);
  return -1;
}

void led_blink(int led, int t)
{
  digitalWrite(led, 1);
  delay(t);
  digitalWrite(led, 0);
  delay(t);
}