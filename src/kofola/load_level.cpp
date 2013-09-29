//------------------------------------------------------------------------------------------------
// version 0.0.6
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <assert.h>
#include <math.h>
#include <time.h>

#include "3d_all.h"
#include "Berusky_universal.h"
#include "load_level.h"
#include "game_logic.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "trigers.h"
#include "grammar.h"
#include "menu_script.h"
#include "animationk.h"
#include "profiles.h"

#ifdef LINUX
#include <dirent.h>
#include <fnmatch.h>
#endif

#define VELIKOST_STREPIN	2.0f
#define VELIKOST_STREPIN_K	20.0f
#define VELIKOST_STREPINPR	2.5f
#define VELIKOST_STREPINKAM	4.5f
#define VELIKOST_STREPINJIS	7.0f
#define VELIKOST_STREPINV1 15.0f
#define VELIKOST_STREPINL   3.0f

#define randf()      ((float)rand())

extern int Xresolution;
extern int Yresolution;
extern int iActualScene;
extern int iActualLevel;
extern PLAYER_PROFILE pPlayerProfile;

#define RWBUFF_VEL 60000

int lsi_file_copy(char *p_src, char *p_out)
{
  FILE *in, *out;
  char *p_buffer;
  int nc;

  in = fopen(p_src, "rb");
  if (!in)
    return (FALSE);
  out = fopen(p_out, "wb");
  if (!out) {
    fclose(in);
    return (FALSE);
  }

  p_buffer = (char *) malloc(RWBUFF_VEL);
//  mtest(p_buffer);

  while ((nc = fread(p_buffer, 1, RWBUFF_VEL, in))) {
    fwrite(p_buffer, 1, nc, out);
  }

  free(p_buffer);
  fclose(in);
  fclose(out);
  return (TRUE);
}

//------------------------------------------------------------------------------------------------
// Find Object with selected GUID and returns it's index
//------------------------------------------------------------------------------------------------
int lsi_Find_Object(int Guid, LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Count_Of_Objects; i++)
    if (p_Level->Object[i].GUID == Guid)
      return i;

  return -1;
}

//------------------------------------------------------------------------------------------------
// Find Object with selected GUID and returns it's index
//------------------------------------------------------------------------------------------------
int lsi_Find_Wall_Object(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Count_Of_Objects; i++)
    if (p_Level->Object[i].Class == 2)
      return i;

  return -1;
}

char lsi_Create_Explosions(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j;
  float bu, bv, v;
  SYSTEMCASTIC *pSystem;

  for (i = 0; i < 20; i++) {
    p_Level->Exploze[i].Sizeof = PocetCastic;
    p_Level->Exploze[i].pCastice =
      (PAR_STREPINA *) malloc(PocetCastic * sizeof(PAR_STREPINA));

    if (!p_Level->Exploze[i].pCastice)
      return 0;

    pSystem = &p_Level->Exploze[i];
    p_Level->Exploze[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      pSystem->pCastice[j].p.x = rand() & 0x1 ? randf() : -randf();
      pSystem->pCastice[j].p.y = randf() * 1.25f + 0.5f;
      pSystem->pCastice[j].p.z = rand() & 0x1 ? randf() : -randf();
      pSystem->pCastice[j].vel1 =
        randf() / (VELIKOST_STREPIN * (float) RAND_MAX);
      pSystem->pCastice[j].vel2 =
        randf() / (VELIKOST_STREPIN * (float) RAND_MAX);
      pSystem->pCastice[j].vel3 =
        randf() / (VELIKOST_STREPIN * (float) RAND_MAX);
      pSystem->pCastice[j].vel4 =
        randf() / (VELIKOST_STREPIN * (float) RAND_MAX);
      v =
        (pSystem->pCastice[j].vel1 + pSystem->pCastice[j].vel2 +
        pSystem->pCastice[j].vel3 + pSystem->pCastice[j].vel4) / 2.0f;

      pSystem->pCastice[j].rychlost =
        ((randf() / (float) (RAND_MAX)) + v * v) * _3DKOREKCE;
      pSystem->pCastice[j].g = 0.15f;
      pSystem->pCastice[j].utlum = 4.0f * v;
      pSystem->pCastice[j].vaha = 1.3f * v * _3DKOREKCE;
      pSystem->pCastice[j].barva = randf() / ((float) RAND_MAX);
      bu = randf() / (float) (RAND_MAX);
      bv = randf() / (float) (RAND_MAX);
      pSystem->pCastice[j].u1 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v1 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u2 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v2 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u3 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v3 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u4 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v4 = bv + randf() / (2 * (float)RAND_MAX);
      vektor_norm(&pSystem->pCastice[j].p);
      pSystem->pCastice[j].dir = pSystem->pCastice[j].p;
    }
  }

  return 1;
}

char lsi_Create_Krompac(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j;
  float bu, bv, v;
  SYSTEMCASTIC *pSystem;

  for (i = 0; i < 10; i++) {
    p_Level->Krompac[i].Sizeof = PocetCastic;
    p_Level->Krompac[i].pCastice =
      (PAR_STREPINA *) malloc(PocetCastic * sizeof(PAR_STREPINA));

    if (!p_Level->Krompac[i].pCastice)
      return 0;

    pSystem = &p_Level->Krompac[i];
    p_Level->Krompac[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      pSystem->pCastice[j].p.x = (rand() & 0x1 ? randf() : -randf()) * 0.25f;
      pSystem->pCastice[j].p.y = randf() * 0.1f + 2.05f;
      pSystem->pCastice[j].p.z = (rand() & 0x1 ? randf() : -randf()) * 0.25f;
      pSystem->pCastice[j].vel1 =
        randf() / (VELIKOST_STREPIN_K * (float) RAND_MAX);
      pSystem->pCastice[j].vel2 =
        randf() / (VELIKOST_STREPIN_K * (float) RAND_MAX);
      pSystem->pCastice[j].vel3 =
        randf() / (VELIKOST_STREPIN_K * (float) RAND_MAX);
      pSystem->pCastice[j].vel4 =
        randf() / (VELIKOST_STREPIN_K * (float) RAND_MAX);
      v =
        (pSystem->pCastice[j].vel1 + pSystem->pCastice[j].vel2 +
        pSystem->pCastice[j].vel3 + pSystem->pCastice[j].vel4) / 2.0f;

      pSystem->pCastice[j].rychlost =
        ((randf() / (float) (RAND_MAX)) + v * v) * _3DKOREKCE;
      pSystem->pCastice[j].g = 0.15f;
      pSystem->pCastice[j].utlum = 4.0f * v;
      pSystem->pCastice[j].vaha = 1.3f * v * _3DKOREKCE;
      pSystem->pCastice[j].barva = randf() / ((float) RAND_MAX);
      bu = randf() / (float) (RAND_MAX);
      bv = randf() / (float) (RAND_MAX);
      pSystem->pCastice[j].u1 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v1 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u2 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v2 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u3 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v3 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u4 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v4 = bv + randf() / (2 * (float)RAND_MAX);
      vektor_norm(&pSystem->pCastice[j].p);
      pSystem->pCastice[j].dir = pSystem->pCastice[j].p;
    }
  }

  return 1;
}

char lsi_Create_Jiskry(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j;
  float bu, bv, v;
  SYSTEMCASTIC *pSystem;

  for (i = 0; i < 20; i++) {
    p_Level->Jiskra[i].Sizeof = PocetCastic;
    p_Level->Jiskra[i].pCastice =
      (PAR_STREPINA *) malloc(PocetCastic * sizeof(PAR_STREPINA));

    if (!p_Level->Jiskra[i].pCastice)
      return 0;

    pSystem = &p_Level->Jiskra[i];
    p_Level->Jiskra[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      pSystem->pCastice[j].p.x = rand() & 0x1 ? randf() : -randf();
      pSystem->pCastice[j].p.y = randf() * 1.25f + 0.5f;
      pSystem->pCastice[j].p.z = rand() & 0x1 ? randf() : -randf();
      pSystem->pCastice[j].vel1 =
        randf() / (VELIKOST_STREPINJIS * (float) RAND_MAX);
      pSystem->pCastice[j].vel2 =
        randf() / (VELIKOST_STREPINJIS * (float) RAND_MAX);
      pSystem->pCastice[j].vel3 =
        randf() / (VELIKOST_STREPINJIS * (float) RAND_MAX);
      pSystem->pCastice[j].vel4 =
        randf() / (VELIKOST_STREPINJIS * (float) RAND_MAX);
      v =
        (pSystem->pCastice[j].vel1 + pSystem->pCastice[j].vel2 +
        pSystem->pCastice[j].vel3 + pSystem->pCastice[j].vel4) / 2.0f;

      pSystem->pCastice[j].rychlost =
        ((randf() / (float) (RAND_MAX)) + v * v) * _3DKOREKCE;
      pSystem->pCastice[j].g = 0.15f;
      pSystem->pCastice[j].utlum = 4.0f * v;
      pSystem->pCastice[j].vaha = (1.3f * v) * _3DKOREKCE;
      pSystem->pCastice[j].barva = 1.0f;
      bu = randf() / (float) (RAND_MAX);
      bv = randf() / (float) (RAND_MAX);
      pSystem->pCastice[j].u1 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v1 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u2 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v2 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u3 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v3 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u4 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v4 = bv + randf() / (2 * (float)RAND_MAX);
      vektor_norm(&pSystem->pCastice[j].p);
      pSystem->pCastice[j].dir = pSystem->pCastice[j].p;
    }
  }

  return 1;
}

char lsi_Create_Trap_Doors(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j;
  float bu, bv, v;
  SYSTEMCASTIC *pSystem;

  for (i = 0; i < 20; i++) {
    p_Level->Propadla[i].Sizeof = PocetCastic;
    p_Level->Propadla[i].pCastice =
      (PAR_STREPINA *) malloc(PocetCastic * sizeof(PAR_STREPINA));

    if (!p_Level->Propadla[i].pCastice)
      return 0;

    pSystem = &p_Level->Propadla[i];
    p_Level->Propadla[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      pSystem->pCastice[j].p.x = rand() & 0x1 ? randf() : -randf();
      pSystem->pCastice[j].p.y = randf() * (-1.5f);
      pSystem->pCastice[j].p.z = rand() & 0x1 ? randf() : -randf();
      pSystem->pCastice[j].vel1 =
        randf() / (VELIKOST_STREPINPR * (float) RAND_MAX);
      pSystem->pCastice[j].vel2 =
        randf() / (VELIKOST_STREPINPR * (float) RAND_MAX);
      pSystem->pCastice[j].vel3 =
        randf() / (VELIKOST_STREPINPR * (float) RAND_MAX);
      pSystem->pCastice[j].vel4 =
        randf() / (VELIKOST_STREPINPR * (float) RAND_MAX);
      v =
        (pSystem->pCastice[j].vel1 + pSystem->pCastice[j].vel2 +
        pSystem->pCastice[j].vel3 + pSystem->pCastice[j].vel4) / 2.0f;

      pSystem->pCastice[j].rychlost = (0.01f) * _3DKOREKCE;
      pSystem->pCastice[j].g = 0.15f;
      pSystem->pCastice[j].utlum = v;
      pSystem->pCastice[j].vaha = v * 0.25f * _3DKOREKCE;
      pSystem->pCastice[j].barva = 1.0f;
      bu = randf() / (float) (RAND_MAX);
      bv = randf() / (float) (RAND_MAX);
      pSystem->pCastice[j].u1 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v1 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u2 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v2 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u3 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v3 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u4 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v4 = bv + randf() / (2 * (float)RAND_MAX);
      vektor_norm(&pSystem->pCastice[j].p);
      pSystem->pCastice[j].dir = pSystem->pCastice[j].p;
    }
  }

  return 1;
}

char lsi_Create_Rocks(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j;
  float bu, bv, v;
  SYSTEMCASTIC *pSystem;

  for (i = 0; i < 20; i++) {
    p_Level->Kamen[i].Sizeof = PocetCastic;
    p_Level->Kamen[i].pCastice =
      (PAR_STREPINA *) malloc(PocetCastic * sizeof(PAR_STREPINA));

    if (!p_Level->Kamen[i].pCastice)
      return 0;

    pSystem = &p_Level->Kamen[i];
    p_Level->Kamen[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      pSystem->pCastice[j].p.x =
        (rand() & 0x1 ? randf() : -randf()) / (float) RAND_MAX;
      pSystem->pCastice[j].p.y =
        ((rand() & 0x1 ? randf() : -randf()) / (float) RAND_MAX);

      if (pSystem->pCastice[j].p.y > 2)
        pSystem->pCastice[j].p.y = 2;

      pSystem->pCastice[j].p.z =
        (rand() & 0x1 ? randf() : -randf()) / (float) RAND_MAX;
      pSystem->pCastice[j].vel1 =
        randf() / (VELIKOST_STREPINKAM * (float) RAND_MAX);
      pSystem->pCastice[j].vel2 =
        randf() / (VELIKOST_STREPINKAM * (float) RAND_MAX);
      pSystem->pCastice[j].vel3 =
        randf() / (VELIKOST_STREPINKAM * (float) RAND_MAX);
      pSystem->pCastice[j].vel4 =
        randf() / (VELIKOST_STREPINKAM * (float) RAND_MAX);
      v =
        (pSystem->pCastice[j].vel1 + pSystem->pCastice[j].vel2 +
        pSystem->pCastice[j].vel3 + pSystem->pCastice[j].vel4) / 2.0f;

      pSystem->pCastice[j].rychlost = (v * 0.05f) * _3DKOREKCE;
      pSystem->pCastice[j].g = 0.05f;
      pSystem->pCastice[j].utlum = v;
      pSystem->pCastice[j].vaha = (v * 0.25f) * _3DKOREKCE;
      pSystem->pCastice[j].barva = 1.0f;
      bu = randf() / (float) (RAND_MAX);
      bv = randf() / (float) (RAND_MAX);
      pSystem->pCastice[j].u1 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v1 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u2 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v2 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u3 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v3 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u4 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v4 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].dir = pSystem->pCastice[j].p;
      vektor_norm(&pSystem->pCastice[j].dir);
    }
  }

  return 1;
}

char lsi_Create_Prach(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j;
  SYSTEMFLEKCASTIC *pSystem;

  for (i = 0; i < 20; i++) {
    p_Level->Prach[i].Sizeof = PocetCastic;
    p_Level->Prach[i].pCastice =
      (PAR_FLEK *) malloc(PocetCastic * sizeof(PAR_FLEK));

    if (!p_Level->Prach[i].pCastice)
      return 0;

    pSystem = &p_Level->Prach[i];
    p_Level->Prach[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      pSystem->pCastice[j].p.x = 0;
      pSystem->pCastice[j].p.y = -1;
      pSystem->pCastice[j].p.z = 0;
      pSystem->pCastice[j].nx.x = 1;
      pSystem->pCastice[j].nx.y = 0;
      pSystem->pCastice[j].nx.z = 0;
      pSystem->pCastice[j].ny.x = 0;
      pSystem->pCastice[j].ny.y = 0;
      pSystem->pCastice[j].ny.z = 1;

      pSystem->pCastice[j].rychlost_x = 1.0f;
      pSystem->pCastice[j].rychlost_y = 1.0f;
      pSystem->pCastice[j].utlum_x = 20.0f;
      pSystem->pCastice[j].utlum_y = 20.0f;

      pSystem->pCastice[j].r =
        pSystem->pCastice[j].g =
        pSystem->pCastice[j].b = 0.0f + randf() / (float) (RAND_MAX);

      pSystem->pCastice[j].a = 1.0f;
      pSystem->pCastice[j].dr =
        pSystem->pCastice[j].dg =
        pSystem->pCastice[j].db = pSystem->pCastice[j].r;
      pSystem->pCastice[j].da = -7.0f;
      pSystem->pCastice[j].ka = 0.0f;
    }
  }

  return 1;
}

char lsi_Create_Vodni_Cakance1(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j, rosa;
  float bu, bv, v;
  SYSTEMCASTIC *pSystem;

  for (i = 0; i < 10; i++) {
    p_Level->VodniCakanec1[i].Sizeof = PocetCastic;
    p_Level->VodniCakanec1[i].pCastice =
      (PAR_STREPINA *) malloc(PocetCastic * sizeof(PAR_STREPINA));

    if (!p_Level->VodniCakanec1[i].pCastice)
      return 0;

    pSystem = &p_Level->VodniCakanec1[i];
    p_Level->VodniCakanec1[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      rosa = rand() % 2;

      if (!rosa) {
        rosa = rand() % 2;

        if (!rosa)
          pSystem->pCastice[j].p.x = -1.1f;
        else
          pSystem->pCastice[j].p.x = 1.1f;

        pSystem->pCastice[j].p.z =
          (rand() & 0x1 ? randf() : -randf()) / (float) RAND_MAX;
      }
      else {
        rosa = rand() % 2;

        if (!rosa)
          pSystem->pCastice[j].p.z = -1.1f;
        else
          pSystem->pCastice[j].p.z = 1.1f;

        pSystem->pCastice[j].p.x =
          (rand() & 0x1 ? randf() : -randf()) / (float) RAND_MAX;
      }

      pSystem->pCastice[j].p.y = (randf() / (float) RAND_MAX) * 2.0f + 2.0f;

      pSystem->pCastice[j].vel1 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);
      pSystem->pCastice[j].vel2 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);
      pSystem->pCastice[j].vel3 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);
      pSystem->pCastice[j].vel4 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);

      v = (pSystem->pCastice[j].vel1 + pSystem->pCastice[j].vel2 +
        pSystem->pCastice[j].vel3 + pSystem->pCastice[j].vel4) / 2.0f;

      pSystem->pCastice[j].rychlost =
        (((randf() / (float) (RAND_MAX)) * v) * 8.0f) * _3DKOREKCE;

      if (pSystem->pCastice[j].rychlost < (0.1f) * _3DKOREKCE)
        pSystem->pCastice[j].rychlost = (0.1f) * _3DKOREKCE;

      pSystem->pCastice[j].g = 0.25f;
      pSystem->pCastice[j].utlum = ((2 * v) * 0.75f) * 5.0f;
      pSystem->pCastice[j].vaha = (((v) * 0.75f) * 6.0f) * _3DKOREKCE;
      pSystem->pCastice[j].barva = 1;
      bu = randf() / (float) (RAND_MAX);
      bv = randf() / (float) (RAND_MAX);
      pSystem->pCastice[j].u1 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v1 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u2 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v2 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u3 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v3 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u4 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v4 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].dir = pSystem->pCastice[j].p;
      vektor_norm(&pSystem->pCastice[j].dir);

      pSystem->pCastice[j].p.y = 0.1f;
    }
  }

  return 1;
}

char lsi_Create_Vodni_Cakance2(LEVELINFO * p_Level, int PocetCastic)
{
  int i, j;
  float bu, bv, v;
  SYSTEMCASTIC *pSystem;

  for (i = 0; i < 10; i++) {
    p_Level->VodniCakanec2[i].Sizeof = PocetCastic;
    p_Level->VodniCakanec2[i].pCastice =
      (PAR_STREPINA *) malloc(PocetCastic * sizeof(PAR_STREPINA));

    if (!p_Level->VodniCakanec2[i].pCastice)
      return 0;

    pSystem = &p_Level->VodniCakanec2[i];
    p_Level->VodniCakanec2[i].flag = -1;

    for (j = 0; j < PocetCastic; j++) {
      pSystem->pCastice[j].p.z =
        ((rand() & 0x1 ? randf() : -randf()) / (float) RAND_MAX) / 5.0f;
      pSystem->pCastice[j].p.x =
        ((rand() & 0x1 ? randf() : -randf()) / (float) RAND_MAX) / 5.0f;
      pSystem->pCastice[j].p.y = (randf() / (float) RAND_MAX) * 2.0f + 2.0f;

      pSystem->pCastice[j].vel1 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);
      pSystem->pCastice[j].vel2 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);
      pSystem->pCastice[j].vel3 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);
      pSystem->pCastice[j].vel4 =
        randf() / (VELIKOST_STREPINV1 * (float) RAND_MAX);

      v = (pSystem->pCastice[j].vel1 + pSystem->pCastice[j].vel2 +
        pSystem->pCastice[j].vel3 + pSystem->pCastice[j].vel4) / 2.0f;

      pSystem->pCastice[j].rychlost =
        (((randf() / (float) (RAND_MAX)) * v) * 8.0f) * _3DKOREKCE;

      if (pSystem->pCastice[j].rychlost < (0.1f) * _3DKOREKCE)
        pSystem->pCastice[j].rychlost = (0.1f) * _3DKOREKCE;

      pSystem->pCastice[j].g = 0.25f;
      pSystem->pCastice[j].utlum = ((2 * v) * 0.75f) * 5.0f;
      pSystem->pCastice[j].vaha = (((v) * 0.75f) * 6.0f) * _3DKOREKCE;
      pSystem->pCastice[j].barva = 1;
      bu = randf() / (float) (RAND_MAX);
      bv = randf() / (float) (RAND_MAX);
      pSystem->pCastice[j].u1 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v1 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u2 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v2 = bv - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u3 = bu - randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v3 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].u4 = bu + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].v4 = bv + randf() / (2 * (float)RAND_MAX);
      pSystem->pCastice[j].dir = pSystem->pCastice[j].p;
      vektor_norm(&pSystem->pCastice[j].dir);

      pSystem->pCastice[j].p.y = 0.1f;
    }
  }

  return 1;
}

void lsi_Load_Level_Script(LEVELINFO * p_Level, char *cFile)
{
  int i, mesh;  
  TRIGER_STRUCTURE ts;
  GAME_TRIGER gt;
  GRAMMAR gr;

  chdir(DATA_DIR);

  gr_Load_Grammar("lsc_grammar.txt", &gr);

  chdir(GAME_DATA_DIR);

  ts.LastStr = 0;
  ts.sizeofT = 0;
  ts.pTriger = NULL;

  trig_Load_Triger(cFile, &gt, &gr, &ts);

  for (i = 0; i < gt.lastcommand; i++)
    switch (gt.command[i].iCommand) {
      case 1:
        {
          if (gt.command[i].LastParam > 0 &&
            gt.command[i].Parametr[0].Type == 2) {
            mesh =
              kom_pridej_mesh_do_levelu(ts.StrTable[gt.command[i].Parametr[0].
                Value]);

            if (mesh != K_CHYBA)
              lani_set(mesh, 0, 0, &p_Level->TrashFlag, GK_LOOP, 0, 0);
          }
        }
        break;
      case 3:
        {
          float pos[3];
          int iStart;
          int iPause;

          if (gt.command[i].LastParam > 0) {
            memcpy(&iStart, &gt.command[i].Parametr[0].Value, sizeof(int));
            memcpy(&iPause, &gt.command[i].Parametr[1].Value, sizeof(int));
            memcpy(&pos[0], &gt.command[i].Parametr[2].Value, sizeof(float));
            memcpy(&pos[1], &gt.command[i].Parametr[3].Value, sizeof(float));
            memcpy(&pos[2], &gt.command[i].Parametr[4].Value, sizeof(float));

            am_Create_Street_Light(p_Level, iStart, iPause, pos);
          }
        }
        break;
      case 4:
        {
          float pos[3];
          float radius;

          if (gt.command[i].LastParam > 0) {
            memcpy(&radius, &gt.command[i].Parametr[0].Value, sizeof(float));
            memcpy(&pos[0], &gt.command[i].Parametr[1].Value, sizeof(float));
            memcpy(&pos[1], &gt.command[i].Parametr[2].Value, sizeof(float));
            memcpy(&pos[2], &gt.command[i].Parametr[3].Value, sizeof(float));

            am_Create_Water_CirclesB(p_Level, &p_Level->StreetRain, radius, 0,
              500, 0.25, pos);
          }
        }
        break;
      case 2:                  //Natureeffect
        {
          if (gt.command[i].LastParam > 0) {
            switch (gt.command[i].Parametr[0].Value) {
              case 0:
                {
                  float pos[3];

                  memcpy(&pos[0], &gt.command[i].Parametr[1].Value,
                    sizeof(float));
                  memcpy(&pos[1], &gt.command[i].Parametr[2].Value,
                    sizeof(float));
                  memcpy(&pos[2], &gt.command[i].Parametr[3].Value,
                    sizeof(float));

                  am_Create_BublSystem_Effect1(pos, p_Level, 750);
                }
                break;
              case 1:
                {
                  float pos[3];

                  memcpy(&pos[0], &gt.command[i].Parametr[1].Value,
                    sizeof(float));
                  memcpy(&pos[1], &gt.command[i].Parametr[2].Value,
                    sizeof(float));
                  memcpy(&pos[2], &gt.command[i].Parametr[3].Value,
                    sizeof(float));

                  am_Create_BublSystem_Effect2(pos, p_Level, 25, 50.0f);
                }
                break;
              case 2:
                {
                  float pos[3];

                  memcpy(&pos[0], &gt.command[i].Parametr[1].Value,
                    sizeof(float));
                  memcpy(&pos[1], &gt.command[i].Parametr[2].Value,
                    sizeof(float));
                  memcpy(&pos[2], &gt.command[i].Parametr[3].Value,
                    sizeof(float));

                  am_Create_BublSystem_Effect2(pos, p_Level, 25, 21.0f);
                }
                break;
            }
          }
        }
        break;
      case 5:
      case 6:
        {
          RECT r;

          if (gt.command[i].LastParam > 0) {
            memcpy(&r.left, &gt.command[i].Parametr[0].Value, sizeof(int));
            memcpy(&r.top, &gt.command[i].Parametr[1].Value, sizeof(int));
            memcpy(&r.right, &gt.command[i].Parametr[2].Value, sizeof(int));
            memcpy(&r.bottom, &gt.command[i].Parametr[3].Value, sizeof(int));

            if (gt.command[i].iCommand == 5)
              am_Gen_Swamp_Lights(p_Level, &r);
            else
              am_Create_Fairies(p_Level, &r);
          }
        }
        break;
      case 7:
        {
          char text[MAX_FILENAME];
          int iVal, j;

          if (gt.command[i].LastParam > 0) {
            memcpy(&iVal, &gt.command[i].Parametr[0].Value, sizeof(int));

            for (j = 0; j < iVal; j++) {
              sprintf(text, "f-vyrez%d", j);

              am_Start_Gen_Animation(text, p_Level);
            }
          }
        }
        break;
      case 8:
      case 9:
        {
          float rect[6];

          if (gt.command[i].LastParam > 0) {
            memcpy(&rect[0], &gt.command[i].Parametr[0].Value, sizeof(float));
            memcpy(&rect[1], &gt.command[i].Parametr[1].Value, sizeof(float));
            memcpy(&rect[2], &gt.command[i].Parametr[2].Value, sizeof(float));
            memcpy(&rect[3], &gt.command[i].Parametr[3].Value, sizeof(float));
            memcpy(&rect[4], &gt.command[i].Parametr[4].Value, sizeof(float));
            memcpy(&rect[5], &gt.command[i].Parametr[5].Value, sizeof(float));

            if (gt.command[i].iCommand == 8)
              am_Gen_Star_Lights(p_Level, rect);
            else
              am_Create_Falling_Stars(p_Level, rect);
          }
        }
        break;
      case 10:
        {
          if (gt.command[i].LastParam > 0) {
            float pos[3];

            memcpy(&pos[0], &gt.command[i].Parametr[0].Value, sizeof(float));
            memcpy(&pos[1], &gt.command[i].Parametr[1].Value, sizeof(float));
            memcpy(&pos[2], &gt.command[i].Parametr[2].Value, sizeof(float));

            am_Create_CandleSystem(pos, p_Level);
            am_Create_CandleSmokeSystem(pos, p_Level);
          }
        }
        break;
      case 11:
        {
          char text[MAX_FILENAME];
          int iVal, j;

          if (gt.command[i].LastParam > 0) {
            memcpy(&iVal, &gt.command[i].Parametr[0].Value, sizeof(int));

            for (j = 1; j < iVal; j++) {
              sprintf(text, "kytka%d", j);

              am_Start_Gen_Animation(text, p_Level);
            }
          }
        }
        break;
    }
}

int lsi_Create_Level_Raw(char *p_Level_Name, BUNKA_LEVELU_DISK ** b_l_d,
  int *size)
{
  LEVEL_HEADER l_h;
  char text[MAX_FILENAME + 1];
  FILE *file;

  kprintf(1, "Tvorba raw struktury pro komata...");
  getcwd(text, MAX_FILENAME);
  kprintf(1, "_getcwd = %s", text);

  file = fopen(p_Level_Name, "rb");
  if (!file) {
    chdir((text));
    sprintf(text, "Unable to find level %s", p_Level_Name);
    kprintf(1, text);
    return 0;
  }

  fread(&l_h, sizeof(LEVEL_HEADER), 1, file);

  (*size) = l_h.x * l_h.y * l_h.z * 2;

  (*b_l_d) =
    (BUNKA_LEVELU_DISK *) malloc((*size) * sizeof(BUNKA_LEVELU_DISK));

  if (!(*b_l_d)) {
    kprintf(1, "Nepodarilo se naalokovat pamet na raw strukturu pro komata");
    chdir((text));
    return 0;
  }

  fread((*b_l_d), sizeof(BUNKA_LEVELU_DISK), (*size), file);

  fclose(file);
  chdir((text));
  return 1;
}

//------------------------------------------------------------------------------------------------
// Loads level into memory
//------------------------------------------------------------------------------------------------
int lsi_Load_Level(char *p_Level_Name, LEVELINFO * p_Level)
{
  char text[256];
  FILE *file;
  int Vazba1, Vazba2, Rotation;
  int i, Guid, errors = 0, error;
  LEVEL_HEADER l_h;
  int x, y, z, pointer, act_pointer, ani_pointer;
  int kvalita_castic, kvalita_casticv;
  BUNKA_LEVELU_DISK b_l_d;
  int Next_Beetle = 0;
  char bVybusniny = 0;
  char bPodlaha = 0;
  char bKamen = 0;
  char bVoda = 0;
  char bNeviditelnaZed;

  p_Level->Sub_Svetla = 0;

  ZeroMemory(&b_l_d, sizeof(b_l_d));

  for (i = 0; i < 6; i++) {
    p_Level->Beetle[i] = -1;
    p_Level->BeetleAnim[i].Mesh = -1;
    p_Level->BeetleAnim[i].Flag = -1;
    p_Level->BeatleSmoke[i].iItem = -1;
    p_Level->BeatleSmoke[i].iStepc = 0;
    ZeroMemory(&p_Level->BeatleSmoke[i].sStopy,
      sizeof(SYSTEMKOUROVYCHCASTIC));
    p_Level->BeatleSmoke[i].sStopy.System = (size_ptr)NULL;
    p_Level->BeetleAnim[i].iAnimCount = 0;
    p_Level->BeetleAnim[i].iEffect = 0;
    p_Level->BeetleAnim[i].iEffectCounter = 0;
    p_Level->BeetleAnim[i].dwTime = timeGetTime();
  }

  getcwd(text, 255);

  kprintf(1, "Loading level from %s ...", text);
  
  kvalita_castic = GetPrivateProfileInt("hra", "kvalita_castic", 0, ini_file);
  kvalita_castic++;

  kvalita_casticv = GetPrivateProfileInt("hra", "kvalita_casticv", 0, ini_file);
  kvalita_casticv++;

  p_Level->KvalitaCastic = kvalita_castic;
  p_Level->KvalitaCasticV = kvalita_casticv;

  file = fopen(p_Level_Name, "rb");
  if (!file) {
    sprintf(text, "Unable to find level %s", p_Level_Name);    
    kprintf(1, text);
    return -1;
  }

  fread(&l_h, sizeof(LEVEL_HEADER), 1, file);

  p_Level->Size[0] = l_h.x;
  p_Level->Size[1] = l_h.z;
  p_Level->Size[2] = l_h.y * 2;
  p_Level->Size_of_Level = l_h.x * l_h.y * l_h.z * 2;

  l_h.y *= 2;
  kom_init_level(&l_h);

  memcpy((void *) &p_Level->LevelHeader, (void *) &l_h, sizeof(LEVEL_HEADER));

  p_Level->Count_Of_Items = l_h.prvku;

  p_Level->Level = (ITEMDESC **) malloc((p_Level->Size_of_Level) * sizeof(ITEMDESC *));
  if (!p_Level->Level) {    
    kprintf(1, "Unable to allocate memory for level");
    return -1;
  }

  p_Level->Square = (SQUAREINFO *) malloc((p_Level->Size_of_Level) * sizeof(SQUAREINFO));
  if (!p_Level->Square) {
    kprintf(1, "Unable to allocate memory for aditional level info");
    return -1;
  }
  else {
    for (i = 0; i < p_Level->Size_of_Level; i++)
      p_Level->Square[i].bUnderWater = -1;
  }

  p_Level->Item = (ITEMDESC *) malloc((p_Level->Count_Of_Items) * sizeof(ITEMDESC));
  if (!p_Level->Item) {
    kprintf(1, "Unable to allocate memory for items");
    return -1;
  }

  memset(p_Level->Item, 0, (p_Level->Count_Of_Items) * sizeof(ITEMDESC));

  for (i = 0; i < p_Level->Count_Of_Items; i++)
    p_Level->Item[i].hSvetlo = -1;

  p_Level->Action_Item = (long *) malloc((p_Level->Count_Of_Items) * sizeof(long) * 2);
  if (!p_Level->Action_Item) {
    kprintf(1, "Unable to allocate memory for action items");
    return -1;
  }

  p_Level->Count_Of_Action_Items = p_Level->Count_Of_Items * 2;

  p_Level->Anim_Item = (long *) malloc((p_Level->Count_Of_Items) * sizeof(long));
  if (!p_Level->Anim_Item) {
    kprintf(1, "Unable to allocate memory for animation items");
    return -1;
  }

  p_Level->pSquare = (SQUAREDESC *) malloc(p_Level->Size[0] * p_Level->Size[1] * sizeof(SQUAREDESC));
  if (!p_Level->pSquare) {
    kprintf(1, "Nelze alokovat pamet pro spodni patro");
    return -1;
  }

  for (i = 0; i < (p_Level->Size[0] * p_Level->Size[1]); i++) {
    p_Level->pSquare[i].iUroven = 0;
    p_Level->pSquare[i].Flek.pFlek = 0;
  }

  for (i = 0; i < 10; i++) {
    p_Level->VodniCakanec1[i].pCastice = NULL;
    p_Level->VodniCakanec2[i].pCastice = NULL;
    p_Level->NatureESystem[i].pSystem = (size_ptr)NULL;
  }

  for (i = 0; i < 10; i++)
    p_Level->Krompac[i].pCastice = NULL;

  for (i = 0; i < 20; i++) {
    p_Level->Exploze[i].pCastice = NULL;
    p_Level->Propadla[i].pCastice = NULL;
    p_Level->Kamen[i].pCastice = NULL;
    p_Level->Jiskra[i].pCastice = NULL;
    p_Level->Prach[i].pCastice = NULL;
    p_Level->Flare[i].pFlare = 0;
    p_Level->Flare[i].hSvetlo = -1;
  }

  for (i = 0; i < 6; i++) {
    p_Level->BublSystem[i].iItem = -1;
    p_Level->BublSystem[i].mesh = -1;
  }

  pointer = 0;
  act_pointer = 0;
  ani_pointer = 0;

  x = 0;
  y = 0;
  z = 0;
  for (i = 0; i < p_Level->Size_of_Level; i++) {
#ifdef _DEBUG
    assert(fread(&b_l_d, sizeof(b_l_d), 1, file) == 1);
#else
    fread(&b_l_d, sizeof(b_l_d), 1, file);
#endif

    Guid = b_l_d.guid;
    Rotation = b_l_d.Rotace;
    Vazba1 = b_l_d.Vazba1;
    Vazba2 = b_l_d.Vazba2;

    if (Guid) {
      kprintf(1, "lsi_Find_Object(GUID = %d)", Guid);

      error = lsi_Find_Object(Guid, p_Level);

      bNeviditelnaZed = 0;

      // jestlize je to neviditelna zed, tak ji preved na zed
      if (error != -1)
        if (p_Level->Object[error].Class == 21) {
          error = lsi_Find_Wall_Object(p_Level);
          bNeviditelnaZed = 1;
        }

      if (error == -1) {
        errors++;
        p_Level->Level[i] = 0;
        kprintf(1, "nerozlustil jsem guid %d", Guid);
      }
      else {
        if (!bNeviditelnaZed) {
          p_Level->Item[pointer].Index_Of_Game_Mesh = kom_pridej_prvek_levelu(&b_l_d, x, z, y);
          kom_umisti_prvek(p_Level->Item[pointer].Index_Of_Game_Mesh, x, z, y, Rotation);
        }
        else
          p_Level->Item[pointer].Index_Of_Game_Mesh = 0;

        kprintf(1, "pridavam prvek %d ...", pointer);

        p_Level->Item[pointer].p_Object = &p_Level->Object[error];
        p_Level->Item[pointer].Connection[0] = Vazba1;
        p_Level->Item[pointer].Connection[1] = Vazba2;
        p_Level->Item[pointer].Rotation = Rotation;
        p_Level->Item[pointer].Pos[0] = x;
        p_Level->Item[pointer].Pos[1] = y;
        p_Level->Item[pointer].Pos[2] = z;
        p_Level->Item[pointer].bDestroed = 0;
        p_Level->Item[pointer].a_run = 0;
        p_Level->Item[pointer].a_flag = -1;
        p_Level->Item[pointer].iItem = pointer;
        p_Level->Item[pointer].Square.iUroven = 0;
        p_Level->Item[pointer].Square.Flek.pFlek = 0;
        p_Level->Item[pointer].Square.pExtFlek = NULL;

        p_Level->Level[i] = &p_Level->Item[pointer];

        if (p_Level->Item[pointer].p_Object->Class == 13) {
          p_Level->Anim_Item[ani_pointer] = pointer;
          ani_pointer++;
        }

        if ((p_Level->Item[pointer].p_Object->Class == 6) ||
          ((p_Level->Item[pointer].p_Object->Class == 13) &&
            (p_Level->Item[pointer].p_Object->SubClass == 4)))
          bVybusniny = 1;

        if (p_Level->Item[pointer].p_Object->Class == 15)
          bPodlaha = 1;

        if (p_Level->Item[pointer].p_Object->Class == 7)
          bKamen = 1;

        if (p_Level->Item[pointer].p_Object->Class == 12) {
          kom_mesh_set_no_test(p_Level->Item[pointer].Index_Of_Game_Mesh);
          bVoda = 1;
        }

        if (((p_Level->Item[pointer].p_Object->Class > 7) &&
            (p_Level->Item[pointer].p_Object->Class < 12)) ||
          (p_Level->Item[pointer].p_Object->Class == 4) ||
          (p_Level->Item[pointer].p_Object->Class == 15)) {
          if (p_Level->Item[pointer].p_Object->Class != 11) {
            p_Level->Action_Item[act_pointer] = i;
            act_pointer++;
          }
          else {
            p_Level->Action_Item[act_pointer] =
              p_Level->Item[pointer].Connection[0];
            act_pointer++;
            p_Level->Action_Item[act_pointer] =
              p_Level->Item[pointer].Connection[1];
            act_pointer++;
          }
        }

        if (p_Level->Item[pointer].p_Object->Class == 1) {
          p_Level->Beetle[Next_Beetle] =
            p_Level->Item[pointer].p_Object->SubClass;
          p_Level->BeetleAnim[Next_Beetle].Mesh =
            p_Level->Item[pointer].Index_Of_Game_Mesh;
          p_Level->BeetleAnim[Next_Beetle].iRot =
            p_Level->Item[pointer].Rotation;

          p_Level->BublSystem[Next_Beetle].iItem = pointer;
          p_Level->BublSystem[Next_Beetle].mesh =
            p_Level->Item[pointer].Index_Of_Game_Mesh;


          ZeroMemory(&p_Level->BeatleSmoke[Next_Beetle],
            sizeof(BEATLE_SMOKE_SYSTEM));
          p_Level->BeatleSmoke[Next_Beetle].dwExpTime = 4000;
          p_Level->BeatleSmoke[Next_Beetle].dwRealTime =
            (int) ceil((randf() / (float) RAND_MAX) * 600.0f);
          p_Level->BeatleSmoke[Next_Beetle].iItem = pointer;

//        kom_mesh_set_mesh(p_Level->BeetleAnim[Next_Beetle].Mesh, 2, p_Level->Item[pointer].Rotation);
          Next_Beetle++;
        }

        if ((p_Level->Item[pointer].p_Object->Class == 1) ||
          (p_Level->Item[pointer].p_Object->Class == 11) ||
          (p_Level->Item[pointer].p_Object->Class == 8) ||
          (p_Level->Item[pointer].p_Object->Class == 10)) {
          int m;

          p_Level->Item[pointer].p_Back_Pack =
            (BACK_PACK *) malloc(sizeof(BACK_PACK));

          for (m = 0; m < 16; m++)
            p_Level->Item[pointer].p_Back_Pack->item[m] = 0;

          p_Level->Item[pointer].p_Back_Pack->Count_of_Items = 0;
          p_Level->Item[pointer].p_Back_Pack->Max_Items = 4;
          p_Level->Item[pointer].p_Back_Pack->Strength = 2;
        }

        pointer++;
      }
    }
    else
      p_Level->Level[i] = 0;

    x++;
    if (x > (p_Level->Size[0] - 1)) {
      x = 0;
      y++;
      if (y > (p_Level->Size[1] - 1)) {
        y = 0;
        z++;
      }
    }
  }
  fclose(file);

  p_Level->Action_Item[act_pointer] = -1;
  p_Level->Anim_Item[ani_pointer] = -1;

  if (!lsi_Create_Krompac(p_Level, (int) ceil(250 / kvalita_castic)))
    kprintf(1, "Nepodarilo ze vytvorit vsechny krompace!");

  if (bVybusniny) {
    if (!lsi_Create_Explosions(p_Level, (int) ceil(500 / kvalita_castic)))
      kprintf(1, "Nepodarilo ze vytvorit vsechny vybuchy!");

    if (!lsi_Create_Jiskry(p_Level, (int) ceil(20 / kvalita_castic)))
      kprintf(1, "Nepodarilo ze vytvorit vsechny jistry!");
  }

  if (bPodlaha)
    if (!lsi_Create_Trap_Doors(p_Level, (int) ceil(15 / kvalita_castic)))
      kprintf(1, "Nepodarilo ze vytvorit vsechny propadla!");

  if (bKamen)
    if (!lsi_Create_Rocks(p_Level, (int) ceil(1000 / kvalita_castic)))
      kprintf(1, "Nepodarilo ze vytvorit vsechny kameny!");

  if (bVoda) {
    if (!lsi_Create_Vodni_Cakance1(p_Level,
        (int) ceil(5000 / kvalita_casticv)))
      kprintf(1, "Nepodarilo ze vytvorit vsechny kameny!");

    if (!lsi_Create_Vodni_Cakance2(p_Level,
        (int) ceil(5000 / kvalita_casticv)))
      kprintf(1, "Nepodarilo ze vytvorit vsechny kameny!");
  }

  if (!lsi_Create_Prach(p_Level, 1))
    kprintf(1, "Nepodarilo ze vytvorit vsechen prach :) !");
/*
	if(!Next_Beetle)
		MessageBox(NULL,"Protože seš lama a spustil si level bez berušky, tak ti to teï spadne!","!!!",MB_OK);
*/
  return errors;
}

//------------------------------------------------------------------------------------------------
// Release Level from memory
//------------------------------------------------------------------------------------------------
void lsi_Release_Level(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Count_Of_Items; i++) {
    if (p_Level->Level[i]) {
      if ((p_Level->Level[i]->p_Object->Class == 1) ||
        (p_Level->Level[i]->p_Object->Class == 11) ||
        (p_Level->Level[i]->p_Object->Class == 8) ||
        (p_Level->Level[i]->p_Object->Class == 10)) {
        kprintf(1, "free(p_Level->Level[%d]->p_Back_Pack);", i);
        free(p_Level->Level[i]->p_Back_Pack);
      }
    }
  }

  kprintf(1, "free (Item, Level, Square, Action_Item, Anim_Item, pSquare");

  free((void *) p_Level->Item);
  free((void *) p_Level->Level);
  free((void *) p_Level->Square);
  free((void *) p_Level->Action_Item);
  free((void *) p_Level->Anim_Item);
  free((void *) p_Level->pSquare);

  for (i = 0; i < 10; i++)
    if (p_Level->Krompac[i].pCastice) {
      kprintf(1, "free p_Level->Krompac[%d].pCastice", i);
      free((void *) p_Level->Krompac[i].pCastice);
    }

  for (i = 0; i < 20; i++) {
    if (p_Level->Exploze[i].pCastice) {
      kprintf(1, "free p_Level->Exploze[%d].pCastice", i);
      free((void *) p_Level->Exploze[i].pCastice);
    }

    if (p_Level->Jiskra[i].pCastice) {
      kprintf(1, "free p_Level->Jiskra[%d].pCastice", i);
      free((void *) p_Level->Jiskra[i].pCastice);
    }

    if (p_Level->Propadla[i].pCastice) {
      kprintf(1, "free p_Level->Propadla[%d].pCastice", i);
      free((void *) p_Level->Propadla[i].pCastice);
    }

    if (p_Level->Kamen[i].pCastice) {
      kprintf(1, "free p_Level->Kamen[%d].pCastice", i);
      free((void *) p_Level->Kamen[i].pCastice);
    }

    if (p_Level->Prach[i].pCastice) {
      kprintf(1, "free p_Level->Prach[%d].pCastice", i);
      free((void *) p_Level->Prach[i].pCastice);
    }
  }

/*	for(i=0;i<6;i++)
		if(p_Level->BeatleSmoke[i].sStopy.System)
		{
			par_zrus(p_Level->BeatleSmoke[i].sStopy.System);
			free((void *) p_Level->BeatleSmoke[i].sStopy.pCastice);
		}*/

  for (i = 0; i < 10; i++) {
    if (p_Level->VodniCakanec1[i].pCastice) {
      kprintf(1, "free p_Level->VodniCakanec1[%d].pCastice", i);
      free((void *) p_Level->VodniCakanec1[i].pCastice);
    }

    if (p_Level->VodniCakanec2[i].pCastice) {
      kprintf(1, "free p_Level->VodniCakanec2[%d].pCastice", i);
      free((void *) p_Level->VodniCakanec2[i].pCastice);
    }

    if (p_Level->NatureESystem[i].pSystem) {
      kprintf(1, "free p_Level->NatureESystem[%d].pSystem", i);
      par_zrus(p_Level->NatureESystem[i].pSystem);

      kprintf(1, "     free (pCastice, hHnizdo, pivot, dir, ny, nx");

      free((void *) p_Level->NatureESystem[i].pCastice);
      free((void *) p_Level->NatureESystem[i].hHnizdo);
      free((void *) p_Level->NatureESystem[i].pivot);
      free((void *) p_Level->NatureESystem[i].dir);
      free((void *) p_Level->NatureESystem[i].ny);
      free((void *) p_Level->NatureESystem[i].nx);
    }

    if (p_Level->KourUst[i].System) {
      kprintf(1, "par_zrus p_Level->KourUst[%d].pSystem", i);
      par_zrus(p_Level->KourUst[i].System);
    }
  }

  if (p_Level->pCara) {
    kprintf(1, "free((void *) p_Level->pCara)");
    free((void *) p_Level->pCara);
  }

  if (p_Level->Rain.bRain) {
    kprintf(1, "p_Level->Rain.pSystem");
    par_zrus(p_Level->Rain.pSystem);

    kprintf(1, "	free (hHnizdo, pivot, pCastice");
    free((void *) p_Level->Rain.hHnizdo);
    free((void *) p_Level->Rain.pivot);
    free((void *) p_Level->Rain.pCastice);
    p_Level->Rain.bRain = 0;
  }

  if (p_Level->Rain.bWaterCircles) {
    kprintf(1, "p_Level->Rain.pWSystem");
    par_zrus(p_Level->Rain.pWSystem);

    kprintf(1, "	free (hWHnizdo, Wpivot, Wpos, pWCastice");
    free((void *) p_Level->Rain.hWHnizdo);
    free((void *) p_Level->Rain.Wpivot);
    free((void *) p_Level->Rain.Wpos);
    free((void *) p_Level->Rain.pWCastice);
    p_Level->Rain.bWaterCircles = 0;
  }

  memset(&p_Level->Rain, 0, sizeof(p_Level->Rain));

  if (p_Level->StreetRain.bWaterCircles) {
    kprintf(1, "p_Level->StreetRain.pWSystem");
    par_zrus(p_Level->StreetRain.pWSystem);

    kprintf(1, "	free (hWHnizdo, Wpivot, Wpos, pWCastice");
    free((void *) p_Level->StreetRain.hWHnizdo);
    free((void *) p_Level->StreetRain.Wpivot);
    free((void *) p_Level->StreetRain.Wpos);
    free((void *) p_Level->StreetRain.pWCastice);
    p_Level->StreetRain.bWaterCircles = 0;
  }

  memset(&p_Level->StreetRain, 0, sizeof(p_Level->StreetRain));

  if (p_Level->Snow.bSnow) {
    kprintf(1, "p_Level->Snow.pSystem");
    par_zrus(p_Level->Snow.pSystem);

    kprintf(1, "	free (hHnizdo, pivot, pCastice");
    free((void *) p_Level->Snow.hHnizdo);
    free((void *) p_Level->Snow.pivot);
    free((void *) p_Level->Snow.pCastice);
    p_Level->Snow.bSnow = 0;
  }

  memset(&p_Level->Snow, 0, sizeof(p_Level->Snow));

  if (p_Level->Mist.bMist) {
    kprintf(1, "edl_svetlo_zrus ,p_Level->Mist.hSvetlo = %d",
      p_Level->Mist.hSvetlo);
    edl_svetlo_zrus(p_Level->Mist.hSvetlo);
    p_Level->Mist.bMist = 0;
  }
}

void lsi_Get_Dir_Name(char *cText, char *cLevel)
{
  cText[0] = 0;
  char *c = strstr(cLevel, ".");  
  if (c) {
    int length = c - cLevel;
    strncpy(cText, cLevel, length);
    cText[length] = 0;
  }
}

void lsi_copy_save(char *cMask, LEVELINFO * p_Level)
{
  char dir[MAX_FILENAME + 1];
  char cLevelMask[MAX_FILENAME];
  char csrc[MAX_FILENAME + 1];
  char cout[MAX_FILENAME + 1];

  strcpy(dir, GAME_LEVEL_DIR);
  lsi_Get_Dir_Name(cLevelMask, p_Level->cLoadedFrom);

  sprintf(cout, "%s%s", cMask, ".b2l");
  sprintf(csrc, "%s%c%s%c%s%s", dir, DIR_SLASH, cLevelMask, DIR_SLASH, cLevelMask, ".b2l");
  lsi_file_copy(csrc, cout);

  sprintf(cout, "%s%s", cMask, ".b2t");
  sprintf(csrc, "%s%c%s%c%s%s", dir, DIR_SLASH, cLevelMask, DIR_SLASH, cLevelMask, ".b2t");
  lsi_file_copy(csrc, cout);

  sprintf(cout, "%s%s", cMask, ".bpr");
  sprintf(csrc, "%s%c%s%c%s%s", dir, DIR_SLASH, cLevelMask, DIR_SLASH, cLevelMask, ".bpr");
  lsi_file_copy(csrc, cout);

  sprintf(cout, "%s%s", cMask, ".lv6");
  sprintf(csrc, "%s%c%s%c%s%s", dir, DIR_SLASH, cLevelMask, DIR_SLASH, cLevelMask, ".lv6");
  lsi_file_copy(csrc, cout);

  sprintf(cout, "%s%s", cMask, ".ply");
  sprintf(csrc, "%s%c%s%c%s%s", dir, DIR_SLASH, cLevelMask, DIR_SLASH, cLevelMask, ".ply");
  lsi_file_copy(csrc, cout);
}

int lsi_Get_Save_Info(char *p_Level_Name, int *pActLevel, int *pActScene)
{
  FILE *file;
  char text[256];
  PLAYER_PROFILE pPlayer;
  LEVEL_HEADER l_h;
  WCHAR wTmp[512];
  int ver;

  chdir(SAVE_DIR);
  chdir(p_Level_Name);

  sprintf(text, "%s.lvc", p_Level_Name);

  file = fopen(text, "rb");

  if (!file)
    return 0;

  fread(&pPlayer, sizeof(PLAYER_PROFILE), 1, file);
  fread(wTmp, 32 * sizeof(WCHAR), 1, file);
  fread(&ver, sizeof(int), 1, file);
  fread(wTmp, (256+1) * sizeof(WCHAR), 1, file);
  fread(&l_h, sizeof(LEVEL_HEADER), 1, file);

  *pActLevel = l_h.rezerved[0];
  *pActScene = l_h.rezerved[1];

  fclose(file);

  return 1;
}

#ifdef WINDOWS
int lsi_Save_Exist(WCHAR * wName, char *cFile)
{
	FILE *file;
	long Done, error;
	struct _finddata_t	Data;
	char	cwd[MAX_FILENAME+1];
	char	text[MAX_FILENAME+1];
	PLAYER_PROFILE	pPlayer;
	WCHAR	wTmp[32];

	Done = _findfirst("*",&Data);
	error = Done;
			
	while(error != -1)
	{
		if(error != -1)
		{
			getcwd(cwd,MAX_FILENAME);

			chdir(Data.name);

			sprintf(text,"%s.lvc", Data.name);

			file = fopen(text, "rb");

			if(file)
			{
				fread(&pPlayer, sizeof(PLAYER_PROFILE), 1, file);
				fread(wTmp, 32 * sizeof(WCHAR), 1, file);

				fclose(file);

				if(!wcscmp(pPlayer.cName, pPlayerProfile.cName) && !wcscmp(wTmp, wName))
				{
					chdir(cwd);
					strcpy(cFile, Data.name);
					_findclose(Done); 
					return 1;
				}
			}

			chdir(cwd);

			error = _findnext(Done,&Data);
		}
	}
	_findclose(Done); 

  return 0;
}

void delete_dir(char *p_Level_Name)
{
	long Done, error;
	struct _finddata_t	Data;

	if(chdir(p_Level_Name) == -1)
		return;

	Done = _findfirst("*.*",&Data);
	error = Done;
			
	while(error != -1)
	{
		if(error != -1)
		{
			if(!(Data.attrib & _A_SUBDIR))
					remove(Data.name);

			error = _findnext(Done,&Data);
		}
	}
	_findclose(Done); 

	chdir("..");
	rmdir(p_Level_Name);
}
#endif

#ifdef LINUX
int lsi_Save_Exist(WCHAR * wName, char *cFile)
{
  struct dirent **namelist;
	char	cwd[MAX_FILENAME+1];	
  int   i;
  
  file_filter_mask("*");
  int c = scandir(".", &namelist, &file_filter, alphasort); 
  if (c < 0) {    
    return 0;
  }

  getcwd(cwd,MAX_FILENAME);

  int ret = false;
  for(i = 0; i < c; i++) {
    char	text[MAX_FILENAME+1];
      
    if(chdir(namelist[i]->d_name)) {
      kwarning(1, "Unable to chdir(%s)", namelist[i]->d_name);
      continue;
    }
    
    sprintf(text,"%s.lvc", namelist[i]->d_name);
    
    FILE *file = fopen(text, "rb");
    if(file)
    {
      PLAYER_PROFILE	pPlayer;
      WCHAR	wTmp[32];
    
      fread(&pPlayer, sizeof(PLAYER_PROFILE), 1, file);
      fread(wTmp, 32 * sizeof(WCHAR), 1, file);
      fclose(file);

      if(!wcscmp(pPlayer.cName, pPlayerProfile.cName) && !wcscmp(wTmp, wName))
      {
        strcpy(cFile, namelist[i]->d_name);
        ret = true;
        break;
      }
    }

    chdir(cwd);
  }

  chdir(cwd);

  for(i = 0; i < c; i++)
    free(namelist[i]);
  free(namelist);
  
  return ret;
}

void delete_dir(char *p_Level_Name)
{
  struct dirent **namelist;
  int i;

	if(chdir(p_Level_Name) == -1)
		return;

  file_filter_mask("*");
  int c = scandir(".", &namelist, &file_filter, alphasort);
  if (c < 0) {    
    return;
  }
  
  for(i = 0; i < c; i++) {
    remove(namelist[i]->d_name);
    free(namelist[i]);
  }
  free(namelist);

	chdir("..");
	rmdir(p_Level_Name);
}
#endif

void lsi_Save_Level(WCHAR * pwc_Level_Name, LEVELINFO * p_Level)
{
	FILE *file;
	int i;
	char p_Level_Name[MAX_FILENAME];
	BUNKA_LEVELU_DISK b_l_d;
	DWORD	time;
	char	pom[128];
	char	pom2[128];
	int		ver = SAVE_VER;

	ZeroMemory(p_Level_Name, MAX_FILENAME);

	chdir(SAVE_DIR);

	_strdate(pom);

	for(i=0;i<(signed)strlen(pom);i++)
		if(pom[i] == '/')
			pom[i] = '_';

	_strtime(pom2);

	for(i=0;i<(signed)strlen(pom2);i++)
		if(pom2[i] == ':')
			pom2[i] = '_';

	sprintf(p_Level_Name, "save_[%s]_[%s]", pom, pom2);
  kprintf(1, "Saving level as %s", p_Level_Name);
    
	if(lsi_Save_Exist(pwc_Level_Name, p_Level_Name))
    delete_dir(p_Level_Name);

#ifdef WINDOWS
  mkdir(p_Level_Name);
#else
	mkdir(p_Level_Name, DEFAULT_DIR_MASK);
#endif
  
	chdir(p_Level_Name);

	lsi_copy_save(p_Level_Name, p_Level);

	sprintf(p_Level_Name, "%s.lvc", p_Level_Name);

	file = fopen(p_Level_Name, "wb");
	if(!file)
		return;

	fwrite(&pPlayerProfile, sizeof(PLAYER_PROFILE), 1, file);

	//zapis jmeno level
	fwrite(pwc_Level_Name, 32 * sizeof(WCHAR), 1, file);
	fwrite(&ver, sizeof(int), 1, file);
	fwrite(p_Level->cLoadedFrom, (256+1)*sizeof(char), 1, file);

	p_Level->LevelHeader.rezerved[0] = iActualLevel;
	p_Level->LevelHeader.rezerved[1] = iActualScene;

	//zapis hlavicku
	fwrite(&p_Level->LevelHeader, sizeof(LEVEL_HEADER), 1, file);

	//zapis cas a kroky
	time = p_Level->dwPlayTime + (timeGetTime() - p_Level->dwStartTime);
	fwrite(&time, sizeof(DWORD), 1, file);
	fwrite(&p_Level->iNumOfSteps, sizeof(int), 1, file);

	//zapis itemy
	for(i=0;i<p_Level->Count_Of_Items;i++)
	{
		ZeroMemory(&b_l_d, sizeof(BUNKA_LEVELU_DISK));

		b_l_d.guid = p_Level->Item[i].p_Object->GUID;
		b_l_d.mesh = p_Level->Item[i].Index_Of_Game_Mesh;
		b_l_d.Rotace = p_Level->Item[i].Rotation;
		b_l_d.Vazba1 = p_Level->Item[i].Connection[0];
		b_l_d.Vazba2 = p_Level->Item[i].Connection[1];
		b_l_d.rez[0] = p_Level->Item[i].bDestroed;
		b_l_d.rez[1] = p_Level->Item[i].Pos[0];
		b_l_d.rez[2] = p_Level->Item[i].Pos[1];
		b_l_d.rez[3] = p_Level->Item[i].Pos[2];

		fwrite(&b_l_d, sizeof(BUNKA_LEVELU_DISK), 1, file);
	}

	//zapis back_packy
	for(i=0;i<p_Level->Count_Of_Items;i++)
		if(p_Level->Item[i].p_Back_Pack)
			fwrite(p_Level->Item[i].p_Back_Pack, sizeof(BACK_PACK), 1, file);

	//ulozi spodni patro fleku
	fwrite(p_Level->pSquare, sizeof(SQUAREDESC), p_Level->Size[0] * p_Level->Size[1], file);
	
	//ulozi flaky privazane k prvkum
	for(i=0;i<p_Level->Count_Of_Items;i++)
		fwrite(&p_Level->Item[i].Square, sizeof(SQUAREDESC), 1, file);

	fclose(file);
}

int lsi_fint_item(int mesh, LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if (p_Level->Level[i]->Index_Of_Game_Mesh == mesh)
        return i;

  return -1;
}

void lsi_Add_Prifile(PLAYER_PROFILE * pProfile)
{
  int i;

  for (i = 0; i < 20; i++)
    if (!pPlayerProfile.cScene[i])
      pPlayerProfile.cScene[i] = pProfile->cScene[i];

  for (i = 0; i < 20; i++)
    if (!pPlayerProfile.cMovie[i])
      pPlayerProfile.cMovie[i] = pProfile->cMovie[i];

  for (i = 0; i < 600; i++)
    if (!pPlayerProfile.cLevel[i])
      pPlayerProfile.cLevel[i] = pProfile->cLevel[i];
}

void lsi_Destroy_Beetle(LEVELINFO * p_Level, int GUID, int mesh)
{
  int obj, i;

  if (GUID < 1000 || GUID > 1999)
    return;

  am_Remove_Beetle_Animation(mesh, p_Level);
  obj = lsi_Find_Object(GUID, p_Level);

  if (obj == -1)
    return;

  for (i = 0; i < 6; i++)
    if (p_Level->Beetle[i] == p_Level->Object[obj].SubClass) {
      p_Level->Beetle[i] = -1;
      p_Level->BublSystem[i].iItem = -1;
      p_Level->BublSystem[i].bOn = 0;
      p_Level->BublSystem[i].mesh = -1;
      //p_Level->BeatleSmoke[i].iItem = -1;
      return;
    }
}

//------------------------------------------------------------------------------------------------
// Loads level into memory
//------------------------------------------------------------------------------------------------
int lsi_Load_Saved_Level(char *p_Level_Name, LEVELINFO * p_Level)
{
  int real;
  char text[MAX_FILENAME];
  char ctext[MAX_FILENAME];
  FILE *file;
  int i, j;
  LEVEL_HEADER l_h;
  BUNKA_LEVELU_DISK b_l_d;
  PLAYER_PROFILE pProfile;
  FLEK_K *pFlek;
  WCHAR pwc_Level_Name[32];
  int ver;

  ZeroMemory(&b_l_d, sizeof(b_l_d));

  lsi_Get_Dir_Name(ctext, p_Level_Name);
  strcat(ctext, ".lvc");

  file = fopen(ctext, "rb");
  if (!file) {
    sprintf(text, "Level change file %s not present", ctext);
    kprintf(1, text);
    return -1;
  }

  fread(&pProfile, sizeof(PLAYER_PROFILE), 1, file);
  lsi_Add_Prifile(&pProfile);

  fread(pwc_Level_Name, 32 * sizeof(WCHAR), 1, file);

  fread(&ver, sizeof(int), 1, file);
  if (ver != SAVE_VER) {
    kprintf(1, "Save version mismatch");
    fclose(file);
    return -2;
  }

  fread(p_Level->cLoadedFrom, (256+1)*sizeof(char), 1, file);
  fread(&l_h, sizeof(LEVEL_HEADER), 1, file);

  memset(p_Level->Level, 0, sizeof(p_Level->Level[0])*p_Level->Size_of_Level);

  iActualLevel = l_h.rezerved[0];
  iActualScene = l_h.rezerved[1];

  fread(&p_Level->dwPlayTime, sizeof(DWORD), 1, file);
  fread(&p_Level->iNumOfSteps, sizeof(int), 1, file);

  for (i = 0; i < p_Level->Count_Of_Items; i++) {
#ifdef _DEBUG
    assert(fread(&b_l_d, sizeof(b_l_d), 1, file) == 1);
#else
    fread(&b_l_d, sizeof(b_l_d), 1, file);
#endif

    if (b_l_d.rez[0]) {
      lsi_Destroy_Beetle(p_Level, b_l_d.guid, b_l_d.mesh);
      kom_zrus_prvek(b_l_d.mesh);
      gl_Logical2Real(b_l_d.rez[1], b_l_d.rez[2], b_l_d.rez[3], &real, p_Level);

      if (b_l_d.guid >= 13000 && b_l_d.guid < 14000)
        am_Remove_Animate_itemB(i, p_Level);

      p_Level->Item[i].bDestroed = 1;
    }
    else {
      p_Level->Item[i].Index_Of_Game_Mesh = b_l_d.mesh;
      p_Level->Item[i].Connection[0] = b_l_d.Vazba1;
      p_Level->Item[i].Connection[1] = b_l_d.Vazba2;
      p_Level->Item[i].Rotation = b_l_d.Rotace;
      p_Level->Item[i].Pos[0] = b_l_d.rez[1];
      p_Level->Item[i].Pos[1] = b_l_d.rez[2];
      p_Level->Item[i].Pos[2] = b_l_d.rez[3];
      p_Level->Item[i].bDestroed = 0;

      kom_umisti_prvek(b_l_d.mesh, b_l_d.rez[1], b_l_d.rez[3], b_l_d.rez[2], b_l_d.Rotace);

      if (p_Level->Item[i].p_Object->Class == 1)
        for (j = 0; j < 6; j++)
          if (p_Level->BeetleAnim[j].Mesh == b_l_d.mesh)
            p_Level->BeetleAnim[j].iRot = b_l_d.Rotace;

      gl_Logical2Real(b_l_d.rez[1], b_l_d.rez[2], b_l_d.rez[3], &real, p_Level);

      p_Level->Level[real] = &p_Level->Item[i];
    }
  }

  for (i = 0; i < p_Level->Count_Of_Items; i++)
    if (p_Level->Item[i].p_Back_Pack)
      fread(p_Level->Item[i].p_Back_Pack, sizeof(BACK_PACK), 1, file);

  //ulozi spodni patro fleku
  fread(p_Level->pSquare, sizeof(SQUAREDESC),
    p_Level->Size[0] * p_Level->Size[1], file);

  for (i = 0; i < p_Level->Size[0] * p_Level->Size[1]; i++) {
    p_Level->pSquare[i].pExtFlek = NULL;

    if (p_Level->pSquare[i].Flek.pFlek) {
      pFlek = &p_Level->pSquare[i].Flek;

      pFlek->pFlek =
        kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos,
        (BOD *) & pFlek->nx, (BOD *) & pFlek->nz, pFlek->dx, pFlek->dy,
        pFlek->Rotation, p_Level->pSquare[i].iUroven, pFlek->Material);
    }
  }

  //ulozi flaky privazane k prvkum
  for (i = 0; i < p_Level->Count_Of_Items; i++) {
    fread(&p_Level->Item[i].Square, sizeof(SQUAREDESC), 1, file);
    p_Level->Item[i].Square.pExtFlek = NULL;
  }

  for (i = 0; i < p_Level->Count_Of_Items; i++)
    if (p_Level->Item[i].Square.Flek.pFlek
      && p_Level->Item[i].p_Object->Class != 1
      && p_Level->Item[i].p_Object->Class != 13) {
      pFlek = &p_Level->Item[i].Square.Flek;

      pFlek->pFlek =
        kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos,
        (BOD *) & pFlek->nx, (BOD *) & pFlek->nz, pFlek->dx, pFlek->dy,
        pFlek->Rotation, p_Level->Item[i].Square.iUroven, pFlek->Material);
    }

  fclose(file);

  return 1;
}

//------------------------------------------------------------------------------------------------
// pripoji k predmetu material
//------------------------------------------------------------------------------------------------
char lsi_Bind_Material(LEVELINFO * p_Level, ITEMDESC * pItem)
{
  int i, mh;

  for (i = 0; i < p_Level->iMaterialb; i++) {
    mh = kom_amat_get_kontejner(pItem->Index_Of_Game_Mesh);

    if (p_Level->pMaterialb[i].handle_meshe == mh) {
      pItem->mMaterial = &p_Level->pMaterialb[i];
      return 1;
    }
  }

  kprintfl(1, "Nepodarilo se nabindovat maretial k prvku %d",
    pItem->p_Object->GUID);
  return 0;
}

//------------------------------------------------------------------------------------------------
// pripoji k predmetum materialy
//------------------------------------------------------------------------------------------------
void lsi_Bind_Materials2Items(LEVELINFO * p_Level)
{
  int i;
  OBJECTDESC *pObj;
  ITEMDESC *pItem;

  for (i = 0; i < p_Level->Count_Of_Items; i++) {
    pObj = p_Level->Item[i].p_Object;
    pItem = &p_Level->Item[i];

    if (pObj->MatEvent[0].cType ||
      pObj->MatEvent[1].cType || pObj->MatEvent[2].cType)
      if (lsi_Bind_Material(p_Level, pItem)) {
        pItem->bMatEvent[0] = 0;
        pItem->bMatEvent[1] = 0;
        pItem->bMatEvent[2] = 0;
        continue;
      }

    pItem->bMatEvent[0] = 1;
    pItem->bMatEvent[1] = 1;
    pItem->bMatEvent[2] = 1;
    pItem->mMaterial = NULL;
  }
}

void lsi_Make_Screenshot(HDC hdc, char *pFile)
{
/*
	HDC					hdcMem;
	BITMAPINFO			*pbmiRGB = (BITMAPINFO *) NULL; 
	HBITMAP				hbmRGB  = (HBITMAP) NULL;
	PBYTE				pjBitsRGB;
	BITMAPFILEHEADER	bmpHeader;
	FILE				*bmpFile = NULL;

	hdcMem  = CreateCompatibleDC(hdc);

	pbmiRGB = (BITMAPINFO *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, sizeof(BITMAPINFO) );
	
	if (!pbmiRGB)
		MessageBox(NULL,"!pbmiRGB","!!!", MB_OK);
	
	pbmiRGB->bmiHeader.biSize	       = sizeof(BITMAPINFOHEADER);     
	pbmiRGB->bmiHeader.biWidth		   = Xresolution;     
	pbmiRGB->bmiHeader.biHeight        = Yresolution;     
	pbmiRGB->bmiHeader.biPlanes        = 1;     
	pbmiRGB->bmiHeader.biBitCount      = 24;     
	pbmiRGB->bmiHeader.biCompression   = BI_RGB;     
	pbmiRGB->bmiHeader.biSizeImage     = pbmiRGB->bmiHeader.biWidth
										 * abs(pbmiRGB->bmiHeader.biHeight) * 3; 

	hbmRGB = CreateDIBSection(hdcMem, pbmiRGB, DIB_RGB_COLORS,
							  (PVOID *) &pjBitsRGB, NULL, 0);
	
	if (!hbmRGB) 
		MessageBox(NULL,"!hbmRGB","!!!", MB_OK);

	if (!SelectObject(hdcMem, hbmRGB))
		MessageBox(NULL,"!SelectObject(hdcMem, hbmRGB)","!!!", MB_OK);

  
	if (!BitBlt(hdcMem, 0,0, Xresolution, Yresolution, hdc, 0,0,SRCCOPY)) 
		MessageBox(NULL,"!BitBlt","!!!", MB_OK);

	pjBitsRGB = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbmiRGB->bmiHeader.biSizeImage);

	if (!pjBitsRGB) 
		MessageBox(NULL,"!pjBitsRGB","!!!", MB_OK);

	if (!GetDIBits(hdcMem, hbmRGB, 0, (UINT)pbmiRGB->bmiHeader.biHeight, pjBitsRGB, 
				   (LPBITMAPINFO)pbmiRGB, DIB_RGB_COLORS))
		MessageBox(NULL,"!GetDIBits","!!!", MB_OK);
		
	bmpHeader.bfType = 'MB';
	bmpHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + pbmiRGB->bmiHeader.biSizeImage;
	bmpHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;

	bmpFile = fopen(pFile,"wb");

	if (!bmpFile)
		MessageBox(NULL,"!bmpFile","!!!", MB_OK);

	fwrite(&bmpHeader,sizeof(BITMAPFILEHEADER),1,bmpFile);
	fwrite(&pbmiRGB->bmiHeader,sizeof(BITMAPINFOHEADER),1,bmpFile);
	fwrite(pjBitsRGB,sizeof(BYTE),pbmiRGB->bmiHeader.biSizeImage,bmpFile);

	fclose(bmpFile);

	DeleteDC(hdcMem);          
	DeleteObject(hbmRGB);
	LocalFree(pbmiRGB);
	GlobalFree((HGLOBAL)pjBitsRGB);
*/
}
