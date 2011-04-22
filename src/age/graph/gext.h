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

#ifndef  __GEXT_H__
#define  __GEXT_H__

typedef class gl_ext
{
  /*
   * Extensions
   */
public:
  
  static bool   extlist_blend_color;
  static bool   extlist_blend_minmax;
  static bool   extlist_fog_coord;
  static bool   extlist_fence;
  static bool   extlist_register_combiners;
  static bool   extlist_text_compresion;
  static bool   extlist_text_sharp;
  static bool   extlist_text_edge_clamp;
  static bool   extlist_text_env_combine;
  static bool   extlist_text_env_dot3;
  static bool   extlist_text_filter_anisotropic;
  static bool   extlist_vertex_array_range;
  static bool   extlist_vertex_array;
  static bool   extlist_indicie_array;
  static bool   extlist_vertex_array_packed;
  static bool   extlist_clip_volume_hit;
  static bool   extlist_ati_env_bump;
  static bool   extlist_ati_env_bump_unit[6];
  static bool   extlist_ati_vertex_array_object;
  static bool   extlist_ati_element_array;
  static bool   extlist_ati_pn_triangles;
  static bool   extlist_arb_vertex_buffer;
  static bool   extlist_ati_draw_buffers;

private:

  bool gl_init_multitexture(void);
  bool gl_init_blend_color(void);
  bool gl_init_blend_minmax(void);
  bool gl_init_fog_coord(void);
  bool gl_init_secondary_color(void);
  bool gl_init_fence(void);
  bool gl_init_texture_compression(void);
  bool gl_init_texture_lod(void);
  bool gl_init_clip_volume_hit(void);
  bool gl_init_text_edge_clamp(void);
  bool gl_init_text_env_combine(void);
  bool gl_init_text_env_dot3(void);
  bool gl_init_ati_env_bump(void);
  bool gl_init_vertex_array_object(void);
  bool gl_init_element_array(void);
  bool gl_init_register_combiners(void);
  bool gl_init_vertex_array_range(void);
  bool gl_init_vertex_array_packed(void);
  bool gl_init_ati_pn_triangles(void);
  bool gl_init_text_filter_anisotropic(void);
  bool gl_init_ati_draw_buffers(void);
  bool gl_init_arb_vertex_buffer(void);

public:
  
  static int   ext_multitext_units;
  static GLint ext_multitexture_text_index[];

public:
  
  void init_extension(void);

private:
   
  char  *p_vend;
  char  *p_rend;
  char  *p_ver;

  char **p_ext;
  int    extnum;

public:
  
  void load_extension(void);
  void free_extension(void);
  bool is_extension(const char *p_text);

} GL_EXT;

extern GL_EXT glext;

extern PFNGLBLENDCOLOREXTPROC glBlendColorEXT;

extern PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT;

extern PFNGLFOGCOORDDEXTPROC glFogCoorddEXT;
extern PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT;
extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;
extern PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT;
extern PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT;

extern PFNGLSECONDARYCOLOR3BEXTPROC glSecondaryColor3bEXT;
extern PFNGLSECONDARYCOLOR3BVEXTPROC glSecondaryColor3bvEXT;
extern PFNGLSECONDARYCOLOR3DEXTPROC glSecondaryColor3dEXT;
extern PFNGLSECONDARYCOLOR3DVEXTPROC glSecondaryColor3dvEXT;
extern PFNGLSECONDARYCOLOR3FEXTPROC glSecondaryColor3fEXT;
extern PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT;
extern PFNGLSECONDARYCOLOR3IEXTPROC glSecondaryColor3iEXT;
extern PFNGLSECONDARYCOLOR3IVEXTPROC glSecondaryColor3ivEXT;
extern PFNGLSECONDARYCOLOR3SEXTPROC glSecondaryColor3sEXT;
extern PFNGLSECONDARYCOLOR3SVEXTPROC glSecondaryColor3svEXT;
extern PFNGLSECONDARYCOLOR3UBEXTPROC glSecondaryColor3ubEXT;
extern PFNGLSECONDARYCOLOR3UBVEXTPROC glSecondaryColor3ubvEXT;
extern PFNGLSECONDARYCOLOR3UIEXTPROC glSecondaryColor3uiEXT;
extern PFNGLSECONDARYCOLOR3UIVEXTPROC glSecondaryColor3uivEXT;
extern PFNGLSECONDARYCOLOR3USEXTPROC glSecondaryColor3usEXT;
extern PFNGLSECONDARYCOLOR3USVEXTPROC glSecondaryColor3usvEXT;
extern PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT;

extern PFNGLGENFENCESNVPROC glGenFencesNV;
extern PFNGLDELETEFENCESNVPROC glDeleteFencesNV;
extern PFNGLSETFENCENVPROC glSetFenceNV;
extern PFNGLTESTFENCENVPROC glTestFenceNV;
extern PFNGLFINISHFENCENVPROC glFinishFenceNV;
extern PFNGLISFENCENVPROC glIsFenceNV;
extern PFNGLGETFENCEIVNVPROC glGetFenceivNV;

extern PFNGLTEXBUMPPARAMETERIVATIPROC glTexBumpParameterivATI;
extern PFNGLTEXBUMPPARAMETERFVATIPROC glTexBumpParameterfvATI;
extern PFNGLGETTEXBUMPPARAMETERIVATIPROC glGetTexBumpParameterivATI;
extern PFNGLGETTEXBUMPPARAMETERFVATIPROC glGetTexBumpParameterfvATI;

extern PFNGLNEWOBJECTBUFFERATIPROC glNewObjectBufferATI;
extern PFNGLISOBJECTBUFFERATIPROC glIsObjectBufferATI;
extern PFNGLUPDATEOBJECTBUFFERATIPROC glUpdateObjectBufferATI;
extern PFNGLGETOBJECTBUFFERFVATIPROC glGetObjectBufferfvATI;
extern PFNGLGETOBJECTBUFFERIVATIPROC glGetObjectBufferivATI;
extern PFNGLFREEOBJECTBUFFERATIPROC glFreeObjectBufferATI;
extern PFNGLARRAYOBJECTATIPROC glArrayObjectATI;
extern PFNGLGETARRAYOBJECTFVATIPROC glGetArrayObjectfvATI;
extern PFNGLGETARRAYOBJECTIVATIPROC glGetArrayObjectivATI;
extern PFNGLVARIANTARRAYOBJECTATIPROC glVariantArrayObjectATI;
extern PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI;
extern PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI;

extern PFNGLELEMENTPOINTERATIPROC glElementPointerATI;
extern PFNGLDRAWELEMENTARRAYATIPROC glDrawElementArrayATI;
extern PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI;

extern PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;
extern PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV;
extern PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
extern PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV;
extern PFNGLCOMBINERINPUTNVPROC glCombinerInputNV;
extern PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV;
extern PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV;
extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV;
extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;

extern PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
extern PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;

extern PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

extern PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
extern PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI; 

extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLISBUFFERARBPROC glIsBufferARB;
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;

#endif // __GEXT_H__
