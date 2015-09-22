/* Arduino SdSpi Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino SdSpi Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdSpi Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * \file
 * \brief SdSpi class for V2 SD/SDHC cards
 */
#ifndef SdSpi_h
#define SdSpi_h

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>
#include <SPI.h>


#define SPI_BYTE_TRANSFER while (!(SPSR & _BV(SPIF)))
#define SPI_DATA_REGISTER SPDR

//------------------------------------------------------------------------------
// define default chip select pin
//
/** The default chip select pin for the SD card is SS. */
uint8_t const SD_CHIP_SELECT_PIN = SS;

extern SPIClass SPI;

//------------------------------------------------------------------------------
/**
 * \class SdSpi
 * \brief SPI class for access to SD and SDHC flash memory cards.
 */
struct fast_SPI
{
	/** Initialize the SPI bus */
	void inline
	begin()
	{	SPI.begin();
	}

	/** Set SPI options for access to SD/SDHC cards.
	 *
	 * \param[in] spiDivisor SCK clock divider relative to the system clock.
	 */
	void inline
	init(uint8_t sckDivisor)
	{
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);

		SPI.setClockDivider(sckDivisor);
	}
//------------------------------------------------------------------------------
	/** Receive a byte.
	 *
	 * \return The byte.
	 */
	uint8_t inline
	receive()
	{
		SPI_DATA_REGISTER = 0xFF;
		SPI_BYTE_TRANSFER;
		return SPI_DATA_REGISTER;
	}

	/** Receive multiple bytes.
	 *
	 * \param[out] buf Buffer to receive the data.
	 * \param[in] n Number of bytes to receive.
	 *
	 * \return Zero for no error or nonzero error code.
	 */
	uint8_t inline
	receive(uint8_t* buf, size_t n)
	{
		if (n == 0 || buf == NULL)
			return uint8_t(-1);
		for(; n > 0; --n){
			SPI_DATA_REGISTER = 0xFF;
			SPI_BYTE_TRANSFER;
			*buf++ = SPI_DATA_REGISTER;
		}
		return 0;
	}

//------------------------------------------------------------------------------
	/** Send a byte.
	 *
	 * \param[in] data Byte to send
	 */
	void inline
	send(uint8_t data)
	{
		SPI_DATA_REGISTER = data;
		SPI_BYTE_TRANSFER;
	}

	/** Send multiple bytes.
	 *
	 * \param[in] buf Buffer for data to be sent.
	 * \param[in] n Number of bytes to send.
	 */
	void inline
	send(const uint8_t* buf, size_t n)
	{
		if (n == 0 || buf == NULL)
			return;
		for(; n > 0; --n){
			SPI_DATA_REGISTER = *buf++;
			SPI_BYTE_TRANSFER;
		}
	}

//------------------------------------------------------------------------------
	// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
	inline static
	uint8_t transfer(uint8_t data)
	{
		SPDR = data;
//		asm volatile("nop");
		while (!(SPSR & _BV(SPIF)));
		return SPDR;
	}

	inline static
	void transfer(void *buf, size_t count, void *rcv = NULL)
	{
		if (count == 0 || buf == NULL)
			return;
		uint8_t *in = (uint8_t *) buf;
		uint8_t *out = (uint8_t *) rcv;
		for(; count > 0; --count){
			SPI_DATA_REGISTER = *in++;
			SPI_BYTE_TRANSFER;
			if(out!=NULL)
				*out++ = SPI_DATA_REGISTER;
		}
	}
};

#endif  // SdSpi_h

