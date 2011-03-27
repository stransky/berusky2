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
 * Basic object intersection interface
 */
 
typedef class object_box : 
  public object_world  
{    
  FRAME_CHANGE_INTERFACE chng;
  
private:
  
  bool global_object_box_changed(void)
  {
    return(chng.changed());
  }
    
  void global_object_box_change_set(void)
  {
    chng.change_set();
  }  

  /*
   * Object boxes - local
   */
public:
  
  virtual BOX * object_box_get(void)
  {
    return(NULL);
  }  
  
  virtual bool object_box_changed(void)
  {
    return(FALSE);
  }  
  
  virtual void object_box_set(BOX *p_box)
  {    
  }  
  
public:
  
  virtual BOX_TYPE object_box_type_get(void)
  {  
    return(BOX_NONE);
  }  
  
  virtual void object_box_type_set(BOX_TYPE type)
  {    
  }  
    
  /*
   * Object boxes - global
   */
protected:
    
  BOX *p_box_global;
  
public:  
  
  BOX * object_box_global_get(void)
  {    
    return(p_box_global ? p_box_global : object_box_get());
  }
  
private:  

  bool box_child_changed(void)
  {    
    object_box *p_child = (object_box *)render_hierarchy_child_get();
    
    while(p_child) {
      if(p_child->object_box_changed() || p_child->global_object_box_changed())
        return(TRUE);
      p_child = (object_box *)p_child->render_hierarchy_sibling_get();
    }
    
    return(FALSE);
  }  

  BOX * box_child_create(void);
  
public:
  
  int object_box_hierarchy_update(bool forced = FALSE)
  {    
    if(forced || object_box_changed() || box_child_changed()) {
      if(p_box_global)
        delete p_box_global;
      p_box_global = box_child_create();
      global_object_box_change_set();
      return(TRUE);
    } 
    else {
      return(FALSE);
    }
  }  

public:
    
  object_box(class scene *p_scene)
    : object_world(p_scene), p_box_global(NULL)
  {  
  }
  
  ~object_box(void)
  {
    if(p_box_global)
      delete p_box_global;
  }
  
} OBJECT_BOX;
