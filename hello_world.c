#include <stdio.h>
#include <io.h>
#include <alt_types.h>
#include <system.h>

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08


#define ADR 0x68
#define READ BIT0
#define WRITE 0

#define BASE MASTER_TOP_0_BASE

#define byte unsigned char

// Low level drivers
void set_EN(unsigned char value)
{
	unsigned long temp;
	temp = IORD_32DIRECT(BASE,0) & ~BIT0;
	IOWR_32DIRECT(BASE,0,temp | value);
}
void set_READ(unsigned char value)
{
	unsigned long temp;
	temp = IORD_32DIRECT(BASE,0) & ~BIT1;
	IOWR_32DIRECT(BASE,0,temp | (value << 1));
}
void set_CNT(unsigned char value)
{
	unsigned long temp;
	temp = IORD_32DIRECT(BASE,0) & ~(BIT2|BIT3);
	IOWR_32DIRECT(BASE,0,temp | (value << 2));
}
void set_RATE(unsigned char value)
{
	unsigned long temp;
	temp = IORD_32DIRECT(BASE,0) & ~0x0000FF00;
	IOWR_32DIRECT(BASE,0,temp | (value << 8));
}
void set_ADR(unsigned char value)
{
	unsigned long temp;
	temp = IORD_32DIRECT(BASE,0) & ~0x007F0000;
	IOWR_32DIRECT(BASE,0,temp | (value << 16));
}
void set_WR_BYTE0(unsigned char value)
{
	unsigned long temp;
	temp = IORD_32DIRECT(BASE,4) & ~0x000000FF;
	IOWR_32DIRECT(BASE,4,temp | value);
}
void set_WR_BYTE1(unsigned char value)
{
	unsigned long temp;
	temp = IORD_32DIRECT(BASE,4) & ~0x0000FF00;
	IOWR_32DIRECT(BASE,4,temp | (value << 8));
}
unsigned char get_EN(void)
{
	return IORD_32DIRECT(BASE,0) & BIT0;
}

unsigned char get_READ(void)
{
	return (IORD_32DIRECT(BASE,0) & BIT1)>> 1;
}

unsigned char get_CNT(void)
{
	return (IORD_32DIRECT(BASE,0) & (BIT2|BIT3)) >> 2;
}

unsigned char get_RATE(void)
{
	return (IORD_32DIRECT(BASE,0) & 0x0000FF00) >> 8;
}

unsigned char get_ADR(void)
{
	return (IORD_32DIRECT(BASE,0) & 0x007F0000) >> 16;
}

unsigned char get_WR_BYTE0(void)
{
	return (IORD_32DIRECT(BASE,4) & 0x000000FF);
}


unsigned char get_WR_BYTE1(void)
{
	return (IORD_32DIRECT(BASE,4) & 0x0000FF00) >> 8;
}

unsigned char get_NO_ACK(void)
{
	return (IORD_32DIRECT(BASE,8) & BIT1) >> 1;
}
unsigned char get_DONE(void)
{
	return IORD_32DIRECT(BASE,8) & BIT0;
}
unsigned char get_RD_BYTE(void)
{
	return IORD_32DIRECT(BASE,12) & 0x000000FF;
}

byte bcd_to_dec(byte val)
{
  return( (val/16*10) + (val%16) );
}

byte dec_to_bcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
void write_I2C_single(unsigned char reg_adr, unsigned char value)
{
	//printf("skrive");
	set_EN(0);
	set_READ(0);
	set_CNT(2);
	set_WR_BYTE0(reg_adr);
	set_WR_BYTE1(value);
	set_READ(0);
	set_EN(1);
	//printf("skrive2");
	while (get_DONE() == 0){
		//printf("done=0");
	}
	if (get_NO_ACK() == '1')
		{printf("fail");
		}
	set_EN(0);
}

unsigned char read_I2C_single(unsigned char reg_adr)
{
	//printf("lese1");
	unsigned char les;
	set_EN(0);
	set_READ(0);
    set_CNT(1);
    set_WR_BYTE0(reg_adr);
    set_EN(1);
   // printf("lese2");
	while (get_DONE() == 0){
		//printf("lese3");
	}
	if (get_NO_ACK() == '1')
			{printf("fail");
			}
	set_EN(0);
	set_CNT(1);
	set_READ(1);
	set_EN(1);
	//printf("lese4");
	while (get_DONE() == 0);
	if (get_NO_ACK() == '1'){
		printf("fail");}
		les = get_RD_BYTE();
	set_EN(0);

	return les;
}

//get temperature
float get_rtc_temp(void)
{

}

void set_rtc_time(byte second, byte minute, byte hour, byte week_day, byte day, byte month, byte year)
{

	write_I2C_single(0x0,dec_to_bcd(second));
	write_I2C_single(0x1,dec_to_bcd(minute));
	write_I2C_single(0x2,dec_to_bcd(hour));
	write_I2C_single(0x3,dec_to_bcd(week_day));
	write_I2C_single(0x4,dec_to_bcd(day));
	write_I2C_single(0x5,dec_to_bcd(month));
	write_I2C_single(0x6,dec_to_bcd(year));
}

void get_rtc_time(byte *second, byte *minute, byte *hour, byte *week_day, byte *day, byte *month, byte *year)
{
	*second   =   bcd_to_dec(read_I2C_single(0));
	*minute   =   bcd_to_dec(read_I2C_single(1));
	*hour 	  =   bcd_to_dec(read_I2C_single(2));
	*week_day =   bcd_to_dec(read_I2C_single(3));
	*day 	  =   bcd_to_dec(read_I2C_single(4));
	*month    =   bcd_to_dec(read_I2C_single(5));
	*year     =   bcd_to_dec(read_I2C_single(6));

}

int main(){
//deklarere variabler
	byte second = 0, minute, hour, week_day, day, month, year ;
	byte prev_sec = 0;
	printf("start:\n");
	alt_32 temp;
	set_ADR((0x68));
	set_RATE(0x88);
	//write_I2C_single(0x2,dec_to_bcd(13));
	set_rtc_time(00, 02, 13, 4, 15, 11, 18);
while(1){
	get_rtc_time(&prev_sec, &minute, &hour, &week_day, &day, &month, &year);
	if(prev_sec != second){
		second = prev_sec;
		printf("\nTime: %02u:%02u:%02u ", hour, minute, second);
		printf("Date: ");
		printf(" %02u/%02u/%02u", day, month, year);
	}
		//temp = read_I2C_single(,0x6,0x5,0x4,0x3,0x2,0x1,0x0);
		//printf("avlest: %i.", bcd_to_dec(temp));



		for(int i =0;i<500000;i++);
}
	return 0;
}
