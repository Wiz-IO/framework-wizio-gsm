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

#include <Arduino.h>

#define DEBUG_DNS

static int dns_Code;
static IPAddress dns_IP;

static void Callback_GetIpByName(u8 contexId, u8 requestId, s32 errCode, u32 ipAddrCnt, u32 *ipAddr)
{
    DEBUG_DNS("CALLBACK = %d, %d", errCode, ipAddrCnt);
    dns_Code = errCode;
    if (errCode == SOC_SUCCESS && ipAddrCnt > 0)
    {
        dns_IP = (uint32_t)*ipAddr;
    }
}

bool getHostByName(const char *host, IPAddress &IP, uint32_t timeout)
{
    uint32_t ip;
    if (Ql_inet_aton(host, &ip))
    {
        IP = (uint32_t)ip;
        return true;
    }
    IP = (uint32_t)0;
    if (host)
    {
        dns_Code = -1;
        int r = Ql_IpHelper_GetIPByHostName(0, 0, (u8 *)host, Callback_GetIpByName);
        uint32_t start = millis();
        while (millis() - start < timeout)
        {
            arduinoProcessMessages(1);
            if (0 == dns_Code)
            {
                IP = dns_IP;
                return true;
            }
        }
    }
    return false;
}