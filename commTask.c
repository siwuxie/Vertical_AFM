#include "afmCommand.h"
#include "stdint.h"
#include "comm_pc.h"
#include "commrtos.h"
#include "task.h"
#include "pm511p.h"
#include "textMode.h"
#include "closeLoop.h"
#include "display.h"
#include <ucos_ii.h>

OS_STK  TaskCommStk[TASK_STK_SIZE];

static void CommReadCommand(unsigned char ch, COMMAND *pCmd);
static void DispatchCommand(COMMAND *pCmd);

void InstallCommTask(void)
{
    CommInit();
    CommCfgPort(COMM1, 115200, 8, COMM_PARITY_NONE, 1);
    if( CommTest16550Compatible(COMM1) )
    {
//        puts("Comm1 is 16550 compatible UART!\n");
        CommSetFIFO(COMM1, 0x02);
        CommSetIntVect16550Mode(COMM1);
    }
    else
    {
//        puts("Comm1 is not 16550 compatible UART!\n");
        CommSetIntVect(COMM1);
    }
    CommRxIntEn (COMM1);
    OSTaskCreate(commTask, (void *)0, &(TaskCommStk[TASK_STK_SIZE - 1]), 1); 
}

void  commTask (void *pdata)
{
    COMMAND cmd;
    while(1)
    {
        /* read a command */
        CommReadCommand(COMM1, &cmd);

        /* dispatch command */
        DispatchCommand(&cmd);
    };
}; 

static void CommReadCommand(unsigned char ch, COMMAND *pCmd)
{
    unsigned char *pStr = (unsigned char *)pCmd;
    unsigned char err;
    int i;
    for(i = 0; i < sizeof(COMMAND); i++)
    {
        pStr[i] = CommGetChar(ch, 1000, &err);
        if (err == COMM_RX_TIMEOUT)  i = -1;
    }
}

static void DispatchCommand(COMMAND *pCmd)
{ 
    //int *pInt;
    //分析命令字
    //激活工作线程
    switch(pCmd->cmd)
    {
        /* motor related */
        case MOTOR_STEP_FORWARD:
            currentTask = MOTOR_STEP_FORWARD;
            setMotorCommand(MOTOR_STEP_FORWARD, pCmd->para1);
            OSMboxPost(MboxMotor, (void*) 1);
            /*                 0123456789012345678901234567890 */
            PC_DispStr(0, 23, "Motor Forward            ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            break;
        case MOTOR_STEP_BACKWARD:
            currentTask = MOTOR_STEP_BACKWARD;
            setMotorCommand(MOTOR_STEP_BACKWARD, pCmd->para1);
            OSMboxPost(MboxMotor, (void*) 1);
            break;
        case MOTOR_AUTO_FORWARD:
            currentTask = MOTOR_AUTO_FORWARD;
            setMotorCommand(MOTOR_AUTO_FORWARD, pCmd->para1);
            OSMboxPost(MboxMotor, (void*) 1);
            break;
        case MOTOR_STOP:
            currentTask = TASK_STOP;
            setMotorCommand(TASK_STOP, 0);
            //puts("motor stop\n");
            break;
        
        /* scan related */
        case CMD_LINESCAN_START:
            currentTask = CMD_LINESCAN_START;
            OSMboxPost(MboxLineScan, (void*) 1);
            break;
        case CMD_SCAN_WHOLE:
            currentTask = CMD_SCAN_WHOLE;
            setScanChannel(pCmd->para1);
            OSMboxPost(MboxNormalScan, (void*) 1);
            break;
        case CMD_SCAN_STOP:
            currentTask = TASK_STOP;
            break;
        case SET_SCAN_RANGE:
            setScanRange(pCmd->para1, pCmd->para2, pCmd->para3, pCmd->para4, (short)pCmd->para5);
            UpdateScanParaDisp();
            break;
        case SET_SCAN_PIXEL:
            setScanPixel(pCmd->para1, pCmd->para2);
            UpdateScanParaDisp();
            break;
        case CMD_AUTO_ZOOM:
            autoZoom();
            UpdateScanParaDisp();
            break;

        /* feedback related */
        case SET_FEEDBACK_MODE:
                //setFeedBackMode(pCmd->para1);
            break;
        case SET_PID_PARA:
            //setPIDPara(short Kp, short Ki, short Kd, short loopCircles);
            setPIDPara((short)pCmd->para1, (short)pCmd->para2, (short)pCmd->para3);
            UpdatePIDDisp();
            break;
        case SET_WORKING_POINT:
            setPIDSetPoint((short)pCmd->para1);
            UpdatePIDDisp();
            break;
        case SET_PID_OTHER:
            //setPIDParaOther(short loops);
            setPIDParaOther(pCmd->para1);
            UpdateScanParaDisp();
            break;
        case SET_PID_CHANNEL:
            setPIDChannel(pCmd->para1);            break;
        case CMD_PID_START:
            enablePID();
            //AD_startTimer();
            break;
        case CMD_PID_STOP:
            disablePID();
            //AD_stopTimer();
            break;
        case CMD_PID_RESPONSE:
            setDeltaSetPoint(pCmd->para1);
            OSMboxPost(MboxPIDStepResponse, (void*) 1);;
            break;
        case CMD_SCANNER_RESPONSE:
            setDeltaZ(pCmd->para1);
            OSMboxPost(MboxScannerResponse, (void*) 1);;
            break;
        case CMD_FORCE_CURVE:
            setForceCurvePara(pCmd->para1, pCmd->para2, pCmd->para3, pCmd->para4);
            OSMboxPost(MboxForceCurve, (void*) 1);;
            break;
            
        /* freq sweep related */
        case SET_FREQ_RANGE:
            setFreqRange(*((int *)(&(pCmd->para1))), *((int *)(&(pCmd->para3))));
            /*                 0123456789012345678901234567890 */
            PC_DispStr(0, 23, "setFreqRange             ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            break;
        case SET_FREQ_AMPLITUDE:
            setDDSAmplitude(pCmd->para1 & 0xff);
            /*                 0123456789012345678901234567890 */
            PC_DispStr(0, 23, "setDDSAmplitude          ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            break;
        case SET_WORKING_FREQ:
            dds_Out(*((int *)(&(pCmd->para1))));
            /*                 0123456789012345678901234567890 */
            PC_DispStr(0, 23, "dds Out                  ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            break;
        case CMD_FREQ_STOP:
            DDSSleep();
            /*                 0123456789012345678901234567890 */
            PC_DispStr(0, 23, "DDS Sleep                ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            break;
        case CMD_FREQ_SCAN:
            currentTask = CMD_FREQ_SCAN;
            OSMboxPost(MboxFreqScan, (void*) 1);
            break;
            
        /* hyster correction related */
        case SET_CORRECTION_PARA_X:
            setHysteresisParaX(pCmd->para1, pCmd->para2, pCmd->para3, pCmd->para4);
            UpdateHYDisp();
            break;
        case SET_CORRECTION_PARA_Y:
            setHysteresisParaY(pCmd->para1, pCmd->para2, pCmd->para3, pCmd->para4);
            UpdateHYDisp();
            break;
        case CMD_AUTO_CORRECTION:
            hyCompute();
            UpdateHYDisp();
            break;

    };
    /*                 0123456789012345678901234567890 */
    //PC_DispStr(0, 22, "Receive Command          ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
}


