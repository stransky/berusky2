/*
  3D interface - interface pro OpenGL
*/
#include "3d_all.h"

int    glstav_blending;    // blending on-off
int    glstav_cull;        // cull mod
int    glstav_cull_mod;    // jaka strana se orezava
int    glstav_lighting;    // gl svetla
int    glstav_specular;    // spekularni svetla
int    glstav_diffuse;     // diffusni svetla
int    glstav_mlha;
int    glstav_mlha_causal;
int    glstav_array_diffuse;
int    glstav_array_specular;
int    glstav_array_normal;
int    glstav_genenv_s[6]; // 1
int    glstav_genenv_t[6]; // 2
int    glstav_genenv_r[6]; // 3
int    glstav_genenv_q[6]; // 4
int    glstav_multitext_units;
int    glstav_alfa_blok;
int    glstav_text_blok[6];
int    glstav_num_bumb_units;
int   *glstav_p_bumb_units;
int    glstav_cliping;
int    glstav_pn_triangles;
int    glstav_deph_test;
int    glstav_alfa_test;
int    glstav_zmask;
int    glstav_bump_mapping;
int    glstav_zmena_kamery;

int    glstav_text_akt;        // aktivni texury
int    glstav_text_akt_client; // aktivni klientska texura
int    glstav_textury_2d[6];      // textury  
int    glstav_textury_1d[6];      // textury  
GLuint glstav_last_text_2d[6];    // posledni textura
GLuint glstav_last_text_1d[6];    // posledni textura
int    glstav_text_array[6];   // texturovaci pole
int    glstav_text_array_akt;  // aktualni texturovaci pole
int    glstav_text_map_indicie[6];// mapovaci indicie pro textury (cislo mapy)
int    glstav_text_poly_indicie;  // mapovaci indicie pro poly (cislo mapy)

int    glstav_posun_flagy2[6] = {MAT2_T1_MATRIX, MAT2_T2_MATRIX, MAT2_T3_MATRIX, MAT2_T4_MATRIX, 0, 0};
int    glstav_texgen_on[6]    = {MAT2_CALC_MAP1, MAT2_CALC_MAP2, MAT2_CALC_MAP3, MAT2_CALC_MAP4, 0, 0};
int    glstav_texgen_eye_lin_flagy[6] = {MAT_T1_EYE_LIN,MAT_T2_EYE_LIN,MAT_T3_EYE_LIN,MAT_T4_EYE_LIN,0,0};
int    glstav_texgen_sphere_flagy[6]  = {MAT_T1_SPHERE, MAT_T2_SPHERE, MAT_T3_SPHERE, MAT_T4_SPHERE, 0,0};
GLMATRIX glstav_texgen[6];

int    glstav_view_x;
int    glstav_view_y;
int    glstav_view_dx;
int    glstav_view_dy;

int    glstav_matrix_2d_mod;

GLenum arb_prevodni_tabulka[6] = {GL_TEXTURE0_ARB,
                                  GL_TEXTURE1_ARB,
                                  GL_TEXTURE2_ARB,
                                  GL_TEXTURE3_ARB,
                                  GL_TEXTURE4_ARB,
                                  GL_TEXTURE5_ARB};
                                  
STAGE_FUNC_ALFA p_alfa_stage_func[STAGE_ALFA_FUNKCI];
STAGE_FUNC      p_text_stage_func[STAGE_TEXT_FUNKCI];
STAGE_FUNC_ELEM p_text_stage_func_elem[STAGE_TEXT_FUNKCI];

EDIT_STATE_ALFA_BLOK alfa_stage_edit_blok[STAGE_ALFA_FUNKCI+STATE_BLOK_PRUHL+1];
EDIT_STATE_TEXT_BLOK text_stage_edit_blok[STAGE_TEXT_FUNKCI];

#define TYP_TEXTURY_NUM 3

#define TYP_TEXT_COLOR  0
#define TYP_TEXT_DOT3   1
#define TYP_TEXT_DUDV   2

byte typ_textury[TYP_TEXTURY_NUM][MAX_JMENO] = {"Color Map","DOT3 Bump-Map","DUDV Map"};


GLMATRIX __mat_kamera;
GLMATRIX __mat_world;
GLMATRIX __mat_vysl;
GLMATRIX __mat_camera_project; // transformuje 3D do 2D
GLMATRIX __mat_init;
GLMATRIX __mat_top_matrix[MAX_WORD_MATRIX_STACK];     // druha word-matrix -> top-matrix
int      __mat_top_matrix_akt;
GLMATRIX __mat_tmp;
int      __mat_text[4];

void glstav_reset(void)
{
 int i;

 glstav_bump_mapping = TRUE;
 
 glstav_cull = FALSE;
 glstav_cull_mod = K_CHYBA;
 cull_on();
 cull_back();

 glstav_lighting = TRUE;
 light_on_off(FALSE);

 glstav_specular = TRUE;
 specular_off();

 glstav_diffuse = FALSE; 

 glstav_mlha = TRUE;
 glstav_mlha_causal = FALSE;
 disable_fog(); 
 
 glstav_blending = TRUE;
 blend_off();
 
 glstav_deph_test = FALSE;
 deph_test_set(TRUE);

 glstav_alfa_test = TRUE;
 alfa_test_off();
 glAlphaFunc(GL_GREATER,0.5f);

 glstav_zmask = FALSE;
 zmask_set(TRUE);
 
 glstav_array_diffuse = FALSE;
 glstav_array_specular = FALSE;
 glstav_array_normal = FALSE;
 
 for(i = 0; i < 6; i++) {
   glstav_genenv_s[i] = FALSE;
   glstav_genenv_t[i] = FALSE;
   glstav_genenv_r[i] = FALSE;
   glstav_genenv_q[i] = FALSE;
   glstav_text_blok[i] = K_CHYBA;
   glstav_textury_1d[i] = FALSE;
   glstav_textury_2d[i] = FALSE;
   glstav_last_text_1d[i] = FALSE;
   glstav_last_text_2d[i] = FALSE;
   glstav_text_array[i] = FALSE;
   glstav_text_map_indicie[i] = FALSE;
 }

 glstav_alfa_blok = K_CHYBA; 
 glstav_text_array_akt = K_CHYBA;
 glstav_text_poly_indicie = K_CHYBA;
 glstav_text_akt = K_CHYBA;
 glstav_text_akt_client = FALSE; 
 
 glstav_pn_triangles = FALSE;
}

/* Elementarni nastavovaci funkce
*/
void el_text_modulace(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

void el_text_modulace_comb(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 
}

void el_text_modulace_2x_comb(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 

  glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 2.0f);
}

void el_text_add(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_ADD);
}

void el_text_add_comb(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 
}

void el_text_decal(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
}

void el_text_blend(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND);
}

void el_text_replace(void)
{
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void el_text_dot3_bump(void)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_DOT3_RGB_EXT);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
 
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
}

/* Stage-blok-sekce
*/
int sbl_text_modulace(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{  
  EDIT_TEXT *p_txt;
  int t,ret = last_text+1;
  int koord;
  
  glstav_multitext_units -= rezerva;
  if(text_set_num(last_text)) { // cislo textury    
    t = p_text->textury[0];
    p_txt = p_mat->p_text[t];
    if(p_txt) {
      text_on(p_txt->typ);
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(last_text,BLOK_MODULACE);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }
            
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }            
    } else {
      ret = last_text;
    }
  } else {    
    ret = K_CHYBA;
  }
  
  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_modulace_2x(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{  
  EDIT_TEXT *p_txt;
  int t,ret = last_text+1;
  int koord;
  
  glstav_multitext_units -= rezerva;
  if(text_set_num(last_text)) { // cislo textury    
    t = p_text->textury[0];
    p_txt = p_mat->p_text[t];
    if(p_txt) {
      text_on(p_txt->typ);
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(last_text,BLOK_MODULACE);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }
            
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }            
    } else {
      ret = last_text;
    }
  } else {    
    ret = K_CHYBA;
  }
  
  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_add(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{
  EDIT_TEXT *p_txt;
  int t,ret = last_text+1;
  int koord;
  
  glstav_multitext_units -= rezerva;
  if(text_set_num(last_text)) { // cislo textury    
    t = p_text->textury[0];
    p_txt = p_mat->p_text[t];
    if(p_mat->textfile[t][0] && p_mat->p_text[t]) {
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);    
      text_set_blok( last_text, BLOK_ADD);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {      
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }

    } else {
      ret = last_text;
    }
  } else {    
    ret = K_CHYBA;
  }
  
  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_decal(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{
  EDIT_TEXT *p_txt;
  int t,ret = last_text+1;
  int koord;
  
  glstav_multitext_units -= rezerva;
  if(text_set_num(last_text)) { // cislo textury    
    t = p_text->textury[0];
    p_txt = p_mat->p_text[t];
    if(p_mat->textfile[t][0] && p_mat->p_text[t]) {
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok( last_text, BLOK_DECAL);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }
            
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    } else {
      ret = last_text;
    }
  } else {    
    ret = K_CHYBA;
  }
  
  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_blend(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{
  EDIT_TEXT *p_txt;
  int t,ret = last_text+1;
  int koord;
  
  glstav_multitext_units -= rezerva;
  if(text_set_num(last_text)) { // cislo textury    
    t = p_text->textury[0];
    p_txt = p_mat->p_text[t];
    if(p_mat->textfile[t][0] && p_mat->p_text[t]) {
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);    
      text_set_blok( last_text, BLOK_BLEND);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }
            
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    } else {
      ret = last_text;
    }
  } else {    
    ret = K_CHYBA;
  }
  
  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_replace(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{
  EDIT_TEXT *p_txt;
  int t,ret = last_text+1;
  int koord;
  
  glstav_multitext_units -= rezerva;
  if(text_set_num(last_text)) { // cislo textury    
    t = p_text->textury[0];
    p_txt = p_mat->p_text[t];
    if(p_mat->textfile[t][0] && p_mat->p_text[t]) {
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok( last_text, BLOK_REPLACE);      

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }
            
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    } else {
      ret = last_text;
    }
  } else {    
    ret = K_CHYBA;
  }
  
  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_dot3_bump(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{
  EDIT_TEXT *p_txt;
  int t,ret;
  int koord;

  /* Test efektu - je to podporovane videokartou?
  */
  glstav_multitext_units -= rezerva;
  if(glstav_bump_mapping && extlist_text_env_dot3 && last_text+p_stage->textur <= glstav_multitext_units) {
    t = p_text->textury[DOT3_BUMP]; // Text 0 - DOT3
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(last_text);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok(last_text,BLOK_DOT3_BUMP);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }    

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }
    last_text++;    
  }

  if(last_text < glstav_multitext_units) {    
    t = p_text->textury[DOT3_TEXT]; // Text 1 - Color-mapa
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(last_text);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok(last_text,p_text->text_funkce[DOT3_TEXT]);
      
      koord = p_text->text_koord[t];      
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }    
    last_text++;
    ret = last_text;
  } else {
    ret = K_CHYBA;
  }
  
  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_dot3_bump_env(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{
  EDIT_TEXT *p_txt;
  int t, ret;
  int koord;

  /* Test efektu - je to podporovane videokartou?
  */
  glstav_multitext_units -= rezerva;
  if(glstav_bump_mapping && extlist_text_env_dot3 && last_text+p_stage->textur <= glstav_multitext_units) {
    t = p_text->textury[DOT3_ENV]; // Text 2 - Lightmapa
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(last_text);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok(last_text,p_text->text_funkce[DOT3_ENV]);
      
      koord = p_text->text_koord[t];      
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      } 
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }
    last_text++;

    t = p_text->textury[DOT3_BUMP]; // Text 0 - DOT3 Color-Mapa
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(last_text);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok(last_text,BLOK_DOT3_BUMP);
      
      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }
    last_text++;
  }  

  if(last_text < glstav_multitext_units) {
    t = p_text->textury[DOT3_TEXT]; // Text 1 - Color-mapa
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(last_text);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok(last_text,p_text->text_funkce[DOT3_TEXT]);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      } 
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }    
    last_text++;
    ret = last_text;
  } else {
    ret = K_CHYBA;
  }

  glstav_multitext_units += rezerva;
  return(ret);
}

int sbl_text_bump_env(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int last_text, int rezerva)
{
  EDIT_TEXT *p_txt;
  int t,ret;
  int bump_unit;
  int text_unit;
  int env_unit;
  int koord;

  glstav_multitext_units -= rezerva;

  // Test efektu - je to podporovane videokartou?  
  if(glstav_bump_mapping && extlist_ati_env_bump && last_text+p_stage->textur <= glstav_multitext_units) {
    assert(extlist_ati_env_bump_unit[last_text]||
           extlist_ati_env_bump_unit[last_text+1]||
           extlist_ati_env_bump_unit[last_text+2]);
    
    // Mam k dispozici text jednotky last_text + 2
    //   Rozhod to do texturovacich jednotek    
    if(extlist_ati_env_bump_unit[last_text+2]) {
      bump_unit = last_text+2;
      env_unit = last_text+1;
      text_unit = last_text;
    } else if (extlist_ati_env_bump_unit[last_text+1]) {
      bump_unit = last_text+1;
      env_unit = last_text+2;
      text_unit = last_text;
    } else {
      bump_unit = last_text;
      env_unit = last_text+2;
      text_unit = last_text+1;
    }

    t = p_text->textury[DUDV_TEXT]; // Zakladni textura
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(text_unit);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok(text_unit,p_text->text_funkce[DUDV_TEXT]);
            
      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(text_unit,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(text_unit);
      }    
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }

    t = p_text->textury[DUDV_ENV]; // Env textura
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(env_unit);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);
      text_set_blok(env_unit,p_text->text_funkce[DUDV_ENV]);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(env_unit,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(env_unit);
      }

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }

    t = p_text->textury[DUDV_BUMP]; // DUDV Bump-mapa    
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(bump_unit);
      text_on(p_txt->typ);
      text_set(p_txt->text,p_txt->typ);

      // Natvrdo nastavim bloky+jednotky      
      if(glstav_text_blok[bump_unit] != BLOK_BUMP_ENV) {
        glstav_text_blok[bump_unit] = BLOK_BUMP_ENV;
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  GL_BUMP_ENVMAP_ATI);
      }      
      // Nahod bumpmaping na env texturu
      glTexEnvi(GL_TEXTURE_ENV, GL_BUMP_TARGET_ATI,  GL_TEXTURE0_ARB+env_unit);

      // we want to scale the bump offsets such that a maximum offset of
      // 2 pixels occurs (assuming that the dudv map is 256 x 256).
      // GLfloat bumpMatrix[4] = {2.0f/256, 0.0f, 2.0f/256, 0.0f};
      glTexBumpParameterfvATI(GL_BUMP_ROT_MATRIX_ATI, p_text->text_ati_bump_matrix);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(bump_unit,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(bump_unit);
      }

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = K_CHYBA;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }
        
    ret = last_text+3;
  } else {
    if(last_text < glstav_multitext_units) {
      t = p_text->textury[DUDV_TEXT]; // Text 1 - Color-mapa
      p_txt = p_mat->p_text[t];
      if(p_mat->p_text[t]) {
        text_set_num(last_text);
        text_on(p_txt->typ);
        text_set(p_txt->text,p_txt->typ);
        text_set_blok(last_text,p_text->text_funkce[DUDV_TEXT]);
        
        koord = p_text->text_koord[t];
        if(p_mat->flag2&glstav_posun_flagy2[koord]) {
          set_matrix_texture(last_text,p_mat->ttext+koord);
        } else {
          ret_matrix_texture(last_text);
        }

        if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
          texgen_env_on(last_text);
          glstav_text_map_indicie[last_text] = K_CHYBA;
        } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
          texgen_linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
          glstav_text_map_indicie[last_text] = K_CHYBA;
        } else {
          texgen_off(last_text);
          glstav_text_map_indicie[last_text] = koord;
        }
      }
      ret = last_text+1;
    } else {
      ret = K_CHYBA;
    }
  }
  return(ret);
}

/* Alfa-stage bloky
*/
void sbl_alfa_default(void)
{
  blend_off();
  alfa_test_off();
}

void sbl_alfa_flek(void)
{
  blend_on();
  alfa_test_off();  
  glBlendFunc(GL_ZERO,GL_SRC_COLOR);
}

void sbl_alfa_kour(void)
{
  blend_on();
  alfa_test_off();
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
}

void sbl_alfa_pruhledny(void)
{
  blend_on();
  alfa_test_on();
  glAlphaFunc(GL_GREATER,0.004f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void sbl_alfa_test(void)
{  
  blend_off();
  alfa_test_on();
  glAlphaFunc(GL_GREATER,0.5f);
}

void nahod_state_bloky(void)
{  
  /* Text-bloky
  */
  p_text_stage_func[0] = sbl_text_modulace;
  p_text_stage_func[1] = sbl_text_add;
  p_text_stage_func[2] = sbl_text_decal;
  p_text_stage_func[3] = sbl_text_blend;
  p_text_stage_func[4] = sbl_text_replace;
  p_text_stage_func[5] = sbl_text_dot3_bump;
  p_text_stage_func[6] = sbl_text_dot3_bump_env;
  p_text_stage_func[7] = sbl_text_bump_env;
  p_text_stage_func[8] = sbl_text_modulace_2x;
  
  /* Elementarni text_bloky funkce
  */
  p_text_stage_func_elem[0] = el_text_modulace;
  p_text_stage_func_elem[1] = el_text_add;
  p_text_stage_func_elem[2] = el_text_decal;
  p_text_stage_func_elem[3] = el_text_blend;
  p_text_stage_func_elem[4] = el_text_replace;
  p_text_stage_func_elem[5] = el_text_dot3_bump;
  p_text_stage_func_elem[6] = el_text_modulace_2x_comb;  

  /* Alfa-bloky
  */
  p_alfa_stage_func[0] = sbl_alfa_default;
  p_alfa_stage_func[1] = sbl_alfa_flek;
  p_alfa_stage_func[2] = sbl_alfa_kour;
  p_alfa_stage_func[3] = sbl_alfa_pruhledny;
  p_alfa_stage_func[4] = sbl_alfa_test;

  /* Text-bloky
  */
  strcpy(text_stage_edit_blok[0].jmeno,"Modulace");
  text_stage_edit_blok[0].funkce = 0;
  text_stage_edit_blok[0].edit_funkce = -1;
  text_stage_edit_blok[0].textur = 1;
  text_stage_edit_blok[0].delka = 1;
  text_stage_edit_blok[0].textury_typ[0] = TYP_TEXT_COLOR;
  text_stage_edit_blok[0].text_funkce[0] = BLOK_MODULACE;

  strcpy(text_stage_edit_blok[1].jmeno,"Add");
  text_stage_edit_blok[1].funkce = 1;
  text_stage_edit_blok[1].edit_funkce = -1;
  text_stage_edit_blok[1].textur = 1;
  text_stage_edit_blok[1].delka = 1;
  text_stage_edit_blok[1].textury_typ[0] = TYP_TEXT_COLOR;
  text_stage_edit_blok[1].text_funkce[0] = BLOK_ADD;
  
  strcpy(text_stage_edit_blok[2].jmeno,"Decal");
  text_stage_edit_blok[2].funkce = 2;
  text_stage_edit_blok[2].edit_funkce = -1;
  text_stage_edit_blok[2].textur = 1;
  text_stage_edit_blok[2].delka = 1;
  text_stage_edit_blok[2].textury_typ[0] = TYP_TEXT_COLOR;
  text_stage_edit_blok[2].text_funkce[0] = BLOK_DECAL;
  
  strcpy(text_stage_edit_blok[3].jmeno,"Blend");
  text_stage_edit_blok[3].funkce = 3;
  text_stage_edit_blok[3].edit_funkce = -1;
  text_stage_edit_blok[3].textur = 1;
  text_stage_edit_blok[3].delka = 1;
  text_stage_edit_blok[3].textury_typ[0] = TYP_TEXT_COLOR;
  text_stage_edit_blok[3].text_funkce[0] = BLOK_BLEND;
  
  strcpy(text_stage_edit_blok[4].jmeno,"Replace");
  text_stage_edit_blok[4].funkce = 4;
  text_stage_edit_blok[4].edit_funkce = -1;
  text_stage_edit_blok[4].textur = 1;
  text_stage_edit_blok[4].delka = 1;
  text_stage_edit_blok[4].textury_typ[0] = TYP_TEXT_COLOR;
  text_stage_edit_blok[4].text_funkce[0] = BLOK_REPLACE;
  
  strcpy(text_stage_edit_blok[5].jmeno,"DOT3Color");
  text_stage_edit_blok[5].funkce = 5;
  text_stage_edit_blok[5].edit_funkce = 0;
  text_stage_edit_blok[5].textur = 2;
  text_stage_edit_blok[5].delka = 2;
  text_stage_edit_blok[5].textury_typ[0] = TYP_TEXT_DOT3; // dot-3 bump-mapa
  text_stage_edit_blok[5].textury_typ[1] = TYP_TEXT_COLOR;// color-mapa
  text_stage_edit_blok[5].text_funkce[0] = BLOK_DOT3_BUMP;
  text_stage_edit_blok[5].text_funkce[1] = BLOK_MODULACE;
  
  strcpy(text_stage_edit_blok[6].jmeno,"DOT3ENV");
  text_stage_edit_blok[6].funkce = 6;
  text_stage_edit_blok[6].edit_funkce = 1;
  text_stage_edit_blok[6].textur = 3;
  text_stage_edit_blok[6].delka = 3;
  text_stage_edit_blok[6].textury_typ[0] = TYP_TEXT_DOT3;  // bump-mapa
  text_stage_edit_blok[6].textury_typ[1] = TYP_TEXT_COLOR; // color-map textura
  text_stage_edit_blok[6].textury_typ[2] = TYP_TEXT_COLOR; // light-mapa
  text_stage_edit_blok[6].text_funkce[0] = BLOK_DOT3_BUMP;
  text_stage_edit_blok[6].text_funkce[1] = BLOK_MODULACE;
  text_stage_edit_blok[6].text_funkce[2] = BLOK_MODULACE;
  
  strcpy(text_stage_edit_blok[7].jmeno,"EnvBump");
  text_stage_edit_blok[7].funkce = 7;
  text_stage_edit_blok[7].edit_funkce = 2;
  text_stage_edit_blok[7].textur = 3;
  text_stage_edit_blok[7].delka = 3;
  text_stage_edit_blok[7].textury_typ[0] = TYP_TEXT_DUDV; // normalova mapa
  text_stage_edit_blok[7].textury_typ[1] = TYP_TEXT_COLOR;// color-map textura
  text_stage_edit_blok[7].textury_typ[2] = TYP_TEXT_COLOR;// spodni mapa
  text_stage_edit_blok[7].text_funkce[0] = BLOK_BUMP_ENV;
  text_stage_edit_blok[7].text_funkce[1] = BLOK_MODULACE;
  text_stage_edit_blok[7].text_funkce[2] = BLOK_MODULACE;

  strcpy(text_stage_edit_blok[8].jmeno,"Modulace 2X");
  text_stage_edit_blok[8].funkce = 0;
  text_stage_edit_blok[8].edit_funkce = -1;
  text_stage_edit_blok[8].textur = 1;
  text_stage_edit_blok[8].delka = 1;
  text_stage_edit_blok[8].textury_typ[0] = TYP_TEXT_COLOR;
  text_stage_edit_blok[8].text_funkce[0] = BLOK_MODULACE2X;

  /* Alfa-bloky
  */
  strcpy(alfa_stage_edit_blok[0].jmeno,"Default");
  alfa_stage_edit_blok[0].alfa_pruhledny = FALSE;
  alfa_stage_edit_blok[0].funkce = 0;
  strcpy(alfa_stage_edit_blok[1].jmeno,"Flek");
  alfa_stage_edit_blok[1].alfa_pruhledny = TRUE;
  alfa_stage_edit_blok[1].funkce = 1;
  strcpy(alfa_stage_edit_blok[2].jmeno,"Kour");
  alfa_stage_edit_blok[2].alfa_pruhledny = TRUE;
  alfa_stage_edit_blok[2].funkce = 2;
  strcpy(alfa_stage_edit_blok[3].jmeno,"Pruhledny");
  alfa_stage_edit_blok[3].alfa_pruhledny = TRUE;
  alfa_stage_edit_blok[3].funkce = 3; 
  strcpy(alfa_stage_edit_blok[4].jmeno,"Maska");
  alfa_stage_edit_blok[4].alfa_pruhledny = FALSE;
  alfa_stage_edit_blok[4].funkce = 4;
  strcpy(alfa_stage_edit_blok[STATE_BLOK_PRUHL].jmeno,"Pruhledny-system");
  alfa_stage_edit_blok[STATE_BLOK_PRUHL].alfa_pruhledny = FALSE;
  alfa_stage_edit_blok[STATE_BLOK_PRUHL].funkce = 3;
}



/*  ATI vertex array - set vertex array
*/
void mesh_vertex_array_init_ati(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_varray = &p_mesh->varray;
  p_varray->ati_handle = glNewObjectBufferATI(p_varray->ati_velikost,NULL,GL_DYNAMIC_ATI);
}

void mesh_vertex_array_zrus_ati(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_varray = &p_mesh->varray;
  if(glIsObjectBufferATI(p_varray->ati_handle)) {
    glFreeObjectBufferATI(p_varray->ati_handle);
    p_varray->ati_handle = 0;
  }
}

void mesh_vertex_array_set_ati(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_va = &p_mesh->varray;
  int handle = p_mesh->varray.ati_handle;
  int m2flag = p_mesh->p_data->m2flag;
  int norm = glstav_pn_triangles || m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);

  glArrayObjectATI(GL_VERTEX_ARRAY, 3, GL_FLOAT, 0, handle, p_va->ati_start_pos);
  array_normal_off();
  array_diffuse_off();
  array_specular_off();

  /*
  if(norm) {
    array_normal_on();
    glArrayObjectATI(GL_NORMAL_ARRAY, 3, GL_FLOAT, 0, handle, p_va->ati_start_norm);
  } else {
    array_normal_off();
  }
  
  if(m2flag&MAT2_DIFFUSE) {
    array_diffuse_on();
    glArrayObjectATI(GL_COLOR_ARRAY, 4, GL_FLOAT, 0, handle, p_va->ati_start_diff);
  } else {
    array_diffuse_off();
  }
  
  if(m2flag&(MAT2_SPECULAR|MAT2_ENV_SPEC) || p_mesh->p_data->kflag&KONT_DRAW_SPEC) {
    array_specular_on();
    glArrayObjectATI(GL_SECONDARY_COLOR_ARRAY_EXT, 3, GL_FLOAT, 0, handle, p_va->ati_start_spec);
  } else {
    array_specular_off();
  } 
  */
}

void mesh_vertex_array_set_koord_ati(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_va = &p_mesh->varray;
  int handle = p_mesh->varray.ati_handle;        
  int *p_int,ind;

  p_int = glstav_text_map_indicie;
  ind = *p_int++;  
  ind = K_CHYBA;
  array_text_set_num(0);  
  if(ind != K_CHYBA) {
    array_text_on();
    glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT, 0, handle, p_va->ati_start_text[ind]);
  } else {
    array_text_off();
  }
  ind = *p_int++;
  ind = K_CHYBA;
  if(array_text_set_num(1)) {
    if(ind != K_CHYBA) {
      array_text_on();
      glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT, 0, handle, p_va->ati_start_text[ind]);
    } else {
      array_text_off();
    }
  }
  ind = *p_int++;
  ind = K_CHYBA;
  if(array_text_set_num(2)) {
    if(ind != K_CHYBA) {
      array_text_on();
      glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT, 0, handle, p_va->ati_start_text[ind]);
    } else {
      array_text_off();
    }
  }
  ind = *p_int++;
  ind = K_CHYBA;
  if(array_text_set_num(3)) {
    if(ind != K_CHYBA) {
      array_text_on();
      glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT, 0, handle, p_va->ati_start_text[ind]);
    } else {
      array_text_off();
    }
  }
}

void mesh_vertex_array_upload_ati(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_va = &p_mesh->varray;
  int handle = p_mesh->varray.ati_handle;
  int vertexu = p_mesh->vertexnum,
      k2flag = p_mesh->p_data->k2flag,
      m2flag = p_mesh->p_data->m2flag,
      kflag = p_mesh->p_data->kflag;  
  int norm = glstav_pn_triangles || p_mesh->p_data->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
/*
  if(k2flag&KONT2_UPDATE_POS)
    glUpdateObjectBufferATI(handle,p_va->ati_start_pos,
      sizeof(p_mesh->p_vertex_pos[0])*vertexu,p_mesh->p_vertex_pos,
      GL_PRESERVE_ATI);
      */
/*
  if(norm && k2flag&KONT2_UPDATE_NORM)
    glUpdateObjectBufferATI(handle,p_va->ati_start_norm,
      sizeof(p_mesh->p_vertex_norm[0])*vertexu,p_mesh->p_vertex_norm,
      GL_PRESERVE_ATI);

  if(k2flag&KONT2_UPDATE_DIFF && m2flag&MAT2_DIFFUSE) {
    if(kflag&KONT_DRAW_VODA) {
      glUpdateObjectBufferATI(handle,p_va->ati_start_diff,
        sizeof(p_mesh->p_vertex_diff_voda[0])*vertexu,p_mesh->p_vertex_diff_voda,
        GL_PRESERVE_ATI);
    } else {  
        glUpdateObjectBufferATI(handle,p_va->ati_start_diff,
          sizeof(p_mesh->p_vertex_diff[0])*vertexu,p_mesh->p_vertex_diff,
          GL_PRESERVE_ATI);
    }
  }

  if(k2flag&KONT2_UPDATE_SPEC) {
    if(kflag&KONT_DRAW_VODA) {
      glUpdateObjectBufferATI(handle,p_va->ati_start_spec,
        sizeof(p_mesh->p_vertex_spec_voda[0])*vertexu,p_mesh->p_vertex_spec_voda,
        GL_PRESERVE_ATI);
    } else {
      glUpdateObjectBufferATI(handle,p_va->ati_start_spec,
        sizeof(p_mesh->p_vertex_spec[0])*vertexu,p_mesh->p_vertex_spec,
        GL_PRESERVE_ATI);
    }
  }

  if(k2flag&KONT2_UPDATE_TEXT1)
    glUpdateObjectBufferATI(handle,p_va->ati_start_text[0],
      sizeof(p_mesh->p_vertex_uv1_material[0])*vertexu,p_mesh->p_vertex_uv1_material,
      GL_PRESERVE_ATI);

  if(k2flag&KONT2_UPDATE_TEXT2)
    glUpdateObjectBufferATI(handle,p_va->ati_start_text[1],
      sizeof(p_mesh->p_vertex_uv2_material[0])*vertexu,p_mesh->p_vertex_uv2_material,
      GL_PRESERVE_ATI);

  if(k2flag&KONT2_UPDATE_TEXT3)
    glUpdateObjectBufferATI(handle,p_va->ati_start_text[2],
      sizeof(p_mesh->p_vertex_uv3_material[0])*vertexu,p_mesh->p_vertex_uv3_material,
      GL_PRESERVE_ATI);

  if(k2flag&KONT2_UPDATE_TEXT4)
    glUpdateObjectBufferATI(handle,p_va->ati_start_text[3],
      sizeof(p_mesh->p_vertex_uv4_material[0])*vertexu,p_mesh->p_vertex_uv4_material,
      GL_PRESERVE_ATI);
*/
  p_mesh->p_data->k2flag&=~(KONT2_UPLOAD);
}


/* Poly-listy
*/
void poly_vertex_array_init_ati(EDIT_MESH_POLY *p_poly)
{
  VERTEX_ARRAYS *p_varray = &p_poly->varray;
  p_varray->ati_handle = glNewObjectBufferATI(sizeof(p_poly->p_koord[0])*p_poly->facenum,
                         p_poly->p_koord,GL_DYNAMIC_ATI);
}

void poly_vertex_array_zrus_ati(EDIT_MESH_POLY *p_poly)
{
  VERTEX_ARRAYS *p_varray = &p_poly->varray;
  if(glIsObjectBufferATI(p_varray->ati_handle)) {
    glFreeObjectBufferATI(p_varray->ati_handle);
    p_varray->ati_handle = 0;
  }
}

void poly_vertex_array_upload_ati(EDIT_MESH_POLY *p_poly)
{  
  glUpdateObjectBufferATI(p_poly->varray.ati_handle,0,sizeof(p_poly->p_koord[0])*p_poly->facenum,
                         p_poly->p_koord,GL_DISCARD_ATI);
}


/* Navaze aktualni renderovaci pole na poly
*/
void poly_vertex_array_set_ati(EDIT_MESH_POLY *p_poly)
{
  int handle = p_poly->varray.ati_handle;
  int spec = p_poly->m2flag&(MAT2_SPECULAR|MAT2_ENV_SPEC) || p_poly->kflag&KONT_DRAW_SPEC;
  int norm = glstav_pn_triangles || p_poly->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
  int *p_int,ind;
    
  glArrayObjectATI(GL_VERTEX_ARRAY, 3, GL_FLOAT, sizeof(p_poly->p_koord[0]), handle, 0);
  
  array_diffuse_on();
  glArrayObjectATI(GL_COLOR_ARRAY,  4, GL_FLOAT, sizeof(p_poly->p_koord[0]), handle, 16 * sizeof(float));

  if(norm) {
    array_normal_on();
    glArrayObjectATI(GL_NORMAL_ARRAY, 3, GL_FLOAT, 
                     sizeof(p_poly->p_koord[0]), 
                     handle, 3 * sizeof(float));
  } else {
    array_normal_off();
  }
  
  if(spec) {
    array_specular_on();
    glArrayObjectATI(GL_SECONDARY_COLOR_ARRAY_EXT, 3, GL_FLOAT, 
                     sizeof(p_poly->p_koord[0]), 
                     handle, 24 * sizeof(float));
  } else {
    array_specular_off();        
  }
  
  p_int = glstav_text_map_indicie;
  
  if(glstav_text_poly_indicie > 0) {
    array_text_set_num(0);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT,
        sizeof(p_poly->p_koord[0]), handle,
        (6+((ind)<<1))*sizeof(float));
    } else {
      array_text_off();
    }
  } 
  p_int++;
  
  if(glstav_text_poly_indicie > 1) {
    array_text_set_num(1);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT,
        sizeof(p_poly->p_koord[0]), handle,
        (6+((ind)<<1))*sizeof(float));
    } else {
      array_text_off();
    }
  } 
  p_int++;

  if(glstav_text_poly_indicie > 2) {
    array_text_set_num(2);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT,
        sizeof(p_poly->p_koord[0]), handle,
        (6+((ind)<<1))*sizeof(float));
    } else {
      array_text_off();
    }
  }
  p_int++;


  if(glstav_text_poly_indicie > 3) {
    array_text_set_num(3);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT,
        sizeof(p_poly->p_koord[0]), handle,
        (6+((ind)<<1))*sizeof(float));
    } else {
      array_text_off();
    }
  }
  
  array_text_set_num(glstav_text_poly_indicie);
  array_text_on();
  glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT, sizeof(p_poly->p_koord[0]), handle,14 * sizeof(float));
}

// vertex-araay nastavim ale nepovilim pro to co neni pozadovany?
// 1 rutina pro rendering vsech poli
void vertex_array_start_render_ati(void)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  if(extlist_ati_element_array)
    glEnableClientState(GL_ELEMENT_ARRAY_ATI);
  
  array_text_set_num(0);
  array_text_on();
  array_text_set_num(1);
  array_text_off();
  array_text_set_num(2);
  array_text_off();
  array_text_set_num(3);
  array_text_off();
}

void vertex_array_stop_render_ati(void)
{
  glDisableClientState(GL_VERTEX_ARRAY);  
  if(extlist_ati_element_array)
    glDisableClientState(GL_ELEMENT_ARRAY_ATI);

  array_diffuse_off();
  array_specular_off();
  array_normal_off();
  
  array_text_set_num(0);
  array_text_off();
  array_text_set_num(1);
  array_text_off();
  array_text_set_num(2);
  array_text_off();
  array_text_set_num(3);
  array_text_off();
}

/* ARB vertex array - inicializuje pole
*/
void mesh_vertex_array_init_arb(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_va = &p_mesh->varray;  
  glGenBuffersARB(1, &p_va->ati_handle);
  glGenBuffersARB(1, &p_va->arb_handle_indicie);
}

// Zrusi pole
void mesh_vertex_array_zrus_arb(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_varray = &p_mesh->varray;
  
  if(glIsBufferARB(p_varray->ati_handle)) {
    glDeleteBuffersARB(1,&p_varray->ati_handle);
    p_varray->ati_handle = 0;
  }
  
  if(glIsBufferARB(p_varray->arb_handle_indicie)) {
    glDeleteBuffersARB(1,&p_varray->arb_handle_indicie);
    p_varray->arb_handle_indicie = 0;
  }
}

// Nastavi mesh pro rendering - vertexy
void mesh_vertex_array_set_arb(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_va = &p_mesh->varray;
  int m2flag = p_mesh->p_data->m2flag;
  int norm = glstav_pn_triangles || m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);

  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, p_va->arb_handle_indicie);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, p_va->ati_handle);
  glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_pos));
  
  if(norm) {
    array_normal_on();
    glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_norm));
  } else {
    array_normal_off();
  }
  
  if(m2flag&MAT2_DIFFUSE) {
    array_diffuse_on();
    glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_diff));
  } else {
    array_diffuse_off();
  }
  
  if(m2flag&(MAT2_SPECULAR|MAT2_ENV_SPEC) || p_mesh->p_data->kflag&KONT_DRAW_SPEC) {
    array_specular_on();
    glSecondaryColorPointerEXT(3, GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_spec));
  } else {
    array_specular_off();
  } 
}

// Nastavi mesh pro rendering - koordinaty
void mesh_vertex_array_set_koord_arb(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_va = &p_mesh->varray;
  int *p_int,ind;
  
  p_int = glstav_text_map_indicie;
  ind = *p_int++;  
  array_text_set_num(0);
  if(ind != K_CHYBA) {
    array_text_on();
    glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_text[ind]));
  } else {
    array_text_off();
  }
  ind = *p_int++;
  if(array_text_set_num(1)) {
    if(ind != K_CHYBA) {
      array_text_on();
      glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_text[ind]));
    } else {
      array_text_off();
    }
  }
  ind = *p_int++;
  if(array_text_set_num(2)) {
    if(ind != K_CHYBA) {
      array_text_on();
      glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_text[ind]));
    } else {
      array_text_off();
    }
  }
  ind = *p_int++;
  if(array_text_set_num(3)) {
    if(ind != K_CHYBA) {
      array_text_on();
      glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(p_va->ati_start_text[ind]));
    } else {
      array_text_off();
    }
  }  
}

// Uploadnuje mesh do videoram po zmene
void mesh_vertex_array_upload_arb(GAME_MESH *p_mesh)
{
  VERTEX_ARRAYS *p_va = &p_mesh->varray;  
  int vertexu = p_mesh->vertexnum,
      k2flag = p_mesh->p_data->k2flag,
      m2flag = p_mesh->p_data->m2flag,
      kflag = p_mesh->p_data->kflag;  
  int norm = glstav_pn_triangles || p_mesh->p_data->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
        
  if(k2flag&KONT2_UPDATE_NEW) {
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, p_va->arb_handle_indicie);  
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(p_mesh->p_face[0])*p_mesh->facevel, p_mesh->p_face, GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, p_va->ati_handle);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, p_va->ati_velikost, p_mesh->p_vertex_pos, GL_DYNAMIC_DRAW_ARB);
  }
  
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, p_va->ati_handle);
  
  if(k2flag&KONT2_UPDATE_POS)
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, p_va->ati_start_pos, sizeof(p_mesh->p_vertex_pos[0])*vertexu, p_mesh->p_vertex_pos);
  
  if(norm && k2flag&KONT2_UPDATE_NORM)
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_norm,sizeof(p_mesh->p_vertex_norm[0])*vertexu,p_mesh->p_vertex_norm);
  
  if(k2flag&KONT2_UPDATE_DIFF && m2flag&MAT2_DIFFUSE) {
    if(kflag&KONT_DRAW_VODA) {
      glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_diff,sizeof(p_mesh->p_vertex_diff_voda[0])*vertexu,p_mesh->p_vertex_diff_voda);
    } else {
      glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_diff,sizeof(p_mesh->p_vertex_diff[0])*vertexu,p_mesh->p_vertex_diff);
    }
  }
  
  if(k2flag&KONT2_UPDATE_SPEC) {
    if(kflag&KONT_DRAW_VODA) {
      glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_spec,sizeof(p_mesh->p_vertex_spec_voda[0])*vertexu,p_mesh->p_vertex_spec_voda);
    } else {
      glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_spec,sizeof(p_mesh->p_vertex_spec[0])*vertexu,p_mesh->p_vertex_spec);
    }
  }
  
  if(k2flag&KONT2_UPDATE_TEXT1)
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_text[0],sizeof(p_mesh->p_vertex_uv1_material[0])*vertexu,p_mesh->p_vertex_uv1_material);
  if(k2flag&KONT2_UPDATE_TEXT2)
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_text[1],sizeof(p_mesh->p_vertex_uv2_material[0])*vertexu,p_mesh->p_vertex_uv2_material);
  if(k2flag&KONT2_UPDATE_TEXT3)
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_text[2],sizeof(p_mesh->p_vertex_uv3_material[0])*vertexu,p_mesh->p_vertex_uv3_material);
  if(k2flag&KONT2_UPDATE_TEXT4)
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,p_va->ati_start_text[3],sizeof(p_mesh->p_vertex_uv4_material[0])*vertexu,p_mesh->p_vertex_uv4_material);

  p_mesh->p_data->k2flag&=~(KONT2_UPLOAD);
}

/* Poly-listy - inicializace
*/
void poly_vertex_array_init_arb(EDIT_MESH_POLY *p_poly)
{
  VERTEX_ARRAYS *p_va = &p_poly->varray;
  glGenBuffersARB(1, &p_va->ati_handle);
}

// Zruseni
void poly_vertex_array_zrus_arb(EDIT_MESH_POLY *p_poly)
{
  VERTEX_ARRAYS *p_va = &p_poly->varray;
  if(glIsBufferARB(p_va->ati_handle)) {
    glDeleteBuffersARB(1,&p_va->ati_handle);
    p_va->ati_handle = 0;
  }
}

// uploaduje poly do videoram
void poly_vertex_array_upload_arb(EDIT_MESH_POLY *p_poly)
{ 
  VERTEX_ARRAYS *p_va = &p_poly->varray;  
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, p_va->ati_handle);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(p_poly->p_koord[0])*p_poly->facenum, p_poly->p_koord, GL_DYNAMIC_DRAW_ARB);
}

// Nastavi poly na rendering
void poly_vertex_array_set_arb(EDIT_MESH_POLY *p_poly)
{
  VERTEX_ARRAYS *p_va = &p_poly->varray;  
  int spec = p_poly->m2flag&(MAT2_SPECULAR|MAT2_ENV_SPEC) || p_poly->kflag&KONT_DRAW_SPEC;
  int norm = glstav_pn_triangles || p_poly->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
  int *p_int,ind;
    
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, p_va->ati_handle);    
  glVertexPointer(3, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET(0));
  
  array_diffuse_on();
  glColorPointer(4, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET(16 * sizeof(float)));  

  if(norm) {
    array_normal_on();
    glNormalPointer(GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET(3 * sizeof(float)));
  } else {
    array_normal_off();
  }
  
  if(spec) {
    array_specular_on();
    glSecondaryColorPointerEXT(3, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET(24 * sizeof(float)));
  } else {
    array_specular_off();        
  }
  
  p_int = glstav_text_map_indicie;
  
  if(glstav_text_poly_indicie > 0) {
    array_text_set_num(0);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glTexCoordPointer(2, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET((6+((ind)<<1))*sizeof(float)));
    } else {
      array_text_off();
    }
  } 
  p_int++;
  
  if(glstav_text_poly_indicie > 1) {
    array_text_set_num(1);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glTexCoordPointer(2, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET((6+((ind)<<1))*sizeof(float)));
    } else {
      array_text_off();
    }
  } 
  p_int++;

  if(glstav_text_poly_indicie > 2) {
    array_text_set_num(2);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glTexCoordPointer(2, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET((6+((ind)<<1))*sizeof(float)));
    } else {
      array_text_off();
    }
  }
  p_int++;


  if(glstav_text_poly_indicie > 3) {
    array_text_set_num(3);
    if((ind = *p_int) != K_CHYBA) {
      array_text_on();
      glTexCoordPointer(2, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET((6+((ind)<<1))*sizeof(float)));
    } else {
      array_text_off();
    }
  }
  
  array_text_set_num(glstav_text_poly_indicie);
  array_text_on();
  glTexCoordPointer(2, GL_FLOAT, sizeof(p_poly->p_koord[0]), BUFFER_OFFSET(14 * sizeof(float)));
}

// start renderu
void vertex_array_start_render_arb(void)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  
  array_text_set_num(0);
  array_text_on();
  array_text_set_num(1);
  array_text_off();
  array_text_set_num(2);
  array_text_off();
  array_text_set_num(3);
  array_text_off();
}

// stop renderu
void vertex_array_stop_render_arb(void)
{
  glDisableClientState(GL_VERTEX_ARRAY);    
  
  array_diffuse_off();
  array_specular_off();
  array_normal_off();

  array_text_set_num(0);
  array_text_off();
  array_text_set_num(1);
  array_text_off();
  array_text_set_num(2);
  array_text_off();
  array_text_set_num(3);
  array_text_off();
}

/* Univerzalni pointer-based
*/
void (* mesh_vertex_array_init)(GAME_MESH *p_mesh);
void (* mesh_vertex_array_zrus)(GAME_MESH *p_mesh);
void (* mesh_vertex_array_upload)(GAME_MESH *p_mesh);
void (* mesh_vertex_array_set)(GAME_MESH *p_mesh);
void (* mesh_vertex_array_set_koord)(GAME_MESH *p_mesh);
void (* vertex_array_start_render)(void);
void (* vertex_array_stop_render)(void);

void (* poly_vertex_array_init)(EDIT_MESH_POLY *p_poly);
void (* poly_vertex_array_zrus)(EDIT_MESH_POLY *p_poly);
void (* poly_vertex_array_upload)(EDIT_MESH_POLY *p_poly);
void (* poly_vertex_array_set)(EDIT_MESH_POLY *p_poly);

void vertex_array_start_render_nic(void) {};
void vertex_array_stop_render_nic(void) {};


/* Inicializuje vretexove pole podle podporovanych extenzi
*/
void vertex_array_ini(void)
{
  if(extlist_arb_vertex_buffer) {

    extlist_vertex_array = TRUE;
    extlist_indicie_array = TRUE;

    poly_vertex_array_init = poly_vertex_array_init_arb;
    poly_vertex_array_zrus = poly_vertex_array_zrus_arb;    
    poly_vertex_array_upload = poly_vertex_array_upload_arb;
    poly_vertex_array_set = poly_vertex_array_set_arb;

    mesh_vertex_array_init = mesh_vertex_array_init_arb;
    mesh_vertex_array_zrus = mesh_vertex_array_zrus_arb;    
    mesh_vertex_array_upload = mesh_vertex_array_upload_arb;
    mesh_vertex_array_set = mesh_vertex_array_set_arb;
    mesh_vertex_array_set_koord = mesh_vertex_array_set_koord_arb;

    vertex_array_start_render = vertex_array_start_render_arb;
    vertex_array_stop_render = vertex_array_stop_render_arb;

  } else if(extlist_ati_vertex_array_object) {

    extlist_vertex_array = extlist_ati_vertex_array_object;
    extlist_indicie_array = extlist_ati_element_array;

    poly_vertex_array_init = poly_vertex_array_init_ati;
    poly_vertex_array_zrus = poly_vertex_array_zrus_ati;    
    poly_vertex_array_upload = poly_vertex_array_upload_ati;
    poly_vertex_array_set = poly_vertex_array_set_ati;

    mesh_vertex_array_init = mesh_vertex_array_init_ati;
    mesh_vertex_array_zrus = mesh_vertex_array_zrus_ati;    
    mesh_vertex_array_upload = mesh_vertex_array_upload_ati;
    mesh_vertex_array_set = mesh_vertex_array_set_ati;
    mesh_vertex_array_set_koord = mesh_vertex_array_set_koord_ati;

    vertex_array_start_render = vertex_array_start_render_ati;
    vertex_array_stop_render = vertex_array_stop_render_ati;

  } else {
    vertex_array_start_render = vertex_array_start_render_nic;
    vertex_array_stop_render = vertex_array_stop_render_nic;
  }
}
