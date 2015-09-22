// include the SPI library:
#include <SPI.h>

struct fin { byte lsb,msb; };

unsigned i = 0;

void setup() {
  // Initialize I/O
  DDRC = DDRC | B0111111;
  DDRD = DDRD | B11111000;
  attachInterrupt(0,timing,FALLING);
  // initialize SPI:
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  
  PORTC = B0001111;
  PORTD = B10100000;
}

void loop() {
   
}

void timing(){
   //Runs when falling edge from Schmitt Trigger is seen
     for (i=0; i<256; ++i) {
         LEDWrite();
     }
   PORTD = B10100000;
}

void LEDWrite() {
  PORTD = 0b10100000;
  SPI.transfer((i >> 1) ^ i);
  SPI.transfer((i >> 2) ^ i);
  SPI.transfer((i >> 3) ^ i);
  SPI.transfer((i >> 4) ^ i);
  PORTD = 0b11100000;
  delayMicroseconds(59);
  PORTD = 0b00100000;
  delayMicroseconds(59); 
}
