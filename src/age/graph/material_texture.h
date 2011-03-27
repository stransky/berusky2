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
 * Material textures
 */

#ifndef  __MATERIAL_TEXTURE_H__
#define  __MATERIAL_TEXTURE_H__

/*
  Textures can be created from:

  dx,dy   - as an empty texture

  *.tpb		-	texture config file, contains:

    surface	= name of image file
    surface_alpha = name of alpha image file
    class = any number from TEXT_CLASS

  *.bmp/jpg/png.. -	texture surface (bmp, jpg, gif...)
  
  *.txp		-	texture param file, contains:

  	type = "GL_TEXTURE_1D" or "GL_TEXTURE_2D"
  	format = any string from TEXT_FORMAT
  	compression = 0/1
  	bpp = any number from TEXT_BPP
  	anis = 0/1
  	anis_level = float point number
  	sharpness = float point number
*/

#include <assert.h>

/*
 * Bitmap surfaces for texture
 */
#define	 TEXTURE_BITMAP_PARAMS_FILE	".tbp"

/*
 * Bitmap texture parameters
 */
typedef class material_text_bitmap_params {
  
public:  
  
  char        pixmap[MAX_FILENAME];
  char        pixmap_alpha[MAX_FILENAME];
  
public:
  
  bool        load(const char *p_file);

public:
  
  material_text_bitmap_params(void);
  
} MATERIAL_TEXT_BITMAP_PARAMS;

/*
 * Bitmap textures
 */
typedef class material_text_bitmap :  
  public change_interface
{

public:  
  
  FILE_NAME   filename;

private:  
  
  SURFACE_SW  surf;

public:
  
  SURFACE * bitmap_get(void)
  {
    return((SURFACE *)&surf);
  }

  void bitmap_size_get(tpos *p_dx, tpos *p_dy)
  { 
    surf.size_get(p_dx, p_dy);
  }

public:
  
  bool bitmap_loaded(void)
  {
    return(surf.loaded());
  }
  
public:

  bool bitmap_changed(void)
  {
    return(changed());
  }
    
  void bitmap_change_set(void)
  {
    change_set();
  }
  
  void bitmap_change_clear(void)
  {
    change_clear();
  }  
  
public:

  bool bitmap_create(tpos dx, tpos dy);
  bool bitmap_create(const char *p_dir, const char *p_file, const char *p_name = NULL);
  bool bitmap_create(SURFACE *p_surf);
  bool bitmap_destroy(void);

public:
  
  material_text_bitmap(void);
  ~material_text_bitmap(void);

} MATERIAL_TEXT_BITMAP;


/*
 * Texture GL params
 */

/*
 * Extension GL config files
 */
#define	 TEXTURE_GL_PARAM_FILE	  ".tgl"

#define  DEFAULT_MIP_LEVEL 9

typedef enum {

  CLASS_DEFAULT = 0,
  CLASS1 = 1,
  CLASS2 = 2,
  CLASS3 = 3,
  CLASS4 = 4,
  CLASS5 = 5,
  CLASS6 = 6,
  CLASS7 = 7,
  CLASS_CUSTOM

} TEXT_CLASS;

#define TEXT_CLASSES_NUM 9

typedef enum {

  FORMAT_RGB        = 0,
  FORMAT_RGBA1      = 1,
  FORMAT_RGBA4      = 2,
  FORMAT_LUM        = 3,
  FORMAT_LUM_ALPHA1 = 4,
  FORMAT_LUM_ALPHA4 = 5,
  FORMAT_ALPHA      = 6,
  FORMAT_LIGHTMAP   = 7

} TEXT_FORMAT;

#define TEXT_FORMATS_NUM 8

typedef enum {

  FILTER_NEAR,
  FILTER_LINEAR,
  FILTER_MIP_NEAR,
  FILTER_MIP_LINEAR,
  FILTER_MIP_TRILINEAR

} TEXT_FILTER;

typedef enum {

  TEXT_REPEAT,
  TEXT_CLAMP

} TEXT_WRAP;

typedef enum {

  TEXT_DEFAULT  = 0,
  TEXT_16BPP    = 1,
  TEXT_32BPP    = 2,
  TEXT_DTX      = 3

} TEXT_BPP;

#define  TEXT_BPP_NUM       4

#define  TEXT_DETAIL_BASE   0
#define  TEXT_MAX_DEFAULT   4096

#define  TXT1_GEN_DOT3      0x0001  // texture will be converted to DOT3 format
#define  TXT1_GEN_DUDV      0x0002  // texture will be converted to DUDV format

#define  TXT2_USED          0x0001
#define  TXT2_FLARE         0x0002
#define  TXT2_NO_CULL       0x0004
#define  TXT2_BUMP          0x0008
#define  TXT2_ALPHA         0x0010

// Parameters what can be attached to any texture
typedef struct material_text_gl_params {

  GLenum        type;          // texture type (1D/2D/3D/4D/5D/6D)
  TEXT_FORMAT   format;        // texture format (from default_texture_config)
  int           compression;   // texture compression
  TEXT_BPP      bpp;           // bit deph
  TEXT_FILTER   filter;        // texture filter
  TEXT_WRAP     wrap;          // wrap params
  int           anis;          // Anisotropy filter
  float         anis_level;    // Anisotropy filter level
  float         sharpness;     // texture sharpness
  tflag         flag1;         // texture flag 1
  tflag         flag2;         // texture flag 2

  /*
   * Load GL params from given TEXTURE_GL_PARAM_FILE
   */
public:

  bool load(char *p_file);

} MATERIAL_TEXT_GL_PARAMS;

/*
 * Default texture configuration (for given classes)
 */
typedef struct default_text_config {
  
  /*
   * Default GL params for each text class
   */
  static MATERIAL_TEXT_GL_PARAMS params[TEXT_CLASSES_NUM];

public:
  
  MATERIAL_TEXT_GL_PARAMS * params_get(TEXT_CLASS texture_class);

  /*
   * Default texture formats
   */
public:
  
  static int                        format1d[TEXT_BPP_NUM][TEXT_FORMATS_NUM]; // Format table for 1d textures
  static int                        format2d[TEXT_BPP_NUM][TEXT_FORMATS_NUM]; // Format table for 2d textures

public:
  
  int format_get(int type, int bpp, int format_index);

  /*
   * Default params
   */
public:

  static int                        details[TEXT_CLASSES_NUM];  // LOD for each texture class

  static int                        max_x[TEXT_CLASSES_NUM];    // max texture resolution (x)
  static int                        max_y[TEXT_CLASSES_NUM];    // max texture resolution (y)

  /*
   * Load the whole default configuration
   */
public:

  void load(char *p_file);

} DEFAULT_TEXT_CONFIG;

DEFAULT_TEXT_CONFIG * default_text_config_get(void);

/*
 * Video-ram texture
 */
typedef class material_text_gl :
  public material_text_bitmap
{
  /*
   * Texture class
   */
private:
  
  TEXT_CLASS tclass;

public:  
  
  void gl_text_class_set(TEXT_CLASS cl)
  {
    if(cl == CLASS_CUSTOM) {
      gl_text_params_custom_create();
    }
    else {
      gl_text_params_custom_destroy();
    }
    tclass = cl;
  }
  
  TEXT_CLASS gl_text_class_get(void)
  {
    return(tclass);
  }
  
  /*
   * Texture parameters
   */
private:
  
  // if it's NULL, take the default one from TXT_CONFIG
  MATERIAL_TEXT_GL_PARAMS *p_params;
  
public:
  
  void gl_text_params_create(void);
  void gl_text_params_destroy(void);
  bool gl_text_params_load(char *p_file);

public:

  void gl_text_params_custom_create(void)
  {
    gl_text_params_create();
    tclass = CLASS_CUSTOM;
  }
  void gl_text_params_custom_destroy(void)
  {
    gl_text_params_destroy();
    tclass = CLASS_DEFAULT;
  }
  
public:

  MATERIAL_TEXT_GL_PARAMS * gl_text_params_get(void)
  {
    if(p_params)
      return(p_params);
    
    return(default_text_config_get()->params_get(tclass));
  }
  
public:

  int gl_text_type_get(void)
  { 
    return(gl_text_params_get()->type);
  }
  bool gl_text_type_set(int type)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->type = type;
      return(TRUE);
    }
    return(FALSE);
  }
  
  int gl_text_bpp_get(void)
  { 
    return(gl_text_params_get()->bpp);
  }
  bool gl_text_bpp_set(TEXT_BPP bpp)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->bpp = bpp;
      return(TRUE);
    }
    return(FALSE);
  }
  
  int gl_text_format_get(void)
  {
    return(gl_text_params_get()->format);
  }
  bool gl_text_format_set(TEXT_FORMAT format)
  {
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->format = format;
      return(TRUE);
    }
    return(FALSE);
  }
  
  int gl_text_gl_format_get(void)
  { 
    DEFAULT_TEXT_CONFIG *p_pr = default_text_config_get();
    return(p_pr->format_get(gl_text_type_get(), 
                            gl_text_bpp_get(),
                            gl_text_format_get()));
  }
  bool gl_text_gl_format_set(void)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      return(TRUE);
    }
    return(FALSE);
  }  

  int gl_text_compression_get(void)
  { 
    return(gl_text_params_get()->compression);
  }
  bool gl_text_compression_set(int compression)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->compression = compression;
      return(TRUE);
    }
    return(FALSE);
  }
  
  TEXT_FILTER gl_text_filter_get(void)
  { 
    return(gl_text_params_get()->filter);
  }
  bool gl_text_filter_set(TEXT_FILTER filter)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->filter = filter;
      return(TRUE);
    }
    return(FALSE);
  }

  TEXT_WRAP gl_text_wrap_get(void)
  { 
    return(gl_text_params_get()->wrap);
  }
  bool gl_text_wrap_set(TEXT_WRAP wrap)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->wrap = wrap;
      return(TRUE);
    }
    return(FALSE);
  }
  
  float gl_text_sharpness_get(void)
  { 
    return(gl_text_params_get()->sharpness);
  }
  bool gl_text_sharpness_set(float sharpness)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->sharpness = sharpness;
      return(TRUE);
    }
    return(FALSE);
  }

  bool gl_text_anisotrophy_filter_get(void)
  { 
    return(gl_text_params_get()->anis);
  }
  bool gl_text_anisotrophy_filter_set(bool anis)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->anis = anis;
      return(TRUE);
    }
    return(FALSE);
  }

  float gl_text_anisotrophy_level_get(void)
  { 
    return(gl_text_params_get()->anis_level);
  }
  bool gl_text_anisotrophy_level_set(float anis_level)
  { 
    if(!gl_text_loaded()) {
      gl_text_params_custom_create();
      p_params->anis_level = anis_level;
      return(TRUE);
    }
    return(FALSE);
  }
  
  /*
   * OpenGL texture name
   */
private:  

  GLuint gl_text_name;

public:

  bool gl_text_loaded(void)
  {
    return(gl_text_name != 0);
  }
  
public:
  
  int gl_text_get(void)
  { 
    return(gl_text_name);
  }

public:    
  
  void gl_text_bind(void)
  { 
    glBindTexture(gl_text_type_get(), gl_text_get());
  }
  
  bool gl_text_create(void);
  bool gl_text_destroy(void);
    
public:

  material_text_gl(void);
  ~material_text_gl(void);
 
} MATERIAL_TEXT_GL;

/*
 * A final material texture
 */
typedef class material_text :
  public material_text_gl,
  public object_list
{

public:
  
  bool loaded(void);
  
  bool load(void);
  bool load(const char *p_dir, const char *p_file, const char *p_name = NULL);
  bool load(SURFACE *p_surf);
  bool save(const char *p_dir, const char *p_file = NULL);
  bool destroy(void);

public:
  
  void texture_remove(void);

public:

  void print(void)
  {
    pprintf("Texture: %s",name_get());
  }

public:

  material_text(SCENE *p_scene);
  ~material_text(void);

public:
  
} MATERIAL_TEXT;


#define TEXT_DIRS 20

typedef class material_text_list :  
  public object_list_head
{
  
public:
        
  MATERIAL_TEXT * create(void)
  {
    MATERIAL_TEXT *p_new = new MATERIAL_TEXT(scene_get());
    object_list_insert(p_new);
    return(p_new);
  }
  
  MATERIAL_TEXT * create_and_load(const char *p_dir, const char *p_file, const char *p_name = NULL)
  {
    MATERIAL_TEXT *p_text = create();
    p_text->load(p_dir, p_file, p_name);
    return(p_text);
  }
  
  void destroy(MATERIAL_TEXT *p_text)
  {
    object_list_remove(p_text);
    delete (MATERIAL_TEXT *)p_text;
  }
  
public:
  
  MATERIAL_TEXT * get_first(void)
  {
    return((MATERIAL_TEXT *)object_list_get_first());
  }  

  // Texture directories
private:

  int     texture_dir_last;
  char    texture_dir[TEXT_DIRS][MAX_FILENAME];    // adresare textur
  
public:
  
  bool    dir_add(const char *p_dir);
  void    dir_reset(void);

private:
  
  int     dir_next_new(void);
  char *  dir_next(int *p_handle);

public:    

  int     size_get(void);
  int     num_get(void);

public:    

  int     errors_print(void);
  int     status_print(void);

public:    

  void    reload(void);

public:

  material_text_list(SCENE *p_scene_ref);
  ~material_text_list(void);

} MATERIAL_TEXT_LIST;

/*
 ***************************************************************************
    Texture-coordinates generator setup
 ***************************************************************************
*/
typedef class material_text_coordinates {

  // Texture transformation matrix
  bool            text_matrix_state;
  GLMATRIX        text_matrix;
  
public:

  GLMATRIX  * text_matrix_get(void)
  {
    return(&text_matrix);
  }
  
public:

  void set(int unit)
  {
    if(text_matrix_state) {
      gl_texture_coordinates::matrix_on(unit, &text_matrix);
    }
    else {
      gl_texture_coordinates::matrix_off(unit);
    }
  }
  
/*
private:
  
  // Texture generator configuration
  bool            generator_state;
  
  bool            generator_s_state;
  float           generator_s[4];
  
  bool            generator_t_state;  
  float           generator_t[4];
  
  bool            generator_r_state;
  float           generator_r[4];
  
  bool            generator_q_state;
  float           generator_q[4];

public:

  
  //  void set(int text_unit, int m1flag, int m2flag, int mat_text)

  //       text_unit    - do jake jednotky se generator ma zapnout
  //       m1,m2 flags  - flagy materialu
  //       mat_text     - cislo textury v materialu (pro prevod z flagu)
  void generator_set(int unit)
  {
    if(m2flag&texgen_matrix_texture_flagy2[mat_text]) {
      set_matrix_texture(text_unit,&stage_ttext);
    } else {
      ret_matrix_texture(text_unit);
    }
    
    if(m1flag&texgen_sphere_flagy[mat_text]) {
      gl_env_on(text_unit);
      texgen_map_indicie[text_unit] = ERROR;
    } else if(m1flag&texgen_eye_lin_flagy[mat_text]) {
      gl_linear_eye_2d_on(text_unit,stage_koord_s,stage_koord_t);
      texgen_map_indicie[text_unit] = ERROR;
    } else {
      gl_off(text_unit);
      texgen_map_indicie[text_unit] = mat_text;
    }
  }

  void st_ttext_matrix(GLMATRIX *p_mat)
  {
    if(p_mat) {
      memcpy(&stage_ttext,p_mat,sizeof(*p_mat));
    }
  }

  void st_texgen(float *p_s, float *p_t, float *p_r, float *p_q)
  {  
    if(p_s) {
      stage_koord_s[0] = p_s[0];
      stage_koord_s[1] = p_s[1];
      stage_koord_s[2] = p_s[2];
      stage_koord_s[3] = p_s[3];
    }

    if(p_t) {
      stage_koord_t[0] = p_t[0];
      stage_koord_t[1] = p_t[1];
      stage_koord_t[2] = p_t[2];
      stage_koord_t[3] = p_t[3];
    }

    if(p_r) {
      stage_koord_r[0] = p_r[0];
      stage_koord_r[1] = p_r[1];
      stage_koord_r[2] = p_r[2];
      stage_koord_r[3] = p_r[3];
    }

    if(p_q) {
      stage_koord_q[0] = p_q[0];
      stage_koord_q[1] = p_q[1];
      stage_koord_q[2] = p_q[2];
      stage_koord_q[3] = p_q[3];
    }
  }
*/  

public:

  material_text_coordinates(void)
  {
    text_matrix_state = FALSE;
    text_matrix.init();
  }

} MATERIAL_TEXT_COORDINATES;

#endif //  __MATERIAL_TEXTURE_H__
