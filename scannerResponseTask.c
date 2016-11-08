#include "task.h"
#include "ucos_ii.h"
#include "textMode.h"
#include "closeLoop.h"
#include <libpc.h>
#include "comm_pc.h"
#include "commrtos.h"
#include "pm511p.h"

#define SCANNER_BUF_SIZE 400
static short pidEBuf1[SCANNER_BUF_SIZE];
static short pidEBuf2[SCANNER_BUF_SIZE];
static short deltaZ = 0;

void setDeltaZ(short det)
{
    if(det > 10000) det = 10000;
    if(det < -10000) det = -10000;
    deltaZ = det;
}

void scannerResponseTask(void* para)
{
    INT8U err = 0;
    int i, tempZ;
    short oldLoop, oldZ;
    unsigned short outZ;
    while(1)
    {
        OSMboxPend(MboxScannerResponse, 0, &err); /* use mail box as a semaphore */
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Scanner Response Start   ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        CommPutShort(COMM1, deltaZ);
        oldLoop = pidLoop;
        oldZ = pidZ;
        
        tempZ = pidZ + deltaZ;
        if(tempZ > 32767)   tempZ = 32767;
        if(tempZ < -32768)  tempZ = -32768;
        outZ = tempZ + 32768;
        
        disablePID();
        setScanDelay(1);
        
        for(i = 0; i < 10; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidEBuf1[i] = pidError;
        }
        //开始得到正向的阶跃响应，相当于扫描器缩短时误差信号的变化。
        write_DA(DA_Z, outZ);
        for(i = 10; i < SCANNER_BUF_SIZE; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidEBuf1[i] = pidError;
        }
        

        //得到反向的阶跃响应，相当于扫描器伸长时误差信号的变化。
        for(i = 0; i < 10; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidEBuf2[i] = pidError;
        }
        
        outZ = pidZ + 32768;
        write_DA(DA_Z, outZ);
        for(i = 10; i < SCANNER_BUF_SIZE; i++) 
        {
            OSMboxPend(MboxCloseLoop, 0, &err); /* use mail box as a semaphore */
            pidEBuf2[i] = pidError;
        }
        setScanDelay(oldLoop);
        enablePID();
        
        // 回传正向的阶跃响应数据，供上位微机分析
        CommWrite(COMM1, (unsigned char*)pidEBuf1, SCANNER_BUF_SIZE * sizeof(short));
        // 回传反向的阶跃响应数据，供上位微机分析
        CommWrite(COMM1, (unsigned char*)pidEBuf2, SCANNER_BUF_SIZE * sizeof(short));
        /*                 0123456789012345678901234567890 */
        PC_DispStr(0, 23, "Scanner Response End     ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    }
}

