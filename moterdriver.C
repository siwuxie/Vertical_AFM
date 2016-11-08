#include<sys/io.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

inline static void udelay(int usecs)
{
	int i; for(i = 0; i < usecs; i++) outb(0, 0x80);
}

#define PM511P_ADDR	0x100
#define AD_CHANNEL	0x00

#define DA_X		0x00
#define DA_Y		0x01
#define DA_Z		0x02

#define AD_LASER_A	0x02
#define AD_LASER_B	0x01
#define AD_LASER_C	0x04
#define AD_LASER_D	0x03
#define AD_LASER_SUM	0x00
#define AD_OSAP		0x07
#define AD_ERROR	0x06
// not true
#define AD_PHASE	0x0B
#define AD_STM		0x0C

//#define BASEADDR 0x100

#define IO_PA (PM511P_ADDR + 0x08)
#define IO_PB (PM511P_ADDR + 0x09)
#define IO_PC (PM511P_ADDR + 0x0A)
#define IO_CONTROL_WORD (PM511P_ADDR + 0x0B) 

#define	ADDR_MOTOR	(PM511P_ADDR + 0x09)


void init_Hardware()
{
	iopl(3);
	outb(0x80, IO_CONTROL_WORD); // 所有 IO 口都设为输出模式
	udelay(200);
//	outb_p(0, ADDR_MOTOR);
}

static unsigned char buf_IO_PB = 0;
void motor_forward_one_step()
{
	int i=0;
	
	buf_IO_PB &= 0xFE;								//	direction
	buf_IO_PB |= 0x2;									//	no sleep
	buf_IO_PB |= 0x4;									//	square wave
	outb_p(buf_IO_PB, ADDR_MOTOR);
//	udelay(700);
	for(i=0; i<20000; i++);
	
	buf_IO_PB &= 0xFB;
	outb_p(buf_IO_PB, ADDR_MOTOR);
//	udelay(700);
	for(i=0; i<20000; i++);
};

void motor_backward_one_step()
{
	int i=0;
	buf_IO_PB |= 0x1;									//	deriction
	buf_IO_PB |= 0x2;									//	no sleep
	buf_IO_PB |= 0x4;									//	square wave
	outb_p(buf_IO_PB, ADDR_MOTOR);
//	udelay(700);
	for(i=0; i<20000; i++);
	
	buf_IO_PB &= 0xFB;
	outb_p(buf_IO_PB, ADDR_MOTOR);
//	udelay(700);
	for(i=0; i<20000; i++);
};

int main (int argc, char* argv[])
{
	init_Hardware();
	
	for(int i=0; i<1000; i++)
	{
		motor_forward_one_step();
	}
	
	for(int i=0; i<1000; i++)
	{
		motor_backward_one_step();
	}
	
	buf_IO_PB &= 0xFD;									//	sleep
	outb_p(buf_IO_PB, ADDR_MOTOR);
	
	
//	while(1)
//	{
//		motor_forward_one_step();
//		outb_p(0, ADDR_MOTOR);
//		udelay(10);
//		outb_p(0x4, ADDR_MOTOR);
//		udelay(10);
//	}
	
	return 0;
}
