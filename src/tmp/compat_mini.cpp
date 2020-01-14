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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "compat_mini.h"

dword karmin_aktivni = 1;

void zero_memory(void *mem, int size)
{
  memset(mem, 0, (size_t)size);
}

char *itoa(int d, char *buf, int base)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;

/* If %d is specified and D is minus, put `-' in the head. */
  if (base == 'd' && d < 0) {
    *p++ = '-';
    buf++;
    ud = -d;
  }
  else if (base == 'x')
    divisor = 16;

  /* Divide UD by DIVISOR until UD == 0. */
  do {
    int remainder = ud % divisor;

    *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
  }
  while (ud /= divisor);

  /* Terminate BUF. */
  *p = 0;

  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2) {
    char tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
    p1++;
    p2--;
  }

  return(buf);
}

THREAD_HANDLE CreateThread(void *lpThreadAttributes, size_t dwStackSize, thread_routine lpStartAddress,
                           void * lpParameter, dword dwCreationFlags, THREAD_ID *lpThreadId)
{  
  THREAD_HANDLE handle;
  lpThreadId = NULL;
  pthread_create(&handle, NULL, lpStartAddress, lpParameter);
  return(handle);
}

int CloseHandle(THREAD_HANDLE handle)
{
  return 1;
}

void ExitThread(dword dwExitCode)
{
  int ret;
  pthread_exit(&ret);
}

int SetThreadPriority(THREAD_HANDLE hThread, int nPriority)
{
  return 1;
}

int GetThreadPriority(THREAD_HANDLE hThread)
{
  return 0/*THREAD_PRIORITY_NORMAL*/;
}

int GetExitCodeThread(THREAD_HANDLE hThread, dword *lpExitCode)
{
  *lpExitCode = 0;
  return(pthread_join(hThread, NULL));
}

void newline_cut(char *p_line)
{
  int len = strlen(p_line);
  if(len > 1 && (p_line[len-1] == '\n' || p_line[len-1] == '\r')) {
    p_line[len-1] = '\0';
    len--;
  
    if(len > 1 && (p_line[len-1] == '\n' || p_line[len-1] == '\r')) {
      p_line[len-1] = '\0';
      len--;
    }
  }
}

#ifdef LINUX
char * _strdate(char *p_tmp)
{ 
  time_t now = time(NULL);
  strftime(p_tmp, 50, "%D", localtime(&now)); //short month name
  return p_tmp;
}

char * _strtime(char *p_tmp)
{
  time_t now = time((time_t*)NULL);
  struct tm *l_time = localtime(&now);  
  sprintf(p_tmp, "%.2d:%.2d:%.2d", l_time->tm_hour, l_time->tm_min, l_time->tm_sec);
  return p_tmp;
}
#endif

#ifdef __cplusplus
}
#endif
