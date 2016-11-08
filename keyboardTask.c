#include <libpc.h>
#include <textMode.h>
#include <i8042.h>
#include "task.h"
#include "afmCommand.h"

int currentTask;

static void disp(char c, int x, int y)
{
    static unsigned char detX = 0;
    if(c == 0)
    {
        detX = 0;
    }
    else
    {
        PC_DispChar(x + detX, y, c, DISP_FGND_WHITE + DISP_BGND_BROWN); 
        detX ++;
    }
}
static void getline(char *str, int x, int y)
{
    char c;
    c = i8042_getc() & 0x7f;
    PC_DispStr(x, y, "              ", DISP_FGND_WHITE + DISP_BGND_BROWN);
    while(c != '\r')
    {
        *(str ++) = c;
        disp(c, x, y);
        c = i8042_getc() & 0x7f;
    } 
    *str = 0;
    disp(0, x, y);
}

/*static char inputBuf[128];*/
/*void  keyboardTask (void *pdata)*/
/*{*/
/*    unsigned int c;*/
/*    while (1)*/
/*    {*/
/*        getline(inputBuf, 0, 20);*/
/*        */
/*    }*/
/*}*/

void  keyboardTask (void *pdata)
{
    unsigned int c;
    while (1)
    {
        c = i8042_getc() & 0x7f;
        if(c != 0x00) 
        {
            //putchar(c);
            switch (c)
            {
            case 'd': 
                currentTask = MOTOR_STEP_FORWARD;
                setMotorCommand(MOTOR_STEP_FORWARD, 100);
                OSMboxPost(MboxMotor, (void*) 1);
                //puts("OSMboxPost\n");
                break;
            case 's': 
                currentTask = TASK_STOP;
                setMotorCommand(TASK_STOP, 0);
                break;
            case 'u': 
                currentTask = MOTOR_STEP_BACKWARD;
                setMotorCommand(MOTOR_STEP_BACKWARD, 200);
                OSMboxPost(MboxMotor, (void*) 1);
                break;
            case 'l':
                currentTask = TASK_LINE_SCAN;
                OSMboxPost(MboxLineScan, (void*) 1);
                break;
            case 'c':
                currentTask = MOTOR_AUTO_FORWARD;
                setMotorCommand(MOTOR_AUTO_FORWARD, 1000);
                OSMboxPost(MboxMotor, (void*) 1);
                break;
            }
        }

    }
}

