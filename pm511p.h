#ifndef _PM511P_H_  /* Avoid multiple inclusions */
#define _PM511P_H_

#define PM511P_ADDR         0x100
#define IO_PA               (PM511P_ADDR + 0x08)
#define IO_PB               (PM511P_ADDR + 0x09)
#define IO_PC               (PM511P_ADDR + 0x0A)
#define IO_CONTROL_WORD     (PM511P_ADDR + 0x0B) 
#define ADDR_MOTOR          IO_PB 


#define PM511P_AD_START     (PM511P_ADDR + 0x00)    /* read only */
#define PM511P_AD_CH        (PM511P_ADDR + 0x00)    /* write only */
#define PM511P_AD_CONTROL   (PM511P_ADDR + 0x01)    /* write only */
#define PM511P_TIMER        (PM511P_ADDR + 0x02)    /* write only */
#define PM511P_AD_VALUE     (PM511P_ADDR + 0x02)    /* read only */
#define PM511P_DA_CH        (PM511P_ADDR + 0x04)    /* write only */
#define PM511P_DA_OUT       (PM511P_ADDR + 0x06)    /* write only */

#define IO_CONTROL_WORD     (PM511P_ADDR + 0x0B)    /* write only */
#define AD_CHANNEL          0x00

#define CHANNEL_DELAY       10
#define AD_CONV_DELAY       9
#define DA_CONV_DELAY       9

#define BIT0                0x01
#define BIT1                0x02
#define BIT2                0x04
#define BIT3                0x08
#define BIT4                0x10
#define BIT5                0x20
#define BIT6                0x40
#define BIT7                0x80

#define AD_CONVERTING       BIT0
#define AD_TIMING_START     BIT1
#define AD_EXT_TRIG         BIT3
#define AD_FINISH_INTERR    BIT4
#define AD_TIMING_INTERR    BIT5
#define AD_FIFO_INTERR      BIT6


#define MOTOR_DIRECTION_BIT BIT0
#define MOTOR_WAKEUP_BIT    BIT1
#define MOTOR_SQUARE_BIT    BIT2

#define DA_X                0x00
#define DA_Y                0x01
#define DA_Z                0x02


#define AD_AMPL_CHANNEL     1 /* need to verified */
#define AD_PHASE_CHANNEL    2/* need to verified */
/* In pm511p_IO.c */
void initPm511p(void);
void AD_startTimer(void);
void AD_stopTimer(void);
void AD_setTimer(void);
void MotorSleep(void);
void MotorApproachOneStep(void);
void MotorRetreatOneStep(void);

void write_DA( unsigned char channel, unsigned short value );
short read_AD(unsigned char channel);


/**
*@brief  设置 DDS 输出频率
*@param  freq 实际频率 * 100，这是为了最小频率分辨达到0.01 Hz
*@return  void
*/
void dds_Out(int freq);
/**
*@brief  设置 DDS 输出幅度
*@param  amplitude 输出幅度
*@return  void
*/
void setDDSAmplitude(unsigned char amplitude);
/**
*@brief  设置 DDS 为休眠状态
*@param  void
*@return  void
*/
void DDSSleep(void);
/**
*@brief  设置 DDS 为工作状态
*@param  void
*@return  void
*/
void DDSWakeup(void);
/**
*@brief  设置 DA 为休眠状态
*@param  void
*@return  void
*/
void DASleep(void);
/**
*@brief  设置 DA 为工作状态
*@param  void
*@return  void
*/
void DAWakeup(void);
#endif /* _PM511P_H_ */

