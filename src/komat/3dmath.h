/*
  3D matematicke funkce
*/

#ifndef   __3D_MATH_H_
#define   __3D_MATH_H_

#ifndef PI
#define PI 3.14159265358979323846f
#endif

GLMATRIX * calc_reflex_matrix(GLMATRIX *p_mat, BOD *p_a, BOD *p_b, BOD *p_c);

int  calc_prusek(ROVINA *r, OBJ_VERTEX *a, OBJ_VERTEX *b, OBJ_VERTEX *p);
int  calc_prusek_bod(ROVINA *r, BOD *a, BOD *b, BOD *p);

int  prusek_face(OBJ_VERTEX *a, OBJ_VERTEX *b, OBJ_VERTEX *p, ROVINA *p_r, OBJ_VERTEX *_1, OBJ_VERTEX *_2, OBJ_VERTEX *_3);
int  prusek_face_editor(OBJ_VERTEX *a, OBJ_VERTEX *b, OBJ_VERTEX *p, OBJ_VERTEX *_1,OBJ_VERTEX *_2,OBJ_VERTEX *_3);

int  intersect_triangle_point(OBJ_VERTEX *_1, OBJ_VERTEX *_2, OBJ_VERTEX *_3, BOD *p);
int  je_uvnitr(float ax, float ay, float bx, float by, float cx, float cy, float px, float py);

int  intersect_triangle( BOD *p_orig, BOD *p_dir, BOD *p_v0, BOD *p_v1, BOD *p_v2, FLOAT *p_t, FLOAT *p_u, FLOAT *p_v);
int  intersect_triangle_ncull( BOD *p_orig, BOD *p_dir, BOD *p_v0, BOD *p_v1, BOD *p_v2, FLOAT *p_t, FLOAT *p_u, FLOAT *p_v);

GLMATRIX * calc_camera_3ds(GLMATRIX *p_cam, GLMATRIX *p_invcam, BOD *p_p, BOD *p_t, float roll);

void calc_camera(GLMATRIX *p_cmatrix, GLMATRIX *r_bod_matrix, float vzdal, float fi);
void calc_camera_bod(GLMATRIX *p_cmatrix, GLMATRIX *p_inv, BOD *p_p, float vzdal, float fi, float rfi);
void calc_r_bod(GLMATRIX *p_cmatrix, GLMATRIX *r_bod_matrix, float vzdal, float fi);

void prusecik(BOD *p_a, BOD *p_b, BOD *p_p, BOD *p_I);
void prusecik_mujbod(MUJ_BOD *p_a, MUJ_BOD *p_b, MUJ_BOD *p_p, MUJ_BOD *p_I);

GLMATRIX * calc_camera_polar(GLMATRIX *p_cam, GLMATRIX *p_invcam, BOD *p_p, QUAT *p_quat, float vzdal);


/* Bodove funkce
*/
inline void bod2mujbod(OBJ_VERTEX *p_obj, BOD *p_bod)
{
 p_obj->x = p_bod->x;
 p_obj->y = p_bod->y;
 p_obj->z = p_bod->z;
}

inline void norm2mujbod(OBJ_VERTEX *p_obj, BOD *p_bod)
{
 p_obj->nx = p_bod->x;
 p_obj->ny = p_bod->y;
 p_obj->nz = p_bod->z;
}

inline BOD * mujbod2bod(BOD *p_bod, OBJ_VERTEX *p_obj)
{
 p_bod->x = p_obj->x;
 p_bod->y = p_obj->y;
 p_bod->z = p_obj->z;
 return(p_bod);
}

inline ROVINA * calc_rovinu(OBJ_VERTEX *a, OBJ_VERTEX *b, OBJ_VERTEX *c, ROVINA *r)
{
 r->x = (b->y - a->y)*(c->z - a->z)-(c->y - a->y)*(b->z - a->z);
 r->y = (b->z - a->z)*(c->x - a->x)-(c->z - a->z)*(b->x - a->x);
 r->z = (b->x - a->x)*(c->y - a->y)-(c->x - a->x)*(b->y - a->y);
 r->e = -(r->x*a->x + r->y*a->y + r->z*a->z);
 return(r);
}

inline ROVINA * calc_rovinu_bod(BOD *a, BOD *b, BOD *c, ROVINA *r)
{
 r->x = (b->y - a->y)*(c->z - a->z)-(c->y - a->y)*(b->z - a->z);
 r->y = (b->z - a->z)*(c->x - a->x)-(c->z - a->z)*(b->x - a->x);
 r->z = (b->x - a->x)*(c->y - a->y)-(c->x - a->x)*(b->y - a->y);
 r->e = -(r->x*a->x + r->y*a->y + r->z*a->z);
 return(r);
}

inline ROVINA * calc_rovinu_bod_vektor(BOD *p, BOD *v, ROVINA *r)
{
  r->x = v->x;
  r->y = v->y;
  r->z = v->z;
  r->e = - v->x*p->x - v->y*p->y - v->z*p->z;
  return(r);
}

inline ROVINAD * calc_drovinu_bod(BOD *a, BOD *b, BOD *c, ROVINAD *r)
{
 r->x = (b->y - a->y)*(c->z - a->z)-(c->y - a->y)*(b->z - a->z);
 r->y = (b->z - a->z)*(c->x - a->x)-(c->z - a->z)*(b->x - a->x);
 r->z = (b->x - a->x)*(c->y - a->y)-(c->x - a->x)*(b->y - a->y);
 r->e = -(r->x*a->x + r->y*a->y + r->z*a->z);
 return(r);
}

inline BOD * calc_face_normal(BOD *a, BOD *b, BOD *c, BOD *n)
{
 n->x = (b->y - a->y)*(c->z - a->z)-(c->y - a->y)*(b->z - a->z);
 n->y = (b->z - a->z)*(c->x - a->x)-(c->z - a->z)*(b->x - a->x);
 n->z = (b->x - a->x)*(c->y - a->y)-(c->x - a->x)*(b->y - a->y); 
 return(n);
}

inline BOD * calc_face_stred(BOD *a, BOD *b, BOD *c, BOD *s)
{
 s->x = (a->x + b->x + c->x)/3.0f;
 s->y = (a->y + b->y + c->y)/3.0f;
 s->z = (a->z + b->z + c->z)/3.0f;
 return(s);
}

inline GLMATRIX * calc_3d_2d_matrix(GLMATRIX *p_word, GLMATRIX *p_cam, GLMATRIX *p_proj, GLMATRIX *p_mat)
{ 
 return(mat_mult(mat_mult(p_word,p_cam,p_mat),p_proj,p_mat));
}

inline void matrix_to_scale(GLMATRIX *p_m, BOD *p_s)
{
  p_s->x = norm_vect(&p_m->_11,&p_m->_21,&p_m->_31);
  p_s->y = norm_vect(&p_m->_12,&p_m->_22,&p_m->_32);
  p_s->z = norm_vect(&p_m->_13,&p_m->_23,&p_m->_33);
}

inline void matrix_to_pos(GLMATRIX *p_m, BOD *p_p)
{
  p_p->x = p_m->_41;
  p_p->y = p_m->_42;
  p_p->z = p_m->_43;
}

inline float matrix_to_float(GLMATRIX *p_mat)
{
  if(fabs(p_mat->_11) > fabs(p_mat->_21))
    return((float)acos(p_mat->_11));
  else
    return((float)asin(p_mat->_21));
}

inline void matrix_to_scale_2d(GLMATRIX *p_m, BOD *p_s)
{
  p_s->x = norm_vect(&p_m->_11,&p_m->_21,&p_m->_31);
  p_s->y = norm_vect(&p_m->_12,&p_m->_22,&p_m->_32);
}

inline void matrix_to_pos_2d(GLMATRIX *p_m, BOD *p_p)
{
  p_p->x = p_m->_41;
  p_p->y = p_m->_42;
}

inline GLMATRIX * calc_transf_3d_2d_matrix(GLMATRIX *p_w, GLMATRIX *p_c, GLMATRIX *p_p, GLMATRIX *p_v)
{
 GLMATRIX m;
 return((GLMATRIX *)glu_invert_matrix((float *)mat_mult(mat_mult(p_w,p_c,p_v),p_p,&m),(float *)p_v));
}

inline GLMATRIX * calc_transf_3d_2d_matrix_smpl(GLMATRIX *p_c, GLMATRIX *p_p, GLMATRIX *p_v)
{
 GLMATRIX m;
 return((GLMATRIX *)glu_invert_matrix((float *)mat_mult(p_c,p_p,&m),(float *)p_v));
}

inline void stred_to_obalka(BOD *p_prv, BOD *p_min, BOD *p_max, float dx, float dy, float dz)
{
  p_min->x = p_prv->x-dx;
  p_min->y = p_prv->y-dy;
  p_min->z = p_prv->z-dz;
  
  p_max->x = p_prv->x+dx;
  p_max->y = p_prv->y+dy;
  p_max->z = p_prv->z+dz;
}

#define DEG2RAD(fi)          (((fi)*PI)/180.0f)
#define RAD2DEG(fi)          (((fi)/PI)*180.0f)

#define atoff(s)             ((float)atof(s))
#define sqrtf(s)             ((float)sqrt((float)(s)))
#define sinf(s)              ((float)sin((float)(s)))
#define cosf(s)              ((float)cos((float)(s)))
#define asinf(s)             ((float)asin((float)(s)))
#define acosf(s)             ((float)acos((float)(s)))
#define fabsf(s)             ((float)fabs((float)(s)))

#define EPSILON 0.000001f

inline void kar2polar(BOD *p_car, float *p_r, float *p_fi, float *p_vzdal)
{
 BOD z = *p_car;
 float p; 
 *p_vzdal = vektor_norm(&z);
 *p_fi = asinf((fabsf(z.y) > 1.0f) ? (int)(z.y) : z.y); 
 *p_r = copysign(acosf((fabsf(p = (-z.z)/cosf(*p_fi)) > 1.0f) ? (int)(p) : p),z.x);
}

// polarni souradnice -> kartezske souradnice
inline void polar2kar(float r, float fi, float vzdal, BOD *p_car)
{   
  p_car->x = vzdal*sinf(r)*cosf(fi);
  p_car->z = -(vzdal*cosf(r)*cosf(fi));
  p_car->y = vzdal*sinf(fi);
}

// odriznuti periody 2PI + normalizace na +PI..-PI
inline float normalizuj_uhel(float uhel)
{
  if(fabs(uhel) > 2*PI) {
    uhel = (float)fmod((float)uhel,(float)2*PI);    
  }

  if(fabs(uhel) > PI) {
    return((uhel = (uhel > 0.0f) ? -(2*PI-uhel) : (2*PI+uhel)));
  }
  else {
    return(uhel);
  }
}

#define DELTA_MIN_ROZDIL 0.00000001f

#define stejny_vertex(v1,v2)       (fabsf(v1.x-v2.x) < DELTA_MIN_ROZDIL &&\
                                    fabsf(v1.y-v2.y) < DELTA_MIN_ROZDIL &&\
                                    fabsf(v1.z-v2.z) < DELTA_MIN_ROZDIL)\

inline int stejny_vertex_point(BOD *v1, BOD *v2)
{
  return(fabsf(v1->x-v2->x) < DELTA_MIN_ROZDIL &&
         fabsf(v1->y-v2->y) < DELTA_MIN_ROZDIL &&
         fabsf(v1->z-v2->z) < DELTA_MIN_ROZDIL);
}

/* Funkce vcetne Delta-Planu
*/
inline int stejny_vertex_point_delta(BOD *v1, BOD *v2, float delta)
{
  return(fabsf(v1->x-v2->x) < delta &&
         fabsf(v1->y-v2->y) < delta &&
         fabsf(v1->z-v2->z) < delta);
}

#endif
