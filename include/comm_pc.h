/*
*********************************************************************************************************
*                                     Embedded Systems Building Blocks
*                                  Complete and Ready-to-Use Modules in C
*
*                                    Asynchronous Serial Communications
*                                    IBM-PC Serial I/O Low Level Driver
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : COMM_PC.H
* Programmer : Jean J. Labrosse
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       CONFIGURATION CONSTANTS
*********************************************************************************************************
*/

#ifndef  COMM_PC_H

#define COMM_PC_H

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void Comm1ISR(void) __asm("Comm1ISR");
void Comm2ISR(void) __asm("Comm2ISR");
void Comm1ISR16550(void) __asm("Comm1ISR16550");
void Comm2ISR16550(void) __asm("Comm2ISR16550");
uint8_t CommCfgPort(uint8_t ch, uint32_t baud, uint8_t bits, uint8_t parity, uint8_t stops);
unsigned char CommTest16550Compatible(unsigned char ch);
void  CommSetIntVect16550Mode (unsigned char ch);
void CommSetFIFO(unsigned char ch, unsigned char triggerlevel);
void CommISRHandler(uint8_t ch);
void CommISRHandler16550 (unsigned char ch);
void CommRxFlush(uint8_t ch);
void CommRxIntDis(uint8_t ch);
void CommRxIntEn(uint8_t ch);
void CommTxIntDis(uint8_t ch);
void CommTxIntEn(uint8_t ch);
void CommRclIntVect(uint8_t ch);
void CommSetIntVect(uint8_t ch);
#endif /*COMM_PC_H*/


