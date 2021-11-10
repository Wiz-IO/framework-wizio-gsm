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

#ifndef _RILCLASS_H
#define _RILCLASS_H

#include <Arduino.h>
extern HardwareSerial Virtual1;

#define RIL_TIMEOUT 100

class RilClass
{
public:
  RilClass(HardwareSerial &uart);

  void begin()
  {
    _uart->end();
    _uart->begin(0);
  }
  void end() { _uart->end(); }

  bool AT();
  bool ATI(String *s);
  bool GSN(String *s);
  bool CGMM(String *s);
  bool CGMI(String *s);

  bool single(const char *command);

  size_t write(uint8_t c);
  size_t write(const uint8_t *, size_t);

  void send(const char *command);
  void send(const String &command) { send(command.c_str()); }
  void sendf(const char *fmt, ...);

  int waitForResponse(unsigned long timeout = RIL_TIMEOUT, String *responseDataStorage = NULL);
  int waitForPrompt(unsigned long timeout = RIL_TIMEOUT);
  int ready();
  void process();
  void setResponseDataStorage(String *responseDataStorage);

private:
  HardwareSerial *_uart;
  enum
  {
    AT_COMMAND_IDLE,
    AT_RECEIVING_RESPONSE
  } _atCommandState;
  int _ready;
  String _buffer;
  String *_responseDataStorage;
};

extern RilClass Ril;

#endif //_RIL_CLASS_H