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
 * Material class
 */

#include "age.h"

MESH_MATERIAL * mesh_material::p_mat_last = NULL;
MESH_MATERIAL * mesh_material::p_mat_scene = NULL;

void mesh_material::clear(void)
{  
  // Texture pointers initialization
  memset(p_text,0,sizeof(p_text));
  
  diff.set(1);  
  spec.set(0);
  factor.set(1);
  
  shine = 1.0f;
  
  m2flags.flag_set(MATERIAL2_DIFFUSE);
  mflags.flag_set(MATERIAL_ZMASK);
  mflags.flag_set(MATERIAL_ZTEST);
  mflags.flag_set(MATERIAL_CULL);
  
  //env_scale = 1.0f;
}

/*
void mesh_material::free(void)
{
  //int i,j;

  //if(flag&MAT_ANIM_FRAME)
    //lo_zrus_animaci(&anim);
  
  for(j = 0; j < MAT_TEXTUR; j++) {
    for(i = 0; i < MAX_TEXT_ANIM; i++) {
      if(p_atext[j][i])
        key_zrus_material_animace(p_atext[j]+i);
    }
  }
   
  if(p_sub)
    zrus_material(&p_sub);  
  
}
*/

// Set the default values for material
mesh_material::mesh_material(class scene *p_scene)
  : object_list(p_scene)
{
  clear();
}

mesh_material::~mesh_material(void)
{
}

void mesh_material::alpha_set(void)
{
  alpha.set();
}

int mesh_material::texture_on(int unit, int config, int reserved)
{
  MATERIAL_TEXT_CONFIG *p_config = text_config+config;
  MATERIAL_TEXT *p_txt;
    
  if(p_config->active) {
    
    int ret = unit+1;
    
    gl_texture::multitext_units -= reserved;
    
    if(gl_texture::set_num(unit)) {
      p_txt = p_text[p_config->text_index];
      if(p_txt) {
        gl_texture::on(p_txt->gl_text_type_get());
        gl_texture::set(p_txt->gl_text_get(), p_txt->gl_text_type_get());
        gl_texture_operators::set(unit,p_config->text_operator);
        text_coord[p_config->text_coordinates].set(unit);
      } else {
        ret = unit;
      }    
    } else {
      ret = FALSE;
    }
    
    gl_texture::multitext_units += reserved;
    
    return(ret);
  }
  
  return(FALSE);
}

void mesh_material::texture_off(int unit)
{
  gl_texture::set_num_off(unit);
}

void mesh_material::texture_set(void)
{
  int texture, i;
  for(texture = 0, i = 0; i < MAT_TEXTURES; i++) {
    if(!(texture = texture_on(texture,i,0))) {
      texture_off(i);
      break;
    }
  }
}

void mesh_material::texture_off(void)
{
  int i;
  for(i = 0; i < MAT_TEXTURES; i++) {    
    texture_off(i);
  }  
}

// Sets material
int mesh_material::set(bool forced)
{
  //int i,t;
/*
  // Nastaveni spekularni barvy  
  if(flag2&(MAT2_SPECULAR|MAT2_ENV_SPEC) || kflag&KONT_DRAW_SPEC)
    spec.on();
  else
    spec.off();

  // Nastaveni pruhlednosti  
  if(kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) {
    alfa.set(STATE_BLOK_PRUHL);
  } else {
    //... nahod aktualni alfa-stage
    // Alfa-blending    
    alfa.set();
  }
*/
  alpha_set();
  
  // Dont set already used active material
  if(!forced && p_mat_last == this)
    return(FALSE);
  p_mat_last = this;

  texture_set();
  
  // Nahozeni matrose-plazivce
  //if(!p_smat || flag&MAT_NO_SCMT || !p_smat->text[0].is_loaded()) {
    /*
  } else {
    STAGE_FUNC_PARAM *p_stg      = stage_param;
    STAGE_FUNC_PARAM *p_stg_smat = p_smat->stage_param;

    int smat_pozice = p_smat->smaterial_pozice; 
    int smat_rezerva;
    int mat_rezerva;
    
    if(p_smat->flag&MAT_SCENE_NUTNY) {
      smat_rezerva = p_smat->textur;
      mat_rezerva = 0;
    } else {
      smat_rezerva = 0;
      mat_rezerva = textur;
    }
    
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if(i == smat_pozice) {
        if((t = p_stg_smat->set(p_smat->text,p_smat->text_koord,t,mat_rezerva,p_smat->flag,p_smat->flag2)) == ERROR)
          break;
        smat_rezerva = 0;
      }            
      if((t = p_stg->set(text,text_koord,t,smat_rezerva,flag,flag2)) == ERROR) {
        break;
      }
      p_stg++;
    }
    if(i < smat_pozice) {
      t = p_stg_smat->set(p_smat->text,p_smat->text_koord,t,mat_rezerva,p_smat->flag,p_smat->flag2);
    }  
    p_stg->el_set_nic(t);
  }
  */
   
  if(m2flags.flag_get(MATERIAL2_DIFFUSE)) {
    gl_diffuse::on();
    gl_diffuse::set(&diff);
  } else {
    gl_diffuse::off();
  }
  
  gl_z_buffer::test(mflags.flag_get(MATERIAL_ZTEST));
  gl_z_buffer::mask_set(mflags.flag_get(MATERIAL_ZMASK));
  
  gl_cull_mod::set(mflags.flag_get(MATERIAL_CULL));
    
  return(TRUE);
}

#define MATERIAL_DEFAULT        ((MESH_MATERIAL *)0)  
#define MATERIAL_SELECTION      ((MESH_MATERIAL *)-1)

void mesh_material::set_default(bool forced)
{
  if(!forced && p_mat_last == MATERIAL_DEFAULT)
    return;
  p_mat_last = MATERIAL_DEFAULT;
  
  mesh_material::texture_off();
  
  gl_specular::off(TRUE);
  material_alpha::alpha_functions.set(ALPHA_OFF);
  
  gl_diffuse::on(TRUE);
  gl_diffuse::set(1.0f,1.0f,1.0f,0.0f);

  gl_z_buffer::on(TRUE);
  gl_z_buffer::mask_on(TRUE);
    
  gl_cull_mod::off(TRUE);
}

void mesh_material::set_selection(bool forced)
{
  if(!forced && p_mat_last == MATERIAL_SELECTION)
    return;
  p_mat_last = MATERIAL_SELECTION;
  
  mesh_material::texture_off();
  
  gl_specular::off();  
  material_alpha::alpha_functions.set(ALPHA_OFF);
  
  gl_diffuse::on();
  gl_diffuse::set(0.0f,0.0f,1.0f,0.0f);

  gl_z_buffer::on();
  gl_z_buffer::mask_on();
  
  //gl_cull_mod::on();
  gl_cull_mod::off();
}

void mesh_material::print(void)
{
  pprintf("Material name '%s'",name_get());
  int i;
  for(i = 0; i < MAT_TEXTURES; i++) {
    pprintf(" Texture [%d]'%s'",i,p_text[i] ? p_text[i]->name_get() : "NULL");
  }
}

void mesh_material::material_remove(void)
{
  SCENE *p_scene = scene_get();
  assert(p_scene != NULL);
  p_scene->material_remove(this);  
}

void material_list::print(void)
{
  MESH_MATERIAL *p_tmp = (MESH_MATERIAL *)get_first();
  while(p_tmp) {
    p_tmp->print();
    p_tmp = (MESH_MATERIAL *)p_tmp->object_list_next();
  }    
}
