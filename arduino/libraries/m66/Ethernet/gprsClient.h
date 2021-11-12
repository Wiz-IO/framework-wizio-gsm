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

#define DEBUG_TCP
//Serial.printf

class gprsClient : public Client
{

private:
  char m_id;
  int m_socket;
  bool m_peeked;
  unsigned char m_peek;

public:
  gprsClient()
  {
    m_id = 0;
    m_socket = -1;
    m_peeked = 0;
    m_peek = 0;
  }

  gprsClient(int contextID)
  {
    m_id = contextID;
    m_socket = -1;
    m_peeked = 0;
    m_peek = 0;
  }

  gprsClient(int socket, int contextID)
  {
    m_id = contextID;
    m_socket = socket;
    m_peeked = 0;
    m_peek = 0;
  }

  ~gprsClient() { stop(); }

  virtual int connect(unsigned int ip, unsigned short port)
  {
    if (m_socket > -1)
      stop();
    if ((m_socket = Ql_SOC_Create(m_id, SOC_TYPE_TCP)) < 0)
    {
      //DEBUG_TCP("[ERROR] Ql_SOC_Create() failed!\n");
      return false;
    }
    int res = Ql_SOC_ConnectEx(m_socket, (unsigned int)Ql_convertIP(ip), port, true); // blocked
    return res == SOC_SUCCESS;                                                        // true = OK
  }

  virtual int connect(IPAddress IP, unsigned short port) { return connect((uint32_t)IP, port); }

  virtual int connect(const char *host, unsigned short port)
  {
    uint32_t ip;
    IPAddress IP;
    if (getHostByName(host, IP))
      return connect(IP, port);
    //DEBUG_TCP("[ERROR] could not get host from dns\n");
    return false;
  }

  int socket() { return m_socket; }

  virtual void stop()
  {
    if (m_socket > -1)
    {
      Ql_SOC_Close(m_socket);
      m_socket = -1;
    }
  }

  virtual int read(unsigned char *buffer, size_t size)
  {
    int res, cnt = 0;
    unsigned char *buf = buffer;
    if (m_socket > -1 && buffer && size)
    {
      if (m_peeked)
      {
        m_peeked = false;
        *buf++ = m_peek; // add to buffre
        size -= 1;
        cnt = 1;
        if (0 == size)
          return 1;
      }
      res = Ql_SOC_Recv(m_socket, buf, size); // no wait
      if (cnt)
        return res > -1 ? res + cnt : -1;
      else
        return res > 0 ? res : -1;
    }
    return -1; // EOF
  }

  virtual int read()
  {
    if (m_socket > -1)
    {
      if (m_peeked)
      {
        m_peeked = false;
        return m_peek;
      }
      unsigned char data;
      return (1 == Ql_SOC_Recv(m_socket, &data, 1)) ? data : -1; // no wait
    }
    return -1; // EOF
  }

  virtual size_t write(unsigned char data) { return (m_socket > -1) && (1 == Ql_SOC_Send(m_socket, &data, 1)); }

  virtual size_t write(const unsigned char *buffer, size_t size) { return (m_socket > -1) && (buffer) && (size) && (size == Ql_SOC_Send(m_socket, (u8 *)buffer, size)); }

  virtual int available()
  {
    if (m_socket > -1)
    {
#ifdef USE_API
      if (HAL)
      {
        short val;
        if (0 == HAL->soc_getsockopt(m_socket, SOC_NREAD, &val, sizeof(short)))
          return val + m_peeked;
        return m_peeked;
      }
#endif
      peek(); // without soc_getsockopt
      return m_peeked;
    }
    return 0;
  }

  virtual int peek()
  {
    if (m_socket > -1)
    {
      if (m_peeked)
        return m_peek;

      if ((m_peeked = (1 == Ql_SOC_Recv(m_socket, &m_peek, 1))))
        return m_peek;
    }
    return -1;
  }

  virtual void flush() {}

  virtual unsigned char connected() { return m_socket > -1; }

  virtual operator bool() { return connected(); }

  friend class gprsClentSecure;
  
  using Print::write;
};

#endif // _GPRS_TCP_H_