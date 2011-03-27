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
 * Object world transformation
 */

#ifndef __OBJECT_WORLD_H__
#define __OBJECT_WORLD_H__

/*
  Object location in space 
*/
typedef class object_world : 
  public scene_object,
  public matrix_notified
{
  GLMATRIX world_global;   // Global (computed) world matrix
  
public:
  
  GLMATRIX * world_global_get(void)
  {
    return(&world_global);
  }
  
public:

  void world_set(GLMATRIX *p_world)
  {    
    set(p_world);
  }
  
  GLMATRIX * world_get(void)
  {
    return(get());
  }
  
public:

  void object_world_hierarchy_update(bool forced = FALSE);
    
public:
  
  object_world(class scene *p_scene)
    : scene_object(p_scene)
  {
    init();
    world_global.init();
  }
  
} OBJECT_WORLD;

#endif // __OBJECT_WORLD_H__
