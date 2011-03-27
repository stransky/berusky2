/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * AGE (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef  __TYP_H__
#define  __TYP_H__

typedef unsigned int        dword;
typedef unsigned short int  word;
typedef unsigned char       byte;

typedef signed int          sdword;
typedef signed short int    sword;
typedef signed char         sbyte;

#define MAX_BYTE            (0xff)
#define MAX_WORD            (0xffff)
#define MAX_DWORD           (0xffffffff)

#define MAX_BYTE_F          (255.0f)
#define MAX_WORD_F          (65353.0f)

#define ERROR               (-1)

typedef int                 tpos;
typedef unsigned int        tcolor;
typedef int                 tcount;
typedef unsigned char       trgbcomp;
typedef unsigned int        tflag;

typedef short int           trec;

typedef int                 tface;

#define MIN_TPOS            (-INT_MAX)
#define MAX_TPOS            INT_MAX

#define INT_UNDEFINED       INT_MAX

// Basic aritmetic operations
typedef enum {
  
  OPERATION_SET         = 0,
  OPERATION_ADD         = 1,
  OPERATION_SUB         = 2,
  OPERATION_MODULATE    = 3,
  OPERATION_MODULATE2X  = 4
  
} ARITMETIC_OPERATOR;

#endif //__TYP_H__
