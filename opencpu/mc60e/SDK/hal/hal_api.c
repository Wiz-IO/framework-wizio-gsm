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

#include "hal_api.h"

hal_api_t *HAL = NULL;

#ifdef USE_API

#define API_COUNT 1

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
/* FIRMWARE DEPENDENT FUNCTIONS */
static hal_api_t HAL_TABLE[API_COUNT] = {
    {
        "M66FAR01A12BT",
        0x100200C0, // FW_VER_ADDR

        0xF026EC59, // soc_getsockopt()
        0xF026F0F5, // soc_setsockopt()

        /* SSL */
        0x1007FB35, // TLSv1_0_client_method
        0x1007FAF5, // TLSv1_1_client_method
        0x1007FB15, // TLSv1_2_client_method
        0x1007FA95, // SSLv2_3_client_method
        0x1007FAB9, // SSLv3_0_client_method
        0x100C6CCD, // SSL_CTX_new
        0x100C48C5, // SSL_CTX_free
        0x100CDDCD, // SSL_CTX_GetError
        0x100C6F2D, // SSL_CTX_set_verify
        0x100C6AAD, // SSL_CTX_set_cipher_list
        0x100C6DC9, // SSL_CTX_use_certificate_chain_file
        0x100C6D79, // SSL_CTX_use_certificate_buffer
        0x100C6C81, // SSL_CTX_use_PrivateKey_file
        0x100C6C2D, // SSL_CTX_use_PrivateKey_buffer
        0x100C6ECD, // SSL_CTX_load_verify_locations
        0x100C6E65, // SSL_CTX_load_verify_buffer
        0x100C6BA5, // SSL_new
        0x100C500D, // SSL_free
        0x100C6BF5, // SSL_set_fd
        0x100D33D5, // SSL_connect
        0x100CDB69, // SSL_write
        0x100C65F1, // SSL_read
        0x100C4D15, // SSL_shutdown

        /* CRYPTO */
        0x100D8815, // InitMd5
        0x100CFC45, // Md5Update
        0x100CF7C7, // Md5Final
        0x100D87E5, // InitSha
        0x100CFBED, // ShaUpdate
        0x100CF721, // ShaFinal
        0x100D8799, // InitSha256
        0x100CB4C7, // Sha256Update
        0x100CBDBD, // Sha256Final
        0x10154F03, // HmacSetKey
        0x100B3D1B, // HmacUpdate
        0x100B3C79, // HmacFinal
        0x100BFC05, // base64_encode
        0x100BEFBD, // base64_decode
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
