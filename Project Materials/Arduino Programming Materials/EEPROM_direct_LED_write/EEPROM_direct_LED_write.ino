#include <SPI.h>

const int SS_Pin = 10;

#define LED_write_begin PORTD &= ~B00001000 //disable LE (pin3)
#define LED_write_end   PORTD &= B00001000  //enable LE (pin3)

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
  
  PORTD = 11110000;
}

void loop(){

  PORTD = B11100000;
  SPI.transfer(instr.RDSR);
  status_reg = SPI.transfer(0);
  PORTD = B11110000;
  
  Serial.begin(9600);
  Serial.print("Status REG = ");
  Serial.println(status_reg,BIN);

  Serial.println("Setting WREN");
  Serial.end();
  
  PORTD = B11100000;
  SPI.transfer(instr.WREN);
  PORTD = B11110000;

  PORTD = B11100000;
  SPI.transfer(instr.RDSR);
  status_reg = SPI.transfer(0);
  PORTD = B11110000;

  Serial.begin(9600);
  Serial.print("Status REG = ");
  Serial.println(status_reg,BIN);

  Serial.println("Beginning WRITE");
  Serial.end();
  
  PORTD = B11100000;
  SPI.transfer(instr.WRITE);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);

  for(int i=0; i<256; i++){
    SPI.transfer(i);
  }
  PORTD = B11110000;
  
  PORTD = B11100000;
  SPI.transfer(instr.RDSR);
  status_reg = SPI.transfer(0);
  PORTD = B11110000;
  
  Serial.begin(9600);
  Serial.print("Status REG = ");
  Serial.println(status_reg,BIN);

  Serial.println("Beginning READ");
  Serial.end();
  
  PORTD = B11100000;
  SPI.transfer(instr.READ);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);
  
  for(int i=0; i<256; i++){
    read_back[i] = SPI.transfer(i);
  }
  PORTD = B11110000;

  Serial.begin(9600);
  Serial.println("Printing READ result");
  
  for(int i=0; i<256; i++){
    Serial.print("Data in Address[");
    Serial.print(i,HEX);
    Serial.print("] = ");
    Serial.println(read_back[i],HEX);
  }
  Serial.end();

  PORTD = B11100000;
  SPI.transfer(instr.RDSR);
  status_reg = SPI.transfer(0);
  PORTD = B11110000;
  
  Serial.begin(9600);
  Serial.print("Status REG = ");
  Serial.println(status_reg,BIN);
  Serial.end();

  while(1){}
}

void LEDWrite( struct fin dat ) {
  PORTD &= ~B00001000;//disable LE (pin3)
  SPI.transfer(dat.msb);
  SPI.transfer(dat.lsb);
  delayMicroseconds(122);
  PORTD &= B00001000;//enable LE (pin3)
}
