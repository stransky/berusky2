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

#ifndef __COMPATIBILITY_H__
#define __COMPATIBILITY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Windows config
 */
#ifdef _WIN32

#define WINDOWS   1
#undef  LINUX

#define  WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#define PATH_MAX	128

#undef  ERROR
#undef  min
#undef  max

#define copysignf(a,b) (float)_copysign((float)a,(float)b)

#endif // _WIN32

/*
 * Linux config
 */
#ifndef _WIN32

#undef  WINDOWS

#ifndef LINUX
#define LINUX   1
#endif // LINUX

#include <unistd.h>

// Enable SDL backend
#undef  ENABLE_GTK_BACKEND
#define ENABLE_SDL_BACKEND 1

// Disable/Enable GUI
#undef ENABLE_GUI

// GL support
#define ENABLE_GL           1

// basic rounds
int ftoi(float t);

#endif // ! _WIN32

#ifdef __cplusplus
}
#endif

#endif // __WINDOWS_COMPATIBILITY_H__
