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

#ifndef __POLAR_H__
#define __POLAR_H__

// return rotation of this vector, +- PI
inline float rotation_get(float x, float y)
{
  VECT2DF tmp(x,y);
  tmp.norm();
  
  float angle = acosf(tmp.x);
  if(tmp.y < 0)
    angle *= -1.0f;
  
  return(angle);
}

inline float lenght_get(float x, float y)
{
  return(vect_size(x,y));
}

inline float lenght_get(float x, float y, float z)
{
  return(vect_size(x,y,z));
}

inline void scalar2polar(float x, float y, float *p_rotation, float *p_lenght)
{
  *p_rotation = rotation_get(x,y);
  *p_lenght = lenght_get(x,y);
}

inline void scalar2polar(float x, float y, float z,
                         float *p_rotation, float *p_elevation, float *p_lenght)
{
  *p_lenght = lenght_get(x,y,z);
  *p_rotation = rotation_get(x,z);
  *p_elevation = rotation_get(x,y);
}

inline void polar2scalar(float rotation, float lenght, float *p_x, float *p_y)
{
  *p_x = cosf(rotation)*lenght;
  *p_y = sinf(rotation)*lenght;
}

inline void polar2scalar(float rotation, float elevation, float lenght,
                         float *p_x, float *p_y, float *p_z)
{
  *p_x = cosf(rotation)*lenght;
  *p_y = sinf(elevation)*lenght;
  *p_z = sinf(rotation)*lenght;
}

inline float angle_norm(float angle)
{
  float mult = angle / (2*PI);
  if(mult > 1) {
    angle -= floorf(mult)*(2*PI);
  }
  if(angle < 0) {
    angle += 2*PI;
  }
  return(angle);
}

// ah*i + al*(1.0f-i)
inline float angle_interpolate_sorted(float ah, float al, float i)
{
  float i1 = 1.0f - i;
  
  float v1 = ah - al;
  float v2 = al + (2*PI-ah);
  
  if(v1 > v2) {
    // normal one
    return(ah*i + al*i1);
  }
  else {
    // move al 
    al += 2*PI;
    return(angle_norm(ah*i + al*i1));
  }
}
// Dist between two normalized angles <0,2PI>
// a1*i + a2*(1.0f-i)
inline float angle_interpolate(float a1, float a2, float i)
{
  // A1 is bigger
  return((a1 < a2) ? angle_interpolate_sorted(a2,a1,1.0f-i) :
                     angle_interpolate_sorted(a1,a2,i));
}

#endif // __POLAR_H__
