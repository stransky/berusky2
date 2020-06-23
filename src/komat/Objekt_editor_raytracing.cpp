/*
  Ray-tracing sceny
*/

#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include "objekt_editor_all.h"

//#include "dbgwnd.h"

__inline void ray_svetlost_vertex_poly_diff(TEXT_KOORD * p_koord);
__inline void ray_svetlost_vertex_poly(K_EDITOR * p_cnf, TEXT_KOORD * p_koord,
  float lesk);
__inline dword ray_interpoluj_2_barvy(dword b1, dword b2, float vaha2);
__inline dword ray_svetlost_bodu(K_EDITOR * p_cnf, BOD * p_p,
  LIGHTMAP_INFO * p_akt, byte stiny, dword ambient, int no_phong);


#define BLBA_BARVA    0xff0000
#define BLBA_BARVA2   0xff0000

/*
#define OKRAJ         4
#define MIN_X_TEXTURA 16
#define MIN_Y_TEXTURA 16
*/

#define OKRAJ         (p_cnf->ray.okraj)
#define MIN_X_TEXTURA (p_cnf->ray.text_min_x)
#define MIN_Y_TEXTURA (p_cnf->ray.text_min_y)

/*
  Brutalni 9-ti pixelovej filtr - radkovej
*/
void ray_surface_filtruj_ctverec_9(bitmapa * p_bmp)
{
  bitmapa *p_tmp;
  dword barva, r = 0, g = 0, b = 0;
  int cisel, i;
  int tabulka[] = { 1, 0, 1, 1, 0, 1, -1, 1, -1, 0, -1, -1, 0, -1, 1, -1 };
  int x, y, dx, dy;
  int px, py;

  p_tmp = bmp_kopituj(p_bmp);

  dx = p_bmp->x;
  dy = p_bmp->y;

  for (x = 0; x < dx; x++) {
    for (y = 0; y < dy; y++) {
      barva = bmp_getpixel(p_tmp, x, y);
      r = GetRValue(barva);
      g = GetGValue(barva);
      b = GetBValue(barva);
      cisel = 1;
      for (i = 0; i < 16; i += 2) {
        px = x + tabulka[i];
        py = y + tabulka[i + 1];
        if (px >= 0 && px < dx && py >= 0 && py < dy) {
          barva = bmp_getpixel(p_tmp, px, py);
          if (barva != BLBA_BARVA) {
            r += GetRValue(barva);
            g += GetGValue(barva);
            b += GetBValue(barva);
            cisel++;
          }
        }
      }
      mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) cisel);
      bmp_putpixel(p_bmp, x, y, barva);
    }
  }

  bmp_zrus(&p_tmp);
}

void ray_surface_filtruj_ctverec_25(bitmapa * p_bmp)
{
  bitmapa *p_tmp;
  dword barva, r = 0, g = 0, b = 0;
  int cisel, i;

  int tabulka[] = { 0, 0,
    1, 0, 1, 1, 0, 1, -1, 1,
    -1, 0, -1, -1, 0, -1, 1, -1,
    -2, -2, -1, -2, 0, -2, 1, -2, 2, -2,        // spodek
    -2, 2, -1, 2, 0, 2, 1, 2, 2, 2,     // vrsek
    -2, 1, -2, 0, -2, -1,
    2, 1, 2, 0, 2, -1
  };
  int x, y, dx, dy;
  int px, py;

  p_tmp = bmp_kopituj(p_bmp);

  dx = p_bmp->x;
  dy = p_bmp->y;

  for (x = 0; x < dx; x++) {
    for (y = 0; y < dy; y++) {
      r = g = b = 0;
      cisel = 0;
      for (i = 0; i < 25; i += 2) {
        px = x + tabulka[i];
        py = y + tabulka[i + 1];
        if (px >= 0 && px < dx && py >= 0 && py < dy) {
          barva = bmp_getpixel(p_tmp, px, py);
          r += GetRValue(barva);
          g += GetGValue(barva);
          b += GetBValue(barva);
          cisel++;
        }
      }
      mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) cisel);
      bmp_putpixel(p_bmp, x, y, barva);
    }
  }

  bmp_zrus(&p_tmp);
}

void ray_surface_filtruj_krizem_9(bitmapa * p_bmp)
{
  bitmapa *p_tmp;
  dword barva, r = 0, g = 0, b = 0;
  int cisel, i;
  int tabulka[] = { 0, 0, 0, 1, 0, -1, 1, 0, -1, 0 };
  int x, y, dx, dy;
  int px, py;

  p_tmp = bmp_kopituj(p_bmp);

  dx = p_bmp->x;
  dy = p_bmp->y;

  for (x = 0; x < dx; x++) {
    for (y = 0; y < dy; y++) {
      r = g = b = 0;
      cisel = 0;
      for (i = 0; i < 10; i += 2) {
        px = x + tabulka[i];
        py = y + tabulka[i + 1];
        if (px >= 0 && px < dx && py >= 0 && py < dy) {
          barva = bmp_getpixel(p_tmp, px, py);
          r += GetRValue(barva);
          g += GetGValue(barva);
          b += GetBValue(barva);
          cisel++;
        }
      }
      mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) cisel);
      bmp_putpixel(p_bmp, x, y, barva);
    }
  }

  bmp_zrus(&p_tmp);
}

void ray_surface_filtruj_krizem_25(bitmapa * p_bmp)
{
  bitmapa *p_tmp;
  dword barva, r = 0, g = 0, b = 0;
  int cisel, i;

  int tabulka[] = { 0, 0,
    0, 1, 0, -1, 1, 0, -1, 0,
    0, 2, 0, -2, 2, 0, -2, 0
  };
  int x, y, dx, dy;
  int px, py;

  p_tmp = bmp_kopituj(p_bmp);

  dx = p_bmp->x;
  dy = p_bmp->y;

  for (x = 0; x < dx; x++) {
    for (y = 0; y < dy; y++) {
      r = g = b = 0;
      cisel = 0;
      for (i = 0; i < 18; i += 2) {
        px = x + tabulka[i];
        py = y + tabulka[i + 1];
        if (px >= 0 && px < dx && py >= 0 && py < dy) {
          barva = bmp_getpixel(p_tmp, px, py);
          r += GetRValue(barva);
          g += GetGValue(barva);
          b += GetBValue(barva);
          cisel++;
        }
      }
      mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) cisel);
      bmp_putpixel(p_bmp, x, y, barva);
    }
  }

  bmp_zrus(&p_tmp);
}

void ray_surface_filtruj_rovne_25(bitmapa * p_bmp)
{
  bitmapa *p_tmp;
  dword barva, r = 0, g = 0, b = 0;
  int cisel, i;

  int tabulka[] = { 0, 0,
    1, 0, -1, 0,
    2, 0, -2, 0
  };
  int x, y, dx, dy;
  int px, py;

  p_tmp = bmp_kopituj(p_bmp);

  dx = p_bmp->x;
  dy = p_bmp->y;

  for (x = 0; x < dx; x++) {
    for (y = 0; y < dy; y++) {
      r = g = b = 0;
      cisel = 0;
      for (i = 0; i < 10; i += 2) {
        px = x + tabulka[i];
        py = y + tabulka[i + 1];
        if (px >= 0 && px < dx && py >= 0 && py < dy) {
          barva = bmp_getpixel(p_tmp, px, py);
          r += GetRValue(barva);
          g += GetGValue(barva);
          b += GetBValue(barva);
          cisel++;
        }
      }
      mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) cisel);
      bmp_putpixel(p_bmp, x, y, barva);
    }
  }

  bmp_zrus(&p_tmp);
}

static int ray_surface_filtruj_medianem_compare(const void *p_b1,
  const void *p_b2)
{
  float b1, b2;

  b1 = ((RGB *) p_b1)->r + ((RGB *) p_b1)->g + ((RGB *) p_b1)->b;
  b2 = ((RGB *) p_b2)->r + ((RGB *) p_b2)->g + ((RGB *) p_b2)->b;
  return ((int) ((b2 - b1) * 0.0001f));
}

void ray_surface_filtruj_medianem(bitmapa * p_bmp)
{
  bitmapa *p_tmp;
  dword barva;
  float b[3];
  int i;

  int tabulka[] = { 1, 0, 1, 1, 0, 1, -1, 1, -1, 0, -1, -1, 0, -1, 1, -1,
    2, 0, 2, 1, 2, 2, 1, 2, 0, 2, -1, 2, -2, 2, -2, 1, -2, 0, -2, -1, -2, -2,
    -1, -2, 0, -2, 1, -2, 2, -2, -2, 1
  };
  RGB ct[25];
  int p;
  int x, y, dx, dy;
  int px, py;

  p_tmp = bmp_kopituj(p_bmp);

  dx = p_bmp->x;
  dy = p_bmp->y;

  for (x = 0; x < dx; x++) {
    for (y = 0; y < dy; y++) {
      barva = bmp_getpixel(p_tmp, x, y);
      p = 0;
      if (barva != BLBA_BARVA) {
        rgb_float(barva, &ct[p].r);
        p++;
        for (i = 0; i < 50; i += 2) {
          px = x + tabulka[i];
          py = y + tabulka[i + 1];
          if (px > 0 && px < dx && py > 0 && py < dy) {
            barva = bmp_getpixel(p_tmp, px, py);
            if (barva != BLBA_BARVA) {
              rgb_float(barva, &ct[p].r);
              p++;
            }
          }
        }
        //spocitam prumer
        b[0] = b[1] = b[3] = 0.0f;

        for (i = 0; i < p; i++) {
          b[0] += ct[i].r;
          b[1] += ct[i].g;
          b[2] += ct[i].b;
        }

        b[0] /= p;
        b[1] /= p;
        b[2] /= p;

        bmp_putpixel(p_bmp, x, y, float_rgb(b));
      }
    }
  }

  bmp_zrus(&p_tmp);
}


/*
  Prevedu pole lightmap na jednotliva podpole s poctama lightmap
*/
void ray_poly_optim_lightmap(EDIT_MESH_POLY * p_poly)
{
  EDIT_TEXT **p_klight =
    (EDIT_TEXT **) _alloca(p_poly->lightnum * sizeof(EDIT_TEXT *));
  int *p_pocty = (int *) _alloca(p_poly->lightnum * sizeof(int *));
  EDIT_TEXT *p_last = NULL;
  int i, p;

  if (p_poly->p_lightnum) {
    assert(0);
  }

  for (i = 0, p = -1; i < p_poly->lightnum; i++) {
    if (p_last != p_poly->p_light[i]) {
      p++;
      p_last = p_klight[p] = p_poly->p_light[i];
      p_pocty[p] = 3;
    }
    else {
      p_pocty[p] += 3;          //pocty facu
    }
  }
  // pocet lightmap je p+1
  p_poly->lightnum = ++p;

  free(p_poly->p_light);
  if (!(p_poly->p_light = (EDIT_TEXT **) malloc(sizeof(EDIT_TEXT *) * p)))
    chyba("Pamet");

  if (!(p_poly->p_lightnum = (int *) malloc(sizeof(int) * p)))
    chyba("Pamet");

  memcpy(p_poly->p_light, p_klight, sizeof(EDIT_TEXT *) * p);
  memcpy(p_poly->p_lightnum, p_pocty, sizeof(int) * p);
}

/* return  
  
  0 - vrsek
  1 - bok
  2 - druhy bok
  3 - fuck

  -- plati pro bod s barvou 0xff000000
*/
int ray_surface_pozice_bodu(bitmapa * p_bmp, int px, int py)
{
  int dx = p_bmp->x, dy = p_bmp->y;

  if (py + 1 >= dy)
    return (3);
  if (py - 1 <= 0)
    return (0);

  if (px + 1 >= dx)
    return (2);
  if (px - 1 <= 0)
    return (1);

  if (bmp_getpixel(p_bmp, px, py + 1) != BLBA_BARVA)
    return (0);
  else {
    if (bmp_getpixel(p_bmp, px, py - 1) == BLBA_BARVA)
      return (3);
    if (bmp_getpixel(p_bmp, px - 1, py) != BLBA_BARVA)
      return (2);
    if (bmp_getpixel(p_bmp, px + 1, py) != BLBA_BARVA)
      return (1);
    else
      return (3);
  }
}

void ray_surface_filtruj_okraj(bitmapa * p_bmp, int okraj, dword * p_p1,
  int dp1, dword * p_p2, int dp2, dword * p_p3, int dp3)
{
  dword barva, r = 0, g = 0, b = 0;
  int cisel, i;
  int tabulka[] =
    { 0, 0, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0, -1, -1, 0, -1, 1, -1 };
//  int tabulka[] = {0,0, 1,0, 0,1, -1,0, 0,-1};
  int x, y, dx, dy;
  int px, py;

  dx = p_bmp->x;
  dy = p_bmp->y;

  for (x = okraj; x < (dx - okraj); x++) {
    for (y = okraj; y < (dy - okraj); y++) {
      if (bmp_getpixel(p_bmp, x, y) == BLBA_BARVA)
        continue;
      for (i = 0, cisel = 0; i < 18; i += 2) {
//      for(i = 0, cisel = 0; i < 10; i+=2) {
        px = x + tabulka[i];
        py = y + tabulka[i + 1];
        if (px > 0 && px < dx && py > 0 && py < dy) {
          barva = bmp_getpixel(p_bmp, px, py);
          if (barva != BLBA_BARVA) {
            r += GetRValue(barva);
            g += GetGValue(barva);
            b += GetBValue(barva);
            cisel++;
          }
          else {
            // nacteni barvy z pruhu
            // 1. zjistim jaky to je pripad (odkud to nacist)
            switch (ray_surface_pozice_bodu(p_bmp, px, py)) {
              case 0:          // je to vrchni pixel
                if (dp1) {
                  barva = p_p1[px];
                  r += GetRValue(barva);
                  g += GetGValue(barva);
                  b += GetBValue(barva);
                  cisel++;
                }
                break;
              case 1:          // je to levy-bocni pixel
                if (dp2) {
                  barva = p_p2[py];
                  r += GetRValue(barva);
                  g += GetGValue(barva);
                  b += GetBValue(barva);
                  cisel++;
                }
                break;
              case 2:          // je to pravy-bocni pixel
                if (dp3) {
                  barva = p_p3[py];
                  r += GetRValue(barva);
                  g += GetGValue(barva);
                  b += GetBValue(barva);
                  cisel++;
                }
                break;
              default:         // je to neco divnyho
                break;
            }
          }
        }
      }
      mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) cisel);
      r = g = b = 0;
      bmp_putpixel(p_bmp, x, y, barva);
    }
  }
}

dword ray_interpoluj_barvy(dword * p_barva, int pocet)
{
  dword r = 0, g = 0, b = 0, barva;
  int i;

  for (i = 0; i < pocet; i++) {
    r += GetRValue(p_barva[i]);
    g += GetGValue(p_barva[i]);
    b += GetBValue(p_barva[i]);
  }

  mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) pocet);
  return (barva);
}

int ray_okraj_roztahni(dword ** p_prh, int delka, int new_delka)
{
  dword *p_pruh = *p_prh;
  dword *p_npruh;
  int i;
  float kf;

  if (delka == new_delka)
    return (new_delka);

  if (!(p_npruh = malloc(new_delka * sizeof(dword))))
    chyba("Pamet");
  memset(p_npruh, 0, new_delka * sizeof(dword));

  kf = (float) delka / (float) new_delka;

  for (i = 0; i < new_delka; i++) {
    assert((int) floor(kf * i) < delka);
    p_npruh[i] = p_pruh[(int) floor(kf * i)];
  }
  free(*p_prh);
  *p_prh = p_npruh;

  return (new_delka);
}

__inline dword ray_interpoluj_2_barvy(dword b1, dword b2, float vaha2)
{
  dword r, g, b, barva;

  r = GetRValue(b1) + (dword) (vaha2 * GetRValue(b2));
  g = GetGValue(b1) + (dword) (vaha2 * GetGValue(b2));
  b = GetBValue(b1) + (dword) (vaha2 * GetBValue(b2));

  mult_color_rgb_max(&barva, r, g, b, 1.0f / (vaha2 + 1));
  return (barva);
}


void ray_surface_nastav_barvu_bodu(bitmapa * p_bmp, QBOD * p_hity, int delka,
  dword barva)
{
  int x, y;
  int i;

  for (i = 0; i < delka; i++) {
    if (p_hity[i].z) {
      x = p_hity[i].x;
      y = p_hity[i].y;
      bmp_putpixel(p_bmp, x, y, barva);
    }
  }
}

/* 
    Zpusob prace:
    while(stupen) {
      0. Nacteni okraju vsech surfacu
      1. Doplneni okraju do surfacu
         Udelani ke kazdemu surfacu seznam zapsanych bodu
      3. Filtrovani surfacu (vcetne hranicnich bodu)
      4. Smaz okraje
    }

    5. Nacti spolecne okraje
    6. Pokud spolu plosky sousedi -> vytvor spolecny okraj
    7. Vloz okraje
    8. Roztahni textury (dopln na pozadovane rozmery)
*/

void ray_poly_filtruj_lightmapy(POLY_INDICIE * p_ind, int indnum,
  EDIT_MESH_POLY * p_poly_list, EDIT_TEXT * p_src,
  int stupen, int mezi, int filtrace)
{
  int st;
  int i;

  if (!stupen)
    return;

  for (st = 0; st < stupen; st++) {
    /************************************************
    Filtruj surfacy plosek
    *************************************************/
    if (filtrace) {
      for (i = 0; i < indnum; i++) {
        p_cnf->ray.p_akt_filtr_func->p_fnc(p_src[i].p_bmp);
      }
    }
  }
}


/*
transformace bodu 3D -> 2D a zpet
  Pro kazdou plosku:   
    0. transformovat 3D plosku do 2D
    1. podle rozmeru vyrobit mapu
    2. vyrobit rastr a pro vsechny body:
      3. kazdy bod mapy transformovat z 2D do 3D
      4. zjistit svetlo bodu
      5. Zapsat svetlo do mapy
*/

/*
  Postup:
  Ke kazdemu svetlu ve scene
*/
void calc_face_matrix(TRIANGLE * p_tri, GLMATRIX * p_mat)
{
  GLMATRIX cm;
  BOD s, up, ri, fr;
  float vzd1, vzd2, vzd3, vzdn, ti;
  MUJ_BOD a, b, c;
  ROVINA r;
  BOD p;

  init_matrix(&cm);

  // stred trojuhelniku
  s.x = (p_tri->a.x + p_tri->b.x + p_tri->c.x) / 3.0f;
  s.y = (p_tri->a.y + p_tri->b.y + p_tri->c.y) / 3.0f;
  s.z = (p_tri->a.z + p_tri->b.z + p_tri->c.z) / 3.0f;

  // nejdelsi strana trojuhelnika
  vzd1 = vzdal_bodu(&p_tri->a, &p_tri->b);
  vzd2 = vzdal_bodu(&p_tri->b, &p_tri->c);
  vzd3 = vzdal_bodu(&p_tri->c, &p_tri->a);

  if (vzd1 > vzd2) {
    if (vzd1 > vzd3) {
      // nejvetsi vzd1
      a = p_tri->a;
      b = p_tri->b;
      c = p_tri->c;
      vzdn = vzd1;
    }
    else {
      // nejvetsi vzd3
      a = p_tri->c;
      b = p_tri->a;
      c = p_tri->b;
      vzdn = vzd3;
    }
  }
  else {
    if (vzd2 > vzd3) {
      // nejvetsi vzd2
      a = p_tri->b;
      b = p_tri->c;
      c = p_tri->a;
      vzdn = vzd2;
    }
    else {
      // nejvetsi vzd3
      a = p_tri->c;
      b = p_tri->a;
      c = p_tri->b;
      vzdn = vzd3;
    }
  }

  // p je bod na te primce
  ti =
    ((b.x - a.x) * (s.x - a.x) + (b.y - a.y) * (s.y - a.y) + (b.z -
      a.z) * (s.z - a.z)) / (vzdn * vzdn);
  p.x = a.x + (b.x - a.x) * ti;
  p.y = a.y + (b.y - a.y) * ti;
  p.z = a.z + (b.z - a.z) * ti;

  // preskladani plosky -> mezi a-b je nejvetsi delka
  p_tri->a = a;
  p_tri->b = b;
  p_tri->c = c;

  // vektor nahoru
  up.x = p.x - s.x;
  up.y = p.y - s.y;
  up.z = p.z - s.z;

  // vektor dopredu -> norm vekt. plosky
  calc_rovinu(&a, &b, &c, &r);
  fr.x = r.x;
  fr.y = r.y;
  fr.z = r.z;

  // vektor nabok -> osa x
  vektor_soucin(&fr, &up, &ri);

  // prisel cas normalizace
  norm_vect(&fr.x, &fr.y, &fr.z);
  norm_vect(&up.x, &up.y, &up.z);
  norm_vect(&ri.x, &ri.y, &ri.z);

  cm._11 = ri.x;
  cm._21 = ri.y;
  cm._31 = ri.z;                // x vektor -> ri
  cm._12 = up.x;
  cm._22 = up.y;
  cm._32 = up.z;                // y vektor -> up
  cm._13 = fr.x;
  cm._23 = fr.y;
  cm._33 = fr.z;                // z vektor -> fr

  cm._41 = s.x;                 // umisteni kamery
  cm._42 = s.y;
  cm._43 = s.z;

  //invert_matrix(&cm,p_mat);
  *p_mat = cm;
}


/*
  transformuje 3D face do 2D prostoru
*/

void trface(TRIANGLE * p_tri, TRIANGLE * p_tr, GLMATRIX * p_mat, float faktor)
{
  transf_3d_2d_matrix_float_text(p_tri->a.x, p_tri->a.y, p_tri->a.z,
    &p_tr->a.x, &p_tr->a.y, &p_tr->a.z, p_mat, faktor);
  transf_3d_2d_matrix_float_text(p_tri->b.x, p_tri->b.y, p_tri->b.z,
    &p_tr->b.x, &p_tr->b.y, &p_tr->b.z, p_mat, faktor);
  transf_3d_2d_matrix_float_text(p_tri->c.x, p_tri->c.y, p_tri->c.z,
    &p_tr->c.x, &p_tr->c.y, &p_tr->c.z, p_mat, faktor);
}


/*
  Prusecit 2D trojuhelniku s 2D bodem
  // validni je akorat x a y ve vsech bodech
*/


__inline int ray_intersect_tri_point(BOD * p_a, BOD * p_b, BOD * p_c,
  BOD * p_p)
{
  float k1, k2, q1, q2;
  float x;

  calc_primku_2d(p_a, p_c, &k1, &q1);
  calc_primku_2d(p_b, p_c, &k2, &q2);

  x = (p_p->y - q1) / k1;
  if (x > p_p->x)
    return (FALSE);

  x = (p_p->y - q2) / k2;
  if (x < p_p->x)
    return (FALSE);
  else
    return (TRUE);
}

// 2D interpolace normaloveho vektoru
__inline void interpolace_vektoru(TRIANGLE * p_tri, BOD * p_a, BOD * p_b,
  BOD * p_c, BOD * p_p, BOD * p_int)
{
  BOD p1, p2, v1, v2;
  float k1, k2, q1, q2;
  float vzd1, vzd2;

  calc_primku_2d(p_a, p_c, &k1, &q1);
  calc_primku_2d(p_b, p_c, &k2, &q2);

  p1.y = p2.y = p_p->y;
  p1.x = (p_p->y - q1) / k1;
  p2.x = (p_p->y - q2) / k2;

  vzd1 = vzdal_bodu_2D_bod(p_a, &p1);
  vzd2 = vzdal_bodu_2D_bod(p_c, &p1);

  v1.x = (p_tri->a.nx * vzd2 + p_tri->c.nx * vzd1) / (vzd1 + vzd2);
  v1.y = (p_tri->a.ny * vzd2 + p_tri->c.ny * vzd1) / (vzd1 + vzd2);
  v1.z = (p_tri->a.nz * vzd2 + p_tri->c.nz * vzd1) / (vzd1 + vzd2);

  vzd1 = vzdal_bodu_2D_bod(p_b, &p2);
  vzd2 = vzdal_bodu_2D_bod(p_c, &p2);

  v2.x = (p_tri->b.nx * vzd2 + p_tri->c.nx * vzd1) / (vzd1 + vzd2);
  v2.y = (p_tri->b.ny * vzd2 + p_tri->c.ny * vzd1) / (vzd1 + vzd2);
  v2.z = (p_tri->b.nz * vzd2 + p_tri->c.nz * vzd1) / (vzd1 + vzd2);

  vzd1 = p_p->x - p1.x;
  vzd2 = p2.x - p_p->x;

  p_int->x = (v1.x * vzd2 + v2.x * vzd1) / (vzd1 + vzd2);
  p_int->y = (v1.y * vzd2 + v2.y * vzd1) / (vzd1 + vzd2);
  p_int->z = (v1.z * vzd2 + v2.z * vzd1) / (vzd1 + vzd2);
}

// y = k*x + q

/*
transformace bodu 3D -> 2D a zpet
  Pro kazdou plosku:   
    0. transformovat 3D plosku do 2D
    1. podle rozmeru vyrobit mapu
    2. vyrobit rastr a pro vsechny body:
      3. kazdy bod mapy transformovat z 2D do 3D
      4. zjistit svetlo bodu
      5. Zapsat svetlo do mapy

typedef struct _LIGHTMAP_INFO {

  int k,o,f;

} LIGHTMAP_INFO;

#define   RAY_TEXTUR    200
bitmapa *    p_tray[RAY_TEXTUR]; // light-map textury k ray-tracingu
LIGHTMAP_INFO           face[RAY_TEXTUR]; // popis
int                     ray_posledni = 0;
*/
void ray_calc_korekci_koordinatu(TRIANGLE * p_tri, TEXT_KOORD * p_koord,
  bitmapa * p_bmp)
{
  BODUV *p_ua, *p_ub, *p_uc;
  BOD *p_da, *p_db, *p_dc;
  float x_tk;
  float y_tk;

  p_da = (BOD *) & p_tri->a;
  p_db = (BOD *) & p_tri->b;
  p_dc = (BOD *) & p_tri->c;

  p_ua = (BODUV *) & p_koord[0].tul;
  p_ub = (BODUV *) & p_koord[1].tul;
  p_uc = (BODUV *) & p_koord[2].tul;

  if (OKRAJ) {
    x_tk = (float) OKRAJ / p_bmp->x;
    y_tk = (float) OKRAJ / p_bmp->y;
  }
  else {
    x_tk = 0;
    y_tk = 0;
  }

  p_ua->u = 0.0f + x_tk;
  p_ua->v = 0.0f + y_tk;
  p_ub->u = 1.0f - x_tk;
  p_ub->v = 0.0f + y_tk;
  p_uc->u = ((p_da->x - p_dc->x) / (p_da->x - p_db->x)) + x_tk;
  p_uc->v = 1.0f - y_tk;
}


bitmapa *ray_calc_lightmap_face(K_EDITOR * p_cnf, TRIANGLE * p_tri,
  TRIANGLE * p_tri2d, LIGHTMAP_INFO * p_akt, byte stiny, int no_phong)
{
  bitmapa *p_text;
  GLMATRIX m, inv_m;
  int dx, dy, x, y;
  float iddx, iddy;
  float dx_faktor, dy_faktor;
  float fx, fy, fz;
  float x_tk;
  float y_tk;
  BOD p, a, b, c, pp;
  dword barva;
  int stinovani = p_cnf->ray.stinovani;
  int max_x, max_y;
  float mx, my, mz;

  calc_face_matrix(p_tri, &m);  //transf. matice teto plosky
  invert_matrix(&m, &inv_m);    // inverzni matice 
  trface(p_tri, p_tri2d, &m, p_cnf->ray.faktor);        // preved plosku do 2D

  //assert(p_tri2d->a.x < p_tri2d->b.x);
  if (p_tri2d->a.x >= p_tri2d->b.x)
    kprintf(TRUE, "p_tri2d->a.x >= p_tri2d->b.x... p = %d f = %f",
      p_akt->poly, p_akt->f);

  mujbod2bod(&a, &p_tri2d->a);
  mujbod2bod(&b, &p_tri2d->b);
  mujbod2bod(&c, &p_tri2d->c);

  // uprava na ctverec a spocitani ctvercovych scale faktoru
  // sirka mezi body trojuhelniku
  iddx = (float) fabs(a.x - b.x);
  iddy = (float) fabs(a.y - c.y);

  max_x = p_cnf->ray.max_x_surface >> 1;
  max_y = p_cnf->ray.max_y_surface >> 1;
  for (dx = MIN_X_TEXTURA;
    (dx - 2 * OKRAJ) < iddx && (dx - 2 * OKRAJ) < max_x; dx <<= 1);
  for (dy = MIN_Y_TEXTURA;
    (dy - 2 * OKRAJ) < iddy && (dy - 2 * OKRAJ) < max_y; dy <<= 1);

  // faktor - sirka jednoho texelu
  dx_faktor = iddx / (float) (dx - 2 * OKRAJ);
  dy_faktor = iddy / (float) (dy - 2 * OKRAJ);

  /*
     text korekce je sirka jednoho pixelu
   */
  if (OKRAJ) {
    x_tk = (float) OKRAJ / dx;
    y_tk = (float) OKRAJ / dy;
  }
  else {
    x_tk = 0;
    y_tk = 0;
  }

  p_tri->a.tul = 0.0f + x_tk;
  p_tri->a.tvl = 0.0f + y_tk;
  p_tri->b.tul = 1.0f - x_tk;
  p_tri->b.tvl = 0.0f + y_tk;
  p_tri->c.tul = ((a.x - c.x) / (a.x - b.x));
  p_tri->c.tvl = 1.0f - y_tk;

  // soupni do light akt facenormal
  p_akt->normal.x = m._13;
  p_akt->normal.y = m._23;
  p_akt->normal.z = m._33;
  if (stinovani == SHAD_FLAT)
    p_akt->normal_bod = p_akt->normal;

  // vyrob odpovidajici texturu
  if ((p_text = bmp_vyrob(dx, dy)) == NULL)
    chybat(STC, "Vyroba bmp -> dx = %d, dy = %d bpp = %d", dx, dy, 32);

  bmp_smaz(p_text, BLBA_BARVA);

  // I. pruchod -> pocitam pixely ulozene uvnitr trojuhelniku
  for (y = 0; y < dy; y++) {
    for (x = 0; x < dx; x++) {
      // prevedu bod textury do 3D
      fx = a.x + ((float) (x - OKRAJ + 0.5f) * dx_faktor);
      fy = a.y - ((float) (y - OKRAJ + 0.5f) * dy_faktor);
      fz = a.z;

      mx = fx / p_cnf->ray.faktor;
      my = fy / p_cnf->ray.faktor;
      mz = fz / p_cnf->ray.faktor;

      p.x = inv_m._11 * mx + inv_m._21 * my + inv_m._31 * mz + inv_m._41;
      p.y = inv_m._12 * mx + inv_m._22 * my + inv_m._32 * mz + inv_m._42;
      p.z = inv_m._13 * mx + inv_m._23 * my + inv_m._33 * mz + inv_m._43;

      pp.x = fx;
      pp.y = fy;
      pp.z = fz;
      p_cnf->stat.ray_bodu++;   // statistika pouziti
      if (stinovani == SHAD_PHONG) {
        interpolace_vektoru(p_tri, &a, &b, &c, &pp, &p_akt->normal_bod);
      }
      barva =
        ray_svetlost_bodu(p_cnf, &p, p_akt, stiny, p_cnf->ray.ambient,
        no_phong);
      barva &= 0x00ffffff;
      bmp_putpixel(p_text, x, y, barva);
    }
  }
  return (p_text);
}

void ray_dopln_okraj_surf(bitmapa * p_bmp)
{
  int x, y, dx, dy;
  int faze;
  dword barva, clamp_barva;

  dx = p_bmp->x;
  dy = p_bmp->y;

  // II. pruchod -> doplnim pixely ulozene vne trojuhelniku
  for (y = OKRAJ; y < (dy - OKRAJ); y++) {
    faze = 0;
    for (x = 0; x < dx; x++) {
      barva = bmp_getpixel(p_bmp, x, y);
      if (barva == BLBA_BARVA) {
        if (faze)
          bmp_putpixel(p_bmp, x, y, clamp_barva);
      }
      else {
        if (faze)
          clamp_barva = barva;
        else {
          faze = x;
          for (x = 0; x < faze; x++)
            bmp_putpixel(p_bmp, x, y, barva);
          faze = TRUE;
        }
      }
    }
  }
  if (OKRAJ) {
    for (x = 0; x < dx; x++) {
      barva = bmp_getpixel(p_bmp, x, OKRAJ);
      bmp_putpixel(p_bmp, x, 0, barva);
      barva = bmp_getpixel(p_bmp, x, dy - OKRAJ - 1);
      if (barva == BLBA_BARVA) {
        barva = bmp_getpixel(p_bmp, x, dy - OKRAJ - 2);
        bmp_putpixel(p_bmp, x, dy - OKRAJ - 1, barva);
      }
      bmp_putpixel(p_bmp, x, dy - 1, barva);
    }
  }
}

void ray_dopln_okraje(K_EDITOR * p_cnf, EDIT_TEXT * p_src, int textnum)
{
  int t;

  for (t = 0; t < textnum; t++) {
    ray_dopln_okraj_surf(p_src[t].p_bmp);
  }
}

#define SUPER_MIN_X 8
#define SUPER_MIN_Y 8

bitmapa *ray_surface_supersampl(bitmapa * p_zdroj, byte hloubka, int kx,
  int ky)
{
  bitmapa *p_cil = NULL;
  bitmapa *p_src = NULL;
  dword r, g, b;
  int x, y, dx, dy;
  int sx, sy, h, p;
  dword barva = 0;

  for (h = 0; h < hloubka; h++) {

    if (p_src) {
      bmp_zrus(&p_src);
      p_src = p_cil;
    }
    else {
      p_src = bmp_kopituj(p_zdroj);
    }

    dx = p_src->x;
    dy = p_src->y;

    if (dx / kx < SUPER_MIN_X || dy / ky < SUPER_MIN_Y) {
      return (p_src);
    }

    p_cil = bmp_vyrob(dx / kx, dy / ky);
    if (!p_cil)
      chyba("Vyrobeni surfacy !");

    for (y = 0; y < dy; y += kx) {
      for (x = 0; x < dx; x += ky) {
        r = g = b = p = 0;

        for (sy = 0; sy < ky; sy++) {
          for (sx = 0; sx < kx; sx++) {
            barva = bmp_getpixel(p_src, x + sx, y + sy);
            if (barva != BLBA_BARVA) {
              r += GetRValue(barva);
              g += GetGValue(barva);
              b += GetBValue(barva);
              p++;
            }
          }
        }

        if (p) {
          mult_color_rgb_max(&barva, r, g, b, 1.0f / (float) p);
          bmp_putpixel(p_cil, x / kx, y / ky, barva);
        }
      }
    }
  }
  bmp_zrus(&p_src);
  return (p_cil);
}

void ray_surface_supersampling(POLY_INDICIE * p_ind, int indnum,
  TRIANGLE * p_tri, EDIT_MESH_POLY * p_poly, EDIT_TEXT * p_src, byte hloubka,
  int kx, int ky)
{
  bitmapa *p_text;
  int i;

  for (i = 0; i < indnum; i++) {
    p_text = ray_surface_supersampl(p_src[i].p_bmp, hloubka, kx, ky);
    bmp_zrus(&p_src[i].p_bmp);
    p_src[i].p_bmp = p_text;
    ray_calc_korekci_koordinatu(p_tri + i,
      p_poly[p_ind[i].poly].p_koord + p_ind[i].face, p_text);
  }
}

void ray_dopln_okraj_spodni(bitmapa * p_bmp, int okraj)
{
  int x, dx, dy;
  dword barva;

  dx = p_bmp->x;
  dy = p_bmp->y;

  assert(okraj < 2);

  if (okraj) {
    for (x = 0; x < dx; x++) {
      barva = bmp_getpixel(p_bmp, x, dy - okraj - 1);
      bmp_putpixel(p_bmp, x, dy - 1, barva);
    }
  }
}

__inline static int ray_zacatek_masky(dword maska)
{
  int i = 0;

  while (!(maska & 0x1)) {
    maska >>= 1;
    i++;
  }
  return (i);
}

#define  MIN_T  0.99f

int ray_intersect_poly(EDIT_MESH_POLY * p_poly, BOD * p_orig, BOD * p_dir,
  LIGHTMAP_INFO * p_akt)
{
  BOD p1, p2, p3;
  float t, u, v, mint = MIN_T;
  int f, mf;

  if (p_poly->mail == p_akt->mail)
    return (FALSE);
  else
    p_poly->mail = p_akt->mail;

  if (obb_intersect_line(&p_poly->obb, p_orig, p_dir)) {
    for (f = 0; f < p_poly->facenum; f += 3) {
      if ((p_poly->poly == p_akt->poly) && (f == p_akt->f
          || f == p_akt->vface[0] || f == p_akt->vface[1]
          || f == p_akt->vface[2]))
        continue;

      p1.x = p_poly->p_koord[f].x;
      p1.y = p_poly->p_koord[f].y;
      p1.z = p_poly->p_koord[f].z;

      p2.x = p_poly->p_koord[f + 1].x;
      p2.y = p_poly->p_koord[f + 1].y;
      p2.z = p_poly->p_koord[f + 1].z;

      p3.x = p_poly->p_koord[f + 2].x;
      p3.y = p_poly->p_koord[f + 2].y;
      p3.z = p_poly->p_koord[f + 2].z;

      if (intersect_triangle_ncull(p_orig, p_dir, &p1, &p2, &p3, &t, &u, &v)) {
        if (t > 0.0f && t < mint) {
          mint = t;
          mf = f;
        }
      }
    }
  }
  return (mint != MIN_T);
}

int ray_intersect_kont(EDIT_KONTEJNER * p_kont, BOD * p_orig, BOD * p_dir,
  LIGHTMAP_INFO * p_akt)
{
  BOD p1, p2, p3, *p_tmp;
  EDIT_OBJEKT *p_obj;
  float t = 1.0f, u, v, mint = MIN_T;
  int f, mf, o, h = 0;

  if (p_kont->mailbox == p_akt->mail)
    return (FALSE);
  else
    p_kont->mailbox = p_akt->mail;

  if (obb_intersect_line(&p_kont->obb, p_orig, p_dir)) {
    for (o = 0; o < p_kont->objektu; o++) {
      p_obj = p_kont->p_obj[o];
      assert(p_obj);

      if (p_kont->objektu && !obb_intersect_line(&p_obj->obb, p_orig, p_dir))
        continue;

      for (f = 0; f < p_obj->facenum; f += 3) {
        p_tmp = (BOD *) (p_obj->p_vertex + p_obj->p_face[f]);
        p1 = *p_tmp;

        p_tmp = (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 1]);
        p2 = *p_tmp;

        p_tmp = (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 2]);
        p3 = *p_tmp;

        if (intersect_triangle_ncull(p_orig, p_dir, &p1, &p2, &p3, &t, &u,
            &v)) {
          if (t > 0.0f && t < mint) {
            mint = t;
            mf = f;
            h++;
          }
        }
      }
    }
  }

  return (mint != MIN_T);
}

/*
  Prusecik stinoveho paprsku se stromem -> presne rozmery paprsku
*/

/*
  Intersektuje scenu paprsku -> zacne tam kde je cil paprsku

  1. Testuj posledni hit
  2. Vlez tam kde je cil posledniho hitu (texel)
  3. Testuj prusecik v teto oblasti
  4. Pokud neni hit->testuj okoli smerem ke zdroji
*/
BOD oct_cilovy_bod;

int oct_intersect_tree_shadow_ray(KD_BUNKA * p_prvni, BOD * p_orig,
  BOD * p_dir, LIGHTMAP_INFO * p_akt)
{
  KD_BUNKA *p_list[2];
  KD_BUNKA *p_bunka = p_prvni;
  BOD cil;
  int i, hit = 0;
  BOD s1, s2;

  /*
     Stinovy paprsek
   */
  cil.x = p_orig->x + p_dir->x;
  cil.y = p_orig->y + p_dir->y;
  cil.z = p_orig->z + p_dir->z;

  /*
     Odkud vychazet
   */
  while (p_bunka->p_next) {     // test na pouzitou rovinu
    switch (p_bunka->rovina) {
      case 0:                  // rovina x
        i = (cil.x > p_bunka->vzdal) ? 1 : 0;
        break;
      case 1:                  // rovina y
        i = (cil.y > p_bunka->vzdal) ? 1 : 0;
        break;
      case 2:                  // rovina z
        i = (cil.z > p_bunka->vzdal) ? 1 : 0;
        break;
      default:
        assert(0);
        break;
    }
    p_bunka = p_bunka->p_next + i;
  }

  while (1) {

    /*
       Je prusecik v teto kostce
     */
    if (!kd_intersect_kostku_dir(p_orig, p_dir, &p_bunka->min, &p_bunka->max)) {
      p_bunka->mail = p_akt->mail;
      if (!(p_bunka = p_bunka->p_up)) {
        p_akt->ray_phit = K_CHYBA;
        p_akt->ray_khit = K_CHYBA;
        return (FALSE);
      }
    }

    /*
       Nalezena intersekce v teto bunce
     */
    if (!p_bunka->p_next) {
      /*
         Je to list stromu
       */
      if (p_bunka->p_polozka) {
        for (i = 0; i < p_bunka->polozek; i++) {
          if (p_bunka->p_polozka_typ[i] == KD_POLY) {
            if (ray_intersect_poly(p_bunka->p_polozka[i], p_orig, p_dir,
                p_akt)) {
              p_akt->ray_phit =
                ((EDIT_MESH_POLY *) (p_bunka->p_polozka[i]))->poly;
              p_akt->ray_khit = K_CHYBA;
              return (TRUE);
            }
          }
          else {
            if (ray_intersect_kont(p_bunka->p_polozka[i], p_orig, p_dir,
                p_akt)) {
              p_akt->ray_phit = K_CHYBA;
              p_akt->ray_khit =
                ((EDIT_KONTEJNER *) p_bunka->p_polozka[i])->kont;
              return (TRUE);
            }
          }
        }
      }
      /*
         V listu nenalezena intersekce -> okolni listy
         + posun nahoru
         Nastavim u miss bunky mail-flag        
       */
      p_bunka->mail = p_akt->mail;
      /*
         Level-Up, pokud jsem u konce
       */
      if (!(p_bunka = p_bunka->p_up)) {
        p_akt->ray_phit = K_CHYBA;
        p_akt->ray_khit = K_CHYBA;
        return (FALSE);
      }
    }
    else {
      /*
         Je to uzel stromu -> spracuj podstromy
         seradim podle stredu blizkosti k originu
       */
      oct_cilovy_bod.x = p_orig->x;
      oct_cilovy_bod.y = p_orig->y;
      oct_cilovy_bod.z = p_orig->z;

      p_list[0] = p_bunka->p_next;
      p_list[1] = p_bunka->p_next + 1;

      kd_stred_bunky(&p_list[0]->min, &p_list[0]->max, &s1);
      kd_stred_bunky(&p_list[1]->min, &p_list[1]->max, &s2);

      if (vzdal_bodu_bod(&s1, &oct_cilovy_bod) > vzdal_bodu_bod(&s2,
          &oct_cilovy_bod)) {
        p_list[1] = p_bunka->p_next;
        p_list[0] = p_bunka->p_next + 1;
      }

      for (i = 0; i < 2; i++) {
        if (p_list[i]->mail != p_akt->mail && (p_list[i]->p_next
            || p_list[i]->p_polozka)) {
          if (kd_intersect_kostku_dir(p_orig, p_dir, &p_list[i]->min,
              &p_list[i]->max)) {
            p_bunka = p_list[i];
            break;
          }
          else {
            p_list[i]->mail = p_akt->mail;
          }
        }
      }

      /* Level-Up
       */
      if (i == 2) {
        p_bunka->mail = p_akt->mail;
        if (!(p_bunka = p_bunka->p_up)) {
          p_akt->ray_phit = K_CHYBA;
          p_akt->ray_khit = K_CHYBA;
          return (FALSE);
        }
      }
    }
  }
}



// spocita svetlost bodu ve scene
// pomoci paprsku
/*
typedef struct _STATIC_LIGHT {

  byte      jmeno[50]; // jmeno svetla
  int       index;     // index svetla
  BOD       p;         // pozice svetla
  dword     rgb;       // barva
  float     k1,k2,k3;  // tlumici koeficienty
  float     dosah;     // max. dosah svetla
  
} STATIC_LIGHT;
*/

// p_p je bod pro ktery pocitam

#define MIN_LIGHT 0.002f

// pokud je pixel uvnitr trojuhelniku -> vloz interpolaci
// pokud je pixel ven z trojuhelniku -> vloz hodnotu barvy nejblizsi hrany

__inline dword ray_svetlost_bodu(K_EDITOR * p_cnf, BOD * p_p,
  LIGHTMAP_INFO * p_akt, byte stiny, dword ambient, int no_phong)
{
  STATIC_LIGHT *p_light;
  BOD sv, dir, mdir;
  int barva = 0;
  int l, h;
  static int akt_ray = 0;
  float dist, k1, k2, k3, I, uhel, dd[3];

  rgb_float(ambient, dd);

  for (l = 0; l < p_cnf->lightnum; l++) {
    p_light = p_cnf->light + l;

    if (!p_light->akt)
      continue;

    I = 1;
    sv = p_cnf->light[l].p;
    dist = vzdal_bodu_bod_vec(p_p, sv);
    if (dist > p_light->dosah)
      continue;
    calc_vect_dir_vec(sv, p_p, dir);
    mdir = dir;
    norm_vect(&dir.x, &dir.y, &dir.z);

    if (!no_phong) {
      uhel = (skal_soucin(p_akt->normal_bod, dir));     //cosinus uhlu
      if (uhel > 0 && p_light->uhel)
        I = uhel;
      else
        I = p_light->uhel ? 0.0f : 1.0f;
    }

    if (p_light->utlum) {
      k1 = p_light->k1;
      k2 = p_light->k2;
      k3 = p_light->k3;
      I *= 1.0f / (k1 + k2 * dist + k3 * dist * dist);
    }

    if (I > MIN_LIGHT && stiny) {
      p_akt->mail = ++akt_ray;

      /*
         Test posledniho hitu -> neni to pribuzny ?
       */
      if (p_light->last_poly != K_CHYBA) {
        if (ray_intersect_poly(p_cnf->p_poly + p_light->last_poly, &sv, &mdir,
            p_akt)) {
          continue;
        }
      }
      if (p_light->last_kont != K_CHYBA) {
        if (ray_intersect_kont(p_cnf->p_kont_raytracer[p_light->last_kont],
            &sv, &mdir, p_akt)) {
          continue;
        }
      }
      /*
         Jinak -> frcime full-serach
       */
      h = oct_intersect_tree_shadow_ray(&p_cnf->kdtree, &sv, &mdir, p_akt);
      p_light->last_poly = p_akt->ray_phit;
      p_light->last_kont = p_akt->ray_khit;
      I = (h) ? 0.0f : I;
    }

    dd[0] += p_light->r * I;
    dd[1] += p_light->g * I;
    dd[2] += p_light->b * I;

    if (dd[0] > 1.0f && dd[1] > 1.0f && dd[2] > 1.0f) {
      break;
    }
  }

  if (dd[0] > 1.0f)
    dd[0] = 1.0f;
  if (dd[1] > 1.0f)
    dd[1] = 1.0f;
  if (dd[2] > 1.0f)
    dd[2] = 1.0f;

  return (float_rgb(dd));
}

void propocitej_barvu_bod(OBJ_VERTEX * p_bod, BOD * p_light, dword rgb,
  float I, GLMATRIX * p_word)
{
  float px, py, pz;
  float tx, ty, tz;
  float uhel;

  px = p_bod->nx;               // normalovy vektor
  py = p_bod->ny;
  pz = p_bod->nz;

  tx = p_word->_11 * px + p_word->_21 * py + p_word->_31 * pz;  // transformace norm vektoru
  ty = p_word->_12 * px + p_word->_22 * py + p_word->_32 * pz;
  tz = p_word->_13 * px + p_word->_23 * py + p_word->_33 * pz;

  uhel = (float) sqrt(p_light->x * p_light->x + p_light->y * p_light->y +
    p_light->z * p_light->z) * (float) sqrt(tx * tx + ty * ty + tz * tz);
  uhel = (tx * p_light->x + ty * p_light->y + tz * p_light->z) / uhel;

  if (uhel > 0)
    add_mult_color_dword(&p_bod->dr, rgb, uhel);
}

#define EPSILON_DIST 0.0001f

void sloz_vertex(OBJ_VERTEX * p_bod, TEXT_KOORD * p_koord)
{
  p_bod->x = p_koord->x;
  p_bod->y = p_koord->y;
  p_bod->z = p_koord->z;

  p_bod->nx = p_koord->nx;
  p_bod->ny = p_koord->ny;
  p_bod->nz = p_koord->nz;

  p_bod->dr = p_koord->dr;
  p_bod->dg = p_koord->dg;
  p_bod->db = p_koord->db;
  p_bod->da = p_koord->da;

  p_bod->sr = p_koord->sr;
  p_bod->sg = p_koord->sg;
  p_bod->sb = p_koord->sb;

  p_bod->msr = p_koord->msr;
  p_bod->msg = p_koord->msg;
  p_bod->msb = p_koord->msb;

  p_bod->tu1 = p_koord->tu1;
  p_bod->tv1 = p_koord->tv1;
  p_bod->tu2 = p_koord->tu2;
  p_bod->tv2 = p_koord->tv2;
  p_bod->tul = p_koord->tul;
  p_bod->tvl = p_koord->tvl;
}

void sloz_vertex_zpet(TEXT_KOORD * p_koord, OBJ_VERTEX * p_bod)
{
  p_koord->x = p_bod->x;
  p_koord->y = p_bod->y;
  p_koord->z = p_bod->z;

  p_koord->nx = p_bod->nx;
  p_koord->ny = p_bod->ny;
  p_koord->nz = p_bod->nz;

  p_koord->dr = p_bod->dr;
  p_koord->dg = p_bod->dg;
  p_koord->db = p_bod->db;
  p_koord->da = p_bod->da;

  p_koord->sr = p_bod->sr;
  p_koord->sg = p_bod->sg;
  p_koord->sb = p_bod->sb;

  p_koord->msr = p_bod->msr;
  p_koord->msg = p_bod->msg;
  p_koord->msb = p_bod->msb;

  p_koord->tu1 = p_bod->tu1;
  p_koord->tv1 = p_bod->tv1;
  p_koord->tu2 = p_bod->tu2;
  p_koord->tv2 = p_bod->tv2;
  p_koord->tul = p_bod->tul;
  p_koord->tvl = p_bod->tvl;
}

void ray_poly_vyrob_lightmap(EDIT_MESH_POLY * p_poly)
{
  int lightmap = p_poly->facenum / 3;

  if (p_poly->p_light) {
    free(p_poly->p_light);
  }
  if (p_poly->p_lightnum) {
    free(p_poly->p_lightnum);
    p_poly->p_lightnum = NULL;
  }

  p_poly->p_light = (EDIT_TEXT **) malloc(sizeof(EDIT_TEXT *) * lightmap);
  mtest(p_poly->p_light);
  memset(p_poly->p_light, 0, sizeof(EDIT_TEXT *) * lightmap);

  p_poly->lightnum = lightmap;
}

void ray_smaz_blby_plosky(K_EDITOR * p_cnf)
{
  int flag = p_cnf->elm.flag;

  p_cnf->elm.flag =
    ELIM_MAZAT | ELIM_STATIC | ELIM_DYNAMIC | ELIM_PRVKY | ELIM_OKOLI;
  oe_prikaz_decimace_sceny(p_cnf);
  if (p_cnf->groupfacenum) {
    oe_prikaz_smaz_face(p_cnf, TRUE);
  }
  p_cnf->elm.flag = flag;
}

void ray_preved_barvy_do_materialu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int ak, ao;
  int mat, v;

  oe_klist_reset(&ak);
  while ((p_kont = oe_klist_next(p_cnf, &ak))) {
    if (p_kont->kflag & KONT_STATIC && p_kont->kflag & KONT_VERTEX_LIGHT) {
      oe_olist_reset(&ao);
      while ((p_obj = oe_olist_next(p_kont, &ao))) {
        mat = p_obj->material;
        if (p_cnf->p_mat[mat]->flag2 & MAT2_DIFFUSE_RAY) {
          for (v = 0; v < p_obj->vertexnum; v++) {
            p_obj->p_vertex[v].mdr = p_obj->p_vertex[v].dr;
            p_obj->p_vertex[v].mdg = p_obj->p_vertex[v].dg;
            p_obj->p_vertex[v].mdb = p_obj->p_vertex[v].db;
          }
        }
      }
    }
  }
}

BOOL CALLBACK doe_callback_raytracer(HWND hDlg, UINT msg, WPARAM wParam,
  LPARAM lParam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          EndDialog(hDlg, 1);
          break;
        case IDCANCEL:
          EndDialog(hDlg, 0);
          break;
        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG:
      break;
    case WM_DESTROY:
      EndDialog(hDlg, IDCANCEL);
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

// prepocita osvetleni stat. map
// spocita z polygonu p_cnf->p_poly
/*
typedef struct _EDIT_MESH_POLY { //celej kontejner
 
 POZICE_BODU *p_pozice;  // list bodu
 TEXT_KOORD  *p_koord;   // list texture koordinatu - plosky po 3
  
 word    flag;          // flag meshu -> stejne jako u kontejneru
 int     facenum;       // co face to objekt -> kazdy face ma svuj material
 int     matnum;        // pocet materialu
 word   *p_pocty;       // pocty plosek
 int    *p_mat;         // pole pointeru na materialy na jednotlive objekty
 EDIT_TEXT  **p_light;  // pole light-map

} EDIT_MESH_POLY;
*/

void ray_prepocitej_staticke_osvetleni(K_EDITOR * p_cnf, int ticho)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int ak, ao;
  EDIT_TEXT_KONFIG konf;
  LIGHTMAP_INFO linf;
  int indnum;
  POLY_INDICIE *p_ind;
  EDIT_TEXT *p_src;
  TRIANGLE *p_tri2d, tri;
  BOD vert;
  int p, f, fm, i, aface = 0, proc, mat, v;
  int vel_bmp;
  int no_phong;
  byte shadow, nadpis[200];
  dword stime, ptime, p2time, barva;
  HWND hwnd = NULL;

  if (!ticho && !doe_prikaz_raytracer(&p_cnf->ray))
    return;

  /* Eliminace degenerovanych plosek
   */
  if (p_cnf->cnf.druh_sceny != SCENA_POLY) {
    kprintf(TRUE, "Prevadim scenu do renderovaciho formatu...");
    oe_scena_to_game(p_cnf);
  }

  stime = timeGetTime();
  shadow = p_cnf->ray.stiny;

  oe_prepocitat_stat_svetla(p_cnf);

  for (i = 0; i < MAX_RAY_TEXTUR; i++) {
    if (p_cnf->p_lightmap[i].p_bmp) {
      bmp_zrus(&p_cnf->p_lightmap[i].p_bmp);
      p_cnf->p_lightmap[i].jmeno[0] = 0;
    }
  }

  if (!ticho) {
    hwnd =
      CreateDialog(hinst, MAKEINTRESOURCE(IDD_PROGRES), hwnd_plocha,
      doe_callback_raytracer);
    doe_printf(hwnd, IDC_STATIC1, "---- Leharo ----");
  }

  p_cnf->stat.ray_bodu = FALSE;

  // rendering sceny - diffusni svetlo
  if (!ticho) {
    doe_progresbar_nastav(hwnd, IDC_PROGRESS3, 0, p_cnf->stat.polyface / 3);
    doe_printf(hwnd, IDC_STATIC2, "Renderuju scenu...");

    doe_printf(hwnd, IDC_STATIC1, "Vertex-listy...");
    doe_printf(hwnd, IDC_STATIC4, "%d:%d", 00, 7);
  }

  oe_klist_reset(&ak);
  while ((p_kont = oe_klist_next(p_cnf, &ak))) {
    if (p_kont->kflag & KONT_STATIC && p_kont->kflag & KONT_VERTEX_LIGHT) {
      shadow = p_kont->kflag & KONT_NO_SELF_SHADOW ? FALSE : p_cnf->ray.stiny;
      no_phong = p_kont->k2flag & KONT2_NO_PHONG;
      oe_olist_reset(&ao);
      while ((p_obj = oe_olist_next(p_kont, &ao))) {
        if (!ticho) {
          doe_printf(hwnd, IDC_STATIC1, "List %s", p_obj->jmeno);
        }
        mat = p_obj->material;
        if (p_cnf->p_mat[mat]->flag2 & MAT2_DIFFUSE_RAY) {
          for (v = 0; v < p_obj->vertexnum; v++) {
            vert.x = p_obj->p_vertex[v].x;
            vert.y = p_obj->p_vertex[v].y;
            vert.z = p_obj->p_vertex[v].z;
            linf.normal_bod.x = p_obj->p_vertex[v].nx;
            linf.normal_bod.y = p_obj->p_vertex[v].ny;
            linf.normal_bod.z = p_obj->p_vertex[v].nz;
            barva =
              ray_svetlost_bodu(p_cnf, &vert, &linf, shadow,
              p_cnf->ray.ambient, no_phong);
            rgb_float(barva, &p_obj->p_vertex[v].dr);
            p_obj->p_vertex[v].dr *= p_obj->p_vertex[v].mdr;
            p_obj->p_vertex[v].dg *= p_obj->p_vertex[v].mdg;
            p_obj->p_vertex[v].db *= p_obj->p_vertex[v].mdb;
          }
        }
      }
    }
  }

  shadow = p_cnf->ray.stiny;

  indnum = 0;
  for (p = 0; p < p_cnf->polynum; p++) {
    indnum += p_cnf->p_poly[p].facenum / 3;
    ray_poly_vyrob_lightmap(p_cnf->p_poly + p);
    p_cnf->p_poly[p].kreslit = FALSE;
  }

  p_ind = malloc(sizeof(p_ind[0]) * indnum);
  mtest(p_ind);
  memset(p_ind, 0, sizeof(p_ind[0]) * indnum);


  indnum = 0;
  for (p = 0; p < p_cnf->polynum; p++) {

    if (p_cnf->p_poly[p].kflag & (KONT_PRVEK | KONT_PODLOZKA)) {

      for (i = indnum,
        indnum += p_cnf->p_poly[p].facenum / 3, f = 0; i < indnum; i++) {
        p_ind[i].poly = p;
        p_ind[i].face = f;
        f += 3;
      }

      if (p + 1 < p_cnf->polynum
        && p_cnf->p_poly[p + 1].kflag & (KONT_PRVEK | KONT_PODLOZKA)) {
        continue;
      }

    }
    else {

      indnum = p_cnf->p_poly[p].facenum / 3;
      for (i = 0; i < indnum; i++) {
        p_ind[i].poly = p;
        p_ind[i].face = i * 3;
      }
    }

    // Hotove indicie jsou hotove

    p_src = malloc(sizeof(p_src[0]) * indnum);
    mtest(p_src);

    p_tri2d = malloc(sizeof(p_tri2d[0]) * indnum);
    mtest(p_tri2d);

    if (ticho) {
      kprintf(TRUE, "Poly %d z %d", p, p_cnf->polynum);
    }

    p = K_CHYBA;

    // Rendering z indicii
    for (i = 0; i < indnum; i++) {

      // Prvni vyskyt urciteho poly - prvnotni inicializace
      if (p != p_ind[i].poly) {

        /* Nahozeni zakladnich hodnot
         */
        p = p_ind[i].poly;
        mat = p_cnf->p_poly[p].material;
        no_phong = p_cnf->p_poly[p].k2flag & KONT2_NO_PHONG;

        /* Testy barev
         */
        //if(!(p_cnf->p_mat[mat]->flag2&MAT2_DIFFUSE_RAY))
        //goto dalsi_poly;

        /* stiny/bez stinu
         */
        shadow =
          (p_cnf->p_poly[p].kflag & KONT_NO_SELF_SHADOW) ? FALSE : p_cnf->
          ray.stiny;

        /* Predpocitani spekularnich barev
         */
        if (p_cnf->p_mat[mat]->flag2 & MAT2_SPECULAR_RAY) {
          int fmax = p_cnf->p_poly[p].facenum;

          for (f = 0; f < fmax; f++) {
            ray_svetlost_vertex_poly(p_cnf,
              p_cnf->p_poly[p].p_koord + f, p_cnf->p_mat[mat]->lesk);
          }
        }
      }


      f = p_ind[i].face;
      fm = p_ind[i].face / 3;

      ptime = (timeGetTime() - stime);
      p2time = ptime / 1000;

      if (!ticho) {
        doe_printf(hwnd, IDC_STATIC1,
          "Polygon-list %d/%d face %d/%d face-bodu %d", p, p_cnf->polynum, fm,
          p_cnf->p_poly[p].facenum / 3, p_cnf->stat.ray_bodu);
        doe_printf(hwnd, IDC_STATIC4, "%d:%d", p2time / 60, p2time % 60);
        proc = ftoi(((float) (aface +=
              3) / (float) p_cnf->stat.polyface) * 100.0f);
        doe_printf(hwnd, IDC_STATIC5, "%d%%", proc);
        sprintf(nadpis, "Render %d%% hotovo - plna priorita", proc);
        SetWindowText(hwnd_aplikace, nadpis);
        spracuj_spravy(TRUE);
      }

      linf.f = f;
      linf.poly = p;
      linf.k = p;
      linf.vface[0] = p_cnf->p_poly[p].p_fsous[f];
      linf.vface[1] = p_cnf->p_poly[p].p_fsous[f + 1];
      linf.vface[2] = p_cnf->p_poly[p].p_fsous[f + 2];

      ray_svetlost_vertex_poly_diff(p_cnf->p_poly[p].p_koord + f);
      ray_svetlost_vertex_poly_diff(p_cnf->p_poly[p].p_koord + f + 1);
      ray_svetlost_vertex_poly_diff(p_cnf->p_poly[p].p_koord + f + 2);

      sloz_vertex(&tri.a, p_cnf->p_poly[p].p_koord + f);
      sloz_vertex(&tri.b, p_cnf->p_poly[p].p_koord + f + 1);
      sloz_vertex(&tri.c, p_cnf->p_poly[p].p_koord + f + 2);

      p_src[i].p_bmp =
        ray_calc_lightmap_face(p_cnf, &tri, p_tri2d + i, &linf, shadow,
        no_phong);

      sloz_vertex_zpet(p_cnf->p_poly[p].p_koord + f, &tri.a);
      sloz_vertex_zpet(p_cnf->p_poly[p].p_koord + f + 1, &tri.b);
      sloz_vertex_zpet(p_cnf->p_poly[p].p_koord + f + 2, &tri.c);

      doe_progresbar_posun(hwnd, IDC_PROGRESS3, 1);
    }

    //ray_dopln_okraje(p_cnf, p_src, indnum);

    if (p_cnf->ray.filtrace) {
      if (!ticho) {
        doe_printf(hwnd, IDC_STATIC1, "Filtrace...");
      }
      ray_poly_filtruj_lightmapy(p_ind, indnum, p_cnf->p_poly, p_src,
        p_cnf->ray.stupen_filtrace,
        p_cnf->ray.disk_mezi, p_cnf->ray.filtrace);
    }

    if (p_cnf->ray.supersampl) {
      if (!ticho) {
        doe_printf(hwnd, IDC_STATIC1, "Supersampling...");
      }
      ray_surface_supersampling(p_ind, indnum, p_tri2d, p_cnf->p_poly,
        p_src,
        p_cnf->ray.sup_hloubka,
        p_cnf->ray.sup_velikost_x, p_cnf->ray.sup_velikost_y);
    }

    if (p_cnf->ray.disk_mezi) {
      for (i = 0; i < indnum; i++) {
        sprintf(p_src[i].jmeno, "e:\\lihen\\lightmap\\face-i%.3d_s.png", i);
        bmp_uloz(p_src[i].jmeno, p_src[i].p_bmp);
      }
    }

    if (!ticho) {
      doe_printf(hwnd, IDC_STATIC1, "Lepeni...");
    }
    ray_slep_lightmapy(p_cnf, p_ind, indnum, p_src);

    for (i = 0; i < indnum; i++)
      bmp_zrus(&p_src[i].p_bmp);
    free(p_src);
    p_src = NULL;

    free(p_tri2d);

    indnum = 0;
  }

  free(p_ind);
  for (p = 0; p < p_cnf->polynum; p++) {
    ray_poly_optim_lightmap(p_cnf->p_poly + p);
    p_cnf->p_poly[p].kreslit = TRUE;
  }


  ptime = (timeGetTime() - stime);
  stime = ptime / 1000;

  vel_bmp = 0;
  for (i = 0, mat = 0; i < MAX_RAY_TEXTUR; i++) {
    if (p_cnf->p_lightmap[i].p_bmp) {
      mat = i + 1;
      vel_bmp +=
        p_cnf->p_lightmap[i].p_bmp->x * p_cnf->p_lightmap[i].p_bmp->y * 4;
    }
  }

  if (!ticho) {
    doe_printf(hwnd, IDC_STATIC1, "---- Prevod light-map do textur ----");
    doe_progresbar_nastav(hwnd, IDC_PROGRESS3, 0, mat);
  }

  {
    int zal_filtr = txconf.text_filtr;

    txconf.text_filtr = p_cnf->ray.lightmap_filtr;
    txt_lightmap_konfig(&konf);
    txconf.text_filtr = zal_filtr;
  }

  if (!ticho) {
    for (i = 0; i < mat; i++) {
      if (p_cnf->p_lightmap[i].p_bmp) {
        //bmp_to_16bit(p_cnf->p_lightmap[i].p_bmp,0x7,FALSE);
//        txt_bmp2textura(p_cnf->p_lightmap[i].p_bmp, p_cnf->p_lightmap+i,&konf);
        p_cnf->p_lightmap[i].load = TRUE;
        doe_progresbar_posun(hwnd, IDC_PROGRESS3, 1);
      }
    }
  }

  for (p = 0; p < p_cnf->polynum; p++) {
    p_cnf->p_poly[p].kreslit = TRUE;
  }

  kprintf(TRUE, "Ray-tracing info:");
  kprintf(TRUE, "Cas (%d:%d:%d)", stime / 60, stime % 60, ptime % 1000);
  kprintf(TRUE, "Bodu %d", p_cnf->stat.ray_bodu);
  kprintf(TRUE, "Velikost %d Byte", vel_bmp);
  kprintf(TRUE, "Stiny %d", p_cnf->ray.stiny);
  kprintf(TRUE, "Faktor %f", p_cnf->ray.faktor);
  kprintf(TRUE, "Filtr %d", p_cnf->ray.stupen_filtrace);
  kprintf(TRUE, "Disk %d", p_cnf->ray.disk);

  if (!ticho) {
    EndDialog(hwnd, TRUE);
  }
}

__inline void ray_rozmnoz_diffuse(OBJ_VERTEX * p_vert)
{
  p_vert->da = p_vert->mda;
  p_vert->dr = p_vert->mdr;
  p_vert->dg = p_vert->mdg;
  p_vert->db = p_vert->mdb;
}

__inline void ray_rozmnoz_specular(OBJ_VERTEX * p_vert)
{
  p_vert->sr = p_vert->msr;
  p_vert->sg = p_vert->msg;
  p_vert->sb = p_vert->msb;
}

// Prepocitani bodu
__inline void ray_svetlost_vertex_bodu(K_EDITOR * p_cnf, OBJ_VERTEX * p_vert,
  GLMATRIX * p_word, int diff, int spec, float lesk, dword ambient)
{
  STATIC_LIGHT *p_light;
  BOD v, n;
  BOD sv, dir;
  float dist, k, I, Is, uhel;
  int l;

  v.x = p_vert->x;
  v.y = p_vert->y;
  v.z = p_vert->z;
  n.x = p_vert->nx;
  n.y = p_vert->ny;
  n.z = p_vert->nz;

  rgb_float(ambient, &p_vert->dr);
  p_vert->sr = p_vert->sg = p_vert->sb = 0.0f;

  transformuj_bod_matici(&v.x, &v.y, &v.z, p_word);
  transformuj_nbod_matici(&n, p_word);

  for (l = 0; l < p_cnf->lightnum; l++) {
    p_light = p_cnf->light + l;


    if (!p_light->akt || p_light->bez_prev)
      continue;

    Is = I = 1;

    if (p_light->flag2 & SL2_MESH) {
      transformuj_bod_matici_bod(&p_light->p,
        kont_world_matrix(p_light->p_mesh_data), &sv);
    }
    else {
      sv = p_light->p;
    }

    dist = vzdal_bodu_bod(&v, &sv);
    if (dist > p_light->dosah)
      continue;

    vektor_sub(&v, &sv, &dir);
    norm_vect(&dir.x, &dir.y, &dir.z);


    if (!p_cnf->cnf.svetla_bez_gourandu) {
      // vektor bod->vektor
      uhel = (skal_soucin(n, dir));     //cosinus uhlu
      if (uhel > 0 && p_light->uhel) {
        Is = I = uhel;
        Is *= lesk;
      }
      else {
        Is = I = p_light->uhel ? 0.0f : 1.0f;
      }
    }

    if (p_light->utlum) {
      k =
        1.0f / (p_light->k1 + p_light->k2 * dist + p_light->k3 * dist * dist);
      I *= k;
      Is *= k;
    }

    if (diff) {
      p_vert->dr += p_vert->mdr * p_light->r * I;
      p_vert->dg += p_vert->mdg * p_light->g * I;
      p_vert->db += p_vert->mdb * p_light->b * I;
    }

    if (spec) {
      p_vert->sr += p_vert->msr * p_light->r * Is;
      p_vert->sg += p_vert->msg * p_light->g * Is;
      p_vert->sb += p_vert->msb * p_light->b * Is;
    }
  }

  if (p_vert->dr > 1.0f)
    p_vert->dr = 1.0f;
  if (p_vert->dg > 1.0f)
    p_vert->dg = 1.0f;
  if (p_vert->db > 1.0f)
    p_vert->db = 1.0f;

  if (p_vert->sr > 1.0f)
    p_vert->sr = 1.0f;
  if (p_vert->sg > 1.0f)
    p_vert->sg = 1.0f;
  if (p_vert->sb > 1.0f)
    p_vert->sb = 1.0f;
}

/* Provede update dynamickych svetel
*/
void ray_objekt_pridej_ddlight(K_EDITOR * p_cnf, DYN_LIGHT * p_ddlight,
  int ddlightnum, EDIT_KONTEJNER * p_kont,
  EDIT_OBJEKT * p_obj, GLMATRIX * p_m)
{
  GLMATRIX *p_mat;
  DYN_LIGHT *p_dlight;
  BOD *p_vertex_pos;
  BOD *p_vertex_norm;
  BODRGBA *p_vertex_diff;
  BODRGBA *p_vertex_diff_mat;
  BOD p, n, d;
  int i, v, dflag, m2flag, j;
  int diffuse;
  int diffuse_ray;
  int dyn_light = 1;
  int kflag;
  float vzdal, I, lesk = 1.0f;
  float *p_looksqrt_linear_float = p_cnf->p_looksqrt_linear_float;
  float *p_looksqrt_quad_float = p_cnf->p_looksqrt_quadratic_float;


  kflag = p_kont->kflag & KONT_KEYFRAME;
  m2flag = p_kont->m2flag;

  diffuse = m2flag & MAT2_DIFFUSE;
  diffuse_ray = m2flag & MAT2_DIFFUSE_RAY;

  if (!(diffuse))
    return;

  for (v = 0; v < p_obj->vertexnum; v++) {
    p_vertex_pos = (BOD *) & p_obj->p_vertex[v].x;
    p_vertex_norm = (BOD *) & p_obj->p_vertex[v].nx;

    transformuj_bod_matici_bod((BOD *) & p_vertex_pos->x, p_m, &p);
    transformuj_nbod_matici_bod((BOD *) & p_vertex_norm->x, p_m, &n);

    p_vertex_diff = (RGBA *) & p_obj->p_vertex[v].dr;
    p_vertex_diff_mat = (RGBA *) & p_obj->p_vertex[v].mdr;

    for (i = 0; i < ddlightnum; i++) {
      p_dlight = p_ddlight + i;
      dflag = p_dlight->flag;
      if (dflag & SDL_MESH && p_dlight->p_mesh_data) {  // pod-meshove svetlo preved s world space kontejneru
        p_mat = kont_world_matrix(p_dlight->p_mesh_data);
        transformuj_bod_matici_bod(&p_dlight->np, p_mat, &d);
        vzdal = vzdal_bodu_bod(&p_dlight->np, &d);
      }
      else {
        vzdal = vzdal_bodu_bod(&p_dlight->np, &p);
      }
      if (vzdal > p_dlight->dosah)
        continue;
      if (vzdal <= p_dlight->min) {
        if (dflag & SDL_ADD_SUB) {
          p_vertex_diff->r += p_vertex_diff_mat->r * p_dlight->dr;
          p_vertex_diff->g += p_vertex_diff_mat->g * p_dlight->dg;
          p_vertex_diff->b += p_vertex_diff_mat->b * p_dlight->db;
          p_vertex_diff->r -= p_vertex_diff_mat->r * p_dlight->sr;
          p_vertex_diff->g -= p_vertex_diff_mat->g * p_dlight->sg;
          p_vertex_diff->b -= p_vertex_diff_mat->b * p_dlight->sb;
        }
        else if (dflag & SDL_SUB) {
          p_vertex_diff->r -= p_vertex_diff_mat->r * p_dlight->dr;
          p_vertex_diff->g -= p_vertex_diff_mat->g * p_dlight->dg;
          p_vertex_diff->b -= p_vertex_diff_mat->b * p_dlight->db;
        }
        else {
          p_vertex_diff->r += p_vertex_diff_mat->r * p_dlight->dr;
          p_vertex_diff->g += p_vertex_diff_mat->g * p_dlight->dg;
          p_vertex_diff->b += p_vertex_diff_mat->b * p_dlight->db;
        }
      }
      else {
        j =
          ftoi(((vzdal - p_dlight->min) / (p_dlight->max -
              p_dlight->min)) * 255.0f);
        I =
          (dflag & SDL_UTLUM_LINEAR) ? p_looksqrt_linear_float[j] :
          p_looksqrt_quad_float[j];
        if (I < DELTA_MIN)
          continue;
        if (dflag & SDL_ADD_SUB) {
          p_vertex_diff->r += p_vertex_diff_mat->r * p_dlight->dr * I;
          p_vertex_diff->g += p_vertex_diff_mat->g * p_dlight->dg * I;
          p_vertex_diff->b += p_vertex_diff_mat->b * p_dlight->db * I;
          p_vertex_diff->r -= p_vertex_diff_mat->r * p_dlight->sr * I;
          p_vertex_diff->g -= p_vertex_diff_mat->g * p_dlight->sg * I;
          p_vertex_diff->b -= p_vertex_diff_mat->b * p_dlight->sb * I;
        }
        else if (dflag & SDL_SUB) {
          p_vertex_diff->r -= p_vertex_diff_mat->r * p_dlight->dr * I;
          p_vertex_diff->g -= p_vertex_diff_mat->g * p_dlight->dg * I;
          p_vertex_diff->b -= p_vertex_diff_mat->b * p_dlight->db * I;
        }
        else {
          p_vertex_diff->r += p_vertex_diff_mat->r * p_dlight->dr * I;
          p_vertex_diff->g += p_vertex_diff_mat->g * p_dlight->dg * I;
          p_vertex_diff->b += p_vertex_diff_mat->b * p_dlight->db * I;
        }
      }
    }
  }
}

/* Provede update extra-dynamickych svetel
*/
void ray_objekt_pridej_edlight(K_EDITOR * p_cnf, EXTRA_DYN_LIGHT * p_edlight,
  int edlightnum, EDIT_KONTEJNER * p_kont,
  EDIT_OBJEKT * p_obj, GLMATRIX * p_m)
{
  EXTRA_DYN_LIGHT *p_light;
  BOD *p_vertex_pos;
  BOD *p_vertex_norm;
  BODRGBA *p_vertex_diff;
  BODRGB *p_vertex_spec;
  BOD p, n, d, dir;
  int i, v, dflag, m2flag;
  int dyn_light = 1;
  int kflag;
  float vzdal, I, lesk = 1.0f;


  kflag = p_kont->kflag & KONT_KEYFRAME;
  m2flag = p_kont->m2flag;

  for (v = 0; v < p_obj->vertexnum; v++) {
    p_vertex_pos = (BOD *) & p_obj->p_vertex[v].x;
    p_vertex_norm = (BOD *) & p_obj->p_vertex[v].nx;

    transformuj_bod_matici_bod((BOD *) & p_vertex_pos->x, p_m, &p);
    transformuj_nbod_matici_bod((BOD *) & p_vertex_norm->x, p_m, &n);

    p_vertex_diff = (RGBA *) & p_obj->p_vertex[v].dr;
    p_vertex_spec = (RGB *) & p_obj->p_vertex[v].sr;

    for (i = 0; i < p_cnf->edl_lightnum; i++) {
      p_light = p_cnf->p_edlight + i;
      if (p_light->akt) {

        dflag = p_light->flag;

        if (dflag & EDL_MESH) {
          transformuj_bod_matici_bod(&p_light->np,
            kont_world_matrix(p_light->p_mesh_data), &d);
        }
        else {
          d = p_light->np;
        }

        if (dflag & EDL_DOSAH) {
          if (dflag & EDL_BODOVE) {
            vzdal = vzdal_bodu_bod(&d, &p);
          }
          else if (dflag & (EDL_PLOSNE_X | EDL_PLOSNE_Y | EDL_PLOSNE_Z)) {
            if (dflag & EDL_PLOSNE_X) {
              vzdal = fabsf(d.x - p.x);
            }
            else if (dflag & EDL_PLOSNE_Y) {
              vzdal = fabsf(d.y - p.y);
            }
            else {
              vzdal = fabsf(d.z - p.z);
            }
          }
          if (vzdal >= p_light->dosah)
            continue;

          if (dflag & EDL_UTLUM_LIN) {
            vzdal = 1.0f - (vzdal * p_light->utlum) / p_light->dosah;
            if (vzdal < DELTA_MIN)
              continue;
          }
          else if (dflag & EDL_UTLUM_KVAD) {
            vzdal = 1.0f - (vzdal * vzdal * p_light->utlum) / p_light->dosah;
            if (vzdal < DELTA_MIN)
              continue;
          }
          else {
            vzdal = 1.0f;
          }
        }
        else {
          vzdal = 1.0f;
        }

        if (dflag & EDL_GOURAND) {
          vektor_sub(&p, &d, &dir);
          norm_vect(&dir.x, &dir.y, &dir.z);
          I = vektor_dot_product(&n, &dir) * vzdal * p_light->gourand;
          if (I < DELTA_MIN)
            continue;
        }
        else {
          I = vzdal;
        }

        // Nastav alfa slozku
        if (dflag & (EDL_ALFA_MOD | EDL_ALFA_SET | EDL_ALFA_ADD |
            EDL_ALFA_SUB)) {
          if (dflag & EDL_ALFA_ADD) {
            p_vertex_diff->a += p_light->da * I;
          }
          else if (dflag & EDL_ALFA_MOD) {
            p_vertex_diff->a *= p_light->da * I;
          }
          else if (dflag & EDL_ALFA_SUB) {
            p_vertex_diff->a -= p_light->da * I;
          }
          else if (dflag & EDL_ALFA_SET) {
            p_vertex_diff->a = p_light->da * I;
          }
        }

        // Nastav spekularni slozku
        if (dflag & (EDL_SPEC_MOD | EDL_SPEC_SET | EDL_SPEC_ADD |
            EDL_SPEC_SUB)) {
          if (dflag & EDL_SPEC_ADD) {
            p_vertex_spec->r += p_light->sr * I;
            p_vertex_spec->g += p_light->sg * I;
            p_vertex_spec->b += p_light->sb * I;
          }
          else if (dflag & EDL_SPEC_MOD) {
            p_vertex_spec->r *= p_light->sr * I;
            p_vertex_spec->g *= p_light->sg * I;
            p_vertex_spec->b *= p_light->sb * I;
          }
          else if (dflag & EDL_SPEC_SUB) {
            p_vertex_spec->r -= p_light->sr * I;
            p_vertex_spec->g -= p_light->sg * I;
            p_vertex_spec->b -= p_light->sb * I;
          }
          else if (dflag & EDL_SPEC_SET) {
            p_vertex_spec->r = p_light->sr * I;
            p_vertex_spec->g = p_light->sg * I;
            p_vertex_spec->b = p_light->sb * I;
          }
        }
      }
    }
  }
}


#define RYCHLOST_ZMENY 100


// Musi byt vsechny veci nachystany -> normalove vektory + barvy md a ms
// Prepocet pokud je scena v obycejnym modu
void ray_prepocitej_staticke_osvetleni_vertexy(K_EDITOR * p_cnf)
{
  EDIT_MATERIAL **p_mat = p_cnf->p_mat;
  EDIT_OBJEKT *p_obj;
  GLMATRIX *p_m;
  dword kflag;
  int i, o, v, d, s;
  static dword stary_cas = 0;
  dword akt_cas = timeGetTime();
  float lesk;

  if (akt_cas - stary_cas < RYCHLOST_ZMENY
    || p_cnf->cnf.druh_sceny == SCENA_POLY)
    return;
  stary_cas = akt_cas;

  for (i = 0; i < KONT_NUM; i++) {
    if (!p_cnf->p_kont[i])
      continue;
    kflag = p_cnf->p_kont[i]->kflag;
    if (!(kflag & KONT_KEYFRAME))
      p_m = kont_world_matrix(p_cnf->p_kont[i]);
    for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
      p_obj = p_cnf->p_kont[i]->p_obj[o];
      if (!p_obj)
        continue;
      if (kflag & KONT_KEYFRAME)
        p_m = &p_obj->m;
      d = p_mat[p_obj->material]->flag2 & MAT2_DIFFUSE_RAY;
      s = p_mat[p_obj->material]->flag2 & MAT2_SPECULAR_RAY;
      lesk = p_mat[p_obj->material]->lesk;
      if (d || s) {
        for (v = 0; v < p_obj->vertexnum; v++) {
          ray_svetlost_vertex_bodu(p_cnf, p_obj->p_vertex + v, p_m, d, s,
            lesk, p_cnf->ray.ambient);
        }
      }
      ray_objekt_pridej_ddlight(p_cnf, p_cnf->p_dlight, p_cnf->dl_lightnum,
        p_cnf->p_kont[i], p_obj, p_m);
      ray_objekt_pridej_edlight(p_cnf, p_cnf->p_edlight, p_cnf->edl_lightnum,
        p_cnf->p_kont[i], p_obj, p_m);
    }
  }
}

void ray_zrus_staticke_osvetleni_vertexy(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  int i, o, v;

  for (i = 0; i < KONT_NUM; i++) {
    if (!p_cnf->p_kont[i])
      continue;
    for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
      p_obj = p_cnf->p_kont[i]->p_obj[o];
      if (!p_obj)
        continue;
      for (v = 0; v < p_obj->vertexnum; v++) {
        ray_rozmnoz_diffuse(p_obj->p_vertex + v);
        ray_rozmnoz_specular(p_obj->p_vertex + v);
      }
    }
  }
}

__inline void ray_svetlost_vertex_poly(K_EDITOR * p_cnf, TEXT_KOORD * p_koord,
  float lesk)
{
  STATIC_LIGHT *p_light;
  BOD v, n;
  BOD sv, dir;
  float dist, k, Is, uhel;
  int l;

  v.x = p_koord->x;
  v.y = p_koord->y;
  v.z = p_koord->z;
  n.x = p_koord->nx;
  n.y = p_koord->ny;
  n.z = p_koord->nz;

  p_koord->sr = p_koord->sg = p_koord->sb = 0.0f;

  for (l = 0; l < p_cnf->lightnum; l++) {
    p_light = p_cnf->light + l;

    if (!p_light->akt)
      continue;

    Is = 1;

    sv = p_cnf->light[l].p;
    dist = vzdal_bodu_bod(&v, &sv);
    vektor_sub(&v, &sv, &dir);
    norm_vect(&dir.x, &dir.y, &dir.z);

    if (dist > p_light->dosah)
      continue;

    if (!p_cnf->cnf.svetla_bez_gourandu) {
      // vektor bod->vektor
      uhel = (skal_soucin(n, dir));     //cosinus uhlu
      if (uhel > 0 && p_light->uhel) {
        Is = uhel * lesk;
      }
      else {
        Is = p_light->uhel ? 0.0f : 1.0f;
      }
    }

    if (p_light->utlum) {
      k =
        1.0f / (p_light->k1 + p_light->k2 * dist + p_light->k3 * dist * dist);
      Is *= k;
    }

    p_koord->sr += p_koord->msr * p_light->r * Is;
    p_koord->sg += p_koord->msg * p_light->g * Is;
    p_koord->sb += p_koord->msb * p_light->b * Is;
  }
}

__inline void ray_svetlost_vertex_poly_diff(TEXT_KOORD * p_koord)
{
  p_koord->dr = p_koord->mdr;
  p_koord->dg = p_koord->mdg;
  p_koord->db = p_koord->mdb;
  p_koord->da = p_koord->mda;
}

void ray_uloz_lightmapy(K_EDITOR * p_cnf, byte * p_file, byte * p_dir)
{
  FFILE f;
  int i;
  byte pom[200];


  strcpy(pom, p_file);
  zamen_koncovku(pom, KONCOVKA_LIGHTMAPY);

  if ((f = ffopen(pom, "wb"))) {
    for (i = 0; i < MAX_RAY_TEXTUR; i++) {
      if (p_cnf->p_lightmap[i].p_bmp) {
        ffwrite(&i, sizeof(i), 1, f);
        bmp_uloz_pack(f, p_cnf->p_lightmap[i].p_bmp);
      }
    }
    ffclose(f);
  }
}

/*
void ray_uloz_lightmapy(K_EDITOR *p_cnf, byte *p_file, byte *p_dir)
{ 
  int i;
  byte pom[200];

  for(i = 0; i < MAX_RAY_TEXTUR; i++) {
    if(p_cnf->p_lightmap[i].p_bmp) {
      sprintf(pom,"l%d.png",i);
      bmp_uloz(pom,p_cnf->p_lightmap[i].p_bmp);
    }
  }  
}
*/

int ray_vyrob_poly_lightmap_compare(const void *p_poly1, const void *p_poly2)
{
  int cmp;

  cmp = ((LIGHTMAP_INFO *) p_poly1)->poly - ((LIGHTMAP_INFO *) p_poly2)->poly;
  if (cmp)
    return (cmp);

  cmp = ((LIGHTMAP_INFO *) p_poly2)->dy - ((LIGHTMAP_INFO *) p_poly1)->dy;
  if (cmp)
    return (cmp);

  cmp = ((LIGHTMAP_INFO *) p_poly2)->dx - ((LIGHTMAP_INFO *) p_poly1)->dx;
  return (cmp);
}

/*
typedef struct _LIGHTMAP_INFO {
  BOD normal;    // face normal
  int p,k,o,f;   // umisteni light-mapy  
  int dx,dy;     // rozmery light-mapy
  int material;
} LIGHTMAP_INFO;
*/

typedef struct _LINKA_TEXTURY
{

  RECT r;                       // ulozeni a velikost na mape
  int radek;                    // radek
  int vyska_radku;              //
  int dx, dy;                   // pomocne recy  
  int indicie;                  // odpovidajici indicie
  struct _LINKA_TEXTURY *p_dalsi;       // pod-bunky -> NULL == Konec

} LINKA_TEXTURY;

__inline LINKA_TEXTURY *ray_vloz_volnou_linku(LINKA_TEXTURY ** p_volne_linky,
  LINKA_TEXTURY * p_tmp, int left, int top, int right, int bottom)
{
  SetRect(&p_tmp->r, left, top, right, bottom);
  p_tmp->dx = right - left;
  p_tmp->dy = bottom - top;
  p_tmp->radek = K_CHYBA;
  p_tmp->vyska_radku = p_tmp->dy;
  p_tmp->p_dalsi = *p_volne_linky;
  *p_volne_linky = p_tmp;
  return (p_tmp);
}

__inline LINKA_TEXTURY *ray_vloz_volnou_linku_all(LINKA_TEXTURY **
  p_volne_linky, int left, int top, int right, int bottom, int indicie)
{
  LINKA_TEXTURY *p_tmp;

  p_tmp = malloc(sizeof(p_tmp[0]));
  mtest(p_tmp);
  memset(p_tmp, 0, sizeof(p_tmp[0]));
  p_tmp->indicie = indicie;
  return (ray_vloz_volnou_linku(p_volne_linky, p_tmp, left, top, right,
      bottom));
}


/* Vlozi linku do lightmapy
*/
int ray_vloz_linku(LINKA_TEXTURY ** p_linka, LINKA_TEXTURY ** p_volne_linky,
  LIGHTMAP_INFO * p_mapa, int max_x_surface, int max_y_surface)
{
  LINKA_TEXTURY *p_nova;
  LINKA_TEXTURY *p_tmp, *p_tmp2, *p_tmp3;
  int zbyva_bodu;               // x zbyvajicich bodu do konce radku
  int radek_vyska;              // vyska zacatku radku v pixelech
  int radek;                    // cislo aktualniho radku
  int best_fit = -1;
  int best_score = 0x0fffffff, score;

  /* Alokace pameti na novou polozku v seznamu pod-textur
   */
  if (!(p_nova = (LINKA_TEXTURY *) malloc(sizeof(LINKA_TEXTURY))))
    chyba("Pamet!");
  p_nova->dx = p_mapa->dx;
  p_nova->dy = p_mapa->dy;
  p_nova->indicie = p_mapa->indicie;
  p_nova->p_dalsi = NULL;

  /* Je to prvni textura lightmapy
   */
  if (!*p_linka) {              // insert-first
    SetRect(&p_nova->r, 0, 0, p_mapa->dx, p_mapa->dy);
    p_nova->radek = 0;
    p_nova->vyska_radku = p_nova->dy;
    *p_linka = p_nova;
    return (TRUE);
  }
  else {

    /* Hledam jestli neni misto v dirach...
     */
    // zkusit hledat v pribuznych lightmapach ? - pribuzne
    // sou na vedlejsich poly
    p_tmp = *p_volne_linky;
    radek = 0;
    if (p_tmp) {
      // test prvni
      while (p_tmp) {
        //p_mapa->dx  p_mapa->dy
        if (p_tmp->dx >= p_mapa->dx && p_tmp->dy >= p_mapa->dy) {
          score = (p_tmp->dy - p_mapa->dy) * 1000 + (p_tmp->dx - p_mapa->dx);
          if (score < best_score) {
            best_score = score;
            best_fit = radek;
          }
        }
        p_tmp = p_tmp->p_dalsi;
        radek++;
      }

      if (best_fit != K_CHYBA) {

        // najiti diry
        p_tmp = *p_volne_linky;

        if (!best_fit) {        // best-fit je 0 - prvni
          ray_vloz_volnou_linku(p_linka, p_nova,
            p_tmp->r.left,
            p_tmp->r.top,
            p_tmp->r.left + p_nova->dx, p_tmp->r.top + p_nova->dy);

          *p_volne_linky = (*p_volne_linky)->p_dalsi;

          if (best_score) {
            if (p_tmp->dx > p_nova->dx) {       // pridej linku do konce radku              
              ray_vloz_volnou_linku_all(p_volne_linky,
                p_nova->r.right,
                p_nova->r.top, max_x_surface, p_nova->r.bottom, 0);
            }
            if (p_tmp->dy > p_nova->dy) {       // pridej top-linku              
              ray_vloz_volnou_linku_all(p_volne_linky,
                p_nova->r.left,
                p_nova->r.bottom, max_x_surface, p_tmp->r.bottom, 0);
            }
          }

          free(p_tmp);
          return (TRUE);

        }
        else {

          radek = 1;
          while (p_tmp->p_dalsi) {
            if (radek == best_fit) {
              p_tmp2 =
                ray_vloz_volnou_linku_all(p_linka, p_tmp->p_dalsi->r.left,
                p_tmp->p_dalsi->r.top, p_tmp->p_dalsi->r.left + p_nova->dx,
                p_tmp->p_dalsi->r.top + p_nova->dy, p_nova->indicie);

              if (best_score) { // je tam dira - vyrob dalsi volne linky
                p_tmp3 = p_tmp; // zazalohuju aktualni tmp
                p_tmp = p_tmp->p_dalsi;

                if (p_tmp->dx > p_tmp2->dx) {   // pridej linku do konce radku
                  ray_vloz_volnou_linku_all(p_volne_linky,
                    p_tmp2->r.right,
                    p_tmp2->r.top, p_tmp->r.right, p_tmp2->r.bottom, 0);
                }
                if (p_tmp->dy > p_tmp2->dy) {   // pridej top-linku
                  ray_vloz_volnou_linku_all(p_volne_linky,
                    p_tmp->r.left,
                    p_tmp2->r.bottom, p_tmp->r.right, p_tmp->r.bottom, 0);
                }
                p_tmp = p_tmp3; // obnovim puvodni p_tmp                
              }

              p_tmp2 = p_tmp->p_dalsi;
              p_tmp->p_dalsi = p_tmp->p_dalsi->p_dalsi;
              free(p_tmp2);
              return (TRUE);
            }

            p_tmp = p_tmp->p_dalsi;
            radek++;
          }
        }
      }
    }


    /* Najdu konec a hledam umisteni...
     */
    zbyva_bodu = max_x_surface;
    radek = 0;
    radek_vyska = 0;            // zacatek radku v pixelech

    p_tmp = (*p_linka);         // zacnu od zacatku...
    while (p_tmp && p_tmp->radek == K_CHYBA) {
      p_tmp = p_tmp->p_dalsi;
    }
    zbyva_bodu = max_x_surface - p_tmp->dx;

    do {

      /* Preteceni vysky lightmapy
       */
      if (radek_vyska + p_mapa->dy > max_y_surface) {
        free(p_nova);
        p_nova = NULL;
        return (FALSE);
      }


      /* Vlozeni za soucastnou lightmapu na konec radku
         nebo do mezery ktera je do konce radku
       */
      if (zbyva_bodu >= p_mapa->dx && (p_tmp->p_dalsi == NULL ||        // v pripade konce radku
          p_tmp->p_dalsi->radek > radek)) {     // v pripade ze dalsi lightmapa 
        // je na dalsim radku
        // max vyska je vyska predchozi textury

        assert(p_tmp->vyska_radku >= p_mapa->dy);

        p_nova->radek = radek;
        p_nova->vyska_radku = p_tmp->vyska_radku;

        p_nova->p_dalsi = p_tmp->p_dalsi;       // insert-first
        p_tmp->p_dalsi = p_nova;

        // rect nove
        SetRect(&p_nova->r, max_x_surface - zbyva_bodu,
          radek_vyska,
          max_x_surface - zbyva_bodu + p_nova->dx, radek_vyska + p_nova->dy);

        // pridam do derovniku volne diry
        if (p_mapa->dy < p_tmp->dy) {
          ray_vloz_volnou_linku_all(p_volne_linky,
            max_x_surface - zbyva_bodu,
            radek_vyska + p_mapa->dy,
            max_x_surface, radek_vyska + p_tmp->dy, 0);
        }
        return (TRUE);
      }

      // Vytvoreni noveho radku + pridani na konec
      if (zbyva_bodu < p_mapa->dx && p_tmp->p_dalsi == NULL) {
        radek_vyska += p_tmp->vyska_radku;

        // Radek presahl vysku lightmapy
        if (radek_vyska + p_nova->dy > max_y_surface) {
          free(p_nova);
          return (FALSE);
        }

        // Vlozeni lightmapy na zacatek noveho radku
        p_nova->radek = ++radek;
        p_nova->vyska_radku = p_mapa->dy;

        SetRect(&p_nova->r, 0, radek_vyska, p_nova->dx,
          radek_vyska + p_nova->dy);

        p_nova->p_dalsi = NULL;
        p_tmp->p_dalsi = p_nova;
        return (TRUE);
      }

      // pokracovani v prohledavani
      if (p_tmp->p_dalsi) {

        // mezi-radkova korekce
        while (p_tmp->p_dalsi && p_tmp->p_dalsi->radek == K_CHYBA) {
          p_tmp = p_tmp->p_dalsi;
        }

        // prechod na dalsi radek
        if (p_tmp->p_dalsi->radek > radek) {
          radek_vyska += p_tmp->vyska_radku;
          zbyva_bodu = max_x_surface - p_tmp->p_dalsi->dx;
          radek = p_tmp->p_dalsi->radek;
        }
        else
          zbyva_bodu -= p_tmp->p_dalsi->dx;

        // pokracovani v soucastnem
        p_tmp = p_tmp->p_dalsi;
      }
      else {
        free(p_nova);
        return (FALSE);         // konec seznamu
      }
    } while (p_tmp);
    free(p_nova);
    p_nova = NULL;
    return (FALSE);             // uz se tam light-mapa nevleze
  }
}

int ray_statistika_linek(LINKA_TEXTURY ** p_linka, int *p_x, int *p_y)
{
  LINKA_TEXTURY *p_tmp;
  int dx = 0, dy = 0;

  if (!*p_linka)
    return (FALSE);

  p_tmp = *p_linka;
  while (p_tmp) {
    if (dy < p_tmp->r.bottom)
      dy = p_tmp->r.bottom;
    if (dx < p_tmp->r.right)
      dx = p_tmp->r.right;
    p_tmp = p_tmp->p_dalsi;
  }

  *p_x = dx;
  *p_y = dy;

  return (TRUE);
}

void ray_free_linky(LINKA_TEXTURY * p_linka)
{
  if (p_linka) {
    ray_free_linky(p_linka->p_dalsi);
    free(p_linka);
    p_linka = NULL;
  }
}

/*
*/
LINKA_TEXTURY **ray_vyrob_poly_lightmap(POLY_INDICIE * p_ind, int indnum,
  EDIT_MESH_POLY * p_poly, int *p_linek,
  LIGHTMAP_INFO * p_mapy, int max_x_surface, int max_y_surface)
{
  LINKA_TEXTURY **p_linka;
  LINKA_TEXTURY *p_linka_volna = NULL, *p_tmp;
  int linek;
  int i;
  int umistenych;
  int umistenych_celkem;

  p_linka = (LINKA_TEXTURY **) malloc(sizeof(LINKA_TEXTURY *) * indnum);
  mtest(p_linka);
  memset(p_linka, 0, sizeof(LINKA_TEXTURY *) * indnum);

  /* Setridim lightmapy poly podle velikosti y,x
     (prednost ma y, potom x)
   */
  qsort(p_mapy, indnum, sizeof(LIGHTMAP_INFO),
    ray_vyrob_poly_lightmap_compare);

  /*
     Optimalizace -> opakovane vkladami mensich textur
     do vetsich
   */
  for (i = 0; i < indnum; i++)
    p_mapy[i].umistena = FALSE;

  /* Dokud neumistim vsechny plosky-opakuj proceduru
   */
  umistenych_celkem = 0;
  linek = 0;

  while (umistenych_celkem != indnum) {

    umistenych = 0;
    for (i = 0; i < indnum; i++) {
      if (!p_mapy[i].umistena) {
        if (ray_vloz_linku(p_linka + linek, &p_linka_volna, p_mapy + i,
            max_x_surface, max_y_surface)) {
          p_mapy[i].umistena = TRUE;
          umistenych++;
          umistenych_celkem++;
        }
      }
    }
    if (!umistenych) {
      //free lin seznam p_linka_volna
      linek++;
    }
  }
  linek++;

  *p_linek = linek;

  /* Uvolneni volnych linek
   */
  while (p_linka_volna) {
    p_tmp = p_linka_volna;
    p_linka_volna = p_linka_volna->p_dalsi;
    free(p_tmp);
  }

  return (p_linka);
}

void ray_link_to_lightmap(EDIT_TEXT * p_dest, EDIT_TEXT * p_src,
  LINKA_TEXTURY * p_linka);
void ray_prepocitej_texture_koordinaty(EDIT_MESH_POLY * p_poly_list,
  POLY_INDICIE * p_ind, LINKA_TEXTURY * p_linka, int dest_x, int dest_y,
  EDIT_TEXT * p_text);



void ray_tiskni_linky(LINKA_TEXTURY * p_linka)
{
  if (p_linka) {
    kprintf(TRUE, "indicie %d -> (%d %d %d %d)", p_linka->indicie,
      p_linka->r.left, p_linka->r.top, p_linka->r.right, p_linka->r.bottom);
    ray_tiskni_linky(p_linka->p_dalsi);
  }
}


void ray_slep_lightmapy(K_EDITOR * p_cnf, POLY_INDICIE * p_ind, int indnum,
  EDIT_TEXT * p_src)
{
  LIGHTMAP_INFO *p_mapy;
  LINKA_TEXTURY **p_linky;
  int l, x, y, dx, dy, t;
  int linek;
  int i;

  if (!(p_mapy = (LIGHTMAP_INFO *) _alloca(sizeof(LIGHTMAP_INFO) * indnum)))
    chyba("Pamet");

  for (i = 0; i < indnum; i++) {
    bmp_cti_rozmery(p_src[i].p_bmp, &p_mapy[i].dx, &p_mapy[i].dy);
    p_mapy[i].material = p_cnf->p_poly[p_ind[i].poly].material;
    p_mapy[i].poly = p_ind[i].poly;
    p_mapy[i].f = p_ind[i].face / 3;
    p_mapy[i].indicie = i;
  }


  // vyrobi seznam lightmap pro tento poly
  p_linky = ray_vyrob_poly_lightmap(p_ind, indnum, p_cnf->p_poly,
    &linek, p_mapy, p_cnf->ray.max_x_lepic, p_cnf->ray.max_y_lepic);

  // preved to do oficialnich light-map
  for (l = 0; l < linek; l++) {
    if (ray_statistika_linek(p_linky + l, &x, &y)) {
      if ((t =
          lo_najdi_volnou_texturu(p_cnf->p_lightmap,
            MAX_RAY_TEXTUR)) == K_CHYBA)
        chyba("Neni volna texutra !");

      for (dx = 2; dx < x; dx <<= 1);
      for (dy = 2; dy < y; dy <<= 1);

      if ((p_cnf->p_lightmap[t].p_bmp = bmp_vyrob(dx, dy)) == NULL)
        chyba("Textura");
      bmp_smaz(p_cnf->p_lightmap[t].p_bmp, BLBA_BARVA2);

      sprintf(p_cnf->p_lightmap[t].jmeno, "light%d", t);

      ray_link_to_lightmap(p_cnf->p_lightmap + t, p_src, p_linky[l]);
      ray_prepocitej_texture_koordinaty(p_cnf->p_poly, p_ind, p_linky[l], dx,
        dy, p_cnf->p_lightmap + t);

      ray_free_linky(p_linky[l]);
    }
  }

  free(p_linky);
  p_linky = NULL;

}

void ray_link_to_lightmap(EDIT_TEXT * p_dest, EDIT_TEXT * p_src,
  LINKA_TEXTURY * p_linka)
{
  // pridej akt bunku
  // cil = p_dest + rec
  // src = p_src[face]
  if (p_linka) {
    ray_link_to_lightmap(p_dest, p_src, p_linka->p_dalsi);
    bmp_kopiruj_rec(p_src[p_linka->indicie].p_bmp, p_dest->p_bmp,
      p_linka->r.left, p_linka->r.top);
  }
}

// in je rect uvnitr
void ray_prepocitej_texture_koordinaty(EDIT_MESH_POLY * p_poly_list,
  POLY_INDICIE * p_ind,
  LINKA_TEXTURY * p_linka, int dest_x, int dest_y, EDIT_TEXT * p_text)
{
  EDIT_MESH_POLY *p_poly;
  float x_start, y_start, dx, dy;
  int f, ind;

  if (p_linka) {
    ray_prepocitej_texture_koordinaty(p_poly_list, p_ind, p_linka->p_dalsi,
      dest_x, dest_y, p_text);

    x_start = (float) p_linka->r.left / (float) dest_x;
    y_start = (float) p_linka->r.top / (float) dest_y;

    dx = (float) (p_linka->r.right - p_linka->r.left) / (float) dest_x;
    dy = (float) (p_linka->r.bottom - p_linka->r.top) / (float) dest_y;

    ind = p_linka->indicie;
    p_poly = p_poly_list + p_ind[ind].poly;
    f = p_ind[ind].face;

    p_poly->p_koord[f].tul = dx * p_poly->p_koord[f].tul + x_start;
    p_poly->p_koord[f].tvl = dy * p_poly->p_koord[f].tvl + y_start;
    p_poly->p_koord[f + 1].tul = dx * p_poly->p_koord[f + 1].tul + x_start;
    p_poly->p_koord[f + 1].tvl = dy * p_poly->p_koord[f + 1].tvl + y_start;
    p_poly->p_koord[f + 2].tul = dx * p_poly->p_koord[f + 2].tul + x_start;
    p_poly->p_koord[f + 2].tvl = dy * p_poly->p_koord[f + 2].tvl + y_start;

    p_poly->p_light[f / 3] = p_text;
  }
}

/****************************************************************************
  Staticke-bodove svetla
 ****************************************************************************
*/
int oe_vloz_staticke_bodove_svetlo(K_EDITOR * p_cnf, STATIC_LIGHT * p_light)
{
  p_cnf->light[p_light->index] = *p_light;
  p_cnf->light[p_light->index].akt = TRUE;
  p_cnf->light[p_light->index].flag = POSUN_VSE;
  //ddw("Svetlo %s na %d upraveno.",p_light->jmeno,p_light->index);
  return (p_light->index);
}

int oe_smaz_staticke_bodove_svetlo(K_EDITOR * p_cnf, STATIC_LIGHT * p_light)
{
  int li = p_light->index;

  p_cnf->light[li].akt = FALSE;

  if (p_cnf->light[li].p_flare) {
    oe_smaz_flare(p_cnf, p_cnf->light[li].p_flare);
  }

  if (p_light->flag2 & SL2_MESH) {
    if (!p_light->p_prev) {     // mazu prvni svetlo kontejneru
      EDIT_KONTEJNER *p_kont = (EDIT_KONTEJNER *) p_light->p_mesh_data;

      assert(p_light == p_kont->p_slight);
      p_kont->p_slight = p_light->p_next;
      if (p_light->p_next)
        p_light->p_next->p_prev = NULL;
    }
    else {
      p_light->p_prev->p_next = p_light->p_next;
      if (p_light->p_next)
        p_light->p_next->p_prev = p_light->p_prev;
    }
  }

  memset(p_light, 0, sizeof(p_light[0]));
  return (TRUE);
}

int oe_najdi_volne_stat_svetlo(K_EDITOR * p_cnf)
{
  int i;

  for (i = 0; i < MAX_EDIT_SVETEL; i++) {
    if (!p_cnf->light[i].akt)
      return (i);
  }
  return (K_CHYBA);
}

void oe_prikaz_pridej_staticke_svetlo(K_EDITOR * p_cnf)
{
  int l = oe_najdi_volne_stat_svetlo(p_cnf);
  byte pom[200];
  int k = p_cnf->vybr_kont;
  STATIC_LIGHT light, *p_light;

  if (!p_cnf->cnf.svetla_sceny) {
    n_kont(k);
  }

  memset(&light, 0, sizeof(light));
  light.dosah = 10000.0f;
  light.index = l;
  light.r = light.g = light.b = 1.0f;
  light.k1 = 1;
  light.uhel = light.utlum = 1;
  light.p_flare = NULL;
  light.akt = TRUE;
  sprintf(light.jmeno, "Svetlo %d", l);

  if (p_cnf->cnf.svetla_sceny) {
    strcpy(pom, "Pridat bodove svetlo sceny...");
  }
  else {
    sprintf(pom, "Pridat bodove svetlo kontejneru %s (%d)",
      p_cnf->p_kont[k]->jmeno, k);
  }

  if (doe_prikaz_bodove_svetlo(hwnd_aplikace, pom, &light)) {
    p_cnf->lightakt = oe_vloz_staticke_bodove_svetlo(p_cnf, &light);
    p_light = p_cnf->light + p_cnf->lightakt;
    if (p_cnf->cnf.svetla_dynamicke) {
      oe_prepocitat_stat_svetla(p_cnf);
      ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
    }
    if (!p_cnf->cnf.svetla_sceny) {
      p_light->flag2 = SL2_MESH;
      p_light->p_mesh_data = p_cnf->p_kont[k];
      p_light->p_next = p_cnf->p_kont[k]->p_slight;
      p_light->p_prev = NULL;
      p_cnf->p_kont[k]->p_slight = p_light;
      if (p_light->p_next) {
        p_light->p_next->p_prev = p_light;
      }
    }
  }
}

void oe_uprav_staticke_svetlo(K_EDITOR * p_cnf, STATIC_LIGHT * p_light)
{
  STATIC_LIGHT light = *p_light;
  byte pom[200];

  if (p_light->flag2 & SL2_MESH) {
    EDIT_KONTEJNER *p_kont = p_light->p_mesh_data;

    sprintf(pom, "Uprav bodove svetlo kontejneru %s...", p_kont->jmeno);
  }
  else {
    strcpy(pom, "Uprav bodove svetlo sceny...");
  }
  if (doe_prikaz_bodove_svetlo(hwnd_aplikace, pom, &light))
    oe_vloz_staticke_bodove_svetlo(p_cnf, &light);
}

void oe_prepocitat_stat_svetla(K_EDITOR * p_cnf)
{
  STATIC_LIGHT *p_light;
  int i;
  float dist, I, det, a1, a2;

  p_cnf->lightnum = 0;
  for (i = 0; i < MAX_EDIT_SVETEL; i++) {
    if (p_cnf->light[i].akt) {
      p_light = p_cnf->light + i;
      p_cnf->lightnum = i + 1;

      // posledni kontejner&poly je nic
      p_light->last_kont = p_light->last_poly = K_CHYBA;

      // prepocitam dosah podle utlumu
      if (p_light->utlum) {
        dist = p_light->dosah;
        I =
          1.0f / (p_light->k1 + p_light->k2 * dist +
          p_light->k3 * dist * dist);

        // nastavim dosah podle utlumu
        if (I < MIN_LIGHT) {
          det =
            p_light->k2 * p_light->k2 - 4 * p_light->k3 * (p_light->k1 -
            1.0f / MIN_LIGHT);
          a1 = (-p_light->k2 + sqrtf(det)) / (2 * p_light->k3);
          a2 = (-p_light->k2 - sqrtf(det)) / (2 * p_light->k3);

          if (a1 > dist || a1 < 0.0f)
            a1 = dist;
          if (a2 > dist || a2 < 0.0f)
            a2 = dist;

          p_light->dosah = (a2 > a1) ? a1 : a2;
        }
      }
    }
  }
}

/*******************************
 Ray-Casting
 *******************************

 Motto:

"To je vec, ktera je dnes uz naprosty standard.
 Vsechny gamesy, pokud vim, to takhle delaji.
 Pekne to popisuje Michael Abrash v Ramblings
 in Realtime (o vyvoji Quaka), posilam ti to PDFko.
 Jde to, ze kazdemu polygonu sceny udelas lightmapu
 tak, ze vytvoris nejakou pravidelnou sit o ne moc
 velkem poctu stejne vzdalenych uzlu, takovy grid
 rovnomerne napamovany na cely polygon. Z kazdeho
 toho uzlu pak vystrelis papresek ke vsem zdrojum
 svetla, kdyz se s nicim neprotne, tak je tam svetlo
 jinak tma. Muzes merit i vzdalenost od svetla a tim
 korigovat mnozstvi energie. Vysledek pak zprumerujes
 s nekolika okolnima uzlama a ziskas tak mekke stiny.
 Kdyz zvolis raster moc veliky, tak jsou mapy kvalitni
 ale zase zabiraji moc mista, takze se tezko rvou do
 akceleratoru. Je dobre zvolit mensi kompromis, pak se
 jsou sice ve vetsi vzdalenosti od svetla videt takove
 rozblurovane ctverce (viz. nektere levely v Quaku),
 ale to je dan pometovym narokum."

                                       ReDox^Movsd 
*/

void oe_uloz_screen(byte * p_file)
{
  /* Nacte HDC a ulozi to na disk
   */

}


/**************************************************************************
  Optimalizacni kvadratura
 *************************************************************************
*/
// analizuje scenu a vyplivne rozmery kvadrat. stromu
#define  X_BUNKA  2
#define  Y_BUNKA  2
#define  Z_BUNKA  2

void ray_analizuj_scenu(K_EDITOR * p_cnf, BOD * p_start, BOD * p_bunek,
  BOD * p_roz)
{
  EDIT_MESH_POLY *p_poly;
  BOD kon = { -FLT_MAX, -FLT_MAX, -FLT_MAX }, zac = {
  FLT_MAX, FLT_MAX, FLT_MAX}, a;
  int p, f;

  for (p = 0; p < p_cnf->polynum; p++) {
    p_poly = p_cnf->p_poly + p;
    for (f = 0; f < p_poly->facenum; f++) {
      a.x = p_poly->p_koord[f].x;
      a.y = p_poly->p_koord[f].y;
      a.z = p_poly->p_koord[f].z;

      if (a.x < zac.x)
        zac.x = a.x;
      if (a.y < zac.y)
        zac.y = a.y;
      if (a.z < zac.z)
        zac.z = a.z;

      if (a.x > kon.x)
        kon.x = a.x;
      if (a.y > kon.y)
        kon.y = a.y;
      if (a.z > kon.z)
        kon.z = a.z;
    }
  }

  *p_start = zac;
  p_bunek->x =
    (float) ceil((2.0f * ceil(kon.x - zac.x)) / (float) p_roz->x) + 1;
  p_bunek->y =
    (float) ceil((2.0f * ceil(kon.y - zac.y)) / (float) p_roz->y) + 1;
  p_bunek->z =
    (float) ceil((2.0f * ceil(kon.z - zac.z)) / (float) p_roz->z) + 1;
}


int ray_bod_do_qbod(QUAD_TREE * p_qtree, QBOD * p_q, BOD * p_b)
{
  p_q->x = (int) floor((p_b->x - p_qtree->st.x) / p_qtree->bunka.x);
  p_q->y = (int) floor((p_b->y - p_qtree->st.y) / p_qtree->bunka.y);
  p_q->z = (int) floor((p_b->z - p_qtree->st.z) / p_qtree->bunka.z);

  if (p_q->x >= 0 && p_q->x < p_qtree->rad.x &&
    p_q->y >= 0 && p_q->y < p_qtree->rad.y &&
    p_q->z >= 0 && p_q->z < p_qtree->rad.z)
    return (TRUE);
  else
    return (FALSE);

}

BOD *ray_interpoluj_face_bodama(BOD * p_face, int *p_bodu)
{
  BOD q1, q2, h1, h2, hq, *p_list;
  int i, j, bodu, lbodu;
  int cbodu;
  float k1, k2, t, dt, ht, hdt;
  float krok = 0.05f;

  k1 = vzdal_bodu_bod(p_face, p_face + 2);
  k2 = vzdal_bodu_bod(p_face + 1, p_face + 2);

  bodu = (int) ceil((k1 < k2) ? k2 / krok : k1 / krok);
  bodu++;

  p_list = malloc(sizeof(BOD) * bodu * bodu);
  if (!p_list)
    chyba("Pamet!");
  cbodu = 0;

  // t je parametr od 0 do 1
  calc_primku_3d(p_face, p_face + 2, &q1);
  calc_primku_3d(p_face + 1, p_face + 2, &q2);
  // P = d + q*t
  // t = 0-1 -> po k1 krocich

  t = 0;
  dt = 1.0f / (float) (bodu - 1);
  for (i = 0; i < bodu; i++) {
    // d[0] -> d[2]
    h1.x = p_face[0].x + q1.x * t;
    h1.y = p_face[0].y + q1.y * t;
    h1.z = p_face[0].z + q1.z * t;

    h2.x = p_face[1].x + q2.x * t;
    h2.y = p_face[1].y + q2.y * t;
    h2.z = p_face[1].z + q2.z * t;
    t += dt;

    // 2 body a mezi nema interpolovat
    // h1 a h2
    lbodu = (int) ceil(vzdal_bodu_bod(&h1, &h2) / krok);
    lbodu++;
    calc_primku_3d(&h1, &h2, &hq);

    ht = 0.0f;
    hdt = 1.0f / ((float) (lbodu - 1));
    for (j = 0; j < lbodu; j++) {
      p_list[cbodu].x = (h1.x + hq.x * ht);
      p_list[cbodu].y = (h1.y + hq.y * ht);
      p_list[cbodu].z = (h1.z + hq.z * ht);
      ht += hdt;
      cbodu++;
    }
  }
  *p_bodu = cbodu;
  return (p_list);
}
