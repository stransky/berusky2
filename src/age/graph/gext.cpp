/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 **
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

#include <assert.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#define  GL_GLEXT_DECLARE_INTERFACE 1

// Constants and types
#include "defs.h"
#include "typ.h"
#include "typ_graph.h"
#include "floats.h"

// Extended types
#include "color_helpers.h"
#include "color.h"
#include "vector.h"
#include "quat.h"
#include "matrix.h"

// OpenGL includes
#include "opengl.h"
#include "gext.h"

// Disk and some minor utilities
#include "utils.h"
#include "ini.h"
#include "log.h"
#include "mem_alloc.h"

// -------------------------------------------------------
//   Extensions
// -------------------------------------------------------
bool gl_ext::extlist_blend_color;
bool gl_ext::extlist_blend_minmax;
bool gl_ext::extlist_fog_coord;
bool gl_ext::extlist_fence;
bool gl_ext::extlist_register_combiners;
bool gl_ext::extlist_text_compresion;
bool gl_ext::extlist_text_sharp;
bool gl_ext::extlist_text_edge_clamp;
bool gl_ext::extlist_vertex_array_range;
bool gl_ext::extlist_vertex_array;
bool gl_ext::extlist_indicie_array;
bool gl_ext::extlist_vertex_array_packed;
bool gl_ext::extlist_text_env_combine;
bool gl_ext::extlist_text_env_dot3;
bool gl_ext::extlist_text_filter_anisotropic;
bool gl_ext::extlist_clip_volume_hit;
bool gl_ext::extlist_ati_env_bump;
bool gl_ext::extlist_ati_env_bump_unit[6];
bool gl_ext::extlist_ati_vertex_array_object;
bool gl_ext::extlist_ati_element_array;
bool gl_ext::extlist_ati_pn_triangles;
bool gl_ext::extlist_arb_vertex_buffer;
bool gl_ext::extlist_ati_draw_buffers;

int gl_ext::ext_multitext_units = 0;

/*
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;
PFNGLMULTITEXCOORD1DARBPROC glMultiTexCoord1dARB = NULL;
PFNGLMULTITEXCOORD1DVARBPROC glMultiTexCoord1dvARB = NULL;
PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD1FVARBPROC glMultiTexCoord1fvARB = NULL;
PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1iARB = NULL;
PFNGLMULTITEXCOORD1IVARBPROC glMultiTexCoord1ivARB = NULL;
PFNGLMULTITEXCOORD1SARBPROC glMultiTexCoord1sARB = NULL;
PFNGLMULTITEXCOORD1SVARBPROC glMultiTexCoord1svARB = NULL;
PFNGLMULTITEXCOORD2DARBPROC glMultiTexCoord2dARB = NULL;
PFNGLMULTITEXCOORD2DVARBPROC glMultiTexCoord2dvARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB = NULL;
PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB = NULL;
PFNGLMULTITEXCOORD2IVARBPROC glMultiTexCoord2ivARB = NULL;
PFNGLMULTITEXCOORD2SARBPROC glMultiTexCoord2sARB = NULL;
PFNGLMULTITEXCOORD2SVARBPROC glMultiTexCoord2svARB = NULL;
PFNGLMULTITEXCOORD3DARBPROC glMultiTexCoord3dARB = NULL;
PFNGLMULTITEXCOORD3DVARBPROC glMultiTexCoord3dvARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB = NULL;
PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3iARB = NULL;
PFNGLMULTITEXCOORD3IVARBPROC glMultiTexCoord3ivARB = NULL;
PFNGLMULTITEXCOORD3SARBPROC glMultiTexCoord3sARB = NULL;
PFNGLMULTITEXCOORD3SVARBPROC glMultiTexCoord3svARB = NULL;
PFNGLMULTITEXCOORD4DARBPROC glMultiTexCoord4dARB = NULL;
PFNGLMULTITEXCOORD4DVARBPROC glMultiTexCoord4dvARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB = NULL;
PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4iARB = NULL;
PFNGLMULTITEXCOORD4IVARBPROC glMultiTexCoord4ivARB = NULL;
PFNGLMULTITEXCOORD4SARBPROC glMultiTexCoord4sARB = NULL;
PFNGLMULTITEXCOORD4SVARBPROC glMultiTexCoord4svARB = NULL;
*/

GLint gl_ext::ext_multitexture_text_index[] = 
{
  GL_TEXTURE0_ARB, 
  GL_TEXTURE1_ARB, 
  GL_TEXTURE2_ARB, 
  GL_TEXTURE3_ARB, 
  GL_TEXTURE4_ARB, 
  GL_TEXTURE5_ARB, 
  GL_TEXTURE6_ARB, 
  GL_TEXTURE7_ARB
};


/* 
 * Extensions
 */
bool gl_ext::gl_init_multitexture(void)
{
  int level = 0;

  if(is_extension("GL_ARB_multitexture")) {
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &ext_multitext_units);
    pprintf("ARB Texture units %d",ext_multitext_units);
    /*
    if(!(glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glActiveTextureARB")))
      return(level);
    if(!(glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glClientActiveTextureARB")))
      return(level);
    if(!(glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1dARB")))
      return(level);
    if(!(glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1dvARB")))
      return(level);
    if(!(glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1fARB")))
      return(level);
    if(!(glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1fvARB")))
      return(level);
    if(!(glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1iARB")))
      return(level);
    if(!(glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1ivARB")))
      return(level);
    if(!(glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1sARB")))
      return(level);
    if(!(glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord1svARB")))
      return(level);
  
    level = 1;
    if(!(glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2dARB")))
      return(level);
    if(!(glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2dvARB")))
      return(level);
    if(!(glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2fARB")))
      return(level);
    if(!(glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2fvARB")))
      return(level);
    if(!(glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2iARB")))
      return(level);
    if(!(glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2ivARB")))
      return(level);
    if(!(glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2sARB")))
      return(level);
    if(!(glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2svARB")))
      return(level);

    level = 2;
    if(!(glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3dARB")))
      return(level);
    if(!(glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3dvARB")))
      return(level);
    if(!(glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3fARB")))
      return(level);
    if(!(glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3fvARB")))
      return(level);
    if(!(glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3iARB")))
      return(level);
    if(!(glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3ivARB")))
      return(level);
    if(!(glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3sARB")))
      return(level);
    if(!(glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord3svARB")))
      return(level);

    level = 3;
    if(!(glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4dARB")))
      return(level);
    if(!(glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4dvARB")))
      return(level);
    if(!(glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4fARB")))
      return(level);
    if(!(glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4fvARB")))
      return(level);
    if(!(glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4iARB")))
      return(level);
    if(!(glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4ivARB")))
      return(level);
    if(!(glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4sARB")))
      return(level);
    if(!(glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4svARB")))
      return(level);
*/
    level = 4;      
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLBLENDCOLOREXTPROC glBlendColorEXT = NULL;

bool gl_ext::gl_init_blend_color(void)
{  
  if(is_extension("GL_EXT_blend_color")) {
    if(!(glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)SDL_GL_GetProcAddress("glBlendColorEXT")))
      return(FALSE);
    else
      return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT = NULL;

bool gl_ext::gl_init_blend_minmax(void)
{  
  if(is_extension("GL_EXT_blend_minmax")) {    
    if(!(glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)SDL_GL_GetProcAddress("glBlendEquationEXT")))
      return(FALSE);  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLFOGCOORDDEXTPROC glFogCoorddEXT = NULL;
PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT = NULL;
PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;
PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT = NULL;
PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT = NULL;

bool gl_ext::gl_init_fog_coord(void)
{  
  if(is_extension("GL_EXT_fog_coord")) {
  
    if(!(glFogCoorddEXT = (PFNGLFOGCOORDDEXTPROC)SDL_GL_GetProcAddress("glFogCoorddEXT")))
      return(FALSE);
    if(!(glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC)SDL_GL_GetProcAddress("glFogCoorddvEXT")))
      return(FALSE);
    if(!(glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)SDL_GL_GetProcAddress("glFogCoordfEXT")))
      return(FALSE);
    if(!(glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC)SDL_GL_GetProcAddress("glFogCoordfvEXT")))
      return(FALSE);
    if(!(glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC)SDL_GL_GetProcAddress("glFogCoordPointerEXT")))
      return(FALSE);

    glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FRAGMENT_DEPTH_EXT);
    glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);  
  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLSECONDARYCOLOR3BEXTPROC glSecondaryColor3bEXT = NULL;
PFNGLSECONDARYCOLOR3BVEXTPROC glSecondaryColor3bvEXT = NULL;
PFNGLSECONDARYCOLOR3DEXTPROC glSecondaryColor3dEXT = NULL;
PFNGLSECONDARYCOLOR3DVEXTPROC glSecondaryColor3dvEXT = NULL;
PFNGLSECONDARYCOLOR3FEXTPROC glSecondaryColor3fEXT = NULL;
PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT = NULL;
PFNGLSECONDARYCOLOR3IEXTPROC glSecondaryColor3iEXT = NULL;
PFNGLSECONDARYCOLOR3IVEXTPROC glSecondaryColor3ivEXT = NULL;
PFNGLSECONDARYCOLOR3SEXTPROC glSecondaryColor3sEXT = NULL;
PFNGLSECONDARYCOLOR3SVEXTPROC glSecondaryColor3svEXT = NULL;
PFNGLSECONDARYCOLOR3UBEXTPROC glSecondaryColor3ubEXT = NULL;
PFNGLSECONDARYCOLOR3UBVEXTPROC glSecondaryColor3ubvEXT = NULL;
PFNGLSECONDARYCOLOR3UIEXTPROC glSecondaryColor3uiEXT = NULL;
PFNGLSECONDARYCOLOR3UIVEXTPROC glSecondaryColor3uivEXT = NULL;
PFNGLSECONDARYCOLOR3USEXTPROC glSecondaryColor3usEXT = NULL;
PFNGLSECONDARYCOLOR3USVEXTPROC glSecondaryColor3usvEXT = NULL;
PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT = NULL;

bool gl_ext::gl_init_secondary_color(void)
{  
  if(is_extension("GL_EXT_secondary_color")) {
    if(!(glSecondaryColor3bEXT = (PFNGLSECONDARYCOLOR3BEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3bEXT")))
      return(FALSE);
    if(!(glSecondaryColor3bvEXT = (PFNGLSECONDARYCOLOR3BVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3bvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3dEXT = (PFNGLSECONDARYCOLOR3DEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3dEXT")))
      return(FALSE);
    if(!(glSecondaryColor3dvEXT = (PFNGLSECONDARYCOLOR3DVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3dvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3fEXT = (PFNGLSECONDARYCOLOR3FEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3fEXT")))
      return(FALSE);
    if(!(glSecondaryColor3fvEXT = (PFNGLSECONDARYCOLOR3FVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3fvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3iEXT = (PFNGLSECONDARYCOLOR3IEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3iEXT")))
      return(FALSE);
    if(!(glSecondaryColor3ivEXT = (PFNGLSECONDARYCOLOR3IVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3ivEXT")))
      return(FALSE);
    if(!(glSecondaryColor3sEXT = (PFNGLSECONDARYCOLOR3SEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3sEXT")))
      return(FALSE);
    if(!(glSecondaryColor3svEXT = (PFNGLSECONDARYCOLOR3SVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3svEXT")))
      return(FALSE);
    if(!(glSecondaryColor3ubEXT = (PFNGLSECONDARYCOLOR3UBEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3ubEXT")))
      return(FALSE);
    if(!(glSecondaryColor3ubvEXT = (PFNGLSECONDARYCOLOR3UBVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3ubvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3uiEXT = (PFNGLSECONDARYCOLOR3UIEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3uiEXT")))
      return(FALSE);
    if(!(glSecondaryColor3uivEXT = (PFNGLSECONDARYCOLOR3UIVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3uivEXT")))
      return(FALSE);
    if(!(glSecondaryColor3usEXT = (PFNGLSECONDARYCOLOR3USEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3usEXT")))
      return(FALSE);
    if(!(glSecondaryColor3usvEXT = (PFNGLSECONDARYCOLOR3USVEXTPROC)SDL_GL_GetProcAddress("glSecondaryColor3usvEXT")))
      return(FALSE);
    if(!(glSecondaryColorPointerEXT = (PFNGLSECONDARYCOLORPOINTEREXTPROC)SDL_GL_GetProcAddress("glSecondaryColorPointerEXT")))
      return(FALSE);  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLGENFENCESNVPROC glGenFencesNV = NULL;
PFNGLDELETEFENCESNVPROC glDeleteFencesNV = NULL;
PFNGLSETFENCENVPROC glSetFenceNV = NULL;
PFNGLTESTFENCENVPROC glTestFenceNV = NULL;
PFNGLFINISHFENCENVPROC glFinishFenceNV = NULL;
PFNGLISFENCENVPROC glIsFenceNV = NULL;
PFNGLGETFENCEIVNVPROC glGetFenceivNV = NULL;

bool gl_ext::gl_init_fence(void)
{  
  if(is_extension("GL_NV_fence")) {
    if(!(glGenFencesNV = (PFNGLGENFENCESNVPROC)SDL_GL_GetProcAddress("glGenFencesNV")))
      return(FALSE);
    if(!(glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)SDL_GL_GetProcAddress("glDeleteFencesNV")))
      return(FALSE);
    if(!(glSetFenceNV = (PFNGLSETFENCENVPROC)SDL_GL_GetProcAddress("glSetFenceNV")))
      return(FALSE);
    if(!(glTestFenceNV = (PFNGLTESTFENCENVPROC)SDL_GL_GetProcAddress("glTestFenceNV")))
      return(FALSE);
    if(!(glFinishFenceNV = (PFNGLFINISHFENCENVPROC)SDL_GL_GetProcAddress("glFinishFenceNV")))
      return(FALSE);
    if(!(glIsFenceNV = (PFNGLISFENCENVPROC)SDL_GL_GetProcAddress("glIsFenceNV")))
      return(FALSE);
    if(!(glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)SDL_GL_GetProcAddress("glGetFenceivNV")))
      return(FALSE);  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_texture_compression(void)
{  
  if(is_extension("GL_ARB_texture_compression")) {  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_texture_lod(void)
{  
  if(is_extension("GL_EXT_texture_lod_bias")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_clip_volume_hit(void)
{  
  if(is_extension("GL_EXT_clip_volume_hint")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_text_edge_clamp(void)
{  
  if(is_extension("GL_EXT_texture_edge_clamp")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_text_env_combine(void)
{  
  if(is_extension("GL_EXT_texture_env_combine")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_text_env_dot3(void)
{  
  if(is_extension("GL_EXT_texture_env_dot3")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLTEXBUMPPARAMETERIVATIPROC glTexBumpParameterivATI = NULL;
PFNGLTEXBUMPPARAMETERFVATIPROC glTexBumpParameterfvATI = NULL;
PFNGLGETTEXBUMPPARAMETERIVATIPROC glGetTexBumpParameterivATI = NULL;
PFNGLGETTEXBUMPPARAMETERFVATIPROC glGetTexBumpParameterfvATI = NULL;

bool gl_ext::gl_init_ati_env_bump(void)
{  
/*
  int *p_units,units;
  int i,u;  

  if(is_extension("GL_ATI_envmap_bumpmap")) {
    glTexBumpParameterivATI = (PFNGLTEXBUMPPARAMETERIVATIPROC) SDL_GL_GetProcAddress("glTexBumpParameterivATI");
    glTexBumpParameterfvATI = (PFNGLTEXBUMPPARAMETERFVATIPROC) SDL_GL_GetProcAddress("glTexBumpParameterfvATI");
    glGetTexBumpParameterivATI = (PFNGLGETTEXBUMPPARAMETERIVATIPROC) SDL_GL_GetProcAddress("glGetTexBumpParameterivATI");
    glGetTexBumpParameterfvATI = (PFNGLGETTEXBUMPPARAMETERFVATIPROC) SDL_GL_GetProcAddress("glGetTexBumpParameterfvATI");

    glGetTexBumpParameterivATI(GL_BUMP_NUM_TEX_UNITS_ATI,&units);
    //kprintf(1,"ATI Bump-units %d",units);
    //p_units = _alloca(sizeof(p_units[0])*units);
    //glGetTexBumpParameterivATI(GL_BUMP_TEX_UNITS_ATI,p_units);

    for(i = 0; i < units; i++) {
      u = p_units[i]-GL_TEXTURE0_ARB;
      extlist_ati_env_bump_unit[u] = TRUE;
      kprintf(1,"Bump unit %d",u);
    }
    return(TRUE);
  } else {
    return(FALSE);
  }
*/
  return(FALSE);
}

PFNGLNEWOBJECTBUFFERATIPROC glNewObjectBufferATI= NULL;
PFNGLISOBJECTBUFFERATIPROC glIsObjectBufferATI= NULL;
PFNGLUPDATEOBJECTBUFFERATIPROC glUpdateObjectBufferATI= NULL;
PFNGLGETOBJECTBUFFERFVATIPROC glGetObjectBufferfvATI= NULL;
PFNGLGETOBJECTBUFFERIVATIPROC glGetObjectBufferivATI= NULL;
PFNGLFREEOBJECTBUFFERATIPROC glFreeObjectBufferATI= NULL;
PFNGLARRAYOBJECTATIPROC glArrayObjectATI= NULL;
PFNGLGETARRAYOBJECTFVATIPROC glGetArrayObjectfvATI= NULL;
PFNGLGETARRAYOBJECTIVATIPROC glGetArrayObjectivATI= NULL;
PFNGLVARIANTARRAYOBJECTATIPROC glVariantArrayObjectATI= NULL;
PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI= NULL;
PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI= NULL;

bool gl_ext::gl_init_vertex_array_object(void)
{  
  if(is_extension("GL_ATI_vertex_array_object")) {
    glNewObjectBufferATI = (PFNGLNEWOBJECTBUFFERATIPROC) SDL_GL_GetProcAddress("glNewObjectBufferATI");
    glIsObjectBufferATI = (PFNGLISOBJECTBUFFERATIPROC) SDL_GL_GetProcAddress("glIsObjectBufferATI");
    glUpdateObjectBufferATI = (PFNGLUPDATEOBJECTBUFFERATIPROC) SDL_GL_GetProcAddress("glUpdateObjectBufferATI");
    glGetObjectBufferfvATI = (PFNGLGETOBJECTBUFFERFVATIPROC) SDL_GL_GetProcAddress("glGetObjectBufferfvATI");
    glGetObjectBufferivATI = (PFNGLGETOBJECTBUFFERIVATIPROC) SDL_GL_GetProcAddress("glGetObjectBufferivATI");
    glFreeObjectBufferATI = (PFNGLFREEOBJECTBUFFERATIPROC) SDL_GL_GetProcAddress("glFreeObjectBufferATI");
    glArrayObjectATI = (PFNGLARRAYOBJECTATIPROC) SDL_GL_GetProcAddress("glArrayObjectATI");
    glGetArrayObjectfvATI = (PFNGLGETARRAYOBJECTFVATIPROC) SDL_GL_GetProcAddress("glGetArrayObjectfvATI");
    glGetArrayObjectivATI = (PFNGLGETARRAYOBJECTIVATIPROC) SDL_GL_GetProcAddress("glGetArrayObjectivATI");
    glVariantArrayObjectATI = (PFNGLVARIANTARRAYOBJECTATIPROC) SDL_GL_GetProcAddress("glVariantArrayObjectATI");
    glGetVariantArrayObjectfvATI = (PFNGLGETVARIANTARRAYOBJECTFVATIPROC) SDL_GL_GetProcAddress("glGetVariantArrayObjectfvATI");
    glGetVariantArrayObjectivATI = (PFNGLGETVARIANTARRAYOBJECTIVATIPROC) SDL_GL_GetProcAddress("glGetVariantArrayObjectivATI");  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLELEMENTPOINTERATIPROC glElementPointerATI = NULL;
PFNGLDRAWELEMENTARRAYATIPROC glDrawElementArrayATI = NULL;
PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI = NULL;

bool gl_ext::gl_init_element_array(void)
{  
  if(is_extension("GL_ATI_element_array")) {
    glElementPointerATI = (PFNGLELEMENTPOINTERATIPROC) SDL_GL_GetProcAddress("glElementPointerATI");
    glDrawElementArrayATI = (PFNGLDRAWELEMENTARRAYATIPROC) SDL_GL_GetProcAddress("glDrawElementArrayATI");
    glDrawRangeElementArrayATI = (PFNGLDRAWRANGEELEMENTARRAYATIPROC) SDL_GL_GetProcAddress("glDrawRangeElementArrayATI");
 
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV = NULL;
PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV = NULL;
PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV = NULL;
PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV = NULL;
PFNGLCOMBINERINPUTNVPROC glCombinerInputNV = NULL;
PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV = NULL;
PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV = NULL;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV = NULL;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV = NULL;

bool gl_ext::gl_init_register_combiners(void)
{  
  if(is_extension("GL_NV_register_combiners")) {    
    if(!(glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)SDL_GL_GetProcAddress("glCombinerParameterfvNV")))
      return(FALSE);
    if(!(glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC)SDL_GL_GetProcAddress("glCombinerParameterfNV")))
      return(FALSE);
    if(!(glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)SDL_GL_GetProcAddress("glCombinerParameterivNV")))
      return(FALSE);
    if(!(glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC)SDL_GL_GetProcAddress("glCombinerParameteriNV")))
      return(FALSE);
    if(!(glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC)SDL_GL_GetProcAddress("glCombinerInputNV")))
      return(FALSE);
    if(!(glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC)SDL_GL_GetProcAddress("glCombinerOutputNV")))
      return(FALSE);
    if(!(glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC)SDL_GL_GetProcAddress("glFinalCombinerInputNV")))
      return(FALSE);
    if(!(glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)SDL_GL_GetProcAddress("glGetCombinerInputParameterfvNV")))
      return(FALSE);
    if(!(glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)SDL_GL_GetProcAddress("glGetCombinerInputParameterivNV")))
      return(FALSE);
    if(!(glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)SDL_GL_GetProcAddress("glGetCombinerOutputParameterfvNV")))
      return(FALSE);
    if(!(glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)SDL_GL_GetProcAddress("glGetCombinerOutputParameterivNV")))
      return(FALSE);
    if(!(glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)SDL_GL_GetProcAddress("glGetFinalCombinerInputParameterfvNV")))
      return(FALSE);
    if(!(glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)SDL_GL_GetProcAddress("glGetFinalCombinerInputParameterivNV")))
      return(FALSE);  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV = NULL;
PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV = NULL;

bool gl_ext::gl_init_vertex_array_range(void)
{
  if(is_extension("GL_NV_vertex_array_range")) {
    if(!(glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)SDL_GL_GetProcAddress("glFlushVertexArrayRangeNV")))
      return(FALSE);
    if(!(glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)SDL_GL_GetProcAddress("glVertexArrayRangeNV")))
      return(FALSE);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLLOCKARRAYSEXTPROC glLockArraysEXT = NULL;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT = NULL;

bool gl_ext::gl_init_vertex_array_packed(void)
{  
  if(is_extension("GL_EXT_compiled_vertex_array")) {    
    if(!(glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)SDL_GL_GetProcAddress("glLockArraysEXT")))
      return(FALSE);
    if(!(glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)SDL_GL_GetProcAddress("glUnlockArraysEXT")))
      return(FALSE); 
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI = NULL;
PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI = NULL; 

bool gl_ext::gl_init_ati_pn_triangles(void)
{  
  if(is_extension("GL_ATI_pn_triangles")) {
    glPNTrianglesiATI = (PFNGLPNTRIANGLESIATIPROC) SDL_GL_GetProcAddress("glPNTrianglesiATI");
    glPNTrianglesfATI = (PFNGLPNTRIANGLESFATIPROC) SDL_GL_GetProcAddress("glPNTrianglesfATI"); 
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_text_filter_anisotropic(void)
{  
  if(is_extension("GL_EXT_texture_filter_anisotropic")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

bool gl_ext::gl_init_ati_draw_buffers(void)
{  
  if(is_extension("GL_ATI_draw_buffers")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLISBUFFERARBPROC glIsBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;
PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB = NULL;
PFNGLMAPBUFFERARBPROC glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;
PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB = NULL;

bool gl_ext::gl_init_arb_vertex_buffer(void)
{  
  if(is_extension("GL_ARB_vertex_buffer_object")) {
    if(!(glBindBufferARB = (PFNGLBINDBUFFERARBPROC)SDL_GL_GetProcAddress("glBindBufferARB")))
      return(FALSE);
    if(!(glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)SDL_GL_GetProcAddress("glDeleteBuffersARB")))
      return(FALSE);
    if(!(glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)SDL_GL_GetProcAddress("glGenBuffersARB")))
      return(FALSE);
    if(!(glIsBufferARB = (PFNGLISBUFFERARBPROC)SDL_GL_GetProcAddress("glIsBufferARB")))
      return(FALSE);
    if(!(glBufferDataARB = (PFNGLBUFFERDATAARBPROC)SDL_GL_GetProcAddress("glBufferDataARB")))
      return(FALSE);
    if(!(glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)SDL_GL_GetProcAddress("glBufferSubDataARB")))
      return(FALSE);
    if(!(glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)SDL_GL_GetProcAddress("glGetBufferSubDataARB")))
      return(FALSE);
    if(!(glMapBufferARB = (PFNGLMAPBUFFERARBPROC)SDL_GL_GetProcAddress("glMapBufferARB")))
      return(FALSE);
    if(!(glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)SDL_GL_GetProcAddress("glUnmapBufferARB")))
      return(FALSE);
    if(!(glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)SDL_GL_GetProcAddress("glGetBufferParameterivARB")))
      return(FALSE);
    if(!(glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)SDL_GL_GetProcAddress("glGetBufferPointervARB")))
      return(FALSE);  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

void gl_ext::init_extension(void)
{ 
  // Load all extensions
  load_extension();
  
  // Mandatory extensions
  gl_init_secondary_color();  
  gl_init_multitexture();
  
  // The optional ones
  extlist_blend_color = gl_init_blend_color();
  extlist_blend_minmax = gl_init_blend_minmax();
  extlist_fog_coord = gl_init_fog_coord();
  extlist_fence = gl_init_fence();
  extlist_register_combiners = gl_init_register_combiners();
  extlist_text_compresion = gl_init_texture_compression();
  extlist_text_sharp = gl_init_texture_lod();
  extlist_text_edge_clamp = gl_init_text_edge_clamp();
  extlist_text_env_combine = gl_init_text_env_combine();  
  extlist_text_filter_anisotropic = gl_init_text_filter_anisotropic();
  extlist_clip_volume_hit = gl_init_clip_volume_hit();
  extlist_vertex_array_range = gl_init_vertex_array_range();
  extlist_vertex_array_packed = gl_init_vertex_array_packed();
  extlist_ati_pn_triangles = gl_init_ati_pn_triangles();
  extlist_ati_draw_buffers = gl_init_ati_draw_buffers();  
/*
  if(vertex_arrays) {
    extlist_arb_vertex_buffer = gl_init_arb_vertex_buffer();
    if(!extlist_arb_vertex_buffer) {
      extlist_ati_vertex_array_object = gl_init_vertex_array_object();
      extlist_ati_element_array = gl_init_element_array();
    }
  }
  
  // pouze pokud je vic jak 2 texturovaci jednotky
  if(bump_mapping && opengl_text::gl_multitext_units > 2) {
    extlist_text_env_dot3 = gl_init_text_env_dot3();
    extlist_ati_env_bump = gl_init_ati_env_bump();
    bump_mapping = extlist_text_env_dot3|extlist_ati_env_bump;
  }
*/
}

/* 
 * Load Extensions
 */
void gl_ext::load_extension(void)
{
  const char *p_et;
  const char *p_start;
  char  *_p_ext;
  char  *p_pom;
  int   i,j;

  p_vend = (char *)glGetString(GL_VENDOR);
  p_rend = (char *)glGetString(GL_RENDERER);
  p_ver  = (char *)glGetString(GL_VERSION);
  p_et   = (char *)glGetString(GL_EXTENSIONS);


  pprintf("OpenGL Vendor: %s",p_vend);
  pprintf("OpenGL Renderer: %s",p_rend);
  pprintf("OpenGL Version: %s",p_ver);

  _p_ext = (char *)mmalloc(sizeof(char)*(strlen(p_et)+1));
  strcpy(_p_ext,p_et);  

  j = strlen(_p_ext);
  extnum = 0;
  for(i = 0; i < j; i++) {
    if(_p_ext[i] == ' ')
      extnum++;
  }

  p_ext = (char **)mmalloc(sizeof(char *)*extnum);

  //pprintf("\nAvailabile extensions:");

  i = 0;
  p_start = _p_ext;
  while((p_pom = (char *)strchr(p_start,' '))) {
    *p_pom = 0;
    assert(i < extnum);
    p_ext[i] = (char *)mmalloc(sizeof(char)*(strlen(p_start)+5));
    strcpy(p_ext[i],p_start);
    //pprintf(p_ext[i]);
    i++;
    p_start = p_pom+1;
  }

  free(_p_ext);  
}

/* Release extensions
*/
void gl_ext::free_extension(void)
{
  int i;
  for(i = 0; i < extnum; i++) {
    if(p_ext[i]) {
      free(p_ext[i]);
      p_ext[i] = NULL;
    }
  }
  free(p_ext);   
}

/* Test the extension
*/
bool gl_ext::is_extension(const char *p_text)
{
  int i;
  for(i = 0; i < extnum; i++) {
    if(!strcmp(p_text,p_ext[i])) {
      return(TRUE);
    }
  }
  return(FALSE);
}

GL_EXT glext;
