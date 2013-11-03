#include <alloca.h>
#include "3d_all.h"

/* OBB -> AABB
*/
void kd_analizuj_scenu(EDIT_MESH_POLY * p_upoly, int polynum,
  EDIT_KONTEJNER ** p_ukont, int kontnum, BOD * p_min, BOD * p_max)
{
  EDIT_MESH_POLY *p_poly;
  EDIT_KONTEJNER *p_kont;
  BOD kon(-FLT_MAX, -FLT_MAX, -FLT_MAX), zac(FLT_MAX, FLT_MAX, FLT_MAX), a;
  GLMATRIX *p_mat;
  int p, f;

  *p_min = zac;
  *p_max = kon;

  for (p = 0; p < polynum; p++) {
    p_poly = p_upoly + p;
    for (f = 0; f < p_poly->facenum; f++) {
      kd_min_max_bod(&p_poly->obb.aabb_min, p_min, p_max);
      kd_min_max_bod(&p_poly->obb.aabb_max, p_min, p_max);
    }
  }

  for (p = 0; p < kontnum; p++) {
    p_kont = p_ukont[p];
    p_mat = kont_world_matrix(p_kont);
    transformuj_bod_matici_bod(&p_kont->obb.aabb_min, p_mat, &a);
    kd_min_max_bod(&a, p_min, p_max);
    transformuj_bod_matici_bod(&p_kont->obb.aabb_max, p_mat, &a);
    kd_min_max_bod(&a, p_min, p_max);
  }
}

/*
  Reprezentace sceny:

  - Prepracovat na KD stromy
  - KD strom + obalky pro dynamicke objekty
*/

void kd_poly_interval(EDIT_MESH_POLY * p_poly, int rovina, float *p_min,
  float *p_max, float h1, float h2)
{
  float a, max = -FLT_MAX, min = FLT_MAX;
  int f;

  switch (rovina) {
    case 0:
      for (f = 0; f < p_poly->facenum; f++) {
        a = p_poly->p_koord[f].x;
        if (a < h1)
          a = h1;
        else if (a > h2)
          a = h2;
        if (a < min)
          min = a;
        if (a > max)
          max = a;
      }
      break;
    case 1:
      for (f = 0; f < p_poly->facenum; f++) {
        a = p_poly->p_koord[f].y;
        if (a < h1)
          a = h1;
        else if (a > h2)
          a = h2;
        if (a < min)
          min = a;
        if (a > max)
          max = a;
      }
      break;
    case 2:
      for (f = 0; f < p_poly->facenum; f++) {
        a = p_poly->p_koord[f].z;
        if (a < h1)
          a = h1;
        else if (a > h2)
          a = h2;
        if (a < min)
          min = a;
        if (a > max)
          max = a;
      }
      break;
  }
  *p_min = min;
  *p_max = max;
}

void kd_kont_interval(EDIT_KONTEJNER * p_kont, int rovina, float *p_min,
  float *p_max, float h1, float h2)
{
  EDIT_OBJEKT *p_obj;
  float a, max = -FLT_MAX, min = FLT_MAX;
  int i, f;

  for (i = 0; i < p_kont->objektu; i++) {
    p_obj = p_kont->p_obj[i];
    if (p_obj) {
      switch (rovina) {
        case 0:
          for (f = 0; f < p_obj->vertexnum; f++) {
            a = p_obj->p_vertex[f].x;
            if (a < h1)
              a = h1;
            else if (a > h2)
              a = h2;
            if (a < min)
              min = a;
            if (a > max)
              max = a;
          }
          break;
        case 1:
          for (f = 0; f < p_obj->vertexnum; f++) {
            a = p_obj->p_vertex[f].y;
            if (a < h1)
              a = h1;
            else if (a > h2)
              a = h2;
            if (a < min)
              min = a;
            if (a > max)
              max = a;
          }
          break;
        case 2:
          for (f = 0; f < p_obj->vertexnum; f++) {
            a = p_obj->p_vertex[f].z;
            if (a < h1)
              a = h1;
            else if (a > h2)
              a = h2;
            if (a < min)
              min = a;
            if (a > max)
              max = a;
          }
          break;
      }
    }
    else
      assert(0);
  }
  *p_min = min;
  *p_max = max;
}

/* Vrati rozmery sceny v min a max
*/

typedef struct _MIN_MAX
{

  float min, max;
  int facu;
  int poly;
  int kont;

} MIN_MAX;

static int kd_comp_hrana(const void *p_hrana1, const void *p_hrana2)
{
  return (ftoi((((MIN_MAX *) p_hrana1)->min -
        ((MIN_MAX *) p_hrana2)->min) * 10000.0f));
}

#define   MIN_X   2.0f
#define   MIN_Y   2.0f
#define   MIN_Z   2.0f
#define   MEZNI_KOEFICIENT       10.0f  // 5 procent mezni odchylka
#define   MIN_OBJEKTU            4

#define   MAX_HLOUBKA            100

#define   DELTA_HRANICE          0.00001f

// Vetsi cislo -> vyssi vaha u toho parametru
// Mensi cislo -> parametr je mene vyznamny
#define   KOEF_FACE              1.0f
#define   KOEF_OBJEKT            3.0f

int kd_hloubka_stromu;

void kd_strom_vyrob_rec(EDIT_MESH_POLY ** p_poly, int polynum,
  EDIT_KONTEJNER ** p_kont, int kontnum, KD_BUNKA * p_prvni, int hloubka)
{
  int objnum = polynum + kontnum;
  MIN_MAX *p_hran = (MIN_MAX *) alloca(sizeof(MIN_MAX) * objnum);
  EDIT_MESH_POLY **p_poly1;
  EDIT_KONTEJNER **p_kont1;
  float dx, dy, dz, h1, h2, koeficient;
  float best_koeficient, lajna;
  float pomer_face;
  float pomer_objektu;
  int best_p, best_strana;
  int p, f, dolni, celkem_facu, rovina;
  int z, z1, z2, objektu_dole, objektu_hore, facu_dole, facu_hore;

  dx = p_prvni->max.x - p_prvni->min.x;
  dy = p_prvni->max.y - p_prvni->min.y;
  dz = p_prvni->max.z - p_prvni->min.z;

  p_prvni->cislo = kd_hloubka_stromu++;

  /* Vloz vsechny objekty do jedne bunky
   */
  if (objnum <= MIN_OBJEKTU || (dx <= MIN_X && dy <= MIN_Y && dz <= MIN_Z)
    || hloubka > MAX_HLOUBKA) {
    assert(objnum);

    p_prvni->polozek = objnum;
    p_prvni->p_polozka =
      (void **) mmalloc(sizeof(p_prvni->p_polozka[0]) * objnum);
    p_prvni->p_polozka_typ =
      (int *) mmalloc(sizeof(p_prvni->p_polozka_typ[0]) * objnum);
    p_prvni->rovina = 0;
    p_prvni->vzdal = 0.0f;
    p_prvni->p_next = NULL;

    for (p = 0; p < polynum; p++) {
      p_prvni->p_polozka[p] = p_poly[p];
      p_prvni->p_polozka_typ[p] = KD_POLY;
    }
    for (f = 0; f < kontnum; f++) {
      p_prvni->p_polozka[p + f] = p_kont[f];
      p_prvni->p_polozka_typ[p + f] = KD_MESH;
    }
  }
  else {
    // podle rozmeru bunky spocitam delici stranu
    // bude to ta nejdelsi

    if (dx > dy) {
      rovina = (dx > dz) ? 0 : 2;
      h1 = (dx > dz) ? p_prvni->min.x : p_prvni->min.z;
      h2 = (dx > dz) ? p_prvni->max.x : p_prvni->max.z;
    }
    else {
      rovina = (dy > dz) ? 1 : 2;
      h1 = (dy > dz) ? p_prvni->min.y : p_prvni->min.z;
      h2 = (dy > dz) ? p_prvni->max.y : p_prvni->max.z;
    }

    // podle souradnic rozdelim objekty do jedne roviny
    // Znam rovinu -> nasypu to do intervalu
    celkem_facu = 0;
    for (p = 0; p < polynum; p++) {
      kd_poly_interval(p_poly[p], rovina, &p_hran[p].min, &p_hran[p].max, h1,
        h2);
      p_hran[p].poly = p;
      p_hran[p].kont = K_CHYBA;
      celkem_facu += (p_hran[p].facu = p_poly[p]->facenum);
    }
    for (f = 0; f < kontnum; f++) {
      kd_kont_interval(p_kont[f], rovina, &p_hran[p + f].min,
        &p_hran[p + f].max, h1, h2);
      p_hran[p + f].poly = K_CHYBA;
      p_hran[p + f].kont = f;
      celkem_facu += (p_hran[p + f].facu = p_kont[f]->facu);
    }

    // setridim to
    qsort(p_hran, objnum, sizeof(p_hran[0]), kd_comp_hrana);

    // Spocitam pocty facu
    best_p = K_CHYBA;
    best_koeficient = 100.0f;

    for (p = 0; p < objnum - 1; p++) {
      if (p_hran[p + 1].min - p_hran[p].max > 0.0f) {
        for (f = 0, dolni = 0; f <= p; f++)
          dolni += p_hran[f].facu;
        // pouziju procentni koeficienty
        koeficient = ((float) dolni / (float) celkem_facu) * 100.0f;
        if (best_koeficient > fabs(50.0f - koeficient)) {
          best_koeficient = fabsf(50.0f - koeficient);
          best_p = p;
          best_strana = 3;
        }
      }
    }


    // Rozdelim to uprosted objektu
    if (best_p == K_CHYBA || best_koeficient > MEZNI_KOEFICIENT) {
      best_p = K_CHYBA;
      best_koeficient = 200.0f;
      for (p = 0; p < objnum; p++) {
        lajna = p_hran[p].min;
        facu_dole = objektu_dole = 0;
        facu_hore = objektu_hore = 0;
        for (z = 0; z < objnum; z++) {
          if (p_hran[z].max < lajna) {  // cisty horni objekt
            objektu_dole++;
            facu_dole += p_hran[z].facu;        // kolik je dole facu
          }
          if (p_hran[z].min > lajna) {  // cisty dolni objekt
            objektu_hore++;
            facu_hore += p_hran[z].facu;        // kolik je nahore facu
          }
        }

        if (facu_dole && objektu_dole && facu_hore && objektu_hore) {

          /*


             Zohlednit v koeficientu take pomer uspesnych objektu
             vzledem ke vsem objektum


           */


          // procentualni odchylka poctu facu        
          pomer_face = ((float) abs(facu_dole - facu_hore) / (float) celkem_facu) * 100.0f;     // pomer rozdeleni
          pomer_objektu = ((float) abs(objektu_dole - objektu_hore) / (float) objnum) * 100.0f; // pomer uspesnosti

          // Jednotlive odchylky chceme mit co nejmensi
          koeficient = KOEF_OBJEKT * pomer_objektu + KOEF_FACE * pomer_face;
          if (best_koeficient > koeficient) {
            best_koeficient = koeficient;
            best_p = p;
            best_strana = 0;
          }
        }

        lajna = p_hran[p].max;

        facu_dole = objektu_dole = 0;
        facu_hore = objektu_hore = 0;
        for (z = 0; z < objnum; z++) {
          if (p_hran[z].max < lajna) {  // cisty horni objekt
            objektu_dole++;
            facu_dole += p_hran[z].facu;        // kolik je dole facu
          }
          if (p_hran[z].min > lajna) {  // cisty dolni objekt
            objektu_hore++;
            facu_hore += p_hran[z].facu;        // kolik je nahore facu
          }
        }

        if (facu_dole && objektu_dole && facu_hore && objektu_hore) {

          // procentualni odchylka poctu facu
          pomer_face = ((float) abs(facu_dole - facu_hore) / (float) celkem_facu) * 100.0f;     // pomer rozdeleni
          pomer_objektu = ((float) abs(objektu_dole - objektu_hore) / (float) objnum) * 100.0f; // pomer uspesnosti

          // Jednotlive odchylky chceme mit co nejmensi        
          koeficient = KOEF_OBJEKT * pomer_objektu + KOEF_FACE * pomer_face;
          if (best_koeficient > koeficient) {
            best_koeficient = koeficient;
            best_p = p;
            best_strana = 1;
          }
        }
      }
    }

    if (best_p == K_CHYBA) {
      // nepodarilo se najit vhodne rozdeleni - strcim to do jedne a hotovo

      p_prvni->polozek = objnum;
      p_prvni->p_polozka =
        (void **) mmalloc(sizeof(p_prvni->p_polozka[0]) * objnum);
      p_prvni->p_polozka_typ =
        (int *) mmalloc(sizeof(p_prvni->p_polozka_typ[0]) * objnum);
      p_prvni->rovina = 0;
      p_prvni->vzdal = 0.0f;
      p_prvni->p_next = NULL;

      for (p = 0; p < polynum; p++) {
        p_prvni->p_polozka[p] = p_poly[p];
        p_prvni->p_polozka_typ[p] = KD_POLY;
      }
      for (f = 0; f < kontnum; f++) {
        p_prvni->p_polozka[p + f] = p_kont[f];
        p_prvni->p_polozka_typ[p + f] = KD_MESH;
      }

      kprintf(1, "Shit-miss, depth %d num %d(%d/%d)", hloubka, objnum,
        polynum, kontnum);
    }
    else {
      /*
         Mam to rozdeleny
         0 .. best_p -> spodni vetev
         best_p+1 .. polynum -> vrchni vetev
       */

      p_prvni->polozek = 0;
      p_prvni->p_polozka = NULL;
      p_prvni->rovina = rovina;
      if (best_strana == 3) {
        p_prvni->vzdal =
          p_hran[best_p].max + fabsf(p_hran[best_p + 1].min -
          p_hran[best_p].max) * 0.5f;
      }
      else {
        p_prvni->vzdal =
          (best_strana) ? p_hran[best_p].max : p_hran[best_p].min;
      }
      p_prvni->p_next = (KD_BUNKA *) mmalloc(sizeof(p_prvni->p_next[0]) * 2);

      p_prvni->p_next[0].p_up = p_prvni;
      p_prvni->p_next[1].p_up = p_prvni;

      p_prvni->p_next[0].min = p_prvni->p_next[1].min = p_prvni->min;
      p_prvni->p_next[0].max = p_prvni->p_next[1].max = p_prvni->max;

      switch (rovina) {
        case 0:                // osa x
          p_prvni->p_next[0].max.x = p_prvni->vzdal;
          p_prvni->p_next[1].min.x = p_prvni->vzdal;
          break;
        case 1:                // osa y
          p_prvni->p_next[0].max.y = p_prvni->vzdal;
          p_prvni->p_next[1].min.y = p_prvni->vzdal;
          break;
        case 2:                // osa z
          p_prvni->p_next[0].max.z = p_prvni->vzdal;
          p_prvni->p_next[1].min.z = p_prvni->vzdal;
          break;
        default:
          break;
      }

      // Poly-list 1
      p_poly1 = (EDIT_MESH_POLY **) mmalloc(sizeof(p_poly1[0]) * polynum);
      p_kont1 = (EDIT_KONTEJNER **) mmalloc(sizeof(p_kont1[0]) * kontnum);

      for (z1 = 0, z2 = 0, p = 0; p < objnum; p++) {
        if (p_hran[p].min < p_prvni->vzdal) {
          if (p_hran[p].poly != K_CHYBA) {
            p_poly1[z1++] = p_poly[p_hran[p].poly];
          }
          else {
            p_kont1[z2++] = p_kont[p_hran[p].kont];
          }
        }
      }
      kd_strom_vyrob_rec(p_poly1, z1, p_kont1, z2, p_prvni->p_next,
        hloubka + 1);

      // Poly-list 2
      for (z1 = 0, z2 = 0, p = 0; p < objnum; p++) {
        if (p_hran[p].max > p_prvni->vzdal) {
          if (p_hran[p].poly != K_CHYBA) {
            p_poly1[z1++] = p_poly[p_hran[p].poly];
          }
          else {
            p_kont1[z2++] = p_kont[p_hran[p].kont];
          }
        }
      }
      kd_strom_vyrob_rec(p_poly1, z1, p_kont1, z2, p_prvni->p_next + 1,
        hloubka + 1);

      free(p_poly1);
      free(p_kont1);
    }
  }
}

// Vyrobi kvadraticky strom -> prvni bunka je obalka cele sceny
void kd_strom_vyrob(EDIT_MESH_POLY * p_upoly, int polynum,
  EDIT_KONTEJNER ** p_kont, int kontnum, KD_BUNKA * p_prvni)
{
  EDIT_MESH_POLY **p_poly =
    (EDIT_MESH_POLY **) alloca(sizeof(p_poly[0]) * polynum);
  BOD min, max;
  int i, num;

  kd_hloubka_stromu = 0;

  kd_analizuj_scenu(p_upoly, polynum, p_kont, kontnum, &min, &max);
  p_prvni->min = min;
  p_prvni->max = max;

  num = 0;
  for (i = 0; i < polynum; i++) {
    if (!(p_upoly[i].kflag & KONT_NO_DRAW_SHADOW)) {
      p_poly[num++] = p_upoly + i;
    }
  }

  if (num || kontnum)
    kd_strom_vyrob_rec(p_poly, num, p_kont, kontnum, p_prvni, 1);
  else
    memset(p_prvni, 0, sizeof(p_prvni[0]));

  p_prvni->cislo = kd_hloubka_stromu++;
}


/*
typedef struct _KD_BUNKA {

  int                polozek;
  void             **p_polozka; //pole polozek (poly/kont)
  int               *p_polozka_typ; // typy polozek (kontejner/poly)
  struct _KD_BUNKA  *p_up;      // otec stromu
  struct _KD_BUNKA  *p_next;    // dalsich 2 podstromu nebo nic

} KD_BUNKA;
*/

void kd_strom_zrus(KD_BUNKA * p_prvni)
{
  //KD_BUNKA *p_next = p_prvni->p_next;
  //free(p_prvni);
}

void kd_strom_tiskni(KD_BUNKA * p_prvni, int hloubka)
{
  char pom[300];
  int i;

  for (i = 0; i < hloubka; i++)
    pom[i] = ' ';
  pom[i] = 0;

  if (!p_prvni->p_next) {
    kprintf(1, "%sH %d P %d", pom, hloubka, p_prvni->polozek);
  }
  else {
    kprintf(1, "%sH", pom);
    kd_strom_tiskni(p_prvni->p_next, hloubka + 1);
    kd_strom_tiskni(p_prvni->p_next + 1, hloubka + 1);
  }
}

int kd_intersect_kostku(BOD * p_a, BOD * p_b, BOD * p_min, BOD * p_max)
{
  // primka a->b  
  float tn = 0.0f, tf = FLT_MAX;
  float t1, t2, t;
  BOD Q;
  int hits;

  hits =
    kd_je_bod_v_kostce(p_a, p_min, p_max) + kd_je_bod_v_kostce(p_b, p_min,
    p_max);
  if (hits) {
    return (hits);
  }

  // p_a->p_b --- blizkost k a
  vektor_sub(p_b, p_a, &Q);

  // X-Souradnice
  t1 = p_max->x - p_a->x;       // defaultne Tf
  t2 = p_min->x - p_a->x;       // defaultne Tn
  if (Q.x < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  if (Q.x != 0) {
    tn = MAX(t2 / Q.x, tn);
    tf = MIN(t1 / Q.x, tf);
  }
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  // Y-Souradnice
  t1 = p_max->y - p_a->y;       // defaultne Tf
  t2 = p_min->y - p_a->y;       // defaultne Tn
  if (Q.y < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  if (Q.y != 0) {
    tn = MAX(t2 / Q.y, tn);
    tf = MIN(t1 / Q.y, tf);
  }
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  // Z-Souradnice
  t1 = p_max->z - p_a->z;       // defaultne Tf
  t2 = p_min->z - p_a->z;       // defaultne Tn
  if (Q.z < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  if (Q.z != 0) {
    tn = MAX(t2 / Q.z, tn);
    tf = MIN(t1 / Q.z, tf);
  }
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  return (1);
}

int kd_intersect_kostku_bod(BOD * p_a, BOD * p_b, BOD * p_min, BOD * p_max,
                            BOD * p_p, float *p_t)
{
  // primka a->b  
  float tn = 0.0f, tf = FLT_MAX;
  float t1, t2, t;
  BOD Q;

  if (kd_je_bod_v_kostce(p_a, p_min, p_max)) {
    *p_p = *p_a;
    *p_t = 0.0f;
    return (kd_je_bod_v_kostce(p_b, p_min, p_max) ? 2 : 1);
  }

  // p_a->p_b --- blizkost k a
  vektor_sub(p_b, p_a, &Q);

  // X-Souradnice
  t1 = p_max->x - p_a->x;       // defaultne Tf
  t2 = p_min->x - p_a->x;       // defaultne Tn
  if (Q.x < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  if (Q.x != 0) {
    tn = MAX(t2 / Q.x, tn);
    tf = MIN(t1 / Q.x, tf);
  }
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  // Y-Souradnice
  t1 = p_max->y - p_a->y;       // defaultne Tf
  t2 = p_min->y - p_a->y;       // defaultne Tn
  if (Q.y < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  if (Q.y != 0) {
    tn = MAX(t2 / Q.y, tn);
    tf = MIN(t1 / Q.y, tf);
  }
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  // Z-Souradnice
  t1 = p_max->z - p_a->z;       // defaultne Tf
  t2 = p_min->z - p_a->z;       // defaultne Tn
  if (Q.z < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  if (Q.z != 0) {
    tn = MAX(t2 / Q.z, tn);
    tf = MIN(t1 / Q.z, tf);
  }
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  *p_t = tn;
  p_p->x = p_a->x + Q.x * tn;
  p_p->y = p_a->y + Q.y * tn;
  p_p->z = p_a->z + Q.z * tn;

  return (3);
}

int kd_intersect_kostku_bod_inter(BOD * p_a, BOD * p_b, BOD * p_min,
  BOD * p_max, BOD * p_p, float *p_t)
{
  // primka a->b  
  float tn = 0.0f, tf = FLT_MAX;
  float t1, t2, t;
  BOD Q;

  // p_a->p_b --- blizkost k a
  vektor_sub(p_b, p_a, &Q);

  // X-Souradnice
  t1 = p_max->x - p_a->x;       // defaultne Tf
  t2 = p_min->x - p_a->x;       // defaultne Tn
  if (Q.x < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2 / Q.x, tn);
  tf = MIN(t1 / Q.x, tf);
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  // Y-Souradnice
  t1 = p_max->y - p_a->y;       // defaultne Tf
  t2 = p_min->y - p_a->y;       // defaultne Tn
  if (Q.y < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2 / Q.y, tn);
  tf = MIN(t1 / Q.y, tf);
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  // Z-Souradnice
  t1 = p_max->z - p_a->z;       // defaultne Tf
  t2 = p_min->z - p_a->z;       // defaultne Tn
  if (Q.z < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2 / Q.z, tn);
  tf = MIN(t1 / Q.z, tf);
  if (tn > tf || tf < 0.0f)
    return (FALSE);

  *p_t = tf;
  p_p->x = p_a->x + Q.x * tf;
  p_p->y = p_a->y + Q.y * tf;
  p_p->z = p_a->z + Q.z * tf;
  return (TRUE);
}

int kd_intersect_kostku_dir(BOD * p_orig, BOD * p_dir, BOD * p_min,
  BOD * p_max)
{
  BOD p;

  vektor_add(p_orig, p_dir, &p);
  return (kd_intersect_kostku(p_orig, &p, p_min, p_max));
}

/*
float kd_vzdal_bod_kostka(BOD *p_a, BOD *p_min, BOD *p_max)
{
  BOD   stred,v1,v2;
  BOD   delka;
  float len;

  kd_stred_bunky(p_min,p_max,&stred);
  vektor_sub(p_max,&stred,&delka);

  vektor_sub(p_a,&stred,&v1);
  v2 = v1;
  len = vektor_norm_mult_vektor(&v2,&delka);
  
  if(fabsf(v1.x) <= fabsf(v2.x) && 
     fabsf(v1.y) <= fabsf(v2.y) && 
     fabsf(v1.z) <= fabsf(v2.z)) {
    return(0.0f); // je uvnitr
  } else {
    return(vektor_velikost(vektor_sub(&v1,&v2,&v1)));
  }
}
*/

/*
  Udela strom z poly
*/
int kd_visibility(BOD * p_min, BOD * p_max, GLMATRIX * p_m)
{
  int i, j, xplane, yplane, xa, ya, m;
  float x, y, z, xt, yt, zt, w, fx, fy, fw;

  if (!kd_min_max_valid(p_min, p_max))
    return (FALSE);

  // ... // -> vyrobeni 8 hranicnich bodu krychle  
  xplane = 0;                   // -2, -1, 1, 2
  yplane = 0;                   // -2, -1, 1, 2

  for (i = 0, j = 0; i < 8; i++) {
    xt = (i & 0x1) ? p_max->x : p_min->x;
    yt = (i & 0x2) ? p_max->y : p_min->y;
    zt = (i & 0x4) ? p_max->z : p_min->z;

    x = p_m->_11 * xt + p_m->_21 * yt + p_m->_31 * zt + p_m->_41;
    y = p_m->_12 * xt + p_m->_22 * yt + p_m->_32 * zt + p_m->_42;
    z = p_m->_13 * xt + p_m->_23 * yt + p_m->_33 * zt + p_m->_43;
    w = p_m->_14 * xt + p_m->_24 * yt + p_m->_34 * zt + p_m->_44;

    // bod je pred kamerou
    m = (-w <= z && z <= w);
    if (m)
      j++;

    // bod lezi v obrazovce -> automaticky uspech
    if (m && -w <= x && x <= w && -w <= y && y <= w) {
      return (TRUE);
    }

    fx = (float) fabs(x);
    fy = (float) fabs(y);
    fw = (float) fabs(w);

    if (fx / fw > 1.0f) {
      xa = (x < 0.0f) ? -2 : 2;
    }
    else {
      xa = (x < 0.0f) ? -1 : 1;
    }

    if (fy / fw > 1.0f) {
      ya = (y < 0.0f) ? -2 : 2;
    }
    else {
      ya = (y < 0.0f) ? -1 : 1;
    }

    xplane += xa;
    yplane += ya;
  }
// if(!j)
  // dbgprintf("j = 0");
  return ((j > 0) ? ((abs(xplane) != 16) && (abs(yplane) != 16)) : FALSE);
}

int kd_visibility_full(BOD * p_min, BOD * p_max, GLMATRIX * p_m)
{
  int i, j, xplane, yplane, xa, ya, m;
  int hits = 0;
  float x, y, z, xt, yt, zt, w, fx, fy, fw;

  if (!kd_min_max_valid(p_min, p_max))
    return (FALSE);

  // ... // -> vyrobeni 8 hranicnich bodu krychle  
  xplane = 0;                   // -2, -1, 1, 2
  yplane = 0;                   // -2, -1, 1, 2

  for (i = 0, j = 0; i < 8; i++) {
    xt = (i & 0x1) ? p_max->x : p_min->x;
    yt = (i & 0x2) ? p_max->y : p_min->y;
    zt = (i & 0x4) ? p_max->z : p_min->z;

    x = p_m->_11 * xt + p_m->_21 * yt + p_m->_31 * zt + p_m->_41;
    y = p_m->_12 * xt + p_m->_22 * yt + p_m->_32 * zt + p_m->_42;
    z = p_m->_13 * xt + p_m->_23 * yt + p_m->_33 * zt + p_m->_43;
    w = p_m->_14 * xt + p_m->_24 * yt + p_m->_34 * zt + p_m->_44;

    // bod je pred kamerou
    m = (-w <= z && z <= w);
    if (m)
      j++;

    // bod lezi v obrazovce -> automaticky uspech
    if (m && -w <= x && x <= w && -w <= y && y <= w) {
      hits++;
      continue;
    }

    fx = (float) fabs(x);
    fy = (float) fabs(y);
    fw = (float) fabs(w);

    if (fx / fw > 1.0f) {
      xa = (x < 0.0f) ? -2 : 2;
    }
    else {
      xa = (x < 0.0f) ? -1 : 1;
    }

    if (fy / fw > 1.0f) {
      ya = (y < 0.0f) ? -2 : 2;
    }
    else {
      ya = (y < 0.0f) ? -1 : 1;
    }

    xplane += xa;
    yplane += ya;
  }
// if(!j)
  // dbgprintf("j = 0");
  return ((hits == 8) ? hits : (j > 0) ? ((abs(xplane) != 16)
      && (abs(yplane) != 16)) : FALSE);
}

void kresli_obalku(BOD min, BOD max, dword barva, GLMATRIX * p_tr)
{
  BOD stred, body[8 * 6];
  float dx, dy, dz;
  int i;

  vektor_sub(&max, &min, &stred);
  vektor_mult_skalar(&stred, 0.05f, &stred);
  vektor_sub(&min, &stred, &min);
  vektor_add(&max, &stred, &max);

  dx = max.x - min.x, dy = max.y - min.y, dz = max.z - min.z;
  dx *= 0.3f;
  dy *= 0.3f;
  dz *= 0.3f;

  vektor_set_all(body + (0 * 6), min.x, min.y, min.z);
  vektor_set_all(body + (0 * 6 + 1), min.x + dx, min.y, min.z);
  vektor_set_all(body + (0 * 6 + 2), min.x, min.y, min.z);
  vektor_set_all(body + (0 * 6 + 3), min.x, min.y + dy, min.z);
  vektor_set_all(body + (0 * 6 + 4), min.x, min.y, min.z);
  vektor_set_all(body + (0 * 6 + 5), min.x, min.y, min.z + dz);

  vektor_set_all(body + (1 * 6), max.x, min.y, min.z);
  vektor_set_all(body + (1 * 6 + 1), max.x - dx, min.y, min.z);
  vektor_set_all(body + (1 * 6 + 2), max.x, min.y, min.z);
  vektor_set_all(body + (1 * 6 + 3), max.x, min.y + dy, min.z);
  vektor_set_all(body + (1 * 6 + 4), max.x, min.y, min.z);
  vektor_set_all(body + (1 * 6 + 5), max.x, min.y, min.z + dz);

  vektor_set_all(body + (2 * 6), min.x, min.y, max.z);
  vektor_set_all(body + (2 * 6 + 1), min.x + dx, min.y, max.z);
  vektor_set_all(body + (2 * 6 + 2), min.x, min.y, max.z);
  vektor_set_all(body + (2 * 6 + 3), min.x, min.y + dy, max.z);
  vektor_set_all(body + (2 * 6 + 4), min.x, min.y, max.z);
  vektor_set_all(body + (2 * 6 + 5), min.x, min.y, max.z - dz);

  vektor_set_all(body + (3 * 6), max.x, min.y, max.z);
  vektor_set_all(body + (3 * 6 + 1), max.x - dx, min.y, max.z);
  vektor_set_all(body + (3 * 6 + 2), max.x, min.y, max.z);
  vektor_set_all(body + (3 * 6 + 3), max.x, min.y + dy, max.z);
  vektor_set_all(body + (3 * 6 + 4), max.x, min.y, max.z);
  vektor_set_all(body + (3 * 6 + 5), max.x, min.y, max.z - dz);

  vektor_set_all(body + (4 * 6), min.x, max.y, min.z);
  vektor_set_all(body + (4 * 6 + 1), min.x + dx, max.y, min.z);
  vektor_set_all(body + (4 * 6 + 2), min.x, max.y, min.z);
  vektor_set_all(body + (4 * 6 + 3), min.x, max.y - dy, min.z);
  vektor_set_all(body + (4 * 6 + 4), min.x, max.y, min.z);
  vektor_set_all(body + (4 * 6 + 5), min.x, max.y, min.z + dz);

  vektor_set_all(body + (5 * 6), max.x, max.y, min.z);
  vektor_set_all(body + (5 * 6 + 1), max.x - dx, max.y, min.z);
  vektor_set_all(body + (5 * 6 + 2), max.x, max.y, min.z);
  vektor_set_all(body + (5 * 6 + 3), max.x, max.y - dy, min.z);
  vektor_set_all(body + (5 * 6 + 4), max.x, max.y, min.z);
  vektor_set_all(body + (5 * 6 + 5), max.x, max.y, min.z + dz);

  vektor_set_all(body + (6 * 6), min.x, max.y, max.z);
  vektor_set_all(body + (6 * 6 + 1), min.x + dx, max.y, max.z);
  vektor_set_all(body + (6 * 6 + 2), min.x, max.y, max.z);
  vektor_set_all(body + (6 * 6 + 3), min.x, max.y - dy, max.z);
  vektor_set_all(body + (6 * 6 + 4), min.x, max.y, max.z);
  vektor_set_all(body + (6 * 6 + 5), min.x, max.y, max.z - dz);

  vektor_set_all(body + (7 * 6), max.x, max.y, max.z);
  vektor_set_all(body + (7 * 6 + 1), max.x - dx, max.y, max.z);
  vektor_set_all(body + (7 * 6 + 2), max.x, max.y, max.z);
  vektor_set_all(body + (7 * 6 + 3), max.x, max.y - dy, max.z);
  vektor_set_all(body + (7 * 6 + 4), max.x, max.y, max.z);
  vektor_set_all(body + (7 * 6 + 5), max.x, max.y, max.z - dz);

  glColor4ubv((byte *) & barva);
  if (p_tr) {
    glBegin(GL_LINES);
    for (i = 0; i < 8 * 6; i++)
      glVertex3fv((const float *) transformuj_bod_bod_matici(body + i, p_tr));
    glEnd();
  }
  else {
    glBegin(GL_LINES);
    for (i = 0; i < 8 * 6; i++)
      glVertex3fv((const float *) (body + i));
    glEnd();
  }
}
