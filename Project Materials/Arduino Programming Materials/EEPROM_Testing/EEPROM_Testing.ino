#include <SPI.h>

const int SS_Pin = 10;

byte readInstr = b00000011;
byte address[3] = {0x00,0x00,0x00};
byte Data_in[4] = {0x00,0x00,0x00,0x00};

void setup(){
  pinMode (SS_Pin, OUTPUT);
  DDRD = B11111110;
  // initialize SPI:
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  Serial.begin(9600);
}

void loop(){

  PORTD = B11100000;
  SPI.transfer(readInstr);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);
  Data_in[0] = SPI.transfer(0);
  Data_in[1] = SPI.transfer(0);
  Data_in[2] = SPI.transfer(0);
  Data_in[3] = SPI.transfer(0);
  PORTD = B11110000;
  Serial.println(Data_in[0],BIN);
  Serial.println(Data_in[1],BIN);
  Serial.println(Data_in[2],BIN);
  Serial.println(Data_in[3],BIN);
  
  while(1){}
}
