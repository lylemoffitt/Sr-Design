/**
 * \file Interrupt_test.ino
 *
 * \author Lyle Moffitt
 *
 * \copyright
 * Created for 3D POV senior design group at Syracuse University.
 * All rights reserved.
 *
 */

#define ENABLE_SERIAL

#include <Arduino.h>
#include <TimerOne.h>
#include <stdint.h>
#include <stddef.h>
#include <SPI.h>

//-----------------------------------------------------------------------------
//		Extraneous Typedefs
//-----------------------------------------------------------------------------
//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned int uint32_t;

//-----------------------------------------------------------------------------
//		Configuration Data Definitions
//-----------------------------------------------------------------------------
/// Number of rotations per second
#define FRAME_RATE 30
/// Number of positions in a rotation
#define THETA_POSITIONS 256
/// Duration spent in each position
#define THETA_DURATION (10e6 / (FRAME_RATE * THETA_POSITIONS))
/// Number of cycles to cache
#define SWEEP_CACHE_POSITIONS 4
/// The total size of the cache
#define SWEEP_CACHE_SIZE (4 * THETA_POSITIONS)
/// SPI Chip Select pins for LCD driver/s
const byte LCDpin[1] = { 6 };
/// SPI Chip Select, and HOLD pins for SRAM external cache
const byte SRAMpin[2] = { 5,4 };
/// External Interrupt pin connected to Hall-Effect sensor
const byte HALLpin = { 2 };

//-----------------------------------------------------------------------------
//		Data Structure Definitions
//-----------------------------------------------------------------------------
/// Union containing data for all LEDs in one theta position
union wing
{
	//condensed value
	uint32_t val;
	//byte array of the aforementioned
	//	0x12345678 == { 0x78, 0x56, 0x34, 0x12 }
	byte raw[sizeof(uint32_t)];
};

/// Struct containing all the data for one rotation
typedef struct
{
	unsigned position; //Current theta position within the ring
	union
	{
		wing data[THETA_POSITIONS];
		byte raw[sizeof(wing) * THETA_POSITIONS];
	};
} sweep;

//-----------------------------------------------------------------------------
//		Global Variables
//-----------------------------------------------------------------------------
/// Status registers indicating if the sweep has been read
static bool cache_dirty[SWEEP_CACHE_POSITIONS];
/// Current sweep being read.
static unsigned cache_index;
/// The primary cache for the LEDs
static sweep cache[SWEEP_CACHE_POSITIONS];

//-----------------------------------------------------------------------------
//		Setup & Loop
//-----------------------------------------------------------------------------
void setup()
{	//configure pins
	pinMode( LCDpin[0], HIGH ); 	//SPI CS for LCD driver [0]
	pinMode( SRAMpin[0], HIGH );	//SPI CS for SRAM cache [0]
        pinMode( SRAMpin[1], HIGH );	//HOLD for SRAM cache [0]
	pinMode( HALLpin, HIGH );   	//interrupt for hall-effect sensor

	//configure SPI bus
	SPI.begin();
	SPI.setDataMode( 0x00 );
	SPI.setClockDivider( 0x04 );

	//set sync signal interrupt handler
	attachInterrupt( HALLpin, sync_handler, FALLING );

	//set timer interrupt
	Timer1.initialize( THETA_DURATION );
	Timer1.attachInterrupt( theta_handler );

#ifdef ENABLE_SERIAL
	Serial.begin( 9600 );
#endif
}

//-----------------------------------------------------------------------------
void loop()
{
	//attempt to fill any dirty caches
	cache_handler();

	//loop execute about 16 times per rotation,
	//	which is about 4 times faster than they can be emptied (with EEPROM)
	delayMicroseconds( THETA_POSITIONS / 16 * THETA_DURATION );
}

//-----------------------------------------------------------------------------
//		ISR & Helper Function Definitions
//-----------------------------------------------------------------------------
inline void theta_handler( void )
{
	static wing w;

	//fetch next theta
	w.val = cache[cache_index].data[cache[cache_index].position++].val;

	//send lower 2 bytes order to 0th LED driver
	digitalWrite( LCDpin[0], LOW );
	SPI.transfer( w.raw[0] );
	SPI.transfer( w.raw[1] );
	digitalWrite( LCDpin[0], HIGH );
	//send higher 2 bytes order to 0th LED driver
	digitalWrite( LCDpin[1], LOW );
	SPI.transfer( w.raw[2] );
	SPI.transfer( w.raw[3] );
	digitalWrite( LCDpin[1], HIGH );

	//increment and check position
	if( cache[cache_index].position >= THETA_POSITIONS )
	{
		cache_dirty[cache_index++] = true;
	}
	//increment and check index
	if( cache_index >= SWEEP_CACHE_POSITIONS )
	{
		cache_index = 0;
	}
}

//-----------------------------------------------------------------------------
inline void cache_handler( void )
{
	static unsigned index( 0 );

	//wait for sweep to be used.
	if( !cache_dirty[index] ) return;

	//overwrite swap data with new from SRAM cache
	digitalWrite( SRAMpin[0], LOW );
	SPI_transfer( cache[index].raw, SWEEP_CACHE_SIZE );
	cache[index].position = 0;
	digitalWrite( SRAMpin[0], HIGH );

	//set bit clean
	cache_dirty[index++] = false;

	//reset index if at end of cache
	if( index >= SWEEP_CACHE_POSITIONS ) index = 0;
}

//-----------------------------------------------------------------------------
inline void sync_handler( void )
{
	Timer1.restart();
}

//-----------------------------------------------------------------------------
inline void SPI_transfer( byte * buf, size_t _sz )
{
  for(size_t i=0; i<_sz; ++i){
    buf[i] = SPI.transfer(buf[i]);
  }
}

