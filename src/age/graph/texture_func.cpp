
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

/* State-bloky pro textury
*/
//#include "age.h"

/*
   Globalni promenne pro tridu stage_text_koord
*/
/*
int stage_text_koord::texgen_on[MAX_GL_TEXT_UNITS] = {MAT2_CALC_MAP1, MAT2_CALC_MAP2, MAT2_CALC_MAP3, MAT2_CALC_MAP4};
int stage_text_koord::texgen_eye_lin_flagy[MAX_GL_TEXT_UNITS] = {MAT_T1_EYE_LIN,MAT_T2_EYE_LIN,MAT_T3_EYE_LIN,MAT_T4_EYE_LIN};
int stage_text_koord::texgen_sphere_flagy[MAX_GL_TEXT_UNITS] = {MAT_T1_SPHERE, MAT_T2_SPHERE, MAT_T3_SPHERE, MAT_T4_SPHERE};
int stage_text_koord::texgen_matrix_texture_flagy2[MAX_GL_TEXT_UNITS] = {MAT2_T1_MATRIX, MAT2_T2_MATRIX, MAT2_T3_MATRIX, MAT2_T4_MATRIX};
int stage_text_koord::texgen_map_indicie[MAX_GL_TEXT_UNITS];
*/

/* 
   Globalni promenne pro tridu stage_func_param
*/
/*
GAME_TEXT_BLOK stage_func_param::st_blok[MAX_STAGE_TEXT_BLOK] = 
{
  {"Modulace",0,-1,1,1,
  {TYP_TEXT_COLOR},
  {STAGE_BLOK_MODULACE}
  },

  {"Add",1,-1,1,1,
  {TYP_TEXT_COLOR},
  {STAGE_BLOK_ADD}
  },

  {"Decal",2,-1,1,1,
  {TYP_TEXT_COLOR},
  {STAGE_BLOK_DECAL}
  },

  {"Blend",3,-1,1,1,
  {TYP_TEXT_COLOR},
  {STAGE_BLOK_BLEND}
  },

  {"Replace",4,-1,1,1,
  {TYP_TEXT_COLOR},
  {STAGE_BLOK_REPLACE}
  },
  
  {"DOT3Color",5,0,2,2,
  {TYP_TEXT_DOT3,TYP_TEXT_COLOR},
  {STAGE_BLOK_DOT3_BUMP,STAGE_BLOK_MODULACE}
  },
  
  {"DOT3ENV",6,1,3,3,
  {TYP_TEXT_DOT3,TYP_TEXT_COLOR,TYP_TEXT_COLOR},
  {STAGE_BLOK_DOT3_BUMP,STAGE_BLOK_MODULACE,STAGE_BLOK_MODULACE}
  },
    
  {"EnvBump",7,2,3,3,
  {TYP_TEXT_DUDV,TYP_TEXT_COLOR,TYP_TEXT_COLOR},
  {STAGE_BLOK_BUMP_ENV,STAGE_BLOK_MODULACE,STAGE_BLOK_MODULACE}
  },
    
  {"Modulace 2X",0,-1,1,1,
  {TYP_TEXT_COLOR},
  {STAGE_BLOK_MODULACE2X}
  }
};
*/
/*
int (*stage_func_param::p_st_func[MAX_STAGE_TEXT_FUNC])(class stage_func_param *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag) =
{ 
  st _modulace,
  st_modulace_2x,
  st_add,
  st_decal,
  st_blend,
  st_replace,
  st_dot3_bump,
  st_dot3_bump_env,  
  st_bump_env
};

STAGE_FUNC_PARAM *p_stage
GAME_TEXT **p_text
STAGE_TEXT_KOORD *p_koord
int m1flag,
int m2flag

int stage_func_param::set(int last_text, int reserved)
{
  GAME_TEXT *p_txt;
  int t,ret = last_text+1;
  
  gl_texture::multitext_units -= reserved;
  
  if(gl_texture::set_num(last_text)) {    // cislo textury
    t = p_stage->st_textury[0];           // pouzije se pouze prvni textura
    p_txt = p_text[t];                    // tyhle textury tam narvi
    if(p_txt) {
      gl_texture::on(p_txt->type_get());
      gl_texture::set(p_txt->text_get(), p_txt->type_get());
      gl_texture_operators::set(last_text,...);
      //gl_texture_coordinates::set(last_text,m1flag,m2flag,p_stage->st_koord[t]);
    } else {
      ret = last_text;
    }    
  } else {
    ret = ERROR;
  }
    
  gl_texture::multitext_units += reserved;
  return(ret);
}

int stage_func_param::st_modulace(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{
  GAME_TEXT *p_txt;
  int t,ret = last_text+1;
  
  gl_texture::multitext_units -= rezerva;
  
  if(gl_texture::set_num(last_text)) {    // cislo textury
    t = p_stage->st_textury[0];           // pouzije se pouze prvni textura
    p_txt = p_text[t];    // tyhle textury tam narvi
    if(p_txt) {
      gl_texture::on(p_txt->type_get());
      gl_texture::set(p_txt->text_get(), p_txt->type_get());
      p_stage->el_set_func(last_text,STAGE_BLOK_MODULACE);
      p_koord->st_set(last_text,m1flag,m2flag,p_stage->st_koord[t]);
    } else {
      ret = last_text;
    }    
  } else {
    ret = ERROR;
  }
    
  gl_texture::multitext_units += rezerva;
  return(ret);
}

int stage_func_param::st_modulace_2x(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{
  GAME_TEXT *p_txt;
  int t,ret = last_text+1;
   
  gl_texture::multitext_units -= rezerva;
  
  if(gl_texture::set_num(last_text)) {    // cislo textury
    t = p_stage->st_textury[0];           // pouzije se pouze prvni textura
    p_txt = p_text[t]; // tyhle textury tam narvi
    if(p_txt) {
      gl_texture::on(p_txt->type_get());
      gl_texture::set(p_txt->text_get(), p_txt->type_get());
      gl_texture_operators::set(last_text,STAGE_BLOK_MODULACE2X);
      p_koord->st_set(last_text,m1flag,m2flag,p_stage->st_koord[t]);
    } else {
      ret = last_text;
    }    
  } else {
    ret = ERROR;
  }
    
  gl_texture::multitext_units += rezerva;
  return(ret);
}

int stage_func_param::st_add(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{
  GAME_TEXT *p_txt;
  int t,ret = last_text+1;
   
  gl_texture::multitext_units -= rezerva;
  
  if(gl_texture::set_num(last_text)) {    // cislo textury
    t = p_stage->st_textury[0];           // pouzije se pouze prvni textura
    p_txt = p_text[t]; // tyhle textury tam narvi
    if(p_txt) {
      gl_texture::on(p_txt->type_get());
      gl_texture::set(p_txt->text_get(), p_txt->type_get());
      p_stage->el_set_func(last_text,STAGE_BLOK_ADD);
      p_koord->st_set(last_text,m1flag,m2flag,p_stage->st_koord[t]);
    } else {
      ret = last_text;
    }    
  } else {
    ret = ERROR;
  }
    
  gl_texture::multitext_units += rezerva;
  return(ret);      
}

int stage_func_param::st_decal(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{
  GAME_TEXT *p_txt;
  int t,ret = last_text+1;
   
  gl_texture::multitext_units -= rezerva;
  
  if(gl_texture::set_num(last_text)) {    // cislo textury
    t = p_stage->st_textury[0];           // pouzije se pouze prvni textura
    p_txt = p_text[t]; // tyhle textury tam narvi
    if(p_txt) {
      gl_texture::on(p_txt->type_get());
      gl_texture::set(p_txt->text_get(), p_txt->type_get());
      p_stage->el_set_func(last_text,STAGE_BLOK_DECAL);
      p_koord->st_set(last_text,m1flag,m2flag,p_stage->st_koord[t]);
    } else {
      ret = last_text;
    }    
  } else {
    ret = ERROR;
  }
    
  gl_texture::multitext_units += rezerva;
  return(ret);      
}

int stage_func_param::st_blend(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{
  GAME_TEXT *p_txt;
  int t,ret = last_text+1;
   
  gl_texture::multitext_units -= rezerva;
  
  if(gl_texture::set_num(last_text)) {    // cislo textury
    t = p_stage->st_textury[0];           // pouzije se pouze prvni textura
    p_txt = p_text[t]; // tyhle textury tam narvi
    if(p_txt) {
      gl_texture::on(p_txt->type_get());
      gl_texture::set(p_txt->text_get(), p_txt->type_get());
      p_stage->el_set_func(last_text,STAGE_BLOK_BLEND);
      p_koord->st_set(last_text,m1flag,m2flag,p_stage->st_koord[t]);
    } else {
      ret = last_text;
    }    
  } else {
    ret = ERROR;
  }
    
  gl_texture::multitext_units += rezerva;
  return(ret);
}

int stage_func_param::st_replace(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{
  GAME_TEXT *p_txt;
  int t,ret = last_text+1;
   
  gl_texture::multitext_units -= rezerva;
  
  if(gl_texture::set_num(last_text)) {     // cislo textury
    t = p_stage->st_textury[0];   // pouzije se pouze prvni textura
    p_txt = p_text[t];            // tyhle textury tam narvi
    if(p_txt) {
      gl_texture::on(p_txt->type_get());
      gl_texture::set(p_txt->text_get(), p_txt->type_get());
      p_stage->el_set_func(last_text,STAGE_BLOK_REPLACE);
      p_koord->st_set(last_text,m1flag,m2flag,p_stage->st_koord[t]);
    } else {
      ret = last_text;
    }    
  } else {
    ret = ERROR;
  }
    
  gl_texture::multitext_units += rezerva;
  return(ret);
}

int stage_func_param::st_dot3_bump(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{  
  GAME_TEXT *p_txt;
  int t,ret;
  int koord;

  // Test efektu - je to podporovane videokartou?  
  glstav_multitext_units -= rezerva;
  if(glstav_bump_mapping && extlist_text_env_dot3 && last_text+p_stage->textur <= glstav_multitext_units) {
    t = p_text->textury[DOT3_BUMP]; // Text 0 - DOT3
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(last_text);
      text_on(p_txt->typ);
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(last_text,BLOK_DOT3_BUMP);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }    

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
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
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(last_text,p_text->text_funkce[DOT3_TEXT]);
      
      koord = p_text->text_koord[t];      
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }    
    last_text++;
    ret = last_text;
  } else {
    ret = ERROR;
  }
  
  glstav_multitext_units += rezerva;
  return(ret); 
}

int stage_func_param::st_dot3_bump_env(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{ 
  GAME_TEXT *p_txt;
  int t, ret;
  int koord;

  // Test efektu - je to podporovane videokartou?  
  glstav_multitext_units -= rezerva;
  if(glstav_bump_mapping && extlist_text_env_dot3 && last_text+p_stage->textur <= glstav_multitext_units) {
    t = p_text->textury[DOT3_ENV]; // Text 2 - Lightmapa
    p_txt = p_mat->p_text[t];
    if(p_mat->p_text[t]) {
      text_set_num(last_text);
      text_on(p_txt->typ);
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(last_text,p_text->text_funkce[DOT3_ENV]);
      
      koord = p_text->text_koord[t];      
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      } 
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
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
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(last_text,BLOK_DOT3_BUMP);
      
      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      }

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
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
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(last_text,p_text->text_funkce[DOT3_TEXT]);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(last_text,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(last_text);
      } 
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
      } else {
        texgen_off(last_text);
        glstav_text_map_indicie[last_text] = koord;
      }
    }    
    last_text++;
    ret = last_text;
  } else {
    ret = ERROR;
  }

  glstav_multitext_units += rezerva;
  return(ret);
}

int stage_func_param::st_bump_env(STAGE_FUNC_PARAM *p_stage, GAME_TEXT **p_text, STAGE_TEXT_KOORD *p_koord, int last_text, int rezerva, int m1flag, int m2flag)
{
  GAME_TEXT *p_txt;
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
    // Rozhod to do texturovacich jednotek
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
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(text_unit,p_text->text_funkce[DUDV_TEXT]);
            
      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(text_unit,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(text_unit);
      }    
      
      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
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
      text_set(p_txt->text, p_txt->typ);
      text_set_blok(env_unit,p_text->text_funkce[DUDV_ENV]);

      koord = p_text->text_koord[t];
      if(p_mat->flag2&glstav_posun_flagy2[koord]) {
        set_matrix_texture(env_unit,p_mat->ttext+koord);
      } else {
        ret_matrix_texture(env_unit);
      }

      if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
        texgen_env_on(last_text);
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
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
      text_set(p_txt->text, p_txt->typ);

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
        glstav_text_map_indicie[last_text] = ERROR;
      } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
        linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
        glstav_text_map_indicie[last_text] = ERROR;
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
        text_set(p_txt->text, p_txt->typ);
        text_set_blok(last_text,p_text->text_funkce[DUDV_TEXT]);
        
        koord = p_text->text_koord[t];
        if(p_mat->flag2&glstav_posun_flagy2[koord]) {
          set_matrix_texture(last_text,p_mat->ttext+koord);
        } else {
          ret_matrix_texture(last_text);
        }

        if(p_mat->flag&glstav_texgen_sphere_flagy[koord]) {
          texgen_env_on(last_text);
          glstav_text_map_indicie[last_text] = ERROR;
        } else if(p_mat->flag&glstav_texgen_eye_lin_flagy[koord]) {
          linear_eye_2d_on(last_text,p_mat->texgen_koord_s[koord],p_mat->texgen_koord_t[koord]);
          glstav_text_map_indicie[last_text] = ERROR;
        } else {
          texgen_off(last_text);
          glstav_text_map_indicie[last_text] = koord;
        }
      }
      ret = last_text+1;
    } else {
      ret = ERROR;
    }
  }
  return(ret); 
}
*/
/*
int    glstav_bump_mapping;
int    glstav_text_array[6];      // texturovaci pole
int    glstav_text_poly_indicie;  // mapovaci indicie pro poly (cislo mapy)
                             

void glstav_reset(void)
{ 
 int i;

 glstav_bump_mapping = TRUE;
 
 for(i = 0; i < 6; i++) {
   glstav_genenv_s[i] = FALSE;
   glstav_genenv_t[i] = FALSE;
   glstav_genenv_r[i] = FALSE;
   glstav_genenv_q[i] = FALSE;
   glstav_text_blok[i] = ERROR;
   glstav_text_array[i] = FALSE;
   glstav_text_map_indicie[i] = FALSE;
 }

 glstav_text_array_akt = ERROR;
 glstav_text_poly_indicie = ERROR;
}
*/
