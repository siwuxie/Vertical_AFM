#include "task.h"
#include "pm511p.h"
#include "ucos_ii.h"
#include "textMode.h"
#include "closeLoop.h"
#include "afmCommand.h"

extern OS_EVENT * MboxMotor;

INT8U MeetMotorStopCondition(void)
{
    if(pidZ > 30000) return 1;
    return 0;
}
/*static INT8U motor_current_step = 0;*/
/*void MotorRetreatOneStep(void)*/
/*{*/
/*    switch(motor_current_step % 4)*/
/*    {*/
/*        case 0: PC_DispChar(39, 24, '-', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*        case 1: PC_DispChar(39, 24, '\\', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*        case 2: PC_DispChar(39, 24, '|', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*        case 3: PC_DispChar(39, 24, '/', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*    }*/
/*    motor_current_step++;*/
/*}*/
/*void MotorApproachOneStep(void)*/
/*{*/
/*    switch(motor_current_step % 4)*/
/*    {*/
/*        case 0: PC_DispChar(39, 24, '-', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*        case 1: PC_DispChar(39, 24, '\\', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*        case 2: PC_DispChar(39, 24, '|', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*        case 3: PC_DispChar(39, 24, '/', DISP_FGND_YELLOW + DISP_BGND_BLUE); break;*/
/*    }*/
/*    motor_current_step--;*/
/*}*/
static unsigned short motorCommand;
static unsigned short motorStep;

void setMotorCommand(unsigned short command, unsigned short step)
{
    motorCommand = command;
    if(step < 0) step = 0;
    if(step > 5000) step = 5000;
    motorStep = step;
}



void motorTask(void *pdata)
{
    INT8U err = 0;
    INT8U stopStatus = 0;
    int step;
//    PC_DispStr(27, 24, "Motor Move:", DISP_FGND_YELLOW + DISP_BGND_BLUE); 
    while(1)
    {
        
        OSMboxPend(MboxMotor, 0, &err); /* use mail box as a semaphore */
        //PC_DispStr(0, 22,"stop", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        if( MOTOR_STEP_BACKWARD == motorCommand ) 
        {/* retreat the tip from the sample */
            stopStatus = RETREAT_OK;
            step = motorStep;
            PC_DispStr(0, 23, "Motor retreat start", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            while(step >= 0)
            {
                MotorRetreatOneStep();
                OSTimeDly(MOTOR_STEP_DELAY);
                step--;
                
                if( MOTOR_STOP == motorCommand ) /* user cancelled */
                {
                    stopStatus = USER_CANCELLED;
                    break;
                };
            };
        }
        else if( MOTOR_STEP_FORWARD == motorCommand ) 
        {/* retreat the tip from the sample */
            stopStatus = APPROACH_OK;
            step = motorStep;
            PC_DispStr(0, 23, "Motor approach start", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            while(step >= 0)
            {
                MotorApproachOneStep();
                OSTimeDly(MOTOR_STEP_DELAY);
                step--;
                
                if( MOTOR_STOP == motorCommand ) /* user cancelled */
                {
                    stopStatus = USER_CANCELLED;
                    break;
                };
            };
        }
        else if( MOTOR_AUTO_FORWARD == motorCommand ) 
        {/* approach the tip to the sample */
            stopStatus = MAX_STEP;
            step = motorStep;
            PC_DispStr(0, 23, "Motor approach start", DISP_FGND_YELLOW + DISP_BGND_BLUE);
            while(step >= 0)
            {
                MotorApproachOneStep();
                OSTimeDly(MOTOR_STEP_DELAY);
                step--;
                if( MeetMotorStopCondition() ) /* user cancelled */
                {
                    stopStatus = APPROACH_OK;
                    break;
                };
                if( MOTOR_STOP == motorCommand ) /* user cancelled */
                {
                    stopStatus = USER_CANCELLED;
                    break;
                };
            };
        }
        MotorSleep();
        //currentTask = TASK_STOP;
/*        switch(stopStatus)*/
/*        {*/
/*            case APPROACH_OK: puts("APPROACH_OK\n");break;*/
/*            case RETREAT_OK: puts("RETREAT_OK\n");break;*/
/*            case USER_CANCELLED: puts("USER_CANCELLED\n");break;*/
/*            case MAX_STEP: puts("MAX_STEP\n");break;*/
/*        }*/
        PC_DispStr(0, 23, "Motor Stop          ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    }
}


