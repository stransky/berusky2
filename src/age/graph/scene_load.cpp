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


// ************************************************************************
//  Materials
// ***********************************************************************





// **********************************************************************
//   Ulozeni environmentu
/*
int lo_chunk_save_environment(FHANDLE f, LEVEL_ENVIRONMENT *p_env)
{
  DATA_CHUNK ch;  
  
  ch.typ = CHUNK_ENVIRONMENT;
  ch.velikost = sizeof(ch)+sizeof(p_env[0]);

  file_write(f,&ch,sizeof(ch),1,f);
  file_write(f,p_env,sizeof(p_env[0]),1,f);
  
  return(TRUE);
}
*/

// **********************************************************************
//   Ulozeni omezeni a nastaveni kamer
/*
int lo_chunk_save_kamset(FHANDLE f, NASTAVENI_KAMER *p_kam)
{
  DATA_CHUNK ch;  
  
  ch.typ = CHUNK_KAMSET;
  ch.velikost = sizeof(ch)+
                sizeof(p_kam->far_plane)+
                sizeof(p_kam->fov)+
                sizeof(p_kam->max_uhel)+
                sizeof(p_kam->min_uhel)+
                sizeof(p_kam->min_vzdal)+
                sizeof(p_kam->max_vzdal);

  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_kam->far_plane,sizeof(p_kam->far_plane),1,f);
  file_write(&p_kam->fov,sizeof(p_kam->fov),1,f);
  file_write(&p_kam->max_uhel,sizeof(p_kam->max_uhel),1,f);
  file_write(&p_kam->min_uhel,sizeof(p_kam->min_uhel),1,f);
  file_write(&p_kam->min_vzdal,sizeof(p_kam->min_vzdal),1,f);
  file_write(&p_kam->max_vzdal,sizeof(p_kam->max_vzdal),1,f);
  
  return(TRUE);
}

int lo_chunk_save_kamset_2(FHANDLE f, NASTAVENI_KAMER *p_kam)
{
  DATA_CHUNK ch;  
  
  ch.typ = CHUNK_KAMSET_2;
  ch.velikost = sizeof(ch)+
                sizeof(p_kam->max)+
                sizeof(p_kam->min);

  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_kam->max,sizeof(p_kam->max),1,f);
  file_write(&p_kam->min,sizeof(p_kam->min),1,f);
  
  return(TRUE);
}
*/
/*
 **********************************************************************
   Ulozeni dynamickeho svetla


int lo_chunk_save_dyn_light_anim(FHANDLE f, DYN_LIGHT_ANIM *p_an, int extra)
{
  file_write(&p_an->pos_keys,sizeof(p_an->pos_keys),1,f);
  file_write(p_an->p_pkeys,sizeof(p_an->p_pkeys[0]),p_an->pos_keys,f);
  file_write(p_an->p_pos,sizeof(p_an->p_pos[0]),p_an->pos_keys,f);
  
  file_write(&p_an->trg_keys,sizeof(p_an->trg_keys),1,f);
  file_write(p_an->p_tkeys,sizeof(p_an->p_tkeys[0]),p_an->trg_keys,f);
  file_write(p_an->p_trg,sizeof(p_an->p_trg[0]),p_an->trg_keys,f);

  file_write(&p_an->dos_keys,sizeof(p_an->dos_keys),1,f);
  file_write(p_an->p_dskeys,sizeof(p_an->p_dskeys[0]),p_an->dos_keys,f);
  file_write(p_an->p_dosah,sizeof(p_an->p_dosah[0]),p_an->dos_keys,f);

  if(!extra) {
    file_write(&p_an->diff_keys,sizeof(p_an->diff_keys),1,f);
    file_write(p_an->p_dfkeys,sizeof(p_an->p_dfkeys[0]),p_an->diff_keys,f);
    file_write(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->diff_keys,f);
  } else {
    int i;  
    p_an->p_diff = (BARVA_RGBA *)mmalloc(sizeof(p_an->p_diff[0])*p_an->alfa_keys);
    for(i = 0 ; i < p_an->alfa_keys; i++) {
      p_an->p_diff[i].r = p_an->p_diff[i].g = p_an->p_diff[i].b = p_an->p_diff[i].a = p_an->p_alfa[i];
    }
    file_write(&p_an->alfa_keys,sizeof(p_an->alfa_keys),1,f);
    file_write(p_an->p_akeys,sizeof(p_an->p_akeys[0]),p_an->alfa_keys,f);
    file_write(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->alfa_keys,f);      
    null_free(p_an->p_diff);
  }

  file_write(&p_an->spec_keys,sizeof(p_an->spec_keys),1,f);
  file_write(p_an->p_spkeys,sizeof(p_an->p_spkeys[0]),p_an->spec_keys,f);
  file_write(p_an->p_spec,sizeof(p_an->p_spec[0]),p_an->spec_keys,f);
  
  return(TRUE);
}

int lo_chunk_get_dyn_light_size(FHANDLE f, DYN_LIGHT_ANIM *p_an)
{
  return(sizeof(p_an->pos_keys)+
    sizeof(p_an->p_pkeys[0])*p_an->pos_keys+
    sizeof(p_an->p_pos[0])*p_an->pos_keys+
    sizeof(p_an->trg_keys)+
    sizeof(p_an->p_tkeys[0])*p_an->trg_keys+
    sizeof(p_an->p_trg[0])*p_an->trg_keys+
    sizeof(p_an->dos_keys)+
    sizeof(p_an->p_dskeys[0])*p_an->dos_keys+
    sizeof(p_an->p_dosah[0])*p_an->dos_keys+
    sizeof(p_an->diff_keys)+
    sizeof(p_an->p_dfkeys[0])*p_an->diff_keys+
    sizeof(p_an->p_diff[0])*p_an->diff_keys+
    sizeof(p_an->spec_keys)+
    sizeof(p_an->p_spkeys[0])*p_an->spec_keys+
    sizeof(p_an->p_spec[0])*p_an->spec_keys);
}

int lo_chunk_save_dyn_light(FHANDLE f, DYN_LIGHT *p_light)
{
  DATA_CHUNK ch;  
  ch.typ = CHUNK_DYNAMIC_LIGHT;
  ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+sizeof(p_light->np)+
                sizeof(p_light->dir)+sizeof(p_light->look_max)+sizeof(p_light->dosah)+
                sizeof(p_light->min)+sizeof(p_light->max)+sizeof(p_light->theta)+
                sizeof(p_light->dr)+sizeof(p_light->dg)+sizeof(p_light->db)+
                sizeof(p_light->an_flag)+sizeof(p_light->flag);

  if(p_light->an.endtime) {
    ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
  }

  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_light->akt,sizeof(p_light->akt),1,f);
  file_write(p_light->name,sizeof(byte),strlen(p_light->name)+1,f);
  file_write(&p_light->np,sizeof(p_light->np),1,f);
  file_write(&p_light->dir,sizeof(p_light->dir),1,f);
  file_write(&p_light->look_max,sizeof(p_light->look_max),1,f);
  file_write(&p_light->dosah,sizeof(p_light->dosah),1,f);
  file_write(&p_light->min,sizeof(p_light->min),1,f);
  file_write(&p_light->max,sizeof(p_light->max),1,f);
  file_write(&p_light->theta,sizeof(p_light->theta),1,f);
  file_write(&p_light->dr,sizeof(p_light->dr),1,f);
  file_write(&p_light->dg,sizeof(p_light->dg),1,f);
  file_write(&p_light->db,sizeof(p_light->db),1,f);
  file_write(&p_light->sr,sizeof(p_light->sr),1,f);
  file_write(&p_light->sg,sizeof(p_light->sg),1,f);
  file_write(&p_light->sb,sizeof(p_light->sb),1,f);
  file_write(&p_light->flag,sizeof(p_light->flag),1,f);
  file_write(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
  file_write(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
  file_write(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
  if(p_light->an.endtime) {
    lo_chunk_save_dyn_light_anim(f, &p_light->an, FALSE);
  }

  return(TRUE);
}
// ulozi linearni seznam svetel
int lo_chunk_save_dyn_light_mesh(FHANDLE f, DYN_LIGHT *p_light)
{
  DATA_CHUNK ch;  
  while(p_light) {
    ch.typ = CHUNK_DYNAMIC_LIGHT_MESH;
    ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+sizeof(p_light->np)+
      sizeof(p_light->dir)+sizeof(p_light->look_max)+sizeof(p_light->dosah)+
      sizeof(p_light->min)+sizeof(p_light->max)+sizeof(p_light->theta)+
      sizeof(p_light->dr)+sizeof(p_light->dg)+sizeof(p_light->db)+
      sizeof(p_light->an_flag)+sizeof(p_light->flag);

    if(p_light->an.endtime) {
      ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
    }
  
    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_light->akt,sizeof(p_light->akt),1,f);
    file_write(p_light->name,sizeof(byte),strlen(p_light->name)+1,f);
    file_write(&p_light->np,sizeof(p_light->np),1,f);
    file_write(&p_light->dir,sizeof(p_light->dir),1,f);
    file_write(&p_light->look_max,sizeof(p_light->look_max),1,f);
    file_write(&p_light->dosah,sizeof(p_light->dosah),1,f);
    file_write(&p_light->min,sizeof(p_light->min),1,f);
    file_write(&p_light->max,sizeof(p_light->max),1,f);
    file_write(&p_light->theta,sizeof(p_light->theta),1,f);
    file_write(&p_light->dr,sizeof(p_light->dr),1,f);
    file_write(&p_light->dg,sizeof(p_light->dg),1,f);
    file_write(&p_light->db,sizeof(p_light->db),1,f);
    file_write(&p_light->sr,sizeof(p_light->sr),1,f);
    file_write(&p_light->sg,sizeof(p_light->sg),1,f);
    file_write(&p_light->sb,sizeof(p_light->sb),1,f);
    file_write(&p_light->flag,sizeof(p_light->flag),1,f);
    file_write(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    file_write(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    file_write(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
    if(p_light->an.endtime) {
      lo_chunk_save_dyn_light_anim(f, &p_light->an,FALSE);
    }
    p_light = p_light->p_next;
  }
  return(TRUE);
}
*/
/*

***********************************************************
   Ulozeni extra-svetel
  ***********************************************************


int lo_chunk_save_extra_light(FHANDLE f, EXTRA_DYN_LIGHT *p_light)
{
  DATA_CHUNK ch;  
  ch.typ = CHUNK_EXTRA_LIGHT;
  ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+
                sizeof(p_light->flag)+sizeof(p_light->mflag)+
                sizeof(p_light->ktrida)+sizeof(p_light->np)+
                sizeof(p_light->dir)+sizeof(p_light->dosah)+
                sizeof(p_light->theta)+sizeof(p_light->utlum)+
                sizeof(p_light->gourand)+4*sizeof(p_light->da)+
                sizeof(p_light->sr)+sizeof(p_light->sg)+sizeof(p_light->sb)+
                sizeof(p_light->an_flag)+sizeof(p_light->an.endtime);

  if(p_light->an.endtime) {
    ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
  }

  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_light->akt,sizeof(p_light->akt),1,f);
  file_write(p_light->name,sizeof(byte),strlen(p_light->name)+1,f);
  file_write(&p_light->flag,sizeof(p_light->flag),1,f);
  file_write(&p_light->mflag,sizeof(p_light->mflag),1,f);
  file_write(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
  file_write(&p_light->np,sizeof(p_light->np),1,f);
  file_write(&p_light->dir,sizeof(p_light->dir),1,f);
  file_write(&p_light->dosah,sizeof(p_light->dosah),1,f);
  file_write(&p_light->theta,sizeof(p_light->theta),1,f);
  file_write(&p_light->utlum,sizeof(p_light->utlum),1,f);
  file_write(&p_light->gourand,sizeof(p_light->gourand),1,f);
  file_write(&p_light->da,sizeof(p_light->da),4,f); // alfa se ulozi jako duplikovana
  file_write(&p_light->sr,sizeof(p_light->sr),1,f);
  file_write(&p_light->sg,sizeof(p_light->sg),1,f);
  file_write(&p_light->sb,sizeof(p_light->sb),1,f);
  file_write(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
  file_write(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
  if(p_light->an.endtime) {
    lo_chunk_save_dyn_light_anim(f, &p_light->an,TRUE);
  }

  return(TRUE);
}

int lo_chunk_save_extra_light_mesh(FHANDLE f, EXTRA_DYN_LIGHT *p_light)
{
  DATA_CHUNK ch;  

  while(p_light) {
    ch.typ = CHUNK_EXTRA_LIGHT_MESH;
    ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+
      sizeof(p_light->flag)+sizeof(p_light->mflag)+
      sizeof(p_light->ktrida)+sizeof(p_light->np)+
      sizeof(p_light->dir)+sizeof(p_light->dosah)+
      sizeof(p_light->theta)+sizeof(p_light->utlum)+
      sizeof(p_light->gourand)+4*sizeof(p_light->da)+
      sizeof(p_light->sr)+sizeof(p_light->sg)+sizeof(p_light->sb)+
      sizeof(p_light->an_flag)+sizeof(p_light->an.endtime);        
    
    if(p_light->an.endtime) {
      ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
    }

    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_light->akt,sizeof(p_light->akt),1,f);
    file_write(p_light->name,sizeof(byte),strlen(p_light->name)+1,f);
    file_write(&p_light->flag,sizeof(p_light->flag),1,f);
    file_write(&p_light->mflag,sizeof(p_light->mflag),1,f);
    file_write(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    file_write(&p_light->np,sizeof(p_light->np),1,f);
    file_write(&p_light->dir,sizeof(p_light->dir),1,f);
    file_write(&p_light->dosah,sizeof(p_light->dosah),1,f);
    file_write(&p_light->theta,sizeof(p_light->theta),1,f);
    file_write(&p_light->utlum,sizeof(p_light->utlum),1,f);
    file_write(&p_light->gourand,sizeof(p_light->gourand),1,f);
    file_write(&p_light->da,sizeof(p_light->da),4,f);
    file_write(&p_light->sr,sizeof(p_light->sr),1,f);
    file_write(&p_light->sg,sizeof(p_light->sg),1,f);
    file_write(&p_light->sb,sizeof(p_light->sb),1,f);
    file_write(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    file_write(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
    if(p_light->an.endtime) {
      lo_chunk_save_dyn_light_anim(f, &p_light->an,TRUE);
    }
    p_light = p_light->p_next;
  }
  return(TRUE);
}

 **********************************************************************
   Ulozeni mlho-kostek


void lo_chunk_save_mlhokostku(FHANDLE f, MLZNA_KOSTKA *p_top, int kont)
{   
  DATA_CHUNK ch;    
  int kid;

  while(p_top) {
    
    if(!kont && p_top->p_kont) {
      p_top = p_top->p_next; // uloz vsechny mlho-kostky bez kontejnerovych
      continue;
    }

    ch.typ = CHUNK_MLHOKOSTKA_2;
    ch.velikost = sizeof(ch)+
      sizeof(byte)+
      sizeof(BOD)+
      sizeof(BOD)+
      sizeof(p_top->flag)+
      sizeof(p_top->mod)+
      sizeof(p_top->priorita)+
      sizeof(p_top->start)+
      sizeof(p_top->stop)+
      sizeof(p_top->intenzita)+
      sizeof(float)*4+
      sizeof(int)+sizeof(int);
    
    file_write(&ch,sizeof(ch),1,f);
    file_write(p_top->name,sizeof(byte),strlen(p_top->name)+1,f);
    file_write(&p_top->min,sizeof(BOD),1,f);
    file_write(&p_top->max,sizeof(BOD),1,f);
    file_write(&p_top->flag,sizeof(p_top->flag),1,f);
    file_write(&p_top->mod,sizeof(p_top->mod),1,f);
    file_write(&p_top->priorita,sizeof(p_top->priorita),1,f);
    file_write(&p_top->start,sizeof(p_top->start),1,f);
    file_write(&p_top->stop,sizeof(p_top->stop),1,f);
    file_write(&p_top->intenzita,sizeof(p_top->intenzita),1,f);
    file_write(&p_top->r,sizeof(float)*4,1,f);
    
    kid = p_top->p_kont ? 1 : K_CHYBA;
    file_write(&kid,sizeof(kid),1,f); 
    
    file_write(&p_top->poly,sizeof(p_top->poly),1,f);
    
    if(kont) // pokud je kont - uloz 1 kostku kontejneru
      return;
    p_top = p_top->p_next;
  }  
}


 **********************************************************************
   Ulozeni flare


void lo_chunk_save_flare(FHANDLE f, LENS_FLARE *p_flare, EDIT_MATERIAL **p_mat, int light)
{ 
  DATA_CHUNK ch;    
  char pom[200];
  int  nic = K_CHYBA;

  if(!p_flare)
    return;

  strcpy(pom,p_mat[p_flare->material]->name);
  ch.typ = light ? CHUNK_FLARE_LIGHT_EXT : CHUNK_FLARE_EXT;
  ch.velikost = sizeof(ch)+
                sizeof(BOD)+
                sizeof(float)*4+
                sizeof(float)*2+
                sizeof(int)+
                sizeof(int)+
                sizeof(int)+
                strlen(pom)+1;

  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_flare->p,sizeof(BOD),1,f);      // umisteni
  file_write(&p_flare->r,sizeof(float),4,f);    // barva
  file_write(&p_flare->dx,sizeof(float),2,f);   // rozmery
  file_write(&nic,sizeof(int),1,f); // navazani  
  file_write(&p_flare->alfa,sizeof(int),1,f);
  file_write(&p_flare->zflag,sizeof(int),1,f);
  file_write(pom,sizeof(byte),strlen(pom)+1,f);  
}

void lo_chunk_save_flare_mesh(FHANDLE f, LENS_FLARE *p_flare, EDIT_MATERIAL **p_mat)
{ 
  DATA_CHUNK ch;    
  char pom[200];
  int  nic = K_CHYBA;

  if(!p_flare)
    return;

  strcpy(pom,p_mat[p_flare->material]->name);
  ch.typ = CHUNK_FLARE_MESH_EXT;
  ch.velikost = sizeof(ch)+
                sizeof(BOD)+
                sizeof(float)*4+
                sizeof(float)*2+
                sizeof(int)+
                sizeof(int)+
                sizeof(int)+
                strlen(pom)+1;

  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_flare->p,sizeof(BOD),1,f);      // umisteni
  file_write(&p_flare->r,sizeof(float),4,f);    // barva
  file_write(&p_flare->dx,sizeof(float),2,f);   // rozmery
  //file_write(&p_flare->svetlo,sizeof(int),1,f); // navazani
  file_write(&nic,sizeof(int),1,f); // navazani  
  file_write(&p_flare->alfa,sizeof(int),1,f);
  file_write(&p_flare->zflag,sizeof(int),1,f);
  file_write(pom,sizeof(byte),strlen(pom)+1,f);  
}

void lo_chunk_save_flare_slozeny(FHANDLE f, LENS_FLARE *p_flare, EDIT_MATERIAL **p_mat)
{
  DATA_CHUNK ch;
  char      pom[SLOZ_FLARE][200];
  int       s,d = 0;

  if(!p_flare || !p_flare->p_sloz)
    return;

  for(s = 0; s < SLOZ_FLARE; s++) {
    if(p_flare->p_sloz[s].vzdal > 0.0f)
      strcpy(pom[s],p_mat[p_flare->p_sloz[s].material]->name);
    else
      pom[s][0] = 0;
    d += strlen(pom[s])+1;
  }

  ch.typ = CHUNK_FLARE_SLOZENY;
  ch.velikost = sizeof(ch)+
                sizeof(float)*SLOZ_FLARE+
                sizeof(float)*4*SLOZ_FLARE+
                sizeof(float)*2*SLOZ_FLARE+
                d;

  file_write(&ch,sizeof(ch),1,f);
  for(s = 0; s < SLOZ_FLARE; s++) {
    file_write(&p_flare->p_sloz[s].vzdal,sizeof(float),1,f);
    file_write(&p_flare->p_sloz[s].r,sizeof(float),4,f);
    file_write(&p_flare->p_sloz[s].dx,sizeof(float),2,f);
    file_write(pom[s],sizeof(byte),strlen(pom[s])+1,f);
  }
}
*/
/*

 **********************************************************************
   Ulozeni ambientu


void lo_chunk_save_ambient(FHANDLE f, dword ambient)
{ 
  DATA_CHUNK ch;    
  ch.typ = CHUNK_AMBIENT;
  ch.velikost = sizeof(ch)+sizeof(ambient);
  file_write(&ch,sizeof(ch),1,f);
  file_write(&ambient,sizeof(ambient),1,f);
}

 **********************************************************************
   Ulozeni ambientu


void lo_chunk_save_lc_ext(FHANDLE f, LEVEL_KONFIG *p_lc)
{ 
  DATA_CHUNK ch;    
  ch.typ = CHUNK_LEVEL_KONFIG_EXT;
  ch.velikost = sizeof(ch)+
                sizeof(p_lc->barva_pozadi)+
                sizeof(p_lc->barva_pozadi_aktivni)+
                sizeof(p_lc->zm.aktivni)+
                sizeof(p_lc->zm.v_start)+
                sizeof(p_lc->zm.v_stop)+
                sizeof(p_lc->zm.h_start)+
                sizeof(p_lc->zm.h_stop)+
                sizeof(p_lc->zm.barva);

  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_lc->barva_pozadi,sizeof(p_lc->barva_pozadi),1,f);
  file_write(&p_lc->barva_pozadi_aktivni,sizeof(p_lc->barva_pozadi_aktivni),1,f);
  file_write(&p_lc->zm.aktivni,sizeof(p_lc->zm.aktivni),1,f);
  file_write(&p_lc->zm.v_start,sizeof(p_lc->zm.v_start),1,f);
  file_write(&p_lc->zm.v_stop,sizeof(p_lc->zm.v_stop),1,f);
  file_write(&p_lc->zm.h_start,sizeof(p_lc->zm.h_start),1,f);
  file_write(&p_lc->zm.h_stop,sizeof(p_lc->zm.h_stop),1,f);
  file_write(&p_lc->zm.barva,sizeof(p_lc->zm.barva),1,f);
}

 **********************************************************************
   Ulozeni zrcadla


void lo_chunk_preved_zrcadlo(EDIT_KONTEJNER **p_kont, int kontnum, ZDRCADLO_DESC *p_desc)
{
  ZDRCADLO_DESC_POLY *p_tmp = p_desc->p_poly;  
  while(p_tmp) {
    int k = p_tmp->zrcadlo_k;
    if(k != K_CHYBA && k < kontnum && p_kont[k]) {
      p_tmp->id_kont = p_kont[k]->kontejner_ID;
    } else {
      p_tmp->id_kont = K_CHYBA;
    }
    p_tmp = p_tmp->p_next;
  }
  
}

void lo_chunk_save_zrcadlo_sub(FHANDLE f, ZDRCADLO_DESC_POLY *p_desc)
{ 
  DATA_CHUNK ch;
  ch.typ = CHUNK_ZRCADLO_3_POLOZKA;
  ch.velikost = sizeof(ch)+sizeof(p_desc[0]);
  file_write(&ch,sizeof(ch),1,f);
  file_write(p_desc,sizeof(p_desc[0]),1,f);
}

void lo_chunk_save_zrcadlo(FHANDLE f, ZDRCADLO_DESC *p_desc)
{ 
  ZDRCADLO_DESC_POLY *p_poly;
  DATA_CHUNK ch;
  
  ch.typ = CHUNK_ZRCADLO_3;
  ch.velikost = sizeof(ch)+sizeof(ZDRCADLO_DESC);
  
  file_write(&ch,sizeof(ch),1,f);
  file_write(p_desc,sizeof(ZDRCADLO_DESC),1,f);
  
  p_poly = p_desc->p_poly;
  while(p_poly) {
    lo_chunk_save_zrcadlo_sub(f, p_poly);
    p_poly = p_poly->p_next;
  }
}
*/
/*
 **********************************************************************
   Ulozeni svetel


int lo_chunk_save_static_light(FHANDLE f, STATIC_LIGHT *p_light, EDIT_MATERIAL **p_mat)
{
  LENS_FLARE *p_flare;
  DATA_CHUNK   ch;
  int j;

  ch.typ = CHUNK_STATIC_LIGHT;
  ch.velikost = sizeof(ch)+sizeof(p_light->akt)+strlen(p_light->name)+1+
                sizeof(p_light->index)+sizeof(p_light->p)+sizeof(float)*3+
                sizeof(float)*4+sizeof(p_light->uhel)+sizeof(p_light->utlum)
                +sizeof(p_light->flag)+sizeof(p_light->flag2);
  
  file_write(&ch,sizeof(ch),1,f);
  file_write(&p_light->akt,sizeof(p_light->akt),1,f);
  file_write(p_light->name,sizeof(byte),strlen(p_light->name)+1,f);
  file_write(&p_light->index,sizeof(p_light->index),1,f);
  file_write(&p_light->p,sizeof(p_light->p),1,f);
  file_write(&p_light->r,sizeof(float),1,f);
  file_write(&p_light->g,sizeof(float),1,f);
  file_write(&p_light->b,sizeof(float),1,f);  
  file_write(&p_light->k1,sizeof(float),4,f);
  file_write(&p_light->uhel,sizeof(p_light->uhel),1,f);
  file_write(&p_light->utlum,sizeof(p_light->utlum),1,f);
  file_write(&p_light->flag,sizeof(p_light->flag),1,f);
  file_write(&p_light->flag2,sizeof(p_light->flag2),1,f);
  
  p_flare = (LENS_FLARE *)p_light->p_flare;
  if(p_flare && p_flare->akt) {      
    lo_uloz_material_chunk(f, p_flare->p_mat);
    lo_chunk_save_flare(f,p_flare,p_mat,TRUE);
    if(p_flare->p_sloz) {
      for(j = 0; j < SLOZ_FLARE; j++) {
        if(p_flare->p_sloz[j].vzdal > 0.0f) 
          lo_uloz_material_chunk(f, p_flare->p_sloz[j].p_mat);
      }
      lo_chunk_save_flare_slozeny(f,p_flare,p_mat);
    }
  }

  return(TRUE);
}

int lo_chunk_save_static_light_mesh(FHANDLE f, STATIC_LIGHT *p_light, EDIT_MATERIAL **p_mat)
{
  LENS_FLARE *p_flare;
  DATA_CHUNK   ch;
  int         j;

  while(p_light) {
    
    // Ulozeni statickeho svetla    
    
    ch.typ = CHUNK_STATIC_LIGHT_MESH;
    ch.velikost = sizeof(ch)+sizeof(p_light->akt)+strlen(p_light->name)+1+
      sizeof(p_light->index)+sizeof(p_light->p)+sizeof(float)*3+
      sizeof(float)*4+sizeof(p_light->uhel)+sizeof(p_light->utlum)
      +sizeof(p_light->flag)+sizeof(p_light->flag2);
    
    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_light->akt,sizeof(p_light->akt),1,f);
    file_write(p_light->name,sizeof(byte),strlen(p_light->name)+1,f);
    file_write(&p_light->index,sizeof(p_light->index),1,f);
    file_write(&p_light->p,sizeof(p_light->p),1,f);
    file_write(&p_light->r,sizeof(float),1,f);
    file_write(&p_light->g,sizeof(float),1,f);
    file_write(&p_light->b,sizeof(float),1,f);  
    file_write(&p_light->k1,sizeof(float),4,f);
    file_write(&p_light->uhel,sizeof(p_light->uhel),1,f);
    file_write(&p_light->utlum,sizeof(p_light->utlum),1,f);
    file_write(&p_light->flag,sizeof(p_light->flag),1,f);
    file_write(&p_light->flag2,sizeof(p_light->flag2),1,f);
    
    // Ulozeni flaru ke statickemu svetlu
    
    p_flare = (LENS_FLARE *)p_light->p_flare;
    if(p_flare && p_flare->akt) {      
      lo_uloz_material_chunk(f, p_flare->p_mat);
      lo_chunk_save_flare_mesh(f,p_flare,p_mat);
      if(p_flare->p_sloz) {
        for(j = 0; j < SLOZ_FLARE; j++) {
          if(p_flare->p_sloz[j].vzdal > 0.0f) 
            lo_uloz_material_chunk(f, p_flare->p_sloz[j].p_mat);
        }
        lo_chunk_save_flare_slozeny(f,p_flare,p_mat);
      }
    }
        
    // Dalsi svetlo
    
    p_light = p_light->p_next;
  }
  return(TRUE);
}
*/
/*
 *********************************************************************
    Ukladani animaci


int lo_chunk_save_keyframe_root(FHANDLE f, HIERARCHY_ROOT *p_root)
{
  DATA_CHUNK ch;  
  if(p_root) {
    ch.typ = CHUNK_KEYFRAME_ROT2;
    ch.velikost = sizeof(ch)+sizeof(HIERARCHY_ROOT);
    file_write(&ch,sizeof(ch),1,f);
    file_write(p_root,sizeof(*p_root),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node(FHANDLE f, HIERARCHY_TRACK_INFO *p_node)
{
  char name[200];
  char otec[200];
  DATA_CHUNK ch;  

  if(p_node) {
    strcpy(name,((EDIT_OBJEKT *)(p_node->p_obj))->name);   
    strcpy(otec,p_node->p_otec ? ((EDIT_OBJEKT *)(p_node->p_otec->p_obj))->name : ROOT_NODE_JMENO);

    ch.typ = CHUNK_KEYFRAME_NODE;
    ch.velikost = sizeof(ch)+
                  sizeof(HIERARCHY_TRACK_INFO)+
                  strlen(name)+1+strlen(otec)+1;
    file_write(&ch,sizeof(ch),1,f);
    
    file_write(name,sizeof(byte),strlen(name)+1,f);
    file_write(otec,sizeof(byte),strlen(otec)+1,f);
    file_write(p_node,sizeof(*p_node),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node_pos(FHANDLE f, HIERARCHY_TRACK_INFO *p_node)
{
  DATA_CHUNK ch;  

  if(p_node) {
    ch.typ = CHUNK_KEYFRAME_POS;
    ch.velikost = sizeof(ch)+
                  sizeof(p_node->p_pos[0])*p_node->pos_keys+
                  sizeof(p_node->p_pkeys[0])*p_node->pos_keys;
    file_write(&ch,sizeof(ch),1,f);    
    file_write(p_node->p_pos,sizeof(p_node->p_pos[0]),p_node->pos_keys,f);
    file_write(p_node->p_pkeys,sizeof(p_node->p_pkeys[0]),p_node->pos_keys,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node_rot(FHANDLE f, HIERARCHY_TRACK_INFO *p_node)
{
  DATA_CHUNK ch;  

  if(p_node) {
    ch.typ = CHUNK_KEYFRAME_ROT;
    ch.velikost = sizeof(ch)+
                  sizeof(p_node->p_at[0])*p_node->rot_keys+
                  sizeof(p_node->p_rkeys[0])*p_node->rot_keys;
    file_write(&ch,sizeof(ch),1,f);    
    file_write(p_node->p_at,sizeof(p_node->p_at[0]),p_node->rot_keys,f);
    file_write(p_node->p_rkeys,sizeof(p_node->p_rkeys[0]),p_node->rot_keys,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node_scale(FHANDLE f, HIERARCHY_TRACK_INFO *p_node)
{
  DATA_CHUNK ch;  

  if(p_node) {
    ch.typ = CHUNK_KEYFRAME_SCS;
    ch.velikost = sizeof(ch)+
                  sizeof(p_node->p_scale[0])*p_node->scs_keys+
                  sizeof(p_node->p_skeys[0])*p_node->scs_keys;
    file_write(&ch,sizeof(ch),1,f);    
    file_write(p_node->p_scale,sizeof(p_node->p_scale[0]),p_node->scs_keys,f);
    file_write(p_node->p_skeys,sizeof(p_node->p_skeys[0]),p_node->scs_keys,f);
    return(TRUE);
  } else
    return(FALSE);
}

 Ulozi root vcetne vsech decek

int lo_chunk_save_keyframe_child(FHANDLE f, HIERARCHY_TRACK_INFO *p_node)
{
  int i;

  lo_chunk_save_keyframe_node(f,p_node);
  lo_chunk_save_keyframe_node_pos(f,p_node);  
  lo_chunk_save_keyframe_node_rot(f,p_node);
  lo_chunk_save_keyframe_node_scale(f,p_node);

  for(i = 0; i < p_node->childnum; i++)
    lo_chunk_save_keyframe_child(f,p_node->p_child[i]);
  
  return(TRUE);
}

int lo_chunk_save_keyframe(FHANDLE f, HIERARCHY_ROOT *p_root)
{
  int i;
  lo_chunk_save_keyframe_root(f,p_root);
  for(i = 0; i < p_root->childnum; i++) {
    lo_chunk_save_keyframe_child(f, p_root->p_child[i]);
  }
  return(TRUE);
}


***********************************************************************
  Ulozeni sim-animaci

  #define  CHUNK_SIM_ROOT      2020 // hlavicka key-framove animace
  #define  CHUNK_SIM_NODE      2021 // vetev animace - pro jeden objekt




typedef struct _HIERARCHY_SIM {
  
  float     delka;         // delka casu animace (fps)
  byte      stav;          // 0 = stop, 1 = run
  
  int       flag;          // flagy animace (GL_LOOP,GL_REMOVE)

  dword     time;          // aktualni cas animace
  dword     time_start;    // aktualni start
  dword     time_stop;     // aktualni stop
  dword     time_delka;    // aktualni delka animace
  
  dword     start;
  dword     stop;  
  
  int       keynum;         // pocet klicu
  byte      name[MAX_JMENO]; // name animace
  int       childnum;         // pocet detskych traku
  SIMPLE_TRACK_INFO *p_child;

} HIERARCHY_SIM;

int lo_chunk_save_sim_root(FHANDLE f, HIERARCHY_SIM *p_sim)
{
  DATA_CHUNK ch;  
  if(p_sim) {
    ch.typ = CHUNK_SIM_ROOT;
    ch.velikost = sizeof(ch)+
                  sizeof(p_sim->flag)+
                  sizeof(p_sim->keynum)+
                  sizeof(p_sim->name[0])*MAX_JMENO+
                  sizeof(p_sim->childnum);

    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_sim->flag,sizeof(p_sim->flag),1,f);
    file_write(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    file_write(&p_sim->name,sizeof(p_sim->name[0]),MAX_JMENO,f);
    file_write(&p_sim->childnum,sizeof(p_sim->childnum),1,f);    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_sim_node_ext_all_matrix(FHANDLE f, SIMPLE_TRACK_INFO *p_sim)
{ 
  if(p_sim && p_sim->p_all_matrix) {
    file_write(p_sim->p_all_matrix,sizeof(p_sim->p_all_matrix[0]),1,f);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int lo_chunk_save_sim_node_ext(FHANDLE f, SIMPLE_TRACK_INFO *p_sim)
{    
  DATA_CHUNK ch;  
  int       klicu,i;

  if(p_sim) {
    ch.typ = p_sim->p_all_matrix ? CHUNK_SIM_NODE_EXT2 : CHUNK_SIM_NODE_EXT;
    ch.velikost = sizeof(ch)+
      sizeof(p_sim->Objekt_ID)+
      sizeof(p_sim->flag)+
      sizeof(p_sim->keynum)+
      sizeof(p_sim->pivot)+
      sizeof(p_sim->pivot3ds)+
      sizeof(p_sim->childnum)+    
      sizeof(klicu)*3+(p_sim->p_pos ? sizeof(p_sim->p_pos[0])*p_sim->keynum:0)
      +(p_sim->p_scale ? sizeof(p_sim->p_scale[0])*p_sim->keynum : 0)
      +(p_sim->p_at ? sizeof(p_sim->p_at[0])*p_sim->keynum : 0)
      +(p_sim->p_norm ? sizeof(p_sim->p_norm[0])*p_sim->keynum : 0)
      +(p_sim->p_all_matrix ? sizeof(p_sim->p_all_matrix[0]) : 0);

    
    file_write(&ch,sizeof(ch),1,f);    
    file_write(&p_sim->Objekt_ID,sizeof(p_sim->Objekt_ID),1,f);
    file_write(&p_sim->flag,sizeof(p_sim->flag),1,f);
    file_write(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    file_write(&p_sim->pivot,sizeof(p_sim->pivot),1,f);
    file_write(&p_sim->pivot3ds,sizeof(p_sim->pivot3ds),1,f);
    file_write(&p_sim->childnum,sizeof(p_sim->childnum),1,f);

    klicu = p_sim->p_pos ? p_sim->keynum : 0;
    file_write(&klicu,sizeof(klicu),1,f);
    if(klicu)
      file_write(p_sim->p_pos,sizeof(p_sim->p_pos[0]),p_sim->keynum,f);
    
    klicu = p_sim->p_scale ? p_sim->keynum : 0;
    file_write(&klicu,sizeof(klicu),1,f);
    if(klicu)
      file_write(p_sim->p_scale,sizeof(p_sim->p_scale[0]),p_sim->keynum,f);
    
    klicu = p_sim->p_at ? p_sim->keynum : 0;
    file_write(&klicu,sizeof(klicu),1,f);
    if(klicu)
      file_write(p_sim->p_at,sizeof(p_sim->p_at[0]),p_sim->keynum,f);

    klicu = p_sim->p_norm ? p_sim->keynum : 0;
    file_write(&klicu,sizeof(klicu),1,f);
    if(klicu)
      file_write(p_sim->p_norm,sizeof(p_sim->p_norm[0]),p_sim->keynum,f);

    if(p_sim->p_all_matrix)
      lo_chunk_save_sim_node_ext_all_matrix(f, p_sim);

    for(i = 0; i < p_sim->childnum; i++)
      lo_chunk_save_sim_node_ext(f,p_sim->p_child+i);
    
    return(TRUE);
  } else
    return(FALSE);
}


  Ulozi koren sim animace

int lo_chunk_save_sim(FHANDLE f, HIERARCHY_SIM *p_sim)
{
  int i;
  lo_chunk_save_sim_root(f,p_sim);
  for(i = 0; i < p_sim->childnum; i++) {
    lo_chunk_save_sim_node_ext(f, p_sim->p_child+i);
  }
  return(TRUE);
}

int lo_uloz_materialy_chunk_kont(FHANDLE f, EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER *p_kont, int kontrola)
{
 int i,j;

  Uloz pouzite materialy
 
 for(i = 0; i < max_mat; i++) {
   if(p_mat[i]) {
     for(j = 0; j < p_kont->max_objektu; j++) {
       if(p_kont->p_obj[j] && p_kont->p_obj[j]->material == i) {
         if(!kontrola || !(p_mat[i]->flag&(MAT_POUZITY|MAT_SYSTEM)))
           lo_uloz_material_chunk(f, p_mat[i]);
         break;
       }
     }
   }
 }

 return(TRUE);
}

int lo_uloz_materialy_chunk_pouzite(FHANDLE f, EDIT_MATERIAL **p_mat, int max_mat)
{ 
 int i;

  Uloz pouzite materialy
 
 for(i = 0; i < max_mat; i++) {
   if(p_mat[i] && p_mat[i]->flag&(MAT_POUZITY|MAT_SYSTEM)) {
     lo_uloz_material_chunk(f, p_mat[i]);
   }
 }
 return(TRUE);
}

// Ulozi vsechny veci do .prj

int lo_uloz_projekt(EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER **p_kontlist, int kontnum, char *p_name, STATIC_LIGHT *p_light, int lightnum, ZDRCADLO_DESC *p_zrcadlo, LENS_FLARE *p_flarr, int flaremax, dword ambient, MLZNA_KOSTKA *p_mlha, DYN_LIGHT *p_ddlight, int ddlightnum, EXTRA_DYN_LIGHT *p_eelight, int eelightnum, LEVEL_ENVIRONMENT *p_env, NASTAVENI_KAMER *p_kam, LEVEL_KONFIG *p_lc, int level, int transf_static)
{
 EDIT_KONTEJNER *p_kont;
 FHANDLE f;
 int   i,j,kID;

  Preved scenu na jedinecna jmena
 
 //lo_kontlist_jedinecne_jmena(p_kontlist,kontnum);

 if((f = ffopen(p_name,"wb")) == NULL) {
   return(FALSE);
 }
 
  Ulozim ambientni svetlo sceny
 
 lo_chunk_save_ambient(f,ambient);

  Ulozeni konfigurace levelu - pokud je
 
 if(p_lc) {
   lo_chunk_save_lc_ext(f, p_lc);
 }

  Ulozeni pouzitych materialu
 
 lo_uloz_materialy_chunk_pouzite(f, p_mat, max_mat); 
 
  Uloz kontejnery
 
 kID = K_CHYBA;
 for(j = 0; j < kontnum; j++) {
   if((p_kont = p_kontlist[j])) {
     
     if(level)
       kID++;
     
     lo_uloz_materialy_chunk_kont(f, p_mat, max_mat, p_kont,TRUE);
     lo_chunk_save_kontejner(f, p_kont, p_mat, max_mat, FALSE, K_CHYBA, kID, transf_static);
     
     p_kont = p_kont->p_next;
     while(p_kont) {
       lo_uloz_materialy_chunk_kont(f, p_mat, max_mat, p_kont,TRUE);
       lo_chunk_save_kontejner(f, p_kont, p_mat, max_mat, TRUE, K_CHYBA, kID, transf_static);
       p_kont = p_kont->p_next;
     }
   }
 } 

  Staticka svetla sceny
 
 if(p_light) {
   for(i = 0; i < lightnum; i++) {
     if(p_light[i].akt && !(p_light[i].flag2&SL2_MESH))
       lo_chunk_save_static_light(f,p_light+i,p_mat);
   }
 }

  Uloz dynamicka svetla sceny
 
 if(p_ddlight) {
   for(i = 0; i < ddlightnum; i++)
     if(p_ddlight[i].akt && !(p_ddlight[i].flag&SDL_MESH))
       lo_chunk_save_dyn_light(f, p_ddlight+i);
 }

  Extra a efektova svetla sceny
 
 if(p_eelight) {
   for(i = 0; i < eelightnum; i++)
     if(p_eelight[i].akt && !(p_eelight[i].flag&EDL_MESH))
       lo_chunk_save_extra_light(f, p_eelight+i);
 }

  Zrcadlo
 
 if(p_zrcadlo) {   
   lo_chunk_preved_zrcadlo(p_kontlist, kontnum, p_zrcadlo);
   lo_chunk_save_zrcadlo(f,p_zrcadlo);
 }

  Flare efekty + jejich materialy (pouze ze sceny)
  
 for(i = 0; i < flaremax; i++) {
   if(p_flarr[i].akt && !p_flarr[i].p_svetlo) {
     lo_uloz_material_chunk(f, p_flarr[i].p_mat);
     lo_chunk_save_flare(f,p_flarr+i,p_mat,FALSE);
     if(p_flarr[i].p_sloz) {
       for(j = 0; j < SLOZ_FLARE; j++) {
         if(p_flarr[i].p_sloz[j].vzdal > 0.0f) 
           lo_uloz_material_chunk(f, p_flarr[i].p_sloz[j].p_mat);
       }
       lo_chunk_save_flare_slozeny(f,p_flarr+i,p_mat);
     }
   }     
 }
 
  Mlho-kostku
 
 lo_chunk_save_mlhokostku(f, p_mlha, FALSE);

  Ulozeni environmentu
 
 if(p_env) {
   lo_chunk_save_environment(f, p_env);
 }

  Ulozeni nastaveni kamer
 
 if(p_kam) {
   lo_chunk_save_kamset(f, p_kam);
   lo_chunk_save_kamset_2(f, p_kam);
 }

 ffclose(f);
 return(j);
}



***********************************************************************
  Loading objektu/materialu pres chunk
  Globalni promenne pro loading


 Nabinduje se na p_cnf->p_mat

static int                matlistnum;    // pocet nactenych materialu
static EDIT_MATERIAL    **p_matlist;     // aktualni material
static ANIM_MATERIAL     *p_fanim;        // aktualni frame animace

static int                textlistnum;   // list textur
static EDIT_TEXT         *p_textlist;    // list textur

static EDIT_KONTEJNER   **p_kontlist;   // seznam kontejneru
static int                kontlistnum;   // max pocet kontejneru

static int                kont_load;
static EDIT_KONTEJNER    *p_kont_old; // stary kontejner
static EDIT_KONTEJNER    *p_kont; // aktualni kontejner
static EDIT_MATERIAL     *p_mat;  // aktualni material
static EDIT_OBJEKT       *p_obj;  // aktualni objekt

static SIMPLE_TRACK_INFO *p_sim_node; // aktivni loadovaci node
static HIERARCHY_SIM     *p_sim;      // aktivni loadovaci root

static STATIC_LIGHT      *p_lightlist;
static STATIC_LIGHT      *p_lightakt; // aktualni staticke svetlo
static int                lightlistnum;

static STATIC_LIGHT      *p_light_mesh;


static ZDRCADLO_DESC     *p_zrcadlolist;
static int               *p_zrcadloload;

static LENS_FLARE        *p_flarelist;  // seznam flaru
static LENS_FLARE        *p_flare;      // posledni nahrany flare
static int                flarelistnum;

static MLZNA_KOSTKA      *p_top_mlha;

static dword             *p_ambient;

static DYN_LIGHT         *p_dlist;  // seznam dynamickych svetel
static int                dlistnum;

static EXTRA_DYN_LIGHT   *p_elist;  // seznam extra svetel
static int                elistnum;

static LEVEL_ENVIRONMENT *p_lenv;
static NASTAVENI_KAMER   *p_kamset;

static LEVEL_KONFIG      *p_lc;

static JOINT             *p_joint;
static JOINT_ANIMACE     *p_joint_animace;

 **********************************************************************
   Loading environmentu

int lo_chunk_load_environment(FHANDLE f, DATA_CHUNK *p_ch)
{     
  if(p_ch->typ == CHUNK_ENVIRONMENT) {
    if(p_lenv) {
      file_read(p_lenv,sizeof(p_lenv[0]),1,f);
    } else {
      LEVEL_ENVIRONMENT env;
      file_read(&env,sizeof(env),1,f);
    }
    return(TRUE);
  }
  else
    return(FALSE);
}

 **********************************************************************
   Loading kamsetu

int lo_chunk_load_kamset(FHANDLE f, DATA_CHUNK *p_ch)
{     
  if(p_kamset && p_ch->typ == CHUNK_KAMSET) {    
    file_read(&p_kamset->far_plane,sizeof(p_kamset->far_plane),1,f);
    file_read(&p_kamset->fov,sizeof(p_kamset->fov),1,f);
    file_read(&p_kamset->max_uhel,sizeof(p_kamset->max_uhel),1,f);
    file_read(&p_kamset->min_uhel,sizeof(p_kamset->min_uhel),1,f);
    file_read(&p_kamset->min_vzdal,sizeof(p_kamset->min_vzdal),1,f);
    file_read(&p_kamset->max_vzdal,sizeof(p_kamset->max_vzdal),1,f);
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_kamset_2(FHANDLE f, DATA_CHUNK *p_ch)
{     
  if(p_kamset && p_ch->typ == CHUNK_KAMSET_2) {
    file_read(&p_kamset->max,sizeof(p_kamset->max),1,f);
    file_read(&p_kamset->min,sizeof(p_kamset->min),1,f);
    return(TRUE);
  }
  else
    return(FALSE);
}

 **********************************************************************
   Loading dynamickeho svetla

int lo_chunk_load_dyn_light_anim(FHANDLE f, DYN_LIGHT_ANIM *p_an, int extra)
{  
  
  file_read(&p_an->pos_keys,sizeof(p_an->pos_keys),1,f);
  if(p_an->pos_keys) {
    p_an->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_an->p_pkeys[0])*p_an->pos_keys);
    file_read(p_an->p_pkeys,sizeof(p_an->p_pkeys[0]),p_an->pos_keys,f);    

    p_an->p_pos = (BOD *)mmalloc(sizeof(p_an->p_pos[0])*p_an->pos_keys);
    file_read(p_an->p_pos,sizeof(p_an->p_pos[0]),p_an->pos_keys,f);
  } else {
    p_an->p_pkeys = NULL;
    p_an->p_pos = NULL;
  }
    
  file_read(&p_an->trg_keys,sizeof(p_an->trg_keys),1,f);
  if(p_an->trg_keys) {
    p_an->p_tkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_an->p_tkeys[0])*p_an->trg_keys);
    file_read(p_an->p_tkeys,sizeof(p_an->p_tkeys[0]),p_an->trg_keys,f);

    p_an->p_trg = (BOD *)mmalloc(sizeof(p_an->p_trg[0])*p_an->trg_keys);
    file_read(p_an->p_trg,sizeof(p_an->p_trg[0]),p_an->trg_keys,f);
  } else {
    p_an->p_tkeys = NULL;
    p_an->p_trg = NULL;
  }
  
  file_read(&p_an->dos_keys,sizeof(p_an->dos_keys),1,f);
  if(p_an->dos_keys) {
    p_an->p_dskeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_an->p_dskeys[0])*p_an->dos_keys);
    file_read(p_an->p_dskeys,sizeof(p_an->p_dskeys[0]),p_an->dos_keys,f);

    p_an->p_dosah = (BOD *)mmalloc(sizeof(p_an->p_dosah[0])*p_an->dos_keys);
    file_read(p_an->p_dosah,sizeof(p_an->p_dosah[0]),p_an->dos_keys,f);
  } else {
    p_an->p_dskeys = NULL;
    p_an->p_dosah = NULL;
  }
  
  if(extra) {    
    file_read(&p_an->alfa_keys,sizeof(p_an->alfa_keys),1,f);
    
    p_an->diff_keys = 0;
    p_an->p_dfkeys = NULL;
    p_an->p_diff = NULL;

    if(p_an->alfa_keys) {
      int i;

      p_an->p_akeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_an->p_akeys[0])*p_an->alfa_keys);
      file_read(p_an->p_akeys,sizeof(p_an->p_akeys[0]),p_an->alfa_keys,f);
      
      p_an->p_diff = (BARVA_RGBA *)mmalloc(sizeof(p_an->p_diff[0])*p_an->alfa_keys);
      file_read(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->alfa_keys,f);

      p_an->p_alfa = (float *)mmalloc(sizeof(p_an->p_alfa[0])*p_an->alfa_keys);
      for(i = 0; i < p_an->alfa_keys; i++) {
        p_an->p_alfa[i] = p_an->p_diff[i].a;
      } 
      
      null_free(p_an->p_diff);
    } else {
      p_an->p_akeys = NULL;
      p_an->p_alfa = NULL;
    }
  } else {
    file_read(&p_an->diff_keys,sizeof(p_an->diff_keys),1,f);
    
    p_an->alfa_keys = 0;
    p_an->p_akeys = NULL;
    p_an->p_alfa = NULL;    
    
    if(p_an->diff_keys) {
      p_an->p_dfkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_an->p_dfkeys[0])*p_an->diff_keys);
      file_read(p_an->p_dfkeys,sizeof(p_an->p_dfkeys[0]),p_an->diff_keys,f);
      
      p_an->p_diff = (BARVA_RGBA *)mmalloc(sizeof(p_an->p_diff[0])*p_an->diff_keys);
      file_read(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->diff_keys,f);
    } else {
      p_an->p_dfkeys = NULL;
      p_an->p_diff = NULL;
    }
  }
  
  file_read(&p_an->spec_keys,sizeof(p_an->spec_keys),1,f);
  if(p_an->spec_keys) {
    p_an->p_spkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_an->p_spkeys[0])*p_an->spec_keys);
    file_read(p_an->p_spkeys,sizeof(p_an->p_spkeys[0]),p_an->spec_keys,f);

    p_an->p_spec = (BARVA_RGBA *)mmalloc(sizeof(p_an->p_spec[0])*p_an->spec_keys);
    file_read(p_an->p_spec,sizeof(p_an->p_spec[0]),p_an->spec_keys,f);
  } else {
    p_an->p_spkeys = NULL;
    p_an->p_spec = NULL;
  }
  
  return(TRUE);
}

int lo_chunk_load_dyn_light(FHANDLE f, DATA_CHUNK *p_ch)
{
  DYN_LIGHT *p_light;
  
  if(p_ch->typ == CHUNK_DYNAMIC_LIGHT && p_dlist) {
    p_light = lo_najdi_volne_dsvetlo_point(p_dlist,dlistnum);
    if(p_light) {
      file_read(&p_light->akt,sizeof(p_light->akt),1,f);
      str_read(p_light->name,f);
      file_read(&p_light->np,sizeof(p_light->np),1,f);
      file_read(&p_light->dir,sizeof(p_light->dir),1,f);
      file_read(&p_light->look_max,sizeof(p_light->look_max),1,f);
      file_read(&p_light->dosah,sizeof(p_light->dosah),1,f);
      file_read(&p_light->min,sizeof(p_light->min),1,f);
      file_read(&p_light->max,sizeof(p_light->max),1,f);
      file_read(&p_light->theta,sizeof(p_light->theta),1,f);
      file_read(&p_light->dr,sizeof(p_light->dr),1,f);
      file_read(&p_light->dg,sizeof(p_light->dg),1,f);
      file_read(&p_light->db,sizeof(p_light->db),1,f);
      file_read(&p_light->sr,sizeof(p_light->sr),1,f);
      file_read(&p_light->sg,sizeof(p_light->sg),1,f);
      file_read(&p_light->sb,sizeof(p_light->sb),1,f);
      file_read(&p_light->flag,sizeof(p_light->flag),1,f);
      file_read(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
      file_read(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
      file_read(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
      p_light->tp = p_light->np;
      if(p_light->an.endtime) {
        lo_chunk_load_dyn_light_anim(f, &p_light->an,FALSE);
      }
      return(TRUE);
    } else
      return(FALSE);
  } else
    return(FALSE);
}

int lo_chunk_load_dyn_light_mesh(FHANDLE f, DATA_CHUNK *p_ch)
{
  DYN_LIGHT *p_light;
  
  if(p_kont && p_ch->typ == CHUNK_DYNAMIC_LIGHT_MESH) {
    p_light = (DYN_LIGHT *)mmalloc(sizeof(p_light[0]));
    file_read(&p_light->akt,sizeof(p_light->akt),1,f);
    str_read(p_light->name,f);
    file_read(&p_light->np,sizeof(p_light->np),1,f);
    file_read(&p_light->dir,sizeof(p_light->dir),1,f);
    file_read(&p_light->look_max,sizeof(p_light->look_max),1,f);
    file_read(&p_light->dosah,sizeof(p_light->dosah),1,f);
    file_read(&p_light->min,sizeof(p_light->min),1,f);
    file_read(&p_light->max,sizeof(p_light->max),1,f);
    file_read(&p_light->theta,sizeof(p_light->theta),1,f);
    file_read(&p_light->dr,sizeof(p_light->dr),1,f);
    file_read(&p_light->dg,sizeof(p_light->dg),1,f);
    file_read(&p_light->db,sizeof(p_light->db),1,f);
    file_read(&p_light->sr,sizeof(p_light->sr),1,f);
    file_read(&p_light->sg,sizeof(p_light->sg),1,f);
    file_read(&p_light->sb,sizeof(p_light->sb),1,f);
    file_read(&p_light->flag,sizeof(p_light->flag),1,f);
    file_read(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    file_read(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    file_read(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
    
    p_light->tp = p_light->np;
    
    if(p_light->an.endtime) {
      lo_chunk_load_dyn_light_anim(f, &p_light->an,FALSE);
    }

    p_light->p_mesh_data = p_kont;
    p_light->p_next = p_kont->p_dlight;
    p_light->p_prev = NULL;
    p_kont->p_dlight = p_light;
    if(p_light->p_next) {
      p_light->p_next->p_prev = p_light;
    }

    return(TRUE);
  } else
    return(FALSE);
}

 **************************************************************
     Nahraje Extra svetla
   **************************************************************

int lo_chunk_load_extra_light(FHANDLE f, DATA_CHUNK *p_ch)
{
  EXTRA_DYN_LIGHT *p_light;
  
  if(p_ch->typ == CHUNK_EXTRA_LIGHT && p_elist) {
    p_light = lo_najdi_volne_extra_svetlo_point(p_elist,elistnum);
    if(p_light) {
      file_read(&p_light->akt,sizeof(p_light->akt),1,f);
      str_read(p_light->name,f);
      file_read(&p_light->flag,sizeof(p_light->flag),1,f);
      file_read(&p_light->mflag,sizeof(p_light->mflag),1,f);
      file_read(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
      file_read(&p_light->np,sizeof(p_light->np),1,f);
      file_read(&p_light->dir,sizeof(p_light->dir),1,f);
      file_read(&p_light->dosah,sizeof(p_light->dosah),1,f);
      file_read(&p_light->theta,sizeof(p_light->theta),1,f);
      file_read(&p_light->utlum,sizeof(p_light->utlum),1,f);
      file_read(&p_light->gourand,sizeof(p_light->gourand),1,f);
      file_read(&p_light->da,sizeof(p_light->da),1,f);
      file_read(&p_light->da,sizeof(p_light->da),1,f);
      file_read(&p_light->da,sizeof(p_light->da),1,f);
      file_read(&p_light->da,sizeof(p_light->da),1,f);
      file_read(&p_light->sr,sizeof(p_light->sr),1,f);
      file_read(&p_light->sg,sizeof(p_light->sg),1,f);
      file_read(&p_light->sb,sizeof(p_light->sb),1,f);
      file_read(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
      file_read(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
      p_light->tp = p_light->np;
      if(p_light->an.endtime) {
        lo_chunk_load_dyn_light_anim(f, &p_light->an,TRUE);
      }
      return(TRUE);
    } else
      return(FALSE);
  } else
    return(FALSE);
}

int lo_chunk_load_extra_light_mesh(FHANDLE f, DATA_CHUNK *p_ch)
{
  EXTRA_DYN_LIGHT *p_light;
  
  if(p_kont && p_ch->typ == CHUNK_EXTRA_LIGHT_MESH) {
    p_light = (EXTRA_DYN_LIGHT *)mmalloc(sizeof(p_light[0]));
    file_read(&p_light->akt,sizeof(p_light->akt),1,f);
    str_read(p_light->name,f);
    file_read(&p_light->flag,sizeof(p_light->flag),1,f);
    file_read(&p_light->mflag,sizeof(p_light->mflag),1,f);
    file_read(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    file_read(&p_light->np,sizeof(p_light->np),1,f);
    file_read(&p_light->dir,sizeof(p_light->dir),1,f);
    file_read(&p_light->dosah,sizeof(p_light->dosah),1,f);
    file_read(&p_light->theta,sizeof(p_light->theta),1,f);
    file_read(&p_light->utlum,sizeof(p_light->utlum),1,f);
    file_read(&p_light->gourand,sizeof(p_light->gourand),1,f);
    file_read(&p_light->da,sizeof(p_light->da),1,f);
    file_read(&p_light->da,sizeof(p_light->da),1,f);
    file_read(&p_light->da,sizeof(p_light->da),1,f);
    file_read(&p_light->da,sizeof(p_light->da),1,f);
    file_read(&p_light->sr,sizeof(p_light->sr),1,f);
    file_read(&p_light->sg,sizeof(p_light->sg),1,f);
    file_read(&p_light->sb,sizeof(p_light->sb),1,f);
    file_read(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    file_read(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);   

    p_light->tp = p_light->np;
    
    if(p_light->an.endtime) {
      lo_chunk_load_dyn_light_anim(f, &p_light->an,TRUE);
    }

    p_light->p_mesh_data = p_kont;
    p_light->p_next = p_kont->p_edlight;
    p_light->p_prev = NULL;
    p_kont->p_edlight = p_light;
    if(p_light->p_next) {
      p_light->p_next->p_prev = p_light;
    }
    return(TRUE);
  } else
    return(FALSE);
}

 ***********************************************************************
   Loading mlho-kostek

int lo_chunk_load_mlho_kostek(FHANDLE f, DATA_CHUNK *p_ch)
{   
  MLZNA_KOSTKA    *p_top;

  if(p_ch->typ == CHUNK_MLHOKOSTKA) {
    p_top = vyrob_mlhokostku("");
    str_read(p_top->name,f);
    file_read(&p_top->min,sizeof(BOD),1,f);
    file_read(&p_top->max,sizeof(BOD),1,f);
    file_read(&p_top->flag,sizeof(p_top->flag),1,f);
    file_read(&p_top->mod,sizeof(p_top->mod),1,f);
    file_read(&p_top->priorita,sizeof(p_top->priorita),1,f);
    file_read(&p_top->start,sizeof(p_top->start),1,f);
    file_read(&p_top->stop,sizeof(p_top->stop),1,f);
    file_read(&p_top->intenzita,sizeof(p_top->intenzita),1,f);
    file_read(&p_top->r,sizeof(float)*4,1,f);
    p_top->p_next = p_top_mlha;
    p_top_mlha = p_top;
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_mlho_kostek_2(FHANDLE f, DATA_CHUNK *p_ch)
{   
  MLZNA_KOSTKA    *p_top;
  int              kid;

  if(p_ch->typ == CHUNK_MLHOKOSTKA_2) {
    p_top = vyrob_mlhokostku("");
    str_read(p_top->name,f);
    file_read(&p_top->min,sizeof(BOD),1,f);
    file_read(&p_top->max,sizeof(BOD),1,f);
    file_read(&p_top->flag,sizeof(p_top->flag),1,f);
    file_read(&p_top->mod,sizeof(p_top->mod),1,f);
    file_read(&p_top->priorita,sizeof(p_top->priorita),1,f);
    file_read(&p_top->start,sizeof(p_top->start),1,f);
    file_read(&p_top->stop,sizeof(p_top->stop),1,f);
    file_read(&p_top->intenzita,sizeof(p_top->intenzita),1,f);
    file_read(&p_top->r,sizeof(float)*4,1,f);
    
    file_read(&kid,sizeof(int),1,f);
    if(kid != K_CHYBA) {
      p_kont->p_mlha = p_top;
      p_top->p_kont = p_kont;
    }
    
    file_read(&p_top->poly,sizeof(p_top->poly),1,f);

    p_top->p_next = p_top_mlha;
    p_top_mlha = p_top;

    return(TRUE);
  }
  else
    return(FALSE);
}

 ***********************************************************************
   Loading flaru

int lo_chunk_load_flare(FHANDLE f, DATA_CHUNK *p_ch)
{
  char pom[200];
  int  i,nic;

  if(p_flarelist && p_ch->typ == CHUNK_FLARE) {

    // najiti flare
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;
    memset(p_flare,0,sizeof(p_flare[0]));
    file_read(&p_flare->p,sizeof(BOD),1,f);
    file_read(&p_flare->r,sizeof(float),4,f);
    file_read(&p_flare->dx,sizeof(float),2,f);
    file_read(&nic,sizeof(int),1,f);
    file_read(&p_flare->alfa,sizeof(int),1,f);
    
    p_flare->index = i;
        
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;    
    p_flare->akt = TRUE;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_ext(FHANDLE f, DATA_CHUNK *p_ch)
{   
  char pom[200];
  int  i,nic;

  if(p_flarelist && p_ch->typ == CHUNK_FLARE_EXT) {

    // najiti flare
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;
    memset(p_flare,0,sizeof(p_flare[0]));
    file_read(&p_flare->p,sizeof(BOD),1,f);
    file_read(&p_flare->r,sizeof(float),4,f);
    file_read(&p_flare->dx,sizeof(float),2,f);
    file_read(&nic,sizeof(int),1,f);
    file_read(&p_flare->alfa,sizeof(int),1,f);
    file_read(&p_flare->zflag,sizeof(int),1,f);
    
    p_flare->index = i;
    
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;
    p_flare->akt = TRUE;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_light(FHANDLE f, DATA_CHUNK *p_ch)
{   
  char pom[200];
  int  i,nic;
 
  if(p_flarelist && p_lightakt && p_ch->typ == CHUNK_FLARE_LIGHT) {
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;

    file_read(&p_flare->p,sizeof(BOD),1,f);
    file_read(&p_flare->r,sizeof(float),4,f);
    file_read(&p_flare->dx,sizeof(float),2,f);
    file_read(&nic,sizeof(int),1,f);
    file_read(&p_flare->alfa,sizeof(int),1,f);
    
    p_flare->index = i;
    
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;    
    p_flare->akt = TRUE;

    p_flare->p_svetlo = p_lightakt;
    p_lightakt->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_light_ext(FHANDLE f, DATA_CHUNK *p_ch)
{   
  char pom[200];
  int  i,nic;
 
  if(p_flarelist && p_lightakt && p_ch->typ == CHUNK_FLARE_LIGHT_EXT) {
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;

    file_read(&p_flare->p,sizeof(BOD),1,f);
    file_read(&p_flare->r,sizeof(float),4,f);
    file_read(&p_flare->dx,sizeof(float),2,f);
    file_read(&nic,sizeof(int),1,f);
    file_read(&p_flare->alfa,sizeof(int),1,f);
    file_read(&p_flare->zflag,sizeof(int),1,f);
 
    p_flare->index = i;
    
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;    
    p_flare->akt = TRUE;

    p_flare->p_svetlo = p_lightakt;
    p_lightakt->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_mesh(FHANDLE f, DATA_CHUNK *p_ch)
{   
  char pom[200];
  int  nic;

  if(p_light_mesh && p_ch->typ == CHUNK_FLARE_MESH) {

    // vyroba flare
    p_flare = (LENS_FLARE *)mmalloc(sizeof(p_flare[0]));

    file_read(&p_flare->p,sizeof(BOD),1,f);
    file_read(&p_flare->r,sizeof(float),4,f);
    file_read(&p_flare->dx,sizeof(float),2,f);    
    file_read(&nic,sizeof(int),1,f);
    file_read(&p_flare->alfa,sizeof(int),1,f);
           
    p_flare->akt = TRUE;
    p_flare->p_svetlo = p_light_mesh;

    p_flare->p_next = (LENS_FLARE *)p_light_mesh->p_flare;
    p_light_mesh->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {      
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_mesh_ext(FHANDLE f, DATA_CHUNK *p_ch)
{   
  char pom[200];
  int  nic;

  if(p_light_mesh && p_ch->typ == CHUNK_FLARE_MESH_EXT) {

    // vyroba flare
    p_flare = (LENS_FLARE *)mmalloc(sizeof(p_flare[0]));

    file_read(&p_flare->p,sizeof(BOD),1,f);
    file_read(&p_flare->r,sizeof(float),4,f);
    file_read(&p_flare->dx,sizeof(float),2,f);    
    file_read(&nic,sizeof(int),1,f);
    file_read(&p_flare->alfa,sizeof(int),1,f);
    file_read(&p_flare->zflag,sizeof(int),1,f);
           
    p_flare->akt = TRUE;
    p_flare->p_svetlo = p_light_mesh;

    p_flare->p_next = (LENS_FLARE *)p_light_mesh->p_flare;
    p_light_mesh->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {      
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

// Ulozeni slozeneho flare
int lo_chunk_load_flare_slozeny(FHANDLE f, DATA_CHUNK *p_ch)
{
  char      pom[200];
  int       s;

  if(p_flare && p_ch->typ == CHUNK_FLARE_SLOZENY) {
    p_flare->p_sloz = (LENS_FLARE_SLOZ *)mmalloc(sizeof(p_flare->p_sloz[0])*SLOZ_FLARE);

    for(s = 0; s < SLOZ_FLARE; s++) {
      file_read(&p_flare->p_sloz[s].vzdal,sizeof(float),1,f);
      file_read(&p_flare->p_sloz[s].r,sizeof(float),4,f);
      file_read(&p_flare->p_sloz[s].dx,sizeof(float),2,f);
      str_read(pom,f);
      if(p_flare->p_sloz[s].vzdal > 0.0f) {
        if((p_flare->p_sloz[s].material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
          kprintf(1,"Cannot find material %s",pom);
          assert(0);
        }
        p_flare->p_sloz[s].p_mat = (p_flare->p_sloz[s].material < matlistnum) ? p_matlist[p_flare->p_sloz[s].material] : NULL;
      } else {
        p_flare->p_sloz[s].p_mat = NULL;
        p_flare->p_sloz[s].material = 0;
      }
    }
    return(TRUE);
  } else
    return(FALSE);
}

 ***********************************************************************
   Loading ambientu

int lo_chunk_load_ambient(FHANDLE f, DATA_CHUNK *p_ch)
{ 
  if(p_ambient && p_ch->typ == CHUNK_AMBIENT) {    
    file_read(p_ambient,sizeof(*p_ambient),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

 ***********************************************************************
   Loading ambientu

int lo_chunk_load_levelconfig(FHANDLE f, DATA_CHUNK *p_ch)
{ 
  if(p_ch->typ == CHUNK_LEVEL_KONFIG_EXT) {    
    file_read(&p_lc->barva_pozadi,sizeof(p_lc->barva_pozadi),1,f);
    file_read(&p_lc->barva_pozadi_aktivni,sizeof(p_lc->barva_pozadi_aktivni),1,f);
    file_read(&p_lc->zm.aktivni,sizeof(p_lc->zm.aktivni),1,f);
    file_read(&p_lc->zm.v_start,sizeof(p_lc->zm.v_start),1,f);
    file_read(&p_lc->zm.v_stop,sizeof(p_lc->zm.v_stop),1,f);
    file_read(&p_lc->zm.h_start,sizeof(p_lc->zm.h_start),1,f);
    file_read(&p_lc->zm.h_stop,sizeof(p_lc->zm.h_stop),1,f);
    file_read(&p_lc->zm.barva,sizeof(p_lc->zm.barva),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

 ***********************************************************************
   Loading zrcadla


int lo_chunk_load_zrcadlo(FHANDLE f, DATA_CHUNK *p_ch)
{     
  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO) {
    *p_zrcadloload = TRUE;
    file_read(&p_zrcadlolist->zrcadlo_k,sizeof(p_zrcadlolist->zrcadlo_k),1,f);
    file_read(&p_zrcadlolist->zrcadlo_o,sizeof(p_zrcadlolist->zrcadlo_o),1,f);
    file_read(&p_zrcadlolist->poly,sizeof(p_zrcadlolist->poly),1,f);
    file_read(&p_zrcadlolist->ref,sizeof(p_zrcadlolist->ref),1,f);
    file_read(p_zrcadlolist->p,sizeof(p_zrcadlolist->p[0]),4,f);
    file_read(p_zrcadlolist->r,sizeof(p_zrcadlolist->r[0]),5,f);    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_zrcadlo_2(FHANDLE f, DATA_CHUNK *p_ch)
{     
  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO_2) {
    *p_zrcadloload = TRUE;
    file_read(p_zrcadlolist,sizeof(ZDRCADLO_DESC),1,f);
    return(TRUE);
  } else
    return(FALSE);
}


int lo_chunk_load_zrcadlo_3_sub(FHANDLE f, DATA_CHUNK *p_ch)
{   
  ZDRCADLO_DESC_POLY *p_poly;

  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO_3_POLOZKA) {
    p_poly = (ZDRCADLO_DESC_POLY *)mmalloc(sizeof(p_poly[0]));
    file_read(p_poly,sizeof(p_poly[0]),1,f);
    p_poly->p_next = p_zrcadlolist->p_poly;
    p_zrcadlolist->p_poly = p_poly;
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_zrcadlo_3(FHANDLE f, DATA_CHUNK *p_ch)
{     
  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO_3) {
    *p_zrcadloload = TRUE;
    
    file_read(p_zrcadlolist,sizeof(ZDRCADLO_DESC),1,f);    
    p_zrcadlolist->p_poly = NULL;

    return(TRUE);
  } else
    return(FALSE);
}

 ***********************************************************************
   Loading statickych svetel


int lo_chunk_load_static_light(FHANDLE f, DATA_CHUNK *p_ch)
{  
  STATIC_LIGHT light;
  if(p_lightlist && p_ch->typ == CHUNK_STATIC_LIGHT) {
    
    memset(&light,0,sizeof(light));    

    file_read(&light.akt,sizeof(light.akt),1,f);    
    str_read(light.name,f);
    file_read(&light.index,sizeof(light.index),1,f);
    
    file_read(&light.p,sizeof(light.p),1,f);
    file_read(&light.r,sizeof(float),1,f);
    file_read(&light.g,sizeof(float),1,f);
    file_read(&light.b,sizeof(float),1,f);
    file_read(&light.k1,sizeof(float),4,f);
    file_read(&light.uhel,sizeof(light.uhel),1,f);
    file_read(&light.utlum,sizeof(light.utlum),1,f);
    file_read(&light.flag,sizeof(light.flag),1,f);
    file_read(&light.flag2,sizeof(light.flag2),1,f);

    if(p_lightlist[light.index].akt) {
      light.index = lo_najdi_volne_stat_svetlo(p_lightlist,lightlistnum);
      if(light.index != K_CHYBA) {
        p_lightlist[light.index] = light;
      }
    } else
      p_lightlist[light.index] = light;
    
    p_lightakt = p_lightlist+light.index;
          
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_static_light_mesh(FHANDLE f, DATA_CHUNK *p_ch)
{    
  STATIC_LIGHT *p_light;

  if(p_kont && p_ch->typ == CHUNK_STATIC_LIGHT_MESH) {
    p_light = (STATIC_LIGHT *)mmalloc(sizeof(p_light[0]));
    
    file_read(&p_light->akt,sizeof(p_light->akt),1,f);
    str_read(p_light->name,f);
    file_read(&p_light->index,sizeof(p_light->index),1,f);
    
    file_read(&p_light->p,sizeof(p_light->p),1,f);
    file_read(&p_light->r,sizeof(float),1,f);
    file_read(&p_light->g,sizeof(float),1,f);
    file_read(&p_light->b,sizeof(float),1,f);
    file_read(&p_light->k1,sizeof(float),4,f);
    file_read(&p_light->uhel,sizeof(p_light->uhel),1,f);
    file_read(&p_light->utlum,sizeof(p_light->utlum),1,f);
    file_read(&p_light->flag,sizeof(p_light->flag),1,f);
    file_read(&p_light->flag2,sizeof(p_light->flag2),1,f);
    p_light->flag2 |= SL2_MESH;

    p_light->p_mesh_data = p_kont;
    p_light->p_next = p_kont->p_slight;
    p_light->p_prev = NULL;
    p_kont->p_slight = p_light;
    if(p_light->p_next) {
      p_light->p_next->p_prev = p_light;
    }

    p_light_mesh = p_light;

    return(TRUE);
  } else
    return(FALSE);
}

*****************************************************************************
  Load sim animaci

inline int lo_najdi_volny_sim(HIERARCHY_SIM *p_sim, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(!p_sim[i].keynum)
      return(i);
  }
  return(K_CHYBA);
}

int lo_chunk_load_sim_node(FHANDLE f, SIMPLE_TRACK_INFO *p_sim)
{    
  DATA_CHUNK ch;  
  int       klicu,i;

  file_read(&ch,sizeof(ch),1,f);

  if(p_sim && (ch.typ == CHUNK_SIM_NODE || ch.typ == CHUNK_SIM_NODE_EXT || ch.typ == CHUNK_SIM_NODE_EXT2)) {
    
    memset(p_sim,0,sizeof(p_sim[0]));

    file_read(&p_sim->Objekt_ID,sizeof(p_sim->Objekt_ID),1,f);
    file_read(&p_sim->flag,sizeof(p_sim->flag),1,f);
    file_read(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    file_read(&p_sim->pivot,sizeof(p_sim->pivot),1,f);
    file_read(&p_sim->pivot3ds,sizeof(p_sim->pivot3ds),1,f);
    file_read(&p_sim->childnum,sizeof(p_sim->childnum),1,f);
    
    file_read(&klicu,sizeof(klicu),1,f);
    if(klicu) {
      p_sim->p_pos = (BOD *)mmalloc(sizeof(p_sim->p_pos[0])*p_sim->keynum);
      file_read(p_sim->p_pos,sizeof(p_sim->p_pos[0]),p_sim->keynum,f);
    } else {
      p_sim->p_pos = NULL;
    }
    
    file_read(&klicu,sizeof(klicu),1,f);
    if(klicu) {
      p_sim->p_scale = (BOD *)mmalloc(sizeof(p_sim->p_scale[0])*p_sim->keynum);
      file_read(p_sim->p_scale,sizeof(p_sim->p_scale[0]),p_sim->keynum,f);
    } else {
      p_sim->p_scale = NULL;
    }
    
    file_read(&klicu,sizeof(klicu),1,f);
    if(klicu) {
      p_sim->p_at = (QUAT *)mmalloc(sizeof(p_sim->p_at[0])*p_sim->keynum);
      file_read(p_sim->p_at,sizeof(p_sim->p_at[0]),p_sim->keynum,f);
    } else {
      p_sim->p_at = NULL;
    }

    if(ch.typ == CHUNK_SIM_NODE_EXT || ch.typ == CHUNK_SIM_NODE_EXT2) {
      file_read(&klicu,sizeof(klicu),1,f);
      if(klicu) {
        p_sim->p_norm = (float *)mmalloc(sizeof(p_sim->p_norm[0])*p_sim->keynum);
        file_read(p_sim->p_norm,sizeof(p_sim->p_norm[0]),p_sim->keynum,f);
      } else {
        p_sim->p_norm = NULL;
      }
    }

    if(ch.typ == CHUNK_SIM_NODE_EXT2) {
      p_sim->p_all_matrix = (GLMATRIX *)mmalloc(sizeof(p_sim->p_all_matrix[0]));
      file_read(p_sim->p_all_matrix,sizeof(p_sim->p_all_matrix[0]),1,f);
    }

    if(p_sim->childnum) {
      p_sim->p_child = (SIMPLE_TRACK_INFO *)mmalloc(sizeof(p_sim->p_child[0])*p_sim->childnum);
      for(i = 0; i < p_sim->childnum; i++)
        lo_chunk_load_sim_node(f, p_sim->p_child+i);
    }
    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_sim_root(FHANDLE f, DATA_CHUNK *p_ch)
{
  int i;
  if(p_kont && p_ch->typ == CHUNK_SIM_ROOT) {
    if((i = lo_najdi_volny_sim(p_kont->sim, KONT_MAX_ANIM)) == K_CHYBA)
      chyba("Fuck-off");    
    p_sim = p_kont->sim+i;
    memset(p_sim,0,sizeof(p_sim[0]));
    
    file_read(&p_sim->flag,sizeof(p_sim->flag),1,f);
    file_read(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    file_read(&p_sim->name,sizeof(p_sim->name[0]),MAX_JMENO,f);
    file_read(&p_sim->childnum,sizeof(p_sim->childnum),1,f);
        
    // udelej misto pro decka
    if(p_sim->childnum) {
      p_sim->p_child = (SIMPLE_TRACK_INFO *)malloc(sizeof(p_sim->p_child[0])*p_sim->childnum);
      for(i = 0; i < p_sim->childnum; i++)
        lo_chunk_load_sim_node(f, p_sim->p_child+i);
    }
    
    key_sim_dopln_matrix_kont(p_kont,p_sim);
    
    return(TRUE);
  } else {
    p_sim = NULL;
    return(FALSE);
  }
}

int lo_chunk_load_neznamy(FHANDLE f, DATA_CHUNK *p_ch)
{
  int delka = p_ch->velikost-sizeof(*p_ch);  
  //if(p_ch->typ < 0 && p_ch->typ > CHUNK_KONT_JMENO)
    //kprintfl(TRUE,"-load neznamy chunk typ %d velikost %d",p_ch->typ,p_ch->velikost);
  ffseek(f,delka,SEEK_CUR);  
  return(TRUE);
}

int lo_load_chunky(FHANDLE f)
{
  DATA_CHUNK ch;
  int load, i, j = 0, ret;
  
  while((ret = file_read(&ch,sizeof(ch),1,f)) && ret > 0) {
    spracuj_spravy(TRUE);
    load = 0;

    for(i = 0; i < (sizeof(chload)/sizeof(chload[0])); i++) {
      if(chload[i].chunk == ch.typ) {        
        load = chload[i].p_fce(f,&ch);
        break;
      }
    }
    
    if(!load) {
      lo_chunk_load_neznamy(f, &ch);
    }

    j++;
  }

  if(ret < 0) {
    if(ret == -1) {
      //kprintf(TRUE,"Chyba loadu projektu '%s'",strerror(errno));
    }    
  }

  return(j);
}

 Nahraje vsechny materialy + prvni kontejner
*/
/*
EDIT_KONTEJNER * lo_nahraj_kontejner_chunk(EDIT_MATERIAL **p_mat, int max_mat,
                 EDIT_TEXT *p_text, int max_text, char *p_name, int mat, int normal)                                  
{
*/
  /*
  FHANDLE f;

  if((f = ffopen(p_name,"rb")) == NULL) {
    //kprintf(1,"Fail loading kont %s error '%s'",p_name,strerror(errno));
    return(NULL);
  }

  p_ambient = NULL;

  p_matlist = p_mat;
  matlistnum = max_mat;
  p_textlist = p_text;
  textlistnum = max_text;
  
  p_lightlist = NULL;
  lightlistnum = 0;
  p_zrcadlolist = NULL;
  p_zrcadloload = NULL;
  p_flarelist = NULL;
  p_flare = NULL;
  p_top_mlha = NULL;
  flarelistnum = 0;

  kont_load = 0;
  kontlistnum = 0;
  p_kontlist = NULL;

  p_kont_old = p_kont = vyrob_kontejner();
  p_mat = NULL;
  p_obj = NULL;

  p_dlist = NULL;
  dlistnum = 0;

  p_elist = NULL;
  elistnum = 0;

  p_lenv = NULL;
  p_kamset = NULL;

  p_light_mesh = NULL;
  
  p_joint = NULL;
  p_joint_animace = NULL;

  lo_load_chunky(f);
  ffclose(f);
  
  p_kont->k2flag &= ~KONT2_LOADED;  
  p_kont = p_kont_old;  
    
  lo_chunk_stage_load_korekce(p_matlist, max_mat);

  p_kont->k2flag&=~KONT2_LOADED;
  updatuj_kontejner_flag(p_kont,p_matlist);
  updatuj_kontejner_statistika(p_kont,FALSE);
  key_kontejner_sim_reanimuj(p_kont);
  if(normal)
    kont_norm_vect(p_kont);
  key_kosti_reanimuj_kont(p_kont);

  return(p_kont);
  */
  /*
  return(NULL);
}
*/
/*
CHUNK_LOADER chunk_load[] = {
  
  {lo_chunk_load_ambient,            CHUNK_AMBIENT},
  {lo_chunk_load_levelconfig,        CHUNK_LEVEL_KONFIG_EXT},

  {lo_chunk_load_sim_root,           CHUNK_SIM_ROOT},

  {lo_chunk_load_static_light,       CHUNK_STATIC_LIGHT},
  {lo_chunk_load_static_light_mesh,  CHUNK_STATIC_LIGHT_MESH},
  {lo_chunk_load_dyn_light,          CHUNK_DYNAMIC_LIGHT},
  {lo_chunk_load_dyn_light_mesh,     CHUNK_DYNAMIC_LIGHT_MESH},
  {lo_chunk_load_extra_light,        CHUNK_EXTRA_LIGHT},
  {lo_chunk_load_extra_light_mesh,   CHUNK_EXTRA_LIGHT_MESH},
  {lo_chunk_load_zrcadlo_3,          CHUNK_ZRCADLO_3},
  {lo_chunk_load_zrcadlo_3_sub,      CHUNK_ZRCADLO_3_POLOZKA},

  {lo_chunk_load_flare,              CHUNK_FLARE},
  {lo_chunk_load_flare_mesh,         CHUNK_FLARE_MESH},
  {lo_chunk_load_flare_slozeny,      CHUNK_FLARE_SLOZENY},
  {lo_chunk_load_flare_light,        CHUNK_FLARE_LIGHT},

  {lo_chunk_load_flare_ext,          CHUNK_FLARE_EXT},
  {lo_chunk_load_flare_mesh_ext,     CHUNK_FLARE_MESH_EXT},
  {lo_chunk_load_flare_light_ext,    CHUNK_FLARE_LIGHT_EXT},

  {lo_chunk_load_mlho_kostek,        CHUNK_MLHOKOSTKA},
  {lo_chunk_load_mlho_kostek_2,      CHUNK_MLHOKOSTKA_2},

  {lo_chunk_load_environment,        CHUNK_ENVIRONMENT},
  {lo_chunk_load_kamset,             CHUNK_KAMSET},
  {lo_chunk_load_kamset_2,           CHUNK_KAMSET_2},

};
*/

/*
int lo_nahraj_projekt(EDIT_MATERIAL **p_mat, int max_mat,
                      EDIT_KONTEJNER **p_kontls, int kontnum,
                      EDIT_TEXT *p_text, int max_text, char *p_name,
                      STATIC_LIGHT *p_light, int lightnum,
                      ZDRCADLO_DESC *p_zrcadlo, int *p_zrcadlo_aktivni,
                      LENS_FLARE *p_flarr, int flaremax,
                      dword *p_amb, MLZNA_KOSTKA **p_mlha, 
                      DYN_LIGHT *p_ddlight, int ddlightnum,
                      EXTRA_DYN_LIGHT *p_eelight, int eelightnum,
                      LEVEL_ENVIRONMENT *p_env,                 
                      //NASTAVENI_KAMER *p_kam,
                      LEVEL_KONFIG *p_lev_konfig,
                      int flag, int normal)
{
  FHANDLE f;
  int   i;

  if((f = ffopen(p_name,"rb")) == NULL) {
    //kprintf(1,"Fail loading kont %s error '%s'",p_name,strerror(errno));    
    return(FALSE);
  }

  p_kontlist  = (flag&LOAD_GEOM) ? p_kontls : NULL;
  kontlistnum = (flag&LOAD_GEOM) ? kontnum  : 0;
   
  p_matlist   = (flag&LOAD_MATERIALY) ? p_mat : NULL;
  matlistnum  = (flag&LOAD_MATERIALY) ? max_mat : 0;
    
  p_textlist  = (flag&LOAD_TEXTURY) ? p_text : NULL;
  textlistnum = (flag&LOAD_TEXTURY) ? max_text : 0;

  p_ambient   = (flag&LOAD_AMBIENT) ? p_amb : NULL;

  p_zrcadlolist = (flag&LOAD_ZRCADLO) ? p_zrcadlo : NULL;
  p_zrcadloload = (flag&LOAD_ZRCADLO) ? p_zrcadlo_aktivni : NULL;

  p_lightlist  = (flag&LOAD_STAT_LIGHT) ? p_light : NULL;
  lightlistnum = (flag&LOAD_STAT_LIGHT) ? lightnum : 0;

  p_dlist  = (flag&LOAD_DYN_LIGHT) ? p_ddlight : NULL;
  dlistnum = (flag&LOAD_DYN_LIGHT) ? ddlightnum : 0;

  p_elist  = (flag&LOAD_EXTRA_DYN_LIGHT) ? p_eelight : NULL;
  elistnum = (flag&LOAD_EXTRA_DYN_LIGHT) ? eelightnum : 0;

  p_top_mlha   = (flag&LOAD_MLHA) ? *p_mlha : NULL;

  p_flarelist  = (flag&LOAD_FLARE) ? p_flarr : NULL;
  flarelistnum = (flag&LOAD_FLARE) ? flaremax : 0;

  p_flare = NULL;
  
  p_lenv = (flag&LOAD_LEVEL_ENV) ? p_env : NULL;
  p_lc   = (flag&LOAD_LEVEL_CONF) ? p_lev_konfig : NULL;
  p_kamset = (flag&LOAD_KAMERY) ? p_kam : NULL;
  
  p_kont = NULL;
  p_mat = NULL;
  p_obj = NULL;

  p_light_mesh = NULL;

  p_joint = NULL;
  p_joint_animace = NULL;

  lo_load_chunky(f);
  ffclose(f);

  lo_chunk_stage_load_korekce(p_matlist, max_mat);

  for(i = 0; i < kontnum; i++) {
    if(p_kontls[i] && p_kontls[i]->k2flag&KONT2_LOADED) {
      p_kontls[i]->k2flag&=~KONT2_LOADED;
      updatuj_kontejner_flag(p_kontls[i],p_matlist);
      updatuj_kontejner_statistika(p_kontls[i],FALSE);
      lo_preved_flare_do_sceny(p_kontls[i]->p_slight, p_flarr, flaremax);
      lo_preved_svetla_do_sceny(p_kontls[i],p_light,lightnum,p_ddlight,ddlightnum,p_eelight,eelightnum);
      key_kontejner_sim_reanimuj(p_kontls[i]);
      if(normal)
        kont_norm_vect(p_kontls[i]);
      key_kosti_reanimuj_kont(p_kontls[i]);
    }
  }
  
 *p_mlha = p_top_mlha;

  return(TRUE);
}
*/
