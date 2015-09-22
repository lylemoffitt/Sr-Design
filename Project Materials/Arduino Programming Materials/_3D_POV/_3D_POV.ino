/**
 * \file 3D_POV_DRIVER.ino
 *
 * \author Lyle Moffitt
 *
 * \copyright
 * Created for 3D POV senior design group at Syracuse University.
 * All rights reserved.
 *
 */

#define IS_SLAVE

//#define IS_MASTER

#if defined(IS_SLAVE) && defined(IS_MASTER)
#  error Cannot be both master and slave!
#endif

#if !defined(IS_SLAVE) && !defined(IS_MASTER)
#  error There is no in between... You must choose.
#endif

#include "fast_Digital.h"
#include "SPI_SRAM.h"
#include <Arduino.h>
#include <TimerOne.h>
#include <SPI.h>
#include <Wire.h>

#include <stdint.h>
#include <stddef.h>
#include <util/atomic.h>

//#include <avr/pgmspace.h>

//#ifdef IS_MASTER
#include "nRF24L01.h"
#include "RF24.h"
//#endif

#include "predef.h"

//-----------------------------------------------------------------------------
//		Configuration Data Definitions
//-----------------------------------------------------------------------------

/// Which level the current blade is, going from the bottom.
#define LEVEL_INDEX 0
/// Number of levels in total
#define LEVEL_MAX 8
/// Number of rotations per second
#define FRAME_RATE 30
/// Number of positions in a rotation
#define THETA_POSITIONS 256
/// Duration spent in each position
#define THETA_DURATION (1000000 / (FRAME_RATE * THETA_POSITIONS))

#if LEVEL_INDEX%2==0
#  define THETA_OFFSET /*EVEN LAYERS*/   \
	((THETA_POSITIONS / LEVEL_MAX) * (LEVEL_INDEX / 2))
#else
#  define THETA_OFFSET /*ODD LAYERS*/   \
	(((THETA_POSITIONS / LEVEL_MAX) * (LEVEL_INDEX / 2)) + (THETA_POSITIONS/2))
#endif
/// Delay between layers
#define THETA_DELAY 0//( THETA_OFFSET * THETA_DURATION )
/// Number of cycles to cache
#define SWEEP_CACHE_POSITIONS 1
/// The total size of the cache
#define SWEEP_CACHE_SIZE (4 * THETA_POSITIONS)

#define ENABLE_SERIAL
#define BAUD_RATE 9600

#ifdef ENABLE_SERIAL
#  define DEBUG( _msg_ ) do{ Serial.begin( BAUD_RATE ); Serial.print( _msg_ ); Serial.end(); }while(0)
#else
#  define DEBUG( _msg_ ) do{}while(0)
#endif

#ifdef ENABLE_SERIAL
#  define SERIAL( _msg_ ) do{ Serial.begin( BAUD_RATE ); Serial.print( _msg_ ); Serial.end(); }while(0)
#endif

#define USE_DYNAMIC_PAYLOADS false//true

#define RM( _STR_ ) ""

#define SF( _str_ ) String(F(_str_))


//-----------------------------------------------------------------------------
//		Pin Configurations
//-----------------------------------------------------------------------------
struct pins
{
#ifdef IS_SLAVE
/// SPI Chip Select pins for LCD driver/s
struct LCD{
	using LE = PIN<'D',6>;
	using OE = PIN<'D',7>;
//	constexpr const static byte LE = 6, OE = 7;
};
#endif
#ifdef IS_MASTER
struct RF{
	constexpr const static uint8_t chipEnable = 9;
	constexpr const static uint8_t chipSelect = 10;
	using CE  = PIN<'B',1>;
	using CSN = PIN<'B',2>;
	using IRQ = PIN<'D',2>;
};
#endif
/// External Interrupt pin connected to Hall-Effect sensor
using HALL = PIN<'D',3>;//INT1
};



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
static_assert(sizeof(wing)==sizeof(uint32_t),RM("Size mismatch"));

/// Struct containing all the data for one rotation
struct sweep
{
	volatile unsigned position; //Current theta position within the ring
	union
	{
		wing data[THETA_POSITIONS];
		byte raw[sizeof(wing) * THETA_POSITIONS];
	};
};


struct block_ref_t
{
	uint8_t layer_indx : 4;///< Layer number
	uint8_t block_num  : 4;///< Block number
};

struct block_spec_t// 4 bytes
{
	block_ref_t     block_ref;///< Number of block on blade [0-3]
	SRAM::addr_t    block_addr;  ///< Byte address within that block
};

struct animate_spec_t// 4 bytes
{
	uint16_t        frame_length;///< Number of frames
	uint16_t        frame_rate;  ///< Duration between frames
};

struct image_spec_t// 5 bytes
{
	uint8_t         image_number;
	uint32_t        image_size;
};

// Received by the master control node
struct external_spec_t// 10 bytes
{
	block_ref_t       block_ref;
	image_spec_t      image_spec;
	animate_spec_t    animate_spec;
};
// Received by the slave display nodes
struct internal_spec_t// 6 bytes
{
	uint8_t           image_number;
	block_ref_t       block_ref;
	animate_spec_t    animate_spec;
};
// Stored by the slave display nodes
struct storage_spec_t// 8 bytes
{
	block_spec_t      block_spec;
	animate_spec_t    animate_spec;
};


struct transmission
{
	enum :byte
	{
		INIT,
		HEADER,
		TEXT,
		WING,
		LAST,
		UPDATE,
	}code;
	union{
		uint8_t         	raw [31];
		external_spec_t 	extern_hdr;
		internal_spec_t 	intern_hdr;
		struct {
			uint8_t  size;      //Number of wings in packet
			uint16_t start_indx;//Theta position of first wing to overwrite
			wing     lines[7];  //Array of wings
		}					wing_data;
		char 				text[31];
	};

};
static_assert(sizeof(transmission)==32,RM("Size mismatch."));
//void process_tm( transmission & );



//-----------------------------------------------------------------------------
//		Global Variables
//-----------------------------------------------------------------------------
#ifdef IS_SLAVE
/// Status registers indicating if the sweep has been read
bool cache_dirty[SWEEP_CACHE_POSITIONS];
/// Current sweep being read.
unsigned cache_index {0};
/// The primary cache for the LEDs
sweep cache[SWEEP_CACHE_POSITIONS];
/// Animation information
storage_spec_t	image_store;

uint8_t			image_curr = 0;
uint16_t		frame_rate_counter = 0;
uint16_t		frame_sequence_counter = 0;

SRAM::chip mem_block[SRAM_MAX] = {SRAM::_1, SRAM::_2, SRAM::_3, SRAM::_3};
SRAM::addr_t mem_addr[SRAM_MAX] = {0,0,0,0};
uint8_t mem_curr = 0;
#endif

#ifdef IS_MASTER
external_spec_t 	extern_ctrl;
internal_spec_t 	intern_ctrl;

// Set up nRF24L01 radio on SPI bus
RF24 radio( pins::RF::chipEnable, pins::RF::chipSelect );
// Single radio pipe address for the 2 nodes to communicate.
constexpr const uint64_t pipe = 0x1337BEEF42LL;
#endif


//-----------------------------------------------------------------------------
//		Setup & Loop
//-----------------------------------------------------------------------------
void setup()
{	//configure pins
	pins::HALL::mode(INPUT);
#ifdef IS_SLAVE
	pins::LCD::LE::mode(OUTPUT);
	pins::LCD::OE::mode(OUTPUT);
	pins::LCD::OE::write(HIGH);
#endif
#ifdef IS_MASTER
	pins::RF::CE::mode(OUTPUT);
	pins::RF::CSN::mode(INPUT);
	pins::RF::IRQ::mode(INPUT);
#endif
	//configure SPI bus
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.setBitOrder(LSBFIRST);

#ifdef IS_SLAVE
	//setup SRAM memory
	SRAM::chip::begin();
	init_image_table();
#endif

#ifdef ENABLE_SERIAL
	Serial.begin( BAUD_RATE );

	Timer1.initialize( 1000000 );
	Timer1.attachInterrupt( ram_print );

	load_from_serial();
	Serial.println(F("Load Complete!"));
#endif

#ifdef IS_SLAVE
	//configure I2C bus
	Wire.begin(LEVEL_INDEX+1);    // join i2c bus with address #
	Wire.onReceive(receiveEvent); // Handler when data received
//	Wire.onRequest(requestEvent); // Handler when data requested

	//set sync signal interrupt handler
	attachInterrupt( INT1 , sync_handler, FALLING );

	//set timer interrupt
	Timer1.initialize( THETA_DURATION );
	Timer1.attachInterrupt( theta_handler );
#endif


#ifdef IS_MASTER
	Wire.begin();                // join i2c bus with address #4

	// Setup and configure rf radio
	radio.begin();
	// We will be using the Ack Payload feature, so please enable it
	radio.enableAckPayload();
#if USE_DYNAMIC_PAYLOADS
	// enable dynamic payloads
	radio.enableDynamicPayloads();
#endif
	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15);
	// Open the pipe
	radio.openReadingPipe(1,pipe);
	// Start listening
	radio.startListening();

	// Attach interrupt handler to interrupt #0 (using pin 2)
	// on BOTH the sender and receiver
	attachInterrupt(INT0, check_radio, FALLING);
#endif


}

//-----------------------------------------------------------------------------
void loop()
{
#ifdef IS_SLAVE
	//attempt to fill any dirty caches
	cache_handler();
	//loop execute about 16 times per rotation,
	//	which is about 4 times faster than they can be emptied (with EEPROM)
	delayMicroseconds( THETA_POSITIONS / 16 * THETA_DURATION );
#endif

#ifdef IS_MASTER
#endif

}


 void ram_print(){
	Serial.flush();
	Serial.begin(BAUD_RATE);
	Serial.print(F("\t[[[ RAM = "));
	Serial.print(freeRam(), DEC);
	Serial.print(F(" ]]]\n"));
	Serial.flush();
}

//-----------------------------------------------------------------------------
//		ISR & Helper Function Definitions
//-----------------------------------------------------------------------------
#ifdef IS_SLAVE
inline void theta_handler( void )
{

	SRAM::chip_pause mux;//SRAM paused when object constructed
	const unsigned indx {(cache[cache_index].position)%THETA_POSITIONS};
	//fetch next theta
	const wing & w = cache[cache_index].data[indx];
	//increment
	cache[cache_index].position = indx+1;
	//BEGIN writing to the LED drivers
	pins::LCD::OE::write(HIGH);
	pins::LCD::LE::write(LOW);


	//send lower 2 bytes to 0th LED driver
	SPI.transfer( w.raw[0] );
	SPI.transfer( w.raw[1] );
	pins::LCD::LE::write(HIGH);
	pins::LCD::LE::write(LOW);

	//send higher 2 bytes to 0th LED driver
	SPI.transfer( w.raw[2] );
	SPI.transfer( w.raw[3] );
	pins::LCD::LE::write(HIGH);
	pins::LCD::LE::write(LOW);

	//END writing to the LED drivers
	pins::LCD::OE::write(LOW);


	//increment and check position
	if( cache[cache_index].position >= THETA_POSITIONS+THETA_OFFSET )
	{
		if( ++frame_rate_counter >= image_store.animate_spec.frame_rate )
		{
			frame_rate_counter = 0;
			cache_dirty[cache_index++] = true;
			if( ++frame_sequence_counter >= image_store.animate_spec.frame_length )
			{
				frame_sequence_counter = 0;
				mem_addr[mem_curr] = image_store.block_spec.block_addr;
			}
		}

	}
	//increment and check index
	if( cache_index >= SWEEP_CACHE_POSITIONS )
	{
		cache_index = 0;
	}
	//SRAM released when object exits scope
}
#endif

//-----------------------------------------------------------------------------
#ifdef IS_SLAVE
inline void cache_handler( void )
{
	static unsigned index( 0 );

	//wait for sweep to be used.
	if( !cache_dirty[index] ) return;

	//overwrite swap data with new from SRAM cache
	mem_block[mem_curr].read(mem_addr[mem_curr],cache[index].raw,SWEEP_CACHE_SIZE);
	//advance memory index
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		mem_addr[mem_curr] = (uint32_t(mem_addr[mem_curr]) + SWEEP_CACHE_SIZE);
	}
//	pins::SRAM::CS::write( LOW );
//	for(unsigned ii=0; ii<SWEEP_CACHE_SIZE; ++ii){
//		noInterrupts();
//		cache[index].raw[ii] = SPIC::transfer(cache[index].raw[ii]);
//		interrupts();
//		delayMicroseconds(10);
//	}
//	SPIC::transfer( cache[index].raw, SWEEP_CACHE_SIZE );
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		cache[index].position = THETA_OFFSET;
	}
//	pins::SRAM::CS::write( HIGH );

	//set bit clean
	cache_dirty[index++] = false;

	//reset index if at end of cache
	if( index >= SWEEP_CACHE_POSITIONS ) index = 0;
}
#endif

//-----------------------------------------------------------------------------
#ifdef IS_SLAVE
inline void sync_handler( void )
{
#if THETA_DELAY
	delayMicroseconds(THETA_DELAY);
#endif
	Timer1.restart();
}
#endif

//-----------------------------------------------------------------------------
void process_tm( transmission * tm)
{
	DEBUG(F("\nProcessing transmission:\t"));
	internal_spec_t intern;
	SRAM::addr_t adr;
	switch(tm->code)
	{
	case tm->TEXT:
	{	DEBUG(F("TEXT\n\t"));
		Serial.begin(BAUD_RATE);
		Serial.print(tm->text);
		Serial.end();
		break;
	}
	case tm->HEADER:
	{	DEBUG(F("HEADER\n\t"));
#ifdef IS_SLAVE
		intern = tm->intern_hdr;
		image_curr = intern.image_number;
		mem_curr = intern.block_ref.block_num;
		image_store.animate_spec = intern.animate_spec;
		image_store.block_spec.block_ref = intern.block_ref;
		image_store.block_spec.block_addr = mem_addr[mem_curr];
		mem_block[0].write(sizeof(storage_spec_t)*image_curr, &image_store, sizeof(storage_spec_t) );
		if( mem_addr[0]<(sizeof(storage_spec_t)*(image_curr+1)) ){
			mem_addr[0]=(sizeof(storage_spec_t)*(image_curr+1));
		}
		break;
#endif
#ifdef IS_MASTER
		extern_ctrl = tm->extern_hdr;

		tm->intern_hdr.block_ref   = extern_ctrl.block_ref;
		tm->intern_hdr.animate_spec = extern_ctrl.animate_spec;
		tm->intern_hdr.image_number = extern_ctrl.image_spec.image_number;

		i2c_sendto( extern_ctrl.block_ref.layer_indx,
					const_cast<transmission*>(tm),
					sizeof(tm->code)+sizeof(tm->intern_hdr) );
		break;
#endif

	}
	case tm->WING:
	{	DEBUG(F("WING\n\t"));
#ifdef IS_SLAVE
		adr = mem_addr[image_store.block_spec.block_ref.block_num];
		mem_block[mem_curr].write(	adr, tm->wing_data.lines, tm->wing_data.size*sizeof(wing) );
		mem_addr[mem_curr] = adr + tm->wing_data.size*sizeof(wing);
		break;
#endif
#ifdef IS_MASTER
		i2c_sendto( extern_ctrl.block_ref.layer_indx,
					const_cast<transmission*>(tm),
					sizeof(transmission) );
		break;
#endif


	}
	case tm->INIT:
	{	DEBUG(F("INIT\n\t"));
		break;
	}
	case tm->LAST:
	{	DEBUG(F("LAST\n\t"));
		break;
	}
	case tm->UPDATE:
	{	DEBUG(F("UPDATE\n\t"));
#ifdef IS_SLAVE
		image_curr = tm->intern_hdr.image_number;
		mem_block[0].read(	sizeof(storage_spec_t)*image_curr,
							&image_store,
							sizeof(storage_spec_t)  );
		mem_curr = image_store.block_spec.block_ref.block_num;
		frame_rate_counter = 0;
		frame_sequence_counter = 0;

		break;
#endif
#ifdef IS_MASTER
		extern_ctrl = tm->extern_hdr;

		tm->intern_hdr.block_ref   = extern_ctrl.block_ref;
		tm->intern_hdr.image_number = extern_ctrl.image_spec.image_number;

		i2c_sendto( extern_ctrl.block_ref.layer_indx,
					const_cast<transmission*>(tm),
					sizeof(tm->code)+sizeof(tm->intern_hdr) );
		break;
#endif


	}
	}
}

//-----------------------------------------------------------------------------
void i2c_sendto(byte addr, void * dat, uint16_t len)
{
	byte * ptr = (byte*)dat;
	while(len > BUFFER_LENGTH)
	{
		Wire.beginTransmission(addr);
		Wire.write( ptr, BUFFER_LENGTH );
		len-=BUFFER_LENGTH; ptr += BUFFER_LENGTH;
		Wire.endTransmission(false);
	}
	Wire.beginTransmission(addr);
	Wire.write( ptr, len );
	Wire.endTransmission();
	return;
}


//-----------------------------------------------------------------------------
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
#ifdef IS_SLAVE
void receiveEvent(int howMany)
{
	DEBUG(F("\nI2C receive event!\n"));
	transmission tm;
	Wire.readBytes((uint8_t*)&tm,howMany);
	process_tm(&tm);

//	for(int i =0; i< howMany; ++i){ char c = Wire.read();}

}
#endif

//-----------------------------------------------------------------------------
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
#ifdef IS_SLAVE
void requestEvent()
{
	DEBUG(F("\nI2C request event!\n"));
//  Wire.write("hello "); // respond with message of 6 bytes
  // as expected by master
}
#endif


//-----------------------------------------------------------------------------
#ifdef IS_SLAVE
void init_image_table()
{
	DEBUG(F("Enitializing image table:\n"));
	transmission tm;
	tm.code = tm.HEADER;
	for(uint16_t i=0; i<256; ++i)
	{
		tm.intern_hdr.animate_spec.frame_length = 0;
		tm.intern_hdr.animate_spec.frame_rate = 0;
		tm.intern_hdr.block_ref.block_num = 0;
		tm.intern_hdr.block_ref.layer_indx = LEVEL_INDEX;
		tm.intern_hdr.image_number = i;
		process_tm(&tm);
	}
}
#endif

//-----------------------------------------------------------------------------
#ifdef IS_MASTER
void check_radio(void)
{
	// What happened?
	bool tx,fail,rx;
	radio.whatHappened(tx,fail,rx);
	DEBUG(F("RF_IRQ:\t"));
	// Have we successfully transmitted?
	if ( tx )
	{	DEBUG(F("TX\t"));

	}

	// Have we failed to transmit?
	if ( fail )
	{	DEBUG(F("FAIL\t"));

	}

	// Have we received a message?
	if ( rx )
	{	DEBUG(F("RX\t"));
		transmission tmp;
#if USE_DYNAMIC_PAYLOADS
		uint8_t len = radio.getDynamicPayloadSize();
		radio.read( &tmp, len );
#else
		radio.read( &tmp, sizeof(transmission) );
#endif
		process_tm( &tmp );
	}
	DEBUG(F("\n"));
}
#endif

//-----------------------------------------------------------------------------
uint8_t to_digit(char c)
{
	switch(c){
	case '0': return 0x0;
	case '1': return 0x1;
	case '2': return 0x2;
	case '3': return 0x3;
	case '4': return 0x4;
	case '5': return 0x5;
	case '6': return 0x6;
	case '7': return 0x7;
	case '8': return 0x8;
	case '9': return 0x8;
	case 'a': case 'A': return 0xA;
	case 'b': case 'B': return 0xB;
	case 'c': case 'C': return 0xC;
	case 'd': case 'D': return 0xD;
	case 'e': case 'E': return 0xE;
	case 'f': case 'F': return 0xF;
	}
	return -1;
}
//-----------------------------------------------------------------------------
unsigned inline to_number(Stream * ss){
	return to_number(ss,false);
}
unsigned to_number(Stream * ss, bool must)
{
	uint8_t base=0;
	unsigned accum=0;
	char chr,ind;
	do{
		while(ss->available()<2){}
		chr=ss->read();
		if(must && chr==EOF)continue;
		switch(base)
		{
		case 0:
		{
			switch(chr)
			{
			case '0':
			{
				ind = ss->peek();
				switch(ind)
				{
				case 'B': case 'b':
					base= 2; ss->read(); continue;
				case 'X': case 'x':
					base=16; ss->read(); continue;
				case 'O': case 'o':
					base= 8; ss->read(); continue;
				case '#':
					base=10; ss->read(); continue;
				default:
					if(isdigit(ind)){
						base=8;
					}else{
						return accum;
					}
					continue;
				}
				continue;
			}
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				base=10;
				accum = to_digit(chr);
				continue;
			}
			default:
				continue;
			}
		}
		case 2:
			if(to_digit(chr)<2){
				accum = (accum<<1)|to_digit(chr);
			}else{
				return accum;
			}
			continue;
		case 8:
			if(to_digit(chr)<8){
				accum = (accum<<3)|to_digit(chr);
			}else{
				return accum;
			}
			continue;
		case 10:
			if(isdigit(chr)){
				accum = (accum*10)+to_digit(chr);
			}else{
				return accum;
			}
			continue;
		case 16:
			if(isxdigit(chr)){
				accum = (accum<<4)|to_digit(chr);
			}else{
				return accum;
			}
			continue;
		default:
			return EOF;
		}
	}while(must||chr!=EOF);
	return EOF;
}


//-----------------------------------------------------------------------------
void load_from_serial()
{
	Serial.begin(BAUD_RATE);
	Serial.println(F("Image number?\n"));
	uint8_t image_num = to_number(&Serial,true);

	Serial.println(F("Block number?\n"));
	uint8_t block_num = to_number(&Serial,true);

#ifdef IS_MASTER
	Serial.println(F("Layer number?\n"));
	uint8_t layer_num = to_number(&Serial,true);
#endif
	Serial.println(F("Frame rate? (0 for static image)\n"));
	uint8_t frame_rate = to_number(&Serial,true);
	Serial.end();
	transmission tm_h;
	tm_h.code = tm_h.HEADER;
	tm_h.intern_hdr.animate_spec.frame_length = 0;
	tm_h.intern_hdr.animate_spec.frame_rate = frame_rate;
	tm_h.intern_hdr.block_ref.block_num = block_num;
#ifdef IS_SLAVE
	tm_h.intern_hdr.block_ref.layer_indx = LEVEL_INDEX;
#endif
#ifdef IS_MASTER
	tm_h.intern_hdr.block_ref.layer_indx = layer_num;
#endif
	tm_h.intern_hdr.image_number = image_num;
	process_tm(&tm_h);

	SERIAL(F("Header is processed.\n"));
//	Serial.flush();
	transmission tm_w;
	tm_w.code = tm_w.WING;

	uint16_t wing_count=0;
	while(Serial.available()<1){}
	do{
		static_assert(sizeof(tm_w.wing_data)/sizeof(wing)==7,"");
		for(uint8_t j=0; j<sizeof(tm_w.wing_data)/sizeof(wing); ++j)
		{
			Serial.begin(BAUD_RATE);
			for(uint8_t i=0; i<sizeof(wing); ++i)
			{
				while(Serial.available()<1){}
				tm_w.wing_data.lines[j].raw[i] = to_number(&Serial,true);
//				Serial.begin(BAUD_RATE);
				Serial.print(F(" "));
				Serial.print(tm_w.wing_data.lines[j].raw[i],HEX);
				Serial.print(F(" "));
//				Serial.end();
			}

			Serial.print(F("-> ("));
			Serial.print(wing_count);
			Serial.print(F(","));
			Serial.print(tm_h.intern_hdr.animate_spec.frame_length);
			Serial.print(F(")\n"));
			Serial.end();
			tm_w.wing_data.size+=1;
			wing_count+=1;
		}
		//------------
		process_tm(&tm_w);
		tm_w.wing_data.size=0;
		if(wing_count==THETA_POSITIONS)
		{
			Serial.begin(BAUD_RATE);
			Serial.print(F("Frame "));
			Serial.print(tm_h.intern_hdr.animate_spec.frame_length);
			Serial.print(F(" is processed.\n"));
			Serial.end();
			tm_h.intern_hdr.animate_spec.frame_length+=1;
			wing_count=0;
			Serial.begin( BAUD_RATE );
			for(bool decide=0; !decide; )
			{
				Serial.println(F("Continue? [y/n]"));
				while(Serial.peek()==EOF){}
				switch(Serial.read()){
				case 'y': case 'Y': decide=true; break;
				case 'n': case 'N': return;
				default: decide=false; break;
				}
			}
			Serial.end();
		}
	}while(false&&Serial.available()>1);
}


