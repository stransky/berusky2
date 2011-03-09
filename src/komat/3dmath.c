/*
  3D matematicke funkce
*/
#include "mss_on.h"
#include <windows.h>
#include "3d_all.h"

GLMATRIX * calc_reflex_matrix(GLMATRIX *p_mat, BOD *p_a, BOD *p_b, BOD *p_c)
{  
  float  e;
  QUAT   q;
 
  q.x = (p_b->y - p_a->y)*(p_c->z - p_a->z)-(p_c->y - p_a->y)*(p_b->z - p_a->z);
  q.y = (p_b->z - p_a->z)*(p_c->x - p_a->x)-(p_c->z - p_a->z)*(p_b->x - p_a->x);
  q.z = (p_b->x - p_a->x)*(p_c->y - p_a->y)-(p_c->x - p_a->x)*(p_b->y - p_a->y);
  
  norm_vect(&q.x,&q.y,&q.z);
  q.w = 0.0f;
  e = 2*(q.x*p_a->x + q.y*p_a->y + q.z*p_a->z);

  /*
    Init na standartni kartezskou souradnou soustavu
  */
  init_matrix(p_mat);

  /*
    Natoceni souradnic dle vektoru q
  */
  quat_to_matrix(p_mat,&q);

  /*
    Posun souradnic do pozadovane roviny
  */
  translate_matrix(p_mat,q.x*e,q.y*e,q.z*e);

  /*
    Reflexivituj souradnice
  */
  scale_matrix(p_mat,-1.0f,-1.0f,-1.0f);

  return(p_mat);
}


/*
 prusek s rovninou r (paprsek A->B) se ulozi do bodu p
*/
int calc_prusek(ROVINA *r, OBJ_VERTEX *a, OBJ_VERTEX *b, OBJ_VERTEX *p)
{
 #define TOL 0.001

 float t;
 float c,j;
 float qx,qy,qz;

 // q = B-A
 qx = b->x - a->x;
 qy = b->y - a->y;
 qz = b->z - a->z;

 c = -(r->x*a->x + r->y*a->y + r->z*a->z + r->e);
 j = r->x*qx + r->y*qy + r->z*qz;

 if((j < TOL)&&(j > -TOL)) {
   return(0); //   printf("Paprsek je rovnobezny z rovinou !");
 }
 t = c/j;

 // P = A - qt
 p->x = a->x + qx*t;
 p->y = a->y + qy*t;
 p->z = a->z + qz*t;

 return(1);
}

int calc_prusek_bod(ROVINA *r, BOD *a, BOD *b, BOD *p)
{
 #define TOL 0.001

 float t;
 float c,j;
 float qx,qy,qz;

 // q = B-A
 qx = b->x - a->x;
 qy = b->y - a->y;
 qz = b->z - a->z;

 c = -(r->x*a->x + r->y*a->y + r->z*a->z + r->e);
 j = r->x*qx + r->y*qy + r->z*qz;

 if((j < TOL)&&(j > -TOL)) {
   return(0); //   printf("Paprsek je rovnobezny z rovinou !");
 }
 t = c/j;

 // P = A - qt
 p->x = a->x + qx*t;
 p->y = a->y + qy*t;
 p->z = a->z + qz*t;

 return(1);
}

/*
  Opravit pruseky !!!!!!!!!!!!!!!!!
*/
int prusek_face(OBJ_VERTEX *a, OBJ_VERTEX *b, OBJ_VERTEX *p, ROVINA *p_r,
                OBJ_VERTEX *_1, OBJ_VERTEX *_2, OBJ_VERTEX *_3)
{
 if(calc_prusek(p_r, a, b, p)) {
   if(((p_r->x > p_r->z)&&(p_r->x > p_r->y))||(p_r->z == p_r->y))
     return(je_uvnitr(_1->y, _1->z, _2->y, _2->z, _3->y, _3->z, p->y, p->z));
   else if(((p_r->y > p_r->z)&&(p_r->y > p_r->x))||(p_r->z == p_r->x))
     return(je_uvnitr(_1->x, _1->z, _2->x, _2->z, _3->x, _3->z, p->x, p->z));
   else if(((p_r->z > p_r->x)&&(p_r->z > p_r->y))||(p_r->x == p_r->y))
     return(je_uvnitr(_1->x, _1->y,_2->x, _2->y,_3->x, _3->y,p->x, p->y));
   else  
     return(FALSE);
 }
 else
   return(FALSE);
}

int prusek_face_editor(OBJ_VERTEX *a, OBJ_VERTEX *b, OBJ_VERTEX *p,
                       OBJ_VERTEX *_1,OBJ_VERTEX *_2,OBJ_VERTEX *_3)
{
 ROVINA r;

 calc_rovinu(_1,_2,_3,&r);

 if(calc_prusek(&r, a, b, p)) {
   r.x = (float)fabs(r.x); r.y = (float)fabs(r.y); r.z = (float)fabs(r.z);
   if(((r.x > r.z)&&(r.x > r.y))||(r.z == r.y))
     return(je_uvnitr(_1->y, _1->z, _2->y, _2->z, _3->y, _3->z, p->y, p->z));
   else if(((r.y > r.z)&&(r.y > r.x))||(r.z == r.x))
     return(je_uvnitr(_1->x, _1->z, _2->x, _2->z, _3->x, _3->z, p->x, p->z));
   else if(((r.z > r.x)&&(r.z > r.y))||(r.x == r.y))
     return(je_uvnitr(_1->x, _1->y,_2->x, _2->y,_3->x, _3->y,p->x, p->y));
   else
     return(FALSE);
 }
 else
   return(FALSE);
}

int intersect_triangle_point(OBJ_VERTEX *_1, OBJ_VERTEX *_2, OBJ_VERTEX *_3, BOD *p)
{
 ROVINA r;

 calc_rovinu(_1,_2,_3,&r);
 r.x = (float)fabs(r.x); r.y = (float)fabs(r.y); r.z = (float)fabs(r.z);   
 if(((r.x > r.z)&&(r.x > r.y))||(r.y == r.z)) // prumet do z/y
   return(je_uvnitr(_1->y, _1->z, _2->y, _2->z, _3->y, _3->z, p->y, p->z));
 
 else if(((r.y > r.z)&&(r.y > r.x))||(r.x == r.z)) //prumet do x/z
   return(je_uvnitr(_1->x, _1->z, _2->x, _2->z, _3->x, _3->z, p->x, p->z));
 
 else if(((r.z > r.x)&&(r.z > r.y))||(r.y == r.x)) // prumet do x/y
   return(je_uvnitr(_1->x, _1->y,_2->x, _2->y,_3->x, _3->y,p->x, p->y));
 
 else {
   ddw("Velka chyba vrhani savle !");
   return(FALSE);
 }
}

#define MAX_HRANOVA_VZDAL 0.0001f

#define frvn(x,y,presnost) (fabs(x-y) < presnost)

int je_uvnitr(float ax, float ay, float bx, float by, float cx, float cy, float px, float py)
{
  float sp;
  int   v = 0;
  int   hrana = 0;

  #define calc_sp(a0,a1,b0,b1,p0,p1) ((b0 - a0)*(p1 - a1) - (b1 - a1)*(p0 - a0))
  
  // Face je z a->b, b->c, c->a
  // Pod musi lezet nalevo (napravo) od vsech techto primek

  if(frvn(ax,px,MAX_HRANOVA_VZDAL) && frvn(ay,py,MAX_HRANOVA_VZDAL))
    return(1);
  if(frvn(bx,px,MAX_HRANOVA_VZDAL) && frvn(by,py,MAX_HRANOVA_VZDAL))
    return(1);
  if(frvn(cx,px,MAX_HRANOVA_VZDAL) && frvn(cy,py,MAX_HRANOVA_VZDAL))
    return(1);

  //a->b
  sp = calc_sp(ax,ay,bx,by,px,py);

  if(MAX_HRANOVA_VZDAL > fabs(sp))
    hrana = 1;
  else if(sp > 0)
    v += 1;
  else
    v -= 1;


  //b->c .. a = b, b = c
  sp = calc_sp(bx,by,cx,cy,px,py);

  if(MAX_HRANOVA_VZDAL > fabs(sp))
    hrana = 1;
  else if(sp > 0)
    v += 1;
  else
    v -= 1;


  sp = calc_sp(cx,cy,ax,ay,px,py);

  if(MAX_HRANOVA_VZDAL > fabs(sp))
    hrana = 1;
  else if(sp > 0)
    v += 1;
  else
    v -= 1;

  if(abs(v) == 3)
    return(1);
  else {
    if(abs(v) == 2 && hrana)
      return(1);
    else
      return(0);
  }
}

int intersect_triangle( BOD *p_orig, BOD *p_dir, BOD *p_v0, BOD *p_v1, BOD *p_v2, FLOAT *p_t, FLOAT *p_u, FLOAT *p_v)
{
  BOD   edge1,edge2,tvec,pvec,qvec;
  float det, inv_det;

  // spocitam ramena trojuhelnika
  edge1.x = p_v1->x - p_v0->x;
  edge1.y = p_v1->y - p_v0->y;
  edge1.z = p_v1->z - p_v0->z;

  edge2.x = p_v2->x - p_v0->x;
  edge2.y = p_v2->y - p_v0->y;
  edge2.z = p_v2->z - p_v0->z;

  //  determinant - uhel mezi plochou dir&edge2 a vektorem edge1
  //  spocitam rovinu mezi dir&edge2
  pvec.x = p_dir->y*edge2.z - p_dir->z*edge2.y;
  pvec.y = p_dir->z*edge2.x - p_dir->x*edge2.z;
  pvec.z = p_dir->x*edge2.y - p_dir->y*edge2.x;

  // uhel mezi normalovym vektorem plochy dir&edge2 a edge1
  det = edge1.x*pvec.x + edge1.y*pvec.y + edge1.z*pvec.z;

  // lezi paprsek v rovine plosky (je rovnobezny s ploskou ?)
  // uhel mezi paprskem a ploskou je det
  // culling off
  if(det > -EPSILON && det < EPSILON)
    return(FALSE);

  // t = 0->orig
  tvec.x = p_orig->x - p_v0->x;
  tvec.y = p_orig->y - p_v0->y;
  tvec.z = p_orig->z - p_v0->z;

  // uhel mezi t a p
  *p_u = tvec.x*pvec.x + tvec.y*pvec.y + tvec.z*pvec.z;
  if( *p_u < 0.0f || *p_u > det)
    return(FALSE);

  // plocha mezi tvec a edge1
  qvec.x = tvec.y*edge1.z - tvec.z*edge1.y;
  qvec.y = tvec.z*edge1.x - tvec.x*edge1.z;
  qvec.z = tvec.x*edge1.y - tvec.y*edge1.x;

  // uhel mezi p_dir a qvec
  *p_v = (p_dir->x*qvec.x + p_dir->y*qvec.y + p_dir->z*qvec.z);

  // test hranic s = u, t = v
  if(*p_v < 0.0f || *p_u + *p_v > det)
    return(FALSE);

  // spocitani t parametru
  *p_t = (edge2.x*qvec.x + edge2.y*qvec.y + edge2.z*qvec.z);
  
  inv_det = 1.0f / det;
  *p_u *= inv_det;
  *p_v *= inv_det;
  *p_t *= inv_det;
  
  return(TRUE);
}

int intersect_triangle_ncull( BOD *p_orig, BOD *p_dir, BOD *p_v0, BOD *p_v1, BOD *p_v2, FLOAT *p_t, FLOAT *p_u, FLOAT *p_v)
{
  BOD   edge1,edge2,tvec,pvec,qvec;
  float det, inv_det;

  // spocitam ramena trojuhelnika
  edge1.x = p_v1->x - p_v0->x;
  edge1.y = p_v1->y - p_v0->y;
  edge1.z = p_v1->z - p_v0->z;

  edge2.x = p_v2->x - p_v0->x;
  edge2.y = p_v2->y - p_v0->y;
  edge2.z = p_v2->z - p_v0->z;

  //  determinant - uhel mezi plochou dir&edge2 a vektorem edge1
  //  spocitam rovinu mezi dir&edge2
  pvec.x = p_dir->y*edge2.z - p_dir->z*edge2.y;
  pvec.y = p_dir->z*edge2.x - p_dir->x*edge2.z;
  pvec.z = p_dir->x*edge2.y - p_dir->y*edge2.x;

  // uhel mezi normalovym vektorem plochy dir&edge2 a edge1
  det = edge1.x*pvec.x + edge1.y*pvec.y + edge1.z*pvec.z;

  // lezi paprsek v rovine plosky (je rovnobezny s ploskou ?)
  // uhel mezi paprskem a ploskou je det
  // culling off
  if(det > -EPSILON && det < EPSILON)
    return(FALSE);
  inv_det = 1.0f / det;

  // t = 0->orig
  tvec.x = p_orig->x - p_v0->x;
  tvec.y = p_orig->y - p_v0->y;
  tvec.z = p_orig->z - p_v0->z;

  // uhel mezi t a p
  *p_u = (tvec.x*pvec.x + tvec.y*pvec.y + tvec.z*pvec.z)*inv_det;
  if( *p_u < 0.0f || *p_u > 1.0f)
    return(FALSE);

  // plocha mezi tvec a edge1
  qvec.x = tvec.y*edge1.z - tvec.z*edge1.y;
  qvec.y = tvec.z*edge1.x - tvec.x*edge1.z;
  qvec.z = tvec.x*edge1.y - tvec.y*edge1.x;

  // uhel mezi p_dir a qvec
  *p_v = (p_dir->x*qvec.x + p_dir->y*qvec.y + p_dir->z*qvec.z)*inv_det;

  // test hranic s = u, t = v
  if(*p_v < 0.0f || *p_u + *p_v > 1.0f)
    return(FALSE);

  // spocitani t parametru
  *p_t = (edge2.x*qvec.x + edge2.y*qvec.y + edge2.z*qvec.z)*inv_det;
  
  return(TRUE);
}

/* Spocita kamerovou matici
*/
GLMATRIX * calc_camera_3ds(GLMATRIX *p_cam, GLMATRIX *p_invcam, BOD *p_p, BOD *p_t, float roll)
{
  GLMATRIX m;
  BOD   x,y,z;

  vektor_norm(vektor_sub(p_t,p_p,&z));

  y.x = y.z = 0.0f; y.y = 1.0f;
  vektor_norm(vektor_soucin(&y,&z,&x));
  vektor_norm(vektor_soucin(&z,&x,&y));
  
  p_cam->_14 = p_cam->_24 = p_cam->_34 = 0.0f; p_cam->_44 = 1.0f;
  p_cam->_41 = p_cam->_42 = p_cam->_43 = 0.0f;
  p_cam->_11 = x.x; p_cam->_21 = x.y; p_cam->_31 = x.z;
  p_cam->_12 = y.x; p_cam->_22 = y.y; p_cam->_32 = y.z;
  p_cam->_13 = z.x; p_cam->_23 = z.y; p_cam->_33 = z.z;

  translate_matrix(p_cam,-p_p->x,-p_p->y,-p_p->z);

  if(roll != 0.0f) {
    rotation_matrix_z(&m, roll);
    mat_mult(p_cam,&m,p_cam);
  }
    
  return(invert_matrix(p_cam,p_invcam));
}

GLMATRIX * calc_camera_polar(GLMATRIX *p_cam, GLMATRIX *p_invcam, BOD *p_p, QUAT *p_quat, float vzdal)
{  
  p_invcam->_14 = p_invcam->_24 = p_invcam->_34 = 0.0f;
  p_invcam->_44 = 1.0f;  
  p_invcam->_41 = p_p->x;
  p_invcam->_42 = p_p->y;
  p_invcam->_43 = p_p->z;
  
  quat_to_matrix(p_invcam,p_quat);       // jednotkove vektory do kamery
  translate_matrix(p_invcam,0,0,-vzdal); //+posun
  
  return(invert_matrix(p_invcam,p_cam));
}

void calc_camera(GLMATRIX *p_cmatrix, GLMATRIX *r_bod_matrix, float vzdal, float fi)
{
 GLMATRIX m;

 m = *r_bod_matrix; 
 rotate_matrix_x(&m,fi); 
 translate_matrix(&m,0,0,-vzdal);
 invert_matrix(&m,p_cmatrix);
}

void calc_camera_bod(GLMATRIX *p_cmatrix, GLMATRIX *p_inv, BOD *p_p, float vzdal, float fi, float rfi)
{
 init_matrix(p_inv);
 p_inv->_41 = p_p->x;
 p_inv->_42 = p_p->y;
 p_inv->_43 = p_p->z;
 rotate_matrix_y(p_inv,rfi);
 rotate_matrix_x(p_inv,fi);
 translate_matrix(p_inv,0,0,-vzdal);
 invert_matrix(p_inv,p_cmatrix);
}

void calc_r_bod(GLMATRIX *p_cmatrix, GLMATRIX *r_bod_matrix, float vzdal, float fi)
{
 *r_bod_matrix = *p_cmatrix;
 translate_matrix(r_bod_matrix,0,0,vzdal);
 rotate_matrix_x(r_bod_matrix,-fi);
}


// prusecik primky a,b a bodu p
void prusecik(BOD *p_a, BOD *p_b, BOD *p_p, BOD *p_I)
{
  float t_i,vz;

  t_i = (p_b->x - p_a->x)*(p_p->x-p_a->x)+(p_b->y - p_a->y)*(p_p->y-p_a->y)+(p_b->z - p_a->z)*(p_p->z-p_a->z);
  vz = (float)vzdal_bodu_xy(p_a->x,p_a->y,p_a->z,p_b->x,p_b->y,p_b->z);
  t_i /= (vz*vz);
    
  p_I->x = p_a->x + (p_b->x - p_a->x)*t_i;
  p_I->y = p_a->y + (p_b->y - p_a->y)*t_i;
  p_I->z = p_a->z + (p_b->z - p_a->z)*t_i;
}

void prusecik_mujbod(MUJ_BOD *p_a, MUJ_BOD *p_b, MUJ_BOD *p_p, MUJ_BOD *p_I)
{
  float t_i,vz;
  BOD   q;

  t_i = (p_b->x - p_a->x)*(p_p->x-p_a->x)+(p_b->y - p_a->y)*(p_p->y-p_a->y)+(p_b->z - p_a->z)*(p_p->z-p_a->z);
  vz = (float)vzdal_bodu_xy(p_a->x,p_a->y,p_a->z,p_b->x,p_b->y,p_b->z);
  t_i /= (vz*vz);
  
  q.x = p_b->x - p_a->x;
  q.y = p_b->y - p_a->y;
  q.z = p_b->z - p_a->z;
  
  p_I->x = p_a->x + q.x*t_i;
  p_I->y = p_a->y + q.y*t_i;
  p_I->z = p_a->z + q.z*t_i;
}
