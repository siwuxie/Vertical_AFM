


#ifndef _PM511P_H_	/* Avoid multiple inclusions */
#define	_PM511P_H_

#define PM511P_ADDR	0x100
#define PM511P_AD_START (PM511P_ADDR + 0x00)    /* read only */
#define PM511P_AD_CH (PM511P_ADDR + 0x00)       /* write only */
#define PM511P_AD_CONTROL (PM511P_ADDR + 0x01)       /* write only */
#define PM511P_TIMER (PM511P_ADDR + 0x02)       /* write only */
#define PM511P_AD_VALUE (PM511P_ADDR + 0x02)    /* read only */
#define PM511P_DA_CH (PM511P_ADDR + 0x04)       /* write only */
#define PM511P_DA_OUT (PM511P_ADDR + 0x06)      /* write only */

#define IO_CONTROL_WORD (PM511P_ADDR + 0x0B)    /* write only */
#define AD_CHANNEL	0x00
	
#define CHANNEL_DELAY 10
#define AD_CONVERSION_DELAY 9
#define DA_CONVERSION_DELAY 9

#endif /*_PM511P_H_s*/
