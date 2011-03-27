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
  Standart object transfromations interface
  Can be connected to any GL/Notified matrix
  All changes can be recorded
*/

#ifndef __OBJECT_TRANSFORMATION_H__
#define __OBJECT_TRANSFORMATION_H__

#define MATRIX_CHECK  { if(p_matrix == NULL) return; }

class object_transformation; 
typedef class object_transformation OBJECT_TRANSFORMATION;
typedef void (object_transformation::*OBJECT_TRANSFORMATION_FUNC)(void);

typedef class object_transformation
{
  
  OBJECT_TRANSFORMATION_FUNC  p_change_function;
  
public:
  
  void change_set(void)
  {
    if(p_change_function)
      (this->*p_change_function)();
  }
  
private:

  GLMATRIX *p_matrix;
  
public:

  void init(void)
  {
    MATRIX_CHECK;
    p_matrix->init();
    change_set();
  }

public:
  
  /*
   * Translations
   */
  void translate(float px, float py, float pz)
  {
    MATRIX_CHECK;
    p_matrix->translate(px, py, pz);
    change_set();
  }

  void translate(VECT *p_pos)
  {
    MATRIX_CHECK;
    p_matrix->translate(p_pos);
    change_set();
  }

  void translation_set(VECT *p_pos)
  {
    MATRIX_CHECK;
    p_matrix->translation_set(p_pos);
    change_set();
  }
  
  void translation_set(float px, float py, float pz)
  {
    MATRIX_CHECK;
    p_matrix->translation_set(px, py, pz);
    change_set();
  }

  void translation_add(VECT *p_pos)
  {
    MATRIX_CHECK;
    p_matrix->translation_add(p_pos);
    change_set();
  }

  void translation_add(float px, float py, float pz)
  {
    MATRIX_CHECK;
    p_matrix->translation_add(px, py, pz);
    change_set();
  }
  
public:
  
  /*
   * Rotations
   */
  void rotation_set(PLANE *p_r, float scale)
  {
    MATRIX_CHECK;
    p_matrix->rotation_set(p_r, scale);
    change_set();
  }
  
  void rotation_set(VECT *p_vx, VECT *p_vy, VECT *p_vz)
  {
    MATRIX_CHECK;
    p_matrix->rotation_set(p_vx, p_vy, p_vz);
    change_set();
  }

  void rotation_set(VECT *p_vx, VECT *p_vy, VECT *p_vz, VECT *p_pos)
  {
    MATRIX_CHECK;
    p_matrix->rotation_set(p_vx, p_vy, p_vz, p_pos);
    change_set();
  }

  void rotation_set(QUAT *p_q)
  {
    MATRIX_CHECK;
    p_matrix->rotation_set(p_q);
    change_set();
  }
  
  void rotation_set(VECT *p_axis, float angle)
  {
    MATRIX_CHECK;
    p_matrix->rotation_set(p_axis, angle);
    change_set();
  }
  
  void rotate_x(float angle)
  {
    MATRIX_CHECK;
    p_matrix->rotate_x(angle);
    change_set();
  }

  void rotate_y(float angle)
  {
    MATRIX_CHECK;
    p_matrix->rotate_y(angle);
    change_set();
  }

  void rotate_z(float angle)
  {    
    MATRIX_CHECK;
    p_matrix->rotate_z(angle);
    change_set();
  }

  void rotation_set_z(float angle)
  {      
    MATRIX_CHECK;
    p_matrix->rotation_set_z(angle);
    change_set();
  }

  void rotation_set_inverse(VECT *p_vx, VECT *p_vy, VECT *p_vz)
  {
    MATRIX_CHECK;
    p_matrix->rotation_set_inverse(p_vx, p_vy, p_vz);
    change_set();
  }
  
  void rotation_set_inverse(VECT *p_vx, VECT *p_vy, VECT *p_vz, VECT *p_pos)
  {
    MATRIX_CHECK;
    p_matrix->rotation_set_inverse(p_vx, p_vy, p_vz, p_pos);
    change_set();
  }
  
  // 2D rotace
  void rotate_2D(VECT *p_pivot, float angle)
  {
    MATRIX_CHECK;
    p_matrix->rotate_2D(p_pivot, angle);
    change_set();
  }
  
  /*
   * Scaling
   */
public:
  
  void scale(float x, float y, float z)
  {
    MATRIX_CHECK;
    p_matrix->scale(x, y, z);
    change_set();
  }
  
  void scale_set(float x, float y, float z)
  {
    MATRIX_CHECK;
    p_matrix->scale_set(x, y, z);
    change_set();
  }

  void scale(VECT *p_scale)
  {
    MATRIX_CHECK;
    p_matrix->scale(p_scale);
    change_set();
  }

  void scale_set(VECT *p_scale)
  {
    MATRIX_CHECK;
    p_matrix->scale_set(p_scale);
    change_set();
  } 

public:
  
  void object_transformation_setup(GLMATRIX *p_affected_matrix = NULL,
                                   OBJECT_TRANSFORMATION_FUNC p_change_func = NULL)
  {
    p_matrix = p_affected_matrix;
    p_change_function = p_change_func;
  }
  
public:

  object_transformation(GLMATRIX *p_affected_matrix = NULL,
                        OBJECT_TRANSFORMATION_FUNC p_change_func = NULL)
  {
    object_transformation_setup(p_affected_matrix,p_change_func);
  }
  
} OBJECT_TRANSFORMATION;

#endif // __OBJECT_TRANSFORMATION_H__
