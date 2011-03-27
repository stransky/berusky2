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

/* A bitmaps routines
  
  A life of bitmap(s):
  
  (load) -> RGB -> BITMAP -> load_to_vram -> clear_from_ram -> clear_from_vram
                     ^
                     |
  (load) -> A   ------
  
  - SW/HW surfaces
*/

#ifndef __BITMAP_H__
#define __BITMAP_H__

/*
  // returns color with alpha color
  PIXEL_RGBA pixel_add_alpha(PIXEL_RGBA color, PIXEL_RGBA alpha_color)
  {
      
  }
*/
  /*
  byte PackFloatInByte(float in)
  {
    return(byte)((in+1.0f) / 2.0f * 255.0f);
  }
  */
  /*
  dword barva_to_bmp(dword barva)
  {
    return((barva << 8)|(barva >> 24));
  }
*/

/*
  void alfa2bmp(bitmap *p_alfa);
  void to_dot3(void);
  void to_16bit(byte maska, int pruhl);
*/


#endif // __BITMAP_H__
