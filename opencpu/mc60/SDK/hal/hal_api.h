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
#include <stdbool.h>
#include <string.h>

    /* FIRMWARE DEPENDENT FUNCTIONS */

    typedef struct
    {
        const char *FW_VER;
        const char *FW_VER_ADDR;

        /* SOCKET */
        int (*soc_getsockopt)(unsigned char soc, unsigned int key, void *val, unsigned char size);
        int (*soc_setsockopt)(unsigned char soc, unsigned int key, void *val, unsigned char size);

        /* SSL */
        int (*TLSv1_0_client_method)(void);
        int (*TLSv1_1_client_method)(void);
        int (*TLSv1_2_client_method)(void);
        int (*SSLv2_3_client_method)(void);
        int (*SSLv3_0_client_method)(void);
        void *(*SSL_CTX_new)(int method);
        int (*SSL_CTX_free)(void *ctx);
        int (*SSL_CTX_GetError)(void *ssl);
        int (*SSL_CTX_set_verify)(void *, int, void *cb);
        int (*SSL_CTX_set_cipher_list)(void *, const char *);                                     /* strings delimited by : */
        int (*SSL_CTX_use_certificate_chain_file)(void *, const char *file);                      /* PEM "UFS:" "RAM:" "NVRAM:" */
        int (*SSL_CTX_use_certificate_buffer)(void *, const unsigned char *, long len, int type); /* PEM */
        int (*SSL_CTX_use_PrivateKey_file)(void *, const char *, int);
        int (*SSL_CTX_use_PrivateKey_buffer)(void *, const unsigned char *, long, int);
        int (*SSL_CTX_load_verify_locations)(void *, const char *, const char *);
        int (*SSL_CTX_load_verify_buffer)(void *, const unsigned char *, long, int);
        void *(*SSL_new)(void *ctx);
        int (*SSL_free)(void *ssl);
        int (*SSL_set_fd)(void *ssl, int soc);
        int (*SSL_connect)(void *ssl);
        int (*SSL_write)(void *ssl, const void *data, int sz);
        int (*SSL_read)(void *ssl, void *data, int sz);
        int (*SSL_shutdown)(void *ssl);

        /* CRYPTO */
        void (*InitMd5)(void *);
        void (*Md5Update)(void *, const unsigned char *, int len);
        void (*Md5Final)(void *, unsigned char *);
        void (*InitSha)(void *);
        void (*ShaUpdate)(void *, const unsigned char *, int len);
        void (*ShaFinal)(void *, unsigned char *);
        void (*InitSha256)(void *);
        void (*Sha256Update)(void *, const unsigned char *, int len);
        void (*Sha256Final)(void *, unsigned char *);
        void (*HmacSetKey)(void *, int type, const unsigned char *key, int len);
        void (*HmacUpdate)(void *, const unsigned char *msg, int len);
        void (*HmacFinal)(void *, unsigned char *hash);
        int (*base64_encode)(const char *src, int src_len, char *dst, int dst_len, bool auto_line_wrap);
        int (*base64_decode)(const char *src, int src_len, char *dst, int dst_len);
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

    typedef enum
    {
        SSL_SUCCESS = 1, // only for handshake
        SSL_ERROR_WANT_READ = -223,
        SSL_ERROR_WANT_WRITE = -227,
    } ssl_errors;

    typedef enum
    {
        TLS_1_0,
        TLS_1_1,
        TLS_1_2,
        SSL_2_3,
        SSL_3_0,
    } client_methods;

    /* 
chiper names
    "ALL"
    "ECDSA"
    "PSK"
    "RC4-MD5"
    "AES256-SHA256"
    "AES128-SHA256"
    "NULL-SHA256"
    "DES-CBC3-SHA"
    "RC4-SHA"      
    "AES256-SHA"
    "AES128-SHA"
    "NULL-SHA"
*/

#ifdef __cplusplus
}
#endif
#endif /* HAL_API_H_ */