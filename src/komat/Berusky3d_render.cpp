/*
  Rendering berusek
*/
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kamery.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Berusky3d_castice2.h"

#include "Berusky3d_kofola2d.h"

extern G_KONFIG ber, *p_ber;

#define TEST_DIFF   0

#define RIF(min,max) (((max)+(min))*0.5f)
#define DIF(min,max) (((max)-(min))*0.5f)

EDIT_MATERIAL *__p_last_mat;

inline float ber_vzdal_bodu_z(BOD *p_stred, GLMATRIX *p_mat)
{
 return(p_mat->_13*p_stred->x + p_mat->_23*p_stred->y + 
        p_mat->_33*p_stred->z + p_mat->_43);
}

static float __flare_faktor;

/* Pointery na renderovaci funkce
*/
void (* ber_kresli_mesh_v1)(GAME_MESH *p_mesh, EDIT_MATERIAL **p_mt);
void (* ber_kresli_mesh_v2)(GAME_MESH *p_mesh, EDIT_MATERIAL **p_mt);
void (* ber_kresli_poly_v1)(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat);

int  (* ber_nastav_material_single)(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag);
int  (* ber_nastav_material_single_poly)(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag);

void ber_nahod_render_funkce(void)
{
  vertex_array_ini();

  ber_nastav_material_single = ber_nastav_material_single_multi_spec;
  ber_nastav_material_single_poly = ber_nastav_material_single_poly_multi_spec;
  /*
  if(extlist_vertex_array) {
    kprintf(TRUE,"Array-Vertex function ok");
    ber_kresli_poly_v1 = ber_kresli_poly_array;
    if(extlist_indicie_array) {
      kprintf(TRUE,"Array-Indices function ok");
      ber_kresli_mesh_v1 = ber_kresli_mesh_array;
      ber_kresli_mesh_v2 = ber_kresli_mesh_array;
    } else {
      ber_kresli_mesh_v1 = ber_kresli_mesh_vertex_multitext;
      ber_kresli_mesh_v2 = extlist_secondary_color ? ber_kresli_mesh_vertex_multitext_specular : ber_kresli_mesh_vertex_multitext;
    }      
  } else {
    ber_kresli_mesh_v1 = ber_kresli_mesh_vertex_multitext;
    ber_kresli_mesh_v2 = extlist_secondary_color ? ber_kresli_mesh_vertex_multitext_specular : ber_kresli_mesh_vertex_multitext;
    ber_kresli_poly_v1 = extlist_secondary_color ? ber_kresli_poly_vertex_multitext_specular : ber_kresli_poly_vertex_multitext;
  }
  */
}

/* Inicializacni funkce
*/
void ber_ini_vertex_arrays(void)
{
 /* Povoleni vertex-arrays
 */
 glEnable(GL_VERTEX_ARRAY_EXT);
}

void ber_ini_nic(void)
{
}

// urci jestli jsou flare viditelne
void ber_viditelnost_flare(G_KONFIG *p_ber, LENS_FLARE *p_flare)
{
 GLMATRIX *p_camera_project = get_matrix_camera_project();
 float pixel_z,t,xw,yw,zw,w,ryw,sx,sy,xq,yq;
 int   s,nx,ny,maxx,maxy,ox,oy; 
 LENS_FLARE_SLOZ *p_sloz;
 STATIC_LIGHT  *p_light;
 BOD   p;

 get_matrix_view(&nx, &ny, &maxx, &maxy);
 ox = (maxx>>1);
 oy = (maxy>>1);
 __flare_faktor = (float)(maxx)/10.0f;
  
 while(p_flare) {
   p_flare->vid = FALSE;
   if(!p_flare->akt) {
     p_flare = p_flare->p_next;
     continue;
   }   

   if(p_flare->p_bod) {
     p = *p_flare->p_bod;
   } else {
     if(p_flare->p_svetlo) {       
       p = p_flare->p_svetlo->p;
     } else {
       p = p_flare->p;
       p_light = NULL;
     }
   }

   w = 1.0f;
   transformuj_wbod_matici(&p.x,&p.y,&p.z,&w,p_camera_project);
   
   p_flare->vid = (0 < p.z && p.z < w && -w < p.x && p.x <= w && -w < p.y && p.y <= w);
   
   if(p.z > 0.0f && (p_flare->vid || p_flare->p_sloz)) {

     p_flare->wz = p.z; //wz musi byt vetsi nez 0 a mensi nez w

     xw = (maxx>>1)*(p.x/w)+ox;
     yw = (maxy>>1)*(p.y/w)+oy;
     zw = (0.5f)*(p.z/w)+0.5f;
     
     p_flare->wx = xw;
     p_flare->wy = ryw = maxy-1-yw;

     if(!(p_flare->zflag&FLR_NO_ZTEST) && p_flare->vid) {
       glReadPixels(ftoi(xw),ny+ftoi(yw),1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&pixel_z);
       p_flare->vid = (pixel_z >= zw);
     }

     if(p_flare->p_sloz) {       
       xq = (float)ox-xw;
       yq = (float)oy-ryw;       
       p_sloz = p_flare->p_sloz;
       for(s = 0; s < SLOZ_FLARE; s++) {
         t = p_sloz[s].vzdal;
         if(t > 0.0f) {
           t *= 1.3f;
           p_sloz[s].wx = sx = xw+xq*t;
           p_sloz[s].wy = sy = ryw+yq*t;
           p_sloz[s].vid = (0 <= sx && sx < maxx && 0 <= sy && sy < maxy);
         } else {
           p_sloz[s].vid = FALSE;
         }
       }   
     }
   }
   p_flare = p_flare->p_next;
 }
}


inline void ber_kresli_flare_pre(G_KONFIG *p_ber)
{
  int   __flare_x,__flare_y,__flare_maxx,__flare_maxy;
    
  get_matrix_view(&__flare_x, &__flare_y, &__flare_maxx, &__flare_maxy);
  set_matrix_2d(__flare_maxx,__flare_maxy);
  
  text_set_num(1);
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);
  
  text_set_num(0);
  text_on(GL_TEXTURE_2D);
  blend_on(); 

  deph_test_set(FALSE);

  disable_fog_causal();

  specular_off();
}

inline void ber_kresli_flare_po(G_KONFIG *p_ber)
{
  deph_test_set(TRUE);
  ret_matrix_2d();
  reset_stage_bloky();
  enable_fog_causal();
}

#define SLOZ_FLARE  14

// vykresli ty viditelne spolu s pruhlednyma mesh/poly
void ber_kresli_flare(G_KONFIG *p_ber, LENS_FLARE *p_flare)
{
  EDIT_TEXT *p_text;
  LENS_FLARE_SLOZ *p_sloz;
  float            dx,dy,xw,yw;
  int              s,i = 0;

  ber_kresli_flare_pre(p_ber);
  /*
  while(p_flare) {
    if(!p_flare->vid || !p_flare->akt) {
      p_flare = p_flare->p_next;
      continue;
    }        

    blend_set(GL_SRC_ALPHA,p_flare->alfa);

    // Nastavim material flaru
    p_text = p_flare->p_mat->p_text[0];
    text_set(p_text->text,p_text->typ);
    glColor4fv(&p_flare->r);
    
    dx = p_flare->dx*__flare_faktor;
    dy = p_flare->dy*__flare_faktor;
    
    xw = p_flare->wx;
    yw = p_flare->wy;
    
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f,0.0f);
      glVertex3f(-dx+xw, dy+yw,0.0f);
    
      glTexCoord2f(1.0f,0.0f);
      glVertex3f( dx+xw, dy+yw,0.0f);
    
      glTexCoord2f(1.0f,1.0f);
      glVertex3f( dx+xw,-dy+yw,0.0f);
    
      glTexCoord2f(0.0f,1.0f);
      glVertex3f(-dx+xw,-dy+yw,0.0f);
    glEnd();
    
    if(p_flare->p_sloz) {
      p_sloz = p_flare->p_sloz;
      for(s = 0; s < SLOZ_FLARE; s++) {
        if(p_sloz[s].vid) {
          xw = p_sloz[s].wx;
          yw = p_sloz[s].wy;  
          
          dx = p_sloz[s].dx*__flare_faktor;
          dy = p_sloz[s].dy*__flare_faktor;

          p_text = p_sloz[s].p_mat->p_text[0];
          text_set(p_text->text,p_text->typ);
          glColor4fv(&p_sloz[s].r);
          
          glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex3f(-dx+xw, dy+yw,0.0f);
          
            glTexCoord2f(1.0f,0.0f);
            glVertex3f( dx+xw, dy+yw,0.0f);
          
            glTexCoord2f(1.0f,1.0f);
            glVertex3f( dx+xw,-dy+yw,0.0f);
          
            glTexCoord2f(0.0f,1.0f);
            glVertex3f(-dx+xw,-dy+yw,0.0f);
          glEnd();
        }
      }
    }  
    p_flare = p_flare->p_next;
  } 
*/  
  ber_kresli_flare_po(p_ber);
}

inline void ber_render_polylistu_start(void)
{
  set_matrix_world_init();
}

int ber_nastav_material_single_multi_spec(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag)
{
  MATERIAL_TEXT *p_stg;
  MATERIAL_TEXT *p_stg_smat;
  int            i,s,t,
                 rezerva_smat,
                 rezerva_mat,
                 pozice_smat,
                 stage_smat;

  /* Nastaveni spekularni barvy
  */
  if(p_mat->flag2&(MAT2_SPECULAR|MAT2_ENV_SPEC) || kflag&KONT_DRAW_SPEC)
    specular_on();
  else
    specular_off();

  /* Nastaveni pruhlednosti
  */
  if(kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) { // ... prehozeni stage bloku + 50    
    set_alfa_blok(STATE_BLOK_PRUHL);
  } else {
    //... nahod aktualni alfa-stage
    /* Alfa-blending
    */
    set_alfa_blok(p_mat->alfa_state);
  }

  /* Test na duplicitu materialu
  */
  if(!p_mat || __p_last_mat == p_mat)
    return(FALSE);
  __p_last_mat = p_mat;

  /* Nahozeni matrose-plazivce
  */
  /*
  if(!p_smat || !p_smat->p_text[0]->load || p_mat->flag&MAT_NO_SCMT) {
    p_stg = p_mat->text_state;
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,0))==K_CHYBA)
          break;
      } else { // defaultni stage - nic
        text_stage_func_nic(t);
        break;
      }
      p_stg++;
    }
  } else {
    p_stg = p_mat->text_state;
    p_stg_smat = p_smat->text_state;
    pozice_smat = p_smat->smaterial_pozice;
    stage_smat = p_stg_smat->text_stage;    
    
    if(p_smat->flag&MAT_SCENE_NUTNY) {
      rezerva_smat = p_smat->textur;
      rezerva_mat = 0;
    } else {
      rezerva_smat = 0;
      rezerva_mat = p_mat->textur;
    }
    
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if(i == pozice_smat) {
        if((t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,rezerva_mat))==K_CHYBA)
          break;
        rezerva_smat = 0;
      }
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,rezerva_smat))==K_CHYBA)
          break;
      } else { // defaultni stage - nic - konec
        break;
      }
      p_stg++;
    }
    if(i < pozice_smat) {
      t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,0);
    }  
    text_stage_func_nic(t);  
  }
 */
  /* Nastaveni difusni barvy
  */
  if(p_mat->flag2&MAT2_DIFFUSE) {
    diffuse_on();
  } else {
    diffuse_off(p_mat->dxmat.diffuse_r,p_mat->dxmat.diffuse_g,p_mat->dxmat.diffuse_b,p_mat->dxmat.diffuse_a);
  }

  /* Nastaveni z-bufferu
  */
  deph_test_set((~p_mat->flag)&MAT_NO_ZTEST);
  zmask_set((~p_mat->flag)&MAT_NO_ZMASK);

  /* Nastaveni cullingu
  */
  if(p_mat->flag&MAT_NO_CULL)
    cull_off();
  else
    cull_on();

  return(TRUE);
}

/* Strc to tam!!!!
*/
int ber_nastav_material_single_multi(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag)
{  
  MATERIAL_TEXT *p_stg;
  MATERIAL_TEXT *p_stg_smat;
  int            i,s,t,
                 rezerva_smat,
                 rezerva_mat,
                 pozice_smat,
                 stage_smat;

  /* Nastaveni pruhlednosti
  */
  if(kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) { // ... prehozeni stage bloku + 50
    set_alfa_blok(STATE_BLOK_PRUHL);
  } else {
    //... nahod aktualni alfa-stage
    /* Alfa-blending
    */
    set_alfa_blok(p_mat->alfa_state);
  }

  /* Test na duplicitu materialu
  */
  if(!p_mat || __p_last_mat == p_mat)
    return(FALSE);
  __p_last_mat = p_mat;

  /* Nahozeni matrose-plazivce
  */
  /*
  if(!p_smat || !p_smat->p_text[0]->load || p_mat->flag&MAT_NO_SCMT) {
    p_stg = p_mat->text_state;
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,0))==K_CHYBA)
          break;
      } else { // defaultni stage - nic
        text_stage_func_nic(t);
        break;
      }
      p_stg++;
    }
  } else {
    p_stg = p_mat->text_state;
    p_stg_smat = p_smat->text_state;
    pozice_smat = p_smat->smaterial_pozice;
    stage_smat = p_stg_smat->text_stage;    
    
    if(p_smat->flag&MAT_SCENE_NUTNY) {
      rezerva_smat = p_smat->textur;
      rezerva_mat = 0;
    } else {
      rezerva_smat = 0;
      rezerva_mat = p_mat->textur;
    }
    
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if(i == pozice_smat) {
        if((t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,rezerva_mat))==K_CHYBA)
          break;
        rezerva_smat = 0;
      }
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,rezerva_smat))==K_CHYBA)
          break;
      } else { // defaultni stage - nic - konec
        break;
      }
      p_stg++;
    }
    if(i < pozice_smat) {
      t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,0);
    }  
    text_stage_func_nic(t);  
  }
*/
  /* Nastaveni difusni barvy
  */
  if(p_mat->flag2&MAT2_DIFFUSE) {
    diffuse_on();
  } else {
    diffuse_off(p_mat->dxmat.diffuse_r,p_mat->dxmat.diffuse_g,p_mat->dxmat.diffuse_b,p_mat->dxmat.diffuse_a);
  }

  /* Nastaveni z-bufferu
  */  
  deph_test_set((~p_mat->flag)&MAT_NO_ZTEST);
  zmask_set((~p_mat->flag)&MAT_NO_ZMASK);

  /* Nastaveni cullingu
  */
  if(p_mat->flag&MAT_NO_CULL)
    cull_off();
  else
    cull_on();

  return(TRUE);
}

int ber_nastav_material_single_nic(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag)
{
  MATERIAL_TEXT *p_stg;
  int s,t;

  /* Nastaveni pruhlednosti
  */
  if(kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) { // ... prehozeni stage bloku + 50
    set_alfa_blok(STATE_BLOK_PRUHL);
  } else {
    //... nahod aktualni alfa-stage
    /* Alfa-blending
    */
    set_alfa_blok(p_mat->alfa_state);
  }

  /* Test na duplicitu materialu
  */
  if(!p_mat || __p_last_mat == p_mat)
    return(FALSE);
  __p_last_mat = p_mat;

  /* Nahozeni matrose-plazivce
  */
  p_stg = p_mat->text_state;
  t = 0;  
/*
  if((s = p_stg->text_stage) != K_CHYBA) {
    p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,0);
  } else { // defaultni stage - nic
    text_stage_func_nic(t);    
  }
*/
  /* Nastaveni difusni barvy
  */
  if(p_mat->flag2&MAT2_DIFFUSE) {
    diffuse_on();
  } else {
    diffuse_off(p_mat->dxmat.diffuse_r,p_mat->dxmat.diffuse_g,p_mat->dxmat.diffuse_b,p_mat->dxmat.diffuse_a);
  }

  /* Nastaveni cullingu
  */
  if(p_mat->flag&MAT_NO_CULL)
    cull_off();
  else
    cull_on();

    
  /* Nastaveni z-bufferu
  */
  deph_test_set((~p_mat->flag)&MAT_NO_ZTEST);
  zmask_set((~p_mat->flag)&MAT_NO_ZMASK);
  
  return(TRUE);
}

/* Rendering meshe -> verze 1 -> multitexturing bez spekularu
   v poly
*/
void ber_kresli_mesh_array(GAME_MESH *p_mesh, EDIT_MATERIAL **p_mt)
{     
/*
  int         i,j,o,facenum,fnum2;
  int         kflag = p_mesh->p_data->kflag;
  int         key_flag = kflag&KONT_KEYFRAME;
  int        *p_face,ind,gltyp;
  dword      *p_vflag = p_mesh->p_kflag;
    
  #ifdef DEBUG_MOD
    p_ber->debug.meshu++;
  #endif

  if(!key_flag) {
    set_matrix_world(&p_mesh->m);
  }  

  // Ma se mesh uploadoat do graficke pameti?
  if(p_mesh->p_data->k2flag&KONT2_UPLOAD) {
    mesh_vertex_array_upload(p_mesh);
  }

  mesh_vertex_array_set(p_mesh);
  
  o = p_mesh->objektu;
  for(i = 0; i < o; i++, p_vflag++) { // kresli to po objektech
    if((*p_vflag)&KONT_VIDITELNY) {
      if(ber_nastav_material_single(p_ber->p_smat, p_mt[p_mesh->p_mat[i]],kflag) || !i)
        mesh_vertex_array_set_koord(p_mesh);
      
#ifdef DEBUG_MOD
      p_ber->debug.bodu += p_mesh->objektnum[i];
      p_ber->debug.objektu++;
#endif
      
      if(key_flag)
        set_matrix_world(p_mesh->p_key+i);
  
      ind = p_mesh->facestart[i];
      p_face = p_mesh->p_face+ind;
      facenum = p_mesh->facenum[i];
      
      for(j = 0; j < facenum; j++) {
        fnum2 = *p_face++;
        gltyp = *p_face++;
        ind += 2;

#ifdef DEBUG_MOD
        p_ber->debug.facu += fnum2;
#endif  

        if(extlist_arb_vertex_buffer) {
          glDrawElements(gltyp, fnum2, GL_UNSIGNED_INT,
                         BUFFER_OFFSET(sizeof(p_mesh->p_face[0])*
                                      (p_face-p_mesh->p_face)));
        } else {
          glElementPointerATI(GL_UNSIGNED_INT,p_face);
          glDrawElementArrayATI(gltyp,fnum2);
        }
        
        p_face += fnum2;
      }      
    }
  }
  */
}


/* Renderig meshe z poly-listu
*/
void ber_kresli_mesh_vertex_multitext_specular(GAME_MESH *p_mesh, EDIT_MATERIAL **p_mt)
{ 
/*
  BOD        *p_vertex_pos = NULL;
  BODUV      *p_vertex_text1 = NULL;
  BODUV      *p_vertex_text2 = NULL;
  BODUV      *p_vertex_text3 = NULL;
  BODUV      *p_vertex_text4 = NULL;
  BODRGBA    *p_vertex_diff = NULL;
  BODRGB     *p_vertex_spec = NULL;
  BOD        *p_vertex_normal;
  int        *p_face;
  int         v,i,j,o,d,facenum,fnum2;
  int         kflag = p_mesh->p_data->kflag;
  int         key_flag = kflag&KONT_KEYFRAME;
  dword      *p_vflag = p_mesh->p_kflag;
  BODUV     **p_map;
  int        *p_int,ind;

    
  #ifdef DEBUG_MOD    
    p_ber->debug.meshu++;
  #endif

  if(!key_flag) {
    set_matrix_world(&p_mesh->m);
  }  
  
  o = p_mesh->objektu;

  p_vertex_pos = p_mesh->p_vertex_pos;  
  p_vertex_normal = (glstav_pn_triangles || p_mesh->p_data->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4)) ? p_mesh->p_vertex_norm : NULL;

  for(i = 0; i < o; i++, p_vflag++) { // kresli to po objektech
    if((*p_vflag)&KONT_VIDITELNY) {
      if(ber_nastav_material_single(p_ber->p_smat, p_mt[p_mesh->p_mat[i]],kflag) || !i) {
        p_map = &p_mesh->p_vertex_uv1_material;
        p_int = glstav_text_map_indicie;
        if((ind = *p_int++)!=K_CHYBA)
          p_vertex_text1 = p_map[ind];
        if((ind = *p_int++)!=K_CHYBA)
          p_vertex_text2 = p_map[ind];
        if((ind = *p_int++)!=K_CHYBA)
          p_vertex_text3 = p_map[ind];
        if((ind = *p_int++)!=K_CHYBA)
          p_vertex_text4 = p_map[ind];
        if(kflag&KONT_DRAW_VODA) {
          if(glstav_diffuse)
            p_vertex_diff = p_mesh->p_vertex_diff_voda;
          p_vertex_spec = p_mesh->p_vertex_spec_voda;
        } else {
          if(glstav_diffuse)
            p_vertex_diff = p_mesh->p_vertex_diff;
          p_vertex_spec = p_mesh->p_vertex_spec;
        }  
      }
      
#ifdef DEBUG_MOD
      p_ber->debug.bodu += p_mesh->objektnum[i];
      p_ber->debug.objektu++;
#endif
      
      if(key_flag)
        set_matrix_world(p_mesh->p_key+i);
      
      p_face = p_mesh->p_face+p_mesh->facestart[i];
      facenum = p_mesh->facenum[i];
      for(j = 0; j < facenum; j++) {
        fnum2 = *p_face++;
        glBegin(*p_face++);
        
#ifdef DEBUG_MOD
        p_ber->debug.facu += fnum2;
#endif        
        for(d = 0; d < fnum2; d++) {
          v = *p_face++;
          if(p_vertex_text1)
            glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(float *)(p_vertex_text1+v));
          if(p_vertex_text2)
            glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,(float *)(p_vertex_text2+v));
          if(p_vertex_text3)
            glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,(float *)(p_vertex_text3+v));
          if(p_vertex_text4)
            glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,(float *)(p_vertex_text4+v));
          if(p_vertex_spec)
            glSecondaryColor3fvEXT((float *)(p_vertex_spec+v));
          if(p_vertex_normal) 
            glNormal3fv((float *)(p_vertex_normal+v));
          if(p_vertex_diff)
            glColor4fv((float *)(p_vertex_diff+v));
          glVertex3fv((float *)(p_vertex_pos+v));
        }
        glEnd();
      }      
    }
  } 
  */
}

void ber_kresli_mesh_vertex_multitext(GAME_MESH *p_mesh, EDIT_MATERIAL **p_mt)
{  
/*
  BOD        *p_vertex_pos = NULL;
  BODUV      *p_vertex_text1 = NULL;
  BODUV      *p_vertex_text2 = NULL;
  BODUV      *p_vertex_text3 = NULL;
  BODUV      *p_vertex_text4 = NULL;
  BODRGBA    *p_vertex_diff = NULL;  
  BOD        *p_vertex_normal;
  int        *p_face;  
  int v,i,j,o,d,facenum,fnum2;
  int         kflag = p_mesh->p_data->kflag;
  int         key_flag = kflag&KONT_KEYFRAME;
  dword      *p_vflag = p_mesh->p_kflag;
  BODUV     **p_map;
  int        *p_int,ind;

  #ifdef DEBUG_MOD    
    p_ber->debug.meshu++;
  #endif

  if(!key_flag) {
    set_matrix_world(&p_mesh->m);
  }  
  
  o = p_mesh->objektu;
  
  p_vertex_pos = p_mesh->p_vertex_pos;
  p_vertex_normal = glstav_pn_triangles || p_mesh->p_data->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4) ? p_mesh->p_vertex_norm : NULL;
  
  for(i = 0; i < o; i++, p_vflag++) { // kresli to po objektech
    if((*p_vflag)&KONT_VIDITELNY) {
      if(ber_nastav_material_single(p_ber->p_smat, p_mt[p_mesh->p_mat[i]],kflag) || !i) {
        p_map = &p_mesh->p_vertex_uv1_material;
        p_int = glstav_text_map_indicie;
        if((ind = *p_int++)!=K_CHYBA) 
          p_vertex_text1 = p_map[ind];
        if((ind = *p_int++)!=K_CHYBA) 
          p_vertex_text2 = p_map[ind];
        if((ind = *p_int++)!=K_CHYBA) 
          p_vertex_text3 = p_map[ind];
        if((ind = *p_int++)!=K_CHYBA) 
          p_vertex_text4 = p_map[ind];
        if(glstav_diffuse)
          p_vertex_diff = kflag&KONT_DRAW_VODA ? p_mesh->p_vertex_diff_voda : p_mesh->p_vertex_diff;
      }
      
#ifdef DEBUG_MOD
      p_ber->debug.bodu += p_mesh->objektnum[i];
      p_ber->debug.objektu++;
#endif
      
      if(key_flag)
        set_matrix_world(p_mesh->p_key+i);
      
      p_face = p_mesh->p_face+p_mesh->facestart[i];
      facenum = p_mesh->facenum[i];

      for(j = 0; j < facenum; j++) {

        fnum2 = *p_face++;
        glBegin(*p_face++);

#ifdef DEBUG_MOD
        p_ber->debug.facu += fnum2;
#endif
        for(d = 0; d < fnum2; d++) {
          v = *p_face++;
          if(p_vertex_text1)
            glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(float *)(p_vertex_text1+v));
          if(p_vertex_text2)
            glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,(float *)(p_vertex_text2+v));
          if(p_vertex_text3)
            glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,(float *)(p_vertex_text3+v));
          if(p_vertex_text4)
            glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,(float *)(p_vertex_text4+v));
          if(p_vertex_normal)
            glNormal3fv((float *)(p_vertex_normal+v));
          if(p_vertex_diff)
            glColor4fv((float *)(p_vertex_diff+v));
          glVertex3fv((float *)(p_vertex_pos+v));
        }
        glEnd();
      }    
    }
  } 
  */
}

void ber_kresli_mesh_vertex(GAME_MESH *p_mesh, EDIT_MATERIAL **p_mt)
{  
/*
  BOD        *p_vertex_pos = NULL;
  BOD        *p_vertex_normal = NULL;
  BODUV      *p_vertex_text1 = NULL;
  BODRGBA    *p_vertex_diff = NULL;  
  int         v,i,j,o,d,facenum,fnum2;
  int         kflag = p_mesh->p_data->kflag;
  int         key_flag = kflag&KONT_KEYFRAME;
  int        *p_face;
  dword      *p_vflag = p_mesh->p_kflag;
  BODUV     **p_map;
  int        *p_int,ind;

    
  #ifdef DEBUG_MOD    
    p_ber->debug.meshu++;
  #endif

  if(!key_flag) {
    set_matrix_world(&p_mesh->m);
  }  
  
  o = p_mesh->objektu;
  
  p_vertex_pos = p_mesh->p_vertex_pos;
  p_vertex_normal = glstav_pn_triangles || p_mesh->p_data->m2flag&MAT2_CALC_MAP1 ? p_mesh->p_vertex_norm : NULL;

  for(i = 0; i < o; i++, p_vflag++) { // kresli to po objektech
    if((*p_vflag)&KONT_VIDITELNY) {
      if(ber_nastav_material_single(p_ber->p_smat, p_mt[p_mesh->p_mat[i]],kflag) || !i) {
        p_map = &p_mesh->p_vertex_uv1_material;
        p_int = glstav_text_map_indicie;
        if((ind = *p_int) != K_CHYBA)
          p_vertex_text1 = p_map[ind];
        if(glstav_diffuse)
          p_vertex_diff = kflag&KONT_DRAW_VODA ? p_mesh->p_vertex_diff_voda : p_mesh->p_vertex_diff;
      }

      
#ifdef DEBUG_MOD
      p_ber->debug.bodu += p_mesh->objektnum[i];
      p_ber->debug.objektu++;
#endif
      
      if(key_flag)
        set_matrix_world(p_mesh->p_key+i);
      
      p_face = p_mesh->p_face+p_mesh->facestart[i];
      facenum = p_mesh->facenum[i];
      for(j = 0; j < facenum; j++) {
        
        fnum2 = *p_face++;
        glBegin(*p_face++);
        
#ifdef DEBUG_MOD
        p_ber->debug.facu += fnum2;
#endif
        if(p_vertex_normal) {
          for(d = 0; d < fnum2; d++) {
            v = *p_face++;
            if(p_vertex_text1)
              glTexCoord2fv((float *)(p_vertex_text1+v));
            if(p_vertex_diff)
              glColor4fv((float *)(p_vertex_diff+v));
            glNormal3fv((float *)(p_vertex_normal+v));
            glVertex3fv((float *)(p_vertex_pos+v));
          }
        } else {
          for(d = 0; d < fnum2; d++) {
            v = *p_face++;
            if(p_vertex_text1)
              glTexCoord2fv((float *)(p_vertex_text1+v));
            if(p_vertex_diff)
              glColor4fv((float *)(p_vertex_diff+v));
            glVertex3fv((float *)(p_vertex_pos+v));
          }
        }
        glEnd();
      }
    }
  } 
  */
}


void ber_kresli_obalku_mesh(GAME_MESH *p_mesh)
{ 
  //set_matrix_world_init();
  //obb_kresli_obalku(&p_mesh->obb_world,DDRGBA(1,1,1,1),NULL);
}

void ber_kresli_obalku_poly(EDIT_MESH_POLY *p_poly)
{ 
  set_matrix_world_init();
  //obb_kresli_obalku(&p_poly->obb,DDRGBA(1,1,1,1),NULL);  
}

inline void ber_kresli_mesh(GAME_MESH *p_mesh, EDIT_MATERIAL **p_mat)
{
/*
  int kflag = p_mesh->p_data->kflag;
  int m2flag = p_mesh->p_data->m2flag;
  int kamera_zmena = p_ber->kamera.zmena||p_ber->kamera.zmena_last;

  // Pripadne vypnuti mlhy
  if(kflag&KONT_NO_FOG) {
    disable_fog_causal();
  } else {
    // Pripadne zapnuti mlhy
    if(p_mesh->p_mlha) {
      if(!p_ber->p_mlha_akt)
        enable_fog();
      nastav_fog(p_mesh->p_mlha);
    }
  }

#ifdef DEBUG_MOD_OBALKY  
  ber_kresli_obalku_mesh(p_mesh);
#endif

  // Nastaveni spec-env-mappingu
  if(m2flag&MAT2_ENV_SPEC && (p_ber->kamera.zmena || kflag&KONT_POHYB))
    mesh_env_maping_spec(p_mesh, get_matrix_camera_point(), p_mat);
    
  // Nastaveni polygon-offsetu
  if(kflag&KONT_PLG_OFFSET) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f,-2.0f);
  }

  // Rendering samotneho meshe
  if(p_mesh->p_data->m2flag&(MAT2_SPECULAR|MAT2_ENV_SPEC)||
     p_mesh->p_data->kflag&KONT_DRAW_SPEC) {
    ber_kresli_mesh_v2(p_mesh,p_mat);
  } else {
    ber_kresli_mesh_v1(p_mesh,p_mat);
  }

  // Smazani polygon-offsetu
  if(kflag&KONT_PLG_OFFSET) {
    glDisable(GL_POLYGON_OFFSET_FILL);
  }

  // Odflagovani meshe
  if(kflag&KONT_POHYB_ZRUS) {
    p_mesh->p_data->kflag&=~(KONT_POHYB_ZRUS|KONT_POHYB);
  }

  // Pripadne zapnuti mlhy
  if(p_mesh->p_data->kflag&KONT_NO_FOG) {
    enable_fog_causal();
  } else {
    // Pripadne vypnuti mlhy
    if(p_mesh->p_mlha) {
      if(p_ber->p_mlha_akt) {
        nastav_fog(p_ber->p_mlha_akt);
      } else {
        disable_fog();
      }
    }
  }
  */
}

int ber_nastav_material_single_poly_multi_spec(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag)
{ 
  MATERIAL_TEXT *p_stg;
  MATERIAL_TEXT *p_stg_smat;
  int            i,s,t,
                 rezerva_smat,
                 rezerva_mat,
                 pozice_smat,
                 stage_smat,
                 poly_text;

  /* Nastaveni spekularni barvy
  */
  if(p_mat->flag2&(MAT2_SPECULAR|MAT2_ENV_SPEC) || kflag&KONT_DRAW_SPEC)
    specular_on();
  else
    specular_off();

  /* Nastaveni pruhlednosti
  */
  if(kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) { // ... prehozeni stage bloku + 50    
    set_alfa_blok(STATE_BLOK_PRUHL);
  } else {
    //... nahod aktualni alfa-stage
    /* Alfa-blending
    */
    set_alfa_blok(p_mat->alfa_state);
  }


  /* Test na duplicitu materialu
  */
  if(!p_mat || __p_last_mat == p_mat)
    return(FALSE);
  __p_last_mat = p_mat;

  /* Nahozeni matrose-plazivce
  */
  /*
  if(!p_smat || !p_smat->p_text[0]->load || p_mat->flag&MAT_NO_SCMT) {
    p_stg = p_mat->text_state;
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,1))==K_CHYBA)
          break;
      } else { // defaultni stage - nic
        text_stage_func_nic_poly(t+1,t);
        break;
      }
      p_stg++;
    }
  } else {
    p_stg = p_mat->text_state;
    p_stg_smat = p_smat->text_state;
    pozice_smat = p_smat->smaterial_pozice;
    stage_smat = p_stg_smat->text_stage;    

    if(p_smat->flag&MAT_SCENE_NUTNY) {
      rezerva_smat = p_smat->textur+1;
      rezerva_mat = 1;
    } else {
      rezerva_smat = 0;
      rezerva_mat = p_mat->textur+1;
    }
    
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if(i == pozice_smat) {
        if((t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,rezerva_mat))==K_CHYBA)
          break;
        rezerva_smat = 1;
      }
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,rezerva_smat))==K_CHYBA)
          break;
      } else { // defaultni stage - nic - konec
        break;
      }
      p_stg++;
    }
    poly_text = t++;
    if(i < pozice_smat) {
      t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,1);
    }
    text_stage_func_nic_poly(t,poly_text);
  }
*/
  /* Nastaveni difusni barvy
  */
  if(p_mat->flag2&MAT2_DIFFUSE) {
    diffuse_on();
  } else {
    diffuse_off(p_mat->dxmat.diffuse_r,p_mat->dxmat.diffuse_g,p_mat->dxmat.diffuse_b,p_mat->dxmat.diffuse_a);
  }

  /* Nastaveni z-bufferu
  */
  deph_test_set((~p_mat->flag)&MAT_NO_ZTEST);
  zmask_set((~p_mat->flag)&MAT_NO_ZMASK);

  /* Nastaveni cullingu
  */
  if(p_mat->flag&MAT_NO_CULL)
    cull_off();
  else
    cull_on();

  return(TRUE);
}

int ber_nastav_material_single_poly_multi(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag)
{ 
  MATERIAL_TEXT *p_stg;
  MATERIAL_TEXT *p_stg_smat;
  int            i,s,t,
                 rezerva_smat,
                 rezerva_mat,
                 pozice_smat,
                 stage_smat,
                 poly_text;

  /* Nastaveni pruhlednosti
  */
  if(kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) { // ... prehozeni stage bloku + 50
    set_alfa_blok(STATE_BLOK_PRUHL);
  } else {
    //... nahod aktualni alfa-stage
    /* Alfa-blending
    */
    set_alfa_blok(p_mat->alfa_state);
  }


  /* Test na duplicitu materialu
  */
  if(!p_mat || __p_last_mat == p_mat)
    return(FALSE);
  __p_last_mat = p_mat;

  /* Nahozeni matrose-plazivce
  */
  /*
  if(!p_smat || !p_smat->p_text[0]->load || p_mat->flag&MAT_NO_SCMT) {
    p_stg = p_mat->text_state;
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,1))==K_CHYBA)
          break;
      } else { // defaultni stage - nic
        text_stage_func_nic_poly(t+1,t);
        break;
      }
      p_stg++;
    }
  } else {    
    p_stg = p_mat->text_state;
    p_stg_smat = p_smat->text_state;
    pozice_smat = p_smat->smaterial_pozice;
    stage_smat = p_stg_smat->text_stage;

    if(p_smat->flag&MAT_SCENE_NUTNY) {
      rezerva_smat = p_smat->textur+1;
      rezerva_mat = 1;
    } else {
      rezerva_smat = 0;
      rezerva_mat = p_mat->textur+1;
    }
    
    t = 0;
    for(i = 0; i < MAT_TEXTUR; i++) {
      if(i == pozice_smat) {
        if((t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,rezerva_mat))==K_CHYBA)
          break;
        rezerva_smat = 1;
      }
      if((s = p_stg->text_stage) != K_CHYBA) {
        if((t = p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,rezerva_smat))==K_CHYBA)
          break;
      } else { // defaultni stage - nic - konec
        break;
      }
      p_stg++;
    }
    poly_text = t++;
    if(i < pozice_smat) {
      t = p_text_stage_func[stage_smat](p_smat,text_stage_edit_blok+stage_smat,p_stg_smat,t,1);
    }  
    text_stage_func_nic_poly(t,poly_text);
  }
*/
  /* Nastaveni difusni barvy
  */
  if(p_mat->flag2&MAT2_DIFFUSE) {
    diffuse_on();
  } else {
    diffuse_off(p_mat->dxmat.diffuse_r,p_mat->dxmat.diffuse_g,p_mat->dxmat.diffuse_b,p_mat->dxmat.diffuse_a);
  }

  /* Nastaveni z-bufferu
  */
  deph_test_set((~p_mat->flag)&MAT_NO_ZTEST);
  zmask_set((~p_mat->flag)&MAT_NO_ZMASK);

  /* Nastaveni cullingu
  */
  if(p_mat->flag&MAT_NO_CULL)
    cull_off();
  else
    cull_on();

  return(TRUE);
}

int ber_nastav_material_single_poly_nic(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, dword kflag)
{ 
  MATERIAL_TEXT *p_stg;
  int s,t;

  /* Nastaveni pruhlednosti
  */
  if(kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) { // ... prehozeni stage bloku + 50
    set_alfa_blok(STATE_BLOK_PRUHL);
  } else {
    //... nahod aktualni alfa-stage
    /* Alfa-blending
    */
    set_alfa_blok(p_mat->alfa_state);
  }


  /* Test na duplicitu materialu
  */
  if(!p_mat || __p_last_mat == p_mat)
    return(FALSE);
  __p_last_mat = p_mat;

  /* Nahozeni matrose-plazivce
  */
  p_stg = p_mat->text_state;
  t = 0;
  /*
  if((s = p_stg->text_stage) != K_CHYBA) {
    p_text_stage_func[s](p_mat,text_stage_edit_blok+s,p_stg,t,1);
  } else { // defaultni stage - nic
    text_stage_func_nic_poly(t+1,t);
  }  
*/
  /* Nastaveni difusni barvy
  */
  if(p_mat->flag2&MAT2_DIFFUSE) {
    diffuse_on();
  } else {
    diffuse_off(p_mat->dxmat.diffuse_r,p_mat->dxmat.diffuse_g,p_mat->dxmat.diffuse_b,p_mat->dxmat.diffuse_a);
  }

  /* Nastaveni z-bufferu
  */
  deph_test_set((~p_mat->flag)&MAT_NO_ZTEST);
  zmask_set((~p_mat->flag)&MAT_NO_ZMASK);

  /* Nastaveni cullingu
  */
  if(p_mat->flag&MAT_NO_CULL)
    cull_off();
  else
    cull_on();

  return(TRUE);
}

/* Rendering bez spekularu
*/
void ber_kresli_poly_array(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat)
{  
  int l,last = 0;
  
  
  /* Ma se poly uploadoat do graficke pameti?
  */
  if(p_poly->kflag&KONT_UPLOAD) {
    poly_vertex_array_upload(p_poly);
    p_poly->kflag&=~KONT_UPLOAD;
  }  

  assert(p_mat[p_poly->material]);
  ber_nastav_material_single_poly(p_ber->p_smat, p_mat[p_poly->material],p_poly->kflag);
  poly_vertex_array_set(p_poly);
   
  #ifdef DEBUG_MOD
    p_ber->debug.poly++;
    p_ber->debug.facu += p_poly->facenum;
    p_ber->debug.bodu += p_poly->facenum;
  #endif
/*
  for(l = 0; l < p_poly->lightnum; l++) {    
    text_set(p_poly->p_light[l]->text,GL_TEXTURE_2D);
    glDrawArrays(GL_TRIANGLES,last,p_poly->p_lightnum[l]);
    last += p_poly->p_lightnum[l];
  }
*/
}

/* Rendering vcetne spekularu
*/
void ber_kresli_poly_vertex_multitext_specular(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat)
{
  TEXT_KOORD *p_koord;
  EDIT_MATERIAL *p_amat = p_mat[p_poly->material];
  int  l,i,last = 0;
  int  kflag = p_poly->kflag;
  int  norm = glstav_pn_triangles || p_poly->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
  int  spec = (p_amat->flag2&(MAT2_SPECULAR|MAT2_ENV_SPEC) || kflag&KONT_DRAW_SPEC);
  int *p_int,ind;

  assert(p_amat);
  ber_nastav_material_single_poly(p_ber->p_smat,p_amat,kflag);
  
  #ifdef DEBUG_MOD
    p_ber->debug.poly++;
    p_ber->debug.facu += p_poly->facenum;
    p_ber->debug.bodu += p_poly->facenum;
  #endif
/*
  for(l = 0; l < p_poly->lightnum; l++) {
    text_set(p_poly->p_light[l]->text,GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
    p_koord = p_poly->p_koord+last;
    for(i = 0; i < p_poly->p_lightnum[l]; i++) {
      p_int = glstav_text_map_indicie;
      if((ind = *p_int) != K_CHYBA)            
        glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      p_int++;
      if((ind = *p_int) != K_CHYBA)
        glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      p_int++;
      if((ind = *p_int) != K_CHYBA)
        glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      p_int++;
      if((ind = *p_int) != K_CHYBA)
        glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      glMultiTexCoord2fvARB(GL_TEXTURE0_ARB+glstav_text_poly_indicie, &p_poly->p_koord[last+i].tul);
      if(norm)
        glNormal3fv(&p_koord[i].nx);
      if(spec)
        glSecondaryColor3fvEXT(&p_koord[i].sr);
      glColor4fv(&p_koord[i].dr);
      glVertex3fv(&p_koord[i].x);
    }
    glEnd();
    last += p_poly->p_lightnum[l];
  }
  */
}

void ber_kresli_poly_vertex_multitext(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat)
{
  TEXT_KOORD *p_koord;
  int l,i,last = 0;
  int norm = glstav_pn_triangles || p_poly->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
  int *p_int,ind;  
  
  assert(p_mat[p_poly->material]);
  ber_nastav_material_single_poly(p_ber->p_smat,p_mat[p_poly->material],p_poly->kflag);

  #ifdef DEBUG_MOD
    p_ber->debug.poly++;
    p_ber->debug.facu += p_poly->facenum;
    p_ber->debug.bodu += p_poly->facenum;
  #endif    
/*
  for(l = 0; l < p_poly->lightnum; l++) {
    text_set(p_poly->p_light[l]->text,GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
    p_koord = p_poly->p_koord+last;
    for(i = 0; i < p_poly->p_lightnum[l]; i++) {
      p_int = glstav_text_map_indicie;
      if((ind = *p_int) != K_CHYBA)            
        glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      p_int++;
      if((ind = *p_int) != K_CHYBA)
        glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      p_int++;
      if((ind = *p_int) != K_CHYBA)
        glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      p_int++;
      if((ind = *p_int) != K_CHYBA)
        glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,((float *)(&p_poly->p_koord[last+i].tu1))+((ind)<<1));
      glMultiTexCoord2fvARB(GL_TEXTURE0_ARB+glstav_text_poly_indicie,
        &p_poly->p_koord[last+i].tul);
      glColor4fv(&p_koord[i].dr);
      if(norm)
        glNormal3fv(&p_koord[i].nx);
      glVertex3fv(&p_koord[i].x);
    }
    glEnd();
    last += p_poly->p_lightnum[l];
  }
  */
}

void ber_kresli_poly_vertex(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat)
{
  TEXT_KOORD *p_koord;
  int l,i,last = 0;
  int norm = glstav_pn_triangles || p_poly->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
  
  assert(p_mat[p_poly->material]);
  ber_nastav_material_single_poly(p_ber->p_smat,p_mat[p_poly->material],p_poly->kflag);
  
  #ifdef DEBUG_MOD
    p_ber->debug.poly++;
    p_ber->debug.facu += p_poly->facenum;
    p_ber->debug.bodu += p_poly->facenum;
  #endif    
  
  if(norm) {
    for(l = 0; l < p_poly->lightnum; l++) {
      glBegin(GL_TRIANGLES);
      p_koord = p_poly->p_koord+last;
      for(i = 0; i < p_poly->p_lightnum[l]; i++) {
        glTexCoord2fv(&p_koord[i].tu1);
        glColor4fv(&p_koord[i].dr);
        glNormal3fv(&p_koord[i].nx);
        glVertex3fv(&p_koord[i].x);
      }
      glEnd();
      last += p_poly->p_lightnum[l];
    }
  } else {
    for(l = 0; l < p_poly->lightnum; l++) {
      glBegin(GL_TRIANGLES);
      p_koord = p_poly->p_koord+last;
      for(i = 0; i < p_poly->p_lightnum[l]; i++) {
        glTexCoord2fv(&p_koord[i].tu1);
        glColor4fv(&p_koord[i].dr);
        glVertex3fv(&p_koord[i].x);
      }
      glEnd();
      last += p_poly->p_lightnum[l];
    }
  }
}

inline void ber_kresli_poly(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat)
{
  int  kamera_zmena = p_ber->kamera.zmena||p_ber->kamera.zmena_last;

  /* Pripadne vypnuti mlhy
  */
  if(p_poly->kflag&KONT_NO_FOG) {
    disable_fog_causal();
  } else {
    /* Pripadne zapnuti mlhy
    */
    if(p_poly->p_mlha) {
      if(!p_ber->p_mlha_akt)
        enable_fog();
      nastav_fog(p_poly->p_mlha);
    }
  }

#ifdef DEBUG_MOD_OBALKY
  ber_kresli_obalku_poly(p_poly);
#endif

  /* Env-spec mapping
  */
  if(p_poly->m2flag&MAT2_ENV_SPEC && kamera_zmena)
    poly_env_maping_spec(p_poly, p_mat[p_poly->material], get_matrix_camera_point());

  /* Rednering poly-listu
  */  
  ber_kresli_poly_v1(p_poly,p_mat);

  /* Pripadne zapnuti mlhy
  */
  if(p_poly->kflag&KONT_NO_FOG) {
    enable_fog_causal();
  } else {
    /* Pripadne vypnuti mlhy
    */
    if(p_poly->p_mlha) {
      if(p_ber->p_mlha_akt) {
        nastav_fog(p_ber->p_mlha_akt);
      } else {
        disable_fog();
      }
    }
  }
}

/* Rendering fleku
*/
void ber_kresli_fleky(G_KONFIG *p_ber, FLEK *p_topflek)
{
  BOD  *p_pos, *p_nx, *p_nz;
  FLEK *p_flek = p_topflek;

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-1.0f,p_flek->bias);
  
  zmask_set_fleky(FALSE);
  
  while(p_flek) {
    if(!(p_flek->flag&FLEK_NEKRESLIT)) {
      ber_nastav_material_single(p_ber->p_smat,p_ber->p_mat[p_flek->material],FALSE);      
  /*    
      glColor3f(1.0f,1.0f,1.0f);
      if(p_flek->mesh != K_CHYBA) {
        set_matrix_world(&p_ber->p_mesh[p_flek->mesh]->m);
      }
*/
      p_pos = &p_flek->p;
      p_nx = &p_flek->nx;
      p_nz = &p_flek->nz;
      
      glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(p_flek->u[0],p_flek->v[0]);
      glVertex3f(p_pos->x-p_nx->x-p_nz->x,
                 p_pos->y-p_nx->y-p_nz->y,
                 p_pos->z-p_nx->z-p_nz->z);
      
      glTexCoord2f(p_flek->u[1],p_flek->v[1]);
      glVertex3f(p_pos->x+p_nx->x-p_nz->x,
                 p_pos->y+p_nx->y-p_nz->y,
                 p_pos->z+p_nx->z-p_nz->z);
      
      glTexCoord2f(p_flek->u[3],p_flek->v[3]);
      glVertex3f(p_pos->x-p_nx->x+p_nz->x,
                 p_pos->y-p_nx->y+p_nz->y,
                 p_pos->z-p_nx->z+p_nz->z);
      
      glTexCoord2f(p_flek->u[2],p_flek->v[2]);
      glVertex3f(p_pos->x+p_nx->x+p_nz->x,
                 p_pos->y+p_nx->y+p_nz->y,
                 p_pos->z+p_nx->z+p_nz->z);
      glEnd();    
    }
    p_flek = p_flek->p_next;
  }
  
  glDisable(GL_POLYGON_OFFSET_FILL);
  zmask_set_fleky(TRUE);
}

/* Porovnani pruhlednych objektu
*/
static int ber_renderuj_scenu_pruhledne_compare(const void *p_r1, const void *p_r2)
{
  return(ftoi((((PRUHLEDNY_OBJEKT *)p_r2)->vzdal-((PRUHLEDNY_OBJEKT *)p_r1)->vzdal)*1000.0f));
}

/* Render hierarchie pruhlednych objektu
*/
void ber_renderuj_scenu_pruhledne(G_KONFIG *p_ber)
{
  int i,last = K_CHYBA;


  if(p_ber->prhlnum) {
    qsort(p_ber->prhl,p_ber->prhlnum,sizeof(p_ber->prhl[0]), ber_renderuj_scenu_pruhledne_compare);
    for(i = 0; i < p_ber->prhlnum; i++) { // render pruhlednych objektu odzadu
      switch(p_ber->prhl[i].typ) {
      case PRUHLEDNY_MESH:
        if(last != PRUHLEDNY_MESH) {
          last = PRUHLEDNY_MESH;
          __p_last_mat = NULL;
        }
        ber_kresli_mesh((GAME_MESH *)p_ber->prhl[i].p_objekt,p_ber->p_mat);
        break;
      case PRUHLEDNY_POLY:
        if(last != PRUHLEDNY_POLY) {
          last = PRUHLEDNY_POLY;
          ber_render_polylistu_start();
          __p_last_mat = NULL;
        }
        ber_kresli_poly((EDIT_MESH_POLY *)p_ber->prhl[i].p_objekt, p_ber->p_mat);
        break;
      default:
        break;
      }
    }    
  }
}

void ber_renderuj_scenu_obyc(int zrc)
{
 EDIT_MESH_POLY *p_poly;
 GAME_MESH      *p_mesh;
 int             flag = zrc ? KONT_VIDITELNY_ZRC : KONT_VIDITELNY;

 /* Vyrobi list pruhlednosti
 */
 ber_render_list_vyrob_pruhlist(p_ber);

 /* Reset posledniho materialu
 */
 __p_last_mat = NULL;
 
 #ifdef DEBUG_MOD
   if(p_ber->debug_draty)
     glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
 #endif

 /*
   Render levelu - budu kreslit podle listu meshu
 */ 
 ber_mesh_render_list_reset(p_ber);  
 while(p_mesh = ber_mesh_render_list_next_flag(p_ber,flag,KONT_VIDITELNY_PRUHL)) {
//   if(p_mesh->p_data->mesh_handle != 0 && p_mesh->p_data->mesh_handle != 2)
      ber_kresli_mesh(p_mesh,p_ber->p_mat);
 }

 /* Reset posledniho materialu
 */
 __p_last_mat = NULL;

 /*
   Render poly-listu
 */   
 ber_render_polylistu_start();
 ber_poly_render_list_reset(p_ber);
 while(p_poly = ber_poly_render_list_next_flag(p_ber,flag,KONT_VIDITELNY_PRUHL)) {
   ber_kresli_poly(p_poly, p_ber->p_mat);
 } 

  __p_last_mat = NULL;

 /* Zjisti viditelne flare
 */ 
 if(p_ber->p_flare)
   ber_viditelnost_flare(p_ber, p_ber->p_flare);

 /* Reset posledniho materialu
 */

 /* Kresli fleky sceny
 */
 if(!zrc) {
   set_matrix_world_init();
   disable_fog_causal();
   if(p_ber->p_fleky)
     ber_kresli_fleky(p_ber,p_ber->p_fleky);
   if(p_ber->p_fleky_mesh)
     ber_kresli_fleky(p_ber,p_ber->p_fleky_mesh);
   enable_fog_causal();
 }

 
 /* Render casticoveho systemu
 */
 pe_renderuj(p_ber);


 /* Vykresli viditelne flare
 */
 if(p_ber->p_flare) {
   disable_fog_causal();
   ber_kresli_flare(p_ber, p_ber->p_flare);
   enable_fog_causal();
 }

 /*   
   Hromadny render pruhlednych objektu
 */ 
 ber_renderuj_scenu_pruhledne(p_ber);
 
 #ifdef DEBUG_MOD
   if(p_ber->debug_draty)
     glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
 #endif
}

void ber_calc_clip_focus(G_KONFIG *p_ber)
{
  BOD cp,*p_a,*p_b,*p_c,*p_d;
  
  cp.x = p_ber->p_invcam->_41;
  cp.y = p_ber->p_invcam->_42;
  cp.z = p_ber->p_invcam->_43;

  p_a = p_ber->zrc.p;
  p_b = p_ber->zrc.p+1;
  p_c = p_ber->zrc.p+2;
  p_d = p_ber->zrc.p+3;
  
  calc_drovinu_bod( p_b, p_a, &cp, p_ber->zrc.r);
  calc_drovinu_bod( p_d, p_b, &cp, p_ber->zrc.r+1);
  calc_drovinu_bod( p_c, p_d, &cp, p_ber->zrc.r+2);
  calc_drovinu_bod( p_a, p_c, &cp, p_ber->zrc.r+3);
  calc_drovinu_bod( p_a, p_c, p_b, p_ber->zrc.r+4);
}

void ber_zrcadlo_nastav(void)
{
  //  Nastaveni propriet k zrcadlu -> klip roviny a pod.
  ber_calc_clip_focus(p_ber);
  
  assert(GL_MAX_CLIP_PLANES >= 4);

  glEnable(GL_CLIP_PLANE0);
  glEnable(GL_CLIP_PLANE1);
  glEnable(GL_CLIP_PLANE2);
  glEnable(GL_CLIP_PLANE3);
  glEnable(GL_CLIP_PLANE4);
  
  glClipPlane(GL_CLIP_PLANE0, (double *)(p_ber->zrc.r));
  glClipPlane(GL_CLIP_PLANE1, (double *)(p_ber->zrc.r+1));
  glClipPlane(GL_CLIP_PLANE2, (double *)(p_ber->zrc.r+2));
  glClipPlane(GL_CLIP_PLANE3, (double *)(p_ber->zrc.r+3));
  glClipPlane(GL_CLIP_PLANE4, (double *)(p_ber->zrc.r+4));
  
  cull_front();

  set_matrix_world_top(&p_ber->zrc.ref);
  set_matrix_camera_project(p_ber->p_project);
}

void ber_zrcadlo_zrus(void)
{
  //  Zruseni klipovacich rovin
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
  glDisable(GL_CLIP_PLANE4);

  //  Zruseni cullingu
  cull_back();

  ret_matrix_world_top();
  set_matrix_camera_project(p_ber->p_project);

  // Reset materialu
  __p_last_mat = NULL;

  //  Vykresleni zrcadloveho poly
  {
    ZDRCADLO_DESC_POLY *p_poly = p_ber->zrc.p_poly;    
    int p;
    while(p_poly) {
      p = p_poly->poly;
      if(p != K_CHYBA && p_ber->p_poly[p].kflag&KONT_ZRCADLO) {
        ber_render_polylistu_start();
        ber_kresli_poly(p_ber->p_poly+p, p_ber->p_mat);
      }
      p_poly = p_poly->p_next;
    }
  }
}

void ber_kresli_kofoli_cary(void)
{
  KOFOLOVA_CARA *p_cara;
  int i;

  if(p_ber->car_a_kouzel && p_ber->p_cary) {

    deph_test_set(0);

    glBegin(GL_LINES);
    for(i = 0; i < p_ber->car_a_kouzel; i++) {
      p_cara = p_ber->p_cary+i;
      glColor3fv((float *)&p_cara->b1);
      glVertex3fv((float *)&p_cara->p1);
      glColor3fv((float *)&p_cara->b2);
      glVertex3fv((float *)&p_cara->p2);
    }
    glEnd();

    deph_test_set(1);
  }
}

void ber_reset_render(void)
{
  int i;

  glstav_text_akt = K_CHYBA;

  blend_off();
  specular_off();
  set_alfa_blok(0);

  zmask_set(TRUE);

  for(i = 0; i < MAT_TEXTUR; i++) {
    if(text_set_num(i)) {
      text_off(GL_TEXTURE_1D);
      text_off(GL_TEXTURE_2D);
      glstav_textury_1d[i] = FALSE;
      glstav_textury_2d[i] = FALSE;
      glstav_last_text_1d[i] = FALSE;
      glstav_last_text_2d[i] = FALSE;
      glstav_text_blok[i] = K_CHYBA;
    }
  }
}

/*
  Redering:

  1. zrcadlova scena
  2. deska zdcadla pruhedne + z_bufer on
  3. vrchni scena
*/

void ber_renderuj_scenu(void)
{
 static  int mail = 0;
 int     kamera_zmena = p_ber->kamera.zmena||p_ber->kamera.zmena_last;

 #ifdef DEBUG_MOD
   memset(&p_ber->debug,0,sizeof(p_ber->debug));
 #endif

 if(!p_ber->conf_menu_render) {
   
   /* Reset nastaveni textur & stage
   */
   ber_reset_render();
   text_sharp(txconf.text_ostrost);
   
   /* Animace kamer (Polarni/3ds)
   */
   if(p_ber->kamera.aktivni&GAME_KAMERA_ANIMACE)
     kani_updatuj(p_ber);
   
   /* Urci viditelnost meshu
   */
   ber_render_list_vyrob(p_ber, p_ber->zrc_akt, kamera_zmena);
   
   /* Update materialovych animaci
   */
   amat_updatuj(p_ber);
   
   /* Update animace textur
   */
   tani_updatuj(p_ber);
   
   /* Update globalni animace
   */
   rani_updatuj(p_ber);
   
   /* Updatuje mesh-key-frame animaci
   */
   lani_updatuj(p_ber);
   
   /* Update chapadelniku
   */
   chani_updatuj(p_ber);
   
   /* Update casticovych systemu
   */
   pe_updatuj(p_ber);
   
   /* Animace svetel
   */
   sdla_updatuj(p_ber);
   edla_updatuj(p_ber);
   
   /* Update dynamickych & statickych svetel
   */
   dl_updatuj(p_ber);
   
   /* Update vodniho systemu
   */
   vod_updatuj(p_ber->p_voda);
   
   /* Zapni/povol rendering mlhy
   */
   ber_renderuj_mlhu(p_ber);
   
   /* Zpruhledneni prvku pred kamerou
   */
   ber_zpruhledni_prvky(p_ber);
   
   /* Zpruhledneni objekty pred kamerou
   */
   ber_kamera_zpruhledni_objekty(p_ber);

   if(karmin_aktivni) {
     
     glClear(GL_DEPTH_BUFFER_BIT);
     if(p_ber->conf_barva_pozadi_pouzit)
       glClear(GL_COLOR_BUFFER_BIT);
     
     set_matrix_world_init();
     
     #ifdef DEBUG_MOD_OBALKY
       obbtree_kresli(&p_ber->obbtree,DDRGB(1,0,0));
     #endif
     
     vertex_array_start_render();
     
     if(p_ber->zrc_akt) {
       ber_zrcadlo_nastav();
       ber_renderuj_scenu_obyc(TRUE);
       ber_zrcadlo_zrus();
     }   
     ber_renderuj_scenu_obyc(FALSE);
     
     vertex_array_stop_render();
     
     ber_reset_render();
     
     set_matrix_world_init();
     ber_kresli_kofoli_cary();
     
     #ifdef DEBUG_MOD
       set_matrix_2d(OXRES,OYRES);
       ber_tiskni_statistiku(p_ber);
       ret_matrix_2d();
     
       if(p_ber->debug_light)
         dl_renderuj_svetla(p_ber);
     
       //ber_test_render_mysi();
     #endif
     
     /* Nastaveni flagu zmeny kamery
     */
     p_ber->kamera.zmena_last = p_ber->kamera.zmena;
     p_ber->kamera.zmena = FALSE;
     reset_kamera_flag();
     
     /* Vypnuti scale-faktoru texturoveho filtru
     */
     text_sharp(0.0f);
   }
 }

 /* Update fps
 */
 ber_updatuj_fps(p_ber);

 /* Kofolovo nastaveni
 */
 text_set_num(0);
 text_on(GL_TEXTURE_2D);
 text_set_blok(0,BLOK_MODULACE);
}

/* Prevzato z DX7 SDK */
void ber_updatuj_fps(G_KONFIG *p_ber)
{  
#ifdef DEBUG_MOD
  static dword akt = 0;
  static dword end = 0;
  static dword framu = 0;
#endif
  static dword time[10] = {0,0,0,0,0,0,0,0,0,0};
  static dword fin_time = 0;

  fin_time = timeGetTime()/DELICKA_CASU;
  p_ber->TimeLastFrame =    fin_time - p_ber->TimeEndLastFrame;
  p_ber->TimeEndLastFrame = fin_time;
  p_ber->frame++;
  
#ifdef DEBUG_MOD  
  framu++;
  if(fin_time - end > 1000) {
    p_ber->fps = (1000*framu)/(float)(fin_time - end);
    end = fin_time;
    framu = 0;
  }
#endif
}


/* Rendering castic
*/
void pe_kresli_strepiny(G_KONFIG *p_ber, PARMETAC *p_par)
{
  PAR_STREPINA  *p_part;
  EDIT_MATERIAL *p_mat;
  GLMATRIX      *p_cam = get_matrix_camera_point();
  BOD            up,rg;
  float          r1x,r1y,r1z;
  float          r2x,r2y,r2z;
  float          r3x,r3y,r3z;
  float          r4x,r4y,r4z;
  float          x,y,z;
  float          v1,v2,v3,v4;
  int            frame,framelast;

  up.x = p_cam->_12; up.y = p_cam->_22; up.z = p_cam->_32;
  rg.x = p_cam->_11; rg.y = p_cam->_21; rg.z = p_cam->_31;
  
  r1x = -up.x-rg.x;
  r1y = -up.y-rg.y;
  r1z = -up.z-rg.z;

  r2x = -up.x+rg.x;
  r2y = -up.y+rg.y;
  r2z = -up.z+rg.z;

  r3x =  up.x-rg.x;
  r3y =  up.y-rg.y;
  r3z =  up.z-rg.z;

  r4x =  up.x+rg.x;
  r4y =  up.y+rg.y;
  r4z =  up.z+rg.z;

  if(p_par->flag&TPAR_NO_FOG)
    disable_fog_causal();

  p_mat = p_ber->p_mat[p_par->material];

  ber_nastav_material_single(NULL,p_mat,FALSE);

  frame = p_par->flag&TPAR_FRAME;
  if(frame) {
    framelast = amat_get_frame(p_mat);
    frame = (framelast != K_CHYBA);
  }

  p_part = (PAR_STREPINA *)p_par->p_first;
  while(p_part) {

    x = p_part->p.x;
    y = p_part->p.y;
    z = p_part->p.z;

    v1 = p_part->vel1;
    v2 = p_part->vel2;
    v3 = p_part->vel3;
    v4 = p_part->vel4;

    if(frame)
      amat_set_frame_text(p_mat, (int)floor(p_part->frame));

    glColor3f(p_part->barva,p_part->barva,p_part->barva);

    glBegin(GL_TRIANGLE_STRIP);        
      glTexCoord2f(p_part->u1,p_part->v1);
      glVertex3f(x+r1x*v1,y+r1y*v1,z+r1z*v1);
      glTexCoord2f(p_part->u2,p_part->v2);
      glVertex3f(x+r2x*v2,y+r2y*v2,z+r2z*v2);
      glTexCoord2f(p_part->u3,p_part->v3);
      glVertex3f(x+r3x*v3,y+r3y*v3,z+r3z*v3);
      glTexCoord2f(p_part->u4,p_part->v4);
      glVertex3f(x+r4x*v4,y+r4y*v4,z+r4z*v4);
    glEnd();

    p_part = p_part->p_next;
  }

  if(frame)
    amat_set_frame_text(p_mat, framelast);

  if(p_par->flag&TPAR_NO_FOG)
    enable_fog_causal();
}

void pe_kresli_fleky(G_KONFIG *p_ber, PARMETAC *p_par)
{
  EDIT_MATERIAL *p_mat;
  PAR_FLEK      *p_part;  
  BOD           *p_bod;
  int            frame,framelast;

  if(p_par->flag&TPAR_NO_FOG)
    disable_fog_causal();

  p_mat = p_ber->p_mat[p_par->material];

  frame = p_par->flag&TPAR_FRAME;
  if(frame) {
    framelast = amat_get_frame(p_mat);
    frame = (framelast != K_CHYBA);
  }
    
  ber_nastav_material_single(p_ber->p_smat,p_mat,FALSE);
  p_part = (PAR_FLEK *)p_par->p_first;
  while(p_part) {
    if(frame)
      amat_set_frame_text(p_mat, (int)floor(p_part->frame));
    glColor4f(p_part->r,p_part->g,p_part->b,p_part->a);
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0,0);
      p_bod = p_part->tmp;
      glVertex3f(p_bod->x,p_bod->y,p_bod->z);
      
      glTexCoord2f(1,0);
      p_bod = p_part->tmp+1;
      glVertex3f(p_bod->x,p_bod->y,p_bod->z);
      
      glTexCoord2f(0,1);
      p_bod = p_part->tmp+2;
      glVertex3f(p_bod->x,p_bod->y,p_bod->z);
      
      glTexCoord2f(1,1);
      p_bod = p_part->tmp+3;
      glVertex3f(p_bod->x,p_bod->y,p_bod->z);
    glEnd();
    p_part = p_part->p_next;
  }
  if(frame)
    amat_set_frame_text(p_mat, framelast);

  if(p_par->flag&TPAR_NO_FOG)
    enable_fog_causal();
}



/* Kresli coudy
*/
void pe_kresli_kour_stopu(G_KONFIG *p_ber, PARMETAC *p_mt)
{
/*
  GLMATRIX        *p_camera_project = get_matrix_camera_project();
  PARMETAC_HNIZDO *p_hnizdo;
  EDIT_MATERIAL   *p_mat;
  PAR_KOUR_STOPA  *p_par;
  BOD             *p_bod,tmp[4],nx,ny,zup,up,zrg,cp,rg;
  float            b1x,b1z,b2x,b2z,b3x,b3z,b4x,b4z;
  float            b1y,b2y,b3y,b4y;
  float            sx,sz;
  float            tu,tv;  
  float            min_x,max_x,min_z,max_z;
  int              frame,framelast;
  int              clip  = p_mt->flag&TPAR_2DCLIP;
  int              par3d = p_mt->flag&TPAR_3D;
  int              parhl = p_mt->flag&TPAR_HALF_LIFE;
  
  if(p_mt->flag&TPAR_NO_FOG)
    disable_fog_causal();

  p_mat = p_ber->p_mat[p_mt->material];

  frame = p_mt->flag&TPAR_FRAME;
  if(frame) {
    framelast = amat_get_frame(p_mat);
    frame = (framelast != K_CHYBA);
  }

  if(!par3d) {
    GLMATRIX *p_cam = get_matrix_world_camera();
    zup.x = p_cam->_12; zup.y = p_cam->_22; zup.z = p_cam->_32;
    zrg.x = p_cam->_11; zrg.y = p_cam->_21; zrg.z = p_cam->_31;
  }  

  if(parhl) {
    GLMATRIX inv;
    invert_matrix(get_matrix_world_camera(),&inv);
    cp.x = inv._41;
    cp.y = inv._42;
    cp.z = inv._43;
  }
  
  ber_nastav_material_single(NULL,p_mat,FALSE);
  zmask_set_fleky(FALSE);
  cull_off();

  #ifdef DEBUG_MOD
    p_ber->debug.hnizd = 0;
    p_ber->debug.hnizd_vid = 0;
  #endif
  
  // Prejedu pres vsechny hnizda
  p_hnizdo = p_mt->p_hnizdo;
  while(p_hnizdo) {
    
    #ifdef DEBUG_MOD_OBALKY
      kresli_oktanovou_bunku_minmax(&p_hnizdo->min, &p_hnizdo->max, DDRGBA(0,0,0,1));
    #endif
    
    #ifdef DEBUG_MOD
      p_ber->debug.hnizd++;
    #endif
    
    if(kd_visibility(&p_hnizdo->min, &p_hnizdo->max, p_camera_project)) {
    #ifdef DEBUG_MOD
      p_ber->debug.hnizd_vid++;    
    #endif
      p_par = (PAR_KOUR_STOPA *)p_hnizdo->p_first;
      while(p_par) {
        if(frame)
          amat_set_frame_text(p_mat, (int)floor(p_par->frame));

        if(!par3d) { // 2d fleky natocene ke kamere - Vypocti nove hodnoty
          vektor_mult_skalar(&zup,p_par->rychlost_x,&up);
          vektor_mult_skalar(&zrg,p_par->rychlost_y,&rg);
                    
          vektor_sub(vektor_sub(&p_par->rp,&up,tmp),  &rg, tmp);
          vektor_add(vektor_sub(&p_par->rp,&up,tmp+1),&rg, tmp+1);
          vektor_sub(vektor_add(&p_par->rp,&up,tmp+2),&rg, tmp+2);
          vektor_add(vektor_add(&p_par->rp,&up,tmp+3),&rg, tmp+3);
        } else if(par3d || parhl) {  // par3d
          if(parhl) {
            BOD t1;
            vektor_soucin(&p_par->nx,vektor_sub(&cp,&p_par->rp,&t1),&ny);
            vektor_norm(&ny);
            vektor_scale(&ny,p_par->rychlost_y);
            vektor_mult_skalar(&p_par->nx,p_par->rychlost_x,&nx);
          } else {
            vektor_mult_skalar(&p_par->nx,p_par->rychlost_x,&nx);
            vektor_mult_skalar(&p_par->ny,p_par->rychlost_y,&ny);
          }

          vektor_sub(vektor_sub(&p_par->rp,&nx,tmp),  &ny, tmp);
          vektor_sub(vektor_add(&p_par->rp,&nx,tmp+1),&ny, tmp+1);
          vektor_add(vektor_sub(&p_par->rp,&nx,tmp+2),&ny, tmp+2);
          vektor_add(vektor_add(&p_par->rp,&nx,tmp+3),&ny, tmp+3);
        }

        glColor4f(p_par->r,p_par->g,p_par->b,p_par->a);
        if(!clip) {          
          glBegin(GL_TRIANGLE_STRIP);
          if(!p_par->rotace) { // rotace textury na castici
            glTexCoord2f(0,0);
            glVertex3fv((float *)(tmp));
            glTexCoord2f(1,0);
            glVertex3fv((float *)(tmp+1));
            glTexCoord2f(0,1);
            glVertex3fv((float *)(tmp+2));
            glTexCoord2f(1,1);
            glVertex3fv((float *)(tmp+3));
          } else if(p_par->rotace == 3) {
            glTexCoord2f(0,1);
            glVertex3fv((float *)(tmp));
            glTexCoord2f(0,0);
            glVertex3fv((float *)(tmp+1));
            glTexCoord2f(1,1);
            glVertex3fv((float *)(tmp+2));
            glTexCoord2f(1,0);
            glVertex3fv((float *)(tmp+3));
          } else if(p_par->rotace == 2) {
            glTexCoord2f(0,1);
            glVertex3fv((float *)(tmp));
            glTexCoord2f(1,1);
            glVertex3fv((float *)(tmp+1));
            glTexCoord2f(0,0);
            glVertex3fv((float *)(tmp+2));
            glTexCoord2f(1,0);
            glVertex3fv((float *)(tmp+3));
          } else if(p_par->rotace == 1) {
            glTexCoord2f(1,0);
            glVertex3fv((float *)(tmp));
            glTexCoord2f(1,1);
            glVertex3fv((float *)(tmp+1));
            glTexCoord2f(0,0);
            glVertex3fv((float *)(tmp+2));
            glTexCoord2f(0,1);
            glVertex3fv((float *)(tmp+3));
          }
          glEnd();          
        } else {
          min_x = p_par->x_min;
          max_x = p_par->x_max;
          min_z = p_par->z_min;
          max_z = p_par->z_max;
          p_bod = tmp;
          b1x = p_bod->x;
          b1y = p_bod->y;
          b1z = p_bod->z;
          p_bod = tmp+1;
          b2x = p_bod->x;
          b2y = p_bod->y;
          b2z = p_bod->z;
          p_bod = tmp+2;
          b3x = p_bod->x;
          b3y = p_bod->y;
          b3z = p_bod->z;
          p_bod = tmp+3;
          b4x = p_bod->x;
          b4y = p_bod->y;
          b4z = p_bod->z;
          sx = p_par->p.x;
          sz = p_par->p.z;
          
          if(b1x < min_x && b2x < min_x && b3x < min_x && b4x < min_x) {
            p_par = p_par->p_next;
            continue;
          }
          if(b1x > max_x && b2x > max_x && b3x > max_x && b4x > max_x) {
            p_par = p_par->p_next;
            continue;
          }
          if(b1z < min_z && b2z < min_z && b3z < min_z && b4z < min_z) {
            p_par = p_par->p_next;
            continue;
          }
          if(b1z > max_z && b2z > max_z && b3z > max_z && b4z > max_z) {
            p_par = p_par->p_next;
            continue;
          }

          glBegin(GL_TRIANGLE_STRIP);

          if(b1x < min_x) {
            tu = 0.5f-0.5f*(sx-min_x)/(sx-b1x);
            b1x = min_x;
          } else if(b1x > max_x) {
            tu = 0.5f+0.5f*(max_x-sx)/(b1x-sx);
            b1x = max_x;            
          } else {
            tu = 0.0f;
          }
          
          if(b1z < min_z) {
            tv = 0.5f-0.5f*(sz-min_z)/(sz-b1z);
            b1z = min_z;
          } else if(b1z > max_z) {
            tv = 0.5f+0.5f*(max_z-sz)/(b1z-sz);
            b1z = max_z;            
          } else {
            tv = 0.0f;
          }

          glTexCoord2f(tu,tv);
          glVertex3f(b1x,b1y,b1z);

          if(b2x < min_x) {
            tu = 0.5f-0.5f*(sx-min_x)/(sx-b2x);
            b2x = min_x;
          } else if(b2x > max_x) {
            tu = 0.5f+0.5f*(max_x-sx)/(b2x-sx);
            b2x = max_x;            
          } else {
            tu = 1.0f;
          }
          
          if(b2z < min_z) {
            tv = 0.5f-0.5f*(sz-min_z)/(sz-b2z);
            b2z = min_z;
          } else if(b2z > max_z) {
            tv = 0.5f+0.5f*(max_z-sz)/(b2z-sz);
            b2z = max_z;            
          } else {
            tv = 0.0f;
          }
          
          glTexCoord2f(tu,tv);
          glVertex3f(b2x,b2y,b2z);
          
          if(b3x < min_x) {
            tu = 0.5f-0.5f*(sx-min_x)/(sx-b3x);
            b3x = min_x;
          } else if(b3x > max_x) {
            tu = 0.5f+0.5f*(max_x-sx)/(b3x-sx);
            b3x = max_x;            
          } else {
            tu = 0.0f;
          }
          
          if(b3z < min_z) {
            tv = 0.5f-0.5f*(sz-min_z)/(sz-b3z);
            b3z = min_z;
          } else if(b3z > max_z) {
            tv = 0.5f+0.5f*(max_z-sz)/(b3z-sz);
            b3z = max_z;            
          } else {
            tv = 1.0f;
          }

          glTexCoord2f(tu,tv);
          glVertex3f(b3x,b3y,b3z);

          if(b4x < min_x) {
            tu = 0.5f-0.5f*(sx-min_x)/(sx-b4x);
            b4x = min_x;
          } else if(b4x > max_x) {
            tu = 0.5f+0.5f*(max_x-sx)/(b4x-sx);
            b4x = max_x;
          } else {
            tu = 1.0f;
          }
          
          if(b4z < min_z) {
            tv = 0.5f-0.5f*(sz-min_z)/(sz-b4z);
            b4z = min_z;
          } else if(b4z > max_z) {
            tv = 0.5f+0.5f*(max_z-sz)/(b4z-sz);
            b4z = max_z;
          } else {
            tv = 1.0f;
          }

          glTexCoord2f(tu,tv);
          glVertex3f(b4x,b4y,b4z);

          glEnd();        
        }
        p_par = p_par->p_next;
      }
    }
    p_hnizdo = p_hnizdo->p_next;
  }

  if(frame)
    amat_set_frame_text(p_mat, framelast);  

  cull_on();
  zmask_set_fleky(TRUE);

  if(p_mt->flag&TPAR_NO_FOG)
    enable_fog_causal();
*/
}

void pe_renderuj(G_KONFIG *p_ber)
{
  GLMATRIX *p_camera_project = get_matrix_camera_project();
  PARMETAC *p_par = p_ber->p_par;
  PARMETAC *p_next = NULL;

  set_matrix_world_init();
  /*
  while(p_par) {    
    p_next = p_par->p_next;
    
    if(p_par->flag&TPAR_STREP) {
      if(p_par->pnum && kd_visibility(&p_par->min, &p_par->max, p_camera_project))
        pe_kresli_strepiny(p_ber, p_par);
    }
    else if(p_par->flag&TPAR_KOUR_STOPA) {
      if(p_par->pnum)
        pe_kresli_kour_stopu(p_ber,p_par);
    }
    else if(p_par->flag&TPAR_FLEK) {
      if(p_par->pnum && kd_visibility(&p_par->min, &p_par->max, p_camera_project))
        pe_kresli_fleky(p_ber, p_par);
    }
    else
      assert(0);
    
    p_par = p_next;
  }
  */
}

void ber_renderuj_mlhu(G_KONFIG *p_ber)
{  
  MLZNA_KOSTKA *p_tmp = p_ber->p_mlha;
  MLZNA_KOSTKA *p_top = NULL;
  BOD *p_cbod = (BOD *)&p_ber->p_invcam->_41;
  int  kamera_zmena = p_ber->kamera.zmena||p_ber->kamera.zmena_last;

  if(kamera_zmena) {
    while(p_tmp) {
      if(!p_tmp->p_kont && kd_je_bod_v_kostce(p_cbod,&p_tmp->min,&p_tmp->max) &&
        (!p_top || p_tmp->priorita >= p_top->priorita)) {
        p_top = p_tmp;
      }    
      p_tmp = p_tmp->p_next;
    }
    
    if(p_top) {
      if(p_ber->p_mlha_akt != p_top) {
        p_ber->p_mlha_akt = p_top;
        nastav_fog(p_top);
        enable_fog();
      }
    } else {
      if(p_ber->p_mlha_akt) {
        p_ber->p_mlha_akt = NULL;
        disable_fog();
      }
    }
  }
}


void ber_test_render_mysi(void)
{
 BOD   a,b,c,v(10,10,10);
 int   ax,ay,bx,by,cx,cy;
 float z1,z2,z3;
 
 transf_2d_3d_z((float)mi.x, (float)mi.y, 500.0f, &a.x, &a.y, &a.z,
                     p_ber->p_camera,p_ber->p_project, 
                     p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy,
                     p_ber->kam.near_plane,p_ber->kam.far_plane);
 transf_2d_3d_z((float)mi.x, (float)mi.y, 250.0f, &b.x, &b.y, &b.z,
                     p_ber->p_camera,p_ber->p_project, 
                     p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy,
                     p_ber->kam.near_plane,p_ber->kam.far_plane);
 transf_2d_3d_z((float)mi.x, (float)mi.y, 0.01f, &c.x, &c.y, &c.z,
                     p_ber->p_camera,p_ber->p_project,
                     p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy,
                     p_ber->kam.near_plane,p_ber->kam.far_plane);

 dbgprintf("m [%d %d]",mi.x,mi.y);
 dbgprintf("a3 [%f %f %f]",a.x,a.y,a.z);
 dbgprintf("b3 [%f %f %f]",b.x,b.y,b.z);
 dbgprintf("c3 [%f %f %f]",c.x,c.y,c.z);

 set_matrix_world_init();

 kresli_oktanovou_bunku_minmax(&a, &v, 0xffffffff);

 kresli_kosoctverec(p_ber->p_project, p_ber->p_camera, get_matrix_init(), 
                   &a, 0.5f,1.0f,0.0f,0.0f);
 kresli_kosoctverec(p_ber->p_project, p_ber->p_camera, get_matrix_init(), 
                   &b, 0.5f,0.0f,1.0f,0.0f);
 kresli_kosoctverec(p_ber->p_project, p_ber->p_camera, get_matrix_init(), 
                   &c, 0.5f,0.0f,0.0f,1.0f);
 

 transf_3d_2d(a.x,a.y,a.z,&ax,&ay,&z1,get_matrix_init(),
              p_ber->p_camera,p_ber->p_project,
              p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy,
              p_ber->kam.near_plane,p_ber->kam.far_plane);
 transf_3d_2d(b.x,b.y,b.z,&bx,&by,&z2,get_matrix_init(),
              p_ber->p_camera,p_ber->p_project,
              p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy,
              p_ber->kam.near_plane,p_ber->kam.far_plane);
 transf_3d_2d(c.x,c.y,c.z,&cx,&cy,&z3,get_matrix_init(),
              p_ber->p_camera,p_ber->p_project,
              p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy,
              p_ber->kam.near_plane,p_ber->kam.far_plane);

 dbgprintf("a2 [%d %d %f]",ax,ay,z1);
 dbgprintf("b2 [%d %d %f]",bx,by,z2);
 dbgprintf("c2 [%d %d %f]",cx,cy,z3);

 set_matrix_2d(p_ber->wxres, p_ber->wyres);
 kresli_ctverec_2d_diff(ax,ay,10,10,1,1,1);
 kresli_ctverec_2d_diff(bx,by,20,20,1,1,1);
 kresli_ctverec_2d_diff(cx,cy,30,30,1,1,1);
 ret_matrix_2d();
}

GAME_MESH * ber_vyrob_stinove_teleso(GAME_MESH *p_mesh, BOD *p_light)
{


 return(NULL);
}

static int mesh_mat_num(GAME_MESH *p_mesh)
{
/*
  int i;
  for(i = 1; i < p_mesh->objektu; i++) {
    if(p_mesh->p_mat[i-1] != p_mesh->p_mat[i])
      return(K_CHYBA);
  }
  return((int)p_mesh->p_mat[0]);
*/
}

static int ber_mesh_render_list_setrid_compare(const void *p_m1, const void *p_m2)
{
  int mat1 = mesh_mat_num(**(GAME_MESH ***)p_m1),
      mat2 = mesh_mat_num(**(GAME_MESH ***)p_m2);
  if(mat1 == K_CHYBA && mat2 == K_CHYBA)
    return(0);
  else {
    if(mat1 == K_CHYBA)
      return(1);
    if(mat2 == K_CHYBA)
      return(-1);
    return(mat1-mat2);
  }
}

static int ber_poly_render_list_setrid_compare(const void *p_m1, const void *p_m2)
{
  EDIT_MESH_POLY *p_p1 = *(EDIT_MESH_POLY **)p_m1;
  EDIT_MESH_POLY *p_p2 = *(EDIT_MESH_POLY **)p_m2;
  return(p_p1->material - p_p2->material);
}

void ber_render_list_setrid(G_KONFIG *p_ber)
{
  qsort(p_ber->p_mesh_renderlist,p_ber->mesh_rendernum,sizeof(p_ber->p_mesh_renderlist[0]),ber_mesh_render_list_setrid_compare);
  qsort(p_ber->p_poly_renderlist,p_ber->poly_rendernum,sizeof(p_ber->p_poly_renderlist[0]),ber_poly_render_list_setrid_compare);
}

inline int obb_visibility_flag(OBB *p_obb, GLMATRIX *p_mat, int viz, int flag)
{
//  return(viz ? flag : obb_visibility(p_obb,p_mat));
}

// viz = TRUE - automaticky viditelny
inline int ber_render_list_vyrob_mesh(G_KONFIG *p_ber, int viz, GAME_MESH **p_src, int add, GLMATRIX *p_mat, int viditelny_flag, int zmena_flag, int mail, int kamera_zmena)
{
  GAME_MESH *p_mesh = *p_src;
  int *p_flag,kflag;
  OBB *p_obb;
  int  j;
  int  viditelny_zmena = FALSE;  
/*
  kflag = p_mesh->p_data->kflag;
  viditelny_zmena = FALSE;

  if(kflag&KONT_ZRCADLO)
    return(TRUE);
  
  if(viz || obb_visibility(&p_mesh->obb_world, p_mat)) {
    p_flag = p_mesh->p_kflag;
    if(p_mesh->objektu > 1) {
      int stat = (kflag&KONT_STATIC && !kamera_zmena);
      for(j = 0, p_obb = p_mesh->p_obb_world; j < p_mesh->objektu; j++, p_obb++, p_flag++) {
        if(obb_visibility_flag(p_obb, p_mat, stat, (*p_flag)&viditelny_flag)) {
          if(!((*p_flag)&viditelny_flag)) { // objekt nebyl driv viditelnej
            viditelny_zmena = TRUE;
          }
          *p_flag |= viditelny_flag;          
        } else {
          *p_flag &= ~viditelny_flag;          
        }
      }
    } else {
      if(!((*p_flag)&viditelny_flag)) { // objekt nebyl driv viditelnej
        viditelny_zmena = TRUE;
      }
      *p_flag |= viditelny_flag;
    }

    if(viditelny_zmena || !(kflag&viditelny_flag) || p_mesh->mail+1 != mail)
      kflag |= zmena_flag;
    else
      kflag &= ~zmena_flag;
    
    p_mesh->p_data->kflag = kflag|viditelny_flag;
    
    if(add) {
      assert(p_mesh->mail != mail);
      p_mesh->mail = mail;
      p_ber->p_mesh_renderlist[p_ber->mesh_rendernum++] = p_src;
    }

    return(TRUE);
  } else {
    p_mesh->p_data->kflag &= ~viditelny_flag;
    
    p_flag = p_mesh->p_kflag;    
    for(j = 0; j < p_mesh->objektu; j++, p_flag++) {
      *p_flag &= ~viditelny_flag;
    }
    
    return(FALSE);
  }
  */
}

inline void ber_render_list_vyrob_poly(G_KONFIG *p_ber, EDIT_MESH_POLY *p_poly, int add, GLMATRIX *p_mat, int viditelny_flag, int zmena_flag, int mail)
{
  int kflag;
  
  kflag = p_poly->kflag;
  
  if(kflag&KONT_ZRCADLO)
    return;
  
  if(!(kflag&viditelny_flag) || p_poly->mail+1 != mail)
    kflag |= zmena_flag;
  else
    kflag &= ~zmena_flag;

  p_poly->kflag = (kflag|viditelny_flag);
  
  if(add) {
    assert(p_poly->mail != mail);
    p_poly->mail = mail;

    p_ber->p_poly_renderlist[p_ber->poly_rendernum++] = p_poly;
    assert(p_ber->poly_rendernum <= p_ber->polynum);
  }
}

void ber_render_list_vyrob_rec(G_KONFIG *p_ber, OBB_TREE *p_prvni, GLMATRIX *p_m, GLMATRIX *p_zrc, int kamera_zmena, int mail)
{  
  EDIT_MESH_POLY   *p_poly;
  OBB_TREE_ITEM    *p_item;
  OBB_TREE         *p_next;
  int               i;
  /*
  p_item = p_prvni->p_item;
  for(i = 0; i < p_prvni->itnum; i++, p_item++) {
    if(kamera_zmena) {
      p_item->viditelny = obb_visibility(p_item->p_obb, p_m);
    } 
    if(p_item->viditelny) {
      p_poly = p_item->p_poly;
      if(p_poly) {
        ber_render_list_vyrob_poly(p_ber, p_poly, TRUE, p_m, KONT_VIDITELNY, KONT_VIDITELNY_ZMENA, mail);
      } else {
        ber_render_list_vyrob_mesh(p_ber, TRUE, p_ber->p_mesh+p_item->mesh, TRUE, p_m, KONT_VIDITELNY, KONT_VIDITELNY_ZMENA, mail, kamera_zmena);
      }    
    }
    
    if(p_zrc) {
      if(kamera_zmena) {
        p_item->zrc_viditelny = obb_visibility(p_item->p_obb, p_zrc);
      }      
      if(p_item->zrc_viditelny) {
        p_poly = p_item->p_poly;
        if(p_poly) {
          ber_render_list_vyrob_poly(p_ber, p_poly, !p_item->viditelny, p_zrc, KONT_VIDITELNY_ZRC, KONT_VIDITELNY_ZMENA, mail);
        } else {
          ber_render_list_vyrob_mesh(p_ber, TRUE, p_ber->p_mesh+p_item->mesh, !p_item->viditelny, p_zrc, KONT_VIDITELNY_ZRC, KONT_VIDITELNY_ZMENA, mail, kamera_zmena);
        }
      }
    }
  }
  
  p_next = p_prvni->p_next;
  for(i = 0; i < p_prvni->nextnum; i++, p_next++) {
    if(obb_visibility(&p_next->obb, p_m) || (p_zrc && obb_visibility(&p_next->obb, p_zrc)))
      ber_render_list_vyrob_rec(p_ber,p_next,p_m,p_zrc,kamera_zmena, mail);
  }
  */
}

void ber_render_list_vyrob(G_KONFIG *p_ber, int zrcadlo, int kamera_zmena)
{
  GLMATRIX         *p_zrc,camera_project_zrc;
  GLMATRIX         *p_camera_project = get_matrix_camera_project();
  GAME_MESH      ***p_src;
  int               i,viditelny, zmena;
  int               mail = ++p_ber->mail;

  /* Nastavi pocet pruhlednych objektu
  */  
  p_zrc = zrcadlo ? mat_mult_dir(__mat_top_matrix+__mat_top_matrix_akt,p_camera_project,&camera_project_zrc) : NULL;
/*
  if(!p_ber->p_poly_renderlist) {
    p_ber->p_poly_renderlist = mmalloc(sizeof(p_ber->p_poly_renderlist[0])*p_ber->polynum);
    p_ber->poly_rendernum = 0;
    p_ber->poly_renderakt = 0;
  }
*/
  p_ber->mesh_rendernum = 0;
  p_ber->poly_rendernum = 0;

  /* Prihozeni statickych veci z obbtree render-listu
  */
  /*
  if(obb_visibility(&p_ber->obbtree.obb, p_camera_project) || (p_zrc && obb_visibility(&p_ber->obbtree.obb, p_zrc))) {
    ber_render_list_vyrob_rec(p_ber, &p_ber->obbtree, p_camera_project, p_zrc, kamera_zmena, mail);
  }
    */
  /* Prihozeni dynamickych veci z dynamickeho render-listu
  */
  /*
  p_src = p_ber->p_dyn_meshlist;
  for(i = 0; i < p_ber->dyn_meshlistnum; i++, p_src++) {
    if(**p_src && !((**p_src)->nekreslit)) {
      zmena = kamera_zmena || (((**p_src)->p_data->kflag)&KONT_POHYB);
      viditelny = ber_render_list_vyrob_mesh(p_ber, FALSE, *p_src, TRUE, p_camera_project, KONT_VIDITELNY, KONT_VIDITELNY_ZMENA, mail, zmena);
      if(p_zrc)
        ber_render_list_vyrob_mesh(p_ber, FALSE, *p_src, !viditelny, p_zrc, KONT_VIDITELNY_ZRC, KONT_VIDITELNY_ZMENA, mail, zmena);
    }
  }
*/
  /* Setrideni podle materialu
  */
  ber_render_list_setrid(p_ber);
}

void ber_render_list_vyrob_pruhlist(G_KONFIG *p_ber)
{
 GLMATRIX       *p_mat = get_matrix_camera_project();
 EDIT_MESH_POLY *p_poly;
 GAME_MESH      *p_mesh;

 /* A zaciname!
 */
 p_ber->prhlnum = 0;

 /*
   Render Mesh-listu
 */   
/*
 ber_mesh_render_list_reset(p_ber);
 while(p_mesh = ber_mesh_render_list_next(p_ber)) {
   if(p_mesh->p_data->m1flag&MAT_PRUHLEDNY || p_mesh->p_data->kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) {
     PRUHLEDNY_OBJEKT *p_pruhl = p_ber->prhl+p_ber->prhlnum++;
     assert(p_ber->prhlnum < MAX_PRHL_OBJEKTU);
     p_pruhl->vzdal = ber_vzdal_bodu_z(&p_mesh->obb_world.obb_stred,p_mat);
     p_pruhl->p_objekt = (void *)p_mesh;
     p_pruhl->typ = PRUHLEDNY_MESH;
     p_mesh->p_data->kflag |= KONT_VIDITELNY_PRUHL;
   } else {
     p_mesh->p_data->kflag &= ~KONT_VIDITELNY_PRUHL;
   }
 }  
*/
 /*
   Render poly-listu
 */   
 /*
 ber_poly_render_list_reset(p_ber);
 while(p_poly = ber_poly_render_list_next(p_ber)) {
   if(p_poly->m1flag&MAT_PRUHLEDNY || p_poly->kflag&(KONT_DRAW_PRUHL|KONT_DRAW_CAMERA)) {
     PRUHLEDNY_OBJEKT *p_pruhl = p_ber->prhl+p_ber->prhlnum++;
     assert(p_ber->prhlnum < MAX_PRHL_OBJEKTU);
     p_pruhl->vzdal = ber_vzdal_bodu_z(&p_poly->obb.obb_stred,p_mat);
     p_pruhl->p_objekt = (void *)(p_poly);
     p_pruhl->typ = PRUHLEDNY_POLY;
     p_poly->kflag |= KONT_VIDITELNY_PRUHL;
   } else {
     p_poly->kflag &= ~KONT_VIDITELNY_PRUHL;
   }   
 }
 */
}
