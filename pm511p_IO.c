#include <libpc.h>
#include "pm511p.h"

static unsigned char buf_IO_PB = 0, buf_IO_PA = 0, buf_IO_PC = 0;
void MotorSleep(void)
{
    buf_IO_PB &= ~MOTOR_WAKEUP_BIT;     /* put int sleep mode */
    outb(buf_IO_PB, ADDR_MOTOR);
}
void MotorApproachOneStep(void)
{
    static unsigned char i = 0;
    
    buf_IO_PB &= ~MOTOR_DIRECTION_BIT;  /* direction */
    buf_IO_PB |= MOTOR_WAKEUP_BIT;      /* wake up */
    if(i % 2)
    {
        buf_IO_PB |= MOTOR_SQUARE_BIT;  /* square wave high */
        outb(buf_IO_PB, ADDR_MOTOR);
    }
    else
    {
        buf_IO_PB &= ~MOTOR_SQUARE_BIT; /* square wave low */
        outb(buf_IO_PB, ADDR_MOTOR);
    }
    i++;
};

void MotorRetreatOneStep(void)
{
    static unsigned char i = 0;
    buf_IO_PB |= MOTOR_DIRECTION_BIT;   /* direction */
    buf_IO_PB |= MOTOR_WAKEUP_BIT;      /* wake up */
    
    if(i % 2)
    {
        buf_IO_PB |= MOTOR_SQUARE_BIT;  /* square wave high */
        outb(buf_IO_PB, ADDR_MOTOR);
    }
    else
    {
        buf_IO_PB &= ~MOTOR_SQUARE_BIT; /* square wave low */
        outb(buf_IO_PB, ADDR_MOTOR);
    }
    i++;
};

