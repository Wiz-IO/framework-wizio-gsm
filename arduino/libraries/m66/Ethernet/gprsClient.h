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
  RingBuffer m_rx;

  int internal_read()
  {
    while (m_rx.availableForStore() > 0)
    {
      uint8_t data;
      if (1 != Ql_SOC_Recv(m_socket, &data, 1))
        break;
      m_rx.store_char(data);
    }
    return m_rx.available();
  }

public:
  gprsClient()
  {
    m_id = 0;
    m_socket = -1;
  }

  gprsClient(int contextID)
  {
    m_id = contextID;
    m_socket = -1;
  }

  gprsClient(int socket, int contextID)
  {
    m_id = contextID;
    m_socket = socket;
  }

  ~gprsClient() { stop(); }

  virtual unsigned char connected()
  {
    return m_socket > -1;
  }

  virtual int connect(uint32_t ip, uint16_t port)
  {
    if (m_socket > -1)
      return true; // already open
    m_rx.clear();
    if ((m_socket = Ql_SOC_Create(m_id, SOC_TYPE_TCP)) < 0)
    {
      DEBUG_TCP("[ERROR] TCP Ql_SOC_Create() failed!\n");
      return false; // error
    }
    int res = Ql_SOC_ConnectEx(m_socket, (uint32_t)Ql_convertIP(ip), port, true); // connect is blocked, socket is SOC_NBIO
    if (SOC_SUCCESS != res)
    {
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

  virtual void stop()
  {
    if (m_socket > -1)
    {
      Ql_SOC_Close(m_socket);
      m_socket = -1;
    }
  }

  virtual size_t write(unsigned char data)
  {
    return (m_socket > -1) && (1 == Ql_SOC_Send(m_socket, &data, 1));
  }

  virtual size_t write(const unsigned char *buffer, size_t size)
  {
    return (m_socket > -1) && (buffer) && (size) && (size == Ql_SOC_Send(m_socket, (u8 *)buffer, size));
  }

  virtual int read(unsigned char *buffer, size_t size)
  {
    int readed = 0;
    if ((m_socket > -1) && buffer && size)
    {
      uint8_t *buf = buffer;
      while (m_rx.available() > 0 && size--)
      {
        *buf++ = m_rx.read_char();
        readed++;
      }
      while (size--)
      {
        unsigned char data;
        if (1 == Ql_SOC_Recv(m_socket, &data, 1))
        {
          *buf++ = data;
          readed++;
        }
      }
    }
    return readed;
  }

  virtual int read()
  {
    if (m_socket > -1)
    {
      if (m_rx.available() > 0)
        return m_rx.read_char();
      unsigned char data;
      return (1 == Ql_SOC_Recv(m_socket, &data, 1)) ? data : EOF;
    }
    return EOF;
  }

  virtual int available() // max RingBuffer[256]
  {
    if (m_socket > -1)
    {
      int res = m_rx.available();
      if (res > 0)
        return res;
      return internal_read();
    }
    return 0;
  }

  virtual int peek()
  {
    if ((m_socket > -1))
    {
      if (m_rx.available() > 0)
        return m_rx.peek();
      if (internal_read() > 0)
        return m_rx.peek();
    }
    return EOF;
  }

  virtual void flush()
  {
    if ((m_socket > -1))
    {
      while (available())
        read();
      m_rx.clear();
    }
  }

    operator bool() { return connected(); }
    bool operator==(const bool value) { return bool() == value; }
    bool operator!=(const bool value) { return bool() != value; }
    bool operator==(const gprsClient &);
    bool operator!=(const gprsClient &r) { return !this->operator==(r); }

  using Print::write;
};

#endif // GPRS_TCP_H