/**
 * \file   SPI_SRAM.h
 *
 * 
 * 
 * \date  		Mar 17, 2015
 * \author		Lyle
 * \copyright	MIT Public License
 */

#ifndef SPI_SRAM_H_
#define SPI_SRAM_H_

#include "fast_Digital.h"
#include <SPI.h>

#define SRAM_MAX 0x04
#define SRAM_ADDR_MAX 0xDEADBEEF

#define RM( _STR_ ) ""


namespace SRAM
{

enum CMD
	: byte
{
	READ  = 0x03,  // Read data from mem. array beggining at selected address
	WRITE = 0x02,  // Write data to mem. array begging at selected address
	EDIO  = 0x3B,  // Enter Dual I/O access
	EQIO  = 0x38,  // Enter Quad I/O access
	RSTIO = 0xFF,  // Reset Dual and Quad I/O access
	RDMR  = 0x05,  // Read Mode Register (MR defaults to Sequential Mode = 0b01000000)
	WRMR  = 0x01,  // Write Mode Register
};
enum MODE
	:byte
{
	BYTE       = (00_b << 5),
	PAGE       = (10_b << 5),
	SEQUENTIAL = (01_b << 5),  //(default)
	RESERVED   = (11_b << 5),
};
enum CHIP
	:byte
{
	_0 = _B { 0 }(),
	_1 = _B { 1 }(),
	_2 = _B { 2 }(),
	_3 = _B { 3 }(),
};

struct addr_t
{
	uint8_t raw[3];
	inline operator uint32_t(){
		return (*((uint32_t*)raw) & 0x00FFFFFF);
	}
	addr_t() :	//	0x12345678 == { 0x78, 0x56, 0x34, 0x12 }
			raw { 0, 0, 0 }
	{
	}
	addr_t( uint32_t val )
//		:raw { (val >> 0) & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff }
	{
		(*(uint32_t*)raw) = val;
	}
};
static_assert(sizeof(addr_t)==3,RM("Address size mismatch"));


struct chip_select
{
	const CHIP N;
	typedef PIN< 'D', 5 > HOLD;  //active low
	typedef PORT< 'C' > SELECT;

	constexpr static inline
	uint8_t MASK()
	{
		return ~(uint8_t(0xff << SRAM_MAX));
	}
	chip_select( const chip_select & ) = delete;
	chip_select( chip_select && ) = default;
	chip_select( const CHIP num ) :
			N( num )
	{
		PORT< 'C' >::set( MASK() ^ _B { N } );
		PORT< 'C' >::clr( _B { N } );
		HOLD::write( HIGH );
	}
	~chip_select()
	{
		PORT< 'C' >::set( _B { N } );
		HOLD::write( LOW );
	}
};

struct chip_pause
{
	typedef PIN< 'D', 5 > HOLD;  //active low
	chip_pause( const chip_pause & ) = delete;
	chip_pause( chip_pause && ) = default;
	chip_pause()
	{
	    while (!(SPSR & _B{SPIF})) ; // wait for current transaction
		HOLD::write( LOW );
	}
	~chip_pause()
	{
		HOLD::write( HIGH );
	}
};

struct chip
{
//	static_assert((chipNum&(~B11))==0,"Chip number must be one of {0,1,2,3}.");

	typedef PIN< 'D', 5 > HOLD;  //active low
	typedef PIN< 'C', 7 > REST;  //active low
	typedef PORT< 'C' > SELECT;

	static byte active_mask;
	const CHIP pin;

	chip( CHIP num ) :
			pin( num )
	{
		active_mask |= num;  //remember which are active
	}

	void static inline
	begin()
	{
		HOLD::mode( OUTPUT );
		REST::mode( OUTPUT );
		SELECT::mode( OUTPUT, active_mask );

		HOLD::write( HIGH );
		REST::write( HIGH );
		SELECT::set( active_mask );
	}

	byte inline
	mode()
	{
		chip_select _CS( pin );
		return SPI.transfer( CMD::RDMR );
	}
	void inline
	mode( const MODE mode )
	{
		chip_select _CS( pin );
		SPI.transfer( CMD::WRMR );
		SPI.transfer( mode );
	}

	template< typename data_t >
	void inline
	write(  const addr_t & address, data_t * src, const uint16_t size )
	{
		chip_select _CS( pin );
		SPI.transfer( CMD::WRITE );
		SPI.transfer( (void*) address.raw, sizeof(address) );

		SPI.transfer( src, size );
	}

	template< typename data_t >
	void inline
	read(  const addr_t & address, data_t * dest, const uint16_t size )
	{
		chip_select _CS( pin );
		SPI.transfer( CMD::READ );
		SPI.transfer( (void*) address.raw, sizeof(address) );
		SPI.transfer( (void*) dest, size );
	}

	template< typename data_t >
	void inline
	transfer( const CMD cmd, const addr_t & adr, data_t * dat, const uint16_t sz )
	{
		chip_select _CS( pin );
		SPI.transfer( cmd );
		SPI.transfer( (void*) adr.raw, sizeof(adr) );
		SPI.transfer( (void*) dat, sz );
	}

};
byte chip::active_mask = 0;

struct TX_async: private chip_select, private addr_t
{
	TX_async() = delete;
	TX_async( const TX_async & ) = delete;
	TX_async( TX_async && ) = default;
	TX_async( CHIP N, addr_t addr ) :
			chip_select(N), addr_t( addr )
	{
		SPI.transfer( SRAM::CMD::WRITE );
		SPI.transfer( (void*) (this->raw), sizeof(addr) );
	}
	void inline write( byte data )
	{
		SPI.transfer( data );
	}

};

struct RX_async: private chip_select, private addr_t
{
	RX_async() = delete;
	RX_async( const RX_async & ) = delete;
	RX_async( RX_async && ) = default;
	RX_async( CHIP N, addr_t addr ) :
		chip_select(N), addr_t( addr )
	{
		SPI.transfer( SRAM::CMD::READ );
		SPI.transfer( (void*) (this->raw), sizeof(addr) );
	}
	void inline read( byte data )
	{
		SPI.transfer( data );
	}

};

};//namespace SRAM

#endif /* SPI_SRAM_H_ */
