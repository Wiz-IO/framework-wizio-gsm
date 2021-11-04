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

#define BUFFER_LENGTH SERIAL_BUFFER_SIZE

class TwoWire : public Stream
{
public:
  TwoWire(uint8_t i2c_port);

  void begin();
  void end();

  void setClock(uint32_t Hz);

  void beginTransmission(uint8_t);
  uint8_t endTransmission(bool stopBit);
  uint8_t endTransmission(void);

  uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
  uint8_t requestFrom(uint8_t address, size_t quantity);

  virtual int available(void);
  virtual int read(void);
  virtual int peek(void);
  virtual void flush(void){}

  virtual size_t write(uint8_t);

  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  using Print::write;

  void onService(void){};
  void onReceive(void (*)(int)){};
  void onRequest(void (*)(void)){};

private:
  u32 i2c_port, i2c_speed;
  Enum_PinName scl, sda;

  bool transmissionBegun;
  uint8_t slaveAddress;

  RingBuffer rx, tx;

  void (*onRequestCallback)(void){};
  void (*onReceiveCallback)(int){};
};

extern TwoWire Wire;

#endif