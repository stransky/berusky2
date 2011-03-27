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

#include "mem_alloc.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void * mmalloc(int size)
{
   void *p_tmp = malloc(size);
   if(!p_tmp) {
     PERROR(TRUE, "Out of memory! Requested size %d.",size);
     exit(0);
   }
   memset(p_tmp,0,size);
   return(p_tmp);   
}

char * sstrdup(char *p_src)
{
   char *p_tmp = strdup(p_src);
   if(!p_tmp) {
     PERROR(TRUE, "Out of memory!");
     exit(0);
   } else {
     return(p_tmp);
   }
}

void * mmemcpy(void *p_src, int size)
{
  void *p_tmp = mmalloc(size);
  memcpy(p_tmp, p_src, size);
  return(p_tmp);
}

void * rrealloc(void *p_mem, int size)
{
   void *p_tmp = realloc(p_mem,size);
   if(!p_tmp) {
     PERROR(TRUE, "Out of memory!");
     exit(0);
   } else {
     return(p_tmp);
   } 
}

void xfree(void **p_mem)
{
   if(p_mem && *p_mem) {
     free(*p_mem);
     *p_mem = NULL;     
   }
}

#ifdef __cplusplus
}
#endif
