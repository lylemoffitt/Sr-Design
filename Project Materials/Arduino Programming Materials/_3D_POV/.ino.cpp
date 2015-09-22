//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2015-04-17 23:12:08

#include "Arduino.h"
#include <fast_Digital.h>
#include <SPI_SRAM.h>
#include <Arduino.h>
#include <TimerOne.h>
#include <SPI.h>
#include <Wire.h>
#include <stdint.h>
#include <stddef.h>
#include <util/atomic.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "predef.h"
void setup() ;
void loop() ;
void ram_print();
inline void theta_handler( void ) ;
inline void cache_handler( void ) ;
inline void sync_handler( void ) ;
void process_tm( transmission * tm) ;
void i2c_sendto(byte addr, void * dat, uint16_t len) ;
void receiveEvent(int howMany) ;
void requestEvent() ;
void init_image_table() ;
uint8_t to_digit(char c) ;
unsigned inline to_number(Stream * ss);
unsigned to_number(Stream * ss, bool must) ;
void load_from_serial() ;


#include "_3D_POV.ino"
