#include "task.h"
#include "ucos_ii.h"
#include "textMode.h"
#include "pm511p.h"
#include "closeLoop.h"
#include <libpc.h>
#include "comm_pc.h"
#include "commrtos.h"
#include "sinArray.c"

int scanRangeX = 30000;
int scanRangeY = 30000;
int scanOffsetX = 0;
int scanOffsetY = 0;
int scanAngle = 0;
int scanPixels = 256;

unsigned short zBuf[512][512];
short errorBuf[512][512];

/**
*@brief  设置 扫描点数
*@param  XDots(int) X 方向扫描点数
*@param  YDots(int) Y 方向扫描点数
*@return  void
*/
void setScanPixel(int XDots, int YDots)
{
    scanPixels = (XDots > YDots) ? XDots : YDots;
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
static unsigned short topoSig(int x, int y)
{
    double z = sinArray[(x / 200) % 90] * sinArray[(y / 300) % 90] ;
    z = 10000 + x / 10.0 + y / 20.0;
    return (unsigned short) z;
}

unsigned short traceBuf[512];
unsigned short retraceBuf[512];
short errorTraceBuf[512];
short errorRetraceBuf[512];
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
        PC_DispStr(0, 23, "Line Scan Start      ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        //开始正式行扫描
    
        /* update scan parameter at the start of each scan line */
        rangeX = scanRangeX;
        rangeY = scanRangeY;
        offsetX = scanOffsetX;
        offsetY = scanOffsetY;
        angle = scanAngle;
        computeSinCos(&sinAngle, &cosAngle, angle);

        /* compute the position of the start point */
        tempX = -rangeX / 2;
        tempY = 0;
        
        posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
        posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;

        /* move tip to the start point in a gentle manner */
        PC_DispStr(0, 23, "move tip to start point", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(i =0 ;i < 100; i++)
        {
            write_DA(DA_X, 32768 + i * posX / 100);
            write_DA(DA_Y, 32768 + i * posY / 100);
            OSTimeDly(LINESCAN_DELAY);
        };
        
        /* trace line scan */
        PC_DispStr(0, 23, "trace line scan ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(i = -LINESCAN_DOTS + 1, ii = 0; i < LINESCAN_DOTS; i += 2, ii++) 
        {/*if LINESCAN_DOTS = 512, then i is from -511 to 511 */
            /* generate the scan range */
            tempX = i * rangeX / (LINESCAN_DOTS - 1) / 2;
            tempY = 0 * rangeY / (LINESCAN_DOTS - 1) / 2;
            
            /* hysterisis conpensation */
            
            /* rotate and offset the pos */
            posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
            posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
            
            //OSTimeDly(delay);
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            //z = topoSig(posX, posY);
            
            traceBuf[ii] = pidZ;
            errorTraceBuf[ii] = pidError;
            
            z = traceBuf[ii];
            CommPutChar(COMM1, z & 0xff, 0);
            CommPutChar(COMM1, (z >> 8)& 0xff, 0);
            
            z = errorTraceBuf[ii];
            CommPutChar(COMM1, z & 0xff, 0);
            CommPutChar(COMM1, (z >> 8)& 0xff, 0);
            write_DA(DA_X, 32768 + posX);
            write_DA(DA_Y, 32768 + posY);
        };
            
        
        if(currentTask == TASK_STOP)
        {
            goto lineScanExit; 
        };
        
        /* retrace line scan */
        PC_DispStr(0, 23, "retrace line scan", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(i = LINESCAN_DOTS - 1; i > -LINESCAN_DOTS; i -= 2)  
        {
            /* generate the scan range */
            tempX = i * rangeX / (LINESCAN_DOTS - 1) / 2;
            tempY = 0 * rangeY / (LINESCAN_DOTS - 1) / 2;
            
            /* hysterisis conpensation */
            
            /* rotate and offset the pos */
            posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
            posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
            
            //OSTimeDly(delay);
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            //z = pidZ;
            //z = topoSig(posX, posY);
            retraceBuf[ii] = pidZ;
            errorRetraceBuf[ii] = pidError;
//            CommPutChar(COMM1, z & 0xff, 0);
//            CommPutChar(COMM1, (z >> 8)& 0xff, 0);
            write_DA(DA_X, 32768 + posX);
            write_DA(DA_Y, 32768 + posY);
        
        };
        
lineScanExit:
        /* move tip to the centre point in a gentle manner */
        for(i =100 ;i >= 0; i--)
        {
                write_DA(DA_X, 32768 + i * posX / 100);
                write_DA(DA_Y, 32768 + i * posY / 100);
                OSTimeDly(LINESCAN_DELAY);
        };
        currentTask = TASK_STOP;
        PC_DispStr(0, 23, "Line Scan Stop       ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    };
};

void normalScanTask(void* para)
{
    INT8U err = 0;
    int i, ii, j, jj;
    int rangeX, rangeY, offsetX, offsetY, angle;
    int tempX, tempY, cosAngle, sinAngle;
    int posX, posY;
    short int z;
    while(1)
    {
        OSMboxPend(MboxNormalScan, 0, &err); /* use mail box as a semaphore */
        PC_DispStr(0, 23, "normal Scan Start       ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        //开始正式行扫描
    
        /* update scan parameter at the start of each scan line */
        rangeX = scanRangeX;
        rangeY = scanRangeY;
        offsetX = scanOffsetX;
        offsetY = scanOffsetY;
        angle = scanAngle;
        computeSinCos(&sinAngle, &cosAngle, angle);

        /* compute the position of the start point */
        tempX = -rangeX / 2;
        tempY = -rangeY / 2;
        
        posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
        posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;

        /* move tip to the start point in a gentle manner */
        for(i =0 ;i < 100; i++)
        {
            write_DA(DA_X, 32768 + i * posX / 100);
            write_DA(DA_Y, 32768 + i * posY / 100);
            OSTimeDly(LINESCAN_DELAY);
        };
        for( j = -LINESCAN_DOTS + 1, jj = 0; j < LINESCAN_DOTS; j += 2, jj++ )
        { /* jj record the line number */
        
            /* update scan parameter at the start of each scan line */
            rangeX = scanRangeX;
            rangeY = scanRangeY;
            offsetX = scanOffsetX;
            offsetY = scanOffsetY;
            angle = scanAngle;
            computeSinCos(&sinAngle, &cosAngle, angle);
            
            for(i = -LINESCAN_DOTS + 1, ii = 0; i < LINESCAN_DOTS; i += 2, ii++) /* trace line scan */
            {/*if LINESCAN_DOTS = 512, then i is from -511 to 511 */
                /* generate the scan range */
                tempX = i * rangeX / (LINESCAN_DOTS - 1) / 2;
                tempY = j * rangeY / (LINESCAN_DOTS - 1) / 2;
                
                /* hysterisis conpensation */
                
                /* rotate and offset the pos */
                posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
                posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
                
                //OSTimeDly(delay);
                OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
                z = pidZ & 0xffff;
                zBuf[jj][ii] = z;
                errorBuf[jj][ii] = pidError;
                
                CommPutChar(COMM1, z & 0xff, 0);
                CommPutChar(COMM1, (z >> 8)& 0xff, 0);
                write_DA(DA_X, 32768 + posX);
                write_DA(DA_Y, 32768 + posY);
            };
        
            PC_DispStr(0, 23, "trace line finish     ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            if(currentTask == TASK_STOP)
            {
                goto normalScanExit; 
            };
            for(i = LINESCAN_DOTS - 1, ii = 511; i > -LINESCAN_DOTS; i -= 2, ii--)  /* retrace line scan */
            {
                /* generate the scan range */
                tempX = i * rangeX / (LINESCAN_DOTS - 1) / 2;
                tempY = j * rangeY / (LINESCAN_DOTS - 1) / 2;
                
                /* hysterisis conpensation */
                
                /* rotate and offset the pos */
                posX = offsetX + (tempX * cosAngle + tempY * sinAngle) / 10000;
                posY = offsetY + (-tempX * sinAngle + tempY * cosAngle) / 10000;
                
                //OSTimeDly(delay);
                OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
                /* 反扫时不记录信息点 */
                write_DA(DA_X, 32768 + posX);
                write_DA(DA_Y, 32768 + posY);
            
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




