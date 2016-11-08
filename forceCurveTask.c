#include "task.h"
#include "ucos_ii.h"
#include "textMode.h"
#include "closeLoop.h"
#include "pm511p.h"
#include "stdint.h"
#include "comm_pc.h"
#include "commrtos.h"
/*
力曲线
上位机向下位机发送命令:

cmd = CMD_FORCE_CURVE    : 0x2A
para1 = z 下限
para2 = z 上限
para3 = 点数
para4 = delay
para5 = channel

下位机向上位机传回数据：（每点传回一次数据）

cmd = CMD_FORCE_CURVE    : 0x2A
para1 = 0
para2 = 当前点 （回程时为负数）1,2,3,4... , -1, -2, -3, ...
para3 = Z
para4 = 通道号
para5 = 数据
*/
#define FORCE_CURVE_STOP    0x00
#define FORCE_CURVE_WORK    0x01
#define FORCE_CURVE_DELAY   0x01

static short forceStartZ;
static short forceEndZ;
static short forceTotalSteps;
static short forceDelay;

//static short forceZBuf1[400], forceZBuf2[400];
//static short forceErrBuf1[400], forceErrBuf2[400];

void setForceCurvePara(short startZ, short endZ, short steps, short delay)
{
    forceStartZ = startZ;
    forceEndZ = endZ;
    forceTotalSteps = steps;
    if(steps < 2) forceTotalSteps = 2;
    if(steps > 1000) forceTotalSteps = 1000;
    forceDelay = delay > 1 ? delay : 2;
}


void forceCurveTask(void *pdata)
{
    INT8U err = 0;
    short delay, totalSteps, i;
    short zStart, zEnd;
    short oldLoop, oldZ;
    unsigned short outZ;
    int tempZ;
    while(1)
    {
        OSMboxPend(MboxForceCurve, 0, &err); /* use mail box as a semaphore */
        PC_DispStr(0, 23, "ForceCurve Start       ", DISP_FGND_YELLOW + DISP_BGND_BLUE);

        zStart = forceStartZ;
        zEnd = forceEndZ;
        totalSteps = forceTotalSteps;
        delay = forceDelay;
        
        CommPutShort(COMM1, totalSteps);
        CommPutShort(COMM1, zStart);
        CommPutShort(COMM1, zEnd);
        CommPutShort(COMM1, delay);
        
        // save the original PID parameters.
        oldZ = pidZ;
        oldLoop = pidLoop;
        disablePID();
        /* move tip to the start point in a gentle manner */
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Move tip to start point  ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(i = 0; i < 20; i++) // 20 步移动到初始位置
        {
            tempZ = oldZ + i * (zStart - oldZ) / 19;
            if(tempZ > 32767)   tempZ = 32767;
            if(tempZ < -32768)  tempZ = -32768;
            outZ = tempZ + 32768;
            write_DA(DA_Z, outZ);
            OSTimeDly(FORCE_CURVE_DELAY);
        }
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Move Down...             ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        setScanDelay(delay);
        
        OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
        for(i = 0; i < totalSteps; i++) // 从起始位置向下移动
        {
            tempZ = zStart + i * (zEnd - zStart) / (totalSteps - 1);
            outZ = tempZ + 32768;
            write_DA(DA_Z, outZ);
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            //record the data.
            //forceZBuf1[i] = tempZ;
            //forceErrBuf1[i] = pidADValue;
            CommPutShort(COMM1, pidADValue);
        }
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Move Up...               ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(i = 0; i < totalSteps; i++) // 从最下面向上移动
        {
            tempZ = zEnd + i * (zStart - zEnd) / (totalSteps - 1);
            outZ = tempZ + 32768;
            write_DA(DA_Z, outZ);
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */

            //forceZBuf2[i] = tempZ;
            //forceErrBuf2[i] = pidADValue;
            CommPutShort(COMM1, pidADValue);
        }
        /* move tip to the start point in a gentle manner */
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Move tip to origin point ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        for(i = 0; i < 20; i++) // 20 步移动到初始位置
        {
            tempZ = zStart + i * (oldZ - zStart) / 19;
            if(tempZ > 32767)   tempZ = 32767;
            if(tempZ < -32768)  tempZ = -32768;
            outZ = tempZ + 32768;
            write_DA(DA_Z, outZ);
            OSTimeDly(FORCE_CURVE_DELAY);
        }
        setScanDelay(oldLoop);
        enablePID();
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "ForceCurve Finish        ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    }
}

