#include <SPI.h>

byte nReadBack[4][256] = {}; 
byte pAddress[3] = {0x00,0x00,0x00};
byte nNumSRAM = 4;
byte nModeRegStatus[4] = {0x00,0x00,0x00,0x00};
byte nPORTC[4] = {B0001110,B0001101,B0001011,B0000111};

struct instruction{
  const static byte READ   = 0x03; // Read data from mem. array beggining at selected address
  const static byte WRITE  = 0x02; // Write data to mem. array begging at selected address
  const static byte EDIO   = 0x3B; // Enter Dual I/O access
  const static byte EQIO   = 0x38; // Enter Quad I/O access
  const static byte RSTIO  = 0xFF; // Reset Dual and Quad I/O access
  const static byte RDMR   = 0x05; // Read Mode Register (MR defaults to Sequential Mode = 0b01000000)
  const static byte WRMR   = 0x01; // Write Mode Register
};
struct instruction instr;

void setup(){
  DDRC = DDRC | B0111111;
  DDRD = DDRD | B11111000;
  // initialize SPI:
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  // Port High/Low initialization
  PORTC = B0001111;  // set chip selects High -Can also set up I2C with byte 4 & 5-
  PORTD = B10100000; // set ~OE,~HOLD High, LE Low
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void loop(){
  // Set Data in, Data out, and ~HOLD high
  digitalWrite(11,HIGH);
  digitalWrite(12,HIGH);
  // Attempt SDI reset
  PORTC = nPORTC[0]; // drop chip select
  for(int i = 0; i<2; i++){ 
    digitalWrite(13,HIGH);
    digitalWrite(13,LOW);
  }
  PORTC = B0001111;
  
  // 1 - Read mode register
  for(byte i=0; i<nNumSRAM; i++){
    // Read M.R. of SRAM I
      ReadMR(i);
      
    // Print Results 
      Serial.begin(9600);
        Serial.print("Mode Register of SRAM ");
        Serial.print(i,DEC);
        Serial.print(" = ");
        Serial.println(nModeRegStatus[i],BIN);
      Serial.end();
  }
        
  // 2 - Confirm sequential mode, otherwise put in sequential mode
    
    for(byte i=0; i<nNumSRAM; i++){
     if(B01000000 != nModeRegStatus[i]){
        WriteMR(i);
      }  
    }
  
  Serial.begin(9600);
  Serial.println("Beggining Write");
  Serial.end();
  
  // 3 - Write Stuff to each SRAM
  
  for(byte i=0; i<nNumSRAM; i++){
    // Write to SRAM i
      WriteSRAM(i);
  }
  
  Serial.begin(9600);
  Serial.println("Beggining Read");
  Serial.end();
      
  // 4 - Read Stuff on each SRAM
  for(byte i=0; i<nNumSRAM; i++){
      // Read from SRAM i
        ReadSRAM(i);
 
      // Print Results
        Serial.begin(9600);
        for(int j=0; j<256; j++){
            Serial.print("SRAM ");
            Serial.print(i,DEC);
            Serial.print(", Block ");
            Serial.print(j,HEX);
            Serial.print(" = ");
            Serial.println(nReadBack[i][j],HEX);
        }
        Serial.end();
  }
  
    for(byte i=0; i<nNumSRAM; i++){
    // Read M.R. of SRAM I
      ReadMR(i);
      
    // Print Results 
      Serial.begin(9600);
        Serial.print("Mode Register of SRAM ");
        Serial.print(i,DEC);
        Serial.print(" = ");
        Serial.println(nModeRegStatus[i],BIN);
      Serial.end();
  }

  while(1){}
}

void ReadMR(byte i){
  PORTC = nPORTC[i];
  SPI.transfer(instr.RDMR);
  nModeRegStatus[i] = SPI.transfer(0);
  PORTC = B0001111;
}

void WriteMR(byte i){
  PORTC = nPORTC[i];
  SPI.transfer(instr.WRMR);
  SPI.transfer(B01000000);
  PORTC = B0001111;
}

void WriteSRAM(byte i){
  PORTC = nPORTC[i];
  SPI.transfer(instr.WRITE);
  SPI.transfer(pAddress[0]);
  SPI.transfer(pAddress[1]);
  SPI.transfer(pAddress[2]);
  for(int j=0; j<256; j++){
    SPI.transfer(j);
  }
  PORTC = B0001111;
}

void ReadSRAM(byte i){
  PORTC = nPORTC[i];
  SPI.transfer(instr.READ);
  SPI.transfer(pAddress[0]);
  SPI.transfer(pAddress[1]);
  SPI.transfer(pAddress[2]);
  for(int j=0; j<256; j++){
    nReadBack[i][j] = SPI.transfer(0);  
  }
  PORTC = B0001111;
}
