#include "task.h"
#include "pm511p.h"
#include "ucos_ii.h"
#include "stdint.h"
#include "comm_pc.h"
#include "commrtos.h"
#include "textMode.h"
#include "closeLoop.h"
#include "afmCommand.h"

static unsigned int freqScanStartFreq;
static unsigned int freqScanEndFreq;
static int hasAmpChannel, hasPhaseChannel;
static unsigned short freqScanDelay = 10;

/**
*@brief  设置扫频频率范围
*@param  startFreq(unsigned int) 起始频率 * 100
*@param  endFreq(unsigned int) 截止频率 * 100
*@return  void
*/
void setFreqRange(unsigned int startFreq, unsigned int endFreq)
{
    freqScanStartFreq = startFreq;
    freqScanEndFreq = endFreq;
}
/**
*@brief  设置扫频时的通道
*@param  unsigned short channelBit
*@return  void
*/
void setFreqChannel(unsigned short channelBit)
{
    /* 读入通道信息 */
    hasAmpChannel = (channelBit & 0x01);
    hasPhaseChannel = (channelBit & 0x02);
}
/**
*@brief  设置扫频时的速度
*@param  unsigned short delay, 单位是 us
*@return  void
*/
void setFreqDelay(unsigned short delay)
{
    freqScanDelay = delay;
}

static short freqScanAmpBuf[512];
static short freqScanPhaseBuf[512];
/**
*@brief  扫频任务
*@param  void *pdata 暂时没有用到
*@return  void
*/
void freqScanTask(void *pdata)
{
    int i;
    double freq;
    short int z;
    unsigned char err;
    while(1)
    {
        OSMboxPend(MboxFreqScan, 0, &err); /* use mail box as a semaphore */
        DDSWakeup();
        AD_stopTimer();
        PC_DispStr(0, 23, "freq sweep start ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        /* STARTING FREQENCY SCANNING */
        for(i = 0; i < 512; i++)
        {
            freq = freqScanStartFreq + (freqScanEndFreq - freqScanStartFreq) / 512 * i;
            dds_Out(freq);
            OSTimeDly(freqScanDelay);
            z = read_AD(AD_AMPL_CHANNEL);
            freqScanAmpBuf[i] = z;
            CommPutChar(COMM1, z & 0xff, 0);
            CommPutChar(COMM1, (z >> 8)& 0xff, 0);
            z = read_AD(AD_PHASE_CHANNEL);
            freqScanPhaseBuf[i] = z;
            CommPutChar(COMM1, z & 0xff, 0);
            CommPutChar(COMM1, (z >> 8)& 0xff, 0);
        };
        AD_startTimer();
        PC_DispStr(0, 23, "freq sweep stop ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    };
    return;
}


