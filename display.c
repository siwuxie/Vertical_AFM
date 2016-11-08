#include <ucos_ii.h>
#include <libpc.h>
#include <textMode.h>
#include "task.h"
#include "closeLoop.h"
#include "display.h"

static void
itoa (char *buf, int d, int size)
{
    unsigned char sign = 0;
    unsigned char remainder;
    buf[size--] = 0;
    if (d < 0)
    {
        sign = 1;
        d = -d;
    };

    do
    {
        remainder = d % 10;
        buf[size--] = remainder + '0';
    }
    while (d /= 10);

    if (sign)
    {
        buf[size--] = '-';
    }

    while (size >= 0)
    {
        buf[size--] = ' ';
    }
}

char space[] = "                                                                                ";
void  DispInit (void)
{
    char str[16];
    PC_initTextMode();
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                      Scanning Force Microscope Controller                      ", DISP_FGND_WHITE + DISP_BGND_RED);
    PC_DispStr( 0,  1, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  2, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  3, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  4, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  5, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  6, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  7, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  8, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  9, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 10, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 11, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 12, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 13, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 14, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 15, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 16, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 17, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 18, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 19, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 20, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 21, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 22, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 23, space, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 24, "Current Time:           Time elapsed:     s   CPU Usage:   %   uC/OS            ", DISP_FGND_WHITE + DISP_BGND_BROWN);
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */

    os_sprintf(str, "V%d.%d", OSVersion() / 100, OSVersion() % 100);   /* Display uC/OS-II's version number    */
    PC_DispStr(69, 24, str, DISP_FGND_WHITE + DISP_BGND_BROWN);
    DispPIDInit();
    UpdatePIDDisp();
    DispScanParaInit();
    DispHyParaInit();
    UpdateScanParaDisp();
    UpdateHYDisp();
}


void  DispPIDInit (void)
{
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  9, "PID Parameters    ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 10, "       P:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 11, "       I:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 12, "       D:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 13, "      SP:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 14, "                  ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 15, "     A/D:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 16, "   Error:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 17, "       Z:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0, 18, "   int Z:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
}

void  DispScanParaInit (void)
{
    PC_DispStr( 0,  2, "Scan Parameters   ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0,  3, " X Range:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0,  4, " Y Range:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0,  5, "X Offset:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0,  6, "Y Offset:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0,  7, "   Delay:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 0,  8, "   Pixel:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
}
void  DispHyParaInit (void)
{
    PC_DispStr( 20,  2, "Hy Parameters ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 20,  3, " X a:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 20,  4, " X b:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 20,  5, " Y a:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
    PC_DispStr( 20,  6, " Y b:         ", DISP_FGND_BLACK + DISP_BGND_GREEN);
}
void UpdatePIDDisp(void)
{
    char str[16];
    itoa (str, pidKp, 6);
    PC_DispStr(9, 10, str, DISP_FGND_WHITE + DISP_BGND_GREEN);

    itoa (str, pidKi, 6);
    PC_DispStr(9, 11, str, DISP_FGND_WHITE + DISP_BGND_GREEN);

    itoa (str, pidKd, 6);
    PC_DispStr(9, 12, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
    
    itoa (str, pidSetPoint, 6);
    PC_DispStr(9, 13, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
    
}
void UpdateHYDisp(void)
{
    char str[16];
    itoa (str, hctc_xA, 6);
    PC_DispStr(26, 3, str, DISP_FGND_WHITE + DISP_BGND_GREEN);

    itoa (str, hctc_xB, 6);
    PC_DispStr(26, 4, str, DISP_FGND_WHITE + DISP_BGND_GREEN);

    itoa (str, hctc_yA, 6);
    PC_DispStr(26, 5, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
    
    itoa (str, hctc_yB, 6);
    PC_DispStr(26, 6, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
}

void UpdateScanParaDisp(void)
{
    char str[16];
    itoa (str, scanRangeX, 6);
    PC_DispStr(9, 3, str, DISP_FGND_WHITE + DISP_BGND_GREEN);

    itoa (str, scanRangeY, 6);
    PC_DispStr(9, 4, str, DISP_FGND_WHITE + DISP_BGND_GREEN);

    itoa (str, scanOffsetX, 6);
    PC_DispStr(9, 5, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
    
    itoa (str, scanOffsetY, 6);
    PC_DispStr(9, 6, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);

    itoa (str, pidLoop, 6);
    PC_DispStr(9, 7, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
    
    itoa (str, scanPixels, 6);
    PC_DispStr(9, 8, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
}

int runTime = 0;
void UpdateSystemDisp(void)
{
    char str[16];
    struct time tn;
    /* Display time elapsed */
    itoa (str, runTime++, 6);
    PC_DispStr(37, 24, str, DISP_FGND_WHITE + DISP_BGND_BROWN); 
    
    /* Display current time */
    //PC_GetTime(&tn);
    //os_sprintf(str, "%02d:%02d:%02d", tn.ti_hour, tn.ti_min, tn.ti_sec);
    //PC_DispStr(13, 24, str, DISP_FGND_WHITE + DISP_BGND_BROWN);            
    
    /* Display CPU usage  */
    itoa (str, OSCPUUsage, 4);
    PC_DispStr(56, 24, str,  DISP_FGND_WHITE + DISP_BGND_BROWN);
}

void UpdateStatusDisp(void)
{
    char str[16];
    itoa (str, pidADValue, 6);
    PC_DispStr(9, 15, str, DISP_FGND_WHITE + DISP_BGND_GREEN);
    
    itoa (str, pidError, 6);
    PC_DispStr(9, 16, str, DISP_FGND_WHITE + DISP_BGND_GREEN);
    
    itoa (str, pidZ, 6);
    PC_DispStr(9, 17, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
    
    itoa (str, pidIntegral >> 5, 6);
    PC_DispStr(9, 18, str,  DISP_FGND_WHITE + DISP_BGND_GREEN);
}

