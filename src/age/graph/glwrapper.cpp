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
  A wrapper for some OpenGL functions
*/

// Constants and types
#include "age.h"

bool    gl_active = FALSE;

int     gl_diffuse::diffuse_state = 0;
int     gl_specular::specular_state = 0;

int     gl_fog::fog_state = 0;
int     gl_fog::fog_state_temporary = 0;

int     gl_light::lighting_state = 0;

int     gl_z_buffer::depth_test_state = 0;
int     gl_z_buffer::zmask_state = 0;

void gl_z_buffer::on(bool force)
{    
  if(!depth_test_state || force) {
    depth_test_state = TRUE;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  }
}
  
void gl_z_buffer::off(bool force)
{
  if(depth_test_state || force) {
    depth_test_state = FALSE;
    glDisable(GL_DEPTH_TEST);
  }
}

int     gl_cull_mod::cull_state = 0;
int     gl_cull_mod::cull_side = 0;

int     gl_texture::multitext_units = 0;
int     gl_texture::text_akt = ERROR;
int     gl_texture::textures_2d[MAX_GL_TEXT_UNITS] = {0,0,0,0,0,0};
int     gl_texture::textures_1d[MAX_GL_TEXT_UNITS] = {0,0,0,0,0,0};
GLuint  gl_texture::last_text_2d[MAX_GL_TEXT_UNITS] = {-1,-1,-1,-1,-1,-1};
GLuint  gl_texture::last_text_1d[MAX_GL_TEXT_UNITS] = {-1,-1,-1,-1,-1,-1};
GLenum  gl_texture::arb_translation_table[MAX_GL_TEXT_UNITS] = 
{
  GL_TEXTURE0_ARB,
  GL_TEXTURE1_ARB, 
  GL_TEXTURE2_ARB, 
  GL_TEXTURE3_ARB, 
  GL_TEXTURE4_ARB,
  GL_TEXTURE5_ARB
};

/*
int         gl_texture_coordinates::texgen_s[MAX_GL_TEXT_UNITS];
int         gl_texture_coordinates::texgen_t[MAX_GL_TEXT_UNITS];
int         gl_texture_coordinates::texgen_r[MAX_GL_TEXT_UNITS];
int         gl_texture_coordinates::texgen_q[MAX_GL_TEXT_UNITS];
GLMATRIX    gl_texture_coordinates::texgen[MAX_GL_TEXT_UNITS];
*/

int   gl_texture_operators::op_last[MAX_GL_TEXT_UNITS] = { -1, -1, -1, -1, -1, -1};
void (*gl_texture_operators::op_func[MAX_TEXTURE_OPERATORS])(void) = 
{
  op_modulation_comb,
  op_modulation_comb,
  op_modulation_2x_comb,
  op_add,
  op_add_comb,
  op_decal,
  op_blend,
  op_replace,
  op_dot3_bump
};

void gl_texture_operators::op_modulation(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

void gl_texture_operators::op_modulation_comb(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 
}

void gl_texture_operators::op_modulation_2x_comb(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 

  glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 2.0f);
}

void gl_texture_operators::op_add(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_ADD);
}

void gl_texture_operators::op_add_comb(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 
}

void gl_texture_operators::op_decal(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void gl_texture_operators::op_blend(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND);
}

void gl_texture_operators::op_replace(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void gl_texture_operators::op_dot3_bump(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_DOT3_RGB_EXT);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
}

GLMATRIX gl_texture_coordinates::text_matrix[MAX_GL_TEXT_UNITS];
bool     gl_texture_coordinates::text_matrix_state[MAX_GL_TEXT_UNITS];

// format -> name
/*
static char * texture_format_get(int format, char *p_name)
{
  static int formats_num[] = {GL_RGB5,
                 GL_RGB5_A1,
                 GL_RGBA4,
                 GL_LUMINANCE8,
                 GL_LUMINANCE6_ALPHA2,
                 GL_LUMINANCE4_ALPHA4,
                 GL_ALPHA8,
                 GL_RGB5,
                 GL_RGB8,
                 GL_RGBA8,
                 GL_RGBA8,
                 GL_LUMINANCE8,
                 GL_LUMINANCE8_ALPHA8,
                 GL_LUMINANCE8_ALPHA8,
                 GL_ALPHA8,
                 GL_RGB8,
                 GL_COMPRESSED_RGB_ARB,
                 GL_COMPRESSED_RGBA_ARB,
                 GL_COMPRESSED_RGBA_ARB,
                 GL_COMPRESSED_LUMINANCE_ARB,
                 GL_COMPRESSED_LUMINANCE_ALPHA_ARB,
                 GL_COMPRESSED_LUMINANCE_ALPHA_ARB,
                 GL_COMPRESSED_ALPHA_ARB,
                 GL_RGB5};

  static char formats_names[][50] = {"GL_RGB5",
                      "GL_RGB5_A1",
                      "GL_RGBA4",
                      "GL_LUMINANCE8",
                      "GL_LUMINANCE6_ALPHA2",
                      "GL_LUMINANCE4_ALPHA4",
                      "GL_ALPHA8",
                      "GL_RGB5",
                      "GL_RGB8",
                      "GL_RGBA8",
                      "GL_RGBA8",
                      "GL_LUMINANCE8",
                      "GL_LUMINANCE8_ALPHA8",
                      "GL_LUMINANCE8_ALPHA8",
                      "GL_ALPHA8",
                      "GL_RGB8",
                      "GL_COMPRESSED_RGB_ARB",
                      "GL_COMPRESSED_RGBA_ARB",
                      "GL_COMPRESSED_RGBA_ARB",
                      "GL_COMPRESSED_LUMINANCE_ARB",
                      "GL_COMPRESSED_LUMINANCE_ALPHA_ARB",
                      "GL_COMPRESSED_LUMINANCE_ALPHA_ARB",
                      "GL_COMPRESSED_ALPHA_ARB",
                      "GL_RGB5"};

  assert((sizeof(formats_num)/sizeof(formats_num[0])) == (sizeof(formats_names)/sizeof(formats_names[0])));

  #define FORMAT_NUM ((int)(sizeof(formats_num)/sizeof(formats_num[0])))
                    
  int i;
  for(i = 0; i < FORMAT_NUM; i++) {
    if(formats_num[i] == format) {
      strcpy(p_name,(const char *)formats_names[i]);
      return(p_name);
    }
  }
  return(NULL);
}
*/
