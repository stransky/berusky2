/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * Berusky (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 * Michal Simonik <simonik@anakreon.cz> 
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

#ifndef __COMPAT_MINI_H__
#define __COMPAT_MINI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "compatibility.h"

#include <string.h>
#include <limits.h>
#include <wchar.h>
#ifndef WINDOWS
#include <pthread.h>
#include <fnmatch.h>
#include <dirent.h>
#endif

#define  MAX_JMENO       50

#define  TRUE   (1==1)
#define  FALSE  0

#ifndef  ERROR
#define  ERROR (-1)
#endif

#ifdef WINDOWS
#define DIR_SLASH         '\\'
#define DIR_SLASH_STRING  "\\"
#elif LINUX
#define DIR_SLASH         '/'
#define DIR_SLASH_STRING  "/"
#endif

typedef unsigned char byte;
typedef unsigned char BYTE;
typedef unsigned short word;
typedef unsigned short WORD;
typedef unsigned int dword;
typedef unsigned int COLORREF;
typedef long LONG;
typedef dword DWORD;
typedef float FLOAT;
typedef wchar_t WCHAR;
typedef void *HWND;
typedef void *HDC;
typedef unsigned int UINT;
typedef int BOOL;
typedef void *HINSTANCE;
typedef word FACE;
typedef size_t size_ptr;

extern dword karmin_aktivni;

typedef struct _MOUSE_INFO
{
  int x_orig, 
      y_orig;
  int x,  y;
  int rx, ry;
  int dx, dy;

  int t1, t2;
  int dt1, dt2;
  int d1, d2;
  int tf1, tf2;
  int lt1, lt2;

} MOUSE_INFO;

#define CP_ACP     0
#define HDC2DD     -1
#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))

void kprintf(char log, const char *p_text, ...);
void kprintfl(char log, const char *p_text, ...);
void kerror(char log, const char *p_text, ...);
void kwarning(char log, const char *p_text, ...);
void ddw(const char *p_text, ...);


#ifndef  PATH_MAX
#define  PATH_MAX 1024
#endif

/*
#define  MAX_FILENAME_LENGTH  256
#if PATH_MAX > MAX_FILENAME_LENGTH
#define  MAX_FILENAME         MAX_FILENAME_LENGTH
#else
#define  MAX_FILENAME         PATH_MAX
#endif
*/
#define  MAX_FILENAME  PATH_MAX

extern char ini_file[MAX_FILENAME];

void Sleep(int ms);

char *itoa(int d, char *buf, int base);

void timeGetTimeInit(void);
unsigned int timeGetTime(void);

void GetFileSize(FILE *f, dword *size);

void zero_memory(void *mem, int size);
#define ZeroMemory(m,s) zero_memory(m,s)

void newline_cut(char *p_line);

#ifndef WINDOWS
enum
{
  EAX_ENVIRONMENT_GENERIC,
  EAX_ENVIRONMENT_PADDEDCELL,
  EAX_ENVIRONMENT_ROOM,
  EAX_ENVIRONMENT_BATHROOM,
  EAX_ENVIRONMENT_LIVINGROOM,
  EAX_ENVIRONMENT_STONEROOM,
  EAX_ENVIRONMENT_AUDITORIUM,
  EAX_ENVIRONMENT_CONCERTHALL,
  EAX_ENVIRONMENT_CAVE,
  EAX_ENVIRONMENT_ARENA,
  EAX_ENVIRONMENT_HANGAR,
  EAX_ENVIRONMENT_CARPETEDHALLWAY,
  EAX_ENVIRONMENT_HALLWAY,
  EAX_ENVIRONMENT_STONECORRIDOR,
  EAX_ENVIRONMENT_ALLEY,
  EAX_ENVIRONMENT_FOREST,
  EAX_ENVIRONMENT_CITY,
  EAX_ENVIRONMENT_MOUNTAINS,
  EAX_ENVIRONMENT_QUARRY,
  EAX_ENVIRONMENT_PLAIN,
  EAX_ENVIRONMENT_PARKINGLOT,
  EAX_ENVIRONMENT_SEWERPIPE,
  EAX_ENVIRONMENT_UNDERWATER,
  EAX_ENVIRONMENT_DRUGGED,
  EAX_ENVIRONMENT_DIZZY,
  EAX_ENVIRONMENT_PSYCHOTIC,

  EAX_ENVIRONMENT_COUNT
};

#define THREAD_PRIORITY_TIME_CRITICAL 1
#define THREAD_PRIORITY_HIGHEST       2
#define THREAD_PRIORITY_ABOVE_NORMAL  3
#define THREAD_PRIORITY_NORMAL        4
#define THREAD_PRIORITY_BELOW_NORMAL  5
#define THREAD_PRIORITY_LOWEST        6
#define THREAD_PRIORITY_IDLE          7

#define STILL_ACTIVE                  259

typedef unsigned long THREAD_ID;
typedef pthread_t THREAD_HANDLE;

typedef void * (*thread_routine)(void *);
 
THREAD_HANDLE CreateThread(void *lpThreadAttributes, size_t dwStackSize, thread_routine lpStartAddress,
                           void * lpParameter, dword dwCreationFlags, THREAD_ID *lpThreadId);
int  CloseHandle(THREAD_HANDLE handle);
void ExitThread(dword dwExitCode);
int  SetThreadPriority(THREAD_HANDLE hThread, int nPriority);
int  GetThreadPriority(THREAD_HANDLE hThread);
int  GetExitCodeThread(THREAD_HANDLE hThread, dword *lpExitCode);

#else

typedef unsigned long THREAD_ID;
typedef void * THREAD_HANDLE;

#endif

#define POINTER_TO_INT(pointer) (*((int*)&(pointer)))

#ifdef LINUX
char * _strdate(char *p_tmp);
char * _strtime(char *p_tmp);
#endif

#ifdef __cplusplus
}
#endif

#endif // __COMPAT_MINI_H__
