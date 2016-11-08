#include "task.h"
#include "ucos_ii.h"
#include "textMode.h"
#include "pm511p.h"
#include "closeLoop.h"
#include <libpc.h>
#include "comm_pc.h"
#include "commrtos.h"
#include "sinArray.c"


/* 内部函数声明区 */
static void updateHCCurve( void );
//static float InvSqrt(float x);
//static float sqrt(float a);
//static float corr2 ( float y, float a );
static float corr3 ( float y, float a, float b );


/* 全局变量声明区 */
int scanRangeX = 30000;
int scanRangeY = 30000;
int scanOffsetX = 0;
int scanOffsetY = 0;
int scanAngle = 0;
int scanPixels = 256;


/**
*@brief  设置 扫描点数
*@param  XDots(int) X 方向扫描点数
*@param  YDots(int) Y 方向扫描点数
*@return  void
*/
void setScanPixel(int XDots, int YDots)
{
    int temp, n; 
    temp = (XDots > YDots) ? XDots : YDots;
    
    if(temp < 0) temp = 64;
    if(temp > 512) temp = 512;
    
    n = temp / 64;
    scanPixels = n * 64;
}

/**
*@brief  设置 扫描范围等参数
*@param  rangeX(int) X 方向扫描范围
*@param  rangeY(int) Y 方向扫描范围
*@param  offsetX(int) X 方向偏移
*@param  offsetY(int) Y 方向偏移
*@param  angle(int) 旋转角度
*@return  void
*/
void setScanRange(int rangeX, int rangeY, int offsetX, int offsetY, int angle)
{
    scanRangeX = rangeX;
    scanRangeY = rangeY;
    scanOffsetX = offsetX;
    scanOffsetY = offsetY;
    while ( angle >= 360 )
    {   
        angle -= 360;
    }
    while ( angle < 0 )
    {   
        angle += 360;
    }
    /* angle is normalized to [0, 359] */
    scanAngle = angle;
}
/**
*@brief  模拟产生形貌数据
*@param  X 位置
*@param  Y 位置
*@return  unsigned short 形貌数据
*/
/*static unsigned short topoSig(int x, int y)*/
/*{*/
/*    double z = sinArray[(x / 200) % 90] * sinArray[(y / 300) % 90] ;*/
/*    z = 10000 + x / 10.0 + y / 20.0;*/
/*    return (unsigned short) z;*/
/*}*/


/**
    以下部分是压电扫描器迟滞非线性矫正所需数据区
*/
int hcMethod = 0;
short hctc_xA = 0;//正扫x方向校正系数 A, -100 +100
short hctc_xB = 0;//正扫x方向校正系数 B, -100 +100
short hcrc_xA = 0;//反扫x方向校正系数 A, -100 +100
short hcrc_xB = 0;//反扫x方向校正系数 B, -100 +100
short hctc_yA = 0;//正扫y方向校正系数 A, -100 +100
short hctc_yB = 0;//正扫y方向校正系数 B, -100 +100
short hcrc_yA = 0;//反扫y方向校正系数 A, -100 +100
short hcrc_yB = 0;//反扫y方向校正系数 B, -100 +100
float hctc_x[SCAN_DOTS]; //hysteresis correction trace curve, x dircection
float hcrc_x[SCAN_DOTS]; //hysteresis correction retrace curve, x dircection
float hctc_y[SCAN_DOTS]; //hysteresis correction trace curve, y dircection
float hcrc_y[SCAN_DOTS]; //hysteresis correction retrace curve, y dircection

/*
static float InvSqrt(float x)
{  
    float xhalf = 0.5f * x;  
    long i = *(long*) &x;  
    i = 0x5f3759df- (i >> 1);  
    x = *(float*) &i;  
    x = x * (1.5f - xhalf * x * x);  
    return x;  
}   
*/
/*static float sqrt(float a)*/
/*{*/
/*    float x1 = a / 2;*/
/*    float x2 = (x1 + a/ x1) / 2;*/
/*   */
/*    while((x2 - x1) >= 1e-5 || (x1 - x2) >= 1e-5)*/
/*    {*/
/*          x1 = x2;*/
/*          x2 = (x1 + a/ x1) / 2;*/
/*    }*/
/*    return x1;*/
/*}*/
/**
*@brief 用二次函数的反函数做非线性校正
*@param y 归一化的位移
*@param a 矫正系数, 不能为 0 否则有除 0 错误
*@return 归一化的电压
*/
//static float corr2 ( float y, float a ) //只对正扫描校正
//{
    /*  假设扫描器的迟滞特性可以表示为 二次多项式形式
        方程为 y = a x^2 - a + x
        根据迟滞系数 a 就可以计算出所需的输入电压
    */
//    float sq;
//    if ( a == 0 ) return y;
//    sq = sqrt ( 1 + 4 * a * ( a + y ) );
//    if ( a > 0 ) //对应的是对正扫曲线的矫正，取大的根
//        return 0.5 * ( -1 + sq ) / a;
//    else //对应的是对反扫曲线的矫正，取小的根
//        return 0.5 * ( -1 - sq ) / a;
//}


//用三次函数的反函数做非线性校正
static float corr3 ( float y, float a, float b )
{
    float x, f, ff;
    float i;
    if ( y > 1 ) y = 1;
    if ( y < -1 ) y = -1;

    //非线性方程为 y = a x^3 + b x^2 - a x - b + x
    x = y; //set the initial value of iteration
    /* 利用牛顿迭代法求方程的根 
        这里精度要求不高，固定迭代次数即可
    */
    for ( i = 0; i < 10; i++ ) 
    {
        f = a * x * x * x + b * x * x - a * x - b + x - y;
        ff = 3 * a * x * x + 2 * b * x - a + 1;
        x = x - f / ff; // x = x - f / f'
    }
    return x;
}
/**
*@brief 设置扫描时快轴的迟滞矫正系数
*@param a1 正扫时的校正系数 A * 100, a1 取值 [-100, 100]
*@param b1 正扫时的校正系数 B * 100, b1 取值 [-50, 50]*@param a2 反扫时的校正系数 A * 100, a2 取值 [-100, 100]
*@param b2 反扫时的校正系数 B * 100, b2 取值 [-50, 50]
*@return void
*/
void setHysteresisParaX(short a1, short b1, short a2, short b2)
{
    if(a1 < -100) a1 = -100;
    if(a1 > 100) a1 = 100;
    if(b1 < -50) b1 = -50;
    if(b1 > 50) b1 = 50;
    if(a2 < -100) a2 = -100;
    if(a2 > 100) a2 = 100;
    if(b2 < -50) b2 = -50;
    if(b2 > 50) b2 = 50;  
    
    hctc_xA = a1;
    hctc_xB = b1;
    hcrc_xA = a2;
    hcrc_xB = b2;
    updateHCCurve();
}


/**
*@brief 设置扫描时慢轴的迟滞矫正系数
*@param a1 正扫时的校正系数 A * 100, a1 取值 [-100, 100]
*@param b1 正扫时的校正系数 B * 100, b1 取值 [-50, 50]*@param a2 反扫时的校正系数 A * 100, a2 取值 [-100, 100]
*@param b2 反扫时的校正系数 B * 100, b2 取值 [-50, 50]
*/
void setHysteresisParaY(short a1, short b1, short a2, short b2)
{

    if(a1 < -100) a1 = -100;
    if(a1 > 100) a1 = 100;
    if(b1 < -50) b1 = -50;
    if(b1 > 50) b1 = 50;
    if(a2 < -100) a2 = -100;
    if(a2 > 100) a2 = 100;
    if(b2 < -50) b2 = -50;
    if(b2 > 50) b2 = 50;  
    
    hctc_yA = a1;
    hctc_yB = b1;
    hcrc_yA = a2;
    hcrc_yB = b2;
    
    updateHCCurve();
}


/*
void updateHCCurve ( void ) //更新非线性校正曲线
{
    int i;
    double y;
    if ( hcMethod == 0 ) // 不做非线性校正
    {
        for ( i = 0; i < 512; i++ )
        {
            y = 2.0 * i / 511.0 - 1;
            hctc_x[i] = hcrc_x[i] = hctc_y[i] = y;
        }
    }
    else if ( hcMethod == 1 ) //利用 2 次函数的反函数校正
    {
        for ( i = 0; i < 512; i++ )
        {
            y = 2.0 * i / 511.0 - 1;
            hctc_x[i] = corr2 ( y, hctc_xB );
            hcrc_x[i] = corr2 ( y, hcrc_xB );
            hctc_y[i] = corr2 ( y, hctc_yB );

        }
    }
    else if ( hcMethod == 2 ) // 利用三次函数的反函数校正
    {
        for ( i = 0; i < 512; i++ )
        {
            y = 2.0 * i / 511.0 - 1;
            hctc_x[i] = corr3 ( y, hctc_xA, hctc_xB );
            hcrc_x[i] = corr3 ( y, hcrc_xA, hcrc_xB );
            hctc_y[i] = corr3 ( y, hctc_yA, hctc_yB );
        }
    }
}
*/

/**
*@brief  更新非线性校正曲线，采用三次函数反函数作为矫正函数
*@param  void
*@return  void
*/
void updateHCCurve ( void )
{
    int i;
    float tc_xA, tc_xB, rc_xA, rc_xB, tc_yA, tc_yB;
    double y;
    tc_xA = hctc_xA / 100.0;
    tc_xB = hctc_xB / 100.0;
    rc_xA = hcrc_xA / 100.0;
    rc_xB = hcrc_xB / 100.0;
    tc_yA = hctc_yA / 100.0;
    tc_yB = hctc_yB / 100.0;
    for ( i = 0; i < scanPixels; i++ )
    {
        y = 2.0 * i / scanPixels - 1;
        hctc_x[i] = corr3 ( y, tc_xA, tc_xB );
        hcrc_x[i] = corr3 ( y, rc_xA, rc_xB );
        hctc_y[i] = corr3 ( y, tc_yA, tc_yB );
    }
}

static float dots(short trace[], short retrace[], int n)
{
    int i, i1, i2;
    float sum = 0;
    for(i = 0; i < n; i++)
    {
        i1 = n * (hctc_x[i] + 1) / 2;
        i2 = n * (hcrc_x[i] + 1) / 2;
        sum += (trace[i1] * retrace[i2]);
    }
    return sum;
}

/**
 以下是线扫描任务的扫描数据存放区
*/
short traceBuf[SCAN_DOTS];
short retraceBuf[SCAN_DOTS];
short errorTraceBuf[SCAN_DOTS];
short errorRetraceBuf[SCAN_DOTS];
short currentLine = 0;

/**
自动计算迟滞系数
*/
void hyCompute(void)
{

    short a, b, aMax, bMax;
    short oldtc_xA, oldtc_xB, oldrc_xA, oldrc_xB;
    float corr, max;
    oldtc_xA = hctc_xA;
    oldtc_xB = hctc_xB;
    oldrc_xA = hcrc_xA;
    oldrc_xB = hcrc_xB;
    max = 0;
    for(a = -20; a < 20; a ++ )
    {
        for(b = 0; b < 30; b++)
        {
            hctc_xA = a;
            hctc_xB = b;
            hcrc_xA = a;
            hcrc_xB = -b;
            updateHCCurve();
            corr = dots(traceBuf, retraceBuf, scanPixels);
            if(corr > max) 
            {
                max = corr;
                aMax = a;
                bMax = b;
            }
        }
    }
    hctc_xA = aMax;
    hctc_xB = bMax;
    hcrc_xA = aMax;
    hcrc_xB = -bMax;
    hctc_yA = aMax;
    hctc_yB = bMax;
    updateHCCurve();
}


/**
*@brief  线扫描线程
*@param  para(void*) 通道选择
*@return  void*
*/
void lineScanTask(void* para)
{
    INT8U err = 0;
    short i, ii;
    int rangeX, rangeY, offsetX, offsetY, angle;
    int tempX, tempY, cosAngle, sinAngle;
    int posX, posY;
    short int z;
    while(1)
    {
        OSMboxPend(MboxLineScan, 0, &err); /* use mail box as a semaphore */
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Line Scan Start          ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        //开始正式行扫描
        //PC_DispStr(0, 1, "1111", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        /* update scan parameter at the start of each scan line */
        rangeX = scanRangeX;
        rangeY = scanRangeY;
        offsetX = scanOffsetX;
        offsetY = scanOffsetY;
        angle = scanAngle;
        
        computeSinCos(&sinAngle, &cosAngle, angle);
        updateHCCurve();

        CommPutShort(COMM1, scanPixels);
        CommPutShort(COMM1, (short)rangeX);
        CommPutShort(COMM1, offsetX);
        CommPutShort(COMM1, offsetY);
        CommPutShort(COMM1, angle);
        CommPutShort(COMM1, pidKp);
        CommPutShort(COMM1, pidKi);
        CommPutShort(COMM1, pidKd);
        CommPutShort(COMM1, pidLoop);
        CommPutShort(COMM1, pidSetPoint);
        /* compute the position of the start point */
        tempX = -rangeX / 2;
        tempY = currentLine * rangeY / (scanPixels - 1) / 2;
        
        posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
        posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;

        /* move tip to the start point in a gentle manner */
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Move tip to start point  ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(i =0 ;i < 100; i++)
        {
            write_DA(DA_X, 32768 + i * posX / 99);
            write_DA(DA_Y, 32768 + i * posY / 99);
            OSTimeDly(LINESCAN_DELAY);
        };
        //PC_DispStr(0, 1, "2222", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        /* trace line scan */
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Trace line scan          ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(ii = 0; ii < scanPixels; ii++) 
        {/*if scanPixels = 512, then i is from -511 to 511 */
            /* generate the scan range without hysterisis correction */
            //tempX = i * rangeX / (scanPixels - 1) / 2;
            //tempY = 0 * rangeY / (scanPixels - 1) / 2;
            //PC_DispStr(0, 1, "3333", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            /* generate the scan range with hysterisis correction */
            tempX = hctc_x[ii] * rangeX / 2;
            tempY = hctc_y[currentLine] * rangeY / 2;
            /* hysterisis conpensation */
            //PC_DispStr(0, 1, "4444", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            /* rotate and offset the pos */
            posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
            posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
            
            //OSTimeDly(delay);
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            //z = topoSig(posX, posY);
            //OSTimeDly(1);
            traceBuf[ii] = pidZ;
            errorTraceBuf[ii] = pidError;
            
            z = traceBuf[ii];
            CommPutShort(COMM1, z);
            
            //z = errorTraceBuf[ii];
            //CommPutChar(COMM1, z & 0xff, 0);
            //CommPutChar(COMM1, (z >> 8)& 0xff, 0);
            write_DA(DA_X, 32768 + posX);
            write_DA(DA_Y, 32768 + posY);
        };
            
        
        if(currentTask == TASK_STOP)
        {
            goto lineScanExit; 
        };
        
        /* retrace line scan */
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Retrace line scan        ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(ii = scanPixels - 1; ii >= 0; ii--)  
        {
            /* generate the scan range */
            //tempX = i * rangeX / (scanPixels - 1) / 2;
            //tempY = 0 * rangeY / (scanPixels - 1) / 2;
            
            tempX = hcrc_x[ii] * rangeX / 2;
            tempY = hctc_y[currentLine] * rangeY / 2;
            //tempY = currentLine * rangeY / (scanPixels - 1) / 2;
            /* hysterisis conpensation */
            
            /* rotate and offset the pos */
            posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
            posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
            
            //OSTimeDly(1);
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            //z = pidZ;
            //z = topoSig(posX, posY);
            retraceBuf[ii] = pidZ;
            errorRetraceBuf[ii] = pidError;
            
            z = retraceBuf[ii];
            CommPutShort(COMM1, z);

            write_DA(DA_X, 32768 + posX);
            write_DA(DA_Y, 32768 + posY);
        
        };
        CommWrite(COMM1, (unsigned char*)errorTraceBuf, scanPixels * sizeof(short));
        CommWrite(COMM1, (unsigned char*)errorRetraceBuf, scanPixels * sizeof(short));
lineScanExit:
        /* move tip to the centre point in a gentle manner */
        for(i =100 ;i >= 0; i--)
        {
                write_DA(DA_X, 32768 + i * posX / 100);
                write_DA(DA_Y, 32768 + i * posY / 100);
                OSTimeDly(LINESCAN_DELAY);
        };
        currentTask = TASK_STOP;
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Line scan stop           ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    };
};


short zBuf[SCAN_DOTS][SCAN_DOTS];
short errorBuf[SCAN_DOTS][SCAN_DOTS];
short (*pBuf)[SCAN_DOTS];
void setScanChannel(short channel)
{
    if(channel == 0)
        pBuf = zBuf;
    else
        pBuf = errorBuf;
}
void normalScanTask(void* para)
{
    INT8U err = 0;
    int i, ii, jj;
    int rangeX, rangeY, offsetX, offsetY, angle;
    int tempX, tempY, cosAngle, sinAngle;
    int posX, posY;
    int z;
    pBuf = zBuf;
    
    while(1)
    {
        OSMboxPend(MboxNormalScan, 0, &err); /* use mail box as a semaphore */
        PC_DispStr(0, 23, "Normal Scan Start       ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        //开始正式行扫描
    
        /* update scan parameter at the start of each scan line */
        rangeX = scanRangeX;
        rangeY = scanRangeY;
        offsetX = scanOffsetX;
        offsetY = scanOffsetY;
        angle = scanAngle;
        computeSinCos(&sinAngle, &cosAngle, angle);
        updateHCCurve();

        CommPutShort(COMM1, scanPixels);
        CommPutShort(COMM1, (short)rangeX);
        CommPutShort(COMM1, offsetX);
        CommPutShort(COMM1, offsetY);
        CommPutShort(COMM1, angle);
        CommPutShort(COMM1, pidKp);
        CommPutShort(COMM1, pidKi);
        CommPutShort(COMM1, pidKd);
        CommPutShort(COMM1, pidLoop);
        CommPutShort(COMM1, pidSetPoint);
        /* compute the position of the start point */
        tempX = -rangeX / 2;
        tempY = -rangeY / 2;
        
        posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
        posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;

        /* move tip to the start point in a gentle manner */
        for(i =0 ;i < 100; i++)
        {
            write_DA(DA_X, 32768 + i * posX / 99);
            write_DA(DA_Y, 32768 + i * posY / 99);
            OSTimeDly(LINESCAN_DELAY);
        };
        for( jj = 0; jj < 2; jj++ )// 空扫2行使扫描器稳定
        {
            for(ii = 0; ii < scanPixels; ii++) /* trace line scan */
            {/*if scanPixels = 512, then i is from -511 to 511 */
                /* hysterisis conpensation */
                tempX = hctc_x[ii] * rangeX / 2; tempY = hctc_y[jj] * rangeY / 2;
                /* rotate and offset the pos */
                posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
                posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
                
                write_DA(DA_X, 32768 + posX); write_DA(DA_Y, 32768 + posY);
                //OSTimeDly(delay);
                OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            };
            for(ii = scanPixels - 1; ii >= 0; ii--)  /* retrace line scan */
            {
                /* hysterisis conpensation */
                tempX = hcrc_x[ii] * rangeX / 2; tempY = hctc_y[jj] * rangeY / 2;
                /* rotate and offset the pos */
                posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
                posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;

                write_DA(DA_X, 32768 + posX); write_DA(DA_Y, 32768 + posY);
                //OSTimeDly(delay);
                OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            };
        }
        OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
        for( jj = 0; jj < scanPixels; jj++ )
        { /* jj record the line number */
        
            /* update scan parameter at the start of each scan line */
            //rangeX = scanRangeX;
            //rangeY = scanRangeY;
            //offsetX = scanOffsetX;
            //offsetY = scanOffsetY;
            //angle = scanAngle;
            //computeSinCos(&sinAngle, &cosAngle, angle);
            for(ii = 0; ii < scanPixels; ii++) /* trace line scan */
            {/*if scanPixels = 512, then i is from -511 to 511 */
                /* generate the scan range */
                //tempX = i * rangeX / (scanPixels - 1) / 2;
                //tempY = j * rangeY / (scanPixels - 1) / 2;
                
                /* hysterisis conpensation */
                tempX = hctc_x[ii] * rangeX / 2;
                tempY = hctc_y[jj] * rangeY / 2;
                /* rotate and offset the pos */
                posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
                posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
                
                write_DA(DA_X, 32768 + posX);
                write_DA(DA_Y, 32768 + posY);
                //OSTimeDly(delay);
                OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */

                zBuf[jj][ii] = pidZ;/* 高度数据 */
                errorBuf[jj][ii] = pidError;
                
                z = pBuf[jj][ii];/* pBuf 指向要传输的数据，形貌或误差信号 */
                CommPutShort(COMM1, z);

            };
        
            PC_DispStr(0, 23, "trace line finish     ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            if(currentTask == TASK_STOP)
            {
                goto normalScanExit; 
            };
            for(ii = scanPixels - 1; ii >= 0; ii--)  /* retrace line scan */
            {
                /* generate the scan range */
                //tempX = i * rangeX / (scanPixels - 1) / 2;
                //tempY = j * rangeY / (scanPixels - 1) / 2;
                
                /* hysterisis conpensation */
                tempX = hcrc_x[ii] * rangeX / 2;
                tempY = hctc_y[jj] * rangeY / 2;
                /* rotate and offset the pos */
                posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
                posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;

                write_DA(DA_X, 32768 + posX);
                write_DA(DA_Y, 32768 + posY);
                //OSTimeDly(delay);
                OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
                /* 反扫时不记录信息点 */
            };
            PC_DispStr(0, 23, "retrace line finish    ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            if(currentTask == TASK_STOP)
            {
                goto normalScanExit; 
            };        };
normalScanExit:
        /* move tip to the centre point in a gentle manner */
        for(i =100 ;i >= 0; i--)
        {
                write_DA(DA_X, 32768 + i * posX / 100);
                write_DA(DA_Y, 32768 + i * posY / 100);
                OSTimeDly(LINESCAN_DELAY);
        };
        currentTask = TASK_STOP;
        PC_DispStr(0, 23, "Full Scan Stop       ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    };
};

/**
自动选区函数
选区边长为原始边长的一半，以1/4边长为步进
也就是将待测区域划分为4*4的网格
选区面积为4个格子，步进为1个格子
在总共9个选区中挑选方差最大的选区，返回结果为选区编号，0-8
*/
short autoZoom(void)
{
    short i, j, ii, jj;
    int avg[4][4];
    int avg2, iMax, jMax;
    int iStart, jStart, div;
    float sum, sdMax;
    // 分别计算 16 个小子块的平均值
    for(i = 0; i < 4; i++)
    {
        iStart = i * scanPixels / 4;
        for(j = 0; j < 4; j++)
        {
            // 求取当前小子块的平均值
            jStart = j * scanPixels / 4;
            sum = 0;
            for(ii = 0; ii < scanPixels / 4; ii ++)
            {
                for(jj = 0; jj < scanPixels / 4; jj++)
                {
                    sum += zBuf[iStart + ii][jStart + jj];
                    
                }
            }
            avg[i][j] = sum / scanPixels * 16 / scanPixels;
            //printf("%d, %d = %d\n", i, j, avg[i][j]);
        }
    }
    // 计算各个子区域的均方差
    sdMax = 0;
    iMax = 0; //如果找不到方差最大的区域，就默认选左上角区域
    jMax = 0; //如果找不到方差最大的区域，就默认选左上角区域
    for(i = 0; i < 3; i++)
    {
        //当前子区域的起始点
        iStart = i * scanPixels / 4;
        for(j = 0; j < 3; j++)
        {
            //当前子区域的起始点
            jStart = j * scanPixels / 4;
            // 计算当前子区域的平均值（就是当前子区域所包含的小子块的平均值的平均值）
            avg2 = (avg[i][j] + avg[i][j+1] + avg[i+1][j] + avg[i+1][j+1]) / 4;
            
            sum = 0;
            for(ii = 0; ii < scanPixels / 2; ii ++)
            {
                for(jj = 0; jj < scanPixels / 2; jj++)
                {
                    div = zBuf[iStart + ii][jStart + jj] - avg2;
                    sum += (div * div);
                }
            }
            if(sum > sdMax)
            {
                sdMax = sum;
                iMax = i;
                jMax = j;
            }
        }
    }
    
    scanRangeX /= 2;
    scanRangeY = scanRangeX;
    scanOffsetX += ( (iMax - 1) * scanRangeX / 2 );
    scanOffsetY += ( (jMax - 1) * scanRangeY / 2 );
    return(iMax * 3 + jMax);
}

