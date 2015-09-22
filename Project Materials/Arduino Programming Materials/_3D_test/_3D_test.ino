// include the SPI library:
#include <SPI.h>

byte led[][4 ] = {{0x80,0x00,0x00,0x00},{0x40,0x00,0x00,0x00},{0x20,0x00,0x00,0x00},{0x10,0x00,0x00,0x00},
                  {0x08,0x00,0x00,0x00},{0x04,0x00,0x00,0x00},{0x02,0x00,0x00,0x00},{0x01,0x00,0x00,0x00},
                  {0x00,0x80,0x00,0x00},{0x00,0x40,0x00,0x00},{0x00,0x20,0x00,0x00},{0x00,0x10,0x00,0x00},
                  {0x00,0x08,0x00,0x00},{0x00,0x04,0x00,0x00},{0x00,0x02,0x00,0x00},{0x00,0x01,0x00,0x00},
                  {0x00,0x00,0x80,0x00},{0x00,0x00,0x40,0x00},{0x00,0x00,0x20,0x00},{0x00,0x00,0x10,0x00},
                  {0x00,0x00,0x08,0x00},{0x00,0x00,0x04,0x00},{0x00,0x00,0x02,0x00},{0x00,0x00,0x01,0x00},
                  {0x00,0x00,0x00,0x80},{0x00,0x00,0x00,0x40},{0x00,0x00,0x00,0x20},{0x00,0x00,0x00,0x10},
                  {0x00,0x00,0x00,0x08},{0x00,0x00,0x00,0x04},{0x00,0x00,0x00,0x02},{0x00,0x00,0x00,0x01}};

unsigned i = 0;
int j = 0;
boolean up = true;

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
   if(up){
     j++;
     if(32 == j){    
       up = false;
       j--;
     }
   }
   else{
     j--;
     if(-1 == j){
       up = true;
       j++;
     }  
   }
}

void LEDWrite() {
  PORTD = 0b10100000;
  SPI.transfer(led[j][0]);
  SPI.transfer(led[j][1]);
  SPI.transfer(led[j][2]);
  SPI.transfer(led[j][3]);
  PORTD = 0b11100000;
  //delayMicroseconds(59);
  PORTD = 0b00100000;
  delayMicroseconds(118); 
}