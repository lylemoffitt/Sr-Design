/*
 SD card test

 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
 Pin 4 used here for consistency with other Arduino examples


 created  28 Mar 2011
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe

 edited  12 Feb 2015
 by Lyle Moffitt
 */
// include the SD library:
#include <SPI.h>
#include "SD_SPI.h"

// set up variables using the SD utility library functions:
Sd2Card card;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const uint8_t SdChipSelect = 4;

// global for card size
uint32_t cardSize;

// global for card erase size
uint32_t eraseSize;
//------------------------------------------------------------------------------
// store error strings in flash
#define sdErrorMsg(msg) sdErrorMsg_P(PSTR(msg));
void sdErrorMsg_P( const char* str )
{
	Serial.println( str );
	if( card.errorCode() )
	{
		Serial.print( "SD errorCode: " );
		Serial.println( card.errorCode(), HEX );
		Serial.print( "SD errorData: " );
		Serial.println( card.errorData(), HEX );
	}
}
//------------------------------------------------------------------------------
uint8_t cidDmp()
{
	cid_t cid;
	if( !card.readCID( &cid ) )
	{
		sdErrorMsg( "readCID failed" );
		return false;
	}
	Serial.print( "\nManufacturer ID: " );
	Serial.println( cid.mid, HEX );
	Serial.print( "OEM ID: " );
	Serial.print( cid.oid[0] );
	Serial.println( cid.oid[1] );
	Serial.print( "Product: " );
	for( uint8_t i = 0; i < 5; i++ )
	{
		Serial.print( cid.pnm[i] );
	}
	Serial.print( "\nVersion: " );
	Serial.print( cid.prv_n );
	Serial.print( "." );
	Serial.println( cid.prv_m );
	Serial.print( "Serial number: " );
	Serial.println( cid.psn, HEX );
	Serial.print( "Manufacturing date: " );
	Serial.print( cid.mdt_month );
	Serial.print( '/' );
	Serial.println( 2000 + cid.mdt_year_low + 10 * cid.mdt_year_high );
	Serial.println();
	return true;
}
//------------------------------------------------------------------------------
uint8_t csdDmp()
{
	csd_t csd;
	uint8_t eraseSingleBlock;
	if( !card.readCSD( &csd ) )
	{
		sdErrorMsg( "readCSD failed" );
		return false;
	}
	if( csd.v1.csd_ver == 0 )
	{
		eraseSingleBlock = csd.v1.erase_blk_en;
		eraseSize = (csd.v1.sector_size_high << 1) | csd.v1.sector_size_low;
	}
	else if( csd.v2.csd_ver == 1 )
	{
		eraseSingleBlock = csd.v2.erase_blk_en;
		eraseSize = (csd.v2.sector_size_high << 1) | csd.v2.sector_size_low;
	}
	else
	{
		Serial.print( "csd version error\n" );
		return false;
	}
	eraseSize++;
	Serial.print( "cardSize: " );
	Serial.print( 0.000512 * cardSize );
	Serial.print( " MB (MB = 1,000,000 bytes)\n" );

	Serial.print( "flashEraseSize: " );
	Serial.print( eraseSize );
	Serial.print( " blocks\n" );
	Serial.print( "eraseSingleBlock: " );
	if( eraseSingleBlock )
	{
		Serial.print( "true\n" );
	}
	else
	{
		Serial.print( "false\n" );
	}
	return true;
}

//------------------------------------------------------------------------------
void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin( 9600 );
	while( !Serial )
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}

	Serial.print( "\nInitializing SD card..." );
	// On the Ethernet Shield, CS is pin 4. It's set as an output by default.
	// Note that even if it's not used as the CS pin, the hardware SS pin
	// (10 on most Arduino boards, 53 on the Mega) must be left as an output
	// or the SD library functions will not work.
	pinMode( 10, OUTPUT );     // change this to 53 on a mega

	// we'll use the initialization code from the utility libraries
	// since we're just testing if the card is working!
	if( !card.init( SPI_HALF_SPEED, SdChipSelect ) )
	{
		Serial.println( "initialization failed. Things to check:" );
		Serial.println( "* is a card is inserted?" );
		Serial.println( "* Is your wiring correct?" );
		Serial.println(
				"* did you change the chipSelect pin to match your shield or module?" );
		return;
	}
	else
	{
		Serial.println( "Wiring is correct and a card is present." );
	}

}
//------------------------------------------------------------------------------
void loop()
{
	// read any existing Serial data
	while( Serial.read() >= 0 )
	{
	}

	// pstr stores strings in flash to save RAM
	Serial.print( "\ntype any character to start\n" );
	while( Serial.read() <= 0 )
	{
	}
	delay( 400 );  // catch Due reset problem

	uint32_t t = millis();
	// initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
	// breadboards.  use SPI_FULL_SPEED for better performance.
	if( !card.init( SPI_HALF_SPEED, SdChipSelect ) )
	{
		sdErrorMsg( "\ncard.init failed" );
		return;
	}
	t = millis() - t;

	cardSize = card.cardSize();
	if( cardSize == 0 )
	{
		sdErrorMsg( "cardSize failed" );
		return;
	}
	Serial.print( "\ninit time: " );
	Serial.print( t );
	Serial.println( " ms" );
	Serial.print( "\nCard type: " );
	switch( card.type() )
	{
	case SD_CARD_TYPE::SD1:
		Serial.print( "SD1\n" );
		break;

	case SD_CARD_TYPE::SD2:
		Serial.print( "SD2\n" );
		break;

	case SD_CARD_TYPE::SDHC:
		if( cardSize < 70000000 )
		{
			Serial.print( "SDHC\n" );
		}
		else
		{
			Serial.print( "SDXC\n" );
		}
		break;

	default:
		Serial.print( "Unknown\n" );
	}
	if( !cidDmp() ) return;
	if( !csdDmp() ) return;
	uint32_t ocr;
	if( !card.readOCR( &ocr ) )
	{
		sdErrorMsg( "\nreadOCR failed" );
		return;
	}
	Serial.print( "OCR: " );
	Serial.println( ocr, HEX );

}
