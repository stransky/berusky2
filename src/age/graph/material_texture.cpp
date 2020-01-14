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

//TODO -> jak rozdelit ram/video ram textury?

/*
 * Bitmap surfaces for texture
 */
material_text_bitmap_params::material_text_bitmap_params(void)
{
  pixmap[0] = '\0';
  pixmap_alpha[0] = '\0';
}

bool material_text_bitmap_params::load(const char *p_file)
{
  // Loads images from texture config file
  if(tail_compare(p_file, TEXTURE_BITMAP_PARAMS_FILE)) {
    FFILE f = ini_open(p_file);
    ini_read_string(f, "surface", pixmap, MAX_FILENAME, "");
    ini_read_string(f, "surface_alpha", pixmap_alpha, MAX_FILENAME, "");    
    f.close();
    return(pixmap[0] != '\0');
  } 
  else {
    assert(strlen(p_file) < MAX_FILENAME);
    strncpy(pixmap,p_file,MAX_FILENAME);
    pixmap_alpha[0] = '\0'; // No alpha file    
    return(TRUE);  
  }
}

bool material_text_bitmap::bitmap_destroy(void)
{
  surf.clear();
  return(FALSE);
}

bool material_text_bitmap::bitmap_create(const char *p_dir, const char *p_file, const char *p_name)
{
  /*
   * If there's a surface there, destroy it
   */
  bitmap_destroy();
  
  /*
   * Create a new surface
   */
  char file[MAX_FILENAME];
  const char *p_disk_file;
  
  p_disk_file = p_dir ? return_path(p_dir, p_file, file, MAX_FILENAME) : p_file;
  
  // Load surface and texture class from disk
  MATERIAL_TEXT_BITMAP_PARAMS params;
  if(!params.load(p_disk_file))
    return(FALSE);

  if(!p_name) {
    char tmp[MAX_FILENAME];
    strcpy(tmp,p_file);
    p_name = tail_name_get(p_file);    
  }
  filename.set(p_name);
  
  if(!surf.load(params.pixmap))
    return(FALSE);
  if(params.pixmap_alpha[0] && !surf.load_alpha(params.pixmap_alpha))
    return(FALSE);
  
  return(TRUE);
}

bool material_text_bitmap::bitmap_create(tpos dx, tpos dy)
{   
  // Check if the surface is already here
  if(bitmap_loaded()) {
    tpos sdx, sdy;
    bitmap_size_get(&sdx, &sdy);
    if(sdx == dx && sdy == dy)
      return(TRUE);
    
    bitmap_destroy();
  }
  
  // Create a new surface
  surf.create(dx,dy);  
  return(TRUE);
}

bool material_text_bitmap::bitmap_create(SURFACE *p_surf)
{
  surf.create(p_surf);  
  return(TRUE);
}

material_text_bitmap::material_text_bitmap(void)
{
  
}

material_text_bitmap::~material_text_bitmap(void)
{
  bitmap_destroy();
}

int default_text_config::details[TEXT_CLASSES_NUM] = 
{ 
  TEXT_DETAIL_BASE,
  TEXT_DETAIL_BASE,
  TEXT_DETAIL_BASE,
  TEXT_DETAIL_BASE,
  TEXT_DETAIL_BASE,
  TEXT_DETAIL_BASE,
  TEXT_DETAIL_BASE,
  TEXT_DETAIL_BASE 
};

int default_text_config::max_x[TEXT_CLASSES_NUM] = 
{ 
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT 
};

int default_text_config::max_y[TEXT_CLASSES_NUM] = 
{ 
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT,
  TEXT_MAX_DEFAULT 
};

/*
  GLenum        type;          // texture type (1D/2D/3D/4D/5D/6D)
  GLint         format;        // texture format
  int           compression;   // texture compression
  TEXT_BPP      bpp;           // bit deph
  TEXT_FILTER   filter;        // texture filter
  TEXT_WRAP     wrap;          // wrap params
  int           anis;          // Anisotropy filter
  float         anis_level;    // Anisotropy filter level
  float         sharpness;     // texture sharpness
  tflag         flag1;         // texture flag 1
  tflag         flag2;         // texture flag 2
*/

MATERIAL_TEXT_GL_PARAMS default_text_config::params[TEXT_CLASSES_NUM] =
{
  { // CLASS DEFAULT
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  },
  { // CLASS1
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  },
  { // CLASS2
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  },
  { // CLASS3
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  },
  { // CLASS4
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  },
  { // CLASS5
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  },
  { // CLASS6
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  },
  { // CLASS7
    GL_TEXTURE_2D,
    FORMAT_RGB,
    FALSE,
    TEXT_DEFAULT,
    FILTER_LINEAR,
    TEXT_REPEAT,
    0,
    0.0f,
    0.0f,
    0,
    0
  }
};


/* - Orded for each array -
  FORMAT_RGB        = 0,
  FORMAT_RGBA1      = 1,
  FORMAT_RGBA4      = 2,
  FORMAT_LUM        = 3,
  FORMAT_LUM_ALPHA1 = 4,
  FORMAT_LUM_ALPHA4 = 5,
  FORMAT_ALPHA      = 6,
  FORMAT_LIGHTMAP   = 7
*/

int default_text_config::format1d[TEXT_BPP_NUM][TEXT_FORMATS_NUM] =
{
  { // TEXT_DEFAULT
    GL_RGB5,
    GL_RGB5_A1,
    GL_RGBA4,
    GL_LUMINANCE8,
    GL_LUMINANCE6_ALPHA2,
    GL_LUMINANCE4_ALPHA4,
    GL_ALPHA8,
    GL_RGB5
  },
  {  // TEXT_16BPP
    GL_RGB5,
    GL_RGB5_A1,
    GL_RGBA4,
    GL_LUMINANCE8,
    GL_LUMINANCE6_ALPHA2,
    GL_LUMINANCE4_ALPHA4,
    GL_ALPHA8,
    GL_RGB5
  },  
  { // TEXT_32BPP
    GL_RGB8,
    GL_RGBA8,
    GL_RGBA8,
    GL_LUMINANCE8,
    GL_LUMINANCE8_ALPHA8,
    GL_LUMINANCE8_ALPHA8,
    GL_ALPHA8,
    GL_RGB5
  },
  { // TEXT_DTX
    GL_RGB5,
    GL_RGB5_A1,
    GL_RGBA4,
    GL_LUMINANCE8,
    GL_LUMINANCE6_ALPHA2,
    GL_LUMINANCE4_ALPHA4,
    GL_ALPHA8,
    GL_RGB5
  }
};

int default_text_config::format2d[TEXT_BPP_NUM][TEXT_FORMATS_NUM] =
{
  { // TEXT_DEFAULT
    GL_RGB5,
    GL_RGB5_A1,
    GL_RGBA4,
    GL_LUMINANCE8,
    GL_LUMINANCE6_ALPHA2,
    GL_LUMINANCE4_ALPHA4,
    GL_ALPHA8,
    GL_RGB5
  },
  {  // TEXT_16BPP
    GL_RGB5,
    GL_RGB5_A1,
    GL_RGBA4,
    GL_LUMINANCE8,
    GL_LUMINANCE6_ALPHA2,
    GL_LUMINANCE4_ALPHA4,
    GL_ALPHA8,
    GL_RGB5
  },  
  { // TEXT_32BPP
    GL_RGB8,
    GL_RGBA8,
    GL_RGBA8,
    GL_LUMINANCE8,
    GL_LUMINANCE8_ALPHA8,
    GL_LUMINANCE8_ALPHA8,
    GL_ALPHA8,
    GL_RGB5
  },
  {
  GL_COMPRESSED_RGB_ARB,
  GL_COMPRESSED_RGBA_ARB,
  GL_COMPRESSED_RGBA_ARB,
  GL_COMPRESSED_LUMINANCE_ARB,
  GL_COMPRESSED_LUMINANCE_ALPHA_ARB,
  GL_COMPRESSED_LUMINANCE_ALPHA_ARB,
  GL_COMPRESSED_ALPHA_ARB,
  GL_RGB5
  }    
};

// Load texture configuration
void default_text_config::load(char *p_file)
{
  char item_name[200];
  int  i;

  FFILE f = ini_open(p_file);
  
  for(i = 0; i < TEXT_CLASSES_NUM; i++) {
    sprintf(item_name,"detail_class_%d",i);
    details[i] = ini_read_int(f, item_name, TEXT_DETAIL_BASE);
    sprintf(item_name,"max_x_class_%d",i);
    max_x[i] = ini_read_int(f, item_name, TEXT_MAX_DEFAULT);
    sprintf(item_name,"max_y_class_%d",i);
    max_y[i] = ini_read_int(f, item_name, TEXT_MAX_DEFAULT);  
  }   
  
  // Shrink texture max sizes  
  int text_max;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE,&text_max);
  for(i = 0; i < TEXT_CLASSES_NUM; i++) {
    if(text_max < max_x[i])
      max_x[i] = text_max;
    if(text_max < max_y[i])
      max_y[i] = text_max;
  }

  f.close();
}

int default_text_config::format_get(int type, int bpp, int format_index)
{
  if(type == GL_TEXTURE_2D) {
    return(format2d[bpp][format_index]);
  } else {
    return(format1d[bpp][format_index]);
  }
}

MATERIAL_TEXT_GL_PARAMS * default_text_config::params_get(TEXT_CLASS texture_class)
{
  assert(texture_class >= 0 && texture_class < TEXT_CLASSES_NUM);
  return(params+texture_class);
}

static DEFAULT_TEXT_CONFIG dt_config;

DEFAULT_TEXT_CONFIG * default_text_config_get(void)
{
  return(&dt_config);
}


// ****************************************************************************
// material_texture_params class - configuration for every texture / texture class
// ****************************************************************************

// name -> format
static int texture_format_translate(char *p_name)
{
  int i;
  static char format_string[][50] = {
       "FORMAT_RGB",
       "FORMAT_RGBA1",
       "FORMAT_RGBA4",
       "FORMAT_LUM",
       "FORMAT_LUM_ALPHA1",
       "FORMAT_LUM_ALPHA4",
       "FORMAT_ALPHA",
       "FORMAT_LIGHTMAP"};
  
  for(i = 0; i < (int)(sizeof(format_string)/sizeof(format_string[0])); i++) {
    if(!strcasecmp(format_string[i],(const char *)p_name)) {
      return(i);      
    }
  }  
  return(0);
}

// name -> format
static bool texture_type_translate(char *p_name, GLenum *p_type)
{
  int i;
  static char type_string[][50] = {
       "GL_TEXTURE_2D",
       "GL_TEXTURE_1D"};

  for(i = 0; i < (int)(sizeof(type_string)/sizeof(type_string[0])); i++) {
    if(!strcasecmp(type_string[i],(const char *)p_name)) {
      *p_type = i;
      return(TRUE);
    }
  }  
  return(FALSE);
}

// Create the class a load data from given file
bool material_text_gl_params::load(char *p_file)
{
  char tmp[200];
  
  FFILE f = ini_open(p_file,FALSE);
  if(!f)
    return(FALSE);
    
  ini_read_string(f, "type", tmp, 200, "GL_TEXTURE_2D");
  texture_type_translate(tmp, &type);
  ini_read_string(f, "format", tmp, 200, "FORMAT_RGB");
  format = (TEXT_FORMAT)texture_format_translate(tmp);

  compression = ini_read_bool(f, "compression", FALSE);
  bpp = (TEXT_BPP)ini_read_int(f, "bpp", TEXT_16BPP);
      
  anis = ini_read_bool(f, "anisotrophy", FALSE);
  anis_level = ini_read_float(f, "anisotrophy_level", 1.0f);

  sharpness = ini_read_float(f, "sharpness", 1.0f);
    
  if(compression && gl_ext::extlist_text_compresion) {
    bpp = TEXT_DTX;
  }

  if(anis && gl_ext::extlist_text_filter_anisotropic) {
    float max_anis,as,akt_anis;
    int   i,imax;
    
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anis);
    
    akt_anis = anis_level;
    if(akt_anis > 5)
      akt_anis = 5;
    
    imax = (int)akt_anis-1;
    for(as = 2, i = 0; i < imax; i++, as *= 2);

    if(as > max_anis)
      as = max_anis;

    anis_level = as/max_anis;
  } else {
    anis = FALSE;
  }

  f.close();
  
  return(TRUE);
}

/*
bool material_texture_params_load(MATERIAL_TEXT_GL_PARAMS *p_params, char *p_file)
{
  assert(p_params);
  if(file_exists(NULL,p_file)) {
    if(p_params->load(p_file))
      return(TRUE);
  }
  return(FALSE);
}
MATERIAL_TEXT_GL_PARAMS * material_texture_params_get(TEXT_CLASS texture_class)
{
  assert(texture_class < CLASS_CUSTOM);
  DEFAULT_TEXTURE_CONFIG *p_conf = default_text_config_get();
  return(p_conf->params+texture_class);
}
*/

material_text_gl::material_text_gl(void)
{  
  tclass = CLASS_DEFAULT; // texture class
  gl_text_name = 0;       // OpenGL texture name
  p_params = NULL;        // Pointer to texture params
}

material_text_gl::~material_text_gl(void)
{
  
}

material_text::material_text(class scene *p_scene_ref)
: object_list(p_scene_ref)
{
}

material_text::~material_text(void)
{
}

void material_text_gl::gl_text_params_create(void)
{
  if(!p_params) {
    p_params = new MATERIAL_TEXT_GL_PARAMS;
    *p_params = *(default_text_config_get()->params_get(tclass));
  }  
}

void material_text_gl::gl_text_params_destroy(void)
{
  if(p_params) {
    delete p_params;
    p_params = NULL;
  }    
}

bool material_text_gl::gl_text_params_load(char *p_file)
{  
  char tmp[MAX_FILENAME];

  assert(strlen(p_file) < MAX_FILENAME);
  strncpy(tmp,p_file,MAX_FILENAME);
  tail_change(tmp,TEXTURE_GL_PARAM_FILE);
  
  gl_text_params_create();
  assert(p_params);  
  return(p_params->load(tmp));
}

bool material_text_gl::gl_text_create(void)
{ 
  if(!gl_active)
    return(FALSE);
  
  if(!bitmap_loaded())
    return(FALSE);

  // If the texture already exists, destroy it
  gl_text_destroy();
  
  GLenum type = gl_text_type_get();
  
  glGenTextures(1, &gl_text_name);  
  glBindTexture(type, gl_text_name);

  MATERIAL_TEXT_GL_PARAMS *p_par = gl_text_params_get();
  SURFACE *p_surf = bitmap_get();
  
  if(p_par->filter == FILTER_MIP_NEAR ||
     p_par->filter == FILTER_MIP_LINEAR ||
     p_par->filter == FILTER_MIP_TRILINEAR) {

    if(type == GL_TEXTURE_2D) {
      int x,y,i = 0;
      
      p_surf->size_get(&x,&y);
      
      while(x > 0 || y > 0) {
        p_surf->scale(x,y);
        glTexImage2D(GL_TEXTURE_2D, i, gl_text_gl_format_get(), p_surf->width_get(),
                     p_surf->height_get(), 0, GL_RGBA,GL_UNSIGNED_BYTE, 
                     p_surf->pixels_get());
        x /= 2;
        y /= 2;
        i++;
      }
    } else if(type == GL_TEXTURE_1D) {
      gluBuild1DMipmaps(GL_TEXTURE_1D, gl_text_gl_format_get(),
                        p_surf->width_get(), GL_RGBA, 
                        GL_UNSIGNED_BYTE, p_surf->pixels_get());
    }

    if(p_par->filter == FILTER_MIP_TRILINEAR) {
      glTexParameteri(type,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(type,GL_TEXTURE_MAG_FILTER,GL_LINEAR);      
    } else {
      glTexParameteri(type,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
      glTexParameteri(type,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }

  } else {
    if(type == GL_TEXTURE_2D) {
      glTexImage2D(GL_TEXTURE_2D, 0, gl_text_gl_format_get(), p_surf->width_get(), p_surf->height_get(), 0, GL_RGBA, GL_UNSIGNED_BYTE, p_surf->pixels_get());
    } else if(type == GL_TEXTURE_1D) {
      glTexImage1D(GL_TEXTURE_1D, 0, gl_text_gl_format_get(), p_surf->width_get(), 0, GL_RGBA, GL_UNSIGNED_BYTE, p_surf->pixels_get());
    }
    
    if(p_par->filter == FILTER_LINEAR) {
      glTexParameteri(type,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(type,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    } else {
      glTexParameteri(type,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexParameteri(type,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }
  }

  if(p_par->anis) {
    glTexParameterfv(type, GL_TEXTURE_MAX_ANISOTROPY_EXT, &(p_par->anis_level));
  }

  int wrap = GL_REPEAT;
  
  switch(p_par->wrap) {
  case TEXT_CLAMP:
    if(gl_ext::extlist_text_edge_clamp) {
      wrap = GL_CLAMP_TO_EDGE;
    } else {
      wrap = GL_CLAMP;
    }
    break;
  case TEXT_REPEAT:
    wrap = GL_REPEAT;
    break;
  }
  glTexParameteri(type,GL_TEXTURE_WRAP_S,wrap);
  glTexParameteri(type,GL_TEXTURE_WRAP_T,wrap);
  
  return(TRUE);
}

bool material_text_gl::gl_text_destroy(void)
{
  if(gl_text_name && glIsTexture(gl_text_name)) {
    glDeleteTextures(1,&gl_text_name);
    gl_text_name = 0;
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool material_text::load(const char *p_dir, const char *p_file, const char *p_name)
{
  if(bitmap_create(p_dir, p_file, p_name)) {
    return(gl_text_create());    
  }
  return(FALSE);
}

bool material_text::load(SURFACE *p_surf)
{
  if(bitmap_create(p_surf)) {
    return(gl_text_create());    
  }
  return(FALSE);
}

bool material_text::save(const char *p_dir, const char *p_file)
{
  SURFACE *p_bitmap = bitmap_get();
  if(!p_bitmap)
    return(FALSE);
    
  char tmp[MAX_FILENAME];
  return(p_bitmap->save(return_path(p_dir,p_file,tmp,MAX_FILENAME)));
}

bool material_text::load(void)
{
  if(!bitmap_loaded())
    return(FALSE);
  
  if(!bitmap_changed())
    return(FALSE);
  
  return(gl_text_create());
}

bool material_text::loaded(void)
{
  return(gl_text_loaded());
}

bool material_text::destroy(void)
{
  bitmap_destroy();
  gl_text_destroy();
  return(TRUE);
}

void material_text::texture_remove(void)
{
  SCENE *p_scene = scene_get();
  assert(p_scene != NULL);
  p_scene->texture_remove(this);
}  


// Zmeni rozliseni textury podle pozadovanych detailu
/*
void material_text::bmp_apply_detail(void)
{
  int x = default_text_max_x;
  int y = default_text_max_y;
  int scale;
  int dx,dy;
  
  if(surf.cti_x() <= x)
    x = surf.cti_x();
  else
    scale = TRUE;
  
  if(surf.cti_y() <= y)
    y = surf.cti_y();
  else
    scale = TRUE;
  
  for(dx = 2; dx < default_text_max_x && x > dx; dx <<= 1);
  for(dy = 2; dy < default_text_max_y && y > dy; dy <<= 1);
  
  if(x != dx || y != dy) {
    x = dx; y = dy;
    scale = TRUE;
  }
  
  if(default_text_detail[trida] > 0) {
    scale = TRUE;
    x >>= default_text_detail[trida];
    y >>= default_text_detail[trida];
  }
  
  if(scale) {
    surf.scale(x,y);    
  }  
}
*/
/*
// Defaultni konstruktor - nahodi default konfig
material_text::material_text(int x, int y, TEXT_FILTR filtr, int format_)
{
  jmeno[0]   = 0;
  load       = TRUE;
  typ        = GL_TEXTURE_2D;
  format     = format_;  
  trida      = default_text_class;
  flag       = 0;
  flag2      = 0;
  text_filtr = filtr;
  text_wrap  = TEXT_CLAMP;
  alfa_stage = K_CHYBA;
  p_next     = NULL;
  p_prev     = NULL;  

  glGenTextures(1,&text);
  glBindTexture(typ,text);
    
  SURFACEA *p_surf = new SURFACEA(x,y);
  glTexImage2D(typ, 0, format, surf.cti_x(), surf.cti_y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, surf.cti_data());
  delete p_surf;
  
  if(filtr == FILTR_LINEAR) {
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  } else {    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  }

  if(extlist_text_edge_clamp) {
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);      
  } else {
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);      
  }
}

static void pridej_alfu(char *p_file, char *p_alfa_file)
{
  char pom[200] = "_a";
  strcat(pom,cti_koncovku(p_file));
  zamen_koncovku(p_alfa_file,pom);
}

// load ok
void material_text::config_nahraj(APAK_HANDLE *pAHandle, const char *p_file)
{
  char  jmeno[200];
  char  jmeno_alfa[200];
  char *p_znak;
  int   format = -1;
  int   alfa;
  KFILE *f;

  strcpy(jmeno,p_file);
  pridej_alfu(jmeno,jmeno_alfa);

  zamen_koncovku(jmeno,KONCOVKA_TEXT_INFO);
  
  alfa = kefile(pAHandle, jmeno_alfa);
  alfa_stage = K_CHYBA;

  if(alfa) {
    flag2 |= TEXT2_ALPHA;
  } else {
    flag2 &= ~TEXT2_ALPHA;    
  }
 
  if((f = kopen(pAHandle,jmeno,"r"))) {
    while(kgets(jmeno,200,f)) {
      fgets_korekce(jmeno);
      if(jmeno[0] == ';')
        continue;
      // Bump-mapping
      if(strstr(jmeno,"nobump")) {
        flag2 &= ~TEXT2_BUMP;
        continue;
      }        
      // culling
      if(strstr(jmeno,"no_cull")) {
        flag2 |= TEXT2_NO_CULL;
        continue;
      }        
      if((p_znak = strchr(jmeno,'='))) {
        *p_znak++ = 0;
        if(strstr(jmeno,"format")) {
          if(!preloz_texture_format(&format, p_znak)) {
            kprintfl(TRUE,"Bad texture format %s",p_znak);
          }
          continue;
        }        
        // Repeat/clamp mody
        if(strstr(jmeno,"wrap_x")) {
          text_wrap = atoi(p_znak) ? TEXT_CLAMP : TEXT_REPEAT;
          continue;
        }  
        // Repeat/clamp mody
        if(strstr(jmeno,"wrap_y")) {
          text_wrap = atoi(p_znak) ? TEXT_CLAMP : TEXT_REPEAT;
          continue;
        }
        if(strstr(jmeno,"wrap")) {
          text_wrap = atoi(p_znak) ? TEXT_CLAMP : TEXT_REPEAT;
          continue;
        }
        // Mip-mapping
        if(strstr(jmeno,"mip_map")) {
          text_filtr = atoi(p_znak) ? FILTR_MIP_TRILINEAR : FILTR_LINEAR;
          continue;        
        }

        // Typ 1D/2D
        if(strstr(jmeno,"typ")) {
          typ = (atoi(p_znak) == 1) ? GL_TEXTURE_1D : GL_TEXTURE_2D;
          continue;
        }
        // alfa-stage
        if(strstr(jmeno,"alfa_stage")) {
          alfa_stage = atoi(p_znak);
          continue;
        }
      }
    }
    kclose(f);
  } 

  if(typ == GL_TEXTURE_1D) {
    format = (format != K_CHYBA) ? format1d[format] : (alfa) ? format1d[FORMAT_RGBA1] : format1d[FORMAT_RGB];
  } else {
    format = (format != K_CHYBA) ? format2d[format] : (alfa) ? format2d[FORMAT_RGBA1] : format2d[FORMAT_RGB];
  }  
}

void material_text::config_default(char *p_file, char *p_koncovka)
{  
  strcpy(jmeno,p_file);
  zamen_koncovku(jmeno,p_koncovka);

  typ = GL_TEXTURE_2D;
  format = format2d[FORMAT_RGB];
  
  text_filtr = default_text_filter;
  text_wrap = default_text_wrap;
  flag2 |= TEXT2_BUMP;
  alfa_stage = K_CHYBA;
}

void material_text::config_lightmap(void)
{
  typ = GL_TEXTURE_2D;
  format = format2d[FORMAT_LIGHTMAP];
  text_filtr = FILTR_LINEAR;
  text_wrap = TEXT_CLAMP;
}

int material_text::nahraj_texturu_z_bmp_lightmap(char *p_file, KFILE *f, int save)
{
  AUX_RGBImageRec *p_tmp = NULL;
      
  config_lightmap();

  if(p_file) {
    if(!(p_tmp = aux_nahraj(NULL,p_file)))
      return(FALSE);
  } else {
    if(!(p_tmp = aux_nahraj_file(f)))
      return(FALSE);
  }

  load = TRUE;
  p_surf = new SURFACEA(p_tmp,FALSE);
  aux_zrus(&p_tmp);
  
  upload_to_vram();
  
  if(!save) {
    delete p_surf;
    p_surf = NULL;
  }

  return(load);
}

int material_text::nahraj_texturu_z_dds(APAK_HANDLE *pHandle, char *p_file, int save)
{
  void            *p_vysl;
  int              ret;
    
  if((ret = txt_dds_to_bmp(pHandle,p_file,&p_vysl))) {
    trida = default_text_class;
    text_filtr = default_text_filter;    
    text_wrap = TEXT_REPEAT;
    if(ret == 3) {
      format = format2d[FORMAT_RGB];      
      p_surf = new SURFACEA((AUX_RGBImageRec *)p_vysl,FALSE);
      aux_zrus((AUX_RGBImageRec **)&p_vysl);
    } else {
      format = format2d[FORMAT_RGBA4];  
      p_surf = (SURFACEA *)p_vysl;
    }
    upload_to_vram();
    load = TRUE;
    if(!save) {
      delete p_surf;
      p_surf = NULL;
    }
  }
  return(load);
}

int material_text::nahraj_texturu_z_tga(APAK_HANDLE *pHandle, char *p_file)
{
  int              ret;
  
  if((ret = tgaLoad(pHandle, p_file, FALSE, GL_DUDV_ATI))) {
    load = TRUE;
    text = ret;
    p_surf = NULL;
    trida = default_text_class;
    text_filtr = default_text_filter;
    text_wrap = TEXT_REPEAT;
    
    strcpy(jmeno,p_file);

    glTexParameterf(typ, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(typ, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(typ, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(typ, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  return(load);
}
*/
/*
  Importni interface soubory pro textury DDS
  vraci pocet plan -> 3/4
*/
/*
int txt_dds_to_bmp(APAK_HANDLE *pHandle, char *p_file, void **p_return)
{
  unsigned char   *p_data;
  unsigned char   *p_rd;
  AUX_RGBImageRec *p_rec;
  SURFACEA         *p_surf;
  byte            *p_src,r;
  int              width;
  int              height;
  int              planes;
  int              lTotalWidth; 
  int              rowBytes;
  int              x,y,dl;

  if((file_load = kopen(pHandle,p_file,"rb")) == NULL) {
    return(FALSE);
  }
  p_data = nvDXTdecompress(width, height, planes, lTotalWidth, rowBytes);

  assert(width == lTotalWidth);
  
  // prehodi r<->b
  for(y = 0; y < height; y ++ ) {    
    for(x = 0; x < width; x++) {
      p_src = p_data+(y*width*planes)+x*planes;
      r = p_src[2];
      p_src[2] = p_src[0];
      p_src[0] = r;                  
    }
  }
  // prehodi radky
  dl = sizeof(byte)*width*planes;
  p_rd = (byte *)malloc(dl*height);
  for(y = 0; y < height; y++ ) {
    memcpy(p_rd+(height-y-1)*dl,p_data+y*dl,dl);
  }
  memcpy(p_data,p_rd,dl*height);

  if(planes == 3) {
    p_rec = (AUX_RGBImageRec *)malloc(sizeof(*p_rec));
    assert(p_rec);      
    p_rec->sizeX = width;
    p_rec->sizeY = height;
    p_rec->data = p_data;
    *p_return = p_rec;
  } else {
    p_surf = new SURFACEA(width,height,(dword *)p_data);    
    *p_return = p_surf;
  }

  free(p_rd);
  kclose(file_load);
  return(planes);
}
        

void WriteDTXnFile(DWORD count, void *buffer)
{

}

void ReadDTXnFile(DWORD count, void *buffer)
{
  kread(buffer,sizeof(byte),count,file_load);
}
*/


/*
class text_list {

  EDIT_TEXT *p_first;

public:
  ~text_list(void);

  text_vloz(EDIT_TEXT *p_text);
  text_vloz_copy(EDIT_TEXT *p_text);

  text_smaz(EDIT_TEXT *p_text);

  text_najdi(char jmeno);
*/

material_text_list::material_text_list(class scene *p_scene_ref)
 : object_list_head(p_scene_ref)
{
  dir_reset();
}

material_text_list::~material_text_list(void)
{ 
  MATERIAL_TEXT *p_text = (MATERIAL_TEXT *)get_first();
  while(p_text) {
    MATERIAL_TEXT *p_tmp = p_text;
    p_text = (MATERIAL_TEXT *)p_text->object_list_next();
    delete p_tmp;
  }
}

void material_text_list::reload(void)
{    
  MATERIAL_TEXT *p_text = (MATERIAL_TEXT *)get_first();
  
  pprintf("material_text_list::reload()");
  
  while(p_text) {
    p_text->print();
    if(!p_text->bitmap_loaded()) {
      int dhandle = dir_next_new();  
      char *p_dir;
      while((p_dir = dir_next(&dhandle))) {
        pprintfnl("Loading %s from '%s'...",p_text->name_get(),p_dir);
        if(p_text->load(p_text->name_get(),p_dir)) {
          pprintf("loaded");
          break;
        }
      }
      if(!p_text->loaded()) {
        pprintf("failed");
      }
    }
    p_text = (MATERIAL_TEXT *)p_text->object_list_next();
  }
}

void material_text_list::dir_reset(void)
{
  texture_dir_last = 0;
  dir_add(".");
}

bool material_text_list::dir_add(const char *p_dir)
{
  if(texture_dir_last+1 < TEXT_DIRS) {
    strcpy(texture_dir[texture_dir_last],p_dir);
    texture_dir_last++;
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}

int material_text_list::dir_next_new(void)
{
  return(texture_dir_last-1);
}

char * material_text_list::dir_next(int *p_handle)
{
  return(*p_handle >= 0 ? texture_dir[(*p_handle)--] : NULL);
}

/*
int material_text_list::reload(int save)
{  
  int i;    

  for(i = 0; i < TEXT_DIRS; i++) {
    if(texture_dir[i][0]) {
      txt_config::txt_trida(texture_dir_class[i]);
      reload_textur_dir(save,NULL,texture_dir[i]);
    }
  }

  for(i = 0; i < TEXT_DIRS; i++) {
    if(texture_file[i][0]) {
      txt_config::txt_trida(texture_file_class[i]);
      reload_textur_file(texture_file[i],save);
    }
  }

  chyby();

  return(TRUE);
}
*/

int material_text_list::errors_print(void)
{  
  MATERIAL_TEXT *p_text = (MATERIAL_TEXT *)get_first();
  int n = 0;

  while(p_text) {
    if(!p_text->gl_text_loaded()) {
      pprintf("%s is not loaded...",p_text->name_get());
      n++;
    }
    p_text = (MATERIAL_TEXT *)p_text->object_list_next();
  }

  pprintf("%d textures unloaded.",n);
  return(n);
}

int material_text_list::status_print(void)
{  
  MATERIAL_TEXT *p_text = (MATERIAL_TEXT *)get_first();
  int n = 0;

  while(p_text) {    
    pprintf("%d texture %s, loaded %d...",n,p_text->name_get(),p_text->gl_text_loaded());
    n++;
    p_text = (MATERIAL_TEXT *)p_text->object_list_next();
  }

  pprintf("%d textures total",n);
  return(n);
}

int material_text_list::size_get(void)
{  
  int        xres,yres,vel = 0;
  int        r,g,b,a,l,in;
 
  MATERIAL_TEXT *p_text = (MATERIAL_TEXT *)get_first();
  while(p_text) {
    if(p_text->gl_text_loaded()) {
      p_text->gl_text_bind();
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_WIDTH,&xres);
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_HEIGHT,&yres);
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_RED_SIZE,&r);
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_GREEN_SIZE,&g);
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_BLUE_SIZE,&b);
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_ALPHA_SIZE,&a);
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_LUMINANCE_SIZE,&l);
      glGetTexLevelParameteriv(p_text->gl_text_type_get(),0,GL_TEXTURE_INTENSITY_SIZE,&in);
      vel += xres*yres*(r+b+g+a+l+in);
    }
    p_text = (MATERIAL_TEXT *)p_text->object_list_next();
  }
  return(vel >> 3);
}

int material_text_list::num_get(void)
{
  MATERIAL_TEXT *p_text = (MATERIAL_TEXT *)get_first();
  int n = 0;

  while(p_text) {
    n++;
    p_text = (MATERIAL_TEXT *)p_text->object_list_next();
  } 
  
  return(n);
}
