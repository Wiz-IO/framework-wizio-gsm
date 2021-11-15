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

#ifdef USE_API

/* FIRMWARE DEPENDENT LIBRARY */

#include <Arduino.h>
#include <IPAddress.h>
#include <Client.h>
#include <RingBuffer.h>

#define DEBUG_SSL Serial.printf

class gprsClientSecure : public Client
{
private:
    int (*m_client_method)(void);
    void *m_ctx;
    void *m_ssl;
    int m_socket;
    bool m_conneted;
    RingBuffer m_rx;
    unsigned int m_timeout;
    unsigned int m_verify;
    void *m_certificate;
    uint32_t m_m_certificate_size;
    void *m_private_key;
    uint32_t m_private_key_size;
    const char *m_cipher_list;

    int internal_read()
    {
        int cnt = m_rx.availableForStore();
        while (cnt--)
        {
            uint8_t data;
            if (1 != HAL->SSL_read(m_ssl, &data, 1))
                break;
            m_rx.store_char(data);
        }
        return m_rx.available();
    }

    bool session_create()
    {
        if (HAL)
        {
            m_ctx = HAL->SSL_CTX_new(m_client_method());
            if (m_ctx)
            {
                HAL->SSL_CTX_set_verify(m_ctx, 0, NULL);
                m_ssl = HAL->SSL_new(m_ctx);
                if (NULL == m_ssl)
                {
                    DEBUG_SSL("[ERROR] SSL create failed\n");
                }
                return (NULL != m_ssl);
            }
            else
            {
                DEBUG_SSL("[ERROR] SSL create context failed\n");
            }
        }
        return false;
    }

    bool init_certificate()
    {
        if (m_certificate)
        {
            int res;
            if (m_m_certificate_size)
            {
                res = HAL->SSL_CTX_use_certificate_buffer(m_ctx, (const unsigned char *)m_certificate, m_m_certificate_size, 1);
            }
            else
            {
                res = HAL->SSL_CTX_use_certificate_chain_file(m_ctx, (const char *)m_certificate);
            }
            if (SSL_SUCCESS != res)
            {
                DEBUG_SSL("[ERROR] SSL certificate( %d )\n", res);
            }
            return (SSL_SUCCESS == res);
        }
        return true; // ignore
    }

    bool init_private_key()
    {
        if (m_private_key)
        {
            int res;
            if (m_private_key_size)
            {
                res = HAL->SSL_CTX_use_PrivateKey_buffer(m_ctx, (const unsigned char *)m_private_key, m_private_key_size, 1);
            }
            else
            {
                res = HAL->SSL_CTX_use_PrivateKey_file(m_ctx, (const char *)m_private_key, 1);
            }
            if (SSL_SUCCESS != res)
            {
                DEBUG_SSL("[ERROR] SSL private key( %d )\n", res);
            }
            return (SSL_SUCCESS == res);
        }
        return true; // ignore
    }

    bool init_cipher()
    {
        if (m_cipher_list)
        {
            if (false == HAL->SSL_CTX_set_cipher_list(m_ctx, m_cipher_list))
            {
                DEBUG_SSL("[ERROR] SSL set cipher failed!\n");
                return false;
            }
        }
        return true; // ignore
    }

#define SSL_TIMEOUT_STEP 100

public:
    gprsClientSecure()
    {
        m_client_method = NULL;
        m_ctx = NULL;
        m_ssl = NULL;
        m_socket = -1;
        m_conneted = false;
        m_rx.clear();
        m_timeout = 10000;

        m_certificate = NULL;
        m_m_certificate_size = 0;
        m_private_key = NULL;
        m_private_key_size = 0;
        m_cipher_list = "ALL";
        m_verify = 0;
    }

    ~gprsClientSecure() { stop(); }

    void stop()
    {
        if (HAL)
        {
            if (connected() && m_ssl)
            {
                HAL->SSL_shutdown(m_ssl);
            }

            if (m_socket > -1)
            {
                Ql_SOC_Close(m_socket);
                m_socket = -1;
            }

            if (m_ssl)
            {
                HAL->SSL_free(m_ssl);
                m_ssl = NULL;
            }

            if (m_ctx)
            {
                HAL->SSL_CTX_free(m_ctx);
                m_ctx = NULL;
            }
        }
        m_conneted = false;
    }

    /* before connect */
    void setCertificate(void *certificate, uint32_t size)
    {
        m_certificate = certificate; // if - certificate is used
        m_m_certificate_size = size; // if size - is buffer else filename
    }

    /* before connect */
    void setPrivateKey(void *private_key, uint32_t size)
    {
        m_private_key = private_key;
        m_private_key_size = size;
    }

    /* before connect */
    void setCipher(const char *list)
    {
        m_cipher_list = list;
    }

    /* before connect */
    void setMethod(client_methods method)
    {
        if (HAL)
        {
            switch (method)
            {
            case SSL_2_3:
                m_client_method = HAL->SSLv2_3_client_method;
                break;
            case TLS_1_0:
                m_client_method = HAL->TLSv1_0_client_method;
                break;
            case TLS_1_1:
                m_client_method = HAL->TLSv1_1_client_method;
                break;
            case TLS_1_2:
                m_client_method = HAL->TLSv1_2_client_method;
                break;
            default: //SSL_3_0
                m_client_method = HAL->SSLv3_0_client_method;
                break;
            }
        }
    }

    /* before connect */
    void setTimeout(unsigned int timeout)
    {
        if (timeout < 3000) // minimal handshake is ~8 sec
            timeout = 3000;
        m_timeout = timeout /= SSL_TIMEOUT_STEP;
    }

    /* before connect */
    void setVerify(unsigned int val)
    {
        m_verify = val;
    }

    uint8_t connected() { return HAL && m_conneted; }

    int connect(unsigned int ip, unsigned short port)
    {
        int res, error;

        if (connected())
            return true; // already

        m_rx.clear();

        if (NULL == m_client_method)
            setMethod(TLS_1_2); 

        if (session_create())
        {
            if (false == init_certificate() ||
                false == init_private_key() ||
                false == init_cipher())
                goto error;
            if (m_verify)
                HAL->SSL_CTX_set_verify(m_ctx, m_verify, NULL);

            if ((m_socket = Ql_SOC_Create(0, SOC_TYPE_TCP)) < 0)
            {
                DEBUG_SSL("[ERROR] SSL Ql_SOC_Create() failed!\n");
                goto error;
            }

            res = Ql_SOC_ConnectEx(m_socket, (uint32_t)Ql_convertIP(ip), port, true); 
            if (SOC_SUCCESS != res)
            {
                DEBUG_SSL("[ERROR] SSL Ql_SOC_ConnectEx( %d )\n", res);
                goto error;
            }

            HAL->SSL_set_fd(m_ssl, m_socket);
            while (m_timeout--)
            {
                if (SSL_SUCCESS == HAL->SSL_connect(m_ssl))
                {
                    m_conneted = true;
                    return m_conneted; // done
                }

                error = HAL->SSL_CTX_GetError(m_ssl);
                if (SSL_ERROR_WANT_READ == error || error == SSL_ERROR_WANT_WRITE)
                {
                    Ql_Sleep(SSL_TIMEOUT_STEP);
                }
                else
                {
                    DEBUG_SSL("[ERROR] SSL connect( %d )\n", error);
                    goto error;
                }
            }
            DEBUG_SSL("[ERROR] SSL connect timeout\n");
            goto error;
        }
    error:
        stop();
        return false;
    }

    int connect(IPAddress IP, uint16_t port)
    {
        return connect((uint32_t)IP, port);
    }

    int connect(const char *host, unsigned short port)
    {
        uint32_t ip;
        IPAddress IP;
        if (getHostByName(host, IP))
            return connect(IP, port);
        DEBUG_SSL("[ERROR] SSL could not get host from dns\n");
        return false;
    }

    size_t write(const uint8_t *buffer, size_t size)
    {
        int res = 0;
        if (connected() && buffer && size)
        {
            res = HAL->SSL_write(m_ssl, buffer, size);
        }
        return res;
    }

    size_t write(uint8_t b) { return write(&b, 1); }

    int read(uint8_t *buffer, size_t size)
    {
        int readed = 0;
        if (connected() && buffer && size)
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
                if (1 == HAL->SSL_read(m_ssl, &data, 1))
                {
                    *buf++ = data;
                    readed++;
                }
            }
        }
        return readed;
    }

    int read()
    {
        if (connected())
        {
            if (m_rx.available() > 0)
                return m_rx.read_char();
            unsigned char data;
            return (1 == HAL->SSL_read(m_ssl, &data, 1)) ? data : EOF;
        }
        return EOF;
    }

    int available()
    {
        if (HAL && connected())
        {
            int res = m_rx.available();
            if (res > 0)
                return res;
            return internal_read();
        }
        return 0;
    }

    int peek()
    {
        if (connected())
        {
            if (m_rx.available() > 0)
                return m_rx.peek();
            if (internal_read() > 0)
                return m_rx.peek();
        }
        return EOF;
    }

    void flush()
    {
        if (connected())
        {
            while (available())
                read();
            m_rx.clear();
        }
    }

    int get_last_error()
    {
        int res = -1;
        if (HAL && m_ssl)
        {
            res = HAL->SSL_CTX_GetError(m_ssl);
        }
        return res;
    }

    operator bool() { return connected(); }
    bool operator==(const bool value) { return bool() == value; }
    bool operator!=(const bool value) { return bool() != value; }
    bool operator==(const gprsClientSecure &);
    bool operator!=(const gprsClientSecure &rhs) { return !this->operator==(rhs); }

    using Print::write;
};

#else
#error gprsClientSecure library use API support
#endif