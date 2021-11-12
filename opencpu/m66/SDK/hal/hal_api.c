
#include "hal_api.h"

hal_api_t *HAL = NULL;

#ifdef USE_API

#define API_COUNT 1

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
static hal_api_t HAL_TABLE[API_COUNT] = {
    {
        "M66FAR01A12BT",
        0x100200C0, // FW_VER_ADDR
        
        0xF026F0F5, // soc_setsockopt()
        0xF026EC59, // soc_getsockopt()

        0x100C6CCC, // SSL_CTX_new()
        0x100C48C4, // SSL_CTX_free()
        0x100C6C80, // SSL_CTX_use_PrivateKey_file()
        0x100C6DC8, // SSL_CTX_use_certificate_chain_file()
        0x100C6BA4, // SSL_new()
        0x100C500C, // SSL_free()
        0x100C6BF4, // SSL_set_fd()
        0x100D33D4, // SSL_connect()
        0x100CDB68, // SSL_write()
        0x100C65F0, // SSL_read()
        0x100C4D14, // SSL_shutdown()
    },
};
#pragma GCC diagnostic pop

void api_init(void)
{
    HAL = NULL;
    for (int i = 0; i < API_COUNT; i++)
    {
        if (0 == memcmp(HAL_TABLE[i].FW_VER_ADDR, HAL_TABLE[i].FW_VER, sizeof(HAL_TABLE[i].FW_VER)))
        {
            HAL = &HAL_TABLE[i];
            break;
        }
    }
}

#else
void api_init(void)
{
    HAL = NULL;
}
#endif

const char *api_getVersion(void)
{
    return HAL ? HAL->FW_VER : NULL;
}
