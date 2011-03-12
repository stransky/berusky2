/*
  Vektory
*/

#ifndef __VEKTORY_H
#define __VEKTORY_H

#define calc_vect(_a,_b,_v)\
{                          \
 (_v.x) = (_b.x) - (_a.x);       \
 (_v.y) = (_b.y) - (_a.y);       \
 (_v.z) = (_b.z) - (_a.z);       \
}                          \

#define calc_vect_dir_vec(_a,_b,_v)\
{                          \
 _v.x = _b->x - _a.x;       \
 _v.y = _b->y - _a.y;       \
 _v.z = _b->z - _a.z;       \
}                          \

#define calc_vect_dir(_a,_b,_v)\
{                          \
 _v.x = _b->x - _a->x;       \
 _v.y = _b->y - _a->y;       \
 _v.z = _b->z - _a->z;       \
}                          \

#define add_vect(_a,_b,_v) \
{                          \
 _v.x = _b.x + _a.x;       \
 _v.y = _b.y + _a.y;       \
 _v.z = _b.z + _a.z;       \
}                          \

#define sub_vect(_a,_b,_v) \
{                          \
 _v.x = _b.x - _a.x;       \
 _v.y = _b.y - _a.y;       \
 _v.z = _b.z - _a.z;       \
}                          \

#define skal_soucin(_a,_b) (float)(((_b.x)*(_a.x))+((_b.y)*(_a.y))+((_b.z)*(_a.z)))

#define vel_vect(_a,vel)                  \
{                                         \
  vel = (float)sqrt(_a.x*_a.x + _a.y*_a.y + _a.z*_a.z);\
}                                         \

#define vel_vect_sqrt(_a) (float)((_a.x)*(_a.x)+(_a.y)*(_a.y)+(_a.z)*(_a.z))

#define vzdal_bodu_dir(_a,_b) ((float)sqrt((_b->x-_a->x)*(_b->x-_a->x)+(_b->y-_a->y)*(_b->y-_a->y)+(_b->z-_a->z)*(_b->z-_a->z)))
#define vzdal_bodu_dir_sqrt(_a,_b) ((float)((_b->x-_a->x)*(_b->x-_a->x)+(_b->y-_a->y)*(_b->y-_a->y)+(_b->z-_a->z)*(_b->z-_a->z)))
#define vzdal_bodu_bod_sqrt(_a,_b) (((_b.x-_a.x)*(_b.x-_a.x)+(_b.y-_a.y)*(_b.y-_a.y)+(_b.z-_a.z)*(_b.z-_a.z)))
#define vzdal_bodu_bod_vec(_a,_b) ((float)sqrt((_b.x-_a->x)*(_b.x-_a->x)+(_b.y-_a->y)*(_b.y-_a->y)+(_b.z-_a->z)*(_b.z-_a->z)))
#define vzdal_bodu_bod_vec_sqrt(_a,_b) ((float)sqrt((_b.x-_a->x)*(_b.x-_a->x)+(_b.y-_a->y)*(_b.y-_a->y)+(_b.z-_a->z)*(_b.z-_a->z)))

#define vel_vect_dir(_a)         ((float)sqrt((_a.x)*(_a.x) + (_a.y)*(_a.y) + (_a.z)*(_a.z)))
#define vel_vect_dir_2d(_a)      ((float)sqrt((_a.x)*(_a.x) + (_a.y)*(_a.y)))
#define vel_vect_dir_vect(_a)    ((float)sqrt((_a->x)*(_a->x) + (_a->y)*(_a->y) + (_a->z)*(_a->z)))
#define vel_vect_dir_vect_2d(_a) ((float)sqrt((_a->x)*(_a->x) + (_a->y)*(_a->y)))

#define vzdal_bodu_xy(x1,y1,z1,x2,y2,z2) ((float)sqrt(((x2)-(x1))*((x2)-(x1))+((y2)-(y1))*((y2)-(y1))+((z2)-(z1))*((z2)-(z1))))

#define vzdal_bodu_2D_xy(x1,y1,x2,y2) ((float)sqrt(((x2)-(x1))*((x2)-(x1))+((y2)-(y1))*((y2)-(y1))))

#define interpoluj_vec(p_a,p_b,p_i)\
{\
  (p_i)->x = ((p_a)->x+(p_b)->x)*0.5f;\
  (p_i)->y = ((p_a)->y+(p_b)->y)*0.5f;\
  (p_i)->z = ((p_a)->z+(p_b)->z)*0.5f;\
}\

#define interpoluj_vec_bod(a,b,i)\
{\
  i.nx = (a.nx+b.nx)*0.5f;\
  i.ny = (a.ny+b.ny)*0.5f;\
  i.nz = (a.nz+b.nz)*0.5f;\
}\


__inline float vektor_uhel(BOD *p_v1, BOD *p_v2)
{
 return((float)p_v1->x*p_v2->x+p_v1->y*p_v2->y+p_v1->z*p_v2->z);
}

__inline float vektor_dot_product(BOD *p_v1, BOD *p_v2)
{
 return((float)p_v1->x*p_v2->x+p_v1->y*p_v2->y+p_v1->z*p_v2->z);
}

__inline float wvektor_dot_product(WBOD *p_v1, WBOD *p_v2)
{
 return((float)p_v1->x*p_v2->x+p_v1->y*p_v2->y+p_v1->z*p_v2->z+p_v1->w*p_v2->w);
}

__inline BOD * vektor_soucin(BOD *u, BOD *v, BOD *c)
{
 c->x = u->y*v->z - u->z*v->y;
 c->y = u->z*v->x - u->x*v->z;
 c->z = u->x*v->y - u->y*v->x;
 return(c);
}

__inline float vektor_velikost(BOD *v)
{
 float dis = v->x*v->x+v->y*v->y+v->z*v->z;
 if(dis > 0.0f)
   return((float)sqrt(dis));
 else
   return(0.0f); 
}

__inline float vektor_velikost_2D(BOD *v)
{
 float dis = v->x*v->x+v->y*v->y;
 if(dis > 0.0f)
   return((float)sqrt(dis));
 else
   return(0.0f); 
}

__inline float vektor_norm(BOD *v)
{
 float vel2,vel = vektor_velikost(v);
 if(vel == 0) 
   return(0.0f);//vel = 1;
 else {
   vel2 = 1.0f/vel; 
   v->x *= vel2;
   v->y *= vel2;
   v->z *= vel2;
   return(vel);
 }
}

__inline float vektor_norm_2D(BOD *v)
{
 float vel2,vel = vektor_velikost_2D(v);
 if(vel == 0) 
   return(0.0f);//vel = 1;
 else {
   vel2 = 1.0f/vel; 
   v->x *= vel2;
   v->y *= vel2; 
   return(vel);
 }
}

__inline float vektor_norm_mult(BOD *v, float num)
{
 float vel2,vel = vektor_velikost(v);
 if(vel == 0) 
   return(0.0f);//vel = 1;
 else {
   vel2 = num/vel;
   v->x *= vel2;
   v->y *= vel2;
   v->z *= vel2;
   return(vel);
 }
}

__inline float vektor_norm_mult_vektor(BOD *v, BOD *p_mult)
{
 float n,vel = vektor_velikost(v);
 if(vel == 0)
   return(0.0f);//vel = 1;
 else {
   n = 1.0f/vel;
   v->x = v->x*n*p_mult->x;
   v->y = v->y*n*p_mult->y;
   v->z = v->z*n*p_mult->z;
   return(vel);
 }
}

__inline BOD * vektor_add(BOD *v, BOD *u, BOD *c)
{
  c->x = v->x + u->x;
  c->y = v->y + u->y;
  c->z = v->z + u->z;
  return(c);
}

__inline BOD * vektor_mult(BOD *v, BOD *u, BOD *c)
{
  c->x = v->x * u->x;
  c->y = v->y * u->y;
  c->z = v->z * u->z;
  return(c);
}

__inline BOD * vektor_sub(BOD *v, BOD *u, BOD *c)
{
  c->x = v->x - u->x;
  c->y = v->y - u->y;
  c->z = v->z - u->z;
  return(c);
}

__inline BOD * vektor_set(BOD *v, float hodnota)
{
  v->x = hodnota;
  v->y = hodnota;
  v->z = hodnota;
  return(v);
}

__inline BOD * vektor_set_all(BOD *v, float x, float y, float z)
{
  v->x = x;
  v->y = y;
  v->z = z;
  return(v);
}

// test na preteceni
#define MAX_UP 1.0f
__inline BOD * vektor_norm_up(BOD *v)
{
  if(v->x > MAX_UP) v->x = MAX_UP;
  if(v->y > MAX_UP) v->y = MAX_UP;
  if(v->z > MAX_UP) v->z = MAX_UP;
  return(v);
}

// test na podteceni
#define MAX_DOWN 0.0f
__inline BOD * vektor_norm_down(BOD *v)
{
  if(v->x < MAX_DOWN) v->x = MAX_DOWN;
  if(v->y < MAX_DOWN) v->y = MAX_DOWN;
  if(v->z < MAX_DOWN) v->z = MAX_DOWN;
  return(v);
}

__inline BOD * vektor_norm_bump(BOD *v)
{
  v->x = v->x*0.5f+0.5f;
  v->y = v->y*0.5f+0.5f;
  v->z = v->z*0.5f+0.5f;
  return(v);
}

__inline BOD * vektor_bod(BOD *v, BOD *u, BOD *c, float t)
{
  float t2 = 1.0f-t;
  c->x = v->x*t2 + u->x*t;
  c->y = v->y*t2 + u->y*t;
  c->z = v->z*t2 + u->z*t;
  return(c);
}

__inline OBJ_VERTEX * vektor_bod_muj(OBJ_VERTEX *v, OBJ_VERTEX *u, OBJ_VERTEX *c, float t)
{
  float t2 = 1.0f-t;
  
  c->x = v->x*t2 + u->x*t;
  c->y = v->y*t2 + u->y*t;
  c->z = v->z*t2 + u->z*t;
  
  c->tu1 = v->tu1*t2 + u->tu1*t;
  c->tv1 = v->tv1*t2 + u->tv1*t;
  
  c->mda = v->mda*t2 + u->mda*t;
  c->mdr = v->mdr*t2 + u->mdr*t;
  c->mdg = v->mdg*t2 + u->mdg*t;
  c->mdb = v->mdb*t2 + u->mdb*t;

  c->da = v->da*t2 + u->da*t;
  c->dr = v->dr*t2 + u->dr*t;
  c->dg = v->dg*t2 + u->dg*t;
  c->db = v->db*t2 + u->db*t;

  c->msr = v->msr*t2 + u->msr*t;
  c->msg = v->msg*t2 + u->msg*t;
  c->msb = v->msb*t2 + u->msb*t;

  c->sr = v->sr*t2 + u->sr*t;
  c->sg = v->sg*t2 + u->sg*t;
  c->sb = v->sb*t2 + u->sb*t;

  return(c);
}

__inline BOD * vektor_copy(BOD *c, BOD *v)
{
  c->x = v->x;
  c->y = v->y;
  c->z = v->z;
  return(c);
}

__inline WBOD * wvektor_sub(WBOD *v, WBOD *u, WBOD *c)
{
  c->x = v->x - u->x;
  c->y = v->y - u->y;
  c->z = v->z - u->z;
  c->w = v->w - u->w;
  return(c);
}

__inline BOD * vektor_mult_skalar(BOD *v, float mult, BOD *c)
{
  c->x = v->x * mult;
  c->y = v->y * mult;
  c->z = v->z * mult;
  return(c);
}

__inline BOD * vektor_scale(BOD *v, float mult)
{
  v->x *= mult;
  v->y *= mult;
  v->z *= mult;
  return(v);
}

__inline BOD * vektor_fabs(BOD *v, BOD *c)
{
  c->x = (float)fabs(v->x);
  c->y = (float)fabs(v->y);
  c->z = (float)fabs(v->z);
  return(c);
}

__inline BOD * vektor_inv(BOD *v)
{
  v->x = -(v->x);
  v->y = -(v->y);
  v->z = -(v->z);
  return(v);
}

__inline float vzdal_bodu(OBJ_VERTEX *a, OBJ_VERTEX *b)
{
  return((float)sqrt((b->x-a->x)*(b->x-a->x)+(b->y-a->y)*(b->y-a->y)+(b->z-a->z)*(b->z-a->z)));
}

__inline float vzdal_bodu_bod(BOD *a, BOD *b)
{
  float v = (b->x-a->x)*(b->x-a->x)+(b->y-a->y)*(b->y-a->y)+(b->z-a->z)*(b->z-a->z);
  return((v > 0.0f) ? (float)sqrt(v) : v);
}

__inline float vzdal_bodu_2D(OBJ_VERTEX *a, OBJ_VERTEX *b)
{
  float v = (b->x-a->x)*(b->x-a->x)+(b->z-a->z)*(b->z-a->z);  
  return((v > 0.0f) ? (float)sqrt(v) : v);
}

__inline float vzdal_bodu_2D_bod(BOD *a, BOD *b)
{
  float v  = (b->x-a->x)*(b->x-a->x)+(b->z-a->z)*(b->z-a->z);
  return((v > 0.0f) ? (float)sqrt(v) : v);
}

__inline void calc_primku_2d(BOD *a, BOD *b, float *p_k, float *p_q)
{
  *p_k = (a->y - b->y)/(a->x - b->x);
  *p_q = a->y - (*p_k)*a->x;
}

__inline void calc_primku_3d(BOD *a, BOD *b, BOD *q) // t je parametr od 0 do 1
{
  q->x = b->x - a->x;
  q->y = b->y - a->y;
  q->z = b->z - a->z;
}

__inline float norm_vect(float *p_x, float *p_y, float *p_z)
{
 float vel = (float)sqrt((*p_x)*(*p_x) + (*p_y)*(*p_y) + (*p_z)*(*p_z));

 if(vel == 0.0f) 
   return(0.0f);//vel = 1;

 *p_x /= vel;
 *p_y /= vel;
 *p_z /= vel;
 
 return(vel);
}

__inline float norm_vect_2D(float *p_x, float *p_y)
{
 float vel = (float)sqrt((*p_x)*(*p_x) + (*p_y)*(*p_y));
 *p_x /= vel;
 *p_y /= vel;
 return(vel);
}

__inline float norm_rovinu(ROVINA *r)
{
 float vel = (float)sqrt(r->x*r->x + r->y*r->y + r->z*r->z);

 if(vel == 0.0f) return(0.0f);//vel = 1;

 r->x /= vel;
 r->y /= vel;
 r->z /= vel;
 r->e /= vel;
 
 return(vel);
}

// p,a,b - primka
__inline float vzdal_bodu_a_primky(BOD *p, BOD *a, BOD *b)
{
 float ti,v;
 float qx,qy,qz;
 BOD pr;

 // q = B-A
 qx = b->x - a->x;
 qy = b->y - a->y;
 qz = b->z - a->z;

 ti = (b->x - a->x)*(p->x - a->x) + (b->y - a->y)*(p->y - a->y) + (b->z - a->z)*(p->z - a->z);
 v = vzdal_bodu_dir(a,b);
 ti /= (v*v);

 // P = A - qt
 pr.x = a->x + qx*ti;
 pr.y = a->y + qy*ti;
 pr.z = a->z + qz*ti;

 return(vzdal_bodu_bod(&pr,p));
}

// p,a,b - primka
__inline float vzdal_bodu_a_primky_bod(BOD *p, BOD *a, BOD *b, BOD *i)
{
 float ti,v;
 float qx,qy,qz;

 // q = B-A
 qx = b->x - a->x;
 qy = b->y - a->y;
 qz = b->z - a->z;

 ti = (b->x - a->x)*(p->x - a->x) + (b->y - a->y)*(p->y - a->y) + (b->z - a->z)*(p->z - a->z);
 v = vzdal_bodu_dir(a,b);
 ti /= (v*v);

 // P = A - qt
 i->x = a->x + qx*ti;
 i->y = a->y + qy*ti;
 i->z = a->z + qz*ti;

 return(vzdal_bodu_dir(i,p));
}

//vraci i bod na primce a->b
__inline float bod_primka_bod_bod(BOD *p, BOD *a, BOD *b, BOD *i)
{
 float ti,v;
 float qx,qy,qz;

 // q = B-A
 qx = b->x - a->x;
 qy = b->y - a->y;
 qz = b->z - a->z;

 ti = (b->x - a->x)*(p->x - a->x) + (b->y - a->y)*(p->y - a->y) + (b->z - a->z)*(p->z - a->z);
 v = vzdal_bodu_dir(a,b);
 ti /= (v*v);

 // P = A - qt
 i->x = a->x + qx*ti;
 i->y = a->y + qy*ti;
 i->z = a->z + qz*ti;

 return(ti);
}

__inline float vzdal_bodu_a_roviny(BOD *p, ROVINA *r)
{
  return((float)fabs(r->x*p->x+r->y*p->y+r->z*p->z+r->e));
}

__inline float vzdal_bodu_a_roviny_nabs(BOD *p, ROVINA *r)
{
  return(r->x*p->x+r->y*p->y+r->z*p->z+r->e);
}

#endif