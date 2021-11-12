#pragma GCC warning "gprsClientSecure is not ready yet"

#include <Arduino.h>
#include <IPAddress.h>
#include "gprsClient.h"

#ifdef USE_API

#define DEBUG_SSL Serial.printf

class gprsClientSecure : public gprsClient
{
private:
    const char *_ca;
    const char *_client_cert;
    const char *_private_key;

    void *m_ctx;
    void *m_ssl;

public:
    gprsClientSecure()
    {
        if (HAL)
        {
            m_ctx = HAL->SSL_CTX_new(HAL->SSL_get_method(1));
            if (m_ctx)
                m_ssl = HAL->SSL_new(m_ctx);
        }
    }

    ~gprsClientSecure()
    {
        if (HAL)
        {
            if (m_ctx)
                HAL->SSL_free(m_ssl);
            if (m_ssl)
                HAL->SSL_CTX_free(m_ctx);
        }
    }

    void stop()
    {
        if (HAL)
        {
            if (m_ssl)
                HAL->SSL_shutdown(m_ssl);
            gprsClient::stop();
        }
    }

    uint8_t connected() { return socket() >= 0; }

    int connect(unsigned int ip, unsigned short port)
    {
        int res = -1;
        if (HAL)
        {
            if (m_ssl && gprsClient::connect(ip, port))
            {
                HAL->SSL_set_fd(m_ssl, gprsClient::socket()); // allways return 1
                res = HAL->SSL_connect(m_ssl);
                DEBUG_SSL("[SSL] SSL_connect( %d )\n", res);
            }
            else
            {
                DEBUG_SSL("[ERROR] connect()\n");
            }
        }
        return res == 0;
    }

    int connect(const char *host, unsigned short port)
    {
        uint32_t ip;
        IPAddress IP;
        if (getHostByName(host, IP))
        {
            DEBUG_SSL("[SSL] HOST: %s IP: %X\n", host, (int)((uint32_t)IP));
            return connect(IP, port);
        }
        DEBUG_SSL("[ERROR] SSL: could not get host from dns\n");
        return false;
    }

    size_t write(const uint8_t *buffer, size_t size)
    {
        int res = -1;
        if (HAL)
        {
            if (m_ssl && connected() && buffer && size)
            {
                res = HAL->SSL_write(m_ssl, (uint8_t *)buffer, size);
                return res == 0 ? res : 0;
            }
            stop();
        }
        return 0;
    }

    size_t write(uint8_t b) { return write(&b, 1); }

    int read(uint8_t *buf, size_t size)
    {
        int res = -1;
        if (m_ssl && connected() && buf && size)
        {
            if ((res = available()))
            {
                ///res = ring.read(&ring, buf, size);
            }
        }
        else
        {
            stop();
        }
        return res;
    }

    int read()
    {
        uint8_t byte;
        return 1 == read(&byte, 1) ? byte : -1;
    }

    int available()
    {
        size_t available = 0;
        if (connected())
        {
            ////
        }
        else
        {
            stop();
        }
        return available;
    }

    int peek()
    {
        int res = -1;
        ////
        return res;
    }

    void flush() {}

    void set_ca_certificate(const char *ca)
    {
        _ca = ca;
    }

    void set_client_certificate(const char *cert, const char *key)
    {
        _client_cert = cert;
        _private_key = key;
    }

    int socket() { return gprsClient::socket(); }

    operator bool() { return connected(); }
    gprsClientSecure &operator=(const gprsClientSecure &other);
    bool operator==(const bool value) { return bool() == value; }
    bool operator!=(const bool value) { return bool() != value; }
    bool operator==(const gprsClientSecure &);
    bool operator!=(const gprsClientSecure &rhs) { return !this->operator==(rhs); }

    using Print::write;
};

#endif