#include <libpc.h>

#define setBitToOne(Data, nBit)  ( Data |= (1 << nBit) )
#define setBitToZero(Data, nBit) ( Data &= ~(1 << nBit) )

const int clockFreq = 8388608;
const short FREQ0_LSB = 768;
const short FREQ0_MSB = 770;
const short DDS_DA_ADDR = 792;
const short CONTROL_ADDR = 794;
const short DA_ADDR = 784;
const short GPIO_ADDR = 798;
short controlWord = 0;
/**
*@brief  设置 DDS 输出频率
*@param  freq 实际频率 * 100，这是为了最小频率分辨达到0.01 Hz
*@return  void
*/
void dds_Out(int freq)
{
    // int phase = 4294967296 / clockFreq * freq / 100; // 4294967296 超出了int范围
    int phase = 512 * freq / 100;
    short lsb, msb;
    lsb = phase & 0xffff;
    msb = (phase >> 16) & 0xffff;
    outw(lsb, FREQ0_LSB);
    outw(msb, FREQ0_MSB);
}
/**
*@brief  设置 DDS 输出幅度
*@param  amplitude 输出幅度
*@return  void
*/
void setDDSAmplitude(unsigned char amplitude)
{
    outw(amplitude, DDS_DA_ADDR);
}
/**
*@brief  设置 DDS 为休眠状态
*@param  void
*@return  void
*/
void DDSSleep(void)
{
    setBitToZero(controlWord, 1);
    outw(controlWord, CONTROL_ADDR);

}
/**
*@brief  设置 DDS 为工作状态
*@param  void
*@return  void
*/
void DDSWakeup(void)
{
    setBitToOne(controlWord, 1);
    outw(controlWord, CONTROL_ADDR);
}


/**
*@brief  设置 DA 为休眠状态
*@param  void
*@return  void
*/
void DASleep(void)
{
    setBitToZero(controlWord, 2);
    outw(controlWord, CONTROL_ADDR);

}
/**
*@brief  设置 DA 为工作状态
*@param  void
*@return  void
*/
void DAWakeup(void)
{
    setBitToOne(controlWord, 2);
    outw(controlWord, CONTROL_ADDR);
}
/**
*@brief  设置 DA 输出
*@param  channel 通道 0-3
*@param  value 无符号数
*@return  void
*/
//void write_DA( unsigned char channel, unsigned short value )
//{
//    OS_ENTER_CRITICAL();
//    outb( channel, PM511P_DA_CH );    /*设置 D/A 通道*/
//    outw( value >> 4, PM511P_DA_OUT );    /*启动 D/A 输出*/
//    OS_EXIT_CRITICAL();
//}

void ledOn(void)
{
    setBitToOne(controlWord, 0);
    outw(controlWord, CONTROL_ADDR);
}

void ledOff(void)
{
    setBitToZero(controlWord, 0);
    outw(controlWord, CONTROL_ADDR);
}
void GpioOut(char a)
{
    outw(a, GPIO_ADDR);
}


