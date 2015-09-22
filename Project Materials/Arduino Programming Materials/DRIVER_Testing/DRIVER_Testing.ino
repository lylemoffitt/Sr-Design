 // include the SPI library:
#include <SPI.h>

struct fin { byte lsb,msb; };

unsigned i = 0;

void setup() {
  // Initialize I/O
  DDRC = DDRC | B0111111;
  DDRD = DDRD | B11111000;
  // initialize SPI:
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  
  PORTC = B0001111;
  PORTD = B10100000;
}

void loop() {
   delay(50);
   timing();
   while(1){}
}

void timing(){
   //Runs when falling edge from Schmitt Trigger is seen
     for (i=0; i<256; ++i) {
         LEDWrite();
         delay(50);
     }
   delay(1000);
   PORTD = B10100000;
}

void LEDWrite() {
  PORTD = 0b10100000;
  SPI.transfer(i);
  SPI.transfer(i);
  SPI.transfer(i);
  SPI.transfer(i);
  PORTD = 0b11100000;
  delayMicroseconds(59);
  PORTD = 0b00100000;
  delayMicroseconds(59);
}
