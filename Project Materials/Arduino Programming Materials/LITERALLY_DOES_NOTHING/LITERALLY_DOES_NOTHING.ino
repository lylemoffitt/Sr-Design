// include the SPI library:
#include <SPI.h>

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
     }
   PORTD = B10100000;
}

void LEDWrite() {
  PORTD = 0b10100000;
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  PORTD = 0b11100000;
  delayMicroseconds(59);
  PORTD = 0b00100000;
  delayMicroseconds(59); 
}
