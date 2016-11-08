/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include <ucos_ii.h>
#include <libpc.h>
#include <textMode.h>
#include <i8042.h>
#include "task.h"
#include "closeLoop.h"
#include "pm511p.h"
#include "display.h"

/*
 * 数据结构及宏定义
 */


/*
 * 全局变量
 */
 /* Tasks stacks                                  */
OS_STK  TaskUpdateDispStk[TASK_STK_SIZE];
OS_STK  TaskStartStk[TASK_STK_SIZE];
OS_STK  TaskMotorStk[TASK_STK_SIZE];
OS_STK  TaskLineScanStk[TASK_STK_SIZE];
OS_STK  TaskNormalScanStk[TASK_STK_SIZE];
//OS_STK  TaskKeyboardStk[TASK_STK_SIZE];
OS_STK  TaskFreqScanStk[TASK_STK_SIZE];
OS_STK  TaskStepResponseStk[TASK_STK_SIZE];
OS_STK  TaskForceCurveStk[TASK_STK_SIZE];
OS_STK  TaskScannerResponseStk[TASK_STK_SIZE];
/*
 * 函数原型
 */
void  TaskUpdateDisp (void *pdata);
void  TaskStart (void *pdata);
//void  KeyboardTask (void *pdata);
//void  MouseTask (void *pdata);

OS_EVENT *MboxMotor;
OS_EVENT *MboxLineScan;
OS_EVENT *MboxNormalScan;
OS_EVENT *MboxFreqScan;
OS_EVENT *MboxPIDStepResponse;
OS_EVENT *MboxForceCurve;
OS_EVENT *MboxScannerResponse;
/*
 * entry.asm初始化结束之后就会进入这里开始执行系统内核的C语言部分.
 */
 
int  main (void)
{
    OSInit();               /* Initialize uC/OS-II                      */
    
    PC_SetISR(uCOS, OSCtxSw);  /* Install uC/OS-II's context switch vector */
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 2);
    

    OSStart();              /* Start multitasking                       */
    while(1);
    return 0;
}

/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
    OS_ENTER_CRITICAL();
    PC_SetTick(OS_TICKS_PER_SEC);
    PC_SetISR(PC_IRQ2INT(IRQ_TICK), OSTickISR);
    PC_EnableIRQ(IRQ_TICK);/* Unmask the timer IRQ in PIC	*/
    OS_EXIT_CRITICAL();  
    
    OSStatInit(); //初始化统计任务

    MboxCloseLoop = OSMboxCreate( (void*) 0 );
    
    OS_ENTER_CRITICAL();
    PC_SetISR(PC_IRQ2INT(IRQ5), CloseLoopISR);
    PC_EnableIRQ(IRQ5);
    outb_p(0, PM511P_AD_CH); // 选择 A/D 通道 0 
    AD_setTimer();
    OS_EXIT_CRITICAL();  
    
//    i8042_kbd_init();
//    OSTaskCreate(keyboardTask, (void *)0, &TaskKeyboardStk[TASK_STK_SIZE - 1], 51);
    MboxLineScan = OSMboxCreate( (void*) 0 );
    MboxNormalScan = OSMboxCreate( (void*) 0 );
    MboxFreqScan = OSMboxCreate( (void*) 0 );
    MboxMotor = OSMboxCreate( (void*) 0 );
    MboxPIDStepResponse = OSMboxCreate( (void*) 0 );
    MboxForceCurve = OSMboxCreate( (void*) 0 );
    MboxScannerResponse = OSMboxCreate( (void*) 0 );
    
    InstallCommTask();
    initPm511p();

    PC_Disp_ClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);
    DispInit();

    OSTaskCreate(TaskUpdateDisp, (void *)0, &TaskUpdateDispStk[TASK_STK_SIZE - 1], 60);
    OSTaskCreate(motorTask, (void *)0, &TaskMotorStk[TASK_STK_SIZE - 1], 59);
    OSTaskCreate(lineScanTask, (void *)0, &TaskLineScanStk[TASK_STK_SIZE - 1], 58);
    OSTaskCreate(normalScanTask, (void *)0, &TaskNormalScanStk[TASK_STK_SIZE - 1], 57);
    OSTaskCreate(freqScanTask, (void *)0, &TaskFreqScanStk[TASK_STK_SIZE - 1], 56);
    OSTaskCreate(stepResponseTask, (void *)0, &TaskStepResponseStk[TASK_STK_SIZE - 1], 55);
    OSTaskCreate(forceCurveTask, (void *)0, &TaskForceCurveStk[TASK_STK_SIZE - 1], 54);
    OSTaskCreate(scannerResponseTask, (void *)0, &TaskScannerResponseStk[TASK_STK_SIZE - 1], 53);
    /*test only*/
    //OSMboxPost(MboxLineScan, (void*) 1);
    /*         */
   // while(1);
    while(1)
    {
        UpdateSystemDisp();
        OSTimeDly(1000);
    }
}
void  TaskUpdateDisp (void *pdata)
{
    while(1)
    {
        UpdateStatusDisp();
        OSTimeDly(100);  
    }

}


