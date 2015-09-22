#include <SPI.h>

const int SS_Pin = 10;

byte status_reg = 0;
byte read_back[256] = {};
byte address[3] = {0x00,0x00,0x00};

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

void setup(){
  pinMode (SS_Pin, OUTPUT);
  DDRD = DDRD | B11111100;
  // initialize SPI:
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  
  PORTD = B11110000;
}

void loop(){
  
  PORTD = B11100000;
  SPI.transfer(instr.WREN);
  PORTD = B11110000;
  
  PORTD = B11100000;
  SPI.transfer(instr.WRITE);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);

  for(int i=0; i<32; i++){
    SPI.transfer(i);
  }
  PORTD = B11110000;
  
  PORTD = B11100000;
  SPI.transfer(instr.READ);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);
  
  for(int i=0; i<32; i++){
    read_back[i] = SPI.transfer(i);
  }
  PORTD = B11110000;
  
  while(1){}
}


