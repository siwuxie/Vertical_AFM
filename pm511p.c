/* 
 *
 * PM 511P AD/DA �ɼ����� 12 λ AD/DA �ɼ�����
 * AD ��һͨ������߲ɼ��ٶ�ԼΪ 80 KHz��
 * DA ���������ٶ�ԼΪ 200 KHz,��DA����ʱ�� 5 us����
 *
 */
#include "pm511p.h"
#include <ucos_ii.h>
#include <libpc.h>

#define IDLE_PORT   0x80
void AD_setTimer(void) /* ѡ��ʱ���� A/D ��ʽ */
{
    outb_p(BIT1 + BIT4, PM511P_AD_CONTROL); /* ѡ��ʱ���� A/D ��ʽ for pm511p */
//    outb_p(BIT1 + BIT3, PM511P_AD_CONTROL); /* ѡ��ʱ���� A/D ��ʽ  for pm511 */
    outw_p(4, PM511P_TIMER); /* �趨ת��Ƶ��Ϊ40KHz     (ʱ��Ƶ��Ϊ200KHz)*/
    inw( PM511P_AD_VALUE );
}
void AD_startTimer(void) /* ѡ��ʱ���� A/D ��ʽ */
{
    outb_p(AD_TIMING_START | AD_FINISH_INTERR, PM511P_AD_CONTROL); 
}
void AD_stopTimer(void) /* �رն�ʱ���� A/D ��ʽ */
{
    outb_p(0, PM511P_AD_CONTROL); /* �л����˳������� A/D ��ʽ */
}
static void udelay(int usecs)
{
    int i; for(i = 0; i < usecs; i++) inb(IDLE_PORT);
}

void write_DA( unsigned char channel, unsigned short value )
{
    OS_ENTER_CRITICAL();
    outb( channel, PM511P_DA_CH );    /*���� D/A ͨ��*/
    outw( value >> 4, PM511P_DA_OUT );    /*���� D/A ���*/
    OS_EXIT_CRITICAL();
}

short read_AD(unsigned char channel)
{
    short value;
    outb( channel , PM511P_ADDR );    /*���� A/D ����ͨ��*/
    udelay(CHANNEL_DELAY);//waiting for setting the channel of A/D
    inb( PM511P_AD_START );    /*���� A/D ת��*/
    
    while( inb( PM511P_ADDR + 1 ) &  0x01);    /*�ж� A/D ת���Ƿ����*/

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

