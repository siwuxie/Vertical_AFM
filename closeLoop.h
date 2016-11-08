#ifndef _CLOSELOOP_H_  /* Avoid multiple inclusions */
#define _CLOSELOOP_H_

#define PID_DIV     512

extern OS_EVENT * MboxCloseLoop;
extern short pidADValue;
extern int pidError;
extern short pidZ;
extern int pidIntegral;
extern short pidSetPoint;
extern short pidLoop;
extern short pidKp;
extern short pidKi;
extern short pidKd;

extern void CloseLoopISR(void) __asm("CloseLoopISR");
void PidISRHandler(void);
void enablePID(void);
void disablePID(void);
void setPIDSetPoint(short setPoint);
unsigned char getPIDChannel(void);
void setPIDChannel(unsigned char channel);
void setPIDPara(short Kp, short Ki, short Kd);
void setPIDParaOther(short loop);
void setScanDelay(int delay);
void setFeedBackMode(short mode);
#endif
