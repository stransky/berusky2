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
  Settings/Animation for camera
*/

#ifndef __CAMERA_H__
#define __CAMERA_H__

// Camera flags
#define CHANGED_CAMERA              (1<<1)  // Camera matrix was changed

#define CHANGED_CAMERA_3DS          (1<<2)  // 3DS camera was changed
#define CHANGED_CAMERA_POLAR        (1<<3)  // Polar camera was changed

#define CAMERA_POLAR_QUAT           (1<<4)  // Polar camera uses QUAT/angles
#define CAMERA_POLAR_ANGLES         (1<<5)

typedef enum {
  
  CAMERA_TYPE_BASE = 0,
  CAMERA_TYPE_3DS,
  CAMERA_TYPE_POLAR
  
} CAMERA_TYPE;

// Base camera object
typedef class camera_object :
  public scene_object,
  public object_transformation,
  public camera_interface
{

protected:
  
  CAMERA_TYPE     camera_type;
  FLAG_INTERFACE  camera_changes;
  
public:
  
  CAMERA_TYPE camera_type_get(void)
  {
    return(camera_type);
  }

public:
  
  void projection_adjust(void);
  
public:

  virtual GLMATRIX * camera_get(GLMATRIX *p_camera)
  {
    update();
    return(camera_interface::camera_get(p_camera));
  }  
  virtual GLMATRIX * camera_invert_get(GLMATRIX *p_camera_invert)
  {
    update();
    return(camera_interface::camera_invert_get(p_camera_invert));
  }

//protected:  
public:  
  
  virtual GLMATRIX * camera_get(void)
  {
    update();
    return(camera_interface::camera_get());
  }  
  virtual GLMATRIX * camera_invert_get(void)
  {
    update();
    return(camera_interface::camera_invert_get());
  }
  
public:  
  
  void camera_set(GLMATRIX *p_camera)
  {
    camera_interface::camera_set(p_camera);
  }
  void camera_set(GLMATRIX *p_camera, GLMATRIX *p_camera_invert)
  {
    camera_interface::camera_set(p_camera, p_camera_invert);
  }
  void camera_invert_set(GLMATRIX *p_camera_invert)
  {
    camera_interface::camera_invert_set(p_camera_invert);
  }

protected:

  // Update all changed parts of camera
  virtual void update(void) {};
  
public:

  // Set this camera as active (i.e. copy it to scene graphics pipeline)
  void set(void);

public:
  // load/save

protected:
  
  void camera_transformed(void)
  {    
    camera_update();
  }

public:
  
  camera_object(class scene *p_scene)
    : scene_object(p_scene)
  {
    object_transformation_setup(camera_interface::camera_invert_get(),
                               (OBJECT_TRANSFORMATION_FUNC)(&camera_object::camera_transformed));
    camera_type = CAMERA_TYPE_BASE;
  }

} CAMERA_OBJECT, CAMERA_BASE;

// 3DS (position/target/roll) camera interface
typedef class camera_3ds :
  public camera_object
{

protected:  
  
  VECT  target; // target-point of camera
  VECT  pos;    // position of camera
  float roll;   // roll

private:  
  
  void camera_3ds_update(void)
  {
    camera_calc(camera_interface::camera_get(),
                camera_interface::camera_invert_get(), 
                &pos, &target, roll);
    camera_projection_needs_update = TRUE;
  }  
  
public:

  /*
   * Camera position settings
   */
  void position_set(VECT *p_pos)
  {
    pos = *p_pos;
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  void position_set(float x, float y, float z)
  {
    pos.x = x;
    pos.y = y;
    pos.z = z;
    
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  void position_add(VECT *p_pos)
  {
    pos += *p_pos;
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  void position_add(float x, float y, float z)
  {
    pos.x += x;
    pos.y += y;
    pos.z += z;
    
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  VECT * position_get(void)
  {
    return(&pos);
  }
  
  /*
   * Camera target settings
   */
  virtual void target_set(VECT *p_target)
  {
    target = *p_target;
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  virtual void target_set(float x, float y, float z)
  {
    target.x = x;
    target.y = y;
    target.z = z;
    
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  virtual void target_add(VECT *p_target)
  {
    target = *p_target;
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  virtual void target_add(float x, float y, float z)
  {
    target.x += x;
    target.y += y;
    target.z += z;
    
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  VECT * target_get(void)
  {
    return(&target);
  }
    
  /*
   * Angles
   */
  void roll_set(float nroll)
  {
    roll = nroll;
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  void roll_add(float nroll)
  {
    roll += nroll;
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
  float roll_get(void)
  {
    return(roll);
  }

  /*
   * Calculation
   */
  void camera_3ds_set(VECT *p_position, VECT *p_target, float nroll)
  {
    pos = *p_position;
    target = *p_target;
    roll = nroll;
    camera_changes.flag_set(CHANGED_CAMERA_3DS);
  }
  
protected:

  virtual void update(void);
  
protected:
  
  void camera_transformed(void)
  {
    camera_object::camera_transformed();

    VECT old_pos = pos, tmp;
    
    GLMATRIX *p_cam = camera_interface::camera_invert_get();
    p_cam->translation_get(&pos);
    
    tmp = pos - old_pos;
    target += tmp;
  }

public:  
    
  camera_3ds(class scene *p_scene)
    : camera_object(p_scene), target(0,0,0), pos(0,0,-1), roll(0)
  {
    object_transformation_setup(camera_interface::camera_invert_get(),
                               (OBJECT_TRANSFORMATION_FUNC)(&camera_3ds::camera_transformed));
    camera_type = CAMERA_TYPE_3DS;
  }
  
} CAMERA_3DS;

#define MAX_ELEVATION (DEG2RAD(89))

// Spherical (point/angle/angle/distance) camera interface
typedef class camera_polar :
  public camera_3ds
{  
  // Rotation is given by separate angles
  float                 fi;     // evelation of camera
  float                 rfi;    // rotation of camera
  
  float                 dist;   // distance of camera
  
  // Rotation is given by quaternion
  QUAT                  qrotation;
  
private:

  void camera_polar_update(void)
  {
    if(camera_changes.flag_get(CAMERA_POLAR_ANGLES)) {
      camera_calc(camera_interface::camera_get(),
                  camera_interface::camera_invert_get(),
                  &target,rfi,fi,dist);
    }
    else {
      camera_calc(camera_interface::camera_get(),
                  camera_interface::camera_invert_get(),
                  &target,&qrotation,dist);
    }
    camera_projection_needs_update = TRUE;
  }

public:
 
  /*
   * Distances
   */
  void distance_set(float distance)
  {
    dist = distance;
    if(dist < 0)
      dist = 0;
    camera_changes.flag_set(CHANGED_CAMERA_POLAR);
    camera_changes.flag_set(CAMERA_POLAR_ANGLES);
  }
  
  void distance_add(float distance)
  {
    dist += distance;    
    if(dist < 0)
      dist = 0;      
    camera_changes.flag_set(CHANGED_CAMERA_POLAR);
    camera_changes.flag_set(CAMERA_POLAR_ANGLES);
  }
  
  float distance_get(void)
  {    
    return(dist);
  }

  /*
   * Camera elevation
   */
  void  elevation_set(float elevation)
  {
    fi = elevation;
    
    if(fi > MAX_ELEVATION)
      fi = MAX_ELEVATION;
    
    camera_changes.flag_set(CHANGED_CAMERA_POLAR);    
    camera_changes.flag_set(CAMERA_POLAR_ANGLES);
  }
  
  void  elevation_add(float elevation)
  {
    fi += elevation;
    
    if(fi > MAX_ELEVATION)
      fi = MAX_ELEVATION;
    
    camera_changes.flag_set(CHANGED_CAMERA_POLAR);    
    camera_changes.flag_set(CAMERA_POLAR_ANGLES);
  }
  
  float elevation_get(void)
  {
    return(camera_changes.flag_get(CAMERA_POLAR_ANGLES) ? fi : 0.0f);
  }

  /*
   * Camera rotation
   */
  void  rotation_set(float rotation)
  {
    rfi = rotation;
    camera_changes.flag_set(CHANGED_CAMERA_POLAR);
    camera_changes.flag_set(CAMERA_POLAR_ANGLES);
  }
  
  void  rotation_add(float rotation)
  {
    rfi += rotation;
    camera_changes.flag_set(CHANGED_CAMERA_POLAR);
    camera_changes.flag_set(CAMERA_POLAR_ANGLES);
  }
  
  float rotation_get(void)
  {
    return(camera_changes.flag_get(CAMERA_POLAR_ANGLES) ? rfi : 0.0f);
  }

  void  rotation_quat_set(QUAT *p_quat)
  {
    qrotation = *p_quat;
    camera_changes.flag_set(CHANGED_CAMERA_POLAR);
    camera_changes.flag_set(CAMERA_POLAR_QUAT);
  }
  
  QUAT * rotation_quat_get(void)
  {
    return(camera_changes.flag_get(CAMERA_POLAR_QUAT) ? &qrotation : NULL);
  }
  
public:

  virtual void update(void);

protected:
  
  void camera_transformed(void)
  {
    camera_3ds::camera_transformed();
  }

public:
  
  camera_polar(class scene *p_scene)
    : camera_3ds(p_scene), fi(DEG2RAD(0)), rfi(DEG2RAD(0)), dist(1)
  { 
    object_transformation_setup(camera_interface::camera_invert_get(),
                               (OBJECT_TRANSFORMATION_FUNC)(&camera_polar::camera_transformed));
    camera_type = CAMERA_TYPE_POLAR;
  }
  
} CAMERA_POLAR;

// Camera list
typedef class camera_list :  
  public scene_object_list
{

private:

  CAMERA_OBJECT * create_basic_object(CAMERA_TYPE type);
    
public:
  
  CAMERA_OBJECT * create(CAMERA_TYPE type)
  {    
    CAMERA_OBJECT *p_obj = create_basic_object(type);
    object_list_insert(p_obj);
    return(p_obj);
  }

  CAMERA_OBJECT * create(void)
  {
    return(create(CAMERA_TYPE_BASE));
  }
  
  void destroy(SCENE_OBJECT *p_camera)
  {
    object_list_remove(p_camera);
    delete (CAMERA_OBJECT *)p_camera;
  }
    
public:
  
  CAMERA_OBJECT * get_first(void)
  {
    return((CAMERA_OBJECT *)object_list_get_first());
  }
  
public:
  
  CAMERA_OBJECT * p_active;

public:

  void active_set(CAMERA_OBJECT *p_camera)
  {
    p_active = p_camera;  
  }
  
  CAMERA_OBJECT * active_get(void)
  {
    return(p_active);
  }
  
public:

  camera_list(class scene *p_scene)
    : scene_object_list(p_scene)
  {
    p_active = NULL;    
  }
  
  ~camera_list(void)
  {    
  }
  
} CAMERA_LIST;

#endif //__CAMERA_H__
