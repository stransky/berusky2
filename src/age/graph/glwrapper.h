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

#ifndef __GLWRAPPER_H__
#define __GLWRAPPER_H__

// ****************************************************************************
//   Global gl state flag
// ****************************************************************************
extern bool gl_active;

// ****************************************************************************
//   Culling settings
// ****************************************************************************
typedef class gl_cull_mod {

public:

  static int cull_state;   // cull mode
  static int cull_side;    // which side
  
public:
  
  static void on(bool force = FALSE)
  {
    if(!cull_state || force) {
      glEnable(GL_CULL_FACE);
      cull_state = TRUE;
    }
  }
  
  static void off(bool force = FALSE)
  {
    if(cull_state || force) {
      glDisable(GL_CULL_FACE);
      cull_state = FALSE;
    }
  }
  
  static void set(int state, bool force = FALSE)
  {
    if(state) {
      on();
    } else {
      off();
    }
  }

  static void set_all(int mod, bool force = FALSE)
  {
    if(!mod) {
      off(force);
    } else if(mod == 1) {
      on(force);
      back(force);
    } else if(mod == 2) {
      on(force);
      front(force);   
    }
  }

  static void back(bool force = FALSE)
  {
    if(cull_side != 1 || force) {
      cull_side = 1;
      glFrontFace(GL_CCW);
    }
  }
  
  static void front(bool force = FALSE)
  {
    if(cull_side != 2 || force) {
      cull_side = 2;
      glFrontFace(GL_CW);
    }
  }
    
  static void change(bool force = FALSE)
  {
    if(cull_side) {
      if(cull_side == 1) {
        front(force);
      } else {
        back(force);
      }
    }
  }

} GL_CULL_MOD_STATE;

// ****************************************************************************
//   Specular color settings
// ****************************************************************************
typedef class gl_specular {
  
  static int  specular_state;    // spekularni svetla
  
public:
  
  static void on(bool force = FALSE)
  {
    if(!specular_state || force) {
      specular_state = TRUE;
      glEnable(GL_COLOR_SUM_EXT);
    }
  }
  
  static void off(bool force = FALSE)
  {
    if(specular_state || force) {
      specular_state = FALSE;
      glDisable(GL_COLOR_SUM_EXT);
    }
  }
  
  static void set(float r, float g, float b)
  {
    glSecondaryColor3fEXT(r,g,b);
  }

} GL_SPECULAR_STATE;

// ****************************************************************************
//   Diffuse color settings
// ****************************************************************************
typedef class gl_diffuse {

  static int diffuse_state;

public:

  static void off(bool force = FALSE)
  {
    diffuse_state = FALSE;
  }
  static void on(bool force = FALSE)
  {
    diffuse_state = TRUE;
  }

public:
  
  static void set(float r, float g, float b, float a)
  {
    glColor4f(r,g,b,a);
  }  
  
  static void set(byte r, byte g, byte b, byte a)
  {
    glColor4ub(r,g,b,a);
  }  

  static void set(RGBB *p_color)
  {
    glColor3ub(p_color->r,p_color->g,p_color->b);
  }
  
  static void set(RGBAF *p_color)
  {
    glColor4f(p_color->r,p_color->g,p_color->b,p_color->a);
  }
  
} GL_DIFFUSE_STATE;

// ****************************************************************************
//   Fog settings
// ****************************************************************************
typedef class gl_fog {

  static int fog_state;
  static int fog_state_temporary;

public:

  void disable(bool force = FALSE)
  {
    if(fog_state || force) {
      glDisable(GL_FOG);
      fog_state = FALSE;
    }
  }
  
  void enable(bool force = FALSE)
  {
    if(!fog_state || force) {
      glEnable(GL_FOG);
      fog_state = TRUE;
    }
  }
  /*
  void disable_temporary(bool force = FALSE)
  {
    if(fog_state && fog_state_temporary) {
      fog_state_temporary = FALSE;
      glDisable(GL_FOG);    
    }
  }
  
  void enable_temporary(bool force = FALSE)
  {
    if(fog_state && !fog_state_temporary) {
      fog_state_temporary = TRUE;
      glEnable(GL_FOG);  
    }
  }
  */
} GL_FOG_STATE;

// ****************************************************************************
//   Light settings
// ****************************************************************************
typedef class gl_light {

  static int lighting_state;    // gl svetla

public:  
  
  static void ambient(float *p_ambient)
  {
    glLightfv( GL_LIGHT0, GL_AMBIENT, p_ambient);
  }
  
  static void ambient(dword barva)
  {
    float amb[4] = {0,0,0,1};
    rgb_float(barva,amb);
    glLightfv( GL_LIGHT0, GL_AMBIENT, amb);
  }
  
  static void set(int state, bool force = FALSE)
  {
    if(state) {
      if(!lighting_state || force) {
        lighting_state = TRUE;
        glEnable(GL_LIGHTING);
      }
    }
    else {
      if(lighting_state || force) {
        lighting_state = FALSE;
        glDisable(GL_LIGHTING);
      }
    }
  }
  
  static void on(bool force = FALSE)
  {
    set(TRUE,force);
  }
  
  static void off(bool force = FALSE)
  {
    set(FALSE,force);
  }
  
  static void smooth(void)
  {
    glShadeModel(GL_SMOOTH);  
  }
  
  static void flat(void)
  {
    glShadeModel(GL_FLAT);
  }  

} GL_LIGHT_STATE;

// ****************************************************************************
//   Nastaveni Z-bufferu
// ****************************************************************************
typedef class gl_z_buffer {

  static int depth_test_state;
  static int zmask_state;

public:

  static void on(bool force = FALSE);
  static void off(bool force = FALSE);

  static void test(int state, bool force = FALSE)
  {
    if(state) {
      on(force);
    } else {
      off(force);
    }  
  }
  
  static void mask_on(bool force = FALSE)
  {
    if(!zmask_state || force) {
      zmask_state = TRUE;
      glDepthMask(TRUE);
    }
  }
  
  static void mask_off(bool force = FALSE)
  {
    if(zmask_state || force) {
      zmask_state = FALSE;
      glDepthMask(FALSE);
    }  
  }
  
  static void mask_set(int state, bool force = FALSE)
  {
    if(state) {
      mask_on(force);
    } else {
      mask_off(force);
    }  
  }
  
  static void mask_set_spots(int state)
  {
    glDepthMask((GLboolean)state);
  }  

} GL_Z_BUFFER_STATE;

/*
 *************************************************************************** 
    Assigned textures in OpenGL texture units
 ***************************************************************************
*/
typedef class gl_texture {
  
public:

  static int      multitext_units;                  // Max textures
  static int      text_akt;                         // cislo aktivni textury
  
  static int      textures_2d[MAX_GL_TEXT_UNITS];   // textury on/off
  static int      textures_1d[MAX_GL_TEXT_UNITS];   // textury on/off
  
  static GLint   last_text_2d[MAX_GL_TEXT_UNITS]; // posledni textura
  static GLint   last_text_1d[MAX_GL_TEXT_UNITS]; // posledni textura

  static GLenum   arb_translation_table[MAX_GL_TEXT_UNITS];// = {GL_TEXTURE0_ARB,GL_TEXTURE1_ARB, GL_TEXTURE2_ARB, GL_TEXTURE3_ARB, GL_TEXTURE4_ARB, GL_TEXTURE5_ARB};
  
public:
  // Reset nastaveni
  static void reset(void)
  {
    for(int i = 0; i < MAX_GL_TEXT_UNITS; i++) {
      textures_1d[i] = FALSE;
      textures_2d[i] = FALSE;
      last_text_1d[i] = FALSE;
      last_text_2d[i] = FALSE;
    }            
    
    for(int i = 1; i < MAX_GL_TEXT_UNITS; i++) {
      glActiveTextureARB(arb_translation_table[i]);
      glDisable(GL_TEXTURE_1D);
      glDisable(GL_TEXTURE_2D);
    }
    glActiveTextureARB(arb_translation_table[0]);    
    
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);  
    
    text_akt = ERROR;
  }

  // Nastavi texturu
  static void set(GLint text, GLenum typ)
  {
    if(typ == GL_TEXTURE_2D) {
      if(textures_2d[text_akt] && last_text_2d[text_akt] != text) {
        last_text_2d[text_akt] = text;
        glBindTexture(GL_TEXTURE_2D, text);
      }
    } else {
      if(textures_1d[text_akt] && last_text_1d[text_akt] != text) {
        last_text_1d[text_akt] = text;
        glBindTexture(GL_TEXTURE_1D, text);
      }
    }
  }
  
  // Zapne texturing
  static void on(GLenum typ)
  {
    if(typ == GL_TEXTURE_2D) {
      if(!textures_2d[text_akt]) {
        textures_2d[text_akt] = TRUE;
        glEnable(GL_TEXTURE_2D);
      }
      if(textures_1d[text_akt]) {
        textures_1d[text_akt] = FALSE;
        glDisable(GL_TEXTURE_1D);
      }
    } else {
      if(!textures_1d[text_akt]) {
        textures_1d[text_akt] = TRUE;
        glEnable(GL_TEXTURE_1D);
      }
      if(textures_2d[text_akt]) {
        textures_2d[text_akt] = FALSE;
        glDisable(GL_TEXTURE_2D);
      }
    }
  }
  
  // Turn off textures
  static void off(GLenum typ)
  {
    if(typ == GL_TEXTURE_2D) {
      if(textures_2d[text_akt]) {
        textures_2d[text_akt] = FALSE;
        glDisable(GL_TEXTURE_2D);      
      }
    } else {
      if(textures_1d[text_akt]) {
        textures_1d[text_akt] = FALSE;
        glDisable(GL_TEXTURE_1D);
      }
    }
  }
  
  // Turn off textures
  static void off(void)
  {
    if(textures_2d[text_akt]) {
      textures_2d[text_akt] = FALSE;
      glDisable(GL_TEXTURE_2D);      
    }  
    if(textures_1d[text_akt]) {
      textures_1d[text_akt] = FALSE;
      glDisable(GL_TEXTURE_1D);
    }  
  }
  
  // Nastavi aktivni texturu
  static int set_num(int text_unit)
  {
    if(text_unit < multitext_units) {
      if(text_akt != text_unit) {
        text_akt = text_unit;
        glActiveTextureARB(arb_translation_table[text_unit]);
      }
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  // Nastavi aktivni texturu a vypne ji
  static int set_num_off(int text_unit)
  {
    if(text_unit < multitext_units) {
      if(textures_1d[text_unit] || textures_2d[text_unit]) {
        if(text_akt != text_unit) {
          text_akt = text_unit;
          glActiveTextureARB(arb_translation_table[text_unit]);
        }    
        if(textures_1d[text_unit]) {
          textures_1d[text_unit] = FALSE;
          glDisable(GL_TEXTURE_1D);
        }
        if(textures_2d[text_unit]) {
          textures_2d[text_unit] = FALSE;
          glDisable(GL_TEXTURE_2D);
        }      
      }            
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  // Nastavi ostrost textur
  static void sharp(float sharp)
  {
    if(gl_ext::extlist_text_sharp) {
      sharp = -sharp;

      int i;
      for(i = 0; i < multitext_units; i++) {
        set_num(i);
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT,GL_TEXTURE_LOD_BIAS_EXT,sharp);
      }
    }
  }  

  static int multitext_units_get(void)
  {
    return(multitext_units);
  }
  
  static void multitext_units_set(int text_units)
  {
    multitext_units = text_units;
  }
  
public:

  static void init(int text_units)
  {
    multitext_units_set(text_units);
  }
  
} GL_TEXTURE_STATE;

/*
 *************************************************************************** 
    Texure-coordinates generator setup
 ***************************************************************************
*/

#define TEXGEN_NIC          0
#define TEXGEN_SPHEREMAP    1
#define TEXGEN_EYE_LINEAR   2

typedef class gl_texture_coordinates {

  // Texture generator configuration
  /*
public: 

  static int      texgen_s[MAX_GL_TEXT_UNITS];
  static int      texgen_t[MAX_GL_TEXT_UNITS];
  static int      texgen_r[MAX_GL_TEXT_UNITS];
  static int      texgen_q[MAX_GL_TEXT_UNITS];
  
public:
    
  static void generator_off(int unit)
  {
    if(texgen_s[unit]) {
      texgen_s[unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_S);
    }
    if(texgen_t[unit]) {
      texgen_t[unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_T);
    }
    if(texgen_r[unit]) {
      texgen_r[unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_R);
    }
    if(texgen_q[unit]) {
      texgen_q[unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_Q);
    }    
  }
    
  static void generator_on_s(int unit, int mode_s, int mode_t, int mode_r, int mode_q)
  {
    if(texgen_s[unit] != TEXGEN_SPHEREMAP) {
      texgen_s[unit] = TEXGEN_SPHEREMAP;
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glEnable(GL_TEXTURE_GEN_S);
    }
    if(texgen_t[unit] != TEXGEN_SPHEREMAP) {
      texgen_t[unit] = TEXGEN_SPHEREMAP;
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glEnable(GL_TEXTURE_GEN_T);
    }
    if(texgen_r[unit]) {
      texgen_r[unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_R);
    }
    if(texgen_q[unit]) {
      texgen_q[unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_Q);
    }    
  }

  static void linear_eye_2d_on(int text_unit, float *p_spar, float *p_tpar)
  {    
    GLMATRIX *p_mat = texgen+text_unit;
    
    if(!texgen_s[text_unit])
      glEnable(GL_TEXTURE_GEN_S);
    if(!texgen_t[text_unit])
      glEnable(GL_TEXTURE_GEN_T);
    
    if(texgen_s[text_unit] != TEXGEN_EYE_LINEAR) {
      texgen_s[text_unit] = TEXGEN_EYE_LINEAR;
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    }
    if(texgen_t[text_unit] != TEXGEN_EYE_LINEAR) {
      texgen_t[text_unit] = TEXGEN_EYE_LINEAR;
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    }
    
    if(camera_change ||
      p_spar[0] != p_mat->_11 || p_spar[1] != p_mat->_21 || 
      p_spar[2] != p_mat->_31 || p_spar[3] != p_mat->_41 ||
      p_tpar[0] != p_mat->_12 || p_tpar[1] != p_mat->_22 || 
      p_tpar[2] != p_mat->_32 || p_tpar[3] != p_mat->_42) {
      
      p_mat->_11 = p_spar[0];
      p_mat->_21 = p_spar[1];
      p_mat->_31 = p_spar[2];
      p_mat->_41 = p_spar[3];
      
      p_mat->_12 = p_tpar[0];
      p_mat->_22 = p_tpar[1];
      p_mat->_32 = p_tpar[2];
      p_mat->_42 = p_tpar[3];
      
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf((float *)get_camera());
      
      glTexGenfv(GL_S,GL_EYE_PLANE,p_spar);
      glTexGenfv(GL_T,GL_EYE_PLANE,p_tpar);
      
      glLoadMatrixf((float *)get_final());
    }
   
    if(texgen_r[text_unit]) {
      texgen_r[text_unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_R);
    }
    if(texgen_q[text_unit]) {
      texgen_q[text_unit] = FALSE;
      glDisable(GL_TEXTURE_GEN_Q);
    }
  }
  */

  // Texture coordinates transformation
public:

  static GLMATRIX text_matrix[MAX_GL_TEXT_UNITS];
  static bool     text_matrix_state[MAX_GL_TEXT_UNITS];

public:
  
  static void matrix_on(int unit, GLMATRIX *p_text)
  {
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf((float *)p_text);
    text_matrix_state[unit] = TRUE;
  }
  
  static void matrix_off(int unit)
  {
    if(text_matrix_state[unit]) {
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      text_matrix_state[unit] = FALSE;
    }
  }

} GL_TEXTURE_COORDINATES_STATE;

/*
 *************************************************************************** 
    Texture operations in texture units
 ***************************************************************************
*/
#define TYP_TEXT_COLOR  0
#define TYP_TEXT_DOT3   1
#define TYP_TEXT_DUDV   2

#define MAX_TEXTURE_OPERATORS           10

#define TEXT_OPERATOR_MODULATION        0
#define TEXT_OPERATOR_MODULATION2X      1
#define TEXT_OPERATOR_ADD               2
#define TEXT_OPERATOR_DECAL             3
#define TEXT_OPERATOR_BLEND             4
#define TEXT_OPERATOR_REPLACE           5
#define TEXT_OPERATOR_DOT3_BUMP         6


typedef class gl_texture_operators {

  // Array of texture operators
  static void          (*op_func[MAX_TEXTURE_OPERATORS])(void);
  static int             op_last[MAX_GL_TEXT_UNITS];

  // Texture operators
  static void            op_modulation(void);
  static void            op_modulation_comb(void);
  static void            op_modulation_2x_comb(void);
  static void            op_add(void);
  static void            op_add_comb(void);
  static void            op_decal(void);
  static void            op_blend(void);
  static void            op_replace(void);
  static void            op_dot3_bump(void);

public:

  static void set(int text_unit, int oper)
  {
    assert(oper >= 0 && oper < MAX_TEXTURE_OPERATORS);
    if(oper != op_last[text_unit]) {
      op_last[text_unit] = oper;
      op_func[oper]();
    }    
  }
  
  static void off(int text_unit)
  {
    int i = text_unit;
    if(i != ERROR) {
      for(; i < MAX_GL_TEXT_UNITS; i++) {
        if(!gl_texture::set_num_off(i))
          return;
      }
    }
  }
  
/*
  static void op_set_num_poly(int text_unit)
  {
    gl_texture::set_num(text_unit);
    gl_texture::on(GL_TEXTURE_2D);
    op_modulation();
    // ret_matrix_texture(text_unit);
    // __UPR__
    //text_poly_indicie = text_unit;
  }
  
  static void op_set_nic_poly(int last_text, int poly_text)
  {
    int i = last_text;
    if(i != ERROR) {
      for(; i < MAX_GL_TEXT_UNITS; i++) {
        if(!gl_texture::set_num_off(i)) {
          break;
        }
      }
    }
    op_set_num_poly(poly_text);
  }  
  */
  
  static void reset(void)
  {
    for(int i = 0; i < MAX_GL_TEXT_UNITS; i++) {
      op_last[i] = ERROR;
    }
  }

  static void init(void)
  {
    reset();
  }
  
public:

  gl_texture_operators(void)
  {
    reset();
  }
  
} GL_TEXTURE_OPERATORS_STATE;


/**********************************************************************
  Fog cubes
  *********************************************************************
*/
/*
typedef class fog_cube : public llist_item {

  char  name[MAX_NAME];   // jmeno mlzne kostky
  BOD   min,max;          // rozmery kostky
  int   flag;             // flagy mlhy
  int   mod;              // mod mlhy (linear/exp/exp2)
  int   priorita;         // priorita mlzne kostky
  float start;            // zacatek
  float stop;             // konec
  float intenzita;        // intenzita
  RGfloat r,g,b,a;          // barva
  void *p_kont;           // pointer na kontejner ke kteremu je privazana
  int   poly;             // zazba na poly

public:

  fog_cube(void)
  {            
    poly = K_CHYBA;
    a = 1.0f;
    mod = GL_LINEAR;
    max.x = 10.0f;
    max.y = 10.0f;
    max.z = 10.0f;
  }

  mlzna_kostka(char *p_jmeno)
  {
    fog_cube();
    strcpy(jmeno,p_jmeno);
  }

  fog_cube(class fog_cube &src)
  {    
   *this = src;
    p_kont = NULL;
  }

  ~fog_cube(void)
  {

  }
  
  //  - Navazovani udelat staticky
  //  - alokaci/free udelat virtualne
  
  static void set(class mlzna_kostka *p_mlha)
  {
    if(p_mlha) {
      glFogi(GL_FOG_MODE, p_mlha->mod);
      glFogfv(GL_FOG_COLOR, &p_mlha->r);
      glFogf(GL_FOG_DENSITY, p_mlha->intenzita);
      glFogf(GL_FOG_START, p_mlha->start);
      glFogf(GL_FOG_END, p_mlha->stop);
    }
  }
  
  void set(void)
  {
    glFogi(GL_FOG_MODE, mod);
    glFogfv(GL_FOG_COLOR, &r);
    glFogf(GL_FOG_DENSITY, intenzita);
    glFogf(GL_FOG_START, start);
    glFogf(GL_FOG_END, stop);
  }

} FOG_CUBE;
*/

/**********************************************************************
  List of fog cubes
  *********************************************************************
*/
/*
typedef struct mlho_kostka_list : public llist_head {
  
  MLZNA_KOSTKA *p_first;

public:

  MLZNA_KOSTKA * zrus_mlhokostku_all(MLZNA_KOSTKA *p_first)
  {  
    MLZNA_KOSTKA *p_tmp;
    while(p_first) {
      p_tmp = p_first->p_next;
      free(p_first);
      p_first = p_tmp;
    }
    return(NULL);    
  }

} MLHO_KOSTKA_LIST;
*/

/*
  void zrus_mlhokostku(MLZNA_KOSTKA **p_first, MLZNA_KOSTKA *p_maz)
  {
    MLZNA_KOSTKA *p_tmp,*p_prev;
    
    if(!p_first) {
      p_maz->p_kont = NULL;
    } else {
      if((*p_first) == p_maz) {
        (*p_first) = p_maz->p_next;
      } else {
        p_tmp = (*p_first)->p_next;
        p_prev = (*p_first);
        while(p_tmp) {
          if(p_tmp == p_maz) {
            p_prev->p_next = p_maz->p_next;
            break;
          } else {    
            p_prev = p_tmp;
            p_tmp = p_tmp->p_next;
          }
        }
      }
      free(p_maz);
    }
  }
*/

#endif //__GLWRAPPER_H__
