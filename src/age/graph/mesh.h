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

/* Game-Mesh
*/
#ifndef __MESH_H__
#define __MESH_H__

#include <assert.h>

class scene;
typedef class scene SCENE;

class game_mesh;
typedef class game_mesh GAME_MESH;

/*
  A structure like in 3D studio

  Animacni Gizmo -> Mesh

  -> Spojeni vice objektu do jednoho objektu (v realnem case?)


  Pozadavky na Meshe:
    - Mesh changes
    - Mesh connections

  Animation_gizmo?

  GAME_MESH (flagy a podobne - konstantni)
    MESH_GEOMETRY (muze se menit nebo jich byt vice)
    MESH_ANIMACE  (muze se menit)
    MESH_LIGHT    (muze se menit)

  -> animatory -> neco co meni geometrii toho meshe??

  GAME_MESH - moznost spojit vice meshu do jednoho meshe
*/

/*
  Vertexes in mesh

  Usage:
  
  lock()  
    *_get()
      modify data    
  unlock()
  
*/

typedef class mesh_vertex : public lock_interface {

  bool              videoram;
  
private:
  
  int               vertexnum;
  
  int               position_num;
  VECT            **p_pos;          // positions
  
  int               normal_num;
  VECT            **p_norm;         // normals

  int               diffuse_num;
  RGBAF           **p_diffuse;      // diffuse colors
  
  int               specular_num;
  RGBF            **p_specular;     // specular colors
    
  int               text_num;
  VECTUV          **p_uv;           // textute coordinates
  
private:
  
  void vertex_allocate(void);
  void vertex_delete(void);
  
public:
  
  void array_position_add(int num = 1);
  void array_normal_add(int num = 1);
  void array_diff_add(int num = 1);
  void array_spec_add(int num = 1);
  void array_text_add(int num = 1);


  void array_set(int positions = 1, int normals = 0, int diff = 0, int spec = 0, int texts = 0);
  void array_delete(void);

public:

  int vertexnum_get(void)
  {
    return(vertexnum);
  }

  VECT   * position_get(int num = 0);
  VECT   * normal_get(int num = 0);
  RGBAF  * diffuse_get(int num = 0);
  RGBF   * specular_get(int num = 0);
  VECTUV * uv_get(int num = 0);
  
private:

  void upload(void);
  void move_to_vram(void);
 
public:

  // submit one indice for rendering (direct-mode)
  void set(int indice);

  // set whole object for rendering (a.k.a. bind buffer)
  void set(void);

public:
  
  mesh_vertex(int vertexes, bool in_videoram = FALSE)
  {
    videoram = in_videoram;
    
    vertexnum = vertexes;
        
    position_num = 0;
    p_pos = NULL;
    
    normal_num = 0;
    p_norm = NULL;
    
    diffuse_num = 0;
    p_diffuse = NULL;
    
    specular_num = 0;
    p_specular = NULL;
    
    text_num = 0;
    p_uv = NULL;    
  }
    
  mesh_vertex(class mesh_vertex &src)
  {
    assert(0); // isn't allowed
  }
  
  ~mesh_vertex(void)
  {
    vertex_delete();
    array_delete();
  }
 
} MESH_VERTEX;

/*
 * Basic rendering method -> from face array
 */
//#define FC_TRIANGLE_LIST    0x10      // render as triangle list

typedef class mesh_face : public lock_interface {
  
protected:  
  
  bool videoram;
  
protected:
  
  MESH_VERTEX *p_vertex;

public:
  
  void vertex_set(MESH_VERTEX *p_vrt)
  {
    p_vertex = p_vrt;
  }

public:  
  
  virtual void draw(void) = 0;

public:
  
  mesh_face(MESH_VERTEX *p_vert, bool in_videoram = FALSE)
  {
    vertex_set(p_vert);
    videoram = in_videoram;
  }
  
  mesh_face(class mesh_face &src)
  {
    assert(0);
  }
  
  virtual ~mesh_face(void)
  {
    
  }
  
} MESH_FACE;

// Base rendering class -> just array of faces
/*  
  Usage:

  1)
    lock()  
      *_get()
        modify data
    unlock()

  2)      
    set(...)
*/

typedef class mesh_face_list : public mesh_face {
  
private:  
  
  tface  *p_face;
  int     facenum;

private:
    
  void destroy(void)
  {
    facenum = 0;
    ffree(p_face);
  }
  
  void create(int face_num)
  {
    destroy();
        
    facenum = face_num;
    p_face = (tface *)mmalloc(sizeof(p_face[0])*facenum);
  }
  
public:
  
  void set(tface *p_faces, int start, int num)
  { 
    assert(start+num < facenum);
    
    lock(LOCK_READ_WRITE);
    memcpy(p_face+start*sizeof(p_face[0]),p_faces,sizeof(p_face[0])*num);
    unlock();
  }  
  
  void set(int index, int indice)
  {
    assert(index < facenum);
    
    lock(LOCK_READ_WRITE);
    p_face[index] = indice;
    unlock();
  }  
  
  tface get(int index)
  { 
    assert(p_face && index < facenum);
    return(p_face[index]);
  }
  
public:  
  
  int facenum_get(void)
  {
    return(facenum);
  }
    
  tface * faces_get(void)
  {    
    return(locked() ? p_face : NULL);
  }
  
private:

  void draw_direct(void);
  void draw_array(void);

public:

  void draw(void)
  {
    if(videoram)
      draw_array();
    else
      draw_direct();
  }

public:
  
  mesh_face_list(MESH_VERTEX *p_vert, int faces = 0, bool in_videoram = FALSE)
    : mesh_face(p_vert, in_videoram)
  {    
    p_face = NULL;
    if(faces)    
      create(faces);
  }

  mesh_face_list(class mesh_face_list &src)
    : mesh_face(NULL)
  {
    assert(0); // it isn't allowed
  }

  ~mesh_face_list(void)
  {
    destroy();
  }

} MESH_FACE_LIST;


// Various geometry flags
#define GEOMETRY_BOX_DIRTY      (1<<0)
#define GEOMETRY_NORMALS_DIRTY  (1<<1)


// Base geometry class - only vertex data
typedef class mesh_geometry  : 
  public flag_interface
{

  friend class game_mesh;
  
  /*
   *  Box change interface
   */
  
  FRAME_CHANGE_INTERFACE  box_changes;
  
public:

  bool box_changed(void)
  {
    return(box_changes.changed() || flag_get(GEOMETRY_BOX_DIRTY));
  }
  
  void box_change_set(void)
  {
    box_changes.change_set();
  }
      
  /*
   *  Mesh vertexes
   */
private:

  MESH_VERTEX *p_vertex;
  
public:
 
  void vertex_delete(void)
  {
    if(p_vertex) {
      delete p_vertex;
      p_vertex = NULL;
    }
  }

  void vertex_new(int vertexes, int flags = 0)
  {
    if(p_vertex)
      vertex_delete();
    
    p_vertex = new MESH_VERTEX(vertexes);
    p_vertex->lock_set(&lck);
  }

public:

  int vertexnum_get(void)
  {
    return(p_vertex ? p_vertex->vertexnum_get() : 0);
  }

  VECT  * position_get(int num = 0)
  {    
    return(p_vertex ? p_vertex->position_get(num) : NULL);
  }
  
  VECT  * normal_get(int num = 0)
  {
    return(p_vertex ? p_vertex->normal_get(num) : NULL);
  }

  RGBAF * diffuse_get(int num = 0)
  {
    return(p_vertex ? p_vertex->diffuse_get(num) : NULL);
  }
  
  RGBF  * specular_get(int num = 0)
  {
    return(p_vertex ? p_vertex->specular_get(num) : NULL);
  }
  
  VECTUV * uv_get(int num = 0)
  {
    return(p_vertex ? p_vertex->uv_get(num) : NULL);
  }
  
  /*
   *  Mesh transformation
   */
public:
  
  void transformate(GLMATRIX *p_mt);

  
  /*
   *  Mesh faces
   */
private:  
  
  MESH_FACE   *p_face;

public:  
  
  void face_delete(void)
  {
    if(p_face) {
      delete p_face;
      p_face = NULL;
    }
  }  
  
  void face_new(int faces, bool videoram = FALSE)
  {
    if(p_face)
      face_delete();
      
    p_face = new MESH_FACE_LIST(p_vertex, faces, videoram);
    p_face->lock_set(&lck);
  }
        
public:
  
  void face_set(tface *p_faces, int start, int num)
  {
    assert(p_face);
    ((MESH_FACE_LIST *)p_face)->set(p_faces, start, num);    
  }  
  
  void face_set(int index, int indice)
  {
    assert(p_face);
    ((MESH_FACE_LIST *)p_face)->set(index, indice);    
  }  
  
  // face is index/3
  void face_set(int face, int i1, int i2, int i3)
  {
    assert(p_face);
    face *= 3;
    ((MESH_FACE_LIST *)p_face)->set(face,  i1);
    ((MESH_FACE_LIST *)p_face)->set(face+1,i2);
    ((MESH_FACE_LIST *)p_face)->set(face+2,i3);
  }
  
  tface face_get(int index)
  { 
    return(((MESH_FACE_LIST *)p_face)->get(index));
  }
  
  tface * faces_get(void)
  {
    return(p_face ? ((MESH_FACE_LIST *)p_face)->faces_get() : NULL);
  }

  int facenum_get(void)
  {    
    return(p_face ? ((MESH_FACE_LIST *)p_face)->facenum_get() : 0);
  }
  
  /*
   * Change face orientation (CW, CCW)
   */
  void face_invert(void);
  
  /*
   * Geometry pivot point
   */
private:
  
  VECT   pivot;
  
public:
  
  VECT * pivot_get(void)
  {
    return(&pivot);
  }
  
  VECT * pivot_get(VECT *p_pivot)
  {
    *p_pivot = pivot;
    return(p_pivot);
  }
  
  void pivot_set(VECT *p_pivot)
  {
    pivot = *p_pivot;
  }

  void pivot_set_center(void)
  {
    center_get(&pivot);
  }
  
  /*
   * Helpers
   */
public:
  
  VECT * center_get(VECT *p_center);
  VECT * size_get(VECT *p_size);
  
  /*
   * Geometry bouding box
   */
private:
  
  BOX_TYPE box_type;
  
public:
  
  BOX_TYPE box_type_get(void)
  {  
    return(box_type);
  }  
  
  void box_type_set(BOX_TYPE type)
  {    
    if(box_type != type) {
      box_remove();
      box_type = type;
    }
  }  
  
private:
  
  BOX  *p_box;

public:

  void  box_remove(void)
  {
    if(p_box) {
      ::box_remove(&p_box);
      flag_set(GEOMETRY_BOX_DIRTY);
    }
  }
  
  void  box_update(bool force = FALSE);

  BOX * box_get(void)
  {
    box_update();
    return(p_box);
  }
    
  void box_set(BOX *p_box_new)
  {
    box_type_set(p_box_new->box_type_get());
    if(!p_box) {
      p_box = box_create(box_type_get());
    }
    p_box->set(p_box_new);
    flag_clear(GEOMETRY_BOX_DIRTY);
  }  
  
  /*
   * Geometry normal vectors calculator
   */
private:

  void normals_update(bool force = FALSE);
  
  /*
   * Update all geometry changes
   * (bouding boxes, normal vectors)
   */
public:
  
  void update(bool force = FALSE)
  {
    box_update(force);
    normals_update(force);
  }

  /*
   * Geometry lock intergace
   */
private:
  
  LOCK_INTERFACE  lck;

private:

  // Lock data
  void lock(LOCK_TYPE type = LOCK_READ)
  {
    if(p_vertex)
      p_vertex->lock(type);
    if(p_face)
      p_face->lock(type);
    
    lck.lock(type);
  }  
    
  // Update all modified data
  bool unlock(LOCK_CHANGE_TYPE changed = LOCK_DEFAULT)
  {
    /*
     * Unlock all parts (if they're present)
     */ 
    if(p_vertex) {
      if(p_vertex->unlock(changed)) {
        flag_set(GEOMETRY_BOX_DIRTY);
      }
    }
    if(p_face) {
      p_face->unlock(changed);
    }
    
    /*
     * Unlock a general lock
     */ 
    /*
     * If geometry is unlocked now and geometry has been changed,
     * update all geometry parts
     */ 
    if(lck.unlock(changed)) {
      
      // Check all subsystems
      if(p_vertex) {
        assert(!p_vertex->locked());
      }
      if(p_face) {
        assert(!p_face->locked());
      }
      
      // Update all parts
      update();
      
      return(TRUE);
    }
    
    return(FALSE);
  }

  bool locked(void)
  {
    return(lck.locked());
  }

private:
  
  void draw_normals(void);

  
  /*
   * Mesh draw
   */
public:
  
  void draw(void)
  {
    if(p_vertex && p_face)
      p_face->draw();
  }
  
public:

  void set(BOX_TYPE geometry_box_type)
  {
    p_vertex = NULL;
    p_face = NULL;
    p_box = NULL;
    
    pivot.set();
    
    box_type_set(geometry_box_type);
  }
  
public:
  
  mesh_geometry(BOX_TYPE geometry_box_type, int vertexnum, int facenum, int flags = 0)
  {
    set(geometry_box_type);
    
    vertex_new(vertexnum);
    face_new(facenum);
  }  

  mesh_geometry(BOX_TYPE geometry_box_type)
  {
    set(geometry_box_type);
  }
    
  mesh_geometry(class mesh_geometry &src)
  {
    assert(0);
  }  
  
  ~mesh_geometry(void)
  {    
  }
  
} MESH_GEOMETRY;

/*
 * Class for meshes and other game objects
 */
typedef class game_mesh :
  public scene_object_geometry
{
  friend class mesh_params;

  /*
   * Mesh flags
   */
public:
  
  FLAG_INTERFACE           mflags;
  FLAG_INTERFACE           m2flags;

  /*
   * Mesh material flags
   */
public:
  
  FLAG_INTERFACE material_flag_get(void)
  {
    return(p_mat ? p_mat->mflags : FLAG_INTERFACE());
  }

  FLAG_INTERFACE material_flag_2_get(void)
  {
    return(p_mat ? p_mat->m2flags : FLAG_INTERFACE());
  }

  /*
   * Material section
   */
private:

  MESH_MATERIAL           *p_mat;         // used material (can be NULL)

public:

  MESH_MATERIAL * material_get(void)
  {
    return(p_mat);
  }

  void material_clear(void)
  {
    if(p_mat) {
      p_mat->reference_dec();
      p_mat = NULL;
    }    
  }
    
  void material_set(MESH_MATERIAL *p_material)
  {
    material_clear();
    
    if(p_material) {
      p_mat = p_material;
      p_mat->reference_add();
    }
  }

  /*
   * Geometry section
   */  
private:

  MESH_GEOMETRY  *p_geom;        // mesh geometry data (can be NULL)

public:
  
  MESH_GEOMETRY * geometry_new(int vertexnum, int facenum, int flags = 0)
  {
    if(p_geom)
      return(NULL);
    p_geom = new MESH_GEOMETRY(object_box_type_get(), vertexnum, facenum, flags);
    return(p_geom);
  }

  void geometry_delete(void)
  {
    if(p_geom) {
      delete p_geom;
      p_geom = NULL;
    }
  }
  
  bool geometry_active(void)
  {
    return(p_geom != NULL);
  }
  
  /*
   * Object box interface - derived from object_box
   */
public:
  
  virtual bool object_box_changed(void)
  {
    return(p_geom ? p_geom->box_changed() : FALSE);
  }  
  
  virtual BOX * object_box_get(void)
  {
    return(p_geom ? p_geom->box_get() : FALSE);
  }  
  
  virtual void object_box_set(BOX *p_box)
  {
    if(p_geom)
      p_geom->box_set(p_box);
  }
  
  /*
   * Object Boxes - Interface
   * Type of box for geometry
   */
private:
  
  BOX_TYPE box_type;
  
public:
  
  /*
   * Geometry box types
   */  
  BOX_TYPE object_box_type_get(void)
  {  
    return(box_type);
  }  
  
  void object_box_type_set(BOX_TYPE type)
  {    
    if(p_geom) {
      p_geom->box_type_set(type);
    }
    box_type = type;
  }
  
  /*
   * Lock interface
   */
public:
  
  MESH_GEOMETRY * lock(LOCK_TYPE type = LOCK_READ)
  {
    if(!p_geom && type == LOCK_READ_WRITE) {
      p_geom = new MESH_GEOMETRY(object_box_type_get());
    }
    
    if(p_geom)
      p_geom->lock(type);
    
    return(p_geom);
  } 

  bool unlock(LOCK_CHANGE_TYPE changed = LOCK_DEFAULT)
  {
    if(p_geom) {
      return(p_geom->unlock(changed));
    }
    return(FALSE);
  }
  
  /*
   * Set mesh world matrix
   */
private:

  void draw_world_set(void);

private:

  void draw_box(void);
  void draw_pivot(void);
  void draw_selection(void);

  /*
   * Vertex / faces manipulation
   */
public:

  VECT    * vertex_position_object_space_get(int *p_vertexnum);
  VECT    * vertex_position_world_space_get(int *p_vertexnum);
  VECT    * vertex_position_camera_space_get(int *p_vertexnum);
  VECT2DI * vertex_position_screen_space_get(int *p_vertexnum);

  tface   * faces_get(int *p_facenum);
      
public:
  
  void draw(void);
  
public:
  
  GAME_MESH * mesh_next(void)
  {
    return((GAME_MESH *)object_list_next());
  }  
  
  void mesh_remove(void);
    
  /*
   * Misc. functions
   */  
public:

  void  print(int ident = 0);

  /*
   * Constructors & so on
   */  

public:
  
  game_mesh(class scene *p_scene = NULL);
  game_mesh(class game_mesh &src);
  ~game_mesh(void);

} GAME_MESH;


/*
 * List of all meshes in scene
 */
typedef class game_mesh_list :  
  public scene_object_list
{

public:  
    
  GAME_MESH * create(void)
  {    
    GAME_MESH *p_mesh = new GAME_MESH(scene_get());
    object_list_insert(p_mesh);
    return(p_mesh);
  }
    
  void remove(GAME_MESH *p_mesh)
  {
    object_list_remove(p_mesh);
    delete (GAME_MESH *)p_mesh;
  }
  
public:
  
  GAME_MESH * get_first(void)
  {
    return((GAME_MESH *)object_list_get_first());
  }

public:
  
  void print(void);
  
public:
  
  game_mesh_list(class scene *p_scene)
    : scene_object_list(p_scene)
  {
  }
  
  ~game_mesh_list(void)
  {
  }
  
} GAME_MESH_LIST;

/*
#define LANI_FRONTA   6

typedef struct _GAME_MESH_ANIMACE {

  int                update;               // update animacniho stromu

  int                akt[LANI_FRONTA];     // cislo aktivni animace, -1 pro zadnou
  int                odkaz;                // odkaz do tabulky

  int                flag[LANI_FRONTA];    // flag animace
  SIMPLE_TRACK_INFO *p_sim[LANI_FRONTA];   // vrchni animace animacniho stromu
  
  dword              time;                 // cas animace (0-1)
  dword              time_stop;
  dword              time_start;           // start-time animace
  dword              time_delka;

  int               *p_flag[LANI_FRONTA];  // kofoluv flag

  dword              start[LANI_FRONTA];   // frame start
  dword              stop[LANI_FRONTA];    // frame konec

} GAME_MESH_ANIMACE;

#define   GAME_MESH_RANI   0x1  //Game-mesh se animuje v sim_animaci

typedef struct _EXTRA_FACE {

  int      typ;
  word     facenum;
  word    *p_face;

} EXTRA_FACE;

typedef struct _GAME_MESH_DATA {

 void                    *p_mesh;      // tatik-mesh

 dword                    m1flag;      // flag materialu 1
 dword                    m2flag;      // flag materialu 2
 dword                    kflag;       // flag kontejneru
 dword                    k2flag;      // druhy flag kontejneru
 int                      mesh_handle; // handle meshe
 int                      mesh_ID;
 
 // Svetla meshe
 void                   **p_light;     // pole svetel meshu
 int                      slightnum;
 int                      lightmax;    // velikost pole na svetla
 int                      lightakt;    // aktualni velikost

 // Extra-dynamicke svetla meshe
 int                      top_edlight; // top_dynamicke svetlo
 //EXTRA_DYN_LIGHT        **p_edlight;
 int                      edlightnum;  // aktualni pocet svetel
 int                      edlightmax;  // maximalni pocet svetel

 // Loadovaci svetla meshe -> kopie z kontejneru
 //struct _STATIC_LIGHT    *p_lslight;   // seznam statickych svetel
 //struct _DYN_LIGHT       *p_ldlight;   // seznam dynamickych svetel
 //struct _EXTRA_DYN_LIGHT *p_lelight;   // seznam extra svetel

} GAME_MESH_DATA;

// GAME-MESH
// Zakladni objekt enginu
// Ma pouze jeden material


// - Provazani kostnich animaci a mesh (objektovych) animaci

typedef struct _GAME_MESH { //celej kontejner

 char                     jmeno[MAX_JMENO];

 dword                    vertexnum;             // pocet vsech vertexu
 VECT                    *p_vertex_pos;          // pozice bodu (po trojicich)
 VECT                    *p_vertex_pos_kosti;    // pozice bodu (po trojicich) pro kostni transformaci
 VECT                    *p_vertex_norm;         // normalove vektory
 VECT                    *p_vertex_norm_kosti;   // normalove vektory pro kosti
 RGBAF                   *p_vertex_diff_material;// materialove difusni barvy
 RGBAF                   *p_vertex_diff;         // vysledne difusni barvy
 RGBAF                   *p_vertex_diff_voda;    // vysledne difusni barvy vody
 BODRGB                  *p_vertex_spec_material;// materialove spekularni barvy
 BODRGB                  *p_vertex_spec;         // vysledne spekularni barvy
 BODRGB                  *p_vertex_spec_voda;    // vysledne spekularni barvy
 BODUV                   *p_vertex_uv1_material; // materialove text-koordinaty 1
 BODUV                   *p_vertex_uv2_material; // materialove text-koordinaty 2
 BODUV                   *p_vertex_uv3_material; // materialove text-koordinaty 3
 BODUV                   *p_vertex_uv4_material; // materialove text-koordinaty 4

 int                      objektu;     // pocet objektu v meshi - pocet pointeru v extra face
 int                     *facenum;     // pocty facu v extra-facich - polich
 int                     *facestart;   // starty facu jednotlivych objektu
 int                     *p_face;
 int                      facevel;     // velikost bloku plosek

 word                    *objektnum;   // pocty vertextu v objektech
 word                    *objektstart; // starty vertextu objektu

 OBB                     *p_obb_local; // lokalni obalky objektu
 OBB                     *p_obb_world; // obalky transformovane do world-space
 OBB                      obb_local;   // lokalni obalka kontejneru
 OBB                      obb_world;   // world-space obalka kontejneru
 
 //EDIT_MATERIAL          **p_mat;       // pole pointeru na materialy na jednotlive objekty
 int                     *p_mat;       // odkaz na meshe
 MLZNA_KOSTKA            *p_mlha;      // mlzna kostka meshe
 
 GLMATRIX                *p_key;       // klicove animacni matice pro kazdy objekt 
 int                     *p_Objekt_ID; // ID objektu
 GLMATRIX                 m;           // top-level matice
 GLMATRIX                 world;       // world-transformace

 GAME_MESH_ANIMACE        siminfo;     // informace o aktualni animaci
 SIMPLE_TRACK_INFO       *p_sim;       // seznam vsech top-level animaci
 int                      simnum;      // pocet simu
 
 dword                   *p_kflag;     // kont-flagy pro urcite kontaky
 
 // Aktualni (vybrana) animace meshe a jeji cas
 SIMPLE_TRACK_INFO       *p_sim_aktivni;   // aktivni animace 
 dword                    sim_aktivni_time;// jeji aktualni cas
 dword                    sim_flag;        // flag meshe

 int                      low_id;      // low-id meshe
 struct _GAME_MESH       *p_next;      // pointer na dalsi mesh v poradi

 GAME_MESH_DATA          *p_data;      // stejne data pro vsechny meshe v multimeshu

 VERTEX_ARRAYS            varray;      // vertex-arrays

 // kazdy objekt ma svoji joint-animaci (pokud to je joint-anim kontejner)
 JOINT_ANIMACE          **p_joint_anim;   // seznam joint animaci

 VECT                     kofola_pivot; // kofolovy pivoty
 int                      mail;
 int                      nekreslit;

} GAME_MESH;
*/

#endif
