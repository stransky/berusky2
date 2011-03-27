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

#include "age.h"

vect2di::vect2di(VECT2DF src)
{
  x = (tpos)roundf(src.x);
  y = (tpos)roundf(src.y);
}

vect2di::vect2di(POLAR2DI src)
{
  float tx,ty;
  polar2scalar(src.rotation, src.length, &tx, &ty);
  x = (int)roundf(tx);
  y = (int)roundf(ty);
}

vect2di::vect2di(RECT2DI src)
{
  x = src.x;
  y = src.y;
}

vect2df::vect2df(VECT2DI src)
{
  x = (float)src.x;
  y = (float)src.y;
}

vect3di::vect3di(VECT3DF src)
{
  x = (tpos)roundf(src.x);
  y = (tpos)roundf(src.y);
  z = (tpos)roundf(src.z);
}

vect3df::vect3df(VECT3DI src)
{
  x = (float)src.x;
  y = (float)src.y;
  z = (float)src.z;
}

void rect2di::set(VECT2DI *p_vect, int num)
{
  VECT2DI min,max;
  
  ADJUST_PREP_INT(min.x,max.x);
  ADJUST_PREP_INT(min.y,max.y);
  
  int i;
  for(i = 0; i < num; i++) {
    ADJUST(min.x,max.x,p_vect[i].x);
    ADJUST(min.y,max.y,p_vect[i].y);
  }
  
  set(&min,&max);
}

POLAR2DI interpolate(POLAR2DI p1, POLAR2DI p2, float i)
{
  POLAR2DI  tmp;
  float     i1 = 1.0f - i;
  
  p1.norm();
  p2.norm();
  
  tmp.length = (tpos)(p1.length*i + p2.length*i1);  
  tmp.rotation = (float)(p1.rotation*i + p2.rotation*i1);
  
  return(tmp);
}

POLAR2DI interpolate(POLAR2DI p1, POLAR2DI p2, float i1, float i2)
{
  POLAR2DI  tmp;
  float     i11 = 1.0f - i1;
  float     i21 = 1.0f - i2;

  tmp.length = (tpos)(p1.length*i1 + p2.length*i11);
  tmp.rotation = (float)(p1.rotation*i2 + p2.rotation*i21);
  
  return(tmp);
}

void polar2di::norm(void)
{
  rotation = angle_norm(rotation);
}

polar2di::polar2di(POLAR2DF src)
{
  length = (int)roundf(src.length);
  rotation = src.rotation;
}

polar2di::polar2di(VECT2DI src)
{
  float flenght;
  scalar2polar(src.x, src.y, &rotation, &flenght);
  length = (int)flenght;
}
