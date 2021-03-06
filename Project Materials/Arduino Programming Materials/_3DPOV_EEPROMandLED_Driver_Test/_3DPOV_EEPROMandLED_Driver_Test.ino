#include <SPI.h>

const int SS_Pin = 10;

byte status_reg = 0;

byte address[3] = {0x00,0x00,0x00};

struct fin { byte lsb,msb; };

static fin data[] = 
{
  {0x00,0x00},{0x77,0x00},{0x88,0x80},{0x88,0x80},{0x88,0x80},{0x80,0x80},{0x80,0x80},{0x00,0x00}, //0-7
  {0x00,0x00},{0x07,0x00},{0x08,0x80},{0x08,0x80},{0x08,0x80},{0x08,0x80},{0x08,0x80},{0xFF,0x00}, //8-15
  {0x00,0x00},{0x00,0x00},{0xFF,0x80},{0x80,0x00},{0x80,0x00},{0x80,0x00},{0x80,0x00},{0x80,0x00}, //16-23
  {0xFF,0x80},{0x00,0x00},{0x00,0x00},{0x7F,0x00},{0x80,0x80},{0x80,0x80},{0x80,0x80},{0x80,0x80}, //24-31
  {0x80,0x80},{0x7F,0x00},{0x00,0x00},{0x00,0x00},{0xF7,0x00},{0x08,0x80},{0x08,0x80},{0x08,0x80}, //32-39
  {0x08,0x80},{0x08,0x80},{0xFF,0x80},{0x00,0x00},{0x20,0x00},{0x63,0x00},{0xA0,0x80},{0x80,0x80}, //40-47
  {0x80,0x80},{0x80,0x80},{0x7F,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //48-55
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x01,0x80},{0x06,0x00},{0x18,0x00},{0x60,0x00},{0x80,0x00}, //56-63
  {0x60,0x00},{0x18,0x00},{0x06,0x00},{0x01,0x80},{0x00,0x00},{0x00,0x00},{0x7F,0x00},{0x80,0x80}, //64-71
  {0x80,0x80},{0x80,0x80},{0x80,0x80},{0x80,0x80},{0x7F,0x00},{0x00,0x00},{0x00,0x00},{0x07,0x00}, //72-79
  {0x08,0x80},{0x08,0x80},{0x08,0x80},{0x08,0x80},{0x08,0x80},{0xFF,0x80},{0x00,0x00},{0x00,0x00}, //80-87
  {0x7F,0x00},{0x80,0x80},{0x80,0x80},{0x80,0x80},{0x80,0x80},{0x80,0x80},{0xFF,0x80},{0x00,0x00}, //88-95
  {0x00,0x00},{0x77,0x00},{0x88,0x80},{0x88,0x80},{0x88,0x80},{0x88,0x80},{0x80,0x80},{0x80,0x80}, //96-103
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //104-111
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //112-119
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //120-127
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //128-135
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //136-143
  {0xFF,0x80},{0x08,0x00},{0x08,0x00},{0x08,0x00},{0x08,0x00},{0x08,0x00},{0xFF,0x80},{0x00,0x00}, //144-151
  {0x00,0x00},{0xFF,0x80},{0x88,0x80},{0x88,0x80},{0x88,0x80},{0x88,0x80},{0x80,0x80},{0x80,0x80}, //152-159
  {0x00,0x00},{0x00,0x00},{0xFF,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x80}, //160-167
  {0x00,0x80},{0x00,0x00},{0x00,0x00},{0xFF,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x80}, //168-175
  {0x00,0x80},{0x00,0x80},{0x00,0x00},{0x00,0x00},{0x7F,0x00},{0x80,0x80},{0x80,0x80},{0x80,0x80}, //176-183
  {0x80,0x80},{0x80,0x80},{0x7F,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //184-191
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0xFF,0x00},{0x00,0x80},{0x00,0x80},{0x03,0x00},{0x00,0x80}, //192-199
  {0x00,0x80},{0xFF,0x00},{0x00,0x00},{0x00,0x00},{0x7F,0x00},{0x80,0x80},{0x80,0x80},{0x80,0x80}, //200-207
  {0x80,0x80},{0x80,0x80},{0x7F,0x00},{0x00,0x00},{0x00,0x00},{0xFF,0x80},{0x88,0x00},{0x88,0x00}, //208-215
  {0x88,0x00},{0x88,0x00},{0x88,0x00},{0x77,0x80},{0x00,0x00},{0x00,0x00},{0xFF,0x80},{0x00,0x80}, //216-223
  {0x00,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x80},{0x00,0x00},{0x00,0x00},{0xFF,0x80}, //224-231
  {0x80,0x80},{0x80,0x80},{0x80,0x80},{0x80,0x80},{0x80,0x80},{0x7F,0x00},{0x00,0x00},{0x00,0x00}, //232-239
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //240-247
  {0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00},{0x00,0x00}, //248-255
};

fin read_back[256] = {};

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
  
  //Read Status Register

  PORTD = B11100000;
  SPI.transfer(instr.RDSR);
  status_reg = SPI.transfer(0);
  PORTD = B11110000;
  
  Serial.begin(9600);
  Serial.print("Status REG = ");
  Serial.println(status_reg,BIN);

  Serial.println("Setting WREN");
  Serial.end();
  
  //Enable Write Enable Latch
  
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
  
  //Write to EEPROM
  
  PORTD = B11100000;
  SPI.transfer(instr.WRITE);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);
  
  int i=0;
  
  for(int i=0; i<128; i++){
    SPI.transfer(data[i].lsb);
    SPI.transfer(data[i].msb);
  }
  
  PORTD = B11110000;
  
  address[1] = 0x01;
  
  delay(15);
  
  PORTD = B11100000;
  SPI.transfer(instr.WREN);
  PORTD = B11110000;
  
  PORTD = B11100000;
  SPI.transfer(instr.WRITE);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);
  
  for(int i=128; i<256; i++){
    SPI.transfer(data[i].lsb);
    SPI.transfer(data[i].msb);
  }
  
  PORTD = B11110000;
  
  delay(6);
  
  PORTD = B11100000;
  SPI.transfer(instr.RDSR);
  status_reg = SPI.transfer(0);
  PORTD = B11110000;
  
  Serial.begin(9600);
  Serial.print("Status REG = ");
  Serial.println(status_reg,BIN);

  Serial.println("Beginning READ");
  Serial.end();
  
  //Read from EEPROM
  
  address[1] = 0x00;
  
  PORTD = B11100000;
  SPI.transfer(instr.READ);
  SPI.transfer(address[0]);
  SPI.transfer(address[1]);
  SPI.transfer(address[2]);
  
  for(int i=0; i<256; i++){
    read_back[i].lsb = SPI.transfer(0);
    read_back[i].msb = SPI.transfer(0);
  }
  
  PORTD = B11110000;

  Serial.begin(9600);
  Serial.println("Printing READ result");
  
  for(int i=0; i<256; i++){
    Serial.print("Data[");
    Serial.print(i);
    Serial.print("].lsb = ");
    Serial.println(read_back[i].lsb,HEX);
    Serial.print("Data[");
    Serial.print(i);
    Serial.print("].msb = ");
    Serial.println(read_back[i].msb,HEX);
  }
  Serial.end();
  
  //Read status register
  
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


