/* 
 *
 * PM 511P AD/DA 采集卡是 12 位 AD/DA 采集卡，
 * AD 单一通道的最高采集速度约为 80 KHz。
 * DA 的最高输出速度约为 200 KHz,（DA建立时间 5 us）。
 *
 */
#include "pm511p.h"
#include <ucos_ii.h>
#include <libpc.h>

#define IDLE_PORT   0x80
void AD_setTimer(void) /* 选择定时启动 A/D 方式 */
{
    outb_p(BIT1 + BIT4, PM511P_AD_CONTROL); /* 选择定时启动 A/D 方式 for pm511p */
//    outb_p(BIT1 + BIT3, PM511P_AD_CONTROL); /* 选择定时启动 A/D 方式  for pm511 */
    outw_p(4, PM511P_TIMER); /* 设定转换频率为40KHz     (时钟频率为200KHz)*/
    inw( PM511P_AD_VALUE );
}
void AD_startTimer(void) /* 选择定时启动 A/D 方式 */
{
    outb_p(AD_TIMING_START | AD_FINISH_INTERR, PM511P_AD_CONTROL); 
}
void AD_stopTimer(void) /* 关闭定时启动 A/D 方式 */
{
    outb_p(0, PM511P_AD_CONTROL); /* 切换到了程序启动 A/D 方式 */
}
static void udelay(int usecs)
{
    int i; for(i = 0; i < usecs; i++) inb(IDLE_PORT);
}

void write_DA( unsigned char channel, unsigned short value )
{
    OS_ENTER_CRITICAL();
    outb( channel, PM511P_DA_CH );    /*设置 D/A 通道*/
    outw( value >> 4, PM511P_DA_OUT );    /*启动 D/A 输出*/
    OS_EXIT_CRITICAL();
}

short read_AD(unsigned char channel)
{
    short value;
    outb( channel , PM511P_ADDR );    /*设置 A/D 采样通道*/
    udelay(CHANNEL_DELAY);//waiting for setting the channel of A/D
    inb( PM511P_AD_START );    /*启动 A/D 转换*/
    
    while( inb( PM511P_ADDR + 1 ) &  0x01);    /*判断 A/D 转换是否完成*/

    value = inw( PM511P_ADDR + 2 ) & 0xfff;
    return (value - 2048) * 16;
}



void initPm511p(void)
{
    outb(0x80, IO_CONTROL_WORD);  /* Init all I/O port to Output mode */
    
    write_DA(DA_X, 32768); 
    write_DA(DA_Y, 32768); 
    write_DA(DA_Z, 32768); 
}

