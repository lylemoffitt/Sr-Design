// include the SPI library:
#include <SPI.h>

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;

struct fin { byte lsb,msb; };

unsigned i = 0;

void setup() {
  //pinMode(out_en_pin, OUTPUT);      // sets the digital pin as output
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  attachInterrupt(1,timing,FALLING);
  DDRD = DDRD | B11110000;
  // initialize SPI:
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(LSBFIRST);
  
  PORTD = B11110000;
}

void loop() {
   
}

void timing(){
   //Runs when falling edge from Schmitt Trigger is seen
     for (i=0; i<256; ++i) {
         LEDWrite();
     }
   PORTD = B11110000;
}

void LEDWrite() {
  PORTD = 0b01110000;
  SPI.transfer((i >> 1) ^ i);
  SPI.transfer((i >> 1) ^ i);
  PORTD = 0b11110000;
  delayMicroseconds(61);
  PORTD = 0b01010000;
  delayMicroseconds(61); 
}
