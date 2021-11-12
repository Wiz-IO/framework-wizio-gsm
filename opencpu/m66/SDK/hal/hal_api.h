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

#ifndef HAL_API_H_
#define HAL_API_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>

    typedef struct
    {
        char *FW_VER;
        char *FW_VER_ADDR;

        int (*soc_setsockopt)(unsigned char, unsigned int, void *, unsigned char);
        int (*soc_getsockopt)(unsigned char, unsigned int, void *, unsigned char);

        int  (*SSL_get_method)(int);
        void * /*ctx*/ (*SSL_CTX_new)(int method);
        int (*SSL_CTX_free)(void *ctx);
        int (*SSL_CTX_use_PrivateKey_file)(void *ctx, const char *file_name, int file_type);
        int (*SSL_CTX_use_certificate_chain_file)(void *ctx, const char *file_name);

        void * /*ssl*/ (*SSL_new)(void *ctx);
        int (*SSL_free)(void *ssl);
        int (*SSL_set_fd)(void *ssl, int soc);
        int (*SSL_connect)(void *ssl);
        int (*SSL_write)(void *ssl, const void *data, int sz);
        int (*SSL_read)(void *ssl, void *data, int sz);
        int (*SSL_shutdown)(void *ssl);
    } hal_api_t;

    extern hal_api_t *HAL;
    void api_init(void);

    const char *api_getVersion(void);

    typedef enum
    {
        SOC_OOBINLINE = 0x01 << 0,        /* not support yet */
        SOC_LINGER = 0x01 << 1,           /* linger on close */
        SOC_NBIO = 0x01 << 2,             /* Nonblocking */
        SOC_ASYNC = 0x01 << 3,            /* Asynchronous notification */
        SOC_NODELAY = 0x01 << 4,          /* disable Nagle algorithm or not */
        SOC_KEEPALIVE = 0x01 << 5,        /* enable/disable the keepalive */
        SOC_RCVBUF = 0x01 << 6,           /* set the socket receive buffer size */
        SOC_SENDBUF = 0x01 << 7,          /* set the socket send buffer size */
        SOC_NREAD = 0x01 << 8,            /* no. of bytes for read, only for soc_getsockopt */
        SOC_PKT_SIZE = 0x01 << 9,         /* datagram max packet size */
        SOC_SILENT_LISTEN = 0x01 << 10,   /* SOC_SOCK_SMS property */
        SOC_QOS = 0x01 << 11,             /* set the socket qos */
        SOC_TCP_MAXSEG = 0x01 << 12,      /* set the max segmemnt size */
        SOC_IP_TTL = 0x01 << 13,          /* set the IP TTL value */
        SOC_LISTEN_BEARER = 0x01 << 14,   /* enable listen bearer */
        SOC_UDP_ANY_FPORT = 0x01 << 15,   /* enable UDP any foreign port */
        SOC_WIFI_NOWAKEUP = 0x01 << 16,   /* send packet in power saving mode */
        SOC_UDP_NEED_ICMP = 0x01 << 17,   /* deliver NOTIFY(close) for ICMP error */
        SOC_IP_HDRINCL = 0x01 << 18,      /* IP header included for raw sockets */
        SOC_IPSEC_POLICY = 0x01 << 19,    /* ip security policy */
        SOC_TCP_ACKED_DATA = 0x01 << 20,  /* TCPIP acked data */
        SOC_TCP_DELAYED_ACK = 0x01 << 21, /* TCP delayed ack */
        SOC_TCP_SACK = 0x01 << 22,        /* TCP selective ack */
        SOC_TCP_TIME_STAMP = 0x01 << 23,  /* TCP time stamp */
        SOC_TCP_ACK_MSEG = 0x01 << 24,    /* TCP ACK multiple segment */
    } soc_option_enum;

#ifdef __cplusplus
}
#endif
#endif /* HAL_API_H_ */