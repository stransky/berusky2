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

BOX * box_create(BOX_TYPE type)
{
  switch(type) {
    case BOX_AABB:
      return((BOX *)(new AABB));
    case BOX_OBB:
      return((BOX *)(new OBB));
    case BOX_CIRCLE:
      return(NULL);
    default:
      return(NULL);
  }
}

BOX * box_copy(BOX *p_src)
{
  switch(p_src->box_type_get()) {
    case BOX_AABB:
      return((BOX *)(new AABB(p_src)));
    case BOX_OBB:
      return((BOX *)(new OBB(p_src)));
    case BOX_CIRCLE:
      return(NULL);
    default:
      return(NULL);
  }
}

BOX * box_create(BOX_TYPE type, BOX * p_list, int num)
{
  BOX *p_ret = NULL;
  
  bool translate = FALSE;      
  int i;
  for(i = 0; i < num; i++) {
    if(p_list[i].box_type_get() != type)
      translate = TRUE;
      break;
  }  
  
  switch(type) {
    case BOX_AABB:
    {
      if(translate) {
        AABB *p_nlist = (AABB *)mmalloc(sizeof(AABB)*num);
        for(i = 0; i < num; i++) {
          p_list[i].to_aabb(p_nlist+i);
        }
        p_ret = new AABB(p_nlist,num);
        free(p_nlist);
      }
      else {
        p_ret = new AABB((AABB *)p_list,num);
      }
      break;
    }
    case BOX_OBB:
    {
      if(translate) {
        OBB *p_nlist = (OBB *)mmalloc(sizeof(OBB)*num);
        for(i = 0; i < num; i++) {
          p_list[i].to_obb(p_nlist+i);
        }
        p_ret = new OBB(p_nlist,num);
        free(p_nlist);
      }
      else {
        p_ret = new OBB((OBB *)p_list,num);
      }
      break;
    }
    case BOX_CIRCLE:
      break;
    default:
      break;
  }  
  
  return(p_ret);
}

void box_remove(BOX **p_box)
{
  if(*p_box) {
    delete *p_box;
    p_box = NULL;
  }
}
