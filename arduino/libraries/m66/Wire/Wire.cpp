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
#include "Wire.h"

#define DEBUG_I2C Serial.printf

#define HAL_TYPE (0 == i2c_port)
#define I2C_TYPE (1 == i2c_port)

void TwoWire::default_init(uint8_t port)
{
	i2c_port = port;
	i2c_speed = 0;
	slaveAddress = 0;
	transmissionBegun = 0;
	scl = PINNAME_RI;
	sda = PINNAME_DCD;
	if (HAL_TYPE)
	{
		ctx = I2C_Create();
	}
}

void TwoWire::end()
{
	if (HAL_TYPE)
	{
		I2C_Close(ctx);
	}
	else
	{
		Ql_IIC_Uninit(i2c_port);
	}
}

void TwoWire::begin(void)
{
	int res;
	end();
	if (HAL_TYPE)
	{
		res = I2C_Open(ctx);
		DEBUG_I2C("[I2C] I2C_Open( %d )\n", res);
	}
	else
	{
		res = Ql_IIC_Init(i2c_port, scl, sda, I2C_TYPE);
		if (res)
		{
			DEBUG_I2C("[ERROR] Ql_IIC_Init( %d )\n", res);
		}
		else
		{
			DEBUG_I2C("[I2C] Ql_IIC_Init( %d )\n", res);
		}
	}
	setClock(100000);
}

/* only for HW, for SW baudrate is ignored */
void TwoWire::setClock(uint32_t Hz)
{
	int res = 0;
	if (i2c_port < 2 && Hz != i2c_speed)
	{
		Hz /= 1000;
		if (Hz == 400)
			Hz -= 1; // work around, high speed mode  [400..3400] kbps.
		i2c_speed = Hz;
		if (HAL_TYPE)
		{
			ctx->config.HiSpeed = Hz;
			ctx->config.LoSpeed = Hz;
			ctx->config.Mode = Hz < 400 ? I2C_TRANSACTION_LO_MODE : I2C_TRANSACTION_HI_MODE;
			I2C_UpdateSpeed(ctx, true);
		}
		else
		{
			res = Ql_IIC_Config(i2c_port, true, slaveAddress, i2c_speed);
			if (res)
			{
				DEBUG_I2C("[ERROR] Ql_IIC_Config( P = %d, B = %d, A = 0x%02X ) %d\n", i2c_port, (int)i2c_speed, (int)slaveAddress, res); // ??? QL_RET_ERR_IIC_SLAVE_TOO_MANY -310;
			}
		}
	}
}

// TODO
uint8_t TwoWire::requestFrom(uint8_t address, size_t size, bool stopBit)
{
	return 0;
}

/*
	0: success
	1: data too long to fit in transmit buffer
	2: received NACK on transmit of address
	3: received NACK on transmit of data
	4: other error
*/
uint8_t TwoWire::endTransmission(bool stopBit)
{
	int res = 4;
	transmissionBegun = false;
	if (stopBit)
	{
		uint32_t size = tx.available();
		if (size && size < SERIAL_BUFFER_SIZE)
		{
			if (HAL_TYPE)
			{
				res = I2C_Transaction(ctx, tx._aucBuffer, size, false);
			}
			else
			{
				res = QL_Write(tx._aucBuffer, size);
			}
			if (res != size)
			{
				DEBUG_I2C("[ERROR] write: %d\n", res);
				res = 4;
			}
			else
			{
				res = 0;
			}
		}
	}
	return res;
}

uint32_t TwoWire::QL_Write(uint8_t *buf, uint32_t len)
{
	if (NULL == buf || 0 == len)
		return -1;
	int size = len, res, cnt;
	while (size > 0)
	{
		cnt = (size / 8) ? 8 : size; // max size 8 bytes
		res = Ql_IIC_Write(i2c_port, slaveAddress, buf, cnt);
		if (res == cnt)
		{
			buf += cnt;
			size -= cnt;
		}
		else
		{
			return res; // ??? QL_RET_ERR_I2CHWFAILED: -34
		}
	}
	return len;
}

/*
	0: 		HAL Port
	1: 		Quectel HW
	2-254 	Quectel SW emulation
*/
TwoWire Wire(0);