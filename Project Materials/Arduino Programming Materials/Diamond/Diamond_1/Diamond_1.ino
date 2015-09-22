// include the SPI library:
#include <SPI.h>

struct fin { byte outer,outer2,inner2,inner; };
byte check = 0;
const static fin dat[] = 
{
   //0b_0111_0000, 0b_0011_1000, 0b_0000_0000, 0b0000_0000
   //     7    0       3    8         0   0        0    0
 {0x00,0x00,0x00,0x00},//0
 {0x00,0x00,0x00,0x00},//1
 {0x00,0x00,0x00,0x00},//2
 {0x00,0x00,0x00,0x00},//3
 {0x00,0x00,0x00,0x00},//4
 {0x00,0x00,0x00,0x00},//5
 {0x00,0x00,0x00,0x00},//6
 {0x00,0x00,0x00,0x00},//7
 {0x00,0x00,0x00,0x00},//8
 {0x00,0x00,0x00,0x00},//9
 {0x00,0x00,0x00,0x00},//0
 {0x00,0x00,0x00,0x00},//11
 {0x00,0x00,0x00,0x00},//12
 {0x00,0x00,0x00,0x00},//13
 {0x00,0x00,0x00,0x00},//14
 {0x00,0x00,0x00,0x00},//15
 {0x00,0x00,0x00,0x00},//16
 {0x00,0x00,0x00,0x00},//17
 {0x00,0x00,0x00,0x00},//18
 {0x00,0x00,0x00,0x00},//19
 {0x00,0x00,0x00,0x00},//20
 {0x00,0x00,0x00,0x00},//21
 {0x00,0x00,0x00,0x00},//22
 {0x00,0x00,0x00,0x00},//23 
 {0x00,0x00,0x00,0x00},//24
 {0x00,0x00,0x00,0x00},//25
 {0x00,0x00,0x00,0x00},//26
 {0x00,0x00,0x00,0x00},//27
 {0x00,0x00,0x00,0x00},//28
 {0x00,0x00,0x00,0x00},//29
 {0x00,0x00,0x00,0x00},//30
 {0x00,0x00,0x00,0x00},//31  
 {0x00,0x00,0x00,0x00},//32
 {0x00,0x00,0x00,0x00},//33
 {0x00,0x00,0x00,0x00},//34
 {0x00,0x00,0x00,0x00},//35
 {0x00,0x00,0x00,0x00},//36
 {0x00,0x00,0x00,0x00},//37
 {0x00,0x00,0x00,0x00},//38
 {0x00,0x00,0x00,0x00},//39 
 {0x00,0x00,0x00,0x00},//40
 {0x00,0x00,0x00,0x00},//41
 {0x00,0x00,0x00,0x00},//42
 {0x00,0x00,0x00,0x00},//43
 {0x00,0x00,0x00,0x00},//44
 {0x00,0x00,0x00,0x00},//45
 {0x00,0x00,0x00,0x00},//46
 {0x00,0x00,0x00,0x00},//47 
 {0x00,0x00,0x00,0x00},//48
 {0x00,0x00,0x00,0x00},//49
 {0x00,0x00,0x00,0x00},//50
 {0x00,0x00,0x00,0x00},//51
 {0x00,0x00,0x00,0x00},//52
 {0x00,0x00,0x00,0x00},//53
 {0x00,0x00,0x00,0x00},//54
 {0x00,0x00,0x00,0x00},//55 
 {0x00,0x00,0x00,0x00},//56
 {0x00,0x00,0x00,0x00},//57
 {0x00,0x00,0x00,0x00},//58
 {0x00,0x00,0x00,0x00},//59
 {0x00,0x00,0x00,0x00},//60
 {0x00,0x00,0x00,0x00},//61
 {0x00,0x00,0x00,0x00},//62
 {0x00,0x00,0x00,0x00},//63 
 {0x00,0x00,0x00,0x00},//64
 {0x00,0x00,0x00,0x00},//65
 {0x00,0x00,0x00,0x00},//66
 {0x00,0x00,0x00,0x00},//67
 {0x00,0x00,0x00,0x00},//68
 {0x00,0x00,0x00,0x00},//69
 {0x00,0x00,0x00,0x00},//70
 {0x00,0x00,0x00,0x00},//71
 {0x00,0x00,0x00,0x00},//72
 {0x00,0x00,0x00,0x00},//73
 {0x00,0x00,0x00,0x00},//74
 {0x00,0x00,0x00,0x00},//75
 {0x00,0x00,0x00,0x00},//76
 {0x00,0x00,0x00,0x00},//77
 {0x00,0x00,0x00,0x00},//78
 {0x00,0x00,0x00,0x00},//79 
 {0x00,0x00,0x00,0x00},//80
 {0x00,0x00,0x00,0x00},//81
 {0x00,0x00,0x00,0x00},//82
 {0x00,0x00,0x00,0x00},//83
 {0x00,0x00,0x00,0x00},//84
 {0x00,0x00,0x00,0x00},//85
 {0x00,0x00,0x00,0x00},//86
 {0x00,0x00,0x00,0x00},//87 
 {0x00,0x00,0x00,0x00},//88
 {0x00,0x00,0x00,0x00},//89
 {0x00,0x00,0x00,0x00},//90
 {0x00,0x00,0x00,0x00},//91
 {0x00,0x00,0x00,0x00},//92
 {0x00,0x00,0x00,0x00},//93
 {0x00,0x00,0x00,0x00},//94
 {0x00,0x00,0x00,0x00},//95 
 {0x00,0x00,0x00,0x00},//96
 {0x00,0x00,0x00,0x00},//97
 {0x00,0x00,0x00,0x00},//98
 {0x00,0x00,0x00,0x00},//99
 {0x00,0x00,0x00,0x00},//100
 {0x00,0x00,0x00,0x00},//101
 {0x00,0x00,0x00,0x00},//102
 {0x00,0x00,0x00,0x00},//103 
 {0x00,0x00,0x00,0x00},//104
 {0x00,0x00,0x00,0x00},//105
 {0x00,0x00,0x00,0x00},//106
 {0x00,0x00,0x00,0x00},//107
 {0x00,0x00,0x00,0x00},//108
 {0x00,0x00,0x00,0x00},//109
 {0x00,0x00,0x00,0x00},//110
 {0x00,0x00,0x00,0x00},//111 
 {0x00,0x00,0x00,0x00},//112
 {0x00,0x00,0x00,0x00},//113
 {0x00,0x00,0x00,0x00},//114
 {0x00,0x00,0x00,0x00},//115
 {0x00,0x00,0x00,0x00},//116
 {0x00,0x00,0x00,0x00},//117
 {0x00,0x00,0x00,0x00},//118
 {0x00,0x00,0x00,0x00},//119 
 {0x00,0x00,0x00,0x00},//120
 {0x00,0x00,0x00,0x00},//121
 {0x00,0x00,0x00,0x00},//122
 {0x00,0x00,0x00,0x00},//123
 {0x00,0x00,0x00,0x00},//124
 {0x00,0x00,0x00,0x00},//125
 {0x00,0x00,0x00,0x00},//126
 {0x00,0x00,0x00,0x00},//127 
 {0x00,0x00,0x00,0x00},//128
 {0x00,0x00,0x00,0x00},//129
 {0x00,0x00,0x00,0x00},//130
 {0x00,0x00,0x00,0x00},//131
 {0x00,0x00,0x00,0x00},//132
 {0x00,0x00,0x00,0x00},//133
 {0x00,0x00,0x00,0x00},//134
 {0x00,0x00,0x00,0x00},//135 
 {0x00,0x00,0x00,0x00},//136
 {0x00,0x00,0x00,0x00},//137
 {0x00,0x00,0x00,0x00},//138
 {0x00,0x00,0x00,0x00},//139
 {0x00,0x00,0x00,0x00},//140
 {0x00,0x00,0x00,0x00},//141
 {0x00,0x00,0x00,0x00},//142
 {0x00,0x00,0x00,0x00},//143 
 {0x00,0x00,0x00,0x00},//144
 {0x00,0x00,0x00,0x00},//145
 {0x00,0x00,0x00,0x00},//146
 {0x00,0x00,0x00,0x00},//147
 {0x00,0x00,0x00,0x00},//148
 {0x00,0x00,0x00,0x00},//149
 {0x00,0x00,0x00,0x00},//150
 {0x00,0x00,0x00,0x00},//151 
 {0x00,0x00,0x00,0x00},//152
 {0x00,0x00,0x00,0x00},//153
 {0x00,0x00,0x00,0x00},//154
 {0x00,0x00,0x00,0x00},//155
 {0x00,0x00,0x00,0x00},//156
 {0x00,0x00,0x00,0x00},//157
 {0x00,0x00,0x00,0x00},//158
 {0x00,0x00,0x00,0x00},//159 
 {0x00,0x00,0x00,0x00},//160
 {0x00,0x00,0x00,0x00},//161
 {0x00,0x00,0x00,0x00},//162
 {0x00,0x00,0x00,0x00},//163
 {0x00,0x00,0x00,0x00},//164
 {0x00,0x00,0x00,0x00},//165
 {0x00,0x00,0x00,0x00},//166
 {0x00,0x00,0x00,0x00},//167 
 {0x00,0x00,0x00,0x00},//168
 {0x00,0x00,0x00,0x00},//169
 {0x00,0x00,0x00,0x00},//170
 {0x00,0x00,0x00,0x00},//171
 {0x00,0x00,0x00,0x00},//172
 {0x00,0x00,0x00,0x00},//173
 {0x00,0x00,0x00,0x00},//174
 {0x00,0x00,0x00,0x00},//175 
 {0x00,0x00,0x00,0x00},//176
 {0x00,0x00,0x00,0x00},//177
 {0x00,0x00,0x00,0x00},//178
 {0x00,0x00,0x00,0x00},//179
 {0x00,0x00,0x00,0x00},//180
 {0x00,0x00,0x00,0x00},//181
 {0x00,0x00,0x00,0x00},//182
 {0x00,0x00,0x00,0x00},//183 
 {0x00,0x00,0x00,0x00},//184
 {0x00,0x00,0x00,0x00},//185
 {0x00,0x00,0x00,0x00},//186
 {0x00,0x00,0x00,0x00},//187
 {0x00,0x00,0x00,0x00},//188
 {0x00,0x00,0x00,0x00},//189
 {0x00,0x00,0x00,0x00},//190
 {0x00,0x80,0x00,0x00},//191 
 {0x01,0xC0,0x00,0x00},//192
 {0x00,0x80,0x00,0x00},//193
 {0x00,0x00,0x00,0x00},//194
 {0x00,0x00,0x00,0x00},//195
 {0x00,0x00,0x00,0x00},//196
 {0x00,0x00,0x00,0x00},//197
 {0x00,0x00,0x00,0x00},//198
 {0x00,0x00,0x00,0x00},//199 
 {0x00,0x00,0x00,0x00},//200
 {0x00,0x00,0x00,0x00},//201
 {0x00,0x00,0x00,0x00},//202
 {0x00,0x00,0x00,0x00},//203
 {0x00,0x00,0x00,0x00},//204
 {0x00,0x00,0x00,0x00},//205
 {0x00,0x00,0x00,0x00},//206
 {0x00,0x00,0x00,0x00},//207 
 {0x00,0x00,0x00,0x00},//208
 {0x00,0x00,0x00,0x00},//209
 {0x00,0x00,0x00,0x00},//210
 {0x00,0x00,0x00,0x00},//211
 {0x00,0x00,0x00,0x00},//212
 {0x00,0x00,0x00,0x00},//213
 {0x00,0x00,0x00,0x00},//214
 {0x00,0x00,0x00,0x00},//215 
 {0x00,0x00,0x00,0x00},//216
 {0x00,0x00,0x00,0x00},//217
 {0x00,0x00,0x00,0x00},//218
 {0x00,0x00,0x00,0x00},//219
 {0x00,0x00,0x00,0x00},//220
 {0x00,0x00,0x00,0x00},//221
 {0x00,0x00,0x00,0x00},//222
 {0x00,0x00,0x00,0x00},//223 
 {0x00,0x00,0x00,0x00},//224
 {0x00,0x00,0x00,0x00},//225
 {0x00,0x00,0x00,0x00},//226
 {0x00,0x00,0x00,0x00},//227
 {0x00,0x00,0x00,0x00},//228
 {0x00,0x00,0x00,0x00},//229
 {0x00,0x00,0x00,0x00},//230
 {0x00,0x00,0x00,0x00},//231 
 {0x00,0x00,0x00,0x00},//232
 {0x00,0x00,0x00,0x00},//233
 {0x00,0x00,0x00,0x00},//234
 {0x00,0x00,0x00,0x00},//235
 {0x00,0x00,0x00,0x00},//236
 {0x00,0x00,0x00,0x00},//237
 {0x00,0x00,0x00,0x00},//238
 {0x00,0x00,0x00,0x00},//239 
 {0x00,0x00,0x00,0x00},//240
 {0x00,0x00,0x00,0x00},//241
 {0x00,0x00,0x00,0x00},//242
 {0x00,0x00,0x00,0x00},//243
 {0x00,0x00,0x00,0x00},//244
 {0x00,0x00,0x00,0x00},//245
 {0x00,0x00,0x00,0x00},//246
 {0x00,0x00,0x00,0x00},//247 
 {0x00,0x00,0x00,0x00},//248
 {0x00,0x00,0x00,0x00},//249
 {0x00,0x00,0x00,0x00},//250
 {0x00,0x00,0x00,0x00},//251
 {0x00,0x00,0x00,0x00},//252
 {0x00,0x00,0x00,0x00},//253
 {0x00,0x00,0x00,0x00},//254
 {0x00,0x00,0x00,0x00},//255 

};

volatile unsigned i = 0;

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
   check = digitalRead(2);   // This is mucho importante
   if(0 != check){
    return;  
   }
   for (i=96+128; i<256; i++) {
        LEDWrite();
   }
   for (i=0; i<96+128; i++) {
        LEDWrite();
   }
   PORTD = B10100000;
}

void LEDWrite() {
  PORTD = 0b10100000;
  SPI.transfer(dat[i].outer);
  SPI.transfer(dat[i].outer2);
  SPI.transfer(dat[i].inner2);
  SPI.transfer(dat[i].inner);
  PORTD = 0b11100000;
  //delayMicroseconds(59);
  PORTD = 0b00100000;
  delayMicroseconds(118); 
}
