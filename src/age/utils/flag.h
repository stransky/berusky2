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

/*
  Flag handler (base class)
*/

#ifndef __FLAG_INTERFACE_H__
#define __FLAG_INTERFACE_H__

typedef class flag_interface {
  
  int iflag;
  
public:  

  operator int(void)
  {
    return(iflag);
  }

  bool flag_get(int flag)
  {
    return(iflag&flag);
  }
 
  void flag_set(int flag)
  {
    iflag |= flag;
  }
  
  void flag_clear(int flag)
  {
    iflag &= ~flag;
  }
  
  void flag_set(int flag, bool state)
  {
    if(state) {
      flag_set(flag);
    }
    else {
      flag_clear(flag);
    }    
  }
  
  void flag_set_all(void)
  {
    iflag = 0;
    iflag = ~iflag;
  }
  
  void flag_clear_all(void)
  {
    iflag = 0;
  }

public:  
  
  flag_interface(void)
  {
    flag_clear_all();
  }
    
} FLAG_INTERFACE;

#endif // __FLAG_INTERFACE_H__
