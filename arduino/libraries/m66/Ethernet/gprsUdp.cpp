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

#include "gprsUdp.h"

#define DEBUG_UDP 
//Serial.printf

#undef write
#undef read

gprsUDP::gprsUDP(int context)
    : socket(-1),
      server_port(0),
      remote_port(0),
      tx_buffer(0),
      tx_buffer_len(0),
      rx_buffer(0),
      id(context)
{
}

gprsUDP::gprsUDP()
    : socket(-1),
      server_port(0),
      remote_port(0),
      tx_buffer(0),
      tx_buffer_len(0),
      rx_buffer(0),
      id(0)
{
}

gprsUDP::~gprsUDP()
{
    stop();
}

uint8_t gprsUDP::begin(IPAddress IP, uint16_t port)
{
    stop();
    server_port = port;
    tx_buffer = new char[UDP_BUFFER_SIZE];
    if (!tx_buffer)
    {
        DEBUG_UDP("[ERROR] could not create tx buffer");
        return 0;
    }
    if ((socket = Ql_SOC_Create(id, SOC_TYPE_UDP)) < 0)
    {
        DEBUG_UDP("[ERROR] could not create socket: %d", socket);
        return 0;
    }
    int res;
    if ((res = Ql_SOC_Bind(socket, port)) < 0)
    {
        DEBUG_UDP("[ERROR] could not bind socket: %d", res);
        stop();
        return 0;
    }
    return 0;
}

uint8_t gprsUDP::begin(uint16_t port)
{
    return begin(IPAddress((uint32_t)0), port);
}

void gprsUDP::stop()
{
    if (tx_buffer)
    {
        delete[] tx_buffer;
        tx_buffer = NULL;
    }
    tx_buffer_len = 0;
    if (rx_buffer)
    {
        cbuf *b = rx_buffer;
        rx_buffer = NULL;
        delete b;
    }
    if (socket == -1)
        return;
    if (multicast_ip != 0)
    {
    }
    Ql_SOC_Close(socket);
    socket = -1;
}

int gprsUDP::beginPacket()
{
    if (!remote_port)
        return 0;
    if (!tx_buffer) // allocate tx_buffer if is necessary
    {
        tx_buffer = new char[UDP_BUFFER_SIZE];
        if (!tx_buffer)
        {
            DEBUG_UDP("[ERROR] could not create tx buffer");
            return 0;
        }
    }
    tx_buffer_len = 0;
    if (socket != -1)
        return 1; //is already open
    if ((socket = Ql_SOC_Create(id, SOC_TYPE_UDP)) < 0)
    {
        DEBUG_UDP("[ERROR] could not create socket: %d", socket);
        return 0;
    }
    return -1;
}

int gprsUDP::beginPacket(IPAddress IP, uint16_t port)
{
    remote_ip = IP;
    remote_port = port;
    return beginPacket();
}

int gprsUDP::beginPacket(const char *host, uint16_t port)
{
    IPAddress IP;
    if (getHostByName(host, IP))
        return beginPacket(IP, port);
    DEBUG_UDP("[ERROR] could not get host from dns");
    return 0;
}

int gprsUDP::endPacket()
{
    int sent = Ql_SOC_SendTo(socket, (unsigned char *)tx_buffer, tx_buffer_len, (unsigned int)Ql_convertIP((uint32_t)remote_ip), remote_port);
    if (sent < 0)
    {
        DEBUG_UDP("[ERROR] could not send data: %d", sent);
        return 0;
    }
    return 1;
}

size_t gprsUDP::write(uint8_t data)
{
    if (tx_buffer_len == UDP_BUFFER_SIZE)
    {
        endPacket();
        tx_buffer_len = 0;
    }
    tx_buffer[tx_buffer_len++] = data;
    return 1;
}

size_t gprsUDP::write(const uint8_t *buffer, size_t size)
{
    size_t i;
    for (i = 0; i < size; i++)
        write(buffer[i]);
    return i;
}

int gprsUDP::parsePacket()
{
    if (rx_buffer)
        return 0;
    int len;
    unsigned int other_ip;
    unsigned short other_port;
    char *buf = new char[UDP_BUFFER_SIZE];
    if (!buf)
        return 0;
    len = Ql_SOC_RecvFrom(socket, (unsigned char *)buf, UDP_BUFFER_SIZE, &other_ip, &other_port); // *?
    if (len < 0)
    {
        delete[] buf;
        return 0;
    }
    remote_ip = IPAddress((uint32_t)other_ip); // *?
    remote_port = other_port;
    if (len > 0)
    {
        rx_buffer = new cbuf(len);
        rx_buffer->write(buf, len);
    }
    delete[] buf;
    return len;
}

int gprsUDP::available()
{
    if (!rx_buffer)
        return 0;
    return rx_buffer->available();
}

int gprsUDP::read()
{
    if (!rx_buffer)
        return -1;
    int out = rx_buffer->read();
    if (!rx_buffer->available())
    {
        cbuf *b = rx_buffer;
        rx_buffer = 0;
        delete b;
    }
    return out;
}

int gprsUDP::read(unsigned char *buffer, size_t len)
{
    return read((char *)buffer, len);
}

int gprsUDP::read(char *buffer, size_t len)
{
    if (!rx_buffer)
        return 0;
    int out = rx_buffer->read(buffer, len);
    if (!rx_buffer->available())
    {
        cbuf *b = rx_buffer;
        rx_buffer = 0;
        delete b;
    }
    return out;
}

int gprsUDP::peek()
{
    if (!rx_buffer)
        return -1;
    return rx_buffer->peek();
}

void gprsUDP::flush()
{
    if (!rx_buffer)
        return;
    cbuf *b = rx_buffer;
    rx_buffer = 0;
    delete b;
}

IPAddress gprsUDP::remoteIP()
{
    return remote_ip;
}

uint16_t gprsUDP::remotePort()
{
    return remote_port;
}
