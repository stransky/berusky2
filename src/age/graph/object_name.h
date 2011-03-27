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

#ifndef __OBJECT_NAME_H__
#define __OBJECT_NAME_H__

#include <string.h>

typedef class object_name 
  : public object_base
{

public:

  char name[MAX_NAME];
  int  name_ID;
  
public:
  
  char * name_get(void)
  {
    return(name);
  }
  
  void name_set(const char *p_name)
  {
    if(p_name)
      strncpy(name,p_name,MAX_NAME);
    else
      name[0] = '\0';
  }
  
  int name_ID_get(void)
  {
    return(name_ID);
  }
  
  void name_ID_set(int mID)
  {
    name_ID = mID;
  }

public:

  bool match_name(const char *p_name)
  {
    return(strncmp(name,p_name,MAX_NAME) == 0);
  }
    
  bool match_name(int name)
  {
    return(name == name_ID);
  }
  
public:
  
  object_name(void)
  {
    name[0] = '\0';
    name_ID = 0;
  }
    
} OBJECT_NAME;

#endif // __OBJECT_NAME_H__
