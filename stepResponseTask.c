#include "task.h"
#include "ucos_ii.h"
#include "textMode.h"
#include "closeLoop.h"
#include <libpc.h>
#include "comm_pc.h"
#include "commrtos.h"

#define PID_BUF_SIZE 500
static short pidZBuf1[PID_BUF_SIZE];
static short pidZBuf2[PID_BUF_SIZE];
static short pidEBuf1[PID_BUF_SIZE];
static short pidEBuf2[PID_BUF_SIZE];
static short deltaSetPoint = 0;

void setDeltaSetPoint(short det)
{
    if(det > 10000) det = 10000;
    if(det < -10000) det = -10000;
    deltaSetPoint = det;
}

void stepResponseTask(void* para)
{
    INT8U err = 0;
    int i;
    int oldPIDSetPoint, oldLoop;
    
    while(1)
    {
        OSMboxPend(MboxPIDStepResponse, 0, &err); /* use mail box as a semaphore */
        PC_DispStr(0, 23, "Step Response Start       ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        //PC_DispStr(0, 23, " ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        CommPutShort(COMM1, pidSetPoint);
        CommPutShort(COMM1, deltaSetPoint);
        CommPutShort(COMM1, pidKp);
        CommPutShort(COMM1, pidKi);
        CommPutShort(COMM1, pidKd);
        oldLoop = pidLoop;
        oldPIDSetPoint = pidSetPoint;
        setScanDelay(1);
        for(i = 0; i < 10; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidZBuf1[i] = pidZ;
            pidEBuf1[i] = pidError;
        }
        //开始得到正向的阶跃响应，相当于样品突然远离探针时探针向样品方向接近的过程。
        pidSetPoint += deltaSetPoint;
        for(i = 10; i < PID_BUF_SIZE; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidZBuf1[i] = pidZ;
            pidEBuf1[i] = pidError;
        }
        //得到反向的阶跃响应，相当于样品变高顶起探针，探针向上远离样品的过程。
        for(i = 0; i < 10; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidZBuf2[i] = pidZ;
            pidEBuf2[i] = pidError;
        }
        pidSetPoint = oldPIDSetPoint;
        for(i = 10; i < PID_BUF_SIZE; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidZBuf2[i] = pidZ;
            pidEBuf2[i] = pidError;
        }
        setScanDelay(oldLoop);
        // 回传正向的阶跃响应数据，供上位微机分析
        CommWrite(COMM1, (unsigned char*)pidZBuf1, PID_BUF_SIZE * sizeof(short));
        CommWrite(COMM1, (unsigned char*)pidEBuf1, PID_BUF_SIZE * sizeof(short));
        // 回传反向的阶跃响应数据，供上位微机分析
        CommWrite(COMM1, (unsigned char*)pidZBuf2, PID_BUF_SIZE * sizeof(short));
        CommWrite(COMM1, (unsigned char*)pidEBuf2, PID_BUF_SIZE * sizeof(short));
        PC_DispStr(0, 23, "Step Response End       ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    }
}

