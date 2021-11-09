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

#define PWM_MAX 8193

static u32 adc_value = 0;

void analogReference(uint8_t mode) {}

void analogWrite(uint8_t pin, int val)
{
  if (PWM0 == pin)
    Ql_PWM_Output((Enum_PinName)pin, val);
}

void analogClose(uint8_t pin)
{
  switch (pin)
  {
  case ADC0:
    Ql_ADC_Sampling((Enum_ADCPin)(pin - PINNAME_END), false);
    adc_value = 0;
    break;
  case PWM0:
    Ql_PWM_Uninit((Enum_PinName)pin);
  }
}

int analogRead(uint8_t pin)
{
  arduinoProcessMessages(1);
  return adc_value; // 0 ~ 2800mV
}

static void onADC(Enum_ADCPin adcPin, u32 adcValue, void *customParam)
{ // every second
  adc_value = adcValue;
}

void analogOpen(uint8_t pin, /* val, src, div */...)
{
  switch (pin)
  {
  case ADC0:
  {
    Enum_ADCPin aPin = (Enum_ADCPin)(pin - PINNAME_END);
    Ql_ADC_Register(aPin, onADC, NULL);
    Ql_ADC_Init(aPin, 5, 200); // minimal values
    Ql_ADC_Sampling(aPin, true);
  }
  break;

  case PWM0:
  {
    va_list list;
    va_start(list, pin);
    uint32_t val = va_arg(list, uint32_t);
    uint32_t pwmSrcClk = va_arg(list, uint32_t);
    uint32_t pwmDiv = va_arg(list, uint32_t);
    Ql_GPIO_Uninit((Enum_PinName)pin);
    uint32_t PWM_lowPulseNum = PWM_MAX / 2;
    uint32_t PWM_highPulseNum = PWM_MAX / 2;
    Ql_PWM_Init((Enum_PinName)pin, (Enum_PwmSource)pwmSrcClk, (Enum_PwmSourceDiv)pwmDiv, PWM_lowPulseNum, PWM_highPulseNum);
    Ql_PWM_Output((Enum_PinName)pin, val);
  }
  break;
  }
}
