#ifndef _TASK_H_  /* Avoid multiple inclusions */
#define _TASK_H_

#include "ucos_ii.h"

#define  TASK_STK_SIZE          1024       /* Size of each task's stacks (# of WORDs)            */

#define TASK_STOP           0x00
#define TASK_LINE_SCAN      0x01

#define TASK_MOTOR_APPROACH     0x02
#define TASK_MOTOR_STEP_APP     0x03
#define TASK_MOTOR_RETREAT      0x04

#define MOTOR_STEP_DELAY    10
#define MOTOR_MAX_APP_STEP  2000


#define APPROACH_OK         0x01
#define RETREAT_OK          0x02
#define USER_CANCELLED      0x03
#define MAX_STEP            0x04
#define FORCE_CURVE_FINISH  0x01


#define LINESCAN_DELAY      0x01
#define SCAN_DOTS           512
#define FULL_RANGE          65535


extern int scanDelays;

extern int currentTask;

extern OS_EVENT *MboxLineScan;
extern OS_EVENT *MboxNormalScan;
extern OS_EVENT *MboxMotor;
extern OS_EVENT *MboxFreqScan;
extern OS_EVENT *MboxPIDStepResponse;
extern OS_EVENT *MboxForceCurve;
extern OS_EVENT *MboxScannerResponse;

/* define in motorTask.c */
void motorTask(void *pdata);
void setMotorCommand(unsigned short command, unsigned short step);

void keyboardTask (void *pdata);

/* define in forceCurveTask.c */
void forceCurveTask(void *pdata);
void setForceCurvePara(short startZ, short endZ, short steps, short delay);

/* define in scanTask.c */
extern int scanRangeX;
extern int scanRangeY;
extern int scanOffsetX;
extern int scanOffsetY;
extern int scanAngle;
extern int scanPixels;

extern short hctc_xA;
extern short hctc_xB;
extern short hcrc_xA;
extern short hcrc_xB;
extern short hctc_yA;
extern short hctc_yB;
extern short hcrc_yA;
extern short hcrc_yB;
void lineScanTask(void* para);
void normalScanTask(void* para);
void stepResponseTask(void* para);
void scannerResponseTask(void* para);
void setDeltaZ(short det);
void setDeltaSetPoint(short det);
void setScanPixel(int XDots, int YDots);
void setScanRange(int rangeX, int rangeY, int offsetX, int offsetY, int angle);
void setScanChannel(short channel);
void setHysteresisParaX(short a1, short b1, short a2, short b2);
void setHysteresisParaY(short a1, short b1, short a2, short b2);
short autoZoom(void);
void hyCompute(void);
/* define in freqScanTask.c */
/**
*@brief  设置扫频频率范围
*@param  startFreq(unsigned int) 起始频率 * 100
*@param  endFreq(unsigned int) 截止频率 * 100
*@return  void
*/
void setFreqRange(unsigned int startFreq, unsigned int endFreq);
/**
*@brief  设置扫频时的通道
*@param  unsigned short channelBit
*@return  void
*/
void setFreqChannel(unsigned short channelBit);
/**
*@brief  设置扫频时的速度
*@param  unsigned short delay, 单位是 us
*@return  void
*/
void setFreqDelay(unsigned short delay);
/**
*@brief  扫频任务
*@param  void *pdata 暂时没有用到
*@return  void
*/
void freqScanTask(void *pdata);

/* define in commTask.c */
void commTask (void *pdata);
void InstallCommTask(void);
#endif
