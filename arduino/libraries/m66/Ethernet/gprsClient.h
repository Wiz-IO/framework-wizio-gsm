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

#ifndef _GPRS_TCP_H_
#define _GPRS_TCP_H_

#include <Arduino.h>
#include <Client.h>

class gprsClient : public Client
{

private:
  char m_id;
  unsigned char m_peek;
  bool m_peeked;
  int m_socket;
  bool m_externalSocket;

public:
  gprsClient();
  gprsClient(int contextID);
  gprsClient(int soc, int contextID);
  ~gprsClient();

  int connect(unsigned int ip, unsigned short port);
  virtual int connect(IPAddress ip, unsigned short port);
  virtual int connect(const char *host, unsigned short port);
  virtual void stop();

  virtual size_t write(unsigned char);
  virtual size_t write(const unsigned char *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(unsigned char *buf, size_t size);
  virtual int peek();
  virtual void flush() {}

  virtual unsigned char connected();
  virtual operator bool();

  friend class WiFiServer;
  using Print::write;
};

#endif // _GPRS_TCP_H_