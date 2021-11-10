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

#include "gprsClient.h"

#define DEBUG_TCP 
//Serial.printf

gprsClient::gprsClient()
{
    m_id = 0;
    m_peeked = 0;
    m_peek = 0;
    m_externalSocket = true;
    m_socket = -1;
}

gprsClient::gprsClient(int contextID)

{
    m_id = contextID;
    m_peeked = 0;
    m_peek = 0;
    m_externalSocket = 0;
    m_socket = -1;
}

gprsClient::gprsClient(int socket, int contextID)
{
    m_id = contextID;
    m_peeked = 0;
    m_peek = 0;
    m_externalSocket = 1;
    m_socket = socket;
}

gprsClient::~gprsClient()
{
    if (!m_externalSocket)
        stop();
}

int gprsClient::connect(unsigned int ip, unsigned short port)
{
    if (m_socket)
        stop();
    m_socket = Ql_SOC_Create(m_id, SOC_TYPE_TCP);
    m_externalSocket = true;
    if (m_socket < 0)
    {
        DEBUG_TCP("[ERROR] Ql_SOC_Create() failed!\n");
        return false;
    }
    int res = Ql_SOC_ConnectEx(m_socket, (unsigned int)Ql_convertIP(ip), port, true); // blocked
    return res == SOC_SUCCESS;                                                        // true = OK
}

int gprsClient::connect(IPAddress IP, unsigned short port)
{
    return connect((uint32_t)IP, port);
}

int gprsClient::connect(const char *host, unsigned short port)
{
    uint32_t ip;
    IPAddress IP;
    if (getHostByName(host, IP))
        return connect(IP, port);
    DEBUG_TCP("[ERROR] could not get host from dns\n");
    return false;
}

size_t gprsClient::write(unsigned char b)
{
    if (m_socket == -1)
        return 0;
    int res = Ql_SOC_Send(m_socket, &b, 1);
    return res > -1 ? 1 : 0;
}

size_t gprsClient::write(const unsigned char *buffer, size_t size)
{
    if (m_socket == -1 || NULL == buffer || size == 0)
        return 0;
    int res = Ql_SOC_Send(m_socket, (u8 *)buffer, size);
    return res > -1 ? res : 0;
}

int gprsClient::read()
{
    if (m_socket == -1)
        return -1;
    if (m_peeked)
    {
        m_peeked = false;
        return m_peek;
    }
    unsigned char b;
    return (1 == Ql_SOC_Recv(m_socket, &b, 1)) ? b : -1; // no wait
}

int gprsClient::read(unsigned char *buffer, size_t size)
{
    if (m_socket == -1 || NULL == buffer || size == 0)
        return 0;
    unsigned char *p = buffer;
    if (m_peeked)
    {
        m_peeked = false;
        *p++ = m_peek;
        size -= 1;
        if (0 == size)
            return 1;
    }
    int res = Ql_SOC_Recv(m_socket, p, size); // no wait
    return res > -1 ? res : -1;
}

int gprsClient::available()
{
    return m_socket > -1 && peek() > -1; // without soc_getsockopt
}

int gprsClient::peek()
{
    if (m_socket > -1)
    {
        if (m_peeked)
        {
            return m_peek;
        }
        m_peeked = 1 == Ql_SOC_Recv(m_socket, &m_peek, 1); // no wait
        if (m_peeked)
        {
            return m_peek;
        }
    }
    return -1;
}

void gprsClient::stop()
{
    if (m_socket > -1)
    {
        Ql_SOC_Close(m_socket);
        m_socket = -1;
    }
}

unsigned char gprsClient::connected()
{
    return m_socket > -1;
}

gprsClient::operator bool()
{
    return connected();
}