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

#ifndef GPRS_TCP_H
#define GPRS_TCP_H

#include <Arduino.h>
#include <Client.h>

#define DEBUG_TCP
//Serial.printf

class gprsClient : public Client
{
private:
  char m_id;
  int m_socket;
  uint32_t m_ip;
  bool m_peeked;
  unsigned char m_peek;

public:
  gprsClient()
  {
    m_id = 0;
    m_socket = -1;
    m_peeked = false;
    m_peek = 0;
  }

  gprsClient(int contextID)
  {
    m_id = contextID;
    m_socket = -1;
    m_peeked = false;
    m_peek = 0;
  }

  gprsClient(int socket, int contextID)
  {
    m_id = contextID;
    m_socket = socket;
    m_peeked = false;
    m_peek = 0;
  }

  ~gprsClient() { stop(); }

  virtual int connect(uint32_t ip, uint16_t port)
  {
    if (m_socket > -1)
      return true; // already open
    if ((m_socket = Ql_SOC_Create(m_id, SOC_TYPE_TCP)) < 0)
    {
      DEBUG_TCP("[ERROR] TCP Ql_SOC_Create() failed!\n");
      return false; // error
    }
    int res = Ql_SOC_ConnectEx(m_socket, (uint32_t)Ql_convertIP(ip), port, true); // connect is blocked, socket is SOC_NBIO
    if (SOC_SUCCESS == res)
    {
      m_ip = ip;
    }
    else
    {
      m_ip = 0;
      DEBUG_TCP("[ERROR] TCP Ql_SOC_ConnectEx( %d )\n", res);
    }
    return SOC_SUCCESS == res;
  }

  virtual int connect(IPAddress IP, uint16_t port)
  {
    return connect((uint32_t)IP, port);
  }

  virtual int connect(const char *host, uint16_t port)
  {
    uint32_t ip;
    IPAddress IP;
    if (getHostByName(host, IP))
      return connect(IP, port);
    DEBUG_TCP("[ERROR] TCP could not get host from dns\n");
    return false;
  }

  int socket() { return m_socket; }

  uint32_t ip() { return m_ip; }

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
        *buf++ = m_peek; // add to buffer
        size -= 1;
        cnt = 1;
        if (0 == size)
          return 1;
      }
      res = Ql_SOC_Recv(m_socket, buf, size); // socket is SOC_NBIO
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
      return (1 == Ql_SOC_Recv(m_socket, &data, 1)) ? data : -1; // socket is SOC_NBIO
    }
    return -1; // EOF
  }

  virtual size_t write(unsigned char data)
  {
    return (m_socket > -1) && (1 == Ql_SOC_Send(m_socket, &data, 1));
  }

  virtual size_t write(const unsigned char *buffer, size_t size)
  {
    return (m_socket > -1) && (buffer) && (size) && (size == Ql_SOC_Send(m_socket, (u8 *)buffer, size));
  }

  virtual int available()
  {
    if (m_socket > -1)
    {
#ifdef USE_API
      if (HAL)
      {
        short val;
        int res = HAL->soc_getsockopt(m_socket, SOC_NREAD, &val, sizeof(short));
        if (0 == res)
        {
          return val + m_peeked;
        }
        else
        {
          DEBUG_TCP("[ERROR] TCP soc_getsockopt()\n");
        }
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
    return -1; // EOF
  }

  virtual void flush()
  {
    while (available() > 0)
      read();
  }

  virtual unsigned char connected()
  {
    return m_socket > -1;
  }

  virtual operator bool()
  {
    return connected();
  }

  using Print::write;
};

#endif // GPRS_TCP_H