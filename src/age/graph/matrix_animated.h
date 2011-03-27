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

#ifndef __MATRIX_ANIMATED_H__
#define __MATRIX_ANIMATED_H__

/*
  
  Animation (bezier / linear)
    ||
    \/
  position/rotation/scale
    ||
    \/
  final notified matrix  

*/

#define CHANGED_POSITION    (1<<1)
#define CHANGED_ROTATION    (1<<2)
#define CHANGED_SCALE       (1<<3)

typedef class matrix_animated :   
  public flag_interface  
{  
  
  VECT  aposition;
  QUAT  arotation;
  VECT  ascale;

  VECT  pivot;
  
public:  

  MATRIX_NOTIFIED * update(MATRIX_NOTIFIED *p_result, MATRIX_NOTIFIED *p_top = NULL)
  {    
    if(flag_get(CHANGED_POSITION|CHANGED_ROTATION|CHANGED_SCALE)) {
      if(flag_get(CHANGED_POSITION)) {
        p_result->translation_set(&aposition);
        flag_clear(CHANGED_POSITION);
      }
      if(flag_get(CHANGED_ROTATION)) {
        p_result->rotation_set(&arotation);
        flag_clear(CHANGED_ROTATION);
      }
      if(flag_get(CHANGED_SCALE)) {
        p_result->scale(&ascale);
        flag_clear(CHANGED_SCALE);
      }      
    } 
    
    if(p_top) {
      if(p_result->matrix_notified_changed() || p_top->matrix_notified_changed()) {
        if(pivot.x != 0.0f || pivot.y != 0.0f || pivot.z != 0.0f) {
          
        }
        else {
          matrix_mult(p_result,p_top,p_result);
        }      
      }      
    }
    
    return(p_result);
  }

public:
  
  matrix_animated(void)
    : aposition(0,0,0), arotation(1,0,0,0), ascale(1,1,1), pivot(0,0,0)
  {
    
  }
  
} MATRIX_ANIMATED;

#endif // __MATRIX_ANIMATED_H__
