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
#include "hal_gpio.h"

#define DEBUG_I2C Serial.printf

#define I2C_TYPE (1 == i2c_port)

static void i2c_io(uint8_t i2c_port)
{
	if (I2C_TYPE)
	{
		GPIO_Setup(GPIO43, GPMODE(GPIO43_MODE_SCL) | GPPULLEN | GPPDN); // GPPUP
		GPIO_Setup(GPIO44, GPMODE(GPIO44_MODE_SDA) | GPPULLEN | GPPDN); // GPPUP
	}
}

TwoWire::TwoWire(uint8_t port)
{
	i2c_speed = 0;
	i2c_port = port & 1; // 0:SW or 1:HW
	slaveAddress = 1;
	transmissionBegun = false;
}

void TwoWire::begin(void)
{
	int res;
	if (I2C_TYPE)
	{
		scl = PINNAME_RI;
		sda = PINNAME_DCD;
	}
	else
	{
		scl = PINNAME_CTS;
		sda = PINNAME_RTS;
	}
	setClock(100000);
}

void TwoWire::setClock(uint32_t Hz)
{
	Ql_IIC_Uninit(i2c_port);
	int res = Ql_IIC_Init(i2c_port, scl, sda, I2C_TYPE);
	if (0 == res)
	{
		i2c_io(i2c_port);
		Hz /= 1000;
		if (Hz == 400)
			Hz += 1; // work around
		if (Hz != i2c_speed)
		{
			i2c_speed = Hz;
			int res = Ql_IIC_Config(i2c_port, true, slaveAddress, i2c_speed);
			if (res)
			{
				DEBUG_I2C("[ERROR] Ql_IIC_Config( P = %d, B = %d, A = 0x%02X ): %d\n", i2c_port, (int)i2c_speed, (int)slaveAddress, res);
			}
		}
	}
}

void TwoWire::end()
{
	Ql_IIC_Uninit(i2c_port);
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
	int res = -1;
	if (quantity == 0)
		return 0;
	if (!stopBit)
		return 0;
	rx.clear();
	//TODO: if IICtype = 1 ,1 < len < 8 . because our IIC contronller at most support 8 bytes
	res = Ql_IIC_Read(i2c_port, address, (uint8_t *)(rx._aucBuffer), (uint32_t)quantity);
	if (res < 0)
	{
		//DEBUG_I2C("[I2C] R( %02X ) %d\n", (int)address, res);
		quantity = 0;
	}
	rx._iHead = quantity;
	return rx.available();
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
	return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address)
{
	slaveAddress = address;
	tx.clear();
	transmissionBegun = true;
}

uint8_t TwoWire::endTransmission(bool stopBit)
{
	if (!stopBit)
		return 1;
	u32 count, i, size;
	u8 buf[8];
	transmissionBegun = false;
	while (tx.available() > 0)
	{
		size = tx.available();
		for (i = 0, count = 0; i < size; i++)
		{
			if (i > 7)
				break;
			buf[i] = tx.read_char();
			count++;
		}
		int res = Ql_IIC_Write(i2c_port, slaveAddress, buf, count /*max 8*/);
		if (res != count)
		{
			DEBUG_I2C("[I2C] Ql_IIC_Write( %02X ): %d\n", (int)buf[0], res);
			return 1;
		}
	}
	return 0;
}

uint8_t TwoWire::endTransmission()
{
	return endTransmission(true);
}

size_t TwoWire::write(uint8_t ucData)
{
	if (!transmissionBegun || tx.isFull())
		return 0;
	tx.store_char(ucData);
	return 1;
}

int TwoWire::available(void) { return rx.available(); }

int TwoWire::read(void) { return rx.read_char(); }

int TwoWire::peek(void) { return rx.peek(); }

TwoWire Wire(1);