#include <hidef.h>        //Basic includes
#include "derivative.h"     

#include <stdio.h>		  //Needed for memset & calloc

void delay(unsigned ms) //tenth of a ms
{   
	// Calibrated for about 1 ms (996 microseconds)
	#define ratio 95
	long long unsigned count = ratio * ms;
	while(count-->0);
	#undef ratio
}
void send(void)
{;
	PT1AD0_PT1AD00 = 0;
	delay(2);
	PT1AD0_PT1AD00 = 1;
} 
void prt_char(char c_ptr)
{
	PT1AD0	= 0x03;
	PTT		= (unsigned char) c_ptr;
	send();
}
void clear_disp(void)
{
	PT1AD0	= 0x01;
	PTT		= 0x01;
	PT1AD0_PT1AD00 = 0;
	delay(20);
	PT1AD0_PT1AD00 = 1;
}	
void return_home(void)
{
	PT1AD0	= 0x01;
	PTT		= 0x02;
	send();
}
void entry_mode(int inc_dec, int shft)
{
	PT1AD0 	 	= 0x01;
	PTT 		= 0x04;
	PTT_PTT1 	= (inc_dec > 0)	?1:0;
	PTT_PTT0 	= (shft==1) 	?1:0;
	send();
}
void disp_on(int disp, int cursor, int carat)
{
	PT1AD0		= 0x01;
	PTT 		= 0x08;
	PTT_PTT2 	= (disp==1)		?1:0;
	PTT_PTT1 	= (cursor==1)	?1:0;
	PTT_PTT0 	= (carat==1)	?1:0;
	send();
}
void fct_set(int dat_l, int num_l, int font)
{
	PT1AD0		= 0x01;
	PTT 		= 0x20;
	PTT_PTT4 	= (dat_l==8)	?1:0;
	
	PTT_PTT3 	= (num_l==2)	?1:0;
	PTT_PTT2 	= (font==510)	?1:0;
	send();
}
void disp_shft(int dir)
{   
	PT1AD0		= 0x01;
	PTT 		= 0x10;
	PTT_PTT2 	= (dir > 0) 	?1:0;
	send();	
}



void curs_shft(int dir)
{
	PT1AD0		= 0x01;
	PTT 		= 0x10;
	PTT_PTT2 	= (dir > 0 )	?1:0;
	send();	
}  
void new_line(void)
{
	PT1AD0		= 0x01;
	PTT			= 0xC0;
	send();	
}
void prt_str(char * str)
{   
	for(; (*str) != 0 ; prt_char(*str), str++);
}
void set_baud_rate(int baud)
{
	SCI0CR2_RE = 1; //Enable Receiver	
	SCI0BDL = 39;	//(unsigned) (6000000/(baud * 16));
}
int chk_rx()
{
	return (SCI0SR1_RDRF == 0 ? -1 :1);
}
char fetch_char()
{
	return SCI0DRL; //Data register holding character	
}
void main(void) {
  /* put your own code here */
  	EnableInterrupts;
    
  	DDR1AD0 =	DDRT	= 0xFF;	
	PT1AD0	= 	PTT		= 0x00;
	
	set_baud_rate(9600);
		 
	send();	
	fct_set(8,2,58);
	disp_on(1,1,1);
	entry_mode(1,0);	
	clear_disp();

	char  val,buf[16] ;
	
#define go 5000
	while(1)
	{
		memset(buf,0,16);
		for(int v=0,i =0; i<16 ; i++)
		{   v=0;
			while(	chk_rx() >= 0	? 0:
					i==0 			? 1:
					v++ < go		? 1:
					0);
			if (v >= go){	break;	}
			buf[i]=fetch_char();				
		}
		return_home();
		clear_disp();
		prt_str(buf);	
	}
	 
  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
