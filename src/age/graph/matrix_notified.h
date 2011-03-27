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
 * Notified matrix interface
 *
 * Notified matrix is a normal matrix but every change is marked
 *
 */
 
#ifndef __MATRIX_NOTIFIED_H__
#define __MATRIX_NOTIFIED_H__

#include <assert.h>
#include <string.h>

typedef class matrix_notified : 
  public glmatrix
{
  
  FRAME_CHANGE_INTERFACE chng;
  
public:  
  
  bool matrix_notified_changed(void)
  {
    return(chng.changed());
  }
    
  void matrix_notified_change_set(void)
  {
    chng.change_set();
  }
    
public:
  
  matrix_notified * zero(void)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::zero());
  } 

  matrix_notified * init(void)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::init());
  }

public: 

  matrix_notified * invert(void)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::invert());
  }

  class matrix_notified * reflexion(VECT *p_a, VECT *p_b, VECT *p_c)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::reflexion(p_a, p_b, p_c));
  }
                      
  matrix_notified * mult(float scale)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::mult(scale));
  }
    
public:
  
  /*
   * Translations
   */
  matrix_notified * translate(float px, float py, float pz)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::translate(px, py, pz));
  }

  matrix_notified * translate(VECT *p_pos)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::translate(p_pos));
  }

  matrix_notified * translation_set(VECT *p_pos)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::translation_set(p_pos));
  }
  
  matrix_notified * translation_set(float px, float py, float pz)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::translation_set(px, py, pz));
  }

  matrix_notified * translation_add(VECT *p_pos)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::translation_add(p_pos));
  }

  matrix_notified * translation_add(float px, float py, float pz)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::translation_add(px, py, pz));
  }
  
public:
  
  /*
   * Rotations
   */
  matrix_notified * rotation_set(PLANE *p_r, float scale)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set(p_r, scale));
  }
  
  matrix_notified * rotation_set(VECT *p_vx, VECT *p_vy, VECT *p_vz)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set(p_vx, p_vy, p_vz));
  }

  matrix_notified * rotation_set(VECT *p_vx, VECT *p_vy, VECT *p_vz, VECT *p_pos)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set(p_vx, p_vy, p_vz, p_pos));
  }

  /*
      ( 1-2yy-2zz        2xy+2wz          2xz-2wy  )
  M = (  2xy-2wz        1-2xx-2zz         2yz+2wx  )
      (  2xz+2wy         2yz-2wx         1-2xx-2yy )
  */
  matrix_notified * rotation_set(QUAT *p_q)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set(p_q));
  }
  
  matrix_notified * rotation_set(VECT *p_axis, float angle)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set(p_axis, angle));
  }
  
  matrix_notified * rotate_x(float angle)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotate_x(angle));
  }

  matrix_notified * rotate_y(float angle)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotate_y(angle));
  }

  matrix_notified * rotate_z(float angle)
  {    
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotate_z(angle));
  }

  matrix_notified * rotation_set_z(float angle)
  {      
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set_z(angle));
  }

  matrix_notified * rotation_set_inverse(VECT *p_vx, VECT *p_vy, VECT *p_vz)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set_inverse(p_vx, p_vy, p_vz));
  }
  
  matrix_notified * rotation_set_inverse(VECT *p_vx, VECT *p_vy, VECT *p_vz, VECT *p_pos)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotation_set_inverse(p_vx, p_vy, p_vz, p_pos));
  }
  
  // 2D rotace
  matrix_notified * rotate_2D(VECT *p_pivot, float angle)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::rotate_2D(p_pivot, angle));
  }
  
  /*
   * Scaling
   */
public:
  
  matrix_notified * scale(float x, float y, float z)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::scale(x, y, z));
  }
  
  matrix_notified * scale_set(float x, float y, float z)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::scale_set(x, y, z));
  }

  matrix_notified * scale(VECT *p_scale)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::scale(p_scale));
  }

  matrix_notified * scale_set(VECT *p_scale)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::scale_set(p_scale));
  }
  
  /*
   * Projection matrixes
   */
  
public:   

  matrix_notified * projection(float fovy, float aspect, float zNear, float zFar)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::projection(fovy, aspect, zNear, zFar));
  }
  
  /*
   * Pivot transformations
   */
public:   
  
  matrix_notified * pivot(VECT *p_pivot)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::pivot(p_pivot));
  }
  
  matrix_notified * pivot_forward(VECT *p_pivot)
  {
    matrix_notified_change_set();
    return((matrix_notified *)glmatrix::pivot_forward(p_pivot));
  }
  
public:

  void set(GLMATRIX *p_mat)
  {
    matrix_notified_change_set();
    *(glmatrix *)this = *p_mat;
  }
  
  GLMATRIX * get(void)
  {
    return((glmatrix *)this);
  }
  
  GLMATRIX * get(GLMATRIX *p_mat)
  {
    *p_mat = *((GLMATRIX *)this);
    return(p_mat);
  }

  /*
   * Constructors
   */
public:
  
  matrix_notified(void)
  {    
  }
  
public:

  matrix_notified(PLANE *p_r, float scale)
    : glmatrix(p_r,scale)
  {
    matrix_notified_change_set();
  }
  
  matrix_notified(VECT *p_axis, float angle)
    : glmatrix(p_axis,angle)
  {
    matrix_notified_change_set();
  }
  
  matrix_notified(VECT *p_pos, QUAT *p_rot, VECT *p_scs)
    : glmatrix(p_pos, p_rot, p_scs)
  {
    matrix_notified_change_set();
  }
  
  matrix_notified(VECT *p_position)
    : glmatrix(p_position)
  {
    matrix_notified_change_set();
  }

} MATRIX_NOTIFIED;
 
#endif // __MATRIX_NOTIFIED_H__
