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