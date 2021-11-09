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

#ifndef __WIRE_H__
#define __WIRE_H__

#include "interface.h"
#include "Stream.h"
#include "variant.h"
#include "RingBuffer.h"
#include "hal_i2c.h"
#include "hal_gpio.h"

#define BUFFER_LENGTH SERIAL_BUFFER_SIZE

class TwoWire : public Stream
{
public:
  /* default pins */
  TwoWire(uint8_t port)
  {
    default_init(port);
  }

  /* pins is valid only SW */
  TwoWire(uint8_t port, Enum_PinName SCL, Enum_PinName SDA)
  {
    default_init(port);
    setPins(SCL, SDA);
  }

  void begin();
  void end();

  /* !!! BEFORE begin(), 7 bits !!! */
  void setAddress(uint8_t address) { slaveAddress = address; }

  /* !!! BEFORE begin(), pins is valid only SW */
  void setPins(Enum_PinName SCL, Enum_PinName SDA)
  {
    scl = SCL;
    sda = SDA;
  }

  void setPins(uint8_t SCL, uint8_t SDA)
  {
    PinDescription *n = getArduinoPin(SCL);
    if (n)
      scl = (Enum_PinName)n->device;

    n = getArduinoPin(SDA);
    if (n)
      sda = (Enum_PinName)n->device;
  }

  void setClock(uint32_t Hz);

  void beginTransmission(uint8_t address)
  {
    tx.clear();
    slaveAddress = address;
    transmissionBegun = true;
  }
  uint8_t endTransmission(bool stopBit);
  uint8_t endTransmission() { return endTransmission(true); }

  uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
  uint8_t requestFrom(uint8_t address, size_t quantity) { return requestFrom(address, quantity, true); }

  virtual int available(void) { return rx.available(); }
  virtual int peek(void) { return rx.peek(); }
  virtual void flush(void) {}

  virtual int read(void) { return rx.read_char(); }

  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  virtual size_t write(uint8_t ucData)
  {
    if (!transmissionBegun || tx.isFull())
      return 0;
    tx.store_char(ucData);
    return 1;
  }
  using Print::write;

  void onService(void){};
  void onReceive(void (*)(int)){};
  void onRequest(void (*)(void)){};

private:
  u32 i2c_port, i2c_speed;
  Enum_PinName scl, sda;
  uint8_t slaveAddress;
  bool transmissionBegun;
  void default_init(uint8_t port);
  uint32_t QL_Write(uint8_t *buf, uint32_t len);
  uint32_t QL_Read(uint8_t *buf, uint32_t len);

  i2c_context_t *ctx;

  RingBuffer rx, tx;
};

extern TwoWire Wire;

#endif