#include <SPI.h>

const int SS_Pin = 10;

byte address[3] = {0x00,0x00,0x00};

struct fin { byte lsb,msb; };

//volatile fin data = {};

struct instruction{
  const static byte READ   = B00000011;
  const static byte WRITE  = B00000010;
  const static byte WREN   = B00000110;
  const static byte WRDI   = B00000100;
  const static byte RDSR   = B00000101;
  const static byte WRSR   = B00000001;
  const static byte PE_    = B01000010;
  const static byte SE_    = B11011000;
  const static byte CE_    = B11000111;
  const static byte RDID   = B10101011;
  const static byte DPD    = B10111001;
};
struct instruction instr;

unsigned i = 0;

void setup(){
  pinMode (SS_Pin, OUTPUT);
  attachInterrupt(1,timing,FALLING);
  DDRD = DDRD | B11110000;
  // initialize SPI:
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  
  PORTD = 11110000;
}

void loop(){
}

void timing(){
   //Runs when falling edge from Schmitt Trigger is seen
  E2PROM_Preamble();
  for(i=0; i<256; i++){
     fin temp = {};
     temp = E2PROM_Reading(temp);
     LEDWrite(temp);
  }
  PORTD = B11110000;
}

void E2PROM_Preamble(){
   PORTD = B11100000;
   SPI.transfer(instr.READ);
   SPI.transfer(address[0]);
   SPI.transfer(address[1]);
   SPI.transfer(address[2]);
}

struct fin E2PROM_Reading( struct fin temp ){
  PORTD = B11100000;
  temp.lsb = SPI.transfer(0);
  temp.msb = SPI.transfer(0);
  PORTD = B10100000;
  return temp;
}

void LEDWrite( struct fin dat ) {
  SPI.setBitOrder(LSBFIRST);
  PORTD = 0b00100000;
  SPI.transfer(dat.msb);
  SPI.transfer(dat.lsb);
  PORTD = 0b11000000;
  SPI.setBitOrder(MSBFIRST);
  delayMicroseconds(110);
}


