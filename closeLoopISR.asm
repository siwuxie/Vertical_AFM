[SECTION .text]
[BITS 32]

            global CloseLoopISR
            
            extern  OSIntExit ;
            extern  PidISRHandler
            extern  OSIntNesting ;:BYTE
            extern  OSTCBCur ;:DWORD

;*********************************************************************************************************
;                                           HANDLE COM1 ISR
;
; Note(s)    : 1) This code has been modified to work with uC/OS-II V2.52 and higher
;
;              2) The following C-like pseudo-code describe the operation being performed in the code below.
;
;                 Save all registers on the current task's stack;
;                 OSIntNesting++;
;                 if (OSIntNesting == 1) {
;                     OSTCBCur->OSTCBStkPtr = SS:SP
;                 }
;                 CommISRHandler(1);         Handle the communication interrupt in C
;                 OSIntExit();               Notify uC/OS-II about end of ISR
;                 Restore all registers that were save on the current task's stack;
;                 Return from Interrupt;
;*********************************************************************************************************
;
CloseLoopISR:
    PUSHAD                          ; Save interrupted task's context

    INC    BYTE [OSIntNesting]      ; Notify uC/OS-II of ISR
    CMP    BYTE [OSIntNesting], 1   ; if (OSIntNesting == 1)
    JNE    SHORT _CloseLoop_1      
    MOV    EBX, [OSTCBCur]          ; Reload DS
    MOV    [EBX], ESP
    
_CloseLoop_1:
    CALL   PidISRHandler        ; Process COMM interrupt
    
    MOV	AL, 20h
    OUT	020h, AL
    
    CALL   OSIntExit            ; Notify OS of end of ISR    
    POPAD
    IRETD                       ; Return to interrupted task


