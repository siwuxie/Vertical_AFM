/* 
    Copyright (C) 2009 Li Yuan (liyuan@ss.buaa.edu.cn)
   
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

/*
    File: pc_display.h

    Author : Li Yuan (liyuan@ss.buaa.edu.cn)

    Purpose: Library routines for access to the display.

    Designed for use with the the GNU C/C++ protected mode 386 compiler.

    Modification History:
	 
*/


#ifndef _TEXT_MODE_H
#define _TEXT_MODE_H

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif


/*
    CONSTANTS: COLOR ATTRIBUTES FOR VGA MONITOR
*/

#define DISP_FGND_BLACK           0x00
#define DISP_FGND_BLUE            0x01
#define DISP_FGND_GREEN           0x02
#define DISP_FGND_CYAN            0x03
#define DISP_FGND_RED             0x04
#define DISP_FGND_PURPLE          0x05
#define DISP_FGND_BROWN           0x06
#define DISP_FGND_LIGHT_GRAY      0x07
#define DISP_FGND_DARK_GRAY       0x08
#define DISP_FGND_LIGHT_BLUE      0x09
#define DISP_FGND_LIGHT_GREEN     0x0A
#define DISP_FGND_LIGHT_CYAN      0x0B
#define DISP_FGND_LIGHT_RED       0x0C
#define DISP_FGND_LIGHT_PURPLE    0x0D
#define DISP_FGND_YELLOW          0x0E
#define DISP_FGND_WHITE           0x0F

#define DISP_BGND_BLACK           0x00
#define DISP_BGND_BLUE            0x10
#define DISP_BGND_GREEN           0x20
#define DISP_BGND_CYAN            0x30
#define DISP_BGND_RED             0x40
#define DISP_BGND_PURPLE          0x50
#define DISP_BGND_BROWN           0x60
#define DISP_BGND_LIGHT_GRAY      0x70

#define DISP_BLINK                0x80

typedef struct ROWCOL
{
    int	first ;
    int	last ;
    int	cursor ;
} ROWCOL ;

typedef struct WINDOW
{
    ROWCOL row;
    ROWCOL col;
    unsigned char color;
    char title[32] ;
} WINDOW ;


/*
    FUNCTION PROTOTYPES
*/

/**
*@brief  初始化字符模式显示，判断是否是 CGA 兼容显卡
*@param  void
*@return void
*/
void PC_initTextMode(void);

/**
*@brief  设置光标位置
*@param  row 行号，从0开始
*@param  col 列号，从0开始
*@return void
*/
void PC_DispSetCur(int col, int row); /* 移动光标 */

/**
*@brief  CLEAR SCREEN
        This function clears the PC's screen by directly accessing video RAM instead of using
        the BIOS.  Each character on the screen is composed of two bytes:
        the ASCII character to appear on the screen followed by a video attribute.  An attribute
        of 0x07 displays the character in WHITE with a black background.
*@param  color   specifies the foreground/background color combination to use
*@return void
*/
void PC_Disp_ClrScr(unsigned char color);

/**
*@brief  屏幕上输出一个字符
*@param  row 行号，从0开始
*@param  col 列号，从0开始
*@param  ch 字符
*@param  color 颜色
*@return void
*/
void PC_DispChar(unsigned char col, unsigned char row, char ch, unsigned char color);

/**
*@brief  屏幕上输出一个字符
*@param  row 行号，从0开始
*@param  col 列号，从0开始
*@param  ch 字符
*@return void
*/
void PC_DispPutCharAt(unsigned char col, unsigned char row, char ch);

/**
*@brief  清除屏幕上一列
*@param  col 列号，从0开始
*@param  color 颜色
*@return void
*/
void PC_Disp_ClrCol(unsigned char col, unsigned char color);

/**
*@brief  清除屏幕上一行
*@param  row 行号，从0开始
*@param  color 颜色
*@return void
*/
void PC_Disp_ClrRow(unsigned char row, unsigned char color);

/**
*@brief  输出一行字符串
*@param  row 行号，从0开始
*@param  color 颜色
*@return void
*/
void PC_DispStr(unsigned char col, unsigned char row, char *string, unsigned char color);

/* ------------------------------------------------------------ */
/* Support for functions implemented in WINDOW.C                */
/* ------------------------------------------------------------ */

/**
*@brief  初始化一个窗体
*@param  win 指向窗体的指针
*@param  title 窗体的名称，字符串，小于 16 个字符, 如果 title 为空，就不画窗体边框
*@param  row_first 起始行号，从 0 开始
*@param  row_last 结束行号，从 0 开始
*@param  col_first 起始列号，从 0 开始
*@param  col_last 结束列号，从 0 开始
*@param  color 窗体颜色，包含前景色和背景色
*@return void
*/
void PC_WindowInit (WINDOW *win, 
                    char *title, 
                    unsigned char col_first, 
                    unsigned char col_last,
                    unsigned char row_first, 
                    unsigned char row_last, 
                    unsigned char color);

/**
*@brief  使用背景色清空一个窗体
*@param  win 指向窗体的指针
*@return void
*/
void PC_WindowErase(WINDOW *win);

/**
*@brief  设置光标到当前窗体
*@param  win 指向窗体的指针
*@return void
*/
void PC_WindowSelect(WINDOW *w);

/**
*@brief  设置光标位置
*@param  win 指向窗体的指针
*@param  row 行号，从0开始
*@param  col 列号，从0开始
*@return void
*/
void PC_WindowSetCursor(WINDOW *win, int col, int row);

/**
*@brief  在指定窗体输出一个字符
*@param  win 指向窗体的指针
*@param  ch 字符
*@return void
*/
void PC_WindowPutChar(WINDOW *w, char ch);

/**
*@brief  在指定窗体输出一个字符串
*@param  win 指向窗体的指针
*@param  str 字符串
*@return void
*/
void PC_WindowPutStr(WINDOW *w, char *str);



void putchar (int c);
void puts(char * str);
void printf (const char *format, ...);
int os_sprintf(char *out, const char *format, ...);
#endif /*_TEXT_MODE_H */
