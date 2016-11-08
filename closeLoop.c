#include <libpc.h>
#include <ucos_ii.h>
#include "closeLoop.h"
#include "pm511p.h"


OS_EVENT * MboxCloseLoop;

unsigned int pidEnabled = 1;
unsigned int pidCounter = 0;
int pidIntegral = 0;
int pidDiff;
int pidDiffOld = 0;

short pidADValue = 0;
int pidError = 0;
int pidErrorOld = 0;

short pidZ = 0;
short pidSetPoint = 1000;
short pidLoop = 30;
short pidKp = 40;
short pidKi = 40;
short pidKd = 40;
unsigned char pidChannel;

/*short (*PidISRHandler)();*/

/*void setFeedBackMode(short mode)*/
/*{*/
/*    switch(mode)*/
/*    {*/
/*        case 0:*/
/*            PidISRHandler = PidISRHandler0;*/
/*            break;*/
/*        case 1:*/
/*            PidISRHandler = PidISRHandler1;*/
/*            break;*/
/*        case 2:*/
/*            PidISRHandler = PidISRHandler2;*/
/*            break;*/
/*        case 3:*/
/*            PidISRHandler = PidISRHandler3;*/
/*            break;*/
/*    }*/
/*}*/

/**
*@brief  只有积分环节的增量式算法
*/
void PidISRHandler0(void)
{
    short ad_value;
    signed int tempZ = 0; /* tempZ is a signed value*/
    int detZ = 0;
                
    ad_value = inw( PM511P_AD_VALUE ) & 0xfff;
    ad_value -= 2048;
    pidADValue = ad_value * 16;
    
    pidError = (pidADValue - pidSetPoint);// 得到误差值
 
    //tempZ = ((pidError + pidLastError) * pidKi + pidKp * (pidError - pidLastError)) / 1000.0;
    detZ = pidError * pidKi / PID_DIV;
    tempZ = pidZ + detZ;
    
    if(tempZ > 65535)    tempZ = 65535;
    if(tempZ < 0)        tempZ = 0;
    pidZ = tempZ;
//    write_DA( DA_Z, pidZ);    outb( DA_Z, PM511P_DA_CH );    /* set D/A channel */
    outw( pidZ >> 4, PM511P_DA_OUT );    /* start D/A output */

//    pidLastError = pidError;    
    
    pidCounter ++;
    if(pidCounter > pidLoop)
    {
        OSMboxPost(MboxCloseLoop, (void*) 1);
        pidCounter = 0;
    }
}
/**
*@brief  PID Controller with Only Proportional Term.
*/
void PidISRHandler1(void)
{
    signed int ad_value;
    signed int tempZ = 0; /* tempZ is a signed value*/
    unsigned short outZ;
    
    ad_value = inw( PM511P_AD_VALUE ) & 0xfff;
    ad_value -= 2048;/* change the a/d value to signed short */
    pidADValue = ad_value * 16; /* zoom to the range -32768 32767 */
    
    pidError = (pidADValue - pidSetPoint);/* compute the error */
 
    tempZ = pidError * pidKp / PID_DIV;
    
    if(tempZ > 32767)   tempZ = 32767;
    if(tempZ < -32768)  tempZ = -32768;
    
    pidZ = tempZ;
    outZ = tempZ + 32768;
        outb( DA_Z, PM511P_DA_CH );    /* set D/A channel */
    outw( outZ >> 4, PM511P_DA_OUT );    /* start D/A output */

    pidCounter ++;
    if(pidCounter > pidLoop)
    {
        OSMboxPost(MboxCloseLoop, (void*) 1);
        pidCounter = 0;
    }
}


/**
*@brief  PID Controller with P and I term, anti integrator windup
*/
void PidISRHandler5(void)
{
    signed int adValue;
    signed int tempZ = 0; /* tempZ is a signed value*/
    unsigned short outZ;
    
    adValue = inw( PM511P_AD_VALUE ) & 0xfff;
    adValue -= 2048;/* change the a/d value to signed short */
    pidADValue = adValue * 16; /* zoom to the range -32768 32767 */
    
    pidError = (pidADValue - pidSetPoint);/* compute the error */
    if(pidEnabled)//控制 PID 的开关，当 PID 关闭时，只计算误差信号，不更新 Z 的输出
    {
        pidIntegral += pidError * pidKi / PID_DIV;
        if(pidIntegral > 32767)   pidIntegral = 32767;  /* anti integrator windup */
        if(pidIntegral < -32768)  pidIntegral = -32768; /* anti integrator windup */
        
        tempZ = pidError * pidKp / PID_DIV + pidIntegral;
        
        if(tempZ > 32767)   tempZ = 32767;
        if(tempZ < -32768)  tempZ = -32768;
        
        pidZ = tempZ;
        outZ = tempZ + 32768;        outb( DA_Z, PM511P_DA_CH );    /* set D/A channel */
        outw( outZ >> 4, PM511P_DA_OUT );    /* start D/A output */
    }

    pidCounter ++;
    if(pidCounter > pidLoop)
    {
        OSMboxPost(MboxCloseLoop, (void*) 1);
        pidCounter = 0;
    }
}
void PidISRHandler(void)
{
    signed int adValue;
    signed int tempZ = 0; /* tempZ is a signed value*/
    unsigned short outZ;
    
    adValue = inw( PM511P_AD_VALUE ) & 0xfff;
    adValue -= 2048;/* change the a/d value to signed short */
    pidADValue = adValue * 16; /* zoom to the range -32768 32767 */
    
    pidError = (pidADValue - pidSetPoint);/* compute the error */
    if(pidEnabled)//控制 PID 的开关，当 PID 关闭时，只计算误差信号，不更新 Z 的输出
    {
        pidIntegral += pidError;
        if(pidIntegral > 1835008)   pidIntegral = 1835008;  /* anti integrator windup */
        if(pidIntegral < -1835008)  pidIntegral = -1835008; /* anti integrator windup */

        pidDiff = (pidError - pidErrorOld) * pidKd / PID_DIV / 2 + pidDiffOld / 2;
        tempZ = (pidError * pidKp + pidIntegral * pidKi ) / PID_DIV + pidDiff;
        pidDiffOld = pidDiff;
        if(tempZ > 32767)   tempZ = 32767;
        if(tempZ < -32768)  tempZ = -32768;
        
        pidZ = tempZ;
        outZ = tempZ + 32768;        outb( DA_Z, PM511P_DA_CH );    /* set D/A channel */
        outw( outZ >> 4, PM511P_DA_OUT );    /* start D/A output */
    }
    pidErrorOld = pidError;
    pidCounter ++;
    if(pidCounter > pidLoop)
    {
        OSMboxPost(MboxCloseLoop, (void*) 1);
        pidCounter = 0;
    }
}
void setPIDPara(short Kp, short Ki, short Kd)
{
    pidKp = Kp;
    pidKi = Ki;
    pidKd = Kd;
}
void setPIDParaOther(short loop)
{
    if(loop < 0) pidLoop = 1;
    else if (loop > 1000) pidLoop = 1000;
    else pidLoop = loop;
}
/**
*@brief  设置 扫描延时
*@param  delay(int) 延时（100 us）
*@return  void
*/
void setScanDelay(int delay)
{
    if(delay < 0) pidLoop = 1;
    else if (delay > 1000) pidLoop = 1000;
    else pidLoop = delay;
}

void setPIDChannel(unsigned char channel)
{
    outb_p(channel, PM511P_AD_CH); /* 选择 A/D 通道 */
    pidChannel = channel;
}
unsigned char getPIDChannel(void)
{
    return pidChannel;
}
void setPIDSetPoint(short setPoint)
{
    pidSetPoint = setPoint;
}
void enablePID(void)
{
    pidEnabled = 1;
}
void disablePID(void)
{
    pidEnabled = 0;
}


