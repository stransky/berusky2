/* OBB a jeho stromy
*/

#include "mss_on.h"
#include <windows.h>
#include "3d_all.h"

#define stejny_bod(a,b)    (a->x == b->x && a->y == b->y && a->z == b->z)

/*
  By Miguel Gomez,Gamasutra,October 18, 1999 
*/
int obb_intersect_obb(OBB *p_obb1, OBB *p_obb2)
{
  BOD *p_a = &p_obb1->obb_len;
  BOD *p_A =  p_obb1->obb;
  BOD *p_Pa = &p_obb1->obb_stred;

  BOD *p_b = &p_obb2->obb_len;
  BOD *p_B =  p_obb2->obb;
  BOD *p_Pb = &p_obb2->obb_stred;

/*  
  BOD a;       // delky ve smerech
  BOD Ax,Ay,Az;// jednotkovy smery
  BOD Pa;      // pozice
  
  BOD b;       // delky ve smerech
  BOD Bx,By,Bz;// jednotkovy smery
  BOD Pb;      // pozice
*/

  //ALGORITHM: Use the separating axis test for all 15 potential
  //separating axes. If a separating axis could not be found, the two
  //boxes overlap.
  
  BOD v,T,Rx,Ry,Rz;
  float ra,rb,t;
  
  vektor_sub(p_Pb,p_Pa,&v);
  
  T.x  = vektor_dot_product(&v,p_A);
  Rx.x = vektor_dot_product(p_A,p_B);
  Rx.y = vektor_dot_product(p_A,p_B+1);
  Rx.z = vektor_dot_product(p_A,p_B+2);
  
  ra = p_a->x;
  rb = p_b->x*fabsf(Rx.x) + p_b->y*fabsf(Rx.y) + p_b->z*fabsf(Rx.z);
  t =  fabsf(T.x);
  if( t > ra + rb )
    return(FALSE);
  
  T.y  = vektor_dot_product(&v,p_A+1);
  Ry.x = vektor_dot_product(p_A+1,p_B);
  Ry.y = vektor_dot_product(p_A+1,p_B+1);
  Ry.z = vektor_dot_product(p_A+1,p_B+2);
  
  ra = p_a->y;
  rb = p_b->x*fabsf(Ry.x) + p_b->y*fabsf(Ry.y) + p_b->z*fabsf(Ry.z);
  t =  fabsf(T.y);
  if( t > ra + rb )
    return(FALSE);
  
  T.z  = vektor_dot_product(&v,p_A+2);
  Rz.x = vektor_dot_product(p_A+2,p_B);
  Rz.y = vektor_dot_product(p_A+2,p_B+1);
  Rz.z = vektor_dot_product(p_A+2,p_B+2);
  
  ra = p_a->z;
  rb = p_b->x*fabsf(Rz.x) + p_b->y*fabsf(Rz.y) + p_b->z*fabsf(Rz.z);
  t =  fabsf(T.z);
  if( t > ra + rb )
    return(FALSE);  
  
  //B->A v(x)
  ra = p_a->x*fabsf(Rx.x) + p_a->y*fabsf(Ry.x) + p_a->z*fabsf(Rz.x);
  rb = p_b->x;
  t =  fabsf(T.x*Rx.x + T.y*Ry.x + T.z*Rz.x);
  if( t > ra + rb )
    return(FALSE);
  
  //B->A v(y)
  ra = p_a->x*fabsf(Rx.y) + p_a->y*fabsf(Ry.y) + p_a->z*fabsf(Rz.y);
  rb = p_b->y;
  t =  fabsf(T.y*Rx.y + T.y*Ry.y + T.z*Rz.y);
  if( t > ra + rb )
    return(FALSE);
  
  //B->A v(z)
  ra = p_a->x*fabsf(Rx.z) + p_a->y*fabsf(Ry.z) + p_a->z*fabsf(Rz.z);
  rb = p_b->z;
  t =  fabsf(T.z*Rx.z + T.y*Ry.z + T.z*Rz.z);
  if( t > ra + rb )
    return(FALSE);
  
  //9 cross products   
  //L = A0 x B0
  ra = p_a->y*fabsf(Rz.x) + p_a->z*fabsf(Ry.z);
  rb = p_b->y*fabsf(Rx.z) + p_b->z*fabsf(Rx.y);
  t =  fabsf(T.z*Ry.x - T.y*Rz.x);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A0 x B1
  ra = p_a->y*fabsf(Rz.y) + p_a->z*fabsf(Ry.y);
  rb = p_b->x*fabsf(Rx.z) + p_b->z*fabsf(Rx.x);
  t = fabsf(T.z*Ry.y - T.y*Rz.y);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A0 x B2
  ra = p_a->y*fabsf(Rz.z) + p_a->z*fabsf(Ry.z);   
  rb = p_b->x*fabsf(Rx.y) + p_b->y*fabsf(Rx.x);
  t = fabsf(T.z*Ry.z - T.y*Rz.z);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A1 x B0
  ra = p_a->x*fabsf(Rz.x) + p_a->z*fabsf(Rx.x);
  rb = p_b->y*fabsf(Ry.z) + p_b->z*fabsf(Ry.y);
  t = fabsf(T.x*Rz.x - T.z*Rx.x);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A1 x B1
  ra = p_a->x*fabsf(Rz.y) + p_a->z*fabsf(Rx.y);
  rb = p_b->x*fabsf(Ry.z) + p_b->z*fabsf(Ry.x);
  t = fabsf(T.x*Rz.y - T.z*Rx.y);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A1 x B2
  ra = p_a->x*fabsf(Rz.z) + p_a->z*fabsf(Rx.z);
  rb = p_b->x*fabsf(Ry.y) + p_b->y*fabsf(Ry.x);
  t = fabsf(T.x*Rz.z - T.z*Rx.z);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A2 x B0
  ra = p_a->x*fabsf(Ry.x) + p_a->y*fabsf(Rx.x);
  rb = p_b->y*fabsf(Rz.z) + p_b->z*fabsf(Rz.y);
  t = fabsf(T.y*Rx.x - T.x*Ry.x);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A2 x B1
  ra = p_a->x*fabsf(Ry.y) + p_a->y*fabsf(Rx.y);
  rb = p_b->x*fabsf(Rz.z) + p_b->z*fabsf(Rz.x);
  t = fabsf(T.y*Rx.y - T.x*Ry.y);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A2 x B2
  ra = p_a->x*fabsf(Ry.z) + p_a->y*fabsf(Rx.z);
  rb = p_b->x*fabsf(Rz.y) + p_b->y*fabsf(Rz.x);
  t =  fabsf(T.y*Rx.z - T.x*Ry.z);
  if( t > ra + rb )
    return(FALSE);
  
  return(TRUE);
}        

// Prusecik obalky OBB a primky
int obb_intersect_line(OBB *p_obb, BOD *p_orig, BOD *p_dir)
{
  BOD *p_a = &p_obb->obb_len;
  BOD *p_A = p_obb->obb;
  BOD *p_Pa = &p_obb->obb_stred;
  
  BOD t,o,dir,min;

  vektor_sub(p_orig,p_Pa,&t);
  o.x = vektor_dot_product(p_A,  &t);
  o.y = vektor_dot_product(p_A+1,&t);
  o.z = vektor_dot_product(p_A+2,&t);

  dir.x = vektor_dot_product(p_A,  p_dir);
  dir.y = vektor_dot_product(p_A+1,p_dir);
  dir.z = vektor_dot_product(p_A+2,p_dir);
  
  vektor_add(&o,&dir,&t);
  return(kd_intersect_kostku(&o,&t,vektor_mult_skalar(p_a,-1.0f,&min),p_a));
}

int obb_intersect_line_dist(OBB *p_obb, BOD *p_orig, BOD *p_dir)
{
  BOD  *p_a = &p_obb->obb_len;
  BOD  *p_A = p_obb->obb;
  BOD  *p_Pa = &p_obb->obb_stred;
  BOD   p;
  float tn;  
  BOD   t,o,dir,min;
  int   ret;

  vektor_sub(p_orig,p_Pa,&t);
  o.x = vektor_dot_product(p_A,  &t);
  o.y = vektor_dot_product(p_A+1,&t);
  o.z = vektor_dot_product(p_A+2,&t);

  dir.x = vektor_dot_product(p_A,  p_dir);
  dir.y = vektor_dot_product(p_A+1,p_dir);
  dir.z = vektor_dot_product(p_A+2,p_dir);
  
  vektor_add(&o,&dir,&t);
  ret = kd_intersect_kostku_bod(&o,&t,vektor_mult_skalar(p_a,-1.0f,&min),p_a,&p,&tn);
  return((tn < 1.0f) ? ret : FALSE);
}

// prevod kostky na hranicni body
void obb_body(OBB *p_obbs)
{
  BOD *p_len = &p_obbs->obb_len;
  BOD *p_obb = p_obbs->obb;
  BOD *p_stred = &p_obbs->obb_stred;
  BOD *p_hran = p_obbs->obb_hran;
  BOD  a1,a2,a3;
  BOD  a12,a22,a32;

  vektor_mult_skalar(p_obb+0,p_len->x,&a1);
  vektor_mult_skalar(p_obb+1,p_len->y,&a2);
  vektor_mult_skalar(p_obb+2,p_len->z,&a3);
  
  vektor_mult_skalar(&a1,2.0f,&a12);
  vektor_mult_skalar(&a2,2.0f,&a22);
  vektor_mult_skalar(&a3,2.0f,&a32);

  vektor_sub(p_stred, &a1,p_hran);
  vektor_sub(p_hran,  &a2,p_hran);
  vektor_sub(p_hran,  &a3,p_hran);
  
  vektor_add(p_hran,  &a12,p_hran+1);

  vektor_add(p_hran  ,&a32,p_hran+2);
  vektor_add(p_hran+1,&a32,p_hran+3);
    
  vektor_add(p_hran  ,&a22,p_hran+4);
  vektor_add(p_hran+1,&a22,p_hran+5);
  vektor_add(p_hran+2,&a22,p_hran+6);
  vektor_add(p_hran+3,&a22,p_hran+7);
}

void obb_obalka(GLMATRIX *p_mat, OBB *p_obb, BOD *p_min, BOD *p_max)
{
  BOD a;
  int i;

  if(p_mat) {
    transformuj_bod_matici_bod(p_obb->obb_hran,p_mat,&a);
  } else {
    a = p_obb->obb_hran[0];
  }

  *p_min = *p_max = a;
  for(i = 1; i < 8; i++) {
    if(p_mat) {
      transformuj_bod_matici_bod(p_obb->obb_hran+i,p_mat,&a);
    } else {
      a = p_obb->obb_hran[i];
    }
    kd_min_max_bod(&a, p_min, p_max);
  }
}

// slepi OBB do vysledneho OBB
void obb_slep(OBB *p_vys, OBB *p_list, int num)
{
  GLMATRIX m;
  BOD      bmin,bmax;
  BOD      nmin,nmax;
  BOD      min,max;
  OBB     *p_obb;
  int      i,j;
  float    obsah, bobsah = FLT_MAX;
  int      bi = -1;

  if(num == 1) {
   *p_vys = p_list[0];
    return;
  }

  /* 1. Test AABB obalky
  */  
  init_matrix(&m);
  
  vektor_set_all(&bmin, FLT_MAX, FLT_MAX, FLT_MAX);
  vektor_set_all(&bmax,-FLT_MAX,-FLT_MAX,-FLT_MAX);
    
  for(j = 0; j < num; j++) {    
    kd_min_max_bod(&p_list[j].aabb_min,&bmin,&bmax);
    kd_min_max_bod(&p_list[j].aabb_max,&bmin,&bmax);
  }
  
  obsah = kd_bunka_obsah(&bmin,&bmax);
  if(obsah < bobsah) {
    bi = -1;
    bobsah = obsah;
    nmin = bmin;
    nmax = bmax;
  }

  /* 2. Test OBB obalky
  */
  for(i = 0; i < num; i++) {
    p_obb = p_list+i;
  
    // Transformuje World Space -> OBB
    mat_rot(&m,p_obb->obb,p_obb->obb+1,p_obb->obb+2);
    
    vektor_set_all(&bmin, FLT_MAX, FLT_MAX, FLT_MAX);
    vektor_set_all(&bmax,-FLT_MAX,-FLT_MAX,-FLT_MAX);

    // Transformuje ostatni obalky do teto obalky
    for(j = 0; j < num; j++) {
      obb_obalka(&m, p_list+j, &min, &max);
      kd_min_max_bod(&min,&bmin,&bmax);
      kd_min_max_bod(&max,&bmin,&bmax);
    }

    obsah = kd_bunka_obsah(&bmin,&bmax);
    if(obsah < bobsah) {
      bi = i;
      bobsah = obsah;
      nmin = bmin;
      nmax = bmax;
    }
  }
  
  assert(bi < num);
  if(bi == -1) {
    p_vys->aabb_min = nmin;
    p_vys->aabb_max = nmax;

    kd_stred_bunky(&nmin,&nmax,&p_vys->obb_stred);
    kd_len_bunky(&nmin,&nmax,&p_vys->obb_len);
    
    vektor_set_all(p_vys->obb  ,1,0,0);
    vektor_set_all(p_vys->obb+1,0,1,0);
    vektor_set_all(p_vys->obb+2,0,0,1);
    
    obb_body(p_vys);    
  } else {
    p_obb = p_list+bi;
    mat_rot(&m,p_obb->obb,p_obb->obb+1,p_obb->obb+2);
    invert_matrix_copy(&m,&m);
    kd_stred_bunky(&nmin,&nmax,&min);
    transformuj_bod_matici_bod(&min,&m,&p_vys->obb_stred);
    kd_len_bunky(&nmin,&nmax,&p_vys->obb_len);
    
    p_vys->obb[0] = p_list[bi].obb[0];
    p_vys->obb[1] = p_list[bi].obb[1];
    p_vys->obb[2] = p_list[bi].obb[2];
    
    obb_body(p_vys);
    obb_aabb(p_vys);
  }
}

void obb_slep_aabb(OBB *p_vys, OBB *p_list, int num)
{  
  BOD     *p_min,
          *p_max;  
  OBB     *p_obb;
  int      i;

  if(num == 1) {
   *p_vys = p_list[0];
    return;
  }

  p_min = &p_vys->aabb_min;
  p_max = &p_vys->aabb_max;
  p_obb = p_list;
 *p_min = p_obb->aabb_min;
 *p_max = p_obb->aabb_max;
  
  for(i = 1; i < num; i++) {
    p_obb = p_list+i;
    kd_min_max_bod(&p_obb->aabb_min,p_min,p_max);
    kd_min_max_bod(&p_obb->aabb_max,p_min,p_max);
  }
 
  kd_korekce_bunky(p_min,p_max);
  kd_stred_bunky(p_min,p_max,&p_vys->obb_stred);
  kd_len_bunky(p_min,p_max,&p_vys->obb_len);

  vektor_set_all(p_vys->obb,   1, 0, 0);
  vektor_set_all(p_vys->obb+1, 0, 1, 0);
  vektor_set_all(p_vys->obb+2, 0, 0, 1);

  obb_body(p_vys);
}

float obb_calc_obalka_obj(EDIT_OBJEKT *p_obj, BOD *p_vx, BOD *p_vy, BOD *p_vz, BOD *p_dist, BOD *p_stred)
{
  BOD      min,max,tmp;
  GLMATRIX m;
  
  if(!p_obj->p_vertex)
    return(0);
    
  mat_rot(&m,p_vx,p_vy,p_vz);
  objekt_obalka(p_obj,&m,&min,&max);
  
  vektor_sub(&max,&min,&tmp);
  vektor_mult_skalar(&tmp,0.5f,p_dist);
  vektor_add(&min,p_dist,p_stred);
  
  invert_matrix_copy(&m,&m);
  transformuj_bod_bod_matici(p_stred,&m);
  
  return(tmp.x*tmp.x+tmp.y*tmp.y+tmp.z*tmp.z);
}

void obb_prvek(OBB *p_obb)
{
  memset(p_obb,0,sizeof(p_obb[0]));
  vektor_set_all(p_obb->obb,  1,0,0);
  vektor_set_all(p_obb->obb+1,0,1,0);
  vektor_set_all(p_obb->obb+2,0,0,1);
  p_obb->obb_len.x = p_obb->obb_len.y = p_obb->obb_len.z = 1.0f;
  p_obb->obb_stred.y = 1.0f;
  obb_aabb(p_obb);
  obb_body(p_obb);
}

void obb_calc_obj(EDIT_OBJEKT *p_obj)
{
  BOD   bnorm,bv1,bv2,bdist,bstred;
  BOD   norm,v1,v2,dist,stred;
  OBB  *p_obb = &p_obj->obb;
  int   f0,f1,f2,f;
  float obsah, bobsah;

  vektor_set_all(&bv1,1,0,0);
  vektor_set_all(&bnorm,0,1,0);
  vektor_set_all(&bv2,0,0,1);
  bobsah = obb_calc_obalka_obj(p_obj, &bv1, &bnorm, &bv2, &bdist, &bstred);

  for(f = 0; f < p_obj->facenum; f += 3) {
  
    f0 = p_obj->p_face[f];
    f1 = p_obj->p_face[f+1];
    f2 = p_obj->p_face[f+2];

    calc_face_normal((BOD *)(p_obj->p_vertex+f0),
                     (BOD *)(p_obj->p_vertex+f1),
                     (BOD *)(p_obj->p_vertex+f2), &norm);   

    if(vektor_velikost(&norm) == 0.0f)
      continue;

    // f0->f1
    vektor_sub((BOD *)(p_obj->p_vertex+f1),(BOD *)(p_obj->p_vertex+f0),&v1);
    vektor_soucin(&v1,&norm,&v2);
    
    vektor_norm(&v1);
    vektor_norm(&v2);
    vektor_norm(&norm);
    
    obsah = obb_calc_obalka_obj(p_obj, &v1, &norm, &v2, &dist, &stred);
    if(obsah < bobsah) {
      bobsah = obsah;
      bv1 = v1;
      bv2 = v2;
      bnorm = norm;
      bdist = dist;
      bstred = stred;
    }
    
    // f0->f2
    vektor_sub((BOD *)(p_obj->p_vertex+f2),(BOD *)(p_obj->p_vertex+f0),&v1);
    vektor_soucin(&v1,&norm,&v2);
    
    vektor_norm(&v1);
    vektor_norm(&v2);
    vektor_norm(&norm);
    
    obsah = obb_calc_obalka_obj(p_obj, &v1, &norm, &v2, &dist, &stred);
    if(obsah < bobsah) {
      bobsah = obsah;
      bv1 = v1;
      bv2 = v2;
      bnorm = norm;
      bdist = dist;
      bstred = stred;
    }

    // f1->f2
    vektor_sub((BOD *)(p_obj->p_vertex+f2),(BOD *)(p_obj->p_vertex+f1),&v1);
    vektor_soucin(&v1,&norm,&v2);
    
    vektor_norm(&v1);
    vektor_norm(&v2);
    vektor_norm(&norm);
    
    obsah = obb_calc_obalka_obj(p_obj, &v1, &norm, &v2, &dist, &stred);
    if(obsah < bobsah) {
      bobsah = obsah;
      bv1 = v1;
      bv2 = v2;
      bnorm = norm;
      bdist = dist;
      bstred = stred;
    }
  }

  // Matice transfromujici word-space -> obalka-space
  p_obb->obb[0] = bv1;
  p_obb->obb[1] = bnorm;
  p_obb->obb[2] = bv2;
  p_obb->obb_len = bdist;
  p_obb->obb_stred = bstred;
  obb_body(p_obb);
  obb_aabb_obj(p_obj,p_obb);
}

void obb_calc_obj_fast(EDIT_OBJEKT *p_obj)
{  
  BOD   vx,vy,vz;  
  
  vx.x = 1; vx.y = 0; vx.z = 0;
  vy.x = 0; vy.y = 1; vy.z = 0;
  vz.x = 0; vz.y = 0; vz.z = 1;
  
  p_obj->obb.obb[0] = vx;
  p_obj->obb.obb[1] = vy;
  p_obj->obb.obb[2] = vz;

  obb_calc_obalka_obj(p_obj, &vx, &vy, &vz, &p_obj->obb.obb_len, &p_obj->obb.obb_stred);  
  obb_body(&p_obj->obb);
  obb_aabb_obj(p_obj,&p_obj->obb);
}

/* Vola se pouze pri exportu sceny - pocitam prave OBB
*/
void obb_calc_kont_keyframe(EDIT_KONTEJNER *p_kont, int num)
{
  HIERARCHY_SIM *p_sim;
  GLMATRIX m;
  OBB *p_list;
  OBB  vysl;
  int  prvni;
  int *p_ind,o;
  int  i,j,k;

  p_list = mmalloc(sizeof(p_list[0])*(num+1));
  p_ind = mmalloc(sizeof(p_ind[0])*num);
  num = 0;
  oe_olist_reset(&o);
  while(oe_olist_next(p_kont,&o)) {
    p_ind[num++] = o;
  }
  
  prvni = TRUE;
  init_matrix(&m);

  /* Pomoci animaci tansformuju obalky
  */
  for(k = 0; k < KONT_MAX_ANIM; k++) {
    p_sim = p_kont->sim+k;
    if(p_sim->keynum) {
      kprintf(TRUE,"KEY-Animace %d...",k);
      for(i = 0; i < p_sim->keynum; i++) {
        key_sim_root_to_matrix(p_kont, p_sim, frame2time(i), &m);
        for(j = 0; j < num; j++) {
          obb_transformuj(&p_kont->p_obj[p_ind[j]]->obb, &p_kont->p_obj[p_ind[j]]->m, p_list+j);
        }
        if(prvni) {
          obb_slep(&vysl,p_list,num);
          prvni = FALSE;
        } else {
          p_list[j] = vysl;
          obb_slep(&vysl,p_list,num+1);
        }
      }
    }
  }

  p_kont->obb = vysl;

  free(p_ind);
  free(p_list);

  // Reanimuju to na puvodni velikost
  for(k = 0; k < KONT_MAX_ANIM; k++) {
    p_sim = p_kont->sim+k;
    if(p_sim->keynum) {
      key_sim_root_to_matrix(p_kont, p_sim, 0, kont_world_matrix(p_kont));
      break;
    }
  }
}

// Prepocita objekt na tuto animaci
void obb_calc_obj_kosti_anim(EDIT_OBJEKT *p_obj, JOINT_ANIMACE *p_anim, OBB *p_obb, int precizne)
{
  OBB *p_list;
  int  k;

  p_list = mmalloc(sizeof(p_list[0])*p_anim->framenum);

  for(k = 0; k < p_anim->framenum; k++) {
    p_anim->time = frame2time(k);
    key_kosti_animuj(p_anim);
    key_kosti_updatuj_vertexy(p_obj, p_anim);
    if(precizne)
      obb_calc_obj(p_obj);
    else
      obb_calc_obj_fast(p_obj);
    p_list[k] = p_obj->obb;
  }

  obb_slep(p_obb,p_list,p_anim->framenum);
  free(p_list);
}

void obb_calc_obj_kosti(EDIT_OBJEKT *p_obj, int precizne)
{
  JOINT_ANIMACE *p_anim;
  OBB *p_list;
  int  num;

  if(!p_obj->p_joit_animace) {
    obb_calc_obj(p_obj);
  } else {
    num = 0;
    p_anim = p_obj->p_joit_animace;
    while(p_anim) {
      p_anim = p_anim->p_next;
      num++;
    }

    p_list = mmalloc(sizeof(p_list[0])*num);

    num = 0;
    p_anim = p_obj->p_joit_animace;
    while(p_anim) {
      obb_calc_obj_kosti_anim(p_obj, p_anim, p_list+num++, precizne);
      p_anim = p_anim->p_next;
    }
    
    obb_slep(&p_obj->obb,p_list,num);
  }
}

/* Vola se pouze pri exportu sceny - pocitam prave OBB
*/
void obb_calc_kont(EDIT_KONTEJNER *p_kont_top, int precizne)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  OBB *p_list;
  int  o,num = 0; 

  p_kont = p_kont_top;
  while(p_kont) {

    oe_olist_reset(&o);
    while((p_obj = oe_olist_next(p_kont,&o))) {
      kprintf(TRUE,"calc-OBB-Objekt %s...",p_obj->jmeno);
      obb_calc_obj_kosti(p_obj,precizne);
      num++;
    }
    
    if(p_kont->kflag&KONT_KEYFRAME) {
      obb_calc_kont_keyframe(p_kont,num);
    } else {
      p_list = mmalloc(sizeof(p_list[0])*num);
      
      num = 0;
      oe_olist_reset(&o);
      while((p_obj = oe_olist_next(p_kont,&o))) {
        p_list[num] = p_obj->obb;
        num++;
      }
      
      obb_slep(&p_kont->obb,p_list,num);
      free(p_list);
    }
    p_kont = p_kont->p_next;
  }
}

float obb_calc_poly_obalka(EDIT_MESH_POLY *p_poly, BOD *p_vx, BOD *p_vy, BOD *p_vz, BOD *p_dist, BOD *p_stred)
{
  BOD      min,max,tmp;
  GLMATRIX m,mi;
      
  mat_rot(&m,p_vx,p_vy,p_vz);
  invert_matrix(&m,&mi);

  poly_obalka(p_poly,&m,&min,&max);
  
  vektor_sub(&max,&min,&tmp); // lokalni stred !!!
  vektor_mult_skalar(&tmp,0.5f,p_dist);
  transformuj_bod_matici_bod(vektor_add(&min,p_dist,p_stred),&mi,p_stred);
  // Globalni stred
  
  return(tmp.x*tmp.x+tmp.y*tmp.y+tmp.z*tmp.z);
}

void obb_calc_poly(EDIT_MESH_POLY *p_poly)
{
  BOD   bnorm,bv1,bv2,bdist,bstred;
  BOD   norm,v1,v2,dist,stred;
  OBB  *p_obb = &p_poly->obb;
  int   f;
  BOD   a,b,c;
  float obsah, bobsah = FLT_MAX;


  for(f = 0; f < p_poly->facenum; f += 3) {
    a.x = p_poly->p_koord[f].x;
    a.y = p_poly->p_koord[f].y;
    a.z = p_poly->p_koord[f].z;

    b.x = p_poly->p_koord[f+1].x;
    b.y = p_poly->p_koord[f+1].y;
    b.z = p_poly->p_koord[f+1].z;
    
    c.x = p_poly->p_koord[f+2].x;
    c.y = p_poly->p_koord[f+2].y;
    c.z = p_poly->p_koord[f+2].z;
  
    calc_face_normal(&a,&b,&c,&norm);

    // f0->f1
    vektor_sub(&a,&b,&v1);
    vektor_soucin(&v1,&norm,&v2);
    
    vektor_norm(&v1);
    vektor_norm(&v2);
    vektor_norm(&norm);
    
    obsah = obb_calc_poly_obalka(p_poly, &v1, &norm, &v2, &dist, &stred);
    if(obsah < bobsah) {
      bobsah = obsah;
      bv1 = v1;
      bv2 = v2;
      bnorm = norm;
      bdist = dist;
      bstred = stred;
    }
    
    // f0->f2
    vektor_sub(&a,&c,&v1);
    vektor_soucin(&v1,&norm,&v2);
    
    vektor_norm(&v1);
    vektor_norm(&v2);
    vektor_norm(&norm);
    
    obsah = obb_calc_poly_obalka(p_poly, &v1, &norm, &v2, &dist, &stred);
    if(obsah < bobsah) {
      bobsah = obsah;
      bv1 = v1;
      bv2 = v2;
      bnorm = norm;
      bdist = dist;
      bstred = stred;
    }

    // f1->f2
    vektor_sub(&b,&c,&v1);
    vektor_soucin(&v1,&norm,&v2);
    
    vektor_norm(&v1);
    vektor_norm(&v2);
    vektor_norm(&norm);
    
    obsah = obb_calc_poly_obalka(p_poly, &v1, &norm, &v2, &dist, &stred);
    if(obsah < bobsah) {
      bobsah = obsah;
      bv1 = v1;
      bv2 = v2;
      bnorm = norm;
      bdist = dist;
      bstred = stred;
    }
  }

  // Matice transfromujici word-space -> obalka-space
  p_obb->obb[0] = bv1;
  p_obb->obb[1] = bnorm;
  p_obb->obb[2] = bv2;
  p_obb->obb_len = bdist;
  p_obb->obb_stred = bstred;
  obb_body(p_obb);
  obb_aabb_poly(p_poly,p_obb);
}

void obb_calc_item(OBB *p_vys, OBB_TREE_ITEM *p_item, int itemnum)
{ 
  OBB *p_list = mmalloc(sizeof(OBB)*itemnum);
  int  i;

  for(i = 0; i < itemnum; i++) {
    p_list[i] = *(p_item[i].p_obb);
  }
  obb_slep_aabb(p_vys,p_list,itemnum);
  free(p_list);
}

void obb_kresli_obalku(OBB *p_obb, dword barva, GLMATRIX *p_mat)
{
  BOD     *p_dir =  p_obb->obb;
  BOD      min,max;
  GLMATRIX m;
  
  // Transformuje World Space->OBB
  mat_rot(&m,p_dir,p_dir+1,p_dir+2);  
  if(matrix_det(&m)) {
    // OBB->World Space
    invert_matrix_copy(&m,&m);
    translate_matrix_set(&m,&p_obb->obb_stred);
    if(p_mat) mat_mult(p_mat,&m,&m);
    kd_bunka_expanze2(&p_obb->obb_len,&min,&max);
    kresli_obalku(min,max,barva,&m);
  }
}

void obbtree_kresli(OBB_TREE *p_prvni, dword barva)
{
  int i;
  if(p_prvni) {
    obb_kresli_obalku(&p_prvni->obb, barva, NULL);    
    for(i = 0; i < p_prvni->nextnum; i++)
      obbtree_kresli(p_prvni->p_next+i,barva);
  }
}

void obbtree_calc_obalky(OBB_TREE *p_prvni)
{
  OBB_TREE_ITEM *p_itn;
  OBB *p_list;
  int  p,i;
  
  p_list = mmalloc(sizeof(p_list[0])*(p_prvni->nextnum+p_prvni->itnum));
  for(i = 0; i < p_prvni->nextnum; i++) {
    obbtree_calc_obalky(p_prvni->p_next+i);
    p_list[i] = p_prvni->p_next[i].obb;
  }
 
  p_itn = p_prvni->p_item;
  for(p = 0; p < p_prvni->itnum; p++, p_itn++) {
    if(p_itn->p_mesh) {
      p_list[i+p] = p_itn->p_mesh->obb_world;
    } else {
      p_list[i+p] = p_itn->p_poly->obb;
    }
  }

  obb_slep(&p_prvni->obb,p_list,i+p);
  free(p_list);
}

#define MAX_VZDAL_OBALEK          20.0f
#define MIN_PODOBALEK_OBJ         10
#define MIN_PODOBALEK_OBJ_ZBYTEK  10

/* Kazdy polygon ma uz implicitne obalku
*/
static EDIT_MATERIAL **p_mat;

void obbtree_vyrob(OBB_TREE *p_prvni, EDIT_MESH_POLY *p_poly, int polynum, GAME_MESH **p_mesh, int meshnum, int *p_ind, EDIT_MATERIAL **p_matlist)
{
  int i,j,itnum = polynum+meshnum;

  p_mat = p_matlist;

  memset(p_prvni,0,sizeof(p_prvni[0]));
  p_prvni->p_item = mmalloc(sizeof(p_prvni->p_item[0])*itnum);
  p_prvni->itnum = itnum;

  for(i = 0; i < polynum; i++) {
    p_prvni->p_item[i].p_poly = p_poly+i;
    p_prvni->p_item[i].p_obb = &p_poly[i].obb;
  }
  for(j = 0; j < meshnum; j++) {
    if(p_ind)
      p_prvni->p_item[i+j].mesh = p_ind[j];
    p_prvni->p_item[i+j].p_mesh = p_mesh[j];
    p_prvni->p_item[i+j].p_obb = &p_mesh[j]->obb_world;
  }
  
  obbtree_vyrob_rec(p_prvni, MAX_VZDAL_OBALEK);
  obbtree_calc_obalky(p_prvni);
}

static int obbtree_vyrob_rec_comp(const void *p_1, const void *p_2)
{
  OBB_TREE_ITEM *p_it1 = (OBB_TREE_ITEM *)p_1;
  OBB_TREE_ITEM *p_it2 = (OBB_TREE_ITEM *)p_2;
  EDIT_MESH_POLY *p_poly1,*p_poly2;
  
  if(p_it1->p_mesh && p_it2->p_poly)
    return(-1);
  else if(p_it2->p_mesh && p_it1->p_poly)
    return(1);  
  if(p_it1->p_mesh && p_it2->p_mesh)
    return(0);
  
  p_poly1 = p_it1->p_poly;
  p_poly2 = p_it2->p_poly;

  if(p_mat[p_poly1->material]->p_text[0] && p_mat[p_poly2->material]->p_text[0])
    return(p_mat[p_poly1->material]->p_text[0]->text - p_mat[p_poly2->material]->p_text[0]->text);
  else
    return(p_mat[p_poly1->material]->p_text[0] - p_mat[p_poly2->material]->p_text[0]);
}


/*
typedef struct _OBB_TREE_ITEM {

  OBB              *p_obb;
  EDIT_MESH_POLY   *p_poly;
  GAME_MESH        *p_mesh;
  int               mesh;

} OBB_TREE_ITEM;

typedef struct _OBB_TREE {

  int                mail;  
  OBB                obb;

  int                itnum;     // polozky bunky - vsechny
  OBB_TREE_ITEM     *p_item;

  struct _OBB_TREE  *p_up;      // otec stromu
  int                nextnum;   // pocet podobalek
  struct _OBB_TREE  *p_next;    // podobalky
  
} OBB_TREE;
*/

/* Ze seznamu polylistu vybere ty co se daji do obalky
   a zbytek se soupne do te hlavni
*/
/* p_prvni -> pointer na prvni obalku, do ktere se strci vsechny
   globalni poly a ty podobalky se strci do pod_obalek
*/
void obbtree_vyrob_rec(OBB_TREE *p_prvni, float max_vzdal)
{
  OBB_TREE_ITEM *p_item = p_prvni->p_item;
  int            itemnum = p_prvni->itnum;
  int            itemzbyva = p_prvni->itnum;
  OBB_TREE_ITEM *p_tmp_item = _alloca(sizeof(p_tmp_item[0])*itemnum);
  int            tmp_itemnum;
  OBB_TREE      *p_obalky = _alloca(sizeof(p_obalky[0])*itemnum);
  int            obalnum = 0;
  int           *p_hits = _alloca(sizeof(p_hits[0])*itemnum);   // globalni hity
  int           *p_lhits = _alloca(sizeof(p_lhits[0])*itemnum); // lokalni hity
  OBB            obb;
  int            p,i,itemzustava;

  if(p_prvni->itnum <= MIN_PODOBALEK_OBJ)
    return;

  memset(p_tmp_item,0,sizeof(p_tmp_item[0])*itemnum);
  memset(p_obalky,0,sizeof(p_obalky[0])*itemnum);
  memset(p_hits,0,sizeof(p_hits[0])*itemnum);
  memset(p_lhits,0,sizeof(p_lhits[0])*itemnum);

  /* Cyklus pres vsechny poly, ktere nejsou jesne vyrazeny ze seznamu volnych
     - hit-list
  */
  for(p = 0; p < itemnum; p++) {
    if(p_hits[p]) 
      continue;
       
    /* seznam tmp poly -> zacatek seznamu potencialni obalky
       momentalne to je pouze ten 1 prvek
    */
    p_tmp_item[0] = p_item[p];
    tmp_itemnum = 1;
    p_lhits[p] = TRUE;
    
    /* rozmery minimalni obalky o 1 poly
    */
    obb = *p_item[p].p_obb;

    /* Pridani vsech blizskych poly do 1 obalky
    */
    for(i = 0; i < itemnum; i++) {
      if(i == p || p_hits[i])
        continue;
      p_lhits[i] = (vzdal_bodu_bod(&obb.obb_stred,&p_item[i].p_obb->obb_stred) <= max_vzdal);
      if(p_lhits[i]) {
        p_tmp_item[tmp_itemnum++] = p_item[i];
        obb_calc_item(&obb,p_tmp_item,tmp_itemnum);
      }
    }
    
    /* Pokud je blizko sebe vice objektu nez co je minimum jedne obalky -> 
       - udelej tuto samostanou pod-obalku se shluku objektu -> vyber je
       - a to pouze pokud jeste neco zbyde v puvodni obalce
    */
    if(tmp_itemnum >= MIN_PODOBALEK_OBJ && itemzbyva-tmp_itemnum >= MIN_PODOBALEK_OBJ_ZBYTEK) {
      itemzbyva -= tmp_itemnum;
      p_obalky[obalnum].obb = obb;
      p_obalky[obalnum].p_item = kopiruj_pole(p_tmp_item,sizeof(p_tmp_item[0])*tmp_itemnum);
      p_obalky[obalnum].itnum = tmp_itemnum;
      p_obalky[obalnum].p_up = p_prvni;
      obalnum++;

      for(i = 0; i < itemnum; i++) // Prekopiruje hit-list
        p_hits[i] |= p_lhits[i];
    }
  }

  /* Spocitam pocet neumistenych polozek
  */
  memcpy(p_tmp_item,p_item,sizeof(p_item[0])*itemnum);
  itemzustava = 0;
  for(i = 0; i < itemnum; i++) {
    if(!p_hits[i])
      itemzustava++;
  }
  
  free(p_prvni->p_item);

  p_prvni->p_item = mmalloc(sizeof(p_prvni->p_item[0])*itemzustava);
  p_prvni->itnum = itemzustava;
  
  /* Skopiruju neumistene polozky
  */ 
  for(i = 0, itemzustava = 0; i < itemnum; i++) {
    if(!p_hits[i])
      p_prvni->p_item[itemzustava++] = p_tmp_item[i];
  }   

  /* Setridim polozky podle pouzitych textur
  */  
  qsort(p_prvni->p_item,p_prvni->itnum,sizeof(p_prvni->p_item[0]),obbtree_vyrob_rec_comp);

  
  /* Skopiruji se obalky
  */
  p_prvni->nextnum = obalnum;
  if(obalnum) {
    p_prvni->p_next = kopiruj_pole(p_obalky,sizeof(p_prvni->p_next[0])*obalnum);

    /* Pro kazdou pod-obalku zavolej delicku...
    */
    max_vzdal /= 2.0f;
    for(i = 0; i < obalnum; i++) {
      obbtree_vyrob_rec(p_prvni->p_next+i,max_vzdal);
    }
  }
}

void obbtree_zrus(OBB_TREE *p_ob)
{  
  int  i;
  if(p_ob->p_item) {
    free(p_ob->p_item);
    p_ob->p_item = NULL;
  }
  if(p_ob->p_next) {
    for(i = 0; i < p_ob->nextnum; i++) {
      obbtree_zrus(p_ob->p_next+i);
    }
    free(p_ob->p_next);
  }
}

/*
  Udela strom z poly
*/
int obb_visibility(OBB *p_obb, GLMATRIX *p_m)
{
 int i,j,xplane,yplane,xa,ya,m;
 float x,y,z,xt,yt,zt,w,fx,fy,fw;
 BOD *p_bod = p_obb->obb_hran;

 // ... // -> vyrobeni 8 hranicnich bodu krychle  
 xplane = 0; // -2, -1, 1, 2
 yplane = 0; // -2, -1, 1, 2

 for(i = 0, j = 0; i < 8; i++) {
   xt = p_bod->x;
   yt = p_bod->y;
   zt = p_bod->z;
   
   x = p_m->_11*xt + p_m->_21*yt + p_m->_31*zt + p_m->_41;
   y = p_m->_12*xt + p_m->_22*yt + p_m->_32*zt + p_m->_42;
   z = p_m->_13*xt + p_m->_23*yt + p_m->_33*zt + p_m->_43;   
   w = p_m->_14*xt + p_m->_24*yt + p_m->_34*zt + p_m->_44;
     
   // bod je pred kamerou
   m = (-w <= z && z <= w);
   if(m)
     j++;

   // bod lezi v obrazovce -> automaticky uspech
   if(m && -w <= x && x <= w && -w <= y && y <= w) {
     return(TRUE);
   }
   
   fx = (float)fabs(x);
   fy = (float)fabs(y);
   fw = (float)fabs(w);
      
   if(fx/fw > 1.0f) {
     xa = (x < 0.0f) ? -2 : 2;
   } else {
     xa = (x < 0.0f) ? -1 : 1;
   }
   
   if(fy/fw > 1.0f) {
     ya = (y < 0.0f) ? -2 : 2;
   } else {
     ya = (y < 0.0f) ? -1 : 1;
   }
      
   xplane += xa;
   yplane += ya;
   p_bod++;
 }

 return((j > 0) ? ((abs(xplane) != 16) && (abs(yplane) != 16)) : FALSE);
}
