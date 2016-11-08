#ifndef _AFM_COMM_
#define _AFM_COMM_

#define VERSION "0.0.4"
#define AUTHOR "LI Yuan"
#define LAST_REVISED "2009.10.14"

#define PI 3.14159265358979
#define SERVER_ADDR "192.168.1.199"
#define SERVER_PORT 7000

#define MAX_LOOP 30
#define ERROR_EPS 20

#define UART_BAUDRATE B115200

typedef struct command
{
    unsigned short cmd;//命令字
    unsigned short para1;//参数 1
    unsigned short para2;//参数 2
    unsigned short para3;//参数 3
    unsigned short para4;//参数 4
    unsigned short para5;//参数 5
} COMMAND;//总共12个字节

typedef struct response
{
    unsigned short cmd;//命令字
    short para1;//参数 1
    short para2;//参数 2
    short para3;//参数 3
    short para4;//参数 4
    short para5;//参数 5
} RESPONSE;//总共12个字节

/* motor related */
#define MOTOR_STEP_FORWARD          0x10
#define MOTOR_STEP_BACKWARD         0x11
#define MOTOR_AUTO_FORWARD          0x12
#define MOTOR_AUTO_BACKWARD         0x17
#define MOTOR_GET_STEPS             0x13
#define MOTOR_FAST_FORWARD          0x14
#define MOTOR_FAST_BACKWARD         0x15
#define MOTOR_STOP                  0x16

/* scan related */
#define CMD_LINESCAN_START          0x20
#define CMD_SCAN_STOP               0x21
#define CMD_FAST_SCAN               0x22
#define CMD_SCAN_WHOLE              0x23
#define SET_SCAN_RANGE              0x24
#define SET_SCAN_PIXEL              0x25
#define CMD_GET_LINE                0x26
#define CMD_GET_IMAGE               0x27
#define CMD_AUTO_ZOOM               0x28

/* feedback related */
#define SET_FEEDBACK_MODE           0x31
#define SET_PID_PARA                0x32
#define SET_PID_OTHER               0x33
#define SET_WORKING_POINT           0x34
#define SET_PID_CHANNEL             0x35
#define CMD_PID_STOP                0x36
#define CMD_PID_START               0x37
#define CMD_PID_RESPONSE            0x38
#define CMD_SCANNER_RESPONSE        0x39

/* freq sweep and force curve related */
#define CMD_FREQ_SCAN               0x40
#define CMD_FREQ_STOP               0x41
#define SET_WORKING_FREQ            0x42
#define SET_FREQ_AMPLITUDE          0x43
#define SET_FREQ_RANGE              0x44

#define CMD_FORCE_CURVE             0x45

/* Hysteresis correction related */
#define SET_CORRECTION_PARA_X       0x50
#define SET_CORRECTION_PARA_Y       0x51
#define CMD_AUTO_CORRECTION         0x52

/* laser related */
#define LASER_ON                    0x60
#define LASER_OFF                   0x61
#define GET_LASER_POS               0x62
#define SET_MODE                    0x63
#define SET_VOLTAGE                 0x64

#define SET_HV_ON                   0x69
#define SET_HV_OFF                  0x6A

/*------------------------------------------------------*/








/*------------------------------------------------------*/
#define LINE_SCAN_ONCE              0x01
#define LINE_SCAN_NOMAL             0x02
#define FAST_SCAN                   0x03
#define NOMAL_SCAN                  0x04
#define STOP                        0x00
#define LASER_POS                   0x06
/*------------------------------------------------------*/
#define SET_LCD_PIC                 0x30
#define SET_LCD_BEEP                0x31
#define SET_CORRECTION_PARA         0x32
/*------------------------------------------------------*/
#define EXPERT_MODE_LED_ON          0xE0
#define EXPERT_MODE_LED_OFF         0xE1
#define EXPERT_MODE_AD              0xf0
#define EXPERT_MODE_DA              0xf1
#define EXPERT_MODE_DDS_FREQ        0xf2
#define EXPERT_MODE_DDS_AMPL        0xf3
#define EXPERT_MODE_DDS_STOP        0xf4
#define EXPERT_MODE_PID             0xf5
#define EXPERT_MODE_LASER_POS       0xf6
#define EXPERT_MODE_IO_OUT1         0xf7
#define EXPERT_MODE_IO_OUT8         0xf8
#define EXPERT_MODE_IO_OUT16        0xf9
#define EXPERT_MODE_DDS_PHASE       0xfA
#define EXPERT_MODE_RS232_OUT       0xE2

/* test only. */
#define EXPERT_MODE_TEST            0xfB
#define EXPERT_MODE_GET_ERR         0xFC
#define EXPERT_MODE_PID_ERR         0xFD

/* test only. */

/*------------------------------------------------------*/
#define MODE_STM                    0x00
#define MODE_AFM_CONTACT            0x01
#define MODE_AFM_TAPPING            0x02
#define MODE_SHEAR_FORCE            0x03

/*------------------------------------------------------*/

#ifdef _DEBUG_
#define DEBUG0( msg ) puts(msg);
#define DEBUG1(msg, val) printf(msg, val);
#define DEBUG2(msg, val1, val2) printf(msg, val1, val2);
#else
#define DEBUG0( msg )
#define DEBUG1(msg, val)
#define DEBUG2(msg, val1, val2)
#endif

#endif
