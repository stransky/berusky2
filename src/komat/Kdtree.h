/*
  Oktanovy stromy

  Scena + okoli -> KD strom
  Prvky -> obalky

*/
#ifndef __KDTREE_H
#define __KDTREE_H

#define KD_FULL_VISIBILITY 8

/*
  Bunka -> min/max
  - zpusob rozdeleni (x/y/z)
  - vzdalenost delici roviny

  - Prusecik focus s tim ctvercem (ray-tracing)
  - prusecik s kamerou

*/

#define  KD_POLY      1
#define  KD_MESH      2

typedef struct _KD_BUNKA
{

  int mail;
  int polozek;
  void **p_polozka;             //pole polozek (poly/kont)
  int *p_polozka_typ;           // typy polozek (kontejner/poly)
  BOD min, max;
  int rovina;                   // delici rovina podstromu
  float vzdal;                  // vzdalenost od min
  int cislo;
  struct _KD_BUNKA *p_up;       // otec stromu
  struct _KD_BUNKA *p_next;     // dalsich 2 podstromu nebo nic

} KD_BUNKA;


void kd_strom_vyrob(EDIT_MESH_POLY * p_poly, int polynum,
  EDIT_KONTEJNER ** p_kont, int kontnum, KD_BUNKA * p_prvni);
void kd_strom_tiskni(KD_BUNKA * p_prvni, int hloubka);
int kd_visibility(BOD * p_min, BOD * p_max, GLMATRIX * p_m);
int kd_visibility_full(BOD * p_min, BOD * p_max, GLMATRIX * p_m);
int kd_intersect_kostku(BOD * p_a, BOD * p_b, BOD * p_min, BOD * p_max);
int kd_intersect_kostku_dir(BOD * p_orig, BOD * p_dir, BOD * p_min,
  BOD * p_max);
int kd_intersect_kostku_bod(BOD * p_a, BOD * p_b, BOD * p_min, BOD * p_max,
  BOD * p_p, float *p_t);
int kd_intersect_kostku_bod_inter(BOD * p_a, BOD * p_b, BOD * p_min,
  BOD * p_max, BOD * p_p, float *p_t);
void kresli_obalku(BOD min, BOD max, dword barva, GLMATRIX * p_tr);

#define RIF(min,max) (((max)+(min))*0.5f)
#ifndef MAX
#define MAX(a,b)     (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)     (((a) < (b)) ? (a) : (b))
#endif

inline void kd_korekce_bunky(BOD * p_min, BOD * p_max)
{
  float tmp;

  if (p_min->x > p_max->x) {
    tmp = p_min->x;
    p_min->x = p_max->x;
    p_max->x = tmp;
  }
  if (p_min->y > p_max->y) {
    tmp = p_min->y;
    p_min->y = p_max->y;
    p_max->y = tmp;
  }
  if (p_min->z > p_max->z) {
    tmp = p_min->z;
    p_min->z = p_max->z;
    p_max->z = tmp;
  }
}

inline void kd_bunka_min(BOD * p_min, BOD * p_gmin)
{
  if (p_min->x < p_gmin->x)
    p_gmin->x = p_min->x;
  if (p_min->y < p_gmin->y)
    p_gmin->y = p_min->y;
  if (p_min->z < p_gmin->z)
    p_gmin->z = p_min->z;
}

inline void kd_bunka_max(BOD * p_max, BOD * p_gmax)
{
  if (p_max->x > p_gmax->x)
    p_gmax->x = p_max->x;
  if (p_max->y > p_gmax->y)
    p_gmax->y = p_max->y;
  if (p_max->z > p_gmax->z)
    p_gmax->z = p_max->z;
}

inline void kd_bunka_min_max(BOD * p_str, BOD * p_gmin, BOD * p_gmax)
{
  if (p_str->x < p_gmin->x)
    p_gmin->x = p_str->x;
  if (p_str->y < p_gmin->y)
    p_gmin->y = p_str->y;
  if (p_str->z < p_gmin->z)
    p_gmin->z = p_str->z;

  if (p_str->x > p_gmax->x)
    p_gmax->x = p_str->x;
  if (p_str->y > p_gmax->y)
    p_gmax->y = p_str->y;
  if (p_str->z > p_gmax->z)
    p_gmax->z = p_str->z;
}

inline void kd_bunka_min_max_koule(BOD * p_str, float radius, BOD * p_gmin,
  BOD * p_gmax)
{
  if (p_str->x + radius < p_gmin->x)
    p_gmin->x = p_str->x + radius;
  if (p_str->y + radius < p_gmin->y)
    p_gmin->y = p_str->y + radius;
  if (p_str->z + radius < p_gmin->z)
    p_gmin->z = p_str->z + radius;

  if (p_str->x - radius > p_gmax->x)
    p_gmax->x = p_str->x - radius;
  if (p_str->y - radius > p_gmax->y)
    p_gmax->y = p_str->y - radius;
  if (p_str->z - radius > p_gmax->z)
    p_gmax->z = p_str->z - radius;
}

inline float kd_bunka_obsah(BOD * p_min, BOD * p_max)
{
  float a = p_max->x - p_min->x;
  float b = p_max->y - p_min->y;
  float c = p_max->z - p_min->z;

  return (a * a + b * b + c * c);
}

/*
  Stred bunky je RIF(min,max)
*/
inline BOD *kd_stred_bunky(BOD * p_min, BOD * p_max, BOD * p_bod)
{
  p_bod->x = RIF(p_min->x, p_max->x);
  p_bod->y = RIF(p_min->y, p_max->y);
  p_bod->z = RIF(p_min->z, p_max->z);
  return (p_bod);
}

inline BOD *kd_len_bunky(BOD * p_min, BOD * p_max, BOD * p_len)
{
  p_len->x = (p_max->x - p_min->x) * 0.5f;
  p_len->y = (p_max->y - p_min->y) * 0.5f;
  p_len->z = (p_max->z - p_min->z) * 0.5f;
  return (p_len);
}

inline void kd_bunka_expanze(BOD * p_stred, BOD * p_len, BOD * p_min,
  BOD * p_max)
{
  vektor_sub(p_stred, p_len, p_min);
  vektor_add(p_stred, p_len, p_max);
}

inline void kd_bunka_expanze2(BOD * p_len, BOD * p_min, BOD * p_max)
{
  BOD v(0, 0, 0);

  vektor_sub(&v, p_len, p_min);
  vektor_add(&v, p_len, p_max);
}

inline void kd_bunka_expanze3(float x, float y, float z, float dx, float dy,
  float dz, BOD * p_min, BOD * p_max)
{
  p_min->x = x - dx;
  p_max->x = x + dx;
  p_min->y = y - dy;
  p_max->y = y + dy;
  p_min->z = z - dz;
  p_max->z = z + dz;
}

inline void kd_je_bod_v_kostce_orez(BOD * p_bod, BOD * p_min, BOD * p_max)
{
  if (p_min->x > p_bod->x)
    p_bod->x = p_min->x;
  else {
    if (p_max->x < p_bod->x)
      p_bod->x = p_max->x;
  }

  if (p_min->y > p_bod->y)
    p_bod->y = p_min->y;
  else {
    if (p_max->y < p_bod->y)
      p_bod->y = p_max->y;
  }

  if (p_min->z > p_bod->z)
    p_bod->z = p_min->z;
  else {
    if (p_max->z < p_bod->z)
      p_bod->z = p_max->z;
  }
}

inline int kd_je_bod_v_kostce(BOD * p_bod, BOD * p_min, BOD * p_max)
{
  return (p_min->x <= p_bod->x && p_bod->x <= p_max->x &&
    p_min->y <= p_bod->y && p_bod->y <= p_max->y &&
    p_min->z <= p_bod->z && p_bod->z <= p_max->z);
}

inline int kd_je_bod_v_bunce(KD_BUNKA * p_bunka, BOD * p_bod)
{
  return (p_bunka->min.x <= p_bod->x && p_bod->x <= p_bunka->max.x &&
    p_bunka->min.y <= p_bod->y && p_bod->y <= p_bunka->max.y &&
    p_bunka->min.z <= p_bod->z && p_bod->z <= p_bunka->max.z);
}

inline int kd_je_bod_v_kostce_stred(BOD * p_bod, BOD * p_str, float vzdal)
{
  return (p_str->x - vzdal <= p_bod->x && p_bod->x <= p_str->x + vzdal &&
    p_str->y - vzdal <= p_bod->y && p_bod->y <= p_str->y + vzdal &&
    p_str->z - vzdal <= p_bod->z && p_bod->z <= p_str->z + vzdal);
}

// rozdily mezi bodem a kostkou
inline float kd_intersect_kostku_xyz(BOD * p_a, BOD * p_min, BOD * p_max,
  int rovina)
{
  float v1, v2;

  // rovina X
  if (!rovina) {
    if (p_a->x >= p_min->x && p_a->x <= p_max->x) {
      return (0.0f);
    }
    else {
      v1 = (float) fabs(p_a->x - p_min->x);
      v2 = (float) fabs(p_a->x - p_max->x);
      return (v1 < v2 ? v1 : v2);
    }
  }
  else if (rovina == 1) {
    if (p_a->y >= p_min->y && p_a->y <= p_max->y) {
      return (0.0f);
    }
    else {
      v1 = (float) fabs(p_a->y - p_min->y);
      v2 = (float) fabs(p_a->y - p_max->y);
      return (v1 < v2 ? v1 : v2);
    }
  }
  else if (rovina == 2) {
    if (p_a->z >= p_min->z && p_a->z <= p_max->z) {
      return (0.0f);
    }
    else {
      v1 = (float) fabs(p_a->z - p_min->z);
      v2 = (float) fabs(p_a->z - p_max->z);
      return (v1 < v2 ? v1 : v2);
    }
  }
  return (FLT_MAX);
}

inline void kd_min_max_bod(BOD * p_a, BOD * p_min, BOD * p_max)
{
  if (p_a->x < p_min->x)
    p_min->x = p_a->x;
  if (p_a->y < p_min->y)
    p_min->y = p_a->y;
  if (p_a->z < p_min->z)
    p_min->z = p_a->z;
  if (p_a->x > p_max->x)
    p_max->x = p_a->x;
  if (p_a->y > p_max->y)
    p_max->y = p_a->y;
  if (p_a->z > p_max->z)
    p_max->z = p_a->z;
}

inline int kd_min_max_valid(BOD * p_min, BOD * p_max)
{
  return (p_min->x < p_max->x && p_min->y < p_max->y && p_min->z < p_max->z);
}

#endif
