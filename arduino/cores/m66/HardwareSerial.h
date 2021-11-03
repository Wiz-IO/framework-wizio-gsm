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

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <Stream.h>
#include <interface.h> 

#define SERIAL_RX_BUFFER_SIZE 256
typedef uint8_t buffer_index_t;

class HardwareSerial : public Stream
{
protected:
  static void callback(Enum_SerialPort port, Enum_UARTEventType event, bool pinLevel, void *serial);
  Enum_SerialPort port;

  volatile buffer_index_t _rx_buffer_head;
  volatile buffer_index_t _rx_buffer_tail;
  unsigned _rx_buffer[SERIAL_RX_BUFFER_SIZE];  

public:
  int save(uint8_t c);
  HardwareSerial(uint32_t id);
  void begin(unsigned long baud, void *user);
  void begin(unsigned long baud);
  void end();
  size_t setRxBufferSize(size_t new_size);
  void clear(int who = -1); //ALL, x01=rx, x10=tx 
  virtual int available(void);
  virtual int peek(void);
  virtual int read(void);
  virtual void flush(void){};
  virtual size_t write(uint8_t);
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  size_t write(const char *buf);
  size_t write(const uint8_t *buf, size_t size);    
  using Print::write;
  operator bool() { return true; }
};

#endif
