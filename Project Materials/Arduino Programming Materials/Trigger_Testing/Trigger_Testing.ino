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
  Serial.begin(9600);
  Serial.println('It works :^>');
  Serial.end();
}

