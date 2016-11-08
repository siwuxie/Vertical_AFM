/* Copyright (C) 2009 Li Yuan (liyuan@ss.buaa.edu.cn)
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/*
    File: stdint.h

    Author : Li Yuan (liyuan@ss.buaa.edu.cn)

    Purpose: A few datatypes to make the operand size more obvious.
     注意：stdint.h 是模仿 ISO C99 中定义的类似头文件写成的。但是并没有实现 ISO C99 
        中的所有要求。此头文件是与所用平台编译器相关的，如果改用其他编译器，需相应的修改。
        如果所用编译器支持 ISO C99 标准，可以直接使用编译器提供的 stdint.h 文件。
        
    Designed for use with the the GNU C/C++ protected mode 386 compiler.

    Modification History:
 */
 
#ifndef _STDINT_H
#define _STDINT_H 1

typedef unsigned int size_t;
/* Exact integral types.  */

/* Signed.  */
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long int int64_t;

/* Unsigned.  */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;

/* Small types.  */

/* Signed.  */
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;
typedef long int int_least64_t;


/* Unsigned.  */
typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long int uint_least64_t;

/* Fast types.  */

/* Signed.  */
typedef signed char int_fast8_t;
typedef int int_fast16_t;
typedef int int_fast32_t;
typedef long int int_fast64_t;

/* Unsigned.  */
typedef unsigned char uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long int uint_fast64_t;


#define __INT64_C(c)    c ## LL
#define __UINT64_C(c)	c ## ULL

/* Limits of integral types.  */

/* Minimum of signed integral types.  */
#define INT8_MIN	  (-128)
#define INT16_MIN  (-32767-1)
#define INT32_MIN  (-2147483647-1)
#define INT64_MIN  (-__INT64_C(9223372036854775807)-1)

/* Maximum of signed integral types.  */
#define INT8_MAX	  (127)
#define INT16_MAX  (32767)
#define INT32_MAX  (2147483647)
#define INT64_MAX  (__INT64_C(9223372036854775807))

/* Maximum of unsigned integral types.  */
#define UINT8_MAX  (255)
#define UINT16_MAX (65535)
#define UINT32_MAX (4294967295U)
#define UINT64_MAX (__UINT64_C(18446744073709551615))


/* Minimum of signed integral types having a minimum size.  */
#define INT_LEAST8_MIN  (-128)
#define INT_LEAST16_MIN (-32767-1)
#define INT_LEAST32_MIN (-2147483647-1)
#define INT_LEAST64_MIN (-__INT64_C(9223372036854775807)-1)

/* Maximum of signed integral types having a minimum size.  */
#define INT_LEAST8_MAX  (127)
#define INT_LEAST16_MAX (32767)
#define INT_LEAST32_MAX (2147483647)
#define INT_LEAST64_MAX (__INT64_C(9223372036854775807))

/* Minimum of fast signed integral types having a minimum size.  */
#define INT_FAST8_MIN   (-128)
#define INT_FAST16_MIN  (-2147483647-1)
#define INT_FAST32_MIN  (-2147483647-1)
#define INT_FAST64_MIN  (-__INT64_C(9223372036854775807)-1)

/* Maximum of fast signed integral types having a minimum size.  */
#define INT_FAST8_MAX	(127)
#define INT_FAST16_MAX	(2147483647)
#define INT_FAST32_MAX	(2147483647)
#define INT_FAST64_MAX	(__INT64_C(9223372036854775807))

/* Maximum of fast unsigned integral types having a minimum size.  */
#define UINT_FAST8_MAX  (255)
#define UINT_FAST16_MAX (4294967295U)
#define UINT_FAST32_MAX (4294967295U)
#define UINT_FAST64_MAX (__UINT64_C(18446744073709551615))


#endif /* _STDINT_H */

