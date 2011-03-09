/* OBB a jeho stromy
*/

#ifndef __OBB_H__
#define __OBB_H__

void  obb_body(OBB *p_obbs);
int   obb_intersect_obb(OBB *p_obb1, OBB *p_obb2);
int   obb_intersect_line(OBB *p_obb, BOD *p_orig, BOD *p_dir);
int   obb_intersect_line_dist(OBB *p_obb, BOD *p_orig, BOD *p_dir);
void  obb_obalka(GLMATRIX *p_mat, OBB *p_obb, BOD *p_min, BOD *p_max);
void  obb_slep(OBB *p_vys, OBB *p_list, int num);
void  obb_slep_aabb(OBB *p_vys, OBB *p_list, int num);

float obb_calc_obalka_obj(EDIT_OBJEKT *p_obj, BOD *p_vx, BOD *p_vy, BOD *p_vz, BOD *p_dist, BOD *p_stred);
void  obb_calc_obj(EDIT_OBJEKT *p_obj);
void  obb_calc_obj_fast(EDIT_OBJEKT *p_obj);
void  obb_calc_kont(EDIT_KONTEJNER *p_kont_top, int precizne);
float obb_calc_poly_obalka(EDIT_MESH_POLY *p_poly, BOD *p_vx, BOD *p_vy, BOD *p_vz, BOD *p_dist, BOD *p_stred);
void  obb_calc_poly(EDIT_MESH_POLY *p_poly);
void  obb_calc_item(OBB *p_vys, OBB_TREE_ITEM *p_item, int itemnum);

void  obbtree_vyrob(OBB_TREE *p_prvni, EDIT_MESH_POLY *p_poly, int polynum, GAME_MESH **p_mesh, int meshnum, int *p_ind, EDIT_MATERIAL **p_matlist);
void  obbtree_vyrob_rec(OBB_TREE *p_prvni, float max_vzdal);
void  obbtree_zrus(OBB_TREE *p_ob);
void  obbtree_kresli(OBB_TREE *p_prvni, dword barva);

int   obb_visibility(OBB *p_obb, GLMATRIX *p_m);
float obb_vzdal_bod(OBB *p_obb, BOD *p_bod);

void  obb_kresli_obalku(OBB *p_obb, dword barva, GLMATRIX *p_mat);

void  obb_prvek(OBB *p_obb);

__inline float obb_vzdal_rovina_x(OBB *p_obb, float x)
{
  float min = p_obb->aabb_min.x,
        max = p_obb->aabb_max.x;
  float vzdal1,vzdal2;

  if(min <= x && x <= max)
    return(0.0f);
  else {
    vzdal1 = fabsf(min-x);
    vzdal2 = fabsf(max-x);
    return(vzdal1 > vzdal2 ? vzdal1 : vzdal2);
  }
}

__inline float obb_vzdal_rovina_y(OBB *p_obb, float y)
{
  float min = p_obb->aabb_min.y,
        max = p_obb->aabb_max.y;  
  float vzdal1,vzdal2;

  if(min <= y && y <= max)
    return(0.0f);
  else {
    vzdal1 = fabsf(min-y);
    vzdal2 = fabsf(max-y);
    return(vzdal1 > vzdal2 ? vzdal1 : vzdal2);
  }
}

__inline float obb_vzdal_rovina_z(OBB *p_obb, float z)
{
  float min = p_obb->aabb_min.z,
        max = p_obb->aabb_max.z;  
  float vzdal1,vzdal2;

  if(min <= z && z <= max)
    return(0.0f);
  else {
    vzdal1 = fabsf(min-z);
    vzdal2 = fabsf(max-z);
    return(vzdal1 > vzdal2 ? vzdal1 : vzdal2);
  }
}

__inline float obb_vzdal_bod(OBB *p_obb, BOD *p_bod)
{
  BOD   v,v1,v2;
  float len;

  vektor_sub(p_bod,&p_obb->obb_stred,&v1);

  v.x = vektor_dot_product(p_obb->obb,  &v1);
  v.y = vektor_dot_product(p_obb->obb+1,&v1);
  v.z = vektor_dot_product(p_obb->obb+2,&v1);
  
  v2 = v1;
  len = vektor_norm_mult_vektor(&v2,&p_obb->obb_len);
  
  if(fabsf(v1.x) <= fabsf(v2.x) &&
     fabsf(v1.y) <= fabsf(v2.y) &&
     fabsf(v1.z) <= fabsf(v2.z)) {
    return(0.0f);
  } else {
    return(vektor_velikost(vektor_sub(&v1,&v2,&v1)));
  }
}

__inline float obb_vzdal_bod_aabb(OBB *p_obb, BOD *p_bod)
{
  BOD   v1,v2;
  float len;

  vektor_sub(p_bod,&p_obb->obb_stred,&v1);
  v2 = v1;
  len = vektor_norm_mult_vektor(&v2,&p_obb->obb_len);
  
  if(fabsf(v1.x) <= fabsf(v2.x) && 
     fabsf(v1.y) <= fabsf(v2.y) && 
     fabsf(v1.z) <= fabsf(v2.z)) {
    return(0.0f);
  } else {
    return(vektor_velikost(vektor_sub(&v1,&v2,&v1)));
  }
}

__inline int obb_je_bod_v_kostce(OBB *p_obb, BOD *p_bod)
{
  BOD   v,v1;
  BOD   *p_len;

  vektor_sub(p_bod,&p_obb->obb_stred,&v1);

  v.x = vektor_dot_product(p_obb->obb,  &v1);
  v.y = vektor_dot_product(p_obb->obb+1,&v1);
  v.z = vektor_dot_product(p_obb->obb+2,&v1);
  p_len = &p_obb->obb_len;
  
  if(fabsf(v.x) <= fabsf(p_len->x) && 
     fabsf(v.y) <= fabsf(p_len->y) && 
     fabsf(v.z) <= fabsf(p_len->z))
    return(TRUE);
  else
    return(FALSE);
}

__inline int obb_je_bod_v_kostce_aabb(OBB *p_obb, BOD *p_bod)
{
 BOD *p_min = &p_obb->aabb_min;
 BOD *p_max = &p_obb->aabb_max;
 return(p_min->x <= p_bod->x && p_bod->x <= p_max->x &&
        p_min->y <= p_bod->y && p_bod->y <= p_max->y &&
        p_min->z <= p_bod->z && p_bod->z <= p_max->z);
}

__inline void obb_aabb_obj(EDIT_OBJEKT *p_obj, OBB *p_obb)
{
  objekt_obalka(p_obj,NULL,&p_obb->aabb_min, &p_obb->aabb_max);  
}

__inline void obb_aabb_poly(EDIT_MESH_POLY *p_poly, OBB *p_obb)
{  
  poly_obalka(p_poly,NULL,&p_obb->aabb_min, &p_obb->aabb_max);  
}

__inline void obb_aabb_kont(EDIT_KONTEJNER *p_kont, OBB *p_obb)
{
  kontejner_obalka_aabb(p_kont, &p_obb->aabb_min, &p_obb->aabb_max);
}

__inline void obb_aabb(OBB *p_obb)
{
  obb_obalka(NULL, p_obb, &p_obb->aabb_min, &p_obb->aabb_max);
}

__inline void obb_transformuj(OBB *p_src, GLMATRIX *p_mat, OBB *p_dest)
{
  GLMATRIX m;
  BOD      a1;
  float    lx,ly,lz;
  
  transformuj_bod_matici_bod(&p_src->obb_stred,p_mat,&p_dest->obb_stred);
  lx = p_src->obb_len.x;
  ly = p_src->obb_len.y;
  lz = p_src->obb_len.z;

  m = *p_mat; m._41 = m._42 = m._43 = 0.0f;

  if(lx != 0.0f) {
    vektor_mult_skalar(p_src->obb,lx,&a1);
    p_dest->obb_len.x = vektor_norm(transformuj_bod_matici_bod(&a1, &m, p_dest->obb));
  } else {
    transformuj_bod_matici_bod(p_src->obb, &m, p_dest->obb);
    p_dest->obb_len.x = 0.0f;
  }

  if(ly != 0.0f) {
    vektor_mult_skalar(p_src->obb+1,ly,&a1);
    p_dest->obb_len.y = vektor_norm(transformuj_bod_matici_bod(&a1, &m, p_dest->obb+1));
  } else {
    transformuj_bod_matici_bod(p_src->obb+1, &m, p_dest->obb+1);
    p_dest->obb_len.y = 0.0f;
  }

  if(lz != 0.0f) {
    vektor_mult_skalar(p_src->obb+2,lz,&a1);
    p_dest->obb_len.z = vektor_norm(transformuj_bod_matici_bod(&a1, &m, p_dest->obb+2));
  } else {
    transformuj_bod_matici_bod(p_src->obb+2, &m, p_dest->obb+2);
    p_dest->obb_len.z = 0.0f;
  }

  obb_body(p_dest);
  obb_aabb(p_dest);
}

__inline void obb_transformuj_aabb(OBB *p_src, GLMATRIX *p_mat, OBB *p_dest)
{
  BOD *p_s1 = p_src->obb_hran,
      *p_s2 = p_dest->obb_hran;
  int i;
  for(i = 0; i < 8; i++) 
    transformuj_bod_matici_bod(p_s1+i, p_mat,p_s2+i);
  obb_aabb(p_dest);
}

__inline void obb_kresli_obalku_obj(EDIT_OBJEKT *p_obj, GLMATRIX *p_mat, dword barva)
{    
  obb_kresli_obalku(&p_obj->obb,barva,p_mat);
}

__inline void obb_kresli_obalku_kont(EDIT_KONTEJNER *p_kont, GLMATRIX *p_mat, dword barva)
{   
  obb_kresli_obalku(&p_kont->obb,barva,p_mat);
}

#endif

