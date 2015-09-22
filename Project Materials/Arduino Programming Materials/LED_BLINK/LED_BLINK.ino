// include the SPI library:
#include <SPI.h>

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;

const int LED = 13;
static bool LEDstate;
struct fin { byte lsb,msb; };

unsigned i = 0;

void setup() {
  //pinMode(out_en_pin, OUTPUT);      // sets the digital pin as output
  // set the slaveSelectPin as an output:
//  pinMode (slaveSelectPin, OUTPUT);
  pinMode(LED, OUTPUT);
  
  attachInterrupt(1,timing,FALLING);
//  DDRD = DDRD | B11110000;
//  // initialize SPI:
//  SPI.begin();
//  SPI.setClockDivider(SPI_CLOCK_DIV2);
//  SPI.setBitOrder(LSBFIRST);
//  
//  PORTD = B11110000;
  LEDstate=true;
}

void loop() {
   
}

void timing(){

  if(LEDstate){
    digitalWrite(LED,HIGH);
  }else{
    digitalWrite(LED,LOW);
  }
  LEDstate = !LEDstate;
}
