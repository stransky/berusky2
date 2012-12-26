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

BOX * object_box::box_child_create(void)
{
  if(object_box_get() == NULL)
    return(NULL);
  
  BOX_TYPE type = object_box_type_get();
  //bool     translate = FALSE;
  int      i,num = 1;
  
  object_box *p_child = (object_box *)render_hierarchy_child_get();
  while(p_child) {
/*  
    if(p_child->object_box_type_get() != type)
      translate = TRUE;
*/  
    p_child = (object_box *)p_child->render_hierarchy_sibling_get();
    num++;
  }

  BOX  *p_ret = NULL;
  
  if(num == 1) {
    return(box_copy(object_box_get()));
  }
  else {  
    switch(type) {
      case BOX_AABB:
      {      
        AABB *p_nlist = (AABB *)mmalloc(sizeof(AABB)*num);
        
        object_box_get()->to_aabb(p_nlist);
        
        object_box *p_child = (object_box *)render_hierarchy_child_get();
        i = 1;
        while(p_child) {
          BOX *p_box = p_child->object_box_get();
          p_box->to_aabb(p_nlist+i++);
          p_child = (object_box *)p_child->render_hierarchy_sibling_get();
        }
        
        p_ret = new AABB(p_nlist,num);
        free(p_nlist);

        break;
      }
      case BOX_OBB:
      {
        OBB *p_nlist = (OBB *)mmalloc(sizeof(OBB)*num);

        object_box_get()->to_obb(p_nlist);
        
        object_box *p_child = (object_box *)render_hierarchy_child_get();
        i = 1;
        while(p_child) {
          BOX *p_box = p_child->object_box_get();
          p_box->to_obb(p_nlist+i++);
          p_child = (object_box *)p_child->render_hierarchy_sibling_get();
        }
                  
        p_ret = new OBB(p_nlist,num);
        free(p_nlist);
        break;
      }
      case BOX_CIRCLE:
        break;
      default:
        break;
    }  
  }  
  return(p_ret);
}
