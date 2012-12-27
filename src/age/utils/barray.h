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

#ifndef  __BIT_ARRAY_H__
#define  __BIT_ARRAY_H__

typedef class bit_array
{
  int *p_pool;
  int  len;
  
  int  offset_mask;
  int  base_shift;

public:
  
  void create(int lenght)
  {
    clear();
    
    int i,tmp = sizeof(p_pool[0])*8;
                                                                              
    for(i = 2, base_shift = 1; i < tmp; i *= 2)                         
      base_shift++;
    
    offset_mask = (i-1);
    
    len = lenght / ((int)(sizeof(p_pool[0])*8)) + 1;
    p_pool = (int *)mmalloc(sizeof(p_pool[0])*len);
    set(0);
  }

  void clear(void)
  {
    ffree(p_pool);
    len = 0;
  }

  void set(int index, bool value)
  {
    if(index>>base_shift < len) {
      if(value) {
        p_pool[index>>base_shift] |= 0x1<<(index&offset_mask);
      }
      else {
        p_pool[index>>base_shift] &= ~(0x1<<(index&offset_mask));
      }
    }
  }

  void set(int value)
  {
    memset(p_pool,value,sizeof(p_pool[0])*len);
  }
  
  bool get(int index)
  {
    if(index>>base_shift < len) {
      return(p_pool[index>>base_shift]&(0x1<<(index&offset_mask)));
    }
    return(FALSE);
  }

  bool loaded(void)
  {
    return(p_pool != NULL);
  }
  
public:  
  
  bit_array(void)
  {
    p_pool = NULL;
    len = 0;
  }
  
} BIT_ARRAY;

#endif // __BIT_ARRAY_H__
