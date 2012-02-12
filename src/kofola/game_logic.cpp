//------------------------------------------------------------------------------------------------
// version 1.0.0
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "3d_all.h"

#include "Object.h"
#include "init.h"
#include "load_level.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Berusky3d_kamery.h"
#include "animationk.h"
#include "ambient_sounds.h"
#include "3D_menus.h"
#include "Demo.h"
#include "Dda.h"
#include "Bind.h"
#include "animend_functions.h"
#include "3D_graphic.h"
#include "2D_graphic.h"
#include "trigers.h"
#include "water.h"
#include "font.h"
#include "game_init.h"
#include "Tools.h"
#include "Menu2.h"
#include "Adas.h"

extern HINT_STATE sHint;
extern KUK_STATE sKuk;
extern _3D_CURSOR _3dCur;

extern int iActualScene;
extern int iActualLevel;
extern char cFontFile[5][64];
extern HINT_STATE sHint;
extern HINT_TEXTURE pHintTexture[26];

#define	NEXTDEMOFRAMETIMERID	10001
#define randf()      ((float)rand())

extern APAK_HANDLE *pGDataArchive;
extern APAK_HANDLE *pBmpArchive;
extern APAK_HANDLE *pControlsArchive;
extern APAK_HANDLE *p3DMArchive;
extern APAK_HANDLE *pSndArchive;
extern APAK_HANDLE *pDataArchive;

typedef struct
{
  int mesh;
  int c_anim;
  int *p_flag;
  int flag;
  int start;
  int stop;
} _3DSANIM;

typedef struct
{
  int TopMesh;
  int LowMesh;
} PMESH;

typedef struct
{
  SIM_ANIMATION animation[32];
  int last;
  PMESH PrivazanyMesh[32];      // privazane meshe
  int pMesh;                    // uikazatel posledniho privazaneho meshe
  _3DSANIM _3dsAnim[6];         // 3ds animace;
  char bSetMesh;                // ma se zmenit mesh?
  int iMeshID;                  // id meshe
  int iMesh;                    // mesh
  int iRot;                     // rotace
} ANIMATION_QUEUE_SET;

char bSet_Frame_Rate;
ANIMATION_MODULE am;

AUDIO_DATA ad;
DEMOSTRUCTURE Demo;
ANIMATION_QUEUE_SET queue_set[3];
ANIMATION_QUEUE_SET *p_set = NULL;
ANIMATION_QUEUE_SET *p_set_anim = NULL;
int cam_flag;
extern char CurrentWorkingDirectory[256];
int CamRotationFlag;
int cameraflag;
int PRCameraFlag;               //indikator, ze je pozno pohybovat kamerou pomoci P&R
int PRCameraFlagChange;         // byl pozit pohyb pomoci P&R
LEVELINFO Level;
CONTROL_KEYS control;
extern int Xresolution;
extern int Yresolution;
extern B2_FONT b2_3d_font;
_2D_DATA _2dd;
static char demo = 0;           // 1 = real play, 2= fast play, 0 = record
int gl_Do_Lift(int Lift, int *pos, LEVELINFO * p_Level);
int gl_Gate_Keeper(int Door, int *Item, LEVELINFO * p_Level);
int gl_Count_Weight(int *column, LEVELINFO * p_Level);
void gl_Destroy_Item(long Item, int Type, LEVELINFO * p_Level);
int gl_Are_Animations_Done(LEVELINFO * p_Level);
void gl_Hrebik(ITEMDESC * pItem);
void gl_Go_Animations(void);
int gl_Find_Bottom(int *p_Pos, int *bVoda, LEVELINFO * p_Level);
int gl_Is_There_Beatle(int *iPos, ITEMDESC ** pItem, LEVELINFO * p_Level);
int gl_Will_Go_Under_Water(int *iPos, int DontCountPos, int Plus, int *pVaha,
  LEVELINFO * p_Level);
int gl_Can_Go_Into_Water(ITEMDESC * pItem);
int gl_Check_Detonation_Pack_Water_Rules(int *iPos, LEVELINFO * p_Level);
int gl_Test_Rule1(ITEMDESC * pItem, ITEMDESC * pTestItem,
  LEVELINFO * p_Level);
void gl_Logical2Real(int x, int y, int z, int *Real_Pos, LEVELINFO * p_Level);
int gl_Test_Rule2(int *pos, LEVELINFO * p_Level);
int gl_Test_Rule1B(ITEMDESC * pItem, int *pTestPos, LEVELINFO * p_Level);
int gl_Is_Included_In(SIM_ANIMATION * p_animation, int mesh);
void gl_Add_Mesh(SIM_ANIMATION * p_animation, int mesh);
int gl_Include_Coincidence(SIM_ANIMATION * p_anim1, SIM_ANIMATION * p_anim2);
SIM_ANIMATION *gl_Is_Included_In_AnimSet(ANIMATION_QUEUE_SET * p_aset,
  int mesh);
int gl_Is_There_Water(int *from, int *to, LEVELINFO * p_Level);
char MenuCheckBossExit(void);
char MenuCheckSuccessExit(void);
void gl_Kofola_End(int DirectX);

int gl_Get_Active_Beetle_Mesh(void)
{
  if (Level.Actual_Item != -1)
    return Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh;

  return -1;
}

void gl_Get_PR_ScanCode(int *pPScan, int *pRScan)
{
  if (pPScan)
    *pPScan = control.camera_move;

  if (pRScan)
    *pRScan = control.camera_rotation;
}

int gl_Chech_Wall(int *Pos, LEVELINFO * p_Level)
{
  int real;

  gl_Logical2Real(Pos[0], Pos[1], Pos[2], &real, p_Level);

  if (Pos[2] < 1 || Pos[2] > p_Level->Size[2] - 1 ||
    Pos[1] < 0 || Pos[1] > p_Level->Size[1] - 1 ||
    Pos[0] < 0 || Pos[0] > p_Level->Size[0] - 1)
    return 0;

  if (p_Level->Level[real])
    if (p_Level->Level[real]->p_Object->Class == 2)
      return 1;
    else
      return 0;
  else
    return 0;
}

void gl_Sand_Fill(int *Pos, int IncomingSide, int bUnderWater,
  LEVELINFO * p_Level)
{
  int real, ireal;
  int iPos[3];

  memcpy((void *) iPos, (void *) Pos, 3 * sizeof(int));

  //jestlize jsem se dostal mimo level, tak konec
  if (iPos[2] < 1 || iPos[2] > p_Level->Size[2] - 1 ||
    iPos[1] < 0 || iPos[1] > p_Level->Size[1] - 1 ||
    iPos[0] < 0 || iPos[0] > p_Level->Size[0] - 1)
    return;

  gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

  //jestlize tento prvek byl jiz nastaven, tak konec
  if (p_Level->Square[real].bUnderWater != -1)
    return;

  //jestlize se jedna o vertikalni pohyb, tak pouze okopiriruj flag
  if (IncomingSide > 1)
    p_Level->Square[real].bUnderWater = bUnderWater;
  else if (!IncomingSide) {
    iPos[2]--;
    gl_Logical2Real(iPos[0], iPos[1], iPos[2], &ireal, p_Level);

    //pohyb smeren navrch a prosel vodou
    if (p_Level->Level[ireal])
      if (p_Level->Level[ireal]->p_Object->Class == 12)
        p_Level->Square[real].bUnderWater = 0;
      else
        p_Level->Square[real].bUnderWater = bUnderWater;
    else
      p_Level->Square[real].bUnderWater = bUnderWater;

    iPos[2]++;
  }
  else if (IncomingSide == 1) {
    iPos[2]++;
    gl_Logical2Real(iPos[0], iPos[1], iPos[2], &ireal, p_Level);

    //pohyb smeren dolu a prosel vodou
    if (iPos[2] < p_Level->Size[2])
      if (p_Level->Level[ireal])
        if (p_Level->Level[ireal]->p_Object->Class == 12)
          p_Level->Square[real].bUnderWater = 1;
        else
          p_Level->Square[real].bUnderWater = bUnderWater;
      else
        p_Level->Square[real].bUnderWater = bUnderWater;
    else
      p_Level->Square[real].bUnderWater = bUnderWater;

    iPos[2]--;
  }


  //jestli jsem u zdi, tak konec  
  if (p_Level->Level[real])
    if (p_Level->Level[real]->p_Object->Class == 2)
      return;

  //postup vsemi smery
  iPos[0]++;
  gl_Sand_Fill(iPos, 2, p_Level->Square[real].bUnderWater, p_Level);

  iPos[0] -= 2;
  gl_Sand_Fill(iPos, 3, p_Level->Square[real].bUnderWater, p_Level);

  iPos[0]++;
  iPos[1]++;
  gl_Sand_Fill(iPos, 4, p_Level->Square[real].bUnderWater, p_Level);

  iPos[1] -= 2;
  gl_Sand_Fill(iPos, 5, p_Level->Square[real].bUnderWater, p_Level);

  iPos[1]++;
  iPos[2] += 2;
  gl_Sand_Fill(iPos, 0, p_Level->Square[real].bUnderWater, p_Level);

  iPos[2] -= 4;
  gl_Sand_Fill(iPos, 1, p_Level->Square[real].bUnderWater, p_Level);

}

void gl_Set_Water_Marks(LEVELINFO * p_Level)
{
  int pos[3] = { 0, 0, p_Level->Size[2] - 1 };

  gl_Sand_Fill(pos, 2, 0, p_Level);
}

//------------------------------------------------------------------------------------------------
// randomly choose wave index to play (depends on object class)
//------------------------------------------------------------------------------------------------
int gl_Choose_Wave_Index(int Plus)
{
  int index;

  index = rand() % 3;
  index += Plus;

  return index;
}

//------------------------------------------------------------------------------------------------
// next queue set
//------------------------------------------------------------------------------------------------
void gl_Next_Queue_Set(LEVELINFO * p_Level)
{
  p_Level->Actual_Qset++;
  if (p_Level->Actual_Qset > 2)
    p_Level->Actual_Qset = 0;

  p_set = &queue_set[p_Level->Actual_Qset];
}

void gl_Next_Anim_Queue_Set(LEVELINFO * p_Level)
{
  p_Level->Actual_Anim_Qset++;
  if (p_Level->Actual_Anim_Qset > 2)
    p_Level->Actual_Anim_Qset = 0;

  p_set_anim = &queue_set[p_Level->Actual_Anim_Qset];
}

ANIMATION_QUEUE_SET *gl_Get_Prev_Queue_Set(LEVELINFO * p_Level)
{
  int set = p_Level->Actual_Qset - 1;

  if (set < 0)
    set = 2;

  return &queue_set[set];
}

ANIMATION_QUEUE_SET *gl_Get_Prev_Queue_SetA(int dec, LEVELINFO * p_Level)
{
  int set = p_Level->Actual_Qset - dec;

  if (set < 0)
    set = 3 + set;

  return &queue_set[set];
}

int gl_Multi_Animation(ANIMATION_QUEUE_SET * p_aset,
  SIM_ANIMATION * p_animation)
{
  int i, c = 0, j;

  for (i = 0; i < p_aset->last; i++)
    for (j = 0; j < 8; j++)
      if (gl_Is_Included_In(&p_aset->animation[i], p_animation->mesh[j]) &&
        p_animation->mesh[j] != -1 &&
        p_animation->p_run != p_aset->animation[i].p_run)
        c++;

  return c;
}

int gl_Animation_Colision(ANIMATION_QUEUE_SET * p_aset,
  SIM_ANIMATION * p_animation)
{
  int i;
  SIM_ANIMATION *p_anim, *p_lanim = NULL;

  for (i = 0; i < 8; i++) {
    p_anim = gl_Is_Included_In_AnimSet(p_aset, p_animation->mesh[i]);

    if (p_anim) {
      if (p_lanim && p_lanim != p_anim)
        return 1;

      p_lanim = p_anim;
    }
  }

  return 0;
}

SIM_ANIMATION *gl_Find_Lowest_Animation(ANIMATION_QUEUE_SET * p_aset)
{
  SIM_ANIMATION *p_anim = NULL;
  int i, min = 101;

  for (i = 0; i < p_aset->last; i++)
    if ((p_aset->animation[i].flag > -1) && (p_aset->animation[i].flag < min)) {
      min = p_aset->animation[i].flag;
      p_anim = &p_aset->animation[i];
    }

  return p_anim;
}

int gl_Check_Connection_Collision(SIM_ANIMATION * p_prev_anim,
  ANIMATION_QUEUE_SET * p_a_set, SIM_ANIMATION * p_anim)
{
  SIM_ANIMATION *p_tanim;
  int i, c = 0;

  for (i = 0; i < 8; i++)
    if (p_prev_anim->mesh[i] == -1)
      return c;
    else {
      p_tanim = gl_Is_Included_In_AnimSet(p_a_set, p_prev_anim->mesh[i]);

      if (p_tanim)
        if (p_tanim->bconnected)
          c++;
    }

  return c;
}

void gl_Connect_Animations(LEVELINFO * p_Level)
{
  int i, j;
  ANIMATION_QUEUE_SET *p_prev_set = gl_Get_Prev_Queue_Set(p_Level);

  if (!p_prev_set->last)
    return;

  for (i = 0; i < p_set->last; i++)
    for (j = p_prev_set->last - 1; j >= 0; j--)
      if (gl_Include_Coincidence(&p_set->animation[i],
          &p_prev_set->animation[j]) && p_prev_set->animation[j].p_run != -1
        && p_set->animation[i].p_run != -1) {
        if (gl_Animation_Colision(p_prev_set, &p_set->animation[i]))
          break;
        else if (gl_Multi_Animation(p_set, &p_set->animation[i])) {
          if (!p_set->animation[i].bnext &&
            !gl_Check_Connection_Collision(&p_prev_set->animation[j], p_set,
              &p_set->animation[i])) {
            p_set->animation[i].bconnected = 1;
            rani_next_animace(p_prev_set->animation[j].p_run,
              p_set->animation[i].p_run, 0, 0, 0);
            break;
          }

        }
        else
          if (!gl_Check_Connection_Collision(&p_prev_set->animation[j], p_set,
            &p_set->animation[i])) {
          p_set->animation[i].bconnected = 1;
          rani_next_animace(p_prev_set->animation[j].p_run,
            p_set->animation[i].p_run, 0, 0, 0);
          break;
        }
      }
}

//------------------------------------------------------------------------------------------------
// transtales real position to logical position
//------------------------------------------------------------------------------------------------
void gl_Logical2Real(int x, int y, int z, int *Real_Pos, LEVELINFO * p_Level)
{
  *Real_Pos =
    x + y * p_Level->Size[0] + z * p_Level->Size[0] * p_Level->Size[1];
}

//------------------------------------------------------------------------------------------------
// transtales real position to logical position
//------------------------------------------------------------------------------------------------
void gl_Logical2Real2D(int x, int y, int *Real_Pos, LEVELINFO * p_Level)
{
  *Real_Pos = x + y * p_Level->Size[0];
}

//------------------------------------------------------------------------------------------------
// transtales real position to logical position
//------------------------------------------------------------------------------------------------
void gl_Real2Logical(int Real_Pos, int *Log_Pos, LEVELINFO * p_Level)
{
  int c;

  Log_Pos[2] = (int) floor(Real_Pos / (p_Level->Size[0] * p_Level->Size[1]));
  c = Real_Pos % (p_Level->Size[0] * p_Level->Size[1]);
  Log_Pos[1] = (int) floor(c / p_Level->Size[0]);
  c = c % p_Level->Size[0];
  Log_Pos[0] = c;
}

void gl_fPos2iPos(float *fPos, int *iPos)
{
  iPos[0] = ftoi(fPos[0]);
  iPos[1] = ftoi(fPos[1]);
  iPos[2] = ftoi(fPos[2]);
}

//------------------------------------------------------------------------------------------------
// finds next beatle
//------------------------------------------------------------------------------------------------
long gl_Find_Next_Beatle(long pos, LEVELINFO * p_Level)
{
  long i;
  int counter = 0;

  if ((p_Level->Beetle_Index + 1) > 5)
    p_Level->Beetle_Index = 0;
  else
    p_Level->Beetle_Index++;

  while (p_Level->Beetle[p_Level->Beetle_Index] == -1) {
    if ((p_Level->Beetle_Index + 1) > 5)
      p_Level->Beetle_Index = 0;
    else
      p_Level->Beetle_Index++;

    counter++;

    if (counter > 6)
      return -1;
  }

  for (i = pos + 1; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if ((p_Level->Level[i]->p_Object->Class == 1) &&
        (p_Level->Level[i]->p_Object->SubClass ==
          p_Level->Beetle[p_Level->Beetle_Index]))
        return i;

  for (i = 0; i < (pos + 1); i++)
    if (p_Level->Level[i])
      if ((p_Level->Level[i]->p_Object->Class == 1) &&
        (p_Level->Level[i]->p_Object->SubClass ==
          p_Level->Beetle[p_Level->Beetle_Index]))
        return i;

  return -1;
}

//------------------------------------------------------------------------------------------------
// finds beatle
//------------------------------------------------------------------------------------------------
long gl_Select_Beatle(int Beatle, LEVELINFO * p_Level)
{
  long i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if ((p_Level->Level[i]->p_Object->Class == 1) &&
        (p_Level->Level[i]->p_Object->SubClass == p_Level->Beetle[Beatle]))
        return i;

  return -1;
}

//------------------------------------------------------------------------------------------------
// spociva posuny po osach v ctvercich
//------------------------------------------------------------------------------------------------
void gl_Get_Move(int *iValue_old, int *iValue_new, int *iMove)
{
  int i;

  for (i = 0; i < 3; i++) {
    iMove[i] = iValue_new[i] - iValue_old[i];
  }
  iMove[2] >>= 1;
}

int gl_Get_Free_Flare(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < 20; i++)
    if (!p_Level->Flare[i].pFlare)
      return i;

  return -1;
}

int gl_Do_Teleport_Flare(float *pos, LEVELINFO * p_Level)
{
  int f, m, flare, i;
  float rr, rg, rb, ra, dx, dy;
  LENS_FLARE *pFlare;
  LENS_FLARE_SLOZ *pSFlare;

  f = gl_Get_Free_Flare(p_Level);

  if (f == -1)
    return -1;

  //udelej flare
  rr = 0.54f;
  rg = 0.83f;
  rb = 1.0f;

  ra = 1;
  dy = dx = 0;

  m = kom_najdi_material("flare4");

  if (m == -1)
    kprintf(1, "Nelze najit material flare4");

  flare = kom_flare_vyrob(FLR_NO_ZTEST);

  kom_flare_set_param(flare, rr, rg, rb, ra, dy, dx, m, GL_ONE);

  kom_flare_set_pos(flare, (BOD *) pos);

  pFlare = kom_flare_get_pointer(flare);

  p_Level->Flare[f].pFlare = flare;

  p_Level->Flare[f].mSpeed = 2.0f;

  pSFlare = (LENS_FLARE_SLOZ *) malloc(14 * sizeof(LENS_FLARE_SLOZ));

  pFlare->p_sloz = pSFlare;

  if (!pSFlare) {
    kprintf(1, "Nealokovala se pamet pro LENS_FLARE_SLOZ!\n");
    return -1;
  }

  for (i = 0; i < 14; i++)
    pSFlare[i].vzdal = 0;

  pSFlare[0].vzdal = 0.001f;
  pSFlare[0].r = 0.97f;
  pSFlare[0].g = 0.57f;
  pSFlare[0].b = 0.5f;
  pSFlare[0].a = 0.15f;
  pSFlare[0].material = kom_najdi_material("flare1");
  pSFlare[0].dx = 0;
  pSFlare[0].dy = 0;
  pSFlare[0].p_mat = kom_preloz_material(pSFlare[0].material);
  p_Level->Flare[f].Speed[0] = 2.0f;

  if (pSFlare[0].material == -1)
    kprintf(1, "Nelze najit material flare1");

  pSFlare[1].vzdal = 0.2f;
  pSFlare[1].r = 0.33f;
  pSFlare[1].g = 0.5f;
  pSFlare[1].b = 0.98f;
  pSFlare[1].a = 0.15f;
  pSFlare[1].material = kom_najdi_material("flare6");
  pSFlare[1].dx = 0;
  pSFlare[1].dy = 0;
  pSFlare[1].p_mat = kom_preloz_material(pSFlare[1].material);
  p_Level->Flare[f].Speed[1] = 0.15f;

  if (pSFlare[1].material == -1)
    kprintf(1, "Nelze najit material flare6");

  pSFlare[2].vzdal = 0.4f;
  pSFlare[2].r = 0.03f;
  pSFlare[2].g = 0.83f;
  pSFlare[2].b = 0.03f;
  pSFlare[2].a = 0.5f;
  pSFlare[2].material = kom_najdi_material("flare5");
  pSFlare[2].dx = 0;
  pSFlare[2].dy = 0;
  pSFlare[2].p_mat = kom_preloz_material(pSFlare[2].material);
  p_Level->Flare[f].Speed[2] = 0.05f;

  if (pSFlare[2].material == -1)
    kprintf(1, "Nelze najit material flare5");

  pSFlare[3].vzdal = 0.5f;
  pSFlare[3].r = 0.73f;
  pSFlare[3].g = 0.09f;
  pSFlare[3].b = 0.03f;
  pSFlare[3].a = 0.4f;
  pSFlare[3].material = kom_najdi_material("flare5");
  pSFlare[3].dx = 0;
  pSFlare[3].dy = 0;
  pSFlare[3].p_mat = kom_preloz_material(pSFlare[3].material);
  p_Level->Flare[f].Speed[3] = 0.05f;

  if (pSFlare[3].material == -1)
    kprintf(1, "Nelze najit material flare5");

  pSFlare[4].vzdal = 0.52f;
  pSFlare[4].r = 0.25f;
  pSFlare[4].g = 0.72f;
  pSFlare[4].b = 0.05f;
  pSFlare[4].a = 0.20f;
  pSFlare[4].material = kom_najdi_material("flare2");
  pSFlare[4].dx = 0;
  pSFlare[4].dy = 0;
  pSFlare[4].p_mat = kom_preloz_material(pSFlare[4].material);
  p_Level->Flare[f].Speed[4] = 0.2f;

  if (pSFlare[4].material == -1)
    kprintf(1, "Nelze najit material flare2");

  pSFlare[5].vzdal = 0.54f;
  pSFlare[5].r = 1;
  pSFlare[5].g = 1;
  pSFlare[5].b = 1;
  pSFlare[5].a = 0.3f;
  pSFlare[5].material = kom_najdi_material("flare5");
  pSFlare[5].dx = 0;
  pSFlare[5].dy = 0;
  pSFlare[5].p_mat = kom_preloz_material(pSFlare[5].material);
  p_Level->Flare[f].Speed[5] = 0.03f;

  if (pSFlare[5].material == -1)
    kprintf(1, "Nelze najit material flare5");

  pSFlare[6].vzdal = 0.59f;
  pSFlare[6].r = 0.64f;
  pSFlare[6].g = 0.64f;
  pSFlare[6].b = 0.05f;
  pSFlare[6].a = 0.15f;
  pSFlare[6].material = kom_najdi_material("flare6");
  pSFlare[6].dx = 0;
  pSFlare[6].dy = 0;
  pSFlare[6].p_mat = kom_preloz_material(pSFlare[6].material);
  p_Level->Flare[f].Speed[6] = 0.13f;

  if (pSFlare[6].material == -1)
    kprintf(1, "Nelze najit material flare6");

  pSFlare[7].vzdal = 0.69f;
  pSFlare[7].r = 0.03f;
  pSFlare[7].g = 0.44f;
  pSFlare[7].b = 0.66f;
  pSFlare[7].a = 0.20f;
  pSFlare[7].material = kom_najdi_material("flare2");
  pSFlare[7].dx = 0;
  pSFlare[7].dy = 0;
  pSFlare[7].p_mat = kom_preloz_material(pSFlare[7].material);
  p_Level->Flare[f].Speed[7] = 0.35f;

  if (pSFlare[7].material == -1)
    kprintf(1, "Nelze najit material flare2");

  pSFlare[8].vzdal = 0.8f;
  pSFlare[8].r = 0.78f;
  pSFlare[8].g = 0.97f;
  pSFlare[8].b = 0.99f;
  pSFlare[8].a = 0.05f;
  pSFlare[8].material = kom_najdi_material("flare1");
  pSFlare[8].dx = 1;
  pSFlare[8].dy = 1;
  pSFlare[8].p_mat = kom_preloz_material(pSFlare[8].material);
  p_Level->Flare[f].Speed[8] = 1.0f;

  if (pSFlare[8].material == -1)
    kprintf(1, "Nelze najit material flare1");

  p_Level->Flare[f].dwStart = timeGetTime();

  return f;
}

int gl_Do_Teleport_Light(float *pos, LEVELINFO * p_Level, int iFlare)
{
  int hSvetlo;
  int hEXSvetlo;
  float rr, rb, rg;

  if (!p_Level->Sub_Svetla) {
    rr = 1;
    rb = 1;
    rg = 1;

    hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);

    sdl_svetlo_set_pos(hSvetlo, (BOD *) pos);
    sdl_svetlo_set_diff(hSvetlo, rr, rg, rb, 0, 0, 0);

    sdl_anim_vyrob(hSvetlo, 20, 0, 0, 3);
    sdl_anim_vloz_klic_vzdal(hSvetlo, 0, 0, 0, 0, 0);
    sdl_anim_vloz_klic_vzdal(hSvetlo, 1, 10, 0, 10, 10);
    sdl_anim_vloz_klic_vzdal(hSvetlo, 2, 0, 0, 0, 19);
    sdl_anim_start(hSvetlo, &p_Level->TrashFlag, 0, 0, 0);
  }
  else {
    rr = 0;
    rg = 0;
    rb = 0;

    //hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR | SDL_SUB);

    hSvetlo = -1;
  }

  if (iFlare != -1) {
    //extra svetlo
    hEXSvetlo =
      edl_svetlo_vyrob(EDL_MESH_LIGHT | EDL_PRUHL_LIGHT | EDL_ALFA_SET, 1);

    if (hEXSvetlo != -1) {
      p_Level->Flare[iFlare].hEXSvetlo = hEXSvetlo;

      edl_svetlo_pridej_mesh(hEXSvetlo, p_Level->Flare[iFlare].mesh);
      edl_svetlo_uzavri_meshe(hEXSvetlo);

      edl_svetlo_set_pos(hEXSvetlo, (BOD *) pos, (BOD *) pos);
      edl_svetlo_set_diff(hEXSvetlo, 0);

      edl_anim_vyrob(hEXSvetlo, 20, 0, 0, 3, 0, 0);
      edl_anim_vloz_klic_diff(hEXSvetlo, 0, 0, 0);
      edl_anim_vloz_klic_diff(hEXSvetlo, 1, 1, 10);
      edl_anim_vloz_klic_diff(hEXSvetlo, 2, 0, 19);
      edl_anim_start(hEXSvetlo, &p_Level->TrashFlag, 0, 0, 0);
    }
    else
      p_Level->Flare[iFlare].hEXSvetlo = -1;
  }
  else
    hEXSvetlo = -1;



  return hSvetlo;
}

int gl_Get_Free_Lift_Particles(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < 10; i++)
    if (p_Level->LiftParticles[i].System == -1)
      return i;

  return -1;
}

int gl_Do_Lift_Particles(float *pos, int mesh, LEVELINFO * p_Level)
{
  int k, m, i;
  PAR_KOUR_STOPA *pKourovaS;
  int size = ftoi(50 / p_Level->KvalitaCastic);

  k = gl_Get_Free_Lift_Particles(p_Level);

  if (k == -1)
    return (0);

  pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

  if (!pKourovaS)
    return (0);

  p_Level->LiftParticles[k].pCastice = pKourovaS;
  p_Level->LiftParticles[k].Sizeof = size;
  p_Level->LiftParticles[k].dwStart = timeGetTime();

  i = rand() % 2;
  if (i)
    m = kom_najdi_material("cvytah1");
  else
    m = kom_najdi_material("cvytah1");

  if (m == -1)
    kprintf(1, "Nelze najit material cvytah1");

  pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y =
    (randf() / (float) (RAND_MAX)) / 25.0f;
  pKourovaS[0].utlum_x = 0;
  pKourovaS[0].utlum_y = 0;

  pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

  pKourovaS[0].a = 1.0f;
  pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
  pKourovaS[0].ka = 0;

  p_Level->LiftParticles[k].System = par_vyrob();

  par_set_param(p_Level->LiftParticles[k].System, m,
    TPAR_YPLANE_LOW | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR | TPAR_NO_FOG,
    (BOD *) pos, NULL);
  par_vloz_kour_stopu(p_Level->LiftParticles[k].System, pKourovaS,
    p_Level->LiftParticles[k].Sizeof);

  p_Level->LiftParticles[k].hHnizdo[0] =
    par_vloz_hnizdo(p_Level->LiftParticles[k].System);

  par_vloz_hnizdo_komplet(p_Level->LiftParticles[k].hHnizdo[0], 100000,
    (BOD *) p_Level->LiftParticles[k].pivot[0], pKourovaS);
  par_vloz_hnizdo_timer(p_Level->LiftParticles[k].hHnizdo[0], 100000, 0);

  memcpy((void *) p_Level->LiftParticles[k].pivot[0], (void *) pos,
    3 * sizeof(float));

  p_Level->LiftParticles[k].pivot[0][0] +=
    (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);
  p_Level->LiftParticles[k].pivot[0][1]--;
  p_Level->LiftParticles[k].pivot[0][2] +=
    (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);

  p_Level->LiftParticles[k].dir[0][0] = 0;
  p_Level->LiftParticles[k].dir[0][1] =
    ((-randf() / (float) (RAND_MAX)) / 10.0f) * _3DKOREKCE;

  if (p_Level->LiftParticles[k].dir[0][1] > (-0.03f) * _3DKOREKCE)
    p_Level->LiftParticles[k].dir[0][1] = (-0.03f) * _3DKOREKCE;

  p_Level->LiftParticles[k].dir[0][2] = 0;

  par_vloz_hnizdo_dir(p_Level->LiftParticles[k].hHnizdo[0],
    (BOD *) p_Level->LiftParticles[k].dir[0]);

  par_vloz_hnizdo_y_plane(p_Level->LiftParticles[k].hHnizdo[0], pos[1]);

  p_Level->LiftParticles[k].dwStart = timeGetTime();
  p_Level->LiftParticles[k].dwTime = 0;

  par_pripoj_funkci(p_Level->LiftParticles[k].System, anmend_ZrusCastice3, 0,
    0, (void *) pKourovaS);

  p_Level->LiftParticles[k].hHnizdo[1] = mesh;
  par_go(p_Level->LiftParticles[k].System, &p_Level->LiftParticles[k].flag, 0,
    0);

  return (0);
}

int gl_Get_Free_Teleport_Sparks(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < 20; i++)
    if (p_Level->TeleportSparks[i].System == -1)
      return i;

  return -1;
}

void gl_Do_Teleport_Sparks(float *pos, LEVELINFO * p_Level)
{
  int k, m, i;
  PAR_KOUR_STOPA *pKourovaS;
  int size = ftoi(60 / p_Level->KvalitaCastic);

  k = gl_Get_Free_Teleport_Sparks(p_Level);

  if (k == -1)
    return;

  pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

  if (!pKourovaS)
    return;

  p_Level->TeleportSparks[k].pCastice = pKourovaS;
  p_Level->TeleportSparks[k].Sizeof = size;
  p_Level->TeleportSparks[k].dwStart = timeGetTime();

/*	i = rand()%3;
	switch(i)
	{
	case 0:
		m = kom_najdi_material("flare8");
		break;
	case 1:
		m = kom_najdi_material("flare9");
		break;
	case 2:
		m = kom_najdi_material("flare10");
		break;
	}*/

  m = kom_najdi_material("flare105");

  if (m == -1)
    kprintf(1, "Nelze najit material flare105");

  pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y =
    (randf() / (float) (RAND_MAX)) / 10.0f;
  pKourovaS[0].utlum_x = -0.5f;
  pKourovaS[0].utlum_y = -0.5f;

  pKourovaS[0].r = 0;
  pKourovaS[0].g = 0.8f;
  pKourovaS[0].b = 0.97f;

  pKourovaS[0].a = 1.0f;
  pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da =
    0.0f;
  pKourovaS[0].ka = 0.0f;

  p_Level->TeleportSparks[k].System = par_vyrob();

  par_set_param(p_Level->TeleportSparks[k].System, m,
    TPAR_NO_FOG | TPAR_YPLANE_LOW | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE
    | TPAR_DIR, (BOD *) pos, NULL);
  par_vloz_kour_stopu(p_Level->TeleportSparks[k].System, pKourovaS,
    p_Level->TeleportSparks[k].Sizeof);

  for (i = 0; i < p_Level->TeleportSparks[k].Sizeof; i++) {
    p_Level->TeleportSparks[k].hHnizdo[i] =
      par_vloz_hnizdo(p_Level->TeleportSparks[k].System);

    par_vloz_hnizdo_komplet(p_Level->TeleportSparks[k].hHnizdo[i], 100000,
      (BOD *) p_Level->TeleportSparks[k].pivot[i], pKourovaS);
    par_vloz_hnizdo_timer(p_Level->TeleportSparks[k].hHnizdo[i], 100000,
      -100000);

    //pridej nahodnosti
    pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y =
      (randf() / (float) (RAND_MAX)) / 10.0f;

    //vygeneruj dir
    p_Level->TeleportSparks[k].dir[i][0] = 0;
    p_Level->TeleportSparks[k].dir[i][1] =
      ((-randf() / (float) (RAND_MAX)) / 100.0f) * _3DKOREKCE;
    p_Level->TeleportSparks[k].dir[i][2] = 0;

    memcpy((void *) p_Level->TeleportSparks[k].pivot[i], (void *) pos,
      3 * sizeof(float));

    p_Level->TeleportSparks[k].pivot[i][0] +=
      (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);
    p_Level->TeleportSparks[k].pivot[i][1] += randf() / (float) (RAND_MAX);
    p_Level->TeleportSparks[k].pivot[i][2] +=
      (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);

    par_vloz_hnizdo_dir(p_Level->TeleportSparks[k].hHnizdo[i],
      (BOD *) p_Level->TeleportSparks[k].dir[i]);

    par_vloz_hnizdo_y_plane(p_Level->TeleportSparks[k].hHnizdo[i],
      pos[1] - 1.0f);
  }

  p_Level->TeleportSparks[k].dwStart = timeGetTime();
  p_Level->TeleportSparks[k].dwTime = 0;

  par_pripoj_funkci(p_Level->TeleportSparks[k].System, anmend_ZrusCastice3, 0,
    0, (void *) pKourovaS);

  par_go(p_Level->TeleportSparks[k].System, &p_Level->TeleportSparks[k].flag,
    0, 0);
}

//------------------------------------------------------------------------------------------------
// teleport item
//------------------------------------------------------------------------------------------------
int gl_Teleport_Item(long Teleport, long Item, LEVELINFO * p_Level)
{
  int old_pos[3], test_pos[3];
  int new_real, test_real;
  ANIMATION_QUEUE_SET *p_prev_set;
  int rot, iConnect = 0;
  float pos[3];
  POINTERSTRUCTURE *pS;

  if (!p_Level->Level[Item])
    return -1;
  if (!p_Level->Level[Teleport])
    return -1;
//      if (p_Level->Level[p_Level->Level[Teleport]->Connection[0]]) return -1;

  new_real = p_Level->Level[Teleport]->Connection[0];

  gl_Real2Logical(new_real, test_pos, p_Level);
  test_pos[2]++;

  gl_Logical2Real(test_pos[0], test_pos[1], test_pos[2], &test_real, p_Level);

  if (p_Level->Level[test_real])
    return -1;

  //pokud teleportuje berusku bez snorchlu pod vodu, tak nic
  if (p_Level->Square[test_real].bUnderWater
    && !gl_Can_Go_Into_Water(p_Level->Level[Item]))
    return -1;

  kom_mesh_get_float(p_Level->Level[Teleport]->Index_Of_Game_Mesh,
    &pos[0], &pos[1], &pos[2], &rot);

  pS = (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

  if (pS) {
    pS->p_Level = p_Level;

    old_pos[0] = p_Level->Level[Item]->Pos[0];
    old_pos[1] = p_Level->Level[Item]->Pos[1];
    old_pos[2] = p_Level->Level[Item]->Pos[2];

    memcpy((void *) pS->viParam1, (void *) old_pos, 3 * sizeof(int));
    memcpy((void *) pS->viParam2, (void *) test_pos, 3 * sizeof(int));
  }

  p_prev_set = gl_Get_Prev_Queue_Set(p_Level);

  if (p_Level->Level[Item]->p_Object->Class == 13 || !p_Level->Flip
    || !p_prev_set->last) {
    anmend_TeleportStart((int) p_Level->Level[Item], Teleport, pS);
  }
  else {
    if (gl_Multi_Animation(p_prev_set, &p_prev_set->animation[0]))
      iConnect = 1;

    rani_pripoj_funkci(p_prev_set->animation[iConnect].p_run,
      anmend_Teleport, (int) p_Level->Level[Item], Teleport, pS);

    if (!p_Level->Sikmina_FlagOld)
      am_Set_Triger_Function(&p_prev_set->animation[iConnect],
        anmend_TeleportStart, (int) p_Level->Level[Item], Teleport, pS, 1, 0);
    else
      am_Set_Triger_Function(&p_prev_set->animation[iConnect],
        anmend_TeleportStart, (int) p_Level->Level[Item],
        Teleport, pS, 50, 0);

    if (p_Level->Level[Item]->p_Object->Class == 1
      && Item == p_Level->Actual_Item && p_Level->bPosouvatKameru)
      cameraflag = 0;
  }

  p_Level->status = 1;
  p_Level->bAllow_Key = 0;
  return test_real;
}

int gl_Get_Column_Height(int x, int y, int z, LEVELINFO * p_Level)
{
  int h = 1, i;
  int r;

  for (i = z; i < p_Level->Size[2]; i += 2) {
    gl_Logical2Real(x, y, i, &r, p_Level);

    if (p_Level->Level[r])
      break;

    if (i + 1 >= p_Level->Size[2])
      break;

    gl_Logical2Real(x, y, i + 1, &r, p_Level);

    if (p_Level->Level[r]) {
      if (p_Level->Level[r]->p_Object->Class == 2 ||
        p_Level->Level[r]->p_Object->Class == 17 ||
        p_Level->Level[r]->p_Object->Class == 19)
        break;
      else
        h++;
    }
    else
      break;
  }

  return h;
}

int gl_Check_Throw_off_Water(int *column, int z, LEVELINFO * p_Level)
{
  int real;

  gl_Logical2Real(column[0], column[1], z - 1, &real, p_Level);

  if (p_Level->Level[real])
    if (p_Level->Level[real]->p_Object->Class == 12)
      return 1;

  return 0;
}

//------------------------------------------------------------------------------------------------
// setrepava sloupce
//------------------------------------------------------------------------------------------------
int gl_Throw_off(int *column, LEVELINFO * p_Level)
{
  int move[100][2];
  int real_pos1, real_pos2;
  int test_pos1, test_pos2;
  int iValue1[3], iValue2[3], iValue3[3];       //, iValue4[3];
  int i, m, test, n;
  int w;
  int ret = 0;
  ITEMDESC *p_test_pos1, *p_test_pos2;
  int test_column[3];
  int counter = 0;
  int voda = -1, vc = 0;
  int LastMesh = -1;
  int vodapos[3];
  int VyskaSloupce = 0;
  int VyskaPadu = 0;
  int Korekce = 0;
  int x, y;

  if ((column[0] < 0) || (column[0] > (p_Level->Size[0] - 1)))
    return 0;
  if ((column[1] < 0) || (column[1] > (p_Level->Size[1] - 1)))
    return 0;

  for (i = 0; i < p_Level->Size[2]; i++) {
    gl_Logical2Real(column[0], column[1], i, &real_pos1, p_Level);
    if (p_Level->Level[real_pos1]) {
      move[i][0] = i;
      move[i][1] = i;
    }
    else {
      move[i][0] = -1;
      move[i][1] = -1;
    }
  }

  for (i = 1; i < p_Level->Size[2]; i += 2) {
    if (move[i][0] != -1) {
      gl_Logical2Real(column[0], column[1], i, &test_pos1, p_Level);

      p_test_pos1 = p_Level->Level[test_pos1];

      if (p_test_pos1->p_Object->Class == 2)
        continue;

      if (move[i - 1][0] != -1) {
        gl_Logical2Real(column[0], column[1], i - 1, &test_pos1, p_Level);

        p_test_pos1 = p_Level->Level[test_pos1];

        if (p_test_pos1->p_Object->Class != 12)
          continue;
      }

      for (m = i - 2; m >= 1; m -= 2) {
        if (i - 2 > 0) {
          gl_Logical2Real(column[0], column[1], i - 2, &test_pos1, p_Level);

          p_test_pos1 = p_Level->Level[test_pos1];

          if (p_test_pos1)
            if (p_test_pos1->p_Object->Class != 1 &&
              p_test_pos1->p_Object->Class != 5 &&
              p_test_pos1->p_Object->Class != 6 &&
              p_test_pos1->p_Object->Class != 7 &&
              p_test_pos1->p_Object->Class != 13)
              break;
        }

        if ((move[m][0] != -1) || (move[m + 1][0] != -1)) {

          gl_Logical2Real(column[0], column[1], i, &test_pos1, p_Level);
          gl_Logical2Real(column[0], column[1], (m + 1), &test_pos2, p_Level);

          p_test_pos1 = p_Level->Level[test_pos1];
          p_test_pos2 = p_Level->Level[test_pos2];

          if (!p_test_pos1 || !p_test_pos2)
            break;

          if (p_test_pos2->p_Object->Class == 12) {
            memcpy((void *) vodapos, (void *) p_test_pos2->Pos,
              3 * sizeof(int));

            if (p_Level->Level[test_pos1]->p_Object->Class == 5 &&
              p_Level->Level[test_pos1]->p_Object->SubClass == 3)
              Korekce = 1;
            else
              Korekce = 0;

            if (m + 2 != p_Level->Level[test_pos1]->Pos[2] || !Korekce) {
              voda = m + 1;
              VyskaPadu = (move[i][0] - m + 1) / 2;
            }
          }
          else
            break;

          if ((p_test_pos1->p_Object->Class != 5) &&
            (p_test_pos2->p_Object->Class == 12))
            goto THROW_OFF_PROPAD;
          else {
            test_column[0] = column[0];
            test_column[1] = column[1];
            test_column[2] = i;
            w = gl_Count_Weight(test_column, p_Level);

            if ((p_test_pos1->p_Object->Class == 5) &&
              (p_test_pos2->p_Object->Class == 12))
              if ((p_test_pos1->p_Object->SubClass == 3) && (w < 3))
                break;
              else {
              THROW_OFF_PROPAD:

                test = 0;

                for (n = m; n >= 0; n -= 2)
                  if (move[n][0] == -1)
                    test++;

                if (!test)
                  break;
              }
          }
        }
      }

      if (i > 2) {
        gl_Logical2Real(column[0], column[1], (m + 1), &test_pos2, p_Level);

        if (p_Level->Level[test_pos2])
          if (p_Level->Level[test_pos2]->p_Object->Class == 4 ||
            p_Level->Level[test_pos2]->p_Object->Class == 8)
            m += 4;
          else
            m += 2;
        else
          m += 2;

        move[i][1] = m;

        if (m == 1 && gl_Check_Throw_off_Water(column, m, p_Level)) {
          VyskaPadu = (move[i][0] - m + 1) / 2;
          voda = 0;
          vodapos[0] = column[0];
          vodapos[1] = column[1];
          vodapos[2] = voda;
        }
      }
    }

    if (((move[i][0] - move[i][1]) >= 2) && (move[i][1] > -1)) {

      gl_Logical2Real(column[0], column[1], move[i][0], &real_pos1, p_Level);
      gl_Logical2Real(column[0], column[1], move[i][1], &real_pos2, p_Level);

      p_test_pos1 = p_Level->Level[real_pos1];
      p_test_pos2 = p_Level->Level[real_pos2];

      if ((p_test_pos1->p_Object->Class != 2) &&
        (p_test_pos1->p_Object->Class != 17) &&
        (p_test_pos1->p_Object->Class != 19)) {

        if (!p_test_pos2) {
          int canim = p_set->last;

          ret = 1;
          p_Level->Throw_off = 1;

          VyskaSloupce =
            gl_Get_Column_Height(column[0], column[1], move[i][0] + 1,
            p_Level) - Korekce;

          iValue1[0] = column[0];
          iValue1[1] = column[1];
          iValue1[2] = move[i][0];

          iValue2[0] = column[0];
          iValue2[1] = column[1];
          iValue2[2] = move[i][1];

          gl_Get_Move(iValue1, iValue2, iValue3);

          if (!vc && voda > -1) {
            //bedna prosla vodou -> bubl,bubl,....
            POINTERSTRUCTURE *pS =
              (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));
            int r;
            int percent, percent1;

            if (pS) {
              gl_Logical2Real(iValue1[0], iValue1[1], voda, &r, p_Level);

              vc++;

              pS->p_Level = p_Level;
              if (p_test_pos1->p_Object->Class != 13) {
                pS->iParam = VyskaSloupce;
                pS->bParam = VyskaPadu;
                pS->viParam2[0] = 0;
                pS->viParam2[1] = 0;
                pS->viParam2[2] = 0;

                percent =
                  (int) floor((float) ((iValue1[2] - voda -
                      1) * 100) / (float) abs(iValue3[2] * 2));

                if (percent > 99)
                  percent = 95;

                percent1 =
                  (int) floor((float) (VyskaSloupce * 100) /
                  (float) abs(iValue3[2] * 2));
              }
              else {
                pS->iParam = 1;
                pS->bParam = VyskaPadu / 2;
                pS->viParam2[0] = 1;
                pS->viParam2[1] = 0;
                pS->viParam2[2] = 0;

                x =
                  (int) floor(((iValue1[2] - voda -
                      1) * 100) / (float) abs(iValue3[2] * 2));
                y =
                  (int) floor((((-1 * iValue3[2] * 2) -
                      1) * 100) / (float) ((-1 * iValue3[2] * 2) + 4));

                percent = (x * y) / 100;

                x = (int) floor((100 / (float) abs(iValue3[2] * 2)));

                percent1 = (x * y) / 100;
              }

              memcpy((void *) pS->viParam1, (void *) vodapos,
                3 * sizeof(int));

              if (voda > iValue2[2])
                if (voda + 1 == iValue1[2]) {
                  am_Set_Triger_Function(&p_set->animation[canim],
                    anmend_Water, 1, r, pS, percent, 0);

                  am_Set_Triger_Function(&p_set->animation[canim],
                    anmend_Cakanec, 1, r, pS, percent + percent1, 1);
                }
                else {
                  am_Set_Triger_Function(&p_set->animation[canim],
                    anmend_Water, 0, r, pS, percent, 0);

                  am_Set_Triger_Function(&p_set->animation[canim],
                    anmend_Cakanec, 0, r, pS, percent + percent1, 1);
                }
              else
                am_Set_Triger_Function(&p_set->animation[canim], anmend_Water,
                  2, r, pS, percent, 0);
            }
          }

          if (p_Level->Level[real_pos1]->p_Object->Class == 13) {
            if (LastMesh != -1) {
              p_set->PrivazanyMesh[p_set->pMesh].TopMesh = LastMesh;
              p_set->PrivazanyMesh[p_set->pMesh].LowMesh =
                p_Level->Level[real_pos1]->Index_Of_Game_Mesh;

              p_set->pMesh++;
            }
            else {
              //pada jako prvni -> pada sam
              int r = rand() % 2;
              BOD b(1, 1, 1);

              am_Remove_Animate_item(real_pos1, p_Level);

              p_set->animation[p_set->last].p_anim =
                sim_vyrob_animaci(3, 3, 0);

              sim_vloz_pivot(p_set->animation[p_set->last].p_anim, 0.0f, 1.0f,
                0.0f);

              sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 0,
                &b, 0, 0);

              if (r)
                sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 1,
                  &b, (-1 * iValue3[2]) * 20.0f, (-1 * iValue3[2] * 2) - 1);
              else
                sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 1,
                  &b, iValue3[2] * 20.0f, (-1 * iValue3[2] * 2) - 1);

              sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 2,
                &b, 0.0f, (-1 * iValue3[2] * 2) + 4);

              sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 0,
                0.0f, 0.0f, 0.0f, 0);
              sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 1,
                0.0f, (float) (iValue3[2] * 2), 0.0f,
                (-1 * iValue3[2] * 2) - 1);

              sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 2,
                0.0f, (float) (iValue3[2] * 2), 0.0f,
                (-1 * iValue3[2] * 2) + 4);

              sim_interpoluj_animaci(p_set->animation[p_set->last].p_anim,
                (-1 * iValue3[2]) * 2 + 5, 0);

              p_set->animation[p_set->last].p_run =
                rani_aktivuj_cekej(p_set->animation[p_set->last].p_anim,
                &p_set->animation[p_set->last].flag);

              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_Level->Level[real_pos1]->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_Level->Level[real_pos1]->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_Level->Level[real_pos1]->Index_Of_Game_Mesh;

              am_Set_Triger_Function(&p_set->animation[p_set->last],
                anmend_Item_Fall, 0, real_pos2, p_Level,
                (int) floor((float) (((-1 * iValue3[2]) * 2 +
                      2) * 100) / (float) (((-1 * iValue3[2]) * 2) + 5)), 1);

              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                anmend_Item_FallStartAnim, 0, real_pos2, p_Level);

              if (!counter && p_Level->Actual_Item != -1)
                am_Set_Start_Function(&p_set->animation[p_set->last],
                  anmend_Add_Beetle_Animation,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                  p_Level->Level[p_Level->Actual_Item]->Rotation,
                  (void *) p_Level, 0);
              p_set->last++;
              counter++;
            }
          }
          else {
            p_set->animation[canim].p_anim = sim_vyrob_animaci(2, 0, 0);

            sim_vloz_klic_posun(p_set->animation[canim].p_anim, 0, 0.0f, 0.0f,
              0.0f, 0);

            sim_vloz_klic_posun(p_set->animation[canim].p_anim, 1, 0.0f,
              (float) (iValue3[2] * 2), 0.0f, (-1 * iValue3[2] * 2) - 1);

            sim_interpoluj_animaci(p_set->animation[canim].p_anim,
              (-1 * iValue3[2]) * 2, 0);
            p_set->animation[canim].p_run =
              rani_aktivuj_cekej(p_set->animation[canim].p_anim,
              &p_set->animation[canim].flag);

            p_set->animation[canim].p_matrix =
              rani_privaz_mesh(p_set->animation[canim].p_run,
              p_test_pos1->Index_Of_Game_Mesh, 0);

            gl_Add_Mesh(&p_set->animation[canim],
              p_test_pos1->Index_Of_Game_Mesh);
            LastMesh = p_test_pos1->Index_Of_Game_Mesh;
            //LastMesh = p_set->animation[canim].mesh = p_test_pos1->Index_Of_Game_Mesh;

            p_set->last++;
          }

          if (p_Level->Level[real_pos1]->a_run != -1
            && p_Level->Level[real_pos1]->p_Object->Class != 13) {
            //rani_rozvaz(p_Level->Level[real_pos1]->a_run,p_Level->Level[real_pos1]->Index_Of_Game_Mesh);
            rani_zrus(p_Level->Level[real_pos1]->a_run);
            p_Level->Level[real_pos1]->a_run = -1;
            kom_umisti_prvek(p_Level->Level[real_pos1]->Index_Of_Game_Mesh,
              p_Level->Level[real_pos1]->Pos[0],
              p_Level->Level[real_pos1]->Pos[2],
              p_Level->Level[real_pos1]->Pos[1],
              p_Level->Level[real_pos1]->Rotation);

          }

          p_Level->Level[real_pos2] = p_Level->Level[real_pos1];
          if (real_pos1 == p_Level->Actual_Item)
            p_Level->Actual_Item = real_pos2;

          p_Level->Level[real_pos1] = 0;
          move[i][0] = -1;
          move[i][1] = -1;
          p_test_pos2 = p_Level->Level[real_pos2];
          p_test_pos2->Pos[0] = iValue2[0];
          p_test_pos2->Pos[1] = iValue2[1];
          p_test_pos2->Pos[2] = iValue2[2];

          if (!counter && p_Level->Actual_Item != -1)
            am_Set_Start_Function(&p_set->animation[p_set->last - 1],
              anmend_Add_Beetle_Animation,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
              p_Level->Level[p_Level->Actual_Item]->Rotation,
              (void *) p_Level, 0);

          if (!counter && voda < 0) {
            rani_pripoj_funkci(p_set->animation[canim].p_run, anmend_PadBedny,
              (int) p_Level, 0, p_test_pos2);
            counter++;
          }
/*			else
				if(counter && voda >= 0 && p_set->pMesh)
					rani_pripoj_funkci(p_set->animation[canim].p_run, anmend_PadBedny, (int)p_Level, 0, p_test_pos2);*/

          //presunu column o pad niz, jelikos se setrasa od spodu navrch, tak posledni
          // column[2] bude nejvissiho prvku
          p_Level->Column[2] = iValue2[2];

          //test na hrebik
          gl_Logical2Real(iValue2[0], iValue2[1], iValue2[2] - 1, &real_pos1,
            p_Level);
          p_test_pos1 = p_Level->Level[real_pos1];

          if (p_test_pos1)
            if (p_test_pos1->p_Object->Class == 16) {
              if (p_test_pos1->Square.Flek.pFlek) {
                kom_flek_zrus(p_test_pos1->Square.Flek.pFlek);
                p_test_pos1->Square.Flek.pFlek = 0;
              }

              kom_zrus_prvek(p_test_pos1->Index_Of_Game_Mesh);
              p_Level->Level[real_pos1] = 0;
            }
          real_pos2 = 0;
          real_pos1 = 0;
          move[m][0] = 1;
          move[m][1] = 1;
        }
      }
    }
  }


  return ret;
}

//------------------------------------------------------------------------------------------------
// setrepava sloupce
//------------------------------------------------------------------------------------------------
int gl_Throw_offAnim(int *column, LEVELINFO * p_Level)
{
  int move[100][2];
  int real_pos1, real_pos2;
  int test_pos1, test_pos2;
  int iValue1[3], iValue2[3], iValue3[3];       //, iValue4[3];
  int i, m, test, n;
  int w;
  int ret = 0;
  ITEMDESC *p_test_pos1, *p_test_pos2;
  int test_column[3];
  int counter = 0;
  int voda = -1, vc = 0;
  int LastMesh = -1;
  int vodapos[3];
  int VyskaSloupce = 0;
  int VyskaPadu = 0;
  int Korekce = 0;
  int x, y;

  if ((column[0] < 0) || (column[0] > (p_Level->Size[0] - 1)))
    return 0;
  if ((column[1] < 0) || (column[1] > (p_Level->Size[1] - 1)))
    return 0;

  for (i = 0; i < p_Level->Size[2]; i++) {
    gl_Logical2Real(column[0], column[1], i, &real_pos1, p_Level);
    if (p_Level->Level[real_pos1]) {
      move[i][0] = i;
      move[i][1] = i;
    }
    else {
      move[i][0] = -1;
      move[i][1] = -1;
    }
  }

  for (i = 1; i < p_Level->Size[2]; i += 2) {
    if (move[i][0] != -1) {
      if (move[i - 1][0] != -1) {
        gl_Logical2Real(column[0], column[1], i - 1, &test_pos1, p_Level);

        p_test_pos1 = p_Level->Level[test_pos1];

        if (p_test_pos1->p_Object->Class != 12)
          continue;
      }

      for (m = i - 2; m >= 1; m -= 2) {
        if (i - 2 > 0) {
          gl_Logical2Real(column[0], column[1], i - 2, &test_pos1, p_Level);

          p_test_pos1 = p_Level->Level[test_pos1];

          if (p_test_pos1)
            if (p_test_pos1->p_Object->Class != 1 &&
              p_test_pos1->p_Object->Class != 5 &&
              p_test_pos1->p_Object->Class != 6 &&
              p_test_pos1->p_Object->Class != 7 &&
              p_test_pos1->p_Object->Class != 13)
              break;
        }

        if ((move[m][0] != -1) || (move[m + 1][0] != -1)) {

          gl_Logical2Real(column[0], column[1], i, &test_pos1, p_Level);
          gl_Logical2Real(column[0], column[1], (m + 1), &test_pos2, p_Level);

          p_test_pos1 = p_Level->Level[test_pos1];
          p_test_pos2 = p_Level->Level[test_pos2];

          if (!p_test_pos1 || !p_test_pos2)
            break;

          if (p_test_pos2->p_Object->Class == 12) {
            memcpy((void *) vodapos, (void *) p_test_pos2->Pos,
              3 * sizeof(int));

            if (p_Level->Level[test_pos1]->p_Object->Class == 5 &&
              p_Level->Level[test_pos1]->p_Object->SubClass == 3)
              Korekce = 1;
            else
              Korekce = 0;

            if (m + 2 != p_Level->Level[test_pos1]->Pos[2] || !Korekce) {
              voda = m + 1;
              VyskaPadu = (move[i][0] - m + 1) / 2;
            }
          }
          else
            break;

          if ((p_test_pos1->p_Object->Class != 5) &&
            (p_test_pos2->p_Object->Class == 12))
            goto THROW_OFF_PROPAD;
          else {
            test_column[0] = column[0];
            test_column[1] = column[1];
            test_column[2] = i;
            w = gl_Count_Weight(test_column, p_Level);

            if ((p_test_pos1->p_Object->Class == 5) &&
              (p_test_pos2->p_Object->Class == 12))
              if ((p_test_pos1->p_Object->SubClass == 3) && (w < 3))
                break;
              else {
              THROW_OFF_PROPAD:

                test = 0;

                for (n = m; n >= 0; n -= 2)
                  if (move[n][0] == -1)
                    test++;

                if (!test)
                  break;
              }
          }
        }
      }

      if (i > 2) {
        gl_Logical2Real(column[0], column[1], (m + 1), &test_pos2, p_Level);

        if (p_Level->Level[test_pos2])
          if (p_Level->Level[test_pos2]->p_Object->Class == 4 ||
            p_Level->Level[test_pos2]->p_Object->Class == 9)
            m += 4;
          else
            m += 2;
        else
          m += 2;

        move[i][1] = m;
      }
    }

    if (((move[i][0] - move[i][1]) >= 2) && (move[i][1] > -1)) {

      gl_Logical2Real(column[0], column[1], move[i][0], &real_pos1, p_Level);
      gl_Logical2Real(column[0], column[1], move[i][1], &real_pos2, p_Level);

      p_test_pos1 = p_Level->Level[real_pos1];
      p_test_pos2 = p_Level->Level[real_pos2];

      if ((p_test_pos1->p_Object->Class != 2) &&
        (p_test_pos1->p_Object->Class != 17) &&
        (p_test_pos1->p_Object->Class != 19)) {

        if (!p_test_pos2) {
          int canim = p_set_anim->last;

          ret = 1;
          p_Level->Throw_off = 1;

          VyskaSloupce =
            gl_Get_Column_Height(column[0], column[1], move[i][0] + 1,
            p_Level) - Korekce;

          iValue1[0] = column[0];
          iValue1[1] = column[1];
          iValue1[2] = move[i][0];

          iValue2[0] = column[0];
          iValue2[1] = column[1];
          iValue2[2] = move[i][1];

          gl_Get_Move(iValue1, iValue2, iValue3);

          if (!vc && voda > -1) {
            //bedna prosla vodou -> bubl,bubl,....
            POINTERSTRUCTURE *pS =
              (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));
            int r;
            int percent, percent1;

            if (pS) {
              gl_Logical2Real(iValue1[0], iValue1[1], voda, &r, p_Level);

              vc++;

              pS->p_Level = p_Level;
              if (p_test_pos1->p_Object->Class != 13) {
                pS->iParam = VyskaSloupce;
                pS->bParam = VyskaPadu;
                pS->viParam2[0] = 0;
                pS->viParam2[1] = 0;
                pS->viParam2[2] = 0;

                percent =
                  (int) floor((float) ((iValue1[2] - voda -
                      1) * 100) / (float) abs(iValue3[2] * 2));

                percent1 =
                  (int) floor((float) (VyskaSloupce * 100) /
                  (float) abs(iValue3[2] * 2));
              }
              else {
                pS->iParam = 1;
                pS->bParam = VyskaPadu / 2;
                pS->viParam2[0] = 1;
                pS->viParam2[1] = 0;
                pS->viParam2[2] = 0;

                x =
                  (int) floor(((iValue1[2] - voda -
                      1) * 100) / (float) abs(iValue3[2] * 2));
                y =
                  (int) floor((((-1 * iValue3[2] * 2) -
                      1) * 100) / (float) ((-1 * iValue3[2] * 2) + 4));

                percent = (x * y) / 100;

                x = (int) floor((100 / (float) abs(iValue3[2] * 2)));

                percent1 = (x * y) / 100;
              }

              memcpy((void *) pS->viParam1, (void *) vodapos,
                3 * sizeof(int));

              if (voda > iValue2[2])
                if (voda + 1 == iValue1[2]) {
                  am_Set_Triger_Function(&p_set_anim->animation[canim],
                    anmend_Water, 1, r, pS, percent, 0);

                  am_Set_Triger_Function(&p_set_anim->animation[canim],
                    anmend_Cakanec, 1, r, pS, percent + percent1, 1);
                }
                else {
                  am_Set_Triger_Function(&p_set_anim->animation[canim],
                    anmend_Water, 0, r, pS, percent, 0);

                  am_Set_Triger_Function(&p_set_anim->animation[canim],
                    anmend_Cakanec, 0, r, pS, percent + percent1, 1);
                }
              else
                am_Set_Triger_Function(&p_set_anim->animation[canim],
                  anmend_Water, 2, r, pS, percent, 0);
            }
          }

          if (p_Level->Level[real_pos1]->p_Object->Class == 13) {
            if (LastMesh != -1) {
              p_set_anim->PrivazanyMesh[p_set_anim->pMesh].TopMesh = LastMesh;
              p_set_anim->PrivazanyMesh[p_set_anim->pMesh].LowMesh =
                p_Level->Level[real_pos1]->Index_Of_Game_Mesh;

              p_set_anim->pMesh++;
            }
            else {
              //pada jako prvni -> pada sam
              int r = rand() % 2;
              BOD b(1, 1, 1);

              am_Remove_Animate_item(real_pos1, p_Level);

              p_set_anim->animation[p_set_anim->last].p_anim =
                sim_vyrob_animaci(3, 3, 0);

              sim_vloz_pivot(p_set_anim->animation[p_set_anim->last].p_anim,
                0.0f, 1.0f, 0.0f);

              sim_vloz_klic_rotace(p_set_anim->animation[p_set_anim->last].
                p_anim, 0, &b, 0, 0);

              if (r)
                sim_vloz_klic_rotace(p_set_anim->animation[p_set_anim->last].
                  p_anim, 1, &b, (-1 * iValue3[2]) * 20.0f,
                  (-1 * iValue3[2] * 2) - 1);
              else
                sim_vloz_klic_rotace(p_set_anim->animation[p_set_anim->last].
                  p_anim, 1, &b, iValue3[2] * 20.0f,
                  (-1 * iValue3[2] * 2) - 1);

              sim_vloz_klic_rotace(p_set_anim->animation[p_set_anim->last].
                p_anim, 2, &b, 0.0f, (-1 * iValue3[2] * 2) + 4);

              sim_vloz_klic_posun(p_set_anim->animation[p_set_anim->last].
                p_anim, 0, 0.0f, 0.0f, 0.0f, 0);
              sim_vloz_klic_posun(p_set_anim->animation[p_set_anim->last].
                p_anim, 1, 0.0f, (float) (iValue3[2] * 2), 0.0f,
                (-1 * iValue3[2] * 2) - 1);

              sim_vloz_klic_posun(p_set_anim->animation[p_set_anim->last].
                p_anim, 2, 0.0f, (float) (iValue3[2] * 2), 0.0f,
                (-1 * iValue3[2] * 2) + 4);

              sim_interpoluj_animaci(p_set_anim->animation[p_set_anim->last].
                p_anim, (-1 * iValue3[2]) * 2 + 5, 0);

              p_set_anim->animation[p_set_anim->last].p_run =
                rani_aktivuj(p_set_anim->animation[p_set_anim->last].p_anim,
                &p_set_anim->animation[p_set_anim->last].flag, 0, 0, 0);

              p_set_anim->animation[p_set_anim->last].p_matrix =
                rani_privaz_mesh(p_set_anim->animation[p_set_anim->last].
                p_run, p_Level->Level[real_pos1]->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set_anim->last],
                p_Level->Level[real_pos1]->Index_Of_Game_Mesh);
              //p_set_anim->animation[p_set_anim->last].mesh = p_Level->Level[real_pos1]->Index_Of_Game_Mesh;

              am_Set_Triger_Function(&p_set_anim->animation[p_set_anim->last],
                anmend_Item_Fall, 0, real_pos2, p_Level,
                (int) floor((float) (((-1 * iValue3[2]) * 2 +
                      2) * 100) / (float) (((-1 * iValue3[2]) * 2) + 5)), 1);

              rani_pripoj_funkci(p_set_anim->animation[p_set_anim->last].
                p_run, anmend_Item_FallStartAnim, 0, real_pos2, p_Level);

              if (!counter && p_Level->Actual_Item != -1)
                am_Set_Start_Function(&p_set_anim->animation[p_set_anim->
                    last], anmend_Add_Beetle_Animation,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                  p_Level->Level[p_Level->Actual_Item]->Rotation,
                  (void *) p_Level, 0);
              p_set_anim->last++;
              counter++;
            }
          }
          else {
            p_set_anim->animation[canim].p_anim = sim_vyrob_animaci(2, 0, 0);

            sim_vloz_klic_posun(p_set_anim->animation[canim].p_anim, 0, 0.0f,
              0.0f, 0.0f, 0);

            sim_vloz_klic_posun(p_set_anim->animation[canim].p_anim, 1, 0.0f,
              (float) (iValue3[2] * 2), 0.0f, (-1 * iValue3[2] * 2) - 1);

            sim_interpoluj_animaci(p_set_anim->animation[canim].p_anim,
              (-1 * iValue3[2]) * 2, 0);
            p_set_anim->animation[canim].p_run =
              rani_aktivuj(p_set_anim->animation[canim].p_anim,
              &p_set_anim->animation[canim].flag, 0, 0, 0);

            p_set_anim->animation[canim].p_matrix =
              rani_privaz_mesh(p_set_anim->animation[canim].p_run,
              p_test_pos1->Index_Of_Game_Mesh, 0);

            gl_Add_Mesh(&p_set->animation[canim],
              p_test_pos1->Index_Of_Game_Mesh);
            //LastMesh = p_set_anim->animation[canim].mesh = p_test_pos1->Index_Of_Game_Mesh;
            LastMesh = p_test_pos1->Index_Of_Game_Mesh;

            p_set_anim->last++;
          }

          if (p_Level->Level[real_pos1]->a_run != -1
            && p_Level->Level[real_pos1]->p_Object->Class != 13) {
            //rani_rozvaz(p_Level->Level[real_pos1]->a_run,p_Level->Level[real_pos1]->Index_Of_Game_Mesh);
            rani_zrus(p_Level->Level[real_pos1]->a_run);
            p_Level->Level[real_pos1]->a_run = -1;
            kom_umisti_prvek(p_Level->Level[real_pos1]->Index_Of_Game_Mesh,
              p_Level->Level[real_pos1]->Pos[0],
              p_Level->Level[real_pos1]->Pos[2],
              p_Level->Level[real_pos1]->Pos[1],
              p_Level->Level[real_pos1]->Rotation);

          }

          p_Level->Level[real_pos2] = p_Level->Level[real_pos1];
          if (real_pos1 == p_Level->Actual_Item)
            p_Level->Actual_Item = real_pos2;

          p_Level->Level[real_pos1] = 0;
          move[i][0] = -1;
          move[i][1] = -1;
          p_test_pos2 = p_Level->Level[real_pos2];
          p_test_pos2->Pos[0] = iValue2[0];
          p_test_pos2->Pos[1] = iValue2[1];
          p_test_pos2->Pos[2] = iValue2[2];

          if (!counter && p_Level->Actual_Item != -1)
            am_Set_Start_Function(&p_set_anim->animation[p_set_anim->last -
                1], anmend_Add_Beetle_Animation,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
              p_Level->Level[p_Level->Actual_Item]->Rotation, p_Level, 0);

          if (!counter && voda < 0) {
            rani_pripoj_funkci(p_set_anim->animation[canim].p_run,
              anmend_PadBedny, (int) p_Level, 0, p_test_pos2);
            counter++;
          }

          //presunu column o pad niz, jelikos se setrasa od spodu navrch, tak posledni
          // column[2] bude nejvissiho prvku
          p_Level->Column[2] = iValue2[2];

          //test na hrebik
          gl_Logical2Real(iValue2[0], iValue2[1], iValue2[2] - 1, &real_pos1,
            p_Level);
          p_test_pos1 = p_Level->Level[real_pos1];

          if (p_test_pos1)
            if (p_test_pos1->p_Object->Class == 16) {
              if (p_test_pos1->Square.Flek.pFlek) {
                kom_flek_zrus(p_test_pos1->Square.Flek.pFlek);
                p_test_pos1->Square.Flek.pFlek = 0;
              }

              kom_zrus_prvek(p_test_pos1->Index_Of_Game_Mesh);
              p_Level->Level[real_pos1] = 0;
            }
          real_pos2 = 0;
          real_pos1 = 0;
          move[m][0] = 1;
          move[m][1] = 1;
        }
      }
    }
  }


  return ret;
}

//------------------------------------------------------------------------------------------------
// prevede cislo na string fleku
//------------------------------------------------------------------------------------------------
void gl_i2FStr(int i, char *cStr)
{
  char text[32];

  strcpy(cStr, "flek");
  if (i < 10)
    strcat(cStr, "0");

  strcat(cStr, itoa(i, text, 10));
  strcat(cStr, "\0");
}

void gl_Create_Flek(int *iPos, LEVELINFO * p_Level)
{
  FLEK_K *pFlek;
  float pos[3];

  pFlek = &p_Level->SelectionFlek;

  pFlek->Flag = K_CHYBA;

  kom_get_fyz_souradnice(iPos[0], iPos[2], iPos[1], (BOD *) pos);

  memcpy(pFlek->pos, pos, 3 * sizeof(float));
  pFlek->Material = kom_najdi_material("select");
  pFlek->nx[0] = 1;
  pFlek->nx[1] = 0;
  pFlek->nx[2] = 0;
  pFlek->nz[0] = 0;
  pFlek->nz[1] = 0;
  pFlek->nz[2] = 1;
  pFlek->dx = 1;
  pFlek->dy = 1;
  pFlek->Rotation = 0;

  if (pFlek->Material == -1)
    kprintf(1, "Nelze najit material select");

  pFlek->pFlek = kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos,
    (BOD *) & pFlek->nx, (BOD *) & pFlek->nz,
    pFlek->dx, pFlek->dy, pFlek->Rotation, 2, pFlek->Material);
}

void gl_TEST(LEVELINFO * p_Level)
{
  int Pos[3], i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Square[i].bUnderWater == 0) {
      gl_Real2Logical(i, Pos, p_Level);

      if (Pos[2] & 0x1)
        gl_Create_Flek(Pos, p_Level);
    }
}

void gl_Set_Underwater_Level(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    p_Level->Square[i].bUnderWater = 1;
}

void gl_Set_Underwater_Level_Snorchl(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Square[i].bUnderWater == 1)
      if (p_Level->Level[i])
        if (p_Level->Level[i]->p_Object->Class == 1)
          p_Level->Level[i]->p_Back_Pack->item[0] = 1;
}

void gl_Create_SelectionFlek(ITEMDESC * p_Item, LEVELINFO * p_Level)
{
  FLEK_K *pFlek;
  float pos[3];

  pFlek = &p_Level->SelectionFlek;

  pFlek->Flag = K_CHYBA;

  kom_get_fyz_souradnice(p_Item->Pos[0], p_Item->Pos[2], p_Item->Pos[1],
    (BOD *) pos);

  pos[1] += 0.1f;

  memcpy(pFlek->pos, pos, 3 * sizeof(float));

  pFlek->Material = kom_najdi_material("select");
  pFlek->nx[0] = 1;
  pFlek->nx[1] = 0;
  pFlek->nx[2] = 0;
  pFlek->nz[0] = 0;
  pFlek->nz[1] = 0;
  pFlek->nz[2] = 1;
  pFlek->dx = 1;
  pFlek->dy = 1;
  pFlek->Rotation = 0;

  if (pFlek->Material == -1)
    kprintf(1, "Nelze najit material select");

  pFlek->pFlek = kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos,
    (BOD *) & pFlek->nx, (BOD *) & pFlek->nz,
    pFlek->dx, pFlek->dy, pFlek->Rotation, 2, pFlek->Material);

  kom_prvek_viditelnost(p_Level->iKursorMesh, 1);
  ber_umisti_prvek(p_Level->iKursorMesh, p_Item->Pos[0], p_Item->Pos[2] + 1,
    p_Item->Pos[1], p_Item->Rotation);

  p_Level->iKursorAnimation =
    rani_aktivuj(am.sim_anim[34], &p_Level->TrashFlag, GK_LOOP, 0, 0);
  rani_privaz_mesh(p_Level->iKursorAnimation, p_Level->iKursorMesh, 0);
}

void gl_Select_BeatleFlek(ITEMDESC * p_Item, LEVELINFO * p_Level)
{
  FLEK_K *pFlek;
  float pos[3];

  pFlek = &p_Level->SelectionFlek;

  kom_get_fyz_souradnice(p_Item->Pos[0], p_Item->Pos[2], p_Item->Pos[1],
    (BOD *) pos);

  pos[1] += 0.1f;

  memcpy(pFlek->pos, pos, 3 * sizeof(float));

  kom_flek_zmen(pFlek->pFlek, (BOD *) pFlek->pos, (BOD *) pFlek->nx,
    (BOD *) pFlek->nz, pFlek->dx, pFlek->dy, pFlek->Rotation, 2,
    pFlek->Material);

  kom_flek_setflag(pFlek->pFlek, 0);

  if (p_Level->iKursorAnimation != K_CHYBA) {
    rani_zrus(p_Level->iKursorAnimation);
    p_Level->iKursorAnimation = K_CHYBA;
  }

  kom_prvek_viditelnost(p_Level->iKursorMesh, 1);
  ber_umisti_prvek(p_Level->iKursorMesh, p_Item->Pos[0], p_Item->Pos[2] + 1,
    p_Item->Pos[1], p_Item->Rotation);

  p_Level->iKursorAnimation =
    rani_aktivuj(am.sim_anim[34], &p_Level->TrashFlag, GK_LOOP, 0, 0);
  rani_privaz_mesh(p_Level->iKursorAnimation, p_Level->iKursorMesh, 0);

/*				rani_zrus(p_item->a_run);
				p_item->a_run = -1;
			}

			p_item->a_run = rani_aktivuj(p_am->sim_anim[34], &p_item->a_flag,GK_LOOP,0,0);
		
			if(p_item->a_run != -1)
			{
				rani_privaz_mesh(p_item->a_run, p_item->Index_Of_Game_Mesh,0);
*/

}

//------------------------------------------------------------------------------------------------
// nakresli flek 
//------------------------------------------------------------------------------------------------
void gl_Do_Flek(float *pos, ITEMDESC * p_Item, LEVELINFO * p_Level)
{
  int rs;
  FLEK_K *pFlek;
  char cFlek[32];
  int pos_[3];
  char bStat = 1;
  float plus_pos = 2.0f;

  gl_i2FStr((rand() % 9) + 1, cFlek);

  memcpy(pos_, p_Item->Pos, 3 * sizeof(int));
  pos_[2]--;

  gl_Logical2Real(pos_[0], pos_[1], pos_[2], &rs, p_Level);

  if (p_Level->Level[rs])
    if (p_Level->Level[rs]->p_Object->Class != 12) {
      bStat = kom_je_prvek_staticky(p_Level->Level[rs]->p_Object->GUID);
      plus_pos = 0.1f;
      goto gl_Do_Flek_SET;
    }

  pos_[2]--;

  if (pos_[2] >= 0) {
    gl_Logical2Real(pos_[0], pos_[1], pos_[2], &rs, p_Level);
    if (p_Level->Level[rs])
      bStat = kom_je_prvek_staticky(p_Level->Level[rs]->p_Object->GUID);
  }

gl_Do_Flek_SET:

  if (bStat) {
    //pridej flek na podlahu
    if (p_Item->Pos[2] == 1) {
      gl_Logical2Real2D(p_Item->Pos[0], p_Item->Pos[1], &rs, p_Level);

      pFlek = &p_Level->pSquare[rs].Flek;
    }

    if (p_Item->Pos[2] > 1) {
      if (!p_Level->Level[rs])
        return;

      pFlek = &p_Level->Level[rs]->Square.Flek;
    }

    pFlek->Flag = K_CHYBA;
    memcpy(pFlek->pos, pos, 3 * sizeof(float));
    pFlek->pos[1]--;
    pFlek->Material = kom_najdi_material(cFlek);
    pFlek->nx[0] = 1;
    pFlek->nx[1] = 0;
    pFlek->nx[2] = 0;
    pFlek->nz[0] = 0;
    pFlek->nz[1] = 0;
    pFlek->nz[2] = 1;
    pFlek->dx = 1;
    pFlek->dy = 1;
    pFlek->Rotation = rand() % 4;

    if (pFlek->Material == -1)
      kprintf(1, "Nelze najit material %s", cFlek);

    if (p_Item->Pos[2] == 1) {
      if (pFlek->pFlek) {
        kom_flek_zrus(pFlek->pFlek);
        pFlek->pFlek = 0;
      }

      pFlek->pFlek = kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos,
        (BOD *) & pFlek->nx, (BOD *) & pFlek->nz,
        pFlek->dx, pFlek->dy, pFlek->Rotation,
        p_Level->pSquare[rs].iUroven, pFlek->Material);
    }


    if (p_Item->Pos[2] > 1) {
      if (pFlek->pFlek) {
        kom_flek_zrus(pFlek->pFlek);
        pFlek->pFlek = 0;
      }

      pFlek->pFlek = kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos,
        (BOD *) & pFlek->nx, (BOD *) & pFlek->nz,
        pFlek->dx, pFlek->dy, pFlek->Rotation,
        p_Level->Level[rs]->Square.iUroven, pFlek->Material);
    }
  }
  else {
    if (!p_Level->Level[rs])
      return;

    pFlek = &p_Level->Level[rs]->Square.Flek;

    pFlek->Flag = p_Level->Level[rs]->Index_Of_Game_Mesh;
    pFlek->pos[0] = 0;
    pFlek->pos[1] = plus_pos;
    pFlek->pos[2] = 0;
    pFlek->Material = kom_najdi_material(cFlek);
    pFlek->nx[0] = 1;
    pFlek->nx[1] = 0;
    pFlek->nx[2] = 0;
    pFlek->nz[0] = 0;
    pFlek->nz[1] = 0;
    pFlek->nz[2] = 1;
    pFlek->dx = 1;
    pFlek->dy = 1;
    pFlek->Rotation = rand() % 4;

    if (pFlek->Material == -1)
      kprintf(1, "Nelze najit material %s", cFlek);

    if (pFlek->pFlek) {
      kom_flek_zrus(pFlek->pFlek);
      pFlek->pFlek = 0;
    }

    pFlek->pFlek =
      kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos, (BOD *) & pFlek->nx,
      (BOD *) & pFlek->nz, pFlek->dx, pFlek->dy, pFlek->Rotation,
      p_Level->Level[rs]->Square.iUroven, pFlek->Material);
  }
}

//------------------------------------------------------------------------------------------------
// pripoj flek k berusce
//------------------------------------------------------------------------------------------------
void gl_Pripoj_Flek_k_Berusce(ITEMDESC * pItem)
{
  FLEK_K *pFlek;

  pFlek = &pItem->Square.Flek;

  pFlek->Flag = pItem->Index_Of_Game_Mesh;
  pFlek->pos[0] = 0;
  pFlek->pos[1] = 0.1f;
  pFlek->pos[2] = 0;
  pFlek->Material = kom_najdi_material("flek10");
  pFlek->nx[0] = 1;
  pFlek->nx[1] = 0;
  pFlek->nx[2] = 0;
  pFlek->nz[0] = 0;
  pFlek->nz[1] = 0;
  pFlek->nz[2] = 1;
  pFlek->dx = 1;
  pFlek->dy = 1;
  pFlek->Rotation = rand() % 4;
  pItem->Square.iUroven = 1;

  if (pFlek->Material == -1)
    kprintf(1, "Nelze najit material flek10");

  pFlek->pFlek =
    kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos, (BOD *) & pFlek->nx,
    (BOD *) & pFlek->nz, pFlek->dx, pFlek->dy, pFlek->Rotation,
    pItem->Square.iUroven, pFlek->Material);
}

void gl_Pripoj_Flek_k_Beruskam(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if (p_Level->Level[i]->p_Object->Class == 1
        && !p_Level->Level[i]->bDestroed)
        gl_Pripoj_Flek_k_Berusce(p_Level->Level[i]);

}

void gl_Pripoj_Flek_k_Predmenu(ITEMDESC * pItem, LEVELINFO * p_Level)
{
  float pos[3];
  int ipos[3];
  int r;
  FLEK_K *pFlek;
  char stat;


  pFlek = &pItem->Square.Flek;

  memcpy((void *) ipos, (void *) pItem->Pos, 3 * sizeof(int));
  ipos[2]--;
  gl_Logical2Real(ipos[0], ipos[1], ipos[2], &r, p_Level);

  if (p_Level->Level[r]) {
    if (p_Level->Level[r]->p_Object->Class != 12) {
      if (kom_je_prvek_staticky(p_Level->Level[r]->p_Object->GUID)) {
        pFlek->Flag = K_CHYBA;
        stat = 1;
      }
      else
        if (p_Level->Level[r]->p_Object->Class == 12 ||
        p_Level->Level[r]->p_Object->Class == 8) {
        pFlek->Flag = K_CHYBA;
        stat = 1;
      }
      else {
        p_Level->Level[r]->Square.pExtFlek = pFlek;
        pFlek->Flag = p_Level->Level[r]->Index_Of_Game_Mesh;
        stat = 0;
      }
    }
    else
      goto NEXT_FLEK_TEST;
  }
  else {
  NEXT_FLEK_TEST:

    ipos[2]--;
    gl_Logical2Real(ipos[0], ipos[1], ipos[2], &r, p_Level);

    if (ipos[2] > 0)
      if (p_Level->Level[r]) {
        if (kom_je_prvek_staticky(p_Level->Level[r]->p_Object->GUID)) {
          pFlek->Flag = K_CHYBA;
          stat = 1;
        }
        else {
          p_Level->Level[r]->Square.pExtFlek = pFlek;
          pFlek->Flag = p_Level->Level[r]->Index_Of_Game_Mesh;
          stat = 0;
        }
      }
      else {
        pFlek->Flag = K_CHYBA;
        stat = 1;
      }
    else {
      pFlek->Flag = K_CHYBA;
      stat = 1;
    }
  }

  kom_get_fyz_souradnice(pItem->Pos[0], pItem->Pos[2], pItem->Pos[1],
    (BOD *) pos);

  if (stat) {
    pFlek->pos[0] = pos[0];
    pFlek->pos[1] = pos[1];
    pFlek->pos[2] = pos[2];
  }
  else {
    pFlek->pos[0] = 0;

    if (p_Level->Level[r]->p_Object->Class == 11 ||
      p_Level->Level[r]->p_Object->Class == 10 ||
      p_Level->Level[r]->p_Object->Class == 15)
      pFlek->pos[1] = 0;
    else
      pFlek->pos[1] = 2;

    pFlek->pos[2] = 0;
  }

  pFlek->Material = kom_najdi_material("flek10");
  pFlek->nx[0] = 1;
  pFlek->nx[1] = 0;
  pFlek->nx[2] = 0;
  pFlek->nz[0] = 0;
  pFlek->nz[1] = 0;
  pFlek->nz[2] = 1;
  pFlek->dx = 1;
  pFlek->dy = 1;
  pFlek->Rotation = rand() % 4;
  pItem->Square.iUroven = 1;

  if (pFlek->Material == -1)
    kprintf(1, "Nelze najit material flek03");

  pFlek->pFlek =
    kom_flek_pridej(pFlek->Flag, (BOD *) & pFlek->pos, (BOD *) & pFlek->nx,
    (BOD *) & pFlek->nz, pFlek->dx, pFlek->dy, pFlek->Rotation,
    pItem->Square.iUroven, pFlek->Material);

  if (pFlek->pFlek == -1) {
    kprintf(1, "Neporarilo ze pripojit flek k predmetu");
    pFlek->pFlek = 0;
  }
}

void gl_Pripoj_Flek_k_Predmetum(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if (p_Level->Level[i]->p_Object->Class == 13
        && !p_Level->Level[i]->bDestroed)
        gl_Pripoj_Flek_k_Predmenu(p_Level->Level[i], p_Level);

}

int gl_Get_Free_Zhavy_System(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < 20; i++)
    if (!p_Level->ZhaveCastice[i].System.pCastice)
      return i;

  return -1;
}

int gl_Get_Free_Kourovy_System(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < 20; i++)
    if (p_Level->KourovaStopa[i].System == -1)
      return i;

  return -1;
}

int gl_Get_Free_Kour(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < 20; i++)
    if (p_Level->Kour[i].System == -1)
      return i;

  return -1;
}

int gl_Get_Free_Kour_Kameni(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < 20; i++)
    if (p_Level->KourKameni[i].System == -1)
      return i;

  return -1;
}

void gl_Do_Smoke(float *pos, LEVELINFO * p_Level, int r)
{
  int k, m, i;
  PAR_KOUR_STOPA *pKourovaS;
  float f;

  k = gl_Get_Free_Kour(p_Level);

  if (k == -1)
    return;

  pKourovaS =
    (PAR_KOUR_STOPA *) malloc(p_Level->Jiskra[r].Sizeof *
    sizeof(PAR_KOUR_STOPA));

  if (!pKourovaS)
    return;

  p_Level->Kour[k].pCastice = pKourovaS;
  p_Level->Kour[k].Sizeof = p_Level->Jiskra[r].Sizeof;
  p_Level->Kour[k].dwStart = timeGetTime();

  m = kom_najdi_material("mrak1_1");

  if (m == -1)
    kprintf(1, "Nelze najit material mrak1_1");

  pKourovaS[0].rychlost_x = 2.0f;
  pKourovaS[0].rychlost_y = 2.0f;
  pKourovaS[0].utlum_x = 0.1f;
  pKourovaS[0].utlum_y = 0.1f;

  pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b =
    0.25f + randf() / (float) (RAND_MAX);
  pKourovaS[0].a = 0.6f;
  pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da =
    -0.075f;
  pKourovaS[0].ka = 0.0f;

  p_Level->Kour[k].System = par_vyrob();

  par_set_param(p_Level->Kour[k].System, m,
    TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR | TPAR_VITR |
    TPAR_NO_FOG, (BOD *) pos, NULL);
  par_vloz_kour_stopu(p_Level->Kour[k].System, pKourovaS,
    p_Level->Kour[k].Sizeof);

  for (i = 0; i < p_Level->Kour[k].Sizeof; i++) {
    p_Level->Kour[k].hHnizdo[i] = par_vloz_hnizdo(p_Level->Kour[k].System);

    par_vloz_hnizdo_komplet(p_Level->Kour[k].hHnizdo[i], 100000,
      (BOD *) p_Level->Kour[k].pivot[i], pKourovaS);
    par_vloz_hnizdo_timer(p_Level->Kour[k].hHnizdo[i], 100000, -100000);

    //vygeneruj dir
    p_Level->Kour[k].dir[i][0] = rand() & 0x1 ? randf() : -randf();
    p_Level->Kour[k].dir[i][1] = randf() * 2.0f;
    p_Level->Kour[k].dir[i][2] = rand() & 0x1 ? randf() : -randf();
    vektor_norm((BOD *) p_Level->Kour[k].dir[i]);

    f = (randf() / (float) (RAND_MAX)) * _3DKOREKCE;

    p_Level->Kour[k].dir[i][0] *= f / 50.0f;
    p_Level->Kour[k].dir[i][1] *= f / 20.0f;
    p_Level->Kour[k].dir[i][2] *= f / 50.0f;

    memcpy((void *) p_Level->Kour[k].pivot[i], (void *) pos,
      3 * sizeof(float));

    p_Level->Kour[k].pivot[i][0] +=
      (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);
    p_Level->Kour[k].pivot[i][1] += randf() / (float) (RAND_MAX);
    p_Level->Kour[k].pivot[i][2] +=
      (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);

    par_vloz_hnizdo_dir(p_Level->Kour[k].hHnizdo[i],
      (BOD *) p_Level->Kour[k].dir[i]);

    par_vloz_hnizdo_vitr(p_Level->Kour[k].hHnizdo[i], (BOD *) p_Level->fVitr);
  }

  p_Level->Kour[k].dwStart = timeGetTime();

  par_pripoj_funkci(p_Level->Kour[k].System, anmend_ZrusCastice3, 0, 0,
    (void *) pKourovaS);

  par_go(p_Level->Kour[k].System, &p_Level->Kour[k].flag, 0, 0);
}

void gl_Do_Prach(float *pos, LEVELINFO * p_Level)
{
  int r, ph, m;
  PAR_FLEK *pCastice;

  pos[1] += 0.25f;

  r = rand() % 10;

  p_Level->Prach[r].dwStart = timeGetTime();

  pCastice = (PAR_FLEK *) malloc(p_Level->Prach[r].Sizeof * sizeof(PAR_FLEK));

  if (!pCastice)
    return;

  memcpy((void *) pCastice, (void *) p_Level->Prach[r].pCastice,
    p_Level->Prach[r].Sizeof * sizeof(PAR_FLEK));

  ph = par_vyrob();

  m = kom_najdi_material("mrak1_1");

  if (m == -1)
    kprintf(1, "Nelze najit material mrak1_1");

  par_set_param(ph, m,
    TPAR_NO_FOG | TPAR_VETSI | TPAR_HTEST | TPAR_AUTOREMOVE | TPAR_SCALE_ADD,
    (BOD *) pos, NULL);

  par_vloz_fleky(ph, pCastice, p_Level->Prach[r].Sizeof);

  par_pripoj_funkci(ph, anmend_ZrusCastice3, 0, 0, (void *) pCastice);

  par_go(ph, &p_Level->Prach[r].flag, 0, 0);
}

void gl_Do_Strepiny_Na_Vode(float *pos, LEVELINFO * p_Level, float fycor)
{
  float fPos[3];
  int real;
  int iPos[3];

  if (p_Level->bUnderWaterLevel)
    return;

  iPos[0] = ftoi((pos[0] - ber.x_start - X_PRVEK / 2) / X_PRVEK);
  iPos[2] = ftoi(((pos[1] - ber.y_start - Y_PRVEK / 2) / Y_PRVEK) * 2);
  iPos[1] = ftoi((pos[2] - ber.z_start - Z_PRVEK / 2) / Z_PRVEK);

  gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

  if (p_Level->Level[real])
    if (p_Level->Level[real]->p_Object->Class == 12) {
      fPos[0] = pos[0];
      fPos[1] = pos[1] - 1;
      fPos[2] = pos[2];

      am_Kola_na_Vode(fPos, 1, 4, 0, p_Level);

      pos[1] += fycor;

      am_Do_Vodni_Cakanec1(pos, 4, 0, p_Level);
      am_Do_Vodni_Cakanec2(pos, 4, 0, p_Level);
    }
}

void gl_Do_Krompac(float *pos, LEVELINFO * p_Level, int material)
{
  int i, r;
  PAR_STREPINA *pCastice;
  int ph;

  for (i = 0; i < 10; i++) {
    r = rand() % 10;
    if (p_Level->Krompac[r].flag == -1)
      break;
  }

  if (i == 20)
    return;

  p_Level->Krompac[r].dwStart = timeGetTime();

  pCastice =
    (PAR_STREPINA *) malloc(p_Level->Krompac[r].Sizeof *
    sizeof(PAR_STREPINA));

  if (!pCastice)
    return;

  memcpy((void *) pCastice, (void *) p_Level->Krompac[r].pCastice,
    p_Level->Krompac[r].Sizeof * sizeof(PAR_STREPINA));

  ph = par_vyrob();

  par_set_param(ph, material, TPAR_HTEST | TPAR_AUTOREMOVE, (BOD *) pos,
    NULL);

  par_vloz_strepy(ph, pCastice, p_Level->Krompac[r].Sizeof);

  par_go(ph, &p_Level->Krompac[r].flag, 0, 0);

  par_pripoj_funkci(ph, anmend_ZrusCastice, -1, 0, (void *) pCastice);

}

void gl_Do_Strepiny(float *pos, LEVELINFO * p_Level, int material,
  char bSvetlo)
{
  int i, r, s, hSvetlo, k, m;
  PAR_STREPINA *pCastice;
  PAR_KOUR_STOPA *pKourovaS;
  int ph, rm;
  float rg, rb, rr, ra, rdx, rdy;
  float rgs, rbs, rrs;

  for (i = 0; i < 20; i++) {
    r = rand() % 20;
    if (p_Level->Exploze[r].flag == -1)
      break;
  }

  if (i == 20)
    return;

  p_Level->Exploze[r].dwStart = timeGetTime();

  pCastice =
    (PAR_STREPINA *) malloc(p_Level->Exploze[r].Sizeof *
    sizeof(PAR_STREPINA));

  if (!pCastice)
    return;

  memcpy((void *) pCastice, (void *) p_Level->Exploze[r].pCastice,
    p_Level->Exploze[r].Sizeof * sizeof(PAR_STREPINA));

  ph = par_vyrob();

  par_set_param(ph, material, TPAR_HTEST | TPAR_AUTOREMOVE, (BOD *) pos,
    NULL);

  par_vloz_strepy(ph, pCastice, p_Level->Exploze[r].Sizeof);

  par_go(ph, &p_Level->Exploze[r].flag, 0, 0);

  if (bSvetlo) {
    if (!p_Level->Sub_Svetla) {
      rr = 1;
      rg = 0.8f + ((float) (rand() % 20) / 100.0f);
      rb = 0;

      hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);
    }
    else {
      rr = 0;
      rg = 1.0f - (0.8f + ((float) (rand() % 20) / 100.0f));
      rb = 1;

      hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR | SDL_SUB);
    }

    sdl_svetlo_set_pos(hSvetlo, (BOD *) pos);
    sdl_svetlo_set_diff(hSvetlo, rr, rg, rb, 0, 0, 0);

    sdl_anim_vyrob(hSvetlo, 15, 0, 0, 3);
    sdl_anim_vloz_klic_vzdal(hSvetlo, 0, 0, 0, 0, 0);
    sdl_anim_vloz_klic_vzdal(hSvetlo, 1, 7, 0, 7, 2);
    sdl_anim_vloz_klic_vzdal(hSvetlo, 2, 0, 0, 0, 14);
    sdl_anim_start(hSvetlo, &p_Level->TrashFlag, 0, 0, 0);
    par_pripoj_funkci(ph, anmend_ZrusCastice, hSvetlo, 0, (void *) pCastice);
  }
  else
    par_pripoj_funkci(ph, anmend_ZrusCastice, -1, 0, (void *) pCastice);

  //zhave jistry
  if (bSvetlo) {
    //najdi volny system jisker
    for (i = 0; i < 20; i++) {
      r = rand() % 20;
      if (p_Level->Jiskra[r].flag == -1)
        break;
    }

    if (i == 20)
      return;

    p_Level->Jiskra[r].dwStart = timeGetTime();

    //najdi alokuj pamet pro system
    pCastice =
      (PAR_STREPINA *) malloc(p_Level->Jiskra[r].Sizeof *
      sizeof(PAR_STREPINA));

    if (!pCastice)
      return;

    memcpy((void *) pCastice, (void *) p_Level->Jiskra[r].pCastice,
      p_Level->Jiskra[r].Sizeof * sizeof(PAR_STREPINA));

    ph = par_vyrob();

    material = kom_najdi_material("lava");

    if (material == -1)
      kprintf(1, "Nelze najit material lava");

    par_set_param(ph, material, TPAR_HTEST | TPAR_AUTOREMOVE, (BOD *) pos,
      NULL);

    par_vloz_strepy(ph, pCastice, p_Level->Jiskra[r].Sizeof);

    par_go(ph, &p_Level->Jiskra[r].flag, 0, 0);

    s = gl_Get_Free_Zhavy_System(p_Level);

    if (s == -1)
      return;

    p_Level->ZhaveCastice[s].System.pCastice = pCastice;
    p_Level->ZhaveCastice[s].System.Sizeof = p_Level->Jiskra[r].Sizeof;
    p_Level->ZhaveCastice[s].System.dwStart = timeGetTime();
    p_Level->ZhaveCastice[s].iKourStopa = -1;

    //kourova stopa za casticema
    k = gl_Get_Free_Kourovy_System(p_Level);

    if (k == -1)
      return;

    p_Level->ZhaveCastice[s].iKourStopa = k;

    pKourovaS =
      (PAR_KOUR_STOPA *) malloc(p_Level->Jiskra[r].Sizeof *
      sizeof(PAR_KOUR_STOPA) * 100);

    if (!pKourovaS)
      return;

    p_Level->KourovaStopa[k].pCastice = pKourovaS;
    p_Level->KourovaStopa[k].Sizeof = p_Level->Jiskra[r].Sizeof * 100;
    p_Level->KourovaStopa[k].dwStart = timeGetTime();

    pKourovaS[0].rychlost_x = 0.1f;
    pKourovaS[0].rychlost_y = 0.1f;
    pKourovaS[0].utlum_x = 0.3f;
    pKourovaS[0].utlum_y = 0.3f;

    pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;
    pKourovaS[0].a = 0.5f;
    pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da =
      -0.1f;
    pKourovaS[0].ka = 0.0f;

    m = kom_najdi_material("mrak1_1");

    if (m == -1)
      kprintf(1, "Nelze najit material mrak1_1");

    p_Level->KourovaStopa[k].System = par_vyrob();
    par_set_param(p_Level->KourovaStopa[k].System, m,
      TPAR_NO_FOG | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *) pos,
      NULL);
    par_vloz_kour_stopu(p_Level->KourovaStopa[k].System, pKourovaS,
      p_Level->KourovaStopa[k].Sizeof);

    par_pripoj_funkci(ph, anmend_ZrusCastice2,
      (int) &p_Level->KourovaStopa[k],
      (int) &p_Level->ZhaveCastice[s], (void *) pCastice);

    for (i = 0; i < p_Level->Jiskra[r].Sizeof; i++) {
      rr = 1;
      rg = 0.8f + ((float) (rand() % 20) / 100.0f);
      rb = 0;

      if (!p_Level->Sub_Svetla) {
        rrs = rr;
        rgs = rg;
        rbs = rb;
      }
      else {
        rrs = 1.0f - rr;
        rgs = 1.0f - rg;
        rbs = 1.0f - rb;
      }

      ra = 0.1f + ((float) (rand() % 30) / 100.0f);
      rdy = rdx = (float) (rand() % 20) / 100.0f;

      rm = rand() % 2;
      if (rm)
        rm = kom_najdi_material("flare4");
      else
        rm = kom_najdi_material("flare5");

      if (rm == -1)
        kprintf(1, "Nelze najit material flare4 nebo flare5");

      //svetlo
      if (!p_Level->Sub_Svetla)
        p_Level->ZhaveCastice[s].hSvetlo[i] =
          sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);
      else
        p_Level->ZhaveCastice[s].hSvetlo[i] =
          sdl_svetlo_vyrob(SDL_UTLUM_LINEAR | SDL_SUB);

      hSvetlo = p_Level->ZhaveCastice[s].hSvetlo[i];

      sdl_svetlo_set_pos(hSvetlo, (BOD *) & p_Level->Jiskra[r].pCastice[i].p);

      sdl_svetlo_set_diff(hSvetlo, rrs, rgs, rbs, 0, rdx + 1.3f, rdx + 1.3f);

      //flare
      p_Level->ZhaveCastice[s].hFlare[i] = kom_flare_vyrob(0);

      hSvetlo = p_Level->ZhaveCastice[s].hFlare[i];

      kom_flare_set_param(hSvetlo, rr, rg, rb, ra, rdy, rdx, rm, GL_ONE);
      kom_flare_set_pivot(hSvetlo, &pCastice[i].p);

      //kourova stopa
      p_Level->KourovaStopa[k].hHnizdo[i] =
        par_vloz_hnizdo(p_Level->KourovaStopa[k].System);

      par_vloz_hnizdo_komplet(p_Level->KourovaStopa[k].hHnizdo[i],
        (int) ceil(1.0f / pCastice[i].rychlost), &pCastice[i].p, pKourovaS);
      par_vloz_hnizdo_timer(p_Level->KourovaStopa[k].hHnizdo[i],
        (int) ceil(1.0f / pCastice[i].rychlost),
        -(int) ceil(1.0f / pCastice[i].rychlost));
    }

    par_go(p_Level->KourovaStopa[k].System, &p_Level->KourovaStopa[k].flag, 0,
      0);

    //kour na miste vybuchu
    gl_Do_Smoke(pos, p_Level, r);

    gl_Do_Strepiny_Na_Vode(pos, p_Level, 0);
  }
}

void gl_Do_Smoke_Kameni(float *pos, LEVELINFO * p_Level)
{
  int k, m, i;
  PAR_KOUR_STOPA *pKourovaS;
  float f;

  k = gl_Get_Free_Kour_Kameni(p_Level);

  if (k == -1)
    return;

  pKourovaS = (PAR_KOUR_STOPA *) malloc(5 * sizeof(PAR_KOUR_STOPA));

  if (!pKourovaS)
    return;

  p_Level->KourKameni[k].pCastice = pKourovaS;
  p_Level->KourKameni[k].Sizeof = 5;
  p_Level->KourKameni[k].dwStart = timeGetTime();

  m = kom_najdi_material("mrak1_1");

  if (m == -1)
    kprintf(1, "Nelze najit material mrak1_1");

  pKourovaS[0].rychlost_x = 2.0f;
  pKourovaS[0].rychlost_y = 2.0f;
  pKourovaS[0].utlum_x = 0.2f;
  pKourovaS[0].utlum_y = 0.2f;

  pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 0.75f;
  pKourovaS[0].a = 0.5f;
  pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da =
    -0.2f;
  pKourovaS[0].ka = 0.0f;

  p_Level->KourKameni[k].System = par_vyrob();

  par_set_param(p_Level->KourKameni[k].System, m,
    TPAR_NO_FOG | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR |
    TPAR_VITR, (BOD *) pos, NULL);
  par_vloz_kour_stopu(p_Level->KourKameni[k].System, pKourovaS,
    p_Level->KourKameni[k].Sizeof);

  for (i = 0; i < p_Level->KourKameni[k].Sizeof; i++) {
    p_Level->KourKameni[k].hHnizdo[i] =
      par_vloz_hnizdo(p_Level->KourKameni[k].System);

    par_vloz_hnizdo_komplet(p_Level->KourKameni[k].hHnizdo[i], 100000,
      (BOD *) p_Level->KourKameni[k].pivot[i], pKourovaS);
    par_vloz_hnizdo_timer(p_Level->KourKameni[k].hHnizdo[i], 100000, -100000);

    //vygeneruj dir
    p_Level->KourKameni[k].dir[i][0] = rand() & 0x1 ? randf() : -randf();
    //p_Level->KourKameni[k].dir[i][1] = randf() * 2.0f;
    p_Level->KourKameni[k].dir[i][2] = rand() & 0x1 ? randf() : -randf();
    vektor_norm((BOD *) p_Level->KourKameni[k].dir[i]);

    f = (randf() / (float) (RAND_MAX)) * _3DKOREKCE;

    p_Level->KourKameni[k].dir[i][0] *= f / 100.0f;
    p_Level->KourKameni[k].dir[i][1] *= f / 100.0f;
    p_Level->KourKameni[k].dir[i][2] *= f / 100.0f;

    memcpy((void *) p_Level->KourKameni[k].pivot[i], (void *) pos,
      3 * sizeof(float));

    p_Level->KourKameni[k].pivot[i][0] +=
      (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);
    p_Level->KourKameni[k].pivot[i][1] += 0.0f;
    p_Level->KourKameni[k].pivot[i][2] +=
      (rand() & 0x1 ? randf() : -randf()) / (float) (RAND_MAX);

    par_vloz_hnizdo_dir(p_Level->KourKameni[k].hHnizdo[i],
      (BOD *) p_Level->KourKameni[k].dir[i]);

    par_vloz_hnizdo_vitr(p_Level->KourKameni[k].hHnizdo[i],
      (BOD *) p_Level->fVitr);
  }

  p_Level->KourKameni[k].dwStart = timeGetTime();

  par_pripoj_funkci(p_Level->KourKameni[k].System, anmend_ZrusCastice3, 0, 0,
    (void *) pKourovaS);

  par_go(p_Level->KourKameni[k].System, &p_Level->KourKameni[k].flag, 0, 0);
}

void gl_Do_Kameni(float *pos, LEVELINFO * p_Level, int material)
{
  int i, r;
  PAR_STREPINA *pCastice;
  int ph;

  for (i = 0; i < 20; i++) {
    r = rand() % 20;
    if (p_Level->Kamen[r].flag == -1)
      break;
  }

  if (i == 20)
    return;

  pCastice =
    (PAR_STREPINA *) malloc(p_Level->Kamen[r].Sizeof * sizeof(PAR_STREPINA));

  if (!pCastice)
    return;

  memcpy((void *) pCastice, (void *) p_Level->Kamen[r].pCastice,
    p_Level->Kamen[r].Sizeof * sizeof(PAR_STREPINA));

  ph = par_vyrob();

  par_set_param(ph, material, TPAR_HTEST | TPAR_AUTOREMOVE, (BOD *) pos,
    NULL);

  par_vloz_strepy(ph, pCastice, p_Level->Kamen[r].Sizeof);

  par_pripoj_funkci(ph, anmend_ZrusCastice, -1, 0, (void *) pCastice);

  par_go(ph, &p_Level->Kamen[r].flag, 0, 0);

  gl_Do_Smoke_Kameni(pos, p_Level);
}

int gl_Get_Mesh_Material(int Mesh)
{
  int *p_mat = NULL;
  int i;
  int matnum = kom_mesh_get_matnum(Mesh);

  p_mat = (int *) malloc(matnum * sizeof(int));

  if (!p_mat)
    return -1;

  kom_mesh_get_mat(Mesh, p_mat);

  i = p_mat[0];

  free((void *) p_mat);

  return i;
}

//------------------------------------------------------------------------------------------------
// destroy item
//------------------------------------------------------------------------------------------------
void gl_Destroy_Item(long Item, int Type, LEVELINFO * p_Level)
{
  ITEMDESC *p_Item = p_Level->Level[Item];

  am_Destroy_Steps(p_Item, p_Level);

  if (p_Item->p_Object->Class == 6 && p_Item->p_Object->GUID == 6014)
    am_Release_BarelSparks(p_Level, p_Item);

  if (p_Level->Level[Item]->a_run != -1) {
    //rani_rozvaz(p_Level->Level[Item]->a_run,p_Level->Level[Item]->Index_Of_Game_Mesh);
    rani_zrus(p_Level->Level[Item]->a_run);
    p_Level->Level[Item]->a_run = -1;
  }

  switch (Type) {
    case 1:
      p_Item->bDestroed = 1;
      p_Level->Level[Item] = 0;
      return;
      break;
    case 2:
      {
        int rot;
        float pos[3];

        kom_mesh_get_float(p_Item->Index_Of_Game_Mesh, &pos[0], &pos[1],
          &pos[2], &rot);
        rot = rand() % 5;
//                      ap_Play_Sound(0,0,0, pos, rot + 14, NULL, &ad);
        p_Item->bDestroed = 1;
        p_Level->Level[Item] = 0;
        return;
      }
      break;
    case 3:
      {
        int rot;
        float pos[3];
        int mat = gl_Get_Mesh_Material(p_Item->Index_Of_Game_Mesh);

        kom_mesh_get_float(p_Item->Index_Of_Game_Mesh, &pos[0], &pos[1],
          &pos[2], &rot);
        rot = rand() % 5;
//                      ap_Play_Sound(0,0,0, pos, rot + 14, NULL, &ad);
        rot = p_Item->Index_Of_Game_Mesh;

        if (p_Item->Square.Flek.pFlek) {
          kom_flek_zrus(p_Item->Square.Flek.pFlek);
          p_Item->Square.Flek.pFlek = 0;
        }

        if (p_Item->Square.pExtFlek && p_Item->Square.pExtFlek->pFlek) {
          kom_flek_zrus(p_Item->Square.pExtFlek->pFlek);
          p_Item->Square.pExtFlek->pFlek = 0;
        }

        kom_zrus_prvek(p_Item->Index_Of_Game_Mesh);
        gl_Do_Strepiny(pos, p_Level, mat, 0);
        p_Item->bDestroed = 1;
        p_Level->Level[Item] = 0;

        return;
      }
      break;
    case 4:
      {
        int rot;
        float pos[3];
        int mat = gl_Get_Mesh_Material(p_Item->Index_Of_Game_Mesh);
        int iPos[3];
        char bSvetlo = 1;

        memcpy((void *) iPos, (void *) p_Item->Pos, 3 * sizeof(int));

        kom_mesh_get_float(p_Item->Index_Of_Game_Mesh, &pos[0], &pos[1],
          &pos[2], &rot);

        gl_Do_Flek(pos, p_Item, p_Level);

        rot = p_Item->Index_Of_Game_Mesh;

        if (p_Item->Square.Flek.pFlek) {
          kom_flek_zrus(p_Item->Square.Flek.pFlek);
          p_Item->Square.Flek.pFlek = 0;
        }

        if (p_Item->Square.pExtFlek && p_Item->Square.pExtFlek->pFlek) {
          kom_flek_zrus(p_Item->Square.pExtFlek->pFlek);
          p_Item->Square.pExtFlek->pFlek = 0;
        }

        gl_Logical2Real(p_Item->Pos[0], p_Item->Pos[1], p_Item->Pos[2], &rot,
          p_Level);

        if (p_Level->Square[rot].bUnderWater)
          bSvetlo = 0;

        kom_zrus_prvek(p_Item->Index_Of_Game_Mesh);
        gl_Do_Strepiny(pos, p_Level, mat, bSvetlo);

        if (!bSvetlo)
          am_Do_Vybuch_Bublin(iPos, pos, p_Level);

        p_Item->bDestroed = 1;
        p_Level->Level[Item] = 0;
        return;
      }
      break;
    case 5:
      if (p_Item->Square.Flek.pFlek) {
        kom_flek_zrus(p_Item->Square.Flek.pFlek);
        p_Item->Square.Flek.pFlek = 0;
      }

      if (p_Item->Square.pExtFlek && p_Item->Square.pExtFlek->pFlek) {
        kom_flek_zrus(p_Item->Square.pExtFlek->pFlek);
        p_Item->Square.pExtFlek->pFlek = 0;
      }
      break;
    case 6:
      {
        int rot;
        float pos[3];
        int mat = gl_Get_Mesh_Material(p_Item->Index_Of_Game_Mesh);

        kom_mesh_get_float(p_Item->Index_Of_Game_Mesh, &pos[0], &pos[1],
          &pos[2], &rot);
        rot = p_Item->Index_Of_Game_Mesh;

        if (p_Item->Square.Flek.pFlek) {
          kom_flek_zrus(p_Item->Square.Flek.pFlek);
          p_Item->Square.Flek.pFlek = 0;
        }

        if (p_Item->Square.pExtFlek && p_Item->Square.pExtFlek->pFlek) {
          kom_flek_zrus(p_Item->Square.pExtFlek->pFlek);
          p_Item->Square.pExtFlek->pFlek = 0;
        }

        kom_zrus_prvek(p_Item->Index_Of_Game_Mesh);
        gl_Do_Strepiny(pos, p_Level, mat, 0);
        p_Item->bDestroed = 1;
        p_Level->Level[Item] = 0;

        return;
      }
      break;
  }

  if (p_Item->Square.Flek.pFlek) {
    kom_flek_zrus(p_Item->Square.Flek.pFlek);
    p_Item->Square.Flek.pFlek = 0;
  }

  if (p_Item->Square.pExtFlek && p_Item->Square.pExtFlek->pFlek) {
    kom_flek_zrus(p_Item->Square.pExtFlek->pFlek);
    p_Item->Square.pExtFlek->pFlek = 0;
  }

  kom_zrus_prvek(p_Item->Index_Of_Game_Mesh);
  p_Item->bDestroed = 1;
  p_Level->Level[Item] = 0;
}

//------------------------------------------------------------------------------------------------
// analyzuje sloupec po padu a provede vybuchy vybusnin u beden
//------------------------------------------------------------------------------------------------
int gl_Analyse_Column(int *column, LEVELINFO * p_Level)
{
  int i, result = 0, w, c[3];
  int item_pos, check_pos;
  OBJECTDESC *p_real_posO, *p_check_posO;


  for (i = column[2]; i > 1; i -= 2) {
    gl_Logical2Real(column[0], column[1], i, &item_pos, p_Level);

    gl_Logical2Real(column[0], column[1], i - 1, &check_pos, p_Level);
    if (p_Level->Level[check_pos])
      if (p_Level->Level[check_pos]->p_Object->Class != 12)
        break;

    gl_Logical2Real(column[0], column[1], i - 2, &check_pos, p_Level);

    if ((p_Level->Level[item_pos]) && p_Level->Level[check_pos]
      && ((i - 2) > 0)) {
      p_real_posO = p_Level->Level[item_pos]->p_Object;
      p_check_posO = p_Level->Level[check_pos]->p_Object;

      if ((p_real_posO->Class == 6) && (p_check_posO->Class == 5)) {

      DESTROY_ITEMS:
        gl_Destroy_Item(item_pos, 4, p_Level);
        gl_Destroy_Item(check_pos, 3, p_Level);
        result++;
        i -= 2;
        continue;
      }

      if ((p_real_posO->Class == 6) &&
        (p_real_posO->SubClass == 1) &&
        ((p_check_posO->Class == 5) ||
          (p_check_posO->Class == 6) ||
          (p_check_posO->Class == 7) ||
          (p_check_posO->Class == 3) || (p_check_posO->Class == 16)
        ))
        goto DESTROY_ITEMS;

      //jestlize je to pontonka, zkontroluji zatez
      if (p_check_posO->Class == 5 && p_check_posO->SubClass == 3) {
        c[0] = column[0];
        c[1] = column[1];
        c[2] = i;

        w = gl_Count_Weight(c, p_Level);

        if (w > 1) {
          result++;
          continue;
        }
      }

    }

    if (!p_Level->Level[item_pos] || !p_Level->Level[check_pos])
      break;
  }

  return result;
}

//------------------------------------------------------------------------------------------------
// provede rychlou predbeznou analyzu, zda je posun mozny (zdi)
//------------------------------------------------------------------------------------------------
int gl_Is_Move_Possible(int *iValue_old, int *iValue_new, LEVELINFO * p_Level)
{
  int Move[3], act_pos[3], pos[3], old_pos[3];
  int Vrstva;
  int real_pos, test_pos, check_pos;
  int i;
  int counter = 0;
  int Weight, Total_Weight = 0;
  char Ignore_Weight = 0;
  ITEMDESC *p_real_posI, *p_test_posI;
  OBJECTDESC *p_real_posO, *p_test_posO, *p_LastO;
  BACK_PACK *p_Back = p_Level->Level[p_Level->Actual_Item]->p_Back_Pack;

  gl_Get_Move(iValue_old, iValue_new, Move);

  if ((iValue_new[0] < 0) || (iValue_new[0] > (p_Level->Size[0] - 1)))
    return 0;
  if ((iValue_new[1] < 0) || (iValue_new[1] > (p_Level->Size[1] - 1)))
    return 0;

  act_pos[0] = iValue_new[0];
  act_pos[1] = iValue_new[1];
  act_pos[2] = iValue_new[2];

  gl_Logical2Real(iValue_new[0], iValue_new[1], iValue_new[2], &real_pos,
    p_Level);
  p_real_posI = p_Level->Level[real_pos];

  if (p_Level->bSikminaMoveExeption && p_Level->Flip)
    return 0;

  p_Level->bSikminaMoveExeption = 0;

  if (p_real_posI)
    p_real_posO = p_Level->Level[real_pos]->p_Object;

  if (p_real_posI) {
    if (p_real_posO->Class == 13)
      return 1;

/*		if(p_real_posO->Class == 6)
			if(gl_Count_Weight(iValue_new, p_Level) > 
				p_Back->Strength)
					return 0;
			else
				Ignore_Weight = 1;*/

    if (p_real_posO->Class == 6)
      Ignore_Weight = 1;


    if (p_real_posO->Class == 7)
      return 1;
  }

  // postupuju do predu az dokud nenarazim na konec radi
  while (p_real_posI) {
    Vrstva = act_pos[2];

    if ((act_pos[0] >= 0) && (act_pos[0] < p_Level->Size[0]) &&
      (act_pos[1] >= 0) && (act_pos[1] < p_Level->Size[1]) &&
      (act_pos[2] >= 0) && (act_pos[2] < p_Level->Size[2])) {
      gl_Logical2Real(act_pos[0], act_pos[1], act_pos[2], &test_pos, p_Level);
      p_test_posI = p_Level->Level[test_pos];
      if (p_test_posI)
        p_test_posO = p_test_posI->p_Object;

      // jedu az na konec sloupce
      if (!Ignore_Weight) {
        Weight = gl_Count_Weight(act_pos, p_Level);
        Total_Weight += Weight;
        if (Total_Weight > p_Back->Strength)
          return 0;
      }

      while (p_Level->Level[test_pos]) {
        if (Vrstva > (p_Level->Size[2] - 1))
          break;
        if (p_test_posO->Class == 2)
          break;

        pos[0] = act_pos[0] + Move[0];
        pos[1] = act_pos[1] + Move[1];
        pos[2] = Vrstva;

        gl_Logical2Real(pos[0], pos[1], pos[2], &check_pos, p_Level);

        // jestli je pred tim zed -> neda se posunout
        if ((pos[0] >= 0) && (pos[0] < p_Level->Size[0]) &&
          (pos[1] >= 0) && (pos[1] < p_Level->Size[1]) &&
          (pos[2] >= 0) && (pos[2] < p_Level->Size[2]))
          if (p_Level->Level[check_pos]) {
            if ((p_Level->Level[check_pos]->p_Object->Class == 2) &&
              (p_test_posO->Class == 19))
              return 1;
            else
              if ((p_Level->Level[check_pos]->p_Object->Class == 19) &&
              ((p_test_posO->Class == 5) || (p_test_posO->Class == 6)))
              return 1;
            else if (p_Level->Level[check_pos]->p_Object->Class == 2)
              return 0;
          }

        // mrknu se, jestli vedle toho neni magnet
        if (Move[0])
          i = 1;
        else
          i = 0;

        pos[0] = act_pos[0];
        pos[1] = act_pos[1];
        pos[i]++;
        gl_Logical2Real(pos[0], pos[1], pos[2], &check_pos, p_Level);

        if (pos[i] < p_Level->Size[i])
          if (p_Level->Level[check_pos])
            if (p_Level->Level[check_pos]->p_Object->Class == 17) {
              gl_Hrebik(p_Level->Level[check_pos]);
              return 0;
            }

        pos[i] -= 2;
        gl_Logical2Real(pos[0], pos[1], pos[2], &check_pos, p_Level);

        if (pos[i] >= 0)
          if (p_Level->Level[check_pos])
            if (p_Level->Level[check_pos]->p_Object->Class == 17) {
              gl_Hrebik(p_Level->Level[check_pos]);
              return 0;
            }

        pos[i]++;
        if ((pos[2] + 2) < p_Level->Size[2]) {
          gl_Logical2Real(pos[0], pos[1], pos[2] + 2, &check_pos, p_Level);

          if (p_Level->Level[check_pos])
            if (p_Level->Level[check_pos]->p_Object->Class == 17) {
              gl_Hrebik(p_Level->Level[check_pos]);
              return 0;
            }
        }

        if ((pos[2] - 2) >= 0) {
          gl_Logical2Real(pos[0], pos[1], pos[2] - 2, &check_pos, p_Level);

          if (p_Level->Level[check_pos])
            if (p_Level->Level[check_pos]->p_Object->Class == 17) {
              gl_Hrebik(p_Level->Level[check_pos]);
              return 0;
            }
        }

        Vrstva += 2;

        if (Vrstva > (p_Level->Size[2] - 1))
          break;

        gl_Logical2Real(act_pos[0], act_pos[1], Vrstva, &test_pos, p_Level);
        p_test_posI = p_Level->Level[test_pos];

        /*if(Vrstva > (p_Level->Size[2]-1))
           break;
           else */
        if (p_test_posI)
          p_test_posO = p_test_posI->p_Object;

        counter++;
        if ((counter > p_Back->Strength) && (!Ignore_Weight))
          return 0;
        else if (Ignore_Weight) {
          int test_weight = 0;

          gl_Logical2Real(act_pos[0], act_pos[1], act_pos[2], &test_pos,
            p_Level);

          old_pos[0] = act_pos[0];
          old_pos[1] = act_pos[1];
          old_pos[2] = act_pos[2];

          act_pos[0] += Move[0];
          act_pos[1] += Move[1];
          act_pos[2] = iValue_new[2];

          gl_Logical2Real(act_pos[0], act_pos[1], act_pos[2], &real_pos,
            p_Level);

          if (p_Level->Level[real_pos]) {
            test_weight +=
              gl_Count_Weight(old_pos, p_Level) + gl_Count_Weight(act_pos,
              p_Level);

            if (p_Level->Level[test_pos]->p_Object->Class == 6 &&
              p_Level->Level[test_pos]->p_Object->SubClass &&
              p_Level->Level[real_pos]->p_Object->Class == 6 &&
              !p_Level->Level[real_pos]->p_Object->SubClass)
              return 1;
            else
              if (p_Level->Level[test_pos]->p_Object->Class == 6 &&
              p_Level->Level[real_pos]->p_Object->Class == 6 &&
              p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength <
              test_weight)
              return 0;
            else
              return 1;
          }
          else                  //tlacim bombu -> Ignore_Wigth, ale musim zamezit, aby se dal tracit sloupec bez omezeni
          if (counter == 1
            && gl_Count_Weight(old_pos, p_Level) > p_Back->Strength)
            return 0;
          else
            return 1;
        }
      }

      act_pos[0] += Move[0];
      act_pos[1] += Move[1];
      act_pos[2] = iValue_new[2];

      p_LastO = p_real_posI->p_Object;

      if ((act_pos[0] >= 0) && (act_pos[0] < p_Level->Size[0]) &&
        (act_pos[1] >= 0) && (act_pos[1] < p_Level->Size[1]) &&
        (act_pos[2] >= 0) && (act_pos[2] < p_Level->Size[2])) {
        gl_Logical2Real(act_pos[0], act_pos[1], act_pos[2], &real_pos,
          p_Level);
        p_real_posI = p_Level->Level[real_pos];
      }

      if ((act_pos[0] >= 0) && (act_pos[0] < p_Level->Size[0]) &&
        (act_pos[1] >= 0) && (act_pos[1] < p_Level->Size[1]) &&
        (act_pos[2] >= 0) && (act_pos[2] < p_Level->Size[2]))
        if (p_real_posI)
          if (p_real_posI->p_Object->Class == 6) {
            if ((p_LastO->Class == 6) && (p_LastO->SubClass == 1) &&
              (!p_real_posI->p_Object->SubClass))
              return 1;
            else {
              //!!!!!!!!!!!!!!!!!!!!!!!!! ZMENA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              if (Total_Weight > p_Back->Strength)
                return 0;
            }
          }
          else {
            //!!!!!!!!!!!!!!!!!!!!!!!!! ZMENA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if (p_LastO->Class == 6 && p_real_posI->p_Object->Class == 5)
              return 1;
          }
/*			else
				return 0;*/
    }
    else
      return 0;
  }
  return 1;
}

//------------------------------------------------------------------------------------------------
// Check Item logic annd take item if possible
//------------------------------------------------------------------------------------------------
int gl_Take_Item(long Item, long Beatle, LEVELINFO * p_Level, char bSound,
  char bRemove)
{
  OBJECTDESC *p_Item_Object = p_Level->Level[Item]->p_Object;
  BACK_PACK *p_Beatle_BP = p_Level->Level[Beatle]->p_Back_Pack;
  float pos[3];
  int rot;

  if (bSound)
    kom_mesh_get_float(p_Level->Level[Item]->Index_Of_Game_Mesh,
      &pos[0], &pos[1], &pos[2], &rot);

  // je to klic
  if (p_Level->Level[Item]->p_Object->SubClass >= 9) {
    if (p_Beatle_BP->Count_of_Items >= p_Beatle_BP->Max_Items)
      return 0;

    p_Beatle_BP->item[p_Level->Level[Item]->p_Object->SubClass]++;
    p_Beatle_BP->Count_of_Items++;

    if (bRemove)
      am_Remove_Animate_item(Item, p_Level);

    //if(bSound)
//                      ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);       

    return 1;
  }

  //batoh
  if (p_Item_Object->SubClass == 1) {
    if (!p_Beatle_BP->item[1]) {
      p_Beatle_BP->item[1]++;
      p_Beatle_BP->Max_Items *= 2;

      if (bRemove)
        am_Remove_Animate_item(Item, p_Level);

      //      if(bSound)
      //      ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

      return 1;
    }
    else
      return 0;
  }

  //vybusniny balicek
  if (p_Item_Object->SubClass == 4) {
    if (p_Beatle_BP->Count_of_Items >= p_Beatle_BP->Max_Items)
      return 0;

    p_Beatle_BP->item[4]++;
    p_Beatle_BP->Count_of_Items++;

    if (bRemove)
      am_Remove_Animate_item(Item, p_Level);

    //if(bSound)
    //ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

    return 1;
  }

  //krompac
  if (p_Item_Object->SubClass == 8) {
    if (p_Beatle_BP->Count_of_Items >= p_Beatle_BP->Max_Items)
      return 0;

    p_Beatle_BP->item[8]++;
    p_Beatle_BP->Count_of_Items++;

    if (bRemove)
      am_Remove_Animate_item(Item, p_Level);

    //if(bSound)
    //ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

    return 1;
  }

  //snotchl
  if (p_Item_Object->SubClass == 0) {
    if (!p_Beatle_BP->item[0]) {
      if (p_Beatle_BP->Count_of_Items >= p_Beatle_BP->Max_Items)
        return 0;

      p_Beatle_BP->item[0]++;
      p_Beatle_BP->Count_of_Items++;

      if (bRemove)
        am_Remove_Animate_item(Item, p_Level);

      //if(bSound)
      //ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

      return 1;
    }
    else
      return 0;
  }

  //potravina
  if (p_Item_Object->SubClass == 3) {
    if (!p_Beatle_BP->item[3]) {
      p_Beatle_BP->item[3]++;

      if (bRemove)
        am_Remove_Animate_item(Item, p_Level);

      //if(bSound)
      //ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

      return 1;
    }
    else
      return 0;
  }

  //hormonalni vytamin
  if (p_Item_Object->SubClass == 5) {
    if (!p_Beatle_BP->item[5]) {
      p_Beatle_BP->item[5]++;
      p_Beatle_BP->Strength *= 2;

      if (bRemove)
        am_Remove_Animate_item(Item, p_Level);

      //if(bSound)
      //ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

      return 1;
    }
    else
      return 0;
  }

  //kahan
  if (p_Item_Object->SubClass == 6) {
    if (!p_Level->Level[Beatle]->p_Back_Pack->item[6]) {
      p_Beatle_BP->item[6]++;
      p_Beatle_BP->Count_of_Items++;

      if (bRemove)
        am_Remove_Animate_item(Item, p_Level);

      //if(bSound)
      //ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

      return 1;
    }
    else
      return 0;
  }

  //zavazi
  if (p_Item_Object->SubClass == 7) {
    if (!p_Beatle_BP->item[7]) {
      int iPos[3];
      int real;

      memcpy((void *) iPos, (void *) p_Level->Level[Item]->Pos,
        3 * sizeof(int));


      if (!gl_Test_Rule2(iPos, p_Level))
        return 0;

      iPos[2] -= 2;

      if (iPos[2] > 0) {
        gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

        if (p_Level->Level[real])
          if (!gl_Test_Rule1(p_Level->Level[p_Level->Actual_Item],
              p_Level->Level[real], p_Level))
            return 0;

      }

      iPos[2]++;

      if (iPos[2] >= 0) {
        gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

        if (p_Level->Level[real])
          if (p_Level->Level[real]->p_Object->Class == 15)
            if (!gl_Test_Rule1B(p_Level->Level[p_Level->Actual_Item],
                iPos, p_Level))
              return 0;

      }

      p_Beatle_BP->item[7]++;
      p_Beatle_BP->Count_of_Items++;

      if (bRemove)
        am_Remove_Animate_item(Item, p_Level);

      //if(bSound)
      //ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);

      p_Level->Throw_off = 1;
      return 1;
    }
    else
      return 0;
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
// Check Item logic annd take item if possible
//------------------------------------------------------------------------------------------------
int gl_Is_Take_Posible(long Item, long Beatle, LEVELINFO * p_Level)
{
  OBJECTDESC *p_Item_Object = p_Level->Level[Item]->p_Object;
  BACK_PACK *p_Beatle_BP = p_Level->Level[Beatle]->p_Back_Pack;

  //batoh
  if (p_Item_Object->SubClass == 1)
    if (!p_Beatle_BP->item[1])
      return 1;
    else
      return 0;

  //vybusniny balicek
  if (p_Item_Object->SubClass == 4 ||
    p_Item_Object->SubClass == 8 || p_Item_Object->SubClass == 0)
    if (p_Beatle_BP->Count_of_Items >= p_Beatle_BP->Max_Items)
      return 0;
    else
      return 1;

  //potravina
  if (p_Item_Object->SubClass == 3)
    if (!p_Beatle_BP->item[3])
      return 1;
    else
      return 0;

  //hormonalni vytamin
  if (p_Item_Object->SubClass == 5)
    if (!p_Beatle_BP->item[5])
      return 1;
    else
      return 0;

  //kahan
  if (p_Item_Object->SubClass == 6)
    if (!p_Level->Level[Beatle]->p_Back_Pack->item[6])
      return 1;
    else
      return 0;

  //zavazi
  if (p_Item_Object->SubClass == 7)
    if (!p_Beatle_BP->item[7])
      return 1;
    else
      return 0;

  return 0;
}

//------------------------------------------------------------------------------------------------
// zvuk hrebiku
//------------------------------------------------------------------------------------------------
void gl_Hrebik(ITEMDESC * pItem)
{
  float pos[3];
  int rot;
  OBJECTDESC *pItemO = pItem->p_Object;

  kom_mesh_get_float(pItem->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2],
    &rot);

  rot = rand() % 3;
/*
	if(pItemO->Specific[rot].Index != -1)
		ap_Play_Sound(0, 0, 0, pos, pItemO->Specific[rot].Index, NULL, &ad);
	else
		if(pItemO->Specific[0].Index != -1)
			ap_Play_Sound(0, 0, 0, pos,pItemO->Specific[0].Index, NULL, &ad);
*/
}


//------------------------------------------------------------------------------------------------
// provede kontrolu, zda je pod predmetem pontonova bedna
//------------------------------------------------------------------------------------------------
void gl_Kontrola_Pontonky(int *iValue, LEVELINFO * p_Level)
{
  int Vrstva = iValue[2] - 2;
  int r;

  if (Vrstva < 0)
    return;

  gl_Logical2Real(iValue[0], iValue[1], Vrstva, &r, p_Level);
  if (p_Level->Level[r])
    if (p_Level->Level[r]->p_Object->Class == 5 &&
      p_Level->Level[r]->p_Object->SubClass == 3)
      p_Level->Throw_off = 1;
}

void gl_Do_Propadlo(float *pos, LEVELINFO * p_Level, int material)
{
  PAR_STREPINA *pCastice;
  int i, r;
  int ph;

  for (i = 0; i < 20; i++) {
    r = rand() % 20;
    if (p_Level->Propadla[r].flag == -1)
      break;
  }

  if (i == 20)
    return;

  pCastice =
    (PAR_STREPINA *) malloc(p_Level->Propadla[r].Sizeof *
    sizeof(PAR_STREPINA));

  if (!pCastice)
    return;

  memcpy((void *) pCastice, (void *) p_Level->Propadla[r].pCastice,
    p_Level->Propadla[r].Sizeof * sizeof(PAR_STREPINA));

  ph = par_vyrob();

  par_set_param(ph, material, TPAR_HTEST | TPAR_AUTOREMOVE, (BOD *) pos,
    NULL);

  par_vloz_strepy(ph, pCastice, p_Level->Propadla[r].Sizeof);

  par_pripoj_funkci(ph, anmend_ZrusCastice, -1, 0, (void *) pCastice);

  par_go(ph, &p_Level->Propadla[r].flag, 0, 0);
}

void gl_Set_3ds_Mesh(char bSet, int iID, int iMesh, int iRotace)
{
  p_set->bSetMesh = bSet;
  p_set->iMeshID = iID;
  p_set->iMesh = iMesh;
  p_set->iRot = iRotace;
}

void gl_Set_3ds_Anim(int mesh, int poradi, int c_anim, int *p_flag, int flag,
  int start, int stop)
{
  p_set->_3dsAnim[poradi].mesh = mesh;
  p_set->_3dsAnim[poradi].c_anim = c_anim;
  p_set->_3dsAnim[poradi].p_flag = p_flag;
  p_set->_3dsAnim[poradi].flag = flag;
  p_set->_3dsAnim[poradi].start = start;
  p_set->_3dsAnim[poradi].stop = stop;
}

int gl_Test_Rule1(ITEMDESC * pItem, ITEMDESC * pTestItem, LEVELINFO * p_Level)
{
  ITEMDESC *pBreatle = NULL;
  int pos[3], c, plus = 0, PlusVaha;
  int r;

  if (pTestItem->p_Object->Class != 5 && pTestItem->p_Object->Class != 6)
    return 1;

  memcpy((void *) pos, (void *) pItem->Pos, 3 * sizeof(int));

  c = gl_Is_There_Beatle(pos, &pBreatle, p_Level);

  if (!pBreatle)
    return 1;

  memcpy((void *) pos, (void *) pTestItem->Pos, 3 * sizeof(int));

  pos[2] += 2;

  gl_Logical2Real(pos[0], pos[1], pos[2], &r, p_Level);

  if (p_Level->Level[r])
    if (p_Level->Level[r]->p_Object->Class == 13 &&
      p_Level->Level[r]->p_Object->SubClass == 7)
      plus = 1;

  memcpy((void *) pos, (void *) pTestItem->Pos, 3 * sizeof(int));

  if (gl_Will_Go_Under_Water(pos, -1, c, &PlusVaha, p_Level)
    && !gl_Can_Go_Into_Water(pItem)) {
    c = gl_Count_Weight(pItem->Pos, p_Level) + plus;

    c += PlusVaha;

    if (c > 2)
      return 0;
    else {
      p_Level->Throw_off = 1;
      return 1;
    }
  }

  //p_Level->Throw_off = 1;
  return 1;
}

int gl_Test_Rule1B(ITEMDESC * pItem, int *pTestPos, LEVELINFO * p_Level)
{
  ITEMDESC *pBreatle = NULL;
  int pos[3], c, plus = 0;
  int r;

  memcpy((void *) pos, (void *) pItem->Pos, 3 * sizeof(int));

  c = gl_Is_There_Beatle(pos, &pBreatle, p_Level);

  if (!pBreatle)
    return 1;

  memcpy((void *) pos, (void *) pTestPos, 3 * sizeof(int));

  pos[2]++;

  if (pos[2] > p_Level->Size[2] - 1)
    return 1;

  gl_Logical2Real(pos[0], pos[1], pos[2], &r, p_Level);

  if (p_Level->Level[r])
    if (p_Level->Level[r]->p_Object->Class == 13 &&
      p_Level->Level[r]->p_Object->SubClass == 7)
      plus = 1;

  memcpy((void *) pos, (void *) pTestPos, 3 * sizeof(int));
  pos[2]--;

  /*if(pos[2] < 0)
     return 1; */

  if (gl_Will_Go_Under_Water(pos, -1, c, NULL, p_Level)
    && !gl_Can_Go_Into_Water(pItem)) {
    c = gl_Count_Weight(pItem->Pos, p_Level) + plus;

    if (c > 1)
      return 0;
    else {
      //p_Level->Throw_off = 1;
      return 1;
    }
  }

//      p_Level->Throw_off = 1;
  return 1;
}

int gl_Test_Rule2(int *pos, LEVELINFO * p_Level)
{
  int real;
  int iPos[3], i;

  memcpy((void *) iPos, (void *) pos, 3 * sizeof(int));

  iPos[2]--;

  if (iPos[2] >= 0) {
    gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

    if (p_Level->Level[real]) {
      if (p_Level->Level[real]->p_Object->Class == 15) {
        iPos[2]--;

        // zkontroluju, jestli to neoporuje pravidlum
        for (i = iPos[2]; i > 0; i -= 2) {
          gl_Logical2Real(iPos[0], iPos[1], i, &real, p_Level);

          if (p_Level->Level[real])
            if ((p_Level->Level[real]->p_Object->Class == 1) ||
              (p_Level->Level[real]->p_Object->Class == 13))
              return 0;
        }
      }
      else
        return 1;
    }
    else
      return 1;
  }
  else
    return 1;

  return 1;
}

int gl_Test_Rule2B(int *pos, LEVELINFO * p_Level)
{
  int real;
  int iPos[3], i;

  memcpy((void *) iPos, (void *) pos, 3 * sizeof(int));

  iPos[2] -= 2;

  if (iPos[2] >= 0) {
    for (i = iPos[2]; i > 0; i -= 2) {
      gl_Logical2Real(iPos[0], iPos[1], i, &real, p_Level);

      if (p_Level->Level[real])
        if ((p_Level->Level[real]->p_Object->Class == 1) ||
          (p_Level->Level[real]->p_Object->Class == 13))
          return 0;
        else
          return 1;

      gl_Logical2Real(iPos[0], iPos[1], i - 1, &real, p_Level);

      if (p_Level->Level[real])
        if (p_Level->Level[real]->p_Object->Class != 12)
          return 1;
    }
  }
  else
    return 1;

  return 1;
}

int gl_Test_Rule3(ITEMDESC * pItem, int *iPos, LEVELINFO * p_Level)
{
  int real;
  int ivPos[3];

  memcpy((void *) ivPos, (void *) iPos, 3 * sizeof(int));

  ivPos[2]--;

  if (ivPos[2] < 0)
    return 1;

  gl_Logical2Real(ivPos[0], ivPos[1], ivPos[2], &real, p_Level);

  if (p_Level->Level[real])
    if (p_Level->Level[real]->p_Object->Class != 12)
      return 1;

  if (!gl_Will_Go_Under_Water(iPos, -1, 0, NULL, p_Level))
    return 1;
  else if (pItem->p_Object->Class == 1)
    return 0;

  if (gl_Test_Rule2B(iPos, p_Level))
    return 1;

  return 0;
}

void gl_Test_Rule4(ITEMDESC * pItem, int *iPos, LEVELINFO * p_Level)
{
  int real;
  int w = gl_Count_Weight(pItem->Pos, p_Level);

  if (iPos[2] - 3 >= 0) {
    gl_Logical2Real(iPos[0], iPos[1], iPos[2] - 3, &real, p_Level);

    if (p_Level->Level[real])
      if (p_Level->Level[real]->p_Object->Class == 12 && w > 1)
        p_Level->Throw_off = 1;
  }
}

void gl_Walk_Out_Of_Water(int *pos, int iMesh, DWORD dwExpire,
  LEVELINFO * p_Level)
{
  int real;
  int iPos[3], i;

  memcpy((void *) iPos, (void *) pos, 3 * sizeof(int));

  iPos[2]--;

  if (iPos[2] < 0)
    return;

  gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

  if (p_Level->Level[real])
    if (p_Level->Level[real]->p_Object->Class == 12)
      for (i = 0; i < 100; i++)
        if (p_Level->VodniKolaB[i].System != -1 &&
          p_Level->VodniKolaB[i].hHnizdo[2] == iMesh) {
          if (dwExpire)
            p_Level->VodniKolaB[i].dwExpire = dwExpire;
          else
            p_Level->VodniKolaB[i].hHnizdo[2] = -1;

          return;
        }
}

int gl_Test_Rule5(int *pos, LEVELINFO * p_Level)
{
  int real;
  int iPos[3];

  if (!pos)
    return 1;

  iPos[0] = pos[0];
  iPos[1] = pos[1];
  iPos[2] = pos[2];

  iPos[2]--;

  if (iPos[2] < 0)
    return 1;

  //musi pod ni byt voda
  gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

  if (p_Level->Level[real]) {
    if (p_Level->Level[real]->p_Object->Class != 12)
      return 1;
  }
  else
    return 1;

  //nesmi pod ni byt bedna
  iPos[2]--;

  if (iPos[2] >= 0) {
    gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

    if (p_Level->Level[real])
      return 1;
  }

  return 0;
}

int gl_TestRule7(ITEMDESC * pItem, int *pos, LEVELINFO * p_Level)
{
  int real;
  int iPos[3];

  memcpy((void *) iPos, (void *) pos, 3 * sizeof(int));

  iPos[2]--;

  if (iPos[2] < 0)
    return 1;

  gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

  if (p_Level->Level[real]) {
    if ((p_Level->Level[real]->p_Object->Class == 12) &&
      (pItem->p_Object->Class == 1)) {
      iPos[2]--;

      if (iPos[2] >= 0) {
        int rtest_pos;

        gl_Logical2Real(iPos[0], iPos[1], iPos[2], &rtest_pos, p_Level);

        if (!p_Level->Level[rtest_pos])
          return 0;
        else {
          switch (p_Level->Level[rtest_pos]->p_Object->Class) {
            case 1:
            case 13:
            case 4:
              return 0;
              break;
            default:
              return 1;
              break;
          }
        }
      }
    }
  }
  else
    return 1;

  return 1;
}

int gl_Test_Rule8(int *Old_Pos, int *New_Pos, LEVELINFO * p_Level)
{
  ITEMDESC *pItem;
  int iVoda, iBottom;
  int from[3];
  char bSnorchl;

  gl_Is_There_Beatle(Old_Pos, &pItem, p_Level);

  if (pItem)
    if (pItem->p_Back_Pack->item[0])
      bSnorchl = 1;
    else
      bSnorchl = 0;
  else
    return 1;

  iBottom = gl_Find_Bottom(New_Pos, &iVoda, p_Level);

  if (iBottom < 0)
    iBottom = 0;


  from[2] = iBottom;
  from[0] = New_Pos[0];
  from[1] = New_Pos[1];

  iVoda = gl_Is_There_Water(from, New_Pos, p_Level);

  if (iVoda == -1) {
    int iTmp = New_Pos[2];

    New_Pos[2] = pItem->Pos[2];
    iVoda = gl_Is_There_Water(from, New_Pos, p_Level);
    New_Pos[2] = iTmp;
  }

  if (!iVoda)
    return 1;

  if ((pItem->Pos[2] - New_Pos[2] < iVoda - iBottom) && !bSnorchl)
    return 0;

  return 1;
}

//------------------------------------------------------------------------------------------------
// move item
//------------------------------------------------------------------------------------------------
long gl_Move_Item(int *iValue_old, int *iValue, long Item,
  LEVELINFO * p_Level)
{
  int i;
  int real_p;
  int iValue_new[3], ret;
  int real_pos;
  int Vrstva = p_Level->Level[Item]->Pos[2];
  OBJECTDESC *p_ItemO = p_Level->Level[Item]->p_Object;
  OBJECTDESC *p_real_pO = NULL, *p_real_posO = NULL;
  ITEMDESC *p_real_pI = NULL, *p_real_posI = NULL;

  gl_Logical2Real(iValue[0], iValue[1], iValue[2], &real_pos, p_Level);
  Vrstva -= 2;

  if (Vrstva > p_Level->Size[2] - 1)
    return -1;

  if (Vrstva >= 0) {
    gl_Logical2Real(iValue[0], iValue[1], Vrstva, &real_p, p_Level);

    p_real_pI = p_Level->Level[real_p];
    if (p_real_pI)
      p_real_pO = p_real_pI->p_Object;
  }

  // zkontroluje, zda nechci prvek posunout mimo hranice
  for (i = 0; i < 3; i++)
    if ((iValue[i] > (p_Level->Size[i] - 1)) || (iValue[i] < 0))
      return -1;

  // nachazi se na miste, kam chci prvek posunout jiny prvek?

//- ne
  if (!p_Level->Level[real_pos]) {
    // muze se prvek vubec posouvat?
    if (!p_Level->Move_Exeption)
      if ((p_ItemO->Class != 1) &&
        (p_ItemO->Class != 5) &&
        (p_ItemO->Class != 6) &&
        (p_ItemO->Class != 19) && (p_ItemO->Class != 20))
        return -1;

    if ((iValue[2] - 1) >= 0) {
      gl_Logical2Real(iValue[0], iValue[1], iValue[2] - 1, &real_p, p_Level);
      p_real_pI = p_Level->Level[real_p];
    }

    if ((iValue[2] - 1) >= 0)
      if (p_real_pI) {
        p_real_pO = p_Level->Level[real_p]->p_Object;

        // test na propadlo
        if (p_real_pO->Class == 15) {
          int iTest[3];
          int test_pos;
          int w;
          OBJECTDESC *p_test_posO;


          iTest[0] = iValue[0];
          iTest[1] = iValue[1];
          iTest[2] = iValue[2] - 1;

          if (!gl_Test_Rule1B(p_Level->Level[Item], iTest, p_Level))
            return -1;

          w = gl_Count_Weight(iValue, p_Level);

          if (w > 1) {
            // zkontroluju, jestli to neoporuje pravidlum
            for (i = Vrstva; i > 0; i -= 2) {
              gl_Logical2Real(iValue[0], iValue[1], i, &test_pos, p_Level);
              p_test_posO = p_Level->Level[test_pos]->p_Object;

              if (p_Level->Level[test_pos])
                if ((p_test_posO->Class == 1) || (p_test_posO->Class == 13))
                  return -1;
            }
            // musi se setrasat
            p_Level->Throw_off = 1;
          }
        }

        // test na prah
        if ((p_real_pO->Class == 3) &&
          (p_ItemO->Class == 6) && (p_ItemO->SubClass)) {
          // znic vybusninu
          gl_Destroy_Item(Item, 0, p_Level);
          queue_set[p_Level->Actual_Qset].last++;
          gl_Throw_off(iValue_old, p_Level);

          p_Level->Column[0] = iValue_old[0];
          p_Level->Column[1] = iValue_old[1];
          p_Level->Column[2] =
            iValue_old[2] + (gl_Get_Column_Height(iValue_old[0],
              iValue_old[1], iValue_old[2] - 1, p_Level) - 2) * 2;

          if (p_Level->Column[2] < 0)
            p_Level->Column[2] = 0;

          p_Level->bColumn++;

          // znic prah
          gl_Destroy_Item(real_p, 2, p_Level);
          p_Level->bDemoSaveExeption = 1;
          return -1;
        }

        if ((p_real_pO->Class == 3) && (p_ItemO->Class != 1))
          return -1;

        // test na exit
        if ((p_real_pO->Class == 4) && (p_ItemO->Class != 1))
          return -1;

        // test na hrebik
        if ((p_real_pO->Class == 16) && (p_ItemO->Class != 1)) {
          gl_Hrebik(p_real_pI);
          return -1;
        }

        // posouvam berusku na vodu
        if ((p_real_pO->Class == 12) &&
          (p_ItemO->Class == 1) && !p_Level->Move_Exeption) {
          int iPos[3];

          memcpy((void *) iPos, (void *) p_real_pI->Pos, 3 * sizeof(int));

          iPos[2]--;

          if (iPos[2] >= 0) {
            int rtest_pos;

            gl_Logical2Real(iPos[0], iPos[1], iPos[2], &rtest_pos, p_Level);

            if (!p_Level->Level[rtest_pos]) {
              iPos[2]--;

              if (iPos[2] >= 0) {
                gl_Logical2Real(iPos[0], iPos[1], iPos[2], &rtest_pos,
                  p_Level);

                if (!p_Level->Level[rtest_pos])
                  return -1;
                else if (p_Level->Level[rtest_pos]->p_Object->Class != 4)
                  return -1;
              }
            }
            else
              if (p_Level->Level[rtest_pos]->p_Object->Class == 1 ||
              p_Level->Level[rtest_pos]->p_Object->Class == 13)
              return -1;
          }
        }

        if ((p_real_pO->Class == 12) && (p_ItemO->Class != 1)) {
          float fpos[3];
          int rot;

          if (!gl_Test_Rule3(p_Level->Level[Item], iValue, p_Level))
            return -1;

          kom_mesh_get_float(p_Level->Level[Item]->Index_Of_Game_Mesh,
            &fpos[0], &fpos[1], &fpos[2], &rot);

          am_Kola_na_VodeB(fpos, 0, p_Level->Level[Item]->Index_Of_Game_Mesh,
            0, p_Level);

          if (!gl_Test_Rule5(iValue, p_Level))
            p_Level->Throw_off = 1;
        }

        // test dveri
        if (p_real_pO->Class == 8) {
          if ((p_real_pO->SubClass > 12) &&
            (p_real_pO->SubClass < 19) && (p_ItemO->Class != 1))
            return -1;

          if (!gl_Gate_Keeper(real_p, iValue_old, p_Level))
            return -1;
        }
      }

    // muze prvek spadnout?
    if (Vrstva >= 0) {
      gl_Logical2Real(iValue[0], iValue[1], iValue[2] - 1, &real_p, p_Level);
      p_real_pI = p_Level->Level[real_p];

      if (!p_real_pI) {
        gl_Logical2Real(iValue[0], iValue[1], Vrstva, &real_p, p_Level);
        p_real_pI = p_Level->Level[real_p];

        if (!p_real_pI) {
          gl_Logical2Real(iValue[0], iValue[1], Vrstva - 1, &real_p, p_Level);
          p_real_pI = p_Level->Level[real_p];

          //muze ale chodit po exitu a teleportu
          if (Vrstva - 1 > -1) {
            if (p_real_pI) {
              if (p_real_pI->p_Object->Class != 4 &&
                p_real_pI->p_Object->Class != 8)
                p_real_pI = NULL;
            }
            else
              p_real_pI = NULL;
          }
          else
            p_real_pI = NULL;
        }

        //ano
        if (!p_real_pI) {

          if (!gl_Test_Rule8(iValue_old, iValue, p_Level))
            return -1;

          // jestli je to brouk, tak to nejde, protoze ten spadnout nesmi
          if (p_ItemO->Class == 1 && !p_Level->Move_Exeption)
            return -1;

          // zkontroluju, jestli to neoporuje pravidlum
          for (i = Vrstva; i > 0; i -= 2) {
            gl_Logical2Real(iValue[0], iValue[1], i, &real_p, p_Level);
            p_real_pI = p_Level->Level[real_p];

            if (p_real_pI) {
              if (p_real_pI->p_Object->Class == 1) {
                float pos[3];
                int wave, rot;

                kom_mesh_get_float(p_Level->Level[real_p]->Index_Of_Game_Mesh,
                  &pos[0], &pos[1], &pos[2], &rot);
                wave = gl_Choose_Wave_Index(13);
/*							if (!adas_Is_Sound_Processed(12,-1) && 
								!adas_Is_Sound_Processed(13,-1) && 
								!adas_Is_Sound_Processed(14,-1))*/
//                                                              ap_Play_Sound(0,0,pos,wave,NULL,p_Level->p_ad);

                return -1;
              }

              if ((p_real_pI->p_Object->Class == 19) ||
                (p_real_pI->p_Object->Class == 20) ||
                (p_real_pI->p_Object->Class == 13))
                return -1;
            }
          }

          // musi se setrasat
          if (!p_Level->Move_Exeption)
            p_Level->Throw_off = 1;

          goto Move_Forward;

        }
        else {
          if (!gl_Test_Rule1(p_Level->Level[Item], p_real_pI, p_Level))
            return -1;

          // beruska sice spadnout nemuze, ale taky se na ni nesmi nic nasunout, 
          // nebo stoupnout na zada
          p_real_pO = p_real_pI->p_Object;
          if ((p_real_pO->Class == 1) || (p_real_pO->Class == 13))
            return -1;

          //p_Level->Throw_off = 1;
        }
      }
    }

  Move_Forward:

    Vrstva = iValue[2] - 2;
    if (Vrstva >= 0) {
      gl_Logical2Real(iValue[0], iValue[1], Vrstva, &real_p, p_Level);
      p_real_pI = p_Level->Level[real_p];

      if (p_real_pI) {
        if ((p_real_pI->p_Object->Class == 20) && (p_ItemO->Class != 1))
          return -1;

        //pokud je pod predmetem pontonka, tak mozna je tam voda a musi se to setrast
        if (p_real_pI->p_Object->Class == 5 &&
          p_real_pI->p_Object->SubClass == 3) {
          gl_Test_Rule4(p_Level->Level[Item], p_Level->Level[Item]->Pos,
            p_Level);
          gl_Test_Rule4(p_Level->Level[Item], iValue, p_Level);
        }

        //-----------------------------------------------------------------------------------
        // posun po sikmine dolu
        //-----------------------------------------------------------------------------------
        if (p_real_pI->p_Object->Class == 19) {
          int dest_pos;
          int Move[3], dest[3], m;
          ITEMDESC *p_destI;

          p_Level->bSikminaMoveExeption = 1;

          gl_Get_Move(iValue_old, iValue, Move);

          if (!Move[0]) {
            if ((Move[1] > 0) && (!p_real_pI->Rotation))
              goto Move_OK_B;
            if ((Move[1] < 0) && (p_real_pI->Rotation == 2))
              goto Move_OK_B;
            return -1;
          }
          else {
            if ((Move[0] > 0) && (p_real_pI->Rotation == 1))
              goto Move_OK_B;
            if ((Move[0] < 0) && (p_real_pI->Rotation == 3))
              goto Move_OK_B;
            return -1;
          }

        Move_OK_B:

          gl_Walk_Out_Of_Water(iValue_old,
            p_Level->Level[Item]->Index_Of_Game_Mesh, 600, p_Level);

          for (m = 0; m < 3; m++)
            dest[m] = iValue_old[m];

          dest[2]--;

          gl_Logical2Real(dest[0], dest[1], dest[2], &dest_pos, p_Level);

          if (p_Level->Level[dest_pos])
            if (p_Level->Level[dest_pos]->p_Object->Class == 12)
              if (!gl_Can_Go_Into_Water(p_Level->Level[Item]))
                return -1;

          for (m = 0; m < 3; m++)
            dest[m] = iValue_old[m] + (2 * Move[m]);

          dest[2] -= 2;

          if (dest[2] - 2 > 0) {
            gl_Logical2Real(dest[0], dest[1], dest[2] - 2, &dest_pos,
              p_Level);
            p_destI = p_Level->Level[dest_pos];
            if (!p_destI) {
              p_Level->Column[0] = dest[0];
              p_Level->Column[1] = dest[1];
              p_Level->Column[2] = dest[2];

              p_Level->Throw_off = 1;
            }
          }

          p_Level->bSikminaUp = 0;
          p_Level->bSikminaDown = 1;

          //posunuju brouka na sikminu
          if (p_ItemO->Class == 1) {
            if (dest[2] - 2 > 0) {
              gl_Logical2Real(dest[0], dest[1], dest[2] - 2, &dest_pos,
                p_Level);
              p_destI = p_Level->Level[dest_pos];
              if (!p_destI)
                return -1;
            }

            gl_Logical2Real(dest[0], dest[1], dest[2], &dest_pos, p_Level);

            p_destI = p_Level->Level[dest_pos];

            if (!p_destI) {
              //normalni posun. na destinaci neni nic

            BEATLE_MOVE_B:
              p_Level->Level[dest_pos] = p_Level->Level[Item];
              p_destI = p_Level->Level[dest_pos];
              p_Level->Level[Item] = 0;
              p_Level->Actual_Item = dest_pos;

              p_destI->Pos[0] = dest[0];
              p_destI->Pos[1] = dest[1];
              p_destI->Pos[2] = dest[2];

              if (p_set->animation[p_set->last].p_matrix == -1 &&
                p_set->animation[p_set->last].p_run != -1)
                rani_zrus(p_set->animation[p_set->last].p_run);

              p_set->animation[p_set->last].flag = 0;

              p_set->animation[p_set->last].p_run =
                rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                    Actual_Item]->Rotation + 14],
                &p_set->animation[p_set->last].flag);

              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_destI->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_destI->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

              gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
                Index_Of_Game_Mesh, 0, p_Level->iWalkAnimation,
                &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

              if (p_Level->bSunuti)
                gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
                  Index_Of_Game_Mesh, 1, 2,
                  &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

/*						if(p_Level->bSunuti)
							rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_kom_mesh_set_mesh,
											   p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 1,
											   (void *)p_Level->Level[p_Level->Actual_Item]->Rotation);*/

              p_set->last++;
              p_set->animation[p_set->last].bnext++;

              p_set->animation[p_set->last].p_run =
                rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                    Actual_Item]->Rotation + 65],
                &p_set->animation[p_set->last].flag);

              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_destI->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_destI->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

              if (!p_Level->bSunuti)
                gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
                  Index_Of_Game_Mesh, 1, p_Level->iWalkAnimation,
                  &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

              rani_next_animace(p_set->animation[p_set->last - 1].p_run,
                p_set->animation[p_set->last].p_run, 0, 0, 0);

              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                anmend_Set_Flek_Flag,
                p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek,
                0, NULL);

/*						if(!p_set->last && p_Level->bPosouvatKameru)
							rani_privaz_kameru(p_set->animation[p_set->last].p_run);*/

              p_Level->bVypniFlek = 1;
              p_Level->bSikminaSound = 1;
              p_Level->bSunuti = 0;

              am_Destroy_Steps(p_Level->Level[Item], p_Level);

              return p_Level->Actual_Item;
            }
            else {
              if (p_destI->p_Object->Class == 13) {
                POINTERSTRUCTURE *pS =
                  (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

                if (pS) {
                  pS->p_Level = p_Level;
                  pS->iParam = p_Level->Level[dest_pos]->iItem;

                  am_Set_Triger_Function(&p_set->animation[p_set->last],
                    anmend_Take_Item,
                    p_Level->Level[dest_pos]->Index_Of_Game_Mesh,
                    p_Level->Level[dest_pos]->p_Object->SubClass, pS, 70, 0);
                }

                //na destinace je predmet
                if (!gl_Take_Item(dest_pos, Item, p_Level, 0, 0)) {
                  free((void *) pS);
                  am_Set_Triger_Function(&p_set->animation[p_set->last],
                    NULL, 0, 0, 0, 1, 0);
                  return -1;
                }

                gl_Destroy_Item(dest_pos, 1, p_Level);

                goto BEATLE_MOVE_B;
              }

              if (p_destI->p_Object->Class == 5) {
              NORMAL_BEATLE_ITEM_MOVE_B:

                //je tam dedna
                int next_dest[3];
                int weight = 0, cweight;
                int Lnext_dest;

                weight = 0;

                for (m = 0; m < 3; m++)
                  next_dest[m] = dest[m];

                //zvazim 4 sloupce za sikminou (4 kvuli prasku, jinak bych vazil 2)
                for (m = 0; m < 3; m++) {
                  cweight = gl_Count_Weight(next_dest, p_Level);

                  if (!cweight)
                    break;

                  weight += cweight;

                  next_dest[0] += Move[0];
                  next_dest[1] += Move[1];
                  next_dest[2] += Move[2];

                  if ((next_dest[0] < 0)
                    || (next_dest[0] >= p_Level->Size[0]))
                    break;
                  if ((next_dest[1] < 0)
                    || (next_dest[1] >= p_Level->Size[1]))
                    break;
                }

                //pokud veci za sikminou vazi vic, nez je povoleno, tak fuj
                if (weight >
                  p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
                  return -1;

                for (m = 0; m < 3; m++)
                  next_dest[m] = dest[m] + Move[m];

                gl_Logical2Real(dest[0], dest[1], dest[2], &Lnext_dest,
                  p_Level);

                if (p_set->animation[p_set->last].p_matrix == -1 &&
                  p_set->animation[p_set->last].p_run != -1)
                  rani_zrus(p_set->animation[p_set->last].p_run);

                p_set->animation[p_set->last].flag = 0;

                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                      Actual_Item]->Rotation + 6],
                  &p_set->animation[p_set->last].flag);

                if (gl_Is_Move_Possible(dest, next_dest, p_Level)) {
                  weight = gl_Move_Item(dest, next_dest, Lnext_dest, p_Level);
                  if (weight < 0)
                    return -1;
                }
                else
                  return -1;

                next_dest[0] = dest[0];
                next_dest[1] = dest[1];
                next_dest[2] = dest[2] - 2;

                if (next_dest[2] >= 0) {
                  gl_Logical2Real(next_dest[0], next_dest[1], next_dest[2],
                    &Lnext_dest, p_Level);

                  if (p_Level->Level[Lnext_dest])
                    am_Set_Triger_Function(&p_set->animation[p_set->last],
                      anmend_Play_Sunuti,
                      p_Level->Level[Lnext_dest]->p_Object->Material,
                      0, p_Level, 55, 4);
                  else
                    am_Set_Triger_Function(&p_set->animation[p_set->last],
                      anmend_Play_Sunuti, -1, 0, p_Level, 55, 4);
                }
                else
                  am_Set_Triger_Function(&p_set->animation[p_set->last],
                    anmend_Play_Sunuti, -1, 0, p_Level, 55, 4);
                p_set->last++;
                p_Level->bSunuti = 1;
                goto BEATLE_MOVE_B;
              }

              if (p_destI->p_Object->Class == 6) {
                //je tam vybusnina
                int next_dest[3];
                int Lnext_dest, Ldest;
                POINTERSTRUCTURE *pS =
                  (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

                for (m = 0; m < 3; m++)
                  next_dest[m] = dest[m] + Move[m];

                // jestlize tou vybusninou neni co vyhodit, tak skoc na normalni posun
                if (!gl_Count_Weight(next_dest, p_Level))
                  goto NORMAL_BEATLE_ITEM_MOVE_B;

                gl_Logical2Real(dest[0], dest[1], dest[2], &Ldest, p_Level);
                gl_Logical2Real(next_dest[0], next_dest[1], next_dest[2],
                  &Lnext_dest, p_Level);

                if ((p_Level->Level[Ldest]->p_Object->SubClass != 1) &&
                  (p_Level->Level[Lnext_dest]->p_Object->Class != 5))
                  return -1;

                if ((p_Level->Level[Ldest]->p_Object->SubClass == 1) &&
                  ((p_Level->Level[Lnext_dest]->p_Object->Class != 5) &&
                    (p_Level->Level[Lnext_dest]->p_Object->Class != 6)))
                  return -1;

                if (p_set->animation[p_set->last].p_matrix == -1 &&
                  p_set->animation[p_set->last].p_run != -1)
                  rani_zrus(p_set->animation[p_set->last].p_run);

                p_set->animation[p_set->last].flag = 0;

                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                      Actual_Item]->Rotation + 39],
                  &p_set->animation[p_set->last].flag);

                p_set->animation[p_set->last].p_matrix =
                  rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                  0);

                gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
                  Index_Of_Game_Mesh, 1, p_Level->iWalkAnimation,
                  &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

/*							rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_kom_mesh_set_mesh,
											   p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0,
											   (void *)p_Level->Level[p_Level->Actual_Item]->Rotation);*/

                p_set->last++;
                p_set->animation[p_set->last].bnext = 1;

                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                      Actual_Item]->Rotation + 61],
                  &p_set->animation[p_set->last].flag);

                p_set->animation[p_set->last].p_matrix =
                  rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                  0);

                rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                  anmend_Set_Flek_Flag,
                  p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek,
                  0, NULL);

                rani_next_animace(p_set->animation[p_set->last - 1].p_run,
                  p_set->animation[p_set->last].p_run, 0, 0, 0);

                if (pS) {
                  memcpy(pS->viParam1, dest, 3 * sizeof(int));
                  memcpy(pS->viParam2, next_dest, 3 * sizeof(int));
                  pS->p_Level = p_Level;
                  pS->pParam = (void *) p_set;

                  am_Set_Triger_Function(&p_set->animation[p_set->last],
                    anmend_ExplozeBednyZaSikminou,
                    Ldest, Lnext_dest, pS, 1, 0);
                  p_set->last++;
                }
                else {
                  int lc;

                  // znic vybusnimu
                  gl_Destroy_Item(Ldest, 4, p_Level);
                  p_set->last++;
                  gl_Throw_off(dest, p_Level);

                  p_Level->Column[0] = dest[0];
                  p_Level->Column[1] = dest[1];
                  p_Level->Column[2] =
                    dest[2] + (gl_Get_Column_Height(dest[0], dest[1],
                      dest[2] - 1, p_Level) - 2) * 2;

                  if (p_Level->Column[2] < 0)
                    lc = 0;
                  else
                    lc = p_Level->Column[2];

                  p_Level->bColumn++;

                  // znic bednu
                  gl_Destroy_Item(Lnext_dest, 3, p_Level);
                  gl_Throw_off(next_dest, p_Level);

                  p_Level->Column[2] = lc;

                  p_Level->SecColumn[0] = next_dest[0];
                  p_Level->SecColumn[1] = next_dest[1];
                  p_Level->SecColumn[2] =
                    next_dest[2] + (gl_Get_Column_Height(next_dest[0],
                      next_dest[1], next_dest[2] - 1, p_Level) - 2) * 2;
                  p_Level->bSecColumn++;

                  p_Level->bCheckDependencesNeeded = 1;
                }

                p_Level->bSikminaSound = 1;
                p_Level->bVypniFlek = 1;
                p_Level->bSunuti = 1;

                am_Destroy_Steps(p_Level->Level[Item], p_Level);

                return p_Level->Actual_Item;
              }

              if (p_destI->p_Object->Class == 7) {
                //je tam kamen
                int Ldest;
                POINTERSTRUCTURE *pS =
                  (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

                if (p_Level->Item_Lock || p_Level->Flip)
                  return -1;

                if (!p_Level->Level[Item]->p_Back_Pack->item[8])
                  return -1;

                p_Level->Level[Item]->p_Back_Pack->item[8]--;
                p_Level->Level[Item]->p_Back_Pack->Count_of_Items--;

                if (p_set->animation[p_set->last].p_matrix == -1 &&
                  p_set->animation[p_set->last].p_run != -1)
                  rani_zrus(p_set->animation[p_set->last].p_run);

                //chuze dolu
                p_set->animation[p_set->last].flag = 0;

                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                      Actual_Item]->Rotation + 86],
                  &p_set->animation[p_set->last].flag);

                p_set->animation[p_set->last].p_matrix =
                  rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                  0);

                rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                  anmend_kom_mesh_set_meshK,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 1,
                  (void *) p_Level->Level[p_Level->Actual_Item]->Rotation);

                gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
                  Index_Of_Game_Mesh, 1, 0,
                  &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

                gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
                  Index_Of_Game_Mesh, 2, p_Level->iWalkAnimation,
                  &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

                //buchanec
                p_set->last++;
                p_set->animation[p_set->last].bnext = 1;

                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[77],
                  &p_set->animation[p_set->last].flag);

                p_set->animation[p_set->last].p_matrix =
                  rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                  0);

                rani_next_animace(p_set->animation[p_set->last - 1].p_run,
                  p_set->animation[p_set->last].p_run, 0, 0, 0);

                /*am_Set_Start_Function(&p_set->animation[p_set->last], anmend_PlayKrumpac, 
                   p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0, NULL, 0); */

                gl_Logical2Real(dest[0], dest[1], dest[2], &Ldest, p_Level);

                if (pS) {
                  pS->p_Level = p_Level;

                  pS->viParam1[0] = Ldest;
                  pS->viParam1[1] = 0;
                  pS->viParam1[2] = 0;

                  pS->viParam2[0] =
                    p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh;
                  pS->viParam2[1] = 0;
                  pS->viParam2[2] =
                    p_Level->Level[p_Level->Actual_Item]->Rotation;

                  rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                    anmend_KamenZaSikmonou, 0, 0, (void *) pS);
                }
                else
                  kprintf(1,
                    "FATAL ERROR - nejde udelat pamet na data ukoncovaci funkce");

                /*am_Set_Triger_Function(&p_set->animation[p_set->last],
                   anmend_Kamen,
                   Ldest, 0, (void *)p_Level, 50, 0); */

                /*rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_kom_mesh_set_mesh,
                   p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0,
                   (void *)p_Level->Level[p_Level->Actual_Item]->Rotation); */

                //chuze spatky
                p_set->last++;
                p_set->animation[p_set->last].bnext = 1;

                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                      Actual_Item]->Rotation + 90],
                  &p_set->animation[p_set->last].flag);

                p_set->animation[p_set->last].p_matrix =
                  rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                  p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                  0);


                rani_next_animace(p_set->animation[p_set->last - 1].p_run,
                  p_set->animation[p_set->last].p_run, 0, 0, 0);

                rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                  anmend_Set_Flek_Flag,
                  p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek,
                  0, NULL);

                p_set->last += 2;
                p_Level->bSikminaSound = 1;
                p_Level->bVypniFlek = 1;
                p_Level->bStoneSound = 1;
                return p_Level->Actual_Item;
              }

              return -1;
            }
          }

          if (p_ItemO->Class == 5) {
            int item_pos[3];
            int p_sikmina;
            int weight;

            gl_Logical2Real(dest[0], dest[1], dest[2], &dest_pos, p_Level);

            p_destI = p_Level->Level[dest_pos];

            //nad bednou nesmi nikdo byt
            item_pos[0] = p_Level->Level[Item]->Pos[0];
            item_pos[1] = p_Level->Level[Item]->Pos[1];
            item_pos[2] = p_Level->Level[Item]->Pos[2];

            // kdyz to moc vazi, tak exit
            weight = gl_Count_Weight(item_pos, p_Level);
            if (weight >
              p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
              return -1;

            item_pos[2] += 2;

            // nad bednou nesmi nic byt
            if (item_pos[2] <= p_Level->Size[2]) {
              gl_Logical2Real(item_pos[0], item_pos[1], item_pos[2],
                &p_sikmina, p_Level);
              if (p_Level->Level[p_sikmina])
                return -1;
            }

            if (!p_destI) {
              //normalni posun. na destinaci neni nic
              int p_tmp;
              int p_sikmina;
              ITEMDESC *p_sikminaI;
              int item_pos[3];

            NORMAL_BEATLE_ITEM_MOVE__B:

              item_pos[0] = p_Level->Level[Item]->Pos[0] + Move[0];
              item_pos[1] = p_Level->Level[Item]->Pos[1] + Move[1];
              item_pos[2] = p_Level->Level[Item]->Pos[2];
              item_pos[2] -= 2;

              gl_Logical2Real(item_pos[0], item_pos[1], item_pos[2],
                &p_sikmina, p_Level);
              p_sikminaI = p_Level->Level[p_sikmina];

              p_Level->Level[dest_pos] = p_Level->Level[Item];
              p_destI = p_Level->Level[dest_pos];
              p_Level->Level[Item] = 0;

              p_destI->Pos[0] = dest[0];
              p_destI->Pos[1] = dest[1];
              p_destI->Pos[2] = dest[2];

              if (p_set->animation[p_set->last].p_matrix == -1 &&
                p_set->animation[p_set->last].p_run != -1) {
                rani_zrus(p_set->animation[p_set->last].p_run);
                p_set->animation[p_set->last].flag = 0;
              }
              else
                p_set->last++;

              //Test jestli za tim jeste neni hned dira
              item_pos[0] += Move[0];
              item_pos[1] += Move[1];
              item_pos[2] -= 2;

              gl_Logical2Real(item_pos[0], item_pos[1], item_pos[2], &p_tmp,
                p_Level);

              if (item_pos[2] < 0)
                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[(p_sikminaI->Rotation * 2) +
                    26], &p_set->animation[p_set->last].flag);
              else if (p_Level->Level[p_tmp])
                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[(p_sikminaI->Rotation * 2) +
                    26], &p_set->animation[p_set->last].flag);
              else
                p_set->animation[p_set->last].p_run =
                  rani_aktivuj_cekej(am.sim_anim[(p_sikminaI->Rotation) + 73],
                  &p_set->animation[p_set->last].flag);

              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_destI->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_destI->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

              p_set->last++;

              p_set->animation[p_set->last].p_run =
                rani_aktivuj_cekej(am.sim_anim[(p_sikminaI->Rotation * 2) +
                  27], &p_set->animation[p_set->last].flag);

              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_destI->Index_Of_Game_Mesh, 0);

              p_set->animation[p_set->last].bnext = 1;

              rani_next_animace(p_set->animation[p_set->last - 1].p_run,
                p_set->animation[p_set->last].p_run, 0, 0, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_destI->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

/*						if(!p_set->last && p_Level->bPosouvatKameru)
							rani_privaz_kameru(p_set->animation[p_set->last].p_run);*/

              p_set->last++;
              p_Level->Sikmina_Flag++;
              p_Level->bSikminaUp = -1;
              p_Level->bExtraSoundSunuti = 2;

              am_Destroy_Steps(p_Level->Level[Item], p_Level);

              return dest_pos;
            }
            else {
              //na destinaci je predmet
              int next_dest[3], m, cweight;
              int rpos;
              ITEMDESC *p_rposI;

              for (m = 0; m < 3; m++)
                next_dest[m] = dest[m];

              //zvazim 4 sloupce za sikminou (4 kvuli prasku, jinak bych vazil 2)
              for (m = 0; m < 3; m++) {
                gl_Logical2Real(next_dest[0], next_dest[1], next_dest[2],
                  &rpos, p_Level);
                p_rposI = p_Level->Level[rpos];

                if (p_rposI)
                  if (p_rposI->p_Object->Class == 1)
                    return -1;

                cweight = gl_Count_Weight(next_dest, p_Level);
                if (!cweight)
                  break;

                weight += cweight;

                next_dest[0] += Move[0];
                next_dest[1] += Move[1];
                next_dest[2] += Move[2];

                if ((next_dest[0] < 0) || (next_dest[0] >= p_Level->Size[0]))
                  break;
                if ((next_dest[1] < 0) || (next_dest[1] >= p_Level->Size[1]))
                  break;
              }

              //pokud veci za sikminou vazi vic, nez je povoleno, tak fuj
              if (weight >
                p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
                return -1;

              for (m = 0; m < 3; m++)
                next_dest[m] = dest[m] + Move[m];

              // posun sloupce za sikminou
              gl_Logical2Real(dest[0], dest[1], dest[2], &p_sikmina, p_Level);

              if (p_set->animation[p_set->last].p_matrix == -1 &&
                p_set->animation[p_set->last].p_run != -1)
                rani_zrus(p_set->animation[p_set->last].p_run);

              p_set->animation[p_set->last].flag = 0;

              p_set->animation[p_set->last].p_run =
                rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                    Actual_Item]->Rotation + 6],
                &p_set->animation[p_set->last].flag);

              if (gl_Is_Move_Possible(dest, next_dest, p_Level)) {
                weight = gl_Move_Item(dest, next_dest, p_sikmina, p_Level);
                if (weight < 0)
                  return -1;
              }
              else
                return -1;

              goto NORMAL_BEATLE_ITEM_MOVE__B;
            }
          }

          if (p_ItemO->Class == 6) {
            //tlasim vybusninu
            int item_pos[3];
            int p_sikmina;
            ITEMDESC *p_sikminaI;

            //nad bednou nesmi nikdo byt
            item_pos[0] = p_Level->Level[Item]->Pos[0];
            item_pos[1] = p_Level->Level[Item]->Pos[1];
            item_pos[2] = p_Level->Level[Item]->Pos[2];

            item_pos[2] += 2;

            // nad bednou nesmi nic byt
            if (item_pos[2] <= p_Level->Size[2]) {
              gl_Logical2Real(item_pos[0], item_pos[1], item_pos[2],
                &p_sikmina, p_Level);
              if (p_Level->Level[p_sikmina])
                return -1;
            }

            item_pos[0] = p_Level->Level[Item]->Pos[0] + Move[0];
            item_pos[1] = p_Level->Level[Item]->Pos[1] + Move[1];
            item_pos[2] = p_Level->Level[Item]->Pos[2] - 2;

            gl_Logical2Real(dest[0], dest[1], dest[2], &dest_pos, p_Level);
            gl_Logical2Real(item_pos[0], item_pos[1], item_pos[2], &p_sikmina,
              p_Level);
            p_sikminaI = p_Level->Level[p_sikmina];
            p_destI = p_Level->Level[dest_pos];

            if (!p_destI)
              goto NORMAL_BEATLE_ITEM_MOVE__B;

            if ((p_ItemO->SubClass != 1) && (p_destI->p_Object->Class != 5))
              return -1;

            if ((p_ItemO->SubClass == 1) &&
              ((p_destI->p_Object->Class != 5) &&
                (p_destI->p_Object->Class != 6)))
              return -1;

            // znic vybusnimu
            item_pos[0] = p_Level->Level[Item]->Pos[0];
            item_pos[1] = p_Level->Level[Item]->Pos[1];
            item_pos[2] = p_Level->Level[Item]->Pos[2];

            if (p_set->animation[p_set->last].p_matrix == -1 &&
              p_set->animation[p_set->last].p_run != -1) {
              rani_zrus(p_set->animation[p_set->last].p_run);
              p_set->animation[p_set->last].flag = 0;
            }
            else
              p_set->last++;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[(p_sikminaI->Rotation * 2) + 26],
              &p_set->animation[p_set->last].flag);

            p_set->animation[p_set->last].p_matrix =
              rani_privaz_mesh(p_set->animation[p_set->last].p_run,
              p_Level->Level[Item]->Index_Of_Game_Mesh, 0);

            gl_Add_Mesh(&p_set->animation[p_set->last],
              p_Level->Level[Item]->Index_Of_Game_Mesh);
            //p_set->animation[p_set->last].mesh = p_Level->Level[Item]->Index_Of_Game_Mesh;

/*					if(!p_set->last && p_Level->bPosouvatKameru)
						rani_privaz_kameru(p_set->animation[p_set->last].p_run);*/

            rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
              anmend_ExplozeBedny,
              (int) p_Level->Level[Item]->Index_Of_Game_Mesh, dest_pos,
              p_Level);

            p_set->last++;
            p_Level->Sikmina_Flag++;
            p_Level->Throw_off = 1;
            p_Level->bSikminaUp = -1;

            am_Destroy_Steps(p_Level->Level[Item], p_Level);
            gl_Destroy_Item(Item, 1, p_Level);

            return Item;
          }

          return -1;
        }
      }
    }

    //nove !!!(mozno nepojede v poho)!!!
    if (p_Level->Level[Item]->p_Object->Class != 1
      && p_Level->Level[Item]->p_Object->Class != 13
      && iValue_old[2] + 2 < p_Level->Size[2]) {
      Vrstva = iValue_old[2] + 2;
      iValue_new[0] = iValue_old[0];
      iValue_new[1] = iValue_old[1];
      iValue_new[2] = Vrstva;
      // zkusi posunout dedny nad ni

      gl_Logical2Real(iValue_old[0], iValue_old[1], Vrstva, &real_p, p_Level);
      p_real_pI = p_Level->Level[real_p];

      // pokud je nad ni bedna, posun ji
      if (Vrstva < p_Level->Size[2] && Vrstva > 0)
        if (p_real_pI) {
          int iValue_new1[3];
          int test_pos;

          // postupuj az ke stropu
          while (Vrstva < p_Level->Size[2]) {
            iValue_new1[0] = iValue[0];
            iValue_new1[1] = iValue[1];
            iValue_new1[2] = Vrstva;

            gl_Logical2Real(iValue_old[0], iValue_old[1], Vrstva - 1,
              &test_pos, p_Level);

            // nepostupuj pres vytah, atd...
            if (p_Level->Level[test_pos])
              if (p_Level->Level[test_pos]->p_Object->Class != 12)
                break;

            if (p_real_pI->p_Object->Class == 2)
              break;

            p_Level->LastMesh = p_Level->Level[Item]->Index_Of_Game_Mesh;

            p_Level->Move_Exeption = 1;
            ret = gl_Move_Item(iValue_new, iValue_new1, real_p, p_Level);
            p_Level->Move_Exeption = 0;
            // jestlize se bedna neda posunou, ukonci
            if (ret < 0) {
              gl_Logical2Real(iValue[0], iValue[1], Vrstva, &real_p, p_Level);
              p_real_pI = p_Level->Level[real_p];

              if (p_real_pI)
                return -1;
            }
            Vrstva += 2;
            if (Vrstva > (p_Level->Size[2] - 1))
              break;
            iValue_new[2] += 2;
            gl_Logical2Real(iValue_old[0], iValue_old[1], Vrstva, &real_p,
              p_Level);
            p_real_pI = p_Level->Level[real_p];

            // jestlize vis uz nic neni,tak ukonci
            if (!p_real_pI)
              break;
          }
        }
    }
    p_Level->Level[real_pos] = p_Level->Level[Item];
    gl_Get_Move(iValue_old, iValue, iValue_new);

    if (p_Level->Level[real_pos]->p_Object->Class == 13) {
      p_set->PrivazanyMesh[p_set->pMesh].TopMesh = p_Level->LastMesh;
      p_set->PrivazanyMesh[p_set->pMesh].LowMesh =
        p_Level->Level[real_pos]->Index_Of_Game_Mesh;

      p_set->pMesh++;
    }
    else
      p_set->animation[p_set->last].p_matrix =
        rani_privaz_mesh(p_set->animation[p_set->last].p_run,
        p_Level->Level[real_pos]->Index_Of_Game_Mesh, 0);

    gl_Add_Mesh(&p_set->animation[p_set->last],
      p_Level->Level[real_pos]->Index_Of_Game_Mesh);
    //p_set->animation[p_set->last].mesh = p_Level->Level[real_pos]->Index_Of_Game_Mesh;

    p_real_posI = p_Level->Level[real_pos];
    p_real_posI->Pos[0] = iValue[0];
    p_real_posI->Pos[1] = iValue[1];
    p_real_posI->Pos[2] = iValue[2];

    am_Destroy_Steps(p_Level->Level[Item], p_Level);

    p_Level->Level[Item] = 0;
    if (!p_Level->bColumn) {
      p_Level->Column[0] = iValue[0];
      p_Level->Column[1] = iValue[1];
      p_Level->Column[2] = iValue[2];
      p_Level->bColumn++;
    }

    if (p_real_posI->p_Object->Class == 5 ||
      p_real_posI->p_Object->Class == 6)
      p_Level->bSunuti = 1;

    return real_pos;
  }
  else {
//----- ano

    gl_Logical2Real(iValue_old[0], iValue_old[1], iValue_old[2] - 2, &real_p,
      p_Level);

    if ((iValue_old[2] - 2) >= 0) {
      p_real_pI = p_Level->Level[real_p];
      if (p_real_pI)
        p_real_pO = p_real_pI->p_Object;
    }

    p_real_posI = p_Level->Level[real_pos];

    // posouvam berusku na vodu
    if (!gl_TestRule7(p_Level->Level[Item], iValue, p_Level))
      return -1;

    //-------------------------------------------------------------------------------------
    // sune se na sikminu
    //-------------------------------------------------------------------------------------
    if ((p_real_posI->p_Object->Class == 19) ||
      (p_real_posI->p_Object->Class == 20)) {
      int dest_pos;
      int Move[3], dest[3], m;
      ITEMDESC *p_destI;

      gl_Get_Move(iValue_old, iValue, Move);

      p_Level->bSikminaMoveExeption = 1;

      if (!Move[0]) {
        if ((Move[1] > 0) && (p_real_posI->Rotation == 2))
          goto Move_OK;
        if ((Move[1] < 0) && (!p_real_posI->Rotation))
          goto Move_OK;
        return -1;
      }
      else {
        if ((Move[0] > 0) && (p_real_posI->Rotation == 3))
          goto Move_OK;
        if ((Move[0] < 0) && (p_real_posI->Rotation == 1))
          goto Move_OK;
        return -1;
      }

    Move_OK:

      gl_Walk_Out_Of_Water(iValue_old,
        p_Level->Level[Item]->Index_Of_Game_Mesh, 400, p_Level);

      if ((p_real_posI->p_Object->Class == 20) && (p_ItemO->Class != 1))
        return -1;

      for (m = 0; m < 3; m++)
        dest[m] = iValue_old[m] + (2 * Move[m]);

      dest[2] += 2;

      p_Level->bSikminaUp = 1;

      //posunuju brouka na sikminu
      if (p_ItemO->Class == 1) {
        gl_Logical2Real(dest[0], dest[1], dest[2], &dest_pos, p_Level);

        p_destI = p_Level->Level[dest_pos];

        if (!p_destI) {
          //normalni posun. na destinaci neni nic

        BEATLE_MOVE:

          p_Level->Level[dest_pos] = p_Level->Level[Item];
          p_destI = p_Level->Level[dest_pos];
          p_Level->Level[Item] = 0;
          p_Level->Actual_Item = dest_pos;

          p_destI->Pos[0] = dest[0];
          p_destI->Pos[1] = dest[1];
          p_destI->Pos[2] = dest[2];

          if (p_set->animation[p_set->last].p_matrix == -1 &&
            p_set->animation[p_set->last].p_run != -1)
            rani_zrus(p_set->animation[p_set->last].p_run);

          p_set->animation[p_set->last].flag = 0;

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                Actual_Item]->Rotation + 10],
            &p_set->animation[p_set->last].flag);

          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            p_destI->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            p_destI->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

          gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
            Index_Of_Game_Mesh, 0, p_Level->iWalkAnimation,
            &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

          if (p_Level->bSunuti)
            gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
              Index_Of_Game_Mesh, 1, 2,
              &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

/*						if(p_Level->bSunuti)
							rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_kom_mesh_set_mesh,
											   p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 1,
											   (void *)p_Level->Level[p_Level->Actual_Item]->Rotation);*/

          p_set->last++;
          p_set->animation[p_set->last].bnext++;

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                Actual_Item]->Rotation + 69],
            &p_set->animation[p_set->last].flag);

          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            p_destI->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

          if (!p_Level->bSunuti)
            gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
              Index_Of_Game_Mesh, 1, p_Level->iWalkAnimation,
              &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

          rani_next_animace(p_set->animation[p_set->last - 1].p_run,
            p_set->animation[p_set->last].p_run, 0, 0, 0);

          rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
            anmend_Set_Flek_Flag_Anim,
            p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek, 0, NULL);

/*					if(!p_set->last && p_Level->bPosouvatKameru)
						rani_privaz_kameru(p_set->animation[p_set->last].p_run);*/

          p_Level->bVypniFlek = 1;
          p_Level->bSikminaSound = 1;
          p_Level->bSunuti = 0;

          am_Destroy_Steps(p_Level->Level[Item], p_Level);

          return p_Level->Actual_Item;
        }
        else {
          if (p_destI->p_Object->Class == 13) {
            POINTERSTRUCTURE *pS =
              (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

            if (pS) {
              pS->p_Level = p_Level;
              pS->iParam = p_Level->Level[dest_pos]->iItem;

              am_Set_Triger_Function(&p_set->animation[p_set->last],
                anmend_Take_Item,
                p_Level->Level[dest_pos]->Index_Of_Game_Mesh,
                p_Level->Level[dest_pos]->p_Object->SubClass, pS, 70, 0);
            }

            //na destinace je predmet
            if (!gl_Take_Item(dest_pos, Item, p_Level, 0, 0)) {
              free((void *) pS);
              am_Set_Triger_Function(&p_set->animation[p_set->last],
                NULL, 0, 0, 0, 1, 0);
              return -1;
            }

            gl_Destroy_Item(dest_pos, 1, p_Level);

            goto BEATLE_MOVE;
          }

          if (p_destI->p_Object->Class == 5) {
          NORMAL_BEATLE_ITEM_MOVE:

            //je tam dedna
            int next_dest[3];
            int weight = 0, cweight;
            int Lnext_dest;

            for (m = 0; m < 3; m++)
              next_dest[m] = dest[m];

            //zvazim 4 sloupce za sikminou (4 kvuli prasku, jinak bych vazil 2)
            for (m = 0; m < 3; m++) {
              cweight = gl_Count_Weight(next_dest, p_Level);
              /*kprintf(1,"[%d,%d,%d] w = %d", next_dest[0], next_dest[1],
                 next_dest[2], cweight); */

              if (!cweight)
                break;

              weight += cweight;
              next_dest[0] += Move[0];
              next_dest[1] += Move[1];
              next_dest[2] += Move[2];

              if ((next_dest[0] < 0) || (next_dest[0] >= p_Level->Size[0]))
                break;
              if ((next_dest[1] < 0) || (next_dest[1] >= p_Level->Size[1]))
                break;
            }

            //kprintf(1, "w = %d", weight);
            //pokud veci za sikminou vazi vic, nez je povoleno, tak fuj
            if (weight >
              p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
              return -1;

            for (m = 0; m < 3; m++)
              next_dest[m] = dest[m] + Move[m];

            // posun sloupce za sikminou
            gl_Logical2Real(dest[0], dest[1], dest[2], &Lnext_dest, p_Level);

            if (p_set->animation[p_set->last].p_matrix == -1 &&
              p_set->animation[p_set->last].p_run != -1)
              rani_zrus(p_set->animation[p_set->last].p_run);

            p_set->animation[p_set->last].flag = 0;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 6],
              &p_set->animation[p_set->last].flag);

            if (gl_Is_Move_Possible(dest, next_dest, p_Level)) {
              weight = gl_Move_Item(dest, next_dest, Lnext_dest, p_Level);

              if (weight < 0)
                return -1;
            }
            else
              return -1;

            next_dest[0] = dest[0];
            next_dest[1] = dest[1];
            next_dest[2] = dest[2] - 2;

            gl_Logical2Real(next_dest[0], next_dest[1], next_dest[2],
              &Lnext_dest, p_Level);

            if (p_Level->Level[Lnext_dest])
              am_Set_Triger_Function(&p_set->animation[p_set->last],
                anmend_Play_Sunuti,
                p_Level->Level[Lnext_dest]->p_Object->Material,
                0, p_Level, 55, 4);
            else
              am_Set_Triger_Function(&p_set->animation[p_set->last],
                anmend_Play_Sunuti, -1, 0, p_Level, 55, 4);

            p_set->last++;
            p_Level->bSunuti = 1;
            goto BEATLE_MOVE;
          }

          if (p_destI->p_Object->Class == 6) {
            //je tam vybusnina
            int next_dest[3];
            int Lnext_dest, Ldest;
            POINTERSTRUCTURE *pS =
              (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

            for (m = 0; m < 3; m++)
              next_dest[m] = dest[m] + Move[m];

            // jestlize tou vybusninou neni co vyhodit, tak skoc na normalni posun
            if (!gl_Count_Weight(next_dest, p_Level))
              goto NORMAL_BEATLE_ITEM_MOVE;

            gl_Logical2Real(dest[0], dest[1], dest[2], &Ldest, p_Level);
            gl_Logical2Real(next_dest[0], next_dest[1], next_dest[2],
              &Lnext_dest, p_Level);

            if ((p_ItemO->SubClass != 1) &&
              (p_Level->Level[Lnext_dest]->p_Object->Class != 5))
              return -1;

            if ((p_ItemO->SubClass == 1) &&
              ((p_Level->Level[Lnext_dest]->p_Object->Class != 5) &&
                (p_Level->Level[Lnext_dest]->p_Object->Class != 6)))
              return -1;

            if (p_set->animation[p_set->last].p_matrix == -1 &&
              p_set->animation[p_set->last].p_run != -1)
              rani_zrus(p_set->animation[p_set->last].p_run);

            p_set->animation[p_set->last].flag = 0;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 35],
              &p_set->animation[p_set->last].flag);

            p_set->animation[p_set->last].p_matrix =
              rani_privaz_mesh(p_set->animation[p_set->last].p_run,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0);

            gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
              Index_Of_Game_Mesh, 1, 2,
              &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

/*						rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_kom_mesh_set_mesh,
										   p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0,
										   (void *)p_Level->Level[p_Level->Actual_Item]->Rotation);*/

            p_set->last++;
            p_set->animation[p_set->last].bnext = 1;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 57],
              &p_set->animation[p_set->last].flag);

            p_set->animation[p_set->last].p_matrix =
              rani_privaz_mesh(p_set->animation[p_set->last].p_run,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0);

            gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
              Index_Of_Game_Mesh, 1, p_Level->iWalkAnimation,
              &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

            rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
              anmend_Set_Flek_Flag,
              p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek,
              0, NULL);

            rani_next_animace(p_set->animation[p_set->last - 1].p_run,
              p_set->animation[p_set->last].p_run, 0, 0, 0);

            am_Destroy_Steps(p_Level->Level[Item], p_Level);

            if (pS) {
              memcpy(pS->viParam1, dest, 3 * sizeof(int));
              memcpy(pS->viParam2, next_dest, 3 * sizeof(int));
              pS->p_Level = p_Level;
              pS->pParam = (void *) p_set;

              am_Set_Triger_Function(&p_set->animation[p_set->last],
                anmend_ExplozeBednyZaSikminou, Ldest, Lnext_dest, pS, 1, 0);
              p_set->last++;

            }
            else {
              int lc;

              // znic vybusnimu
              gl_Destroy_Item(Ldest, 4, p_Level);
              p_set->last++;
              gl_Throw_off(dest, p_Level);

              p_Level->Column[0] = dest[0];
              p_Level->Column[1] = dest[1];
              p_Level->Column[2] =
                dest[2] + (gl_Get_Column_Height(dest[0], dest[1], dest[2] - 1,
                  p_Level) - 2) * 2;

              if (p_Level->Column[2] < 0)
                lc = 0;
              else
                lc = p_Level->Column[2];

              p_Level->bColumn++;

              // znic bednu
              gl_Destroy_Item(Lnext_dest, 3, p_Level);
              gl_Throw_off(next_dest, p_Level);


              p_Level->Column[2] = lc;

              p_Level->SecColumn[0] = next_dest[0];
              p_Level->SecColumn[1] = next_dest[1];
              p_Level->SecColumn[2] =
                next_dest[2] + (gl_Get_Column_Height(next_dest[0],
                  next_dest[1], next_dest[2] - 1, p_Level) - 2) * 2;
              p_Level->bSecColumn++;

              p_Level->bCheckDependencesNeeded = 1;
            }

            p_Level->bSikminaSound = 1;
            p_Level->bVypniFlek = 1;
            p_Level->bSunuti = 1;
            Level.bSikminaUpExeption = 1;

            return p_Level->Actual_Item;
          }

          if (p_destI->p_Object->Class == 7) {
            //je tam kamen
            POINTERSTRUCTURE *pS =
              (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));
            int Ldest;

            if (p_Level->Item_Lock || p_Level->Flip)
              return -1;

            if (!p_Level->Level[Item]->p_Back_Pack->item[8])
              return -1;

            p_Level->Level[Item]->p_Back_Pack->item[8]--;
            p_Level->Level[Item]->p_Back_Pack->Count_of_Items--;

            if (p_set->animation[p_set->last].p_matrix == -1)
              rani_zrus(p_set->animation[p_set->last].p_run);

            p_set->animation[p_set->last].flag = 0;

            //chuze navrch
            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 78],
              &p_set->animation[p_set->last].flag);

            p_set->animation[p_set->last].p_matrix =
              rani_privaz_mesh(p_set->animation[p_set->last].p_run,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0);


            gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
              Index_Of_Game_Mesh, 1, 0,
              &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

            gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
              Index_Of_Game_Mesh, 2, p_Level->iWalkAnimation,
              &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

            rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
              anmend_kom_mesh_set_meshK,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 1,
              (void *) p_Level->Level[p_Level->Actual_Item]->Rotation);

            //buchnuti
            p_set->last++;
            p_set->animation[p_set->last].bnext = 1;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[77],
              &p_set->animation[p_set->last].flag);

            p_set->animation[p_set->last].p_matrix =
              rani_privaz_mesh(p_set->animation[p_set->last].p_run,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0);

            /*am_Set_Start_Function(&p_set->animation[p_set->last], anmend_PlayKrumpac, 
               p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0, NULL, 0); */

            gl_Logical2Real(dest[0], dest[1], dest[2], &Ldest, p_Level);

            if (pS) {
              pS->p_Level = p_Level;

              pS->viParam1[0] = Ldest;
              pS->viParam1[1] = 0;
              pS->viParam1[2] = 0;

              pS->viParam2[0] =
                p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh;
              pS->viParam2[1] = 0;
              pS->viParam2[2] =
                p_Level->Level[p_Level->Actual_Item]->Rotation;

              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                anmend_KamenZaSikmonou, 0, 0, (void *) pS);
            }
            else
              kprintf(1,
                "FATAL ERROR - nejde udelat pamet na data ukoncovaci funkce");

            rani_next_animace(p_set->animation[p_set->last - 1].p_run,
              p_set->animation[p_set->last].p_run, 0, 0, 0);

            //chuze dolu
            p_set->last++;
            p_set->animation[p_set->last].bnext = 1;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 82],
              &p_set->animation[p_set->last].flag);

            p_set->animation[p_set->last].p_matrix =
              rani_privaz_mesh(p_set->animation[p_set->last].p_run,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0);


            rani_next_animace(p_set->animation[p_set->last - 1].p_run,
              p_set->animation[p_set->last].p_run, 0, 0, 0);

            rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
              anmend_Set_Flek_Flag,
              p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek,
              0, NULL);

            p_set->last += 2;
            p_Level->bSikminaSound = 1;
            p_Level->bVypniFlek = 1;
            p_Level->bStoneSound = 1;
            return p_Level->Actual_Item;
          }

          return -1;
        }

      }

      //posunuju bednu na sikminu
      if (p_ItemO->Class == 5) {
        int p_sikmina;
        ITEMDESC *p_sikminaI;
        int item_pos[3];
        int weight;

        p_sikminaI = p_Level->Level[Item];

        // siknima se schodkem -> zadna bedna pred to nemuze
        if (p_real_posI->p_Object->Class == 20)
          return -1;

        gl_Logical2Real(dest[0], dest[1], dest[2], &dest_pos, p_Level);

        p_destI = p_Level->Level[dest_pos];

        item_pos[0] = p_Level->Level[Item]->Pos[0];
        item_pos[1] = p_Level->Level[Item]->Pos[1];
        item_pos[2] = p_Level->Level[Item]->Pos[2];

        item_pos[2] += 2;

        // nad bednou nesmi nic byt
        if (item_pos[2] <= p_Level->Size[2]) {
          gl_Logical2Real(item_pos[0], item_pos[1], item_pos[2], &p_sikmina,
            p_Level);
          if (p_Level->Level[p_sikmina])
            return -1;
        }

        // jestlize je to moc tezky, tak exit
        item_pos[2] -= 2;
        weight = gl_Count_Weight(item_pos, p_Level);
        if (weight >
          p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
          return -1;

        if (!p_destI) {
        NORMAL_BEATLE_ITEM_MOVE_:

          //normalni posun. na destinaci neni nic
          int anm_i;

          gl_Logical2Real(dest[0], dest[1], dest[2], &dest_pos, p_Level);
          gl_Logical2Real(iValue[0], iValue[1], iValue[2], &p_sikmina,
            p_Level);
          p_sikminaI = p_Level->Level[p_sikmina];

          p_Level->Level[dest_pos] = p_Level->Level[Item];
          p_destI = p_Level->Level[dest_pos];
          p_Level->Level[Item] = 0;

          p_destI->Pos[0] = dest[0];
          p_destI->Pos[1] = dest[1];
          p_destI->Pos[2] = dest[2];

          if (p_set->animation[p_set->last].p_matrix == -1 &&
            p_set->animation[p_set->last].p_run != -1) {
            rani_zrus(p_set->animation[p_set->last].p_run);
            p_set->animation[p_set->last].flag = 0;
          }
          else
            p_set->last++;

          switch (p_sikminaI->Rotation) {
            case 0:
              anm_i = 22;
              break;
            case 1:
              anm_i = 24;
              break;
            case 2:
              anm_i = 18;
              break;
            case 3:
              anm_i = 20;
              break;
          }

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[anm_i],
            &p_set->animation[p_set->last].flag);

          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            p_destI->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            p_destI->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

          p_set->last++;

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[anm_i + 1],
            &p_set->animation[p_set->last].flag);

          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            p_destI->Index_Of_Game_Mesh, 0);

          p_set->animation[p_set->last].bnext = 1;

          rani_next_animace(p_set->animation[p_set->last - 1].p_run,
            p_set->animation[p_set->last].p_run, 0, 0, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            p_destI->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = p_destI->Index_Of_Game_Mesh;

/*					if(!p_set->last && p_Level->bPosouvatKameru)
						rani_privaz_kameru(p_set->animation[p_set->last].p_run);*/

          p_set->last++;
          p_Level->Sikmina_Flag++;
          p_Level->bSikminaUp = 1;
          p_Level->bExtraSoundSunuti = 1;

          am_Destroy_Steps(p_Level->Level[Item], p_Level);

          return dest_pos;
        }
        else {
          //na destinaci je predmet
          ITEMDESC *p_sikminaI = p_Level->Level[Item];
          int next_dest[3];
          int cweight;

          if (p_destI->p_Object->Class == 5) {
            //na destinaci je bedna

            // jestlize soucet vah je moc velky, tak exit
            for (m = 0; m < 3; m++)
              next_dest[m] = dest[m];

            //zvazim 4 sloupce za sikminou (4 kvuli prasku, jinak bych vazil 2)
            for (m = 0; m < 3; m++) {
              cweight = gl_Count_Weight(next_dest, p_Level);
              if (!cweight)
                break;

              weight += cweight;

              next_dest[0] += Move[0];
              next_dest[1] += Move[1];
              next_dest[2] += Move[2];

              if ((next_dest[0] < 0) || (next_dest[0] >= p_Level->Size[0]))
                break;
              if ((next_dest[1] < 0) || (next_dest[1] >= p_Level->Size[1]))
                break;
            }

            if (weight >
              p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
              return -1;

            for (m = 0; m < 3; m++)
              next_dest[m] = dest[m] + Move[m];

            // posun sloupce za sikminou
            gl_Logical2Real(dest[0], dest[1], dest[2], &p_sikmina, p_Level);

            if (p_set->animation[p_set->last].p_matrix == -1 &&
              p_set->animation[p_set->last].p_run != -1)
              rani_zrus(p_set->animation[p_set->last].p_run);

            p_set->animation[p_set->last].flag = 0;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 6],
              &p_set->animation[p_set->last].flag);

            if (gl_Is_Move_Possible(dest, next_dest, p_Level)) {
              weight = gl_Move_Item(dest, next_dest, p_sikmina, p_Level);
              if (weight < 0)
                return -1;
            }
            else
              return -1;

            goto NORMAL_BEATLE_ITEM_MOVE_;
          }

          if (p_destI->p_Object->Class == 6) {
            //chci vedet, co je za vybusninou
            for (m = 0; m < 3; m++)
              next_dest[m] = dest[m] + Move[m];

            gl_Logical2Real(next_dest[0], next_dest[1], next_dest[2],
              &p_sikmina, p_Level);

            if (!p_Level->Level[p_sikmina]) {
              weight += gl_Count_Weight(dest, p_Level);

              if (weight >
                p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
                return -1;
            }

            gl_Logical2Real(dest[0], dest[1], dest[2], &p_sikmina, p_Level);

            if (p_set->animation[p_set->last].p_matrix == -1 &&
              p_set->animation[p_set->last].p_run != -1)
              rani_zrus(p_set->animation[p_set->last].p_run);

            p_set->animation[p_set->last].flag = 0;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 6],
              &p_set->animation[p_set->last].flag);

            if (gl_Is_Move_Possible(dest, next_dest, p_Level)) {
              weight = gl_Move_Item(dest, next_dest, p_sikmina, p_Level);
              if (weight < 0)
                return -1;
            }
            else
              return -1;
          }
        }
      }

      if (p_ItemO->Class == 6) {
        //tlasim vybusninu
        int Ldest;
        int item_pos[3];
        int weight;
        int anm_i;
        int p_sikmina;
        ITEMDESC *p_sikminaI;

        // jestlize tou vybusninou neni co vyhodit, tak skoc na normalni posun
        weight = gl_Count_Weight(p_Level->Level[Item]->Pos, p_Level);

        if (weight >
          p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Strength)
          return -1;

        gl_Logical2Real(dest[0], dest[1], dest[2], &Ldest, p_Level);
        gl_Logical2Real(iValue[0], iValue[1], iValue[2], &p_sikmina, p_Level);
        p_sikminaI = p_Level->Level[p_sikmina];

        if (!p_Level->Level[Ldest])
          goto NORMAL_BEATLE_ITEM_MOVE_;

        if ((p_ItemO->SubClass != 1) &&
          (p_Level->Level[Ldest]->p_Object->Class != 5))
          return -1;

        if ((p_ItemO->SubClass == 1) &&
          ((p_Level->Level[Ldest]->p_Object->Class != 5) &&
            (p_Level->Level[Ldest]->p_Object->Class != 6)))
          return -1;

        // znic vybusnimu
        item_pos[0] = p_Level->Level[Item]->Pos[0];
        item_pos[1] = p_Level->Level[Item]->Pos[1];
        item_pos[2] = p_Level->Level[Item]->Pos[2];

        if (p_set->animation[p_set->last].p_matrix == -1 &&
          p_set->animation[p_set->last].p_run != -1) {
          rani_zrus(p_set->animation[p_set->last].p_run);
          p_set->animation[p_set->last].flag = 0;
        }
        else
          p_set->last++;

        switch (p_sikminaI->Rotation) {
          case 0:
            anm_i = 22;
            break;
          case 1:
            anm_i = 24;
            break;
          case 2:
            anm_i = 18;
            break;
          case 3:
            anm_i = 20;
            break;
        }

        p_set->animation[p_set->last].p_run =
          rani_aktivuj_cekej(am.sim_anim[anm_i],
          &p_set->animation[p_set->last].flag);

        p_set->animation[p_set->last].p_matrix =
          rani_privaz_mesh(p_set->animation[p_set->last].p_run,
          p_Level->Level[Item]->Index_Of_Game_Mesh, 0);

        gl_Add_Mesh(&p_set->animation[p_set->last],
          p_Level->Level[Item]->Index_Of_Game_Mesh);
        //p_set->animation[p_set->last].mesh = p_Level->Level[Item]->Index_Of_Game_Mesh;

/*				if(!p_set->last && p_Level->bPosouvatKameru)
					rani_privaz_kameru(p_set->animation[p_set->last].p_run);*/

        rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
          anmend_ExplozeBedny, (int) p_Level->Level[Item]->Index_Of_Game_Mesh,
          Ldest, p_Level);

        p_Level->Sikmina_Flag++;
        p_Level->bSikminaUp = -1;
        p_set->last++;
        p_Level->Throw_off = 1;

        am_Destroy_Steps(p_Level->Level[Item], p_Level);
        gl_Destroy_Item(Item, 1, p_Level);

        return Item;
      }

      return -1;
    }

    //pokud je pod predmetem pontonka, tak mozna je tam voda a musi se to setrast
    gl_Kontrola_Pontonky(iValue, p_Level);

    p_real_posO = p_Level->Level[real_pos]->p_Object;
    // pokud beruska chce vzit predmet
    if ((p_ItemO->Class == 1) && (p_real_posO->Class == 13)) {
      POINTERSTRUCTURE *pS =
        (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

      if (pS) {
        pS->p_Level = p_Level;
        pS->iParam = p_Level->Level[real_pos]->iItem;

        am_Set_Triger_Function(&p_set->animation[p_set->last],
          anmend_Take_Item,
          p_Level->Level[real_pos]->Index_Of_Game_Mesh,
          p_Level->Level[real_pos]->p_Object->SubClass, pS, 1, 0);
      }

      //na destinace je predmet
      if (!gl_Take_Item(real_pos, Item, p_Level, 0, 0)) {
        free((void *) pS);
        am_Set_Triger_Function(&p_set->animation[p_set->last],
          NULL, 0, 0, 0, 1, 0);
        return -1;
      }

      gl_Destroy_Item(real_pos, 1, p_Level);

      p_Level->Level[real_pos] = p_Level->Level[Item];
      p_real_posI = p_Level->Level[real_pos];

      gl_Get_Move(iValue_old, iValue, iValue_new);

      p_set->animation[p_set->last].p_matrix =
        rani_privaz_mesh(p_set->animation[p_set->last].p_run,
        p_Level->Level[real_pos]->Index_Of_Game_Mesh, 0);

      gl_Add_Mesh(&p_set->animation[p_set->last],
        p_Level->Level[real_pos]->Index_Of_Game_Mesh);
      //p_set->animation[p_set->last].mesh = p_Level->Level[real_pos]->Index_Of_Game_Mesh;

      p_real_posI->Pos[0] = iValue[0];
      p_real_posI->Pos[1] = iValue[1];
      p_real_posI->Pos[2] = iValue[2];
      p_Level->Level[Item] = 0;
      return real_pos;
    }

    // pokud posouvam berusku na kamen
    if ((p_ItemO->Class == 1) && (p_real_posO->Class == 7)) {
      if (p_Level->Item_Lock || p_Level->Flip)
        return -1;

      if (p_Level->Level[Item]->p_Back_Pack->item[8]) {
        float fpos[3];
        int rot;
        int mat = gl_Get_Mesh_Material(p_real_posI->Index_Of_Game_Mesh);

        kom_mesh_get_float(p_real_posI->Index_Of_Game_Mesh, &fpos[0],
          &fpos[1], &fpos[2], &rot);

        if (p_set->animation[p_set->last].p_run != -1) {
          rani_zrus(p_set->animation[p_set->last].p_run);
          p_set->animation[p_set->last].p_run = -1;
        }

        p_set->animation[p_set->last].flag = 0;
        p_set->animation[p_set->last].p_run =
          rani_aktivuj_cekej(am.sim_anim[77],
          &p_set->animation[p_set->last].flag);
        p_set->animation[p_set->last].p_matrix =
          rani_privaz_mesh(p_set->animation[p_set->last].p_run,
          Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh, 0);

        rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_Kamen,
          real_pos, 0, (void *) p_Level);

        p_set->last++;

        am_Remove_Beetle_Animation(p_Level->Level[p_Level->Actual_Item]->
          Index_Of_Game_Mesh, p_Level);

        gl_Set_3ds_Mesh(1, 1,
          p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
          p_Level->Level[p_Level->Actual_Item]->Rotation);
        gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
          Index_Of_Game_Mesh, 0, 0,
          &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);

        p_Level->Level[Item]->p_Back_Pack->item[8]--;
        p_Level->Level[Item]->p_Back_Pack->Count_of_Items--;

        //                      ap_Play_Sound(0,0,0, fpos, 162 + (rand()%2), NULL, &ad);

        //gl_Destroy_Item(real_pos,0,p_Level);
        //gl_Do_Kameni(fpos, p_Level, mat);
        p_Level->Flip++;
        gl_Next_Queue_Set(&Level);
        gl_Go_Animations();
        //gl_Throw_off(iValue,p_Level);
        p_Level->status = 5;
        p_Level->bAllow_Key = 0;
        p_Level->bStone = 1;
        p_Level->Throw_off = 1;
        p_Level->bDemoSaveExeption = 1;
        return -1;
      }
    }

    // pokud pousouvam vybusninu, tak to co je predemnou vyhodit?
    if (p_ItemO->Class == 6)
      if ((p_real_posO->Class == 5) ||
        ((p_ItemO->SubClass == 1) &&
          ((p_real_posO->Class == 5) ||
            (p_real_posO->Class == 6) ||
            (p_real_posO->Class == 7) ||
            (p_real_posO->Class == 3) || (p_real_posO->Class == 16)
          )
        )
        ) {
        int lc;

        //jeslize je flip, tak to znamena, ze animac jeste nedobehla a ja bych ji
        //smazal mesh a spadlo by to, takze nic
        if (p_Level->Flip)
          return -1;

        if (!gl_Check_Detonation_Pack_Water_Rules(p_real_posI->Pos, p_Level))
          return -1;

        // znic vybusninu
        rani_zrus(p_set->animation[p_set->last].p_run);
        p_set->animation[p_set->last].p_run = -1;
        gl_Destroy_Item(Item, 4, p_Level);
        //p_set->last++;
        gl_Throw_off(iValue_old, p_Level);

        p_Level->Column[0] = iValue_old[0];
        p_Level->Column[1] = iValue_old[1];
        p_Level->Column[2] =
          iValue_old[2] + (gl_Get_Column_Height(iValue_old[0], iValue_old[1],
            iValue_old[2] - 1, p_Level) - 2) * 2;

        if (p_Level->Column[2] < 0)
          lc = 0;
        else
          lc = p_Level->Column[2];

        p_Level->bColumn++;

        // znic bednu
        gl_Destroy_Item(real_pos, 3, p_Level);
        gl_Throw_off(iValue, p_Level);

        p_Level->Column[2] = lc;

        p_Level->SecColumn[0] = iValue[0];
        p_Level->SecColumn[1] = iValue[1];
        p_Level->SecColumn[2] =
          iValue[2] + (gl_Get_Column_Height(iValue[0], iValue[1],
            iValue[2] - 1, p_Level) - 2) * 2;
        p_Level->bSecColumn++;

        p_Level->bCheckDependencesNeeded = 1;
        p_Level->bDemoSaveExeption = 1;
        return -1;
      }


    // pokud je to bedna, nebo vybusnina tak ji posun
    if ((p_real_posO->Class == 5) || (p_real_posO->Class == 6)) {
      if (!gl_Test_Rule3(p_Level->Level[Item], iValue, p_Level))
        return -1;

      for (i = 0; i < 3; i++) {
        iValue_new[i] = iValue[i] - iValue_old[i];
        iValue_new[i] += iValue[i];
      }

      // posouva bednu
      ret = gl_Move_Item(iValue, iValue_new, real_pos, p_Level);

      //bylo posouvano na sikmine, je treba udelat novou animaci pro predmety pred timto
      if (p_Level->Sikmina_Flag) {
        //neni-li to brouk, udelam normalni animaci
        if (p_Level->Level[Item]->p_Object->Class != 1) {
          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                Actual_Item]->Rotation], &p_set->animation[p_set->last].flag);
          p_Level->Sikmina_Flag = 0;
        }
        else                    //jinak udelam special animaci
        {
          if (p_Level->bSikminaUp == 1) {
            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 45],
              &p_set->animation[p_set->last].flag);

            p_set->last++;
            p_set->animation[p_set->last].bnext = 1;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 53],
              &p_set->animation[p_set->last].flag);

            rani_next_animace(p_set->animation[p_set->last - 1].p_run,
              p_set->animation[p_set->last].p_run, 0, 0, 0);

            p_Level->bSunuti = 1;
            p_Level->bSikminaSound = 2;
            p_Level->bMultiConnect = 1;
          }
          else if (!p_Level->bSikminaUp) {
            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation + 49],
              &p_set->animation[p_set->last].flag);

            p_Level->bSunuti = 1;
            p_Level->bSikminaSound = 2;
          }
          else {
            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[p_Level->Level[p_Level->
                  Actual_Item]->Rotation],
              &p_set->animation[p_set->last].flag);

            p_Level->bSunuti = 1;

            rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
              anmend_Add_Beetle_Animation,
              p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
              p_Level->Level[p_Level->Actual_Item]->Rotation,
              (void *) p_Level);
          }

        }
      }

      // podarilo se ji posunout?
      if (ret >= 0) {
        Vrstva += 4;
        iValue_new[2] += 2;
        // zkusi posunout dedny nad ni
        if (Vrstva >= 0 && Vrstva < p_Level->Size[2]
          && iValue_new[2] < p_Level->Size[2]) {
          gl_Logical2Real(iValue[0], iValue[1], Vrstva, &real_p, p_Level);
          p_real_pI = p_Level->Level[real_p];

          // pokud je nad ni bedna, posun ji
          if (p_real_pI) {
            int iValue_new1[3];
            int test_pos;

            // postupuj az ke stropu
            while (Vrstva < p_Level->Size[2]) {
              iValue_new1[0] = iValue[0];
              iValue_new1[1] = iValue[1];
              iValue_new1[2] = iValue_new[2];

              if (p_Level->Level[real_p]->p_Object->Class == 2)
                break;

              if (Vrstva - 1 < 0)
                break;

              gl_Logical2Real(iValue_old[0], iValue_old[1], Vrstva - 1,
                &test_pos, p_Level);

              // nepostupuj pres vytah, atd...
              if (p_Level->Level[test_pos])
                if (p_Level->Level[test_pos]->p_Object->Class != 12)
                  break;

              p_Level->Move_Exeption = 1;
              ret = gl_Move_Item(iValue_new1, iValue_new, real_p, p_Level);
              p_Level->Move_Exeption = 0;

              // jestlize se bedna neda posunou, ukonci
              if (ret < 0) {
                gl_Logical2Real(iValue_old[0], iValue_old[1], Vrstva,
                  &real_p, p_Level);
                if (p_Level->Level[real_p])
                  return -1;
              }

              Vrstva += 2;
              if (Vrstva > (p_Level->Size[2] - 1))
                break;

              iValue_new[2] += 2;

              //????????????????????????????????????????????????????????????????????????????????????????
              if (iValue_new[2] > (p_Level->Size[2] - 1))
                break;          //////////////////////////?????????????????????
              //????????????????????????????????????????????????????????????????????????????????????????

              gl_Logical2Real(iValue[0], iValue[1], Vrstva, &real_p, p_Level);
              // jestlize vis uz nic neni,tak ukonci
              if (!p_Level->Level[real_p])
                break;
            }
          }
        }

        p_Level->Level[real_pos] = p_Level->Level[Item];
        p_real_posI = p_Level->Level[real_pos];
        gl_Get_Move(iValue_old, iValue, iValue_new);
        if (!p_Level->bMultiConnect)
          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            p_real_posI->Index_Of_Game_Mesh, 0);
        else {
          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            p_real_posI->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh);
//                                      p_set->animation[p_set->last].mesh = 
//                                              Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh;

          p_set->animation[p_set->last - 1].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last - 1].p_run,
            p_real_posI->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last - 1],
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh);
//                                      p_set->animation[p_set->last-1].mesh = 
//                                              Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh;

          gl_Set_3ds_Anim(p_Level->Level[p_Level->Actual_Item]->
            Index_Of_Game_Mesh, 1, 2,
            &p_Level->Level[p_Level->Actual_Item]->_3ds_flag, 0, 0, 0);

/*					rani_pripoj_funkci(p_set->animation[p_set->last-1].p_run, anmend_kom_mesh_set_mesh,
									   p_real_posI->Index_Of_Game_Mesh, 0,
									   (void *)p_real_posI->Rotation);*/

          p_Level->bMultiConnect = 0;
        }

        gl_Add_Mesh(&p_set->animation[p_set->last],
          p_real_posI->Index_Of_Game_Mesh);
        //p_set->animation[p_set->last].mesh = p_real_posI->Index_Of_Game_Mesh;

        p_real_posI->Pos[0] = iValue[0];
        p_real_posI->Pos[1] = iValue[1];
        p_real_posI->Pos[2] = iValue[2];

        am_Destroy_Steps(p_Level->Level[Item], p_Level);

        p_Level->Level[Item] = 0;
        return real_pos;
      }

      // nelze posunout
      return -1;
    }
  }

  return -1;
}

//------------------------------------------------------------------------------------------------
// zjisti vahu od urcite vrstvy
//------------------------------------------------------------------------------------------------
int gl_Count_Weight(int *column, LEVELINFO * p_Level)
{
  int i, weight = 0;
  int real_pos;
  OBJECTDESC *p_real_posO;

  for (i = column[2]; i < p_Level->Size[2]; i += 2) {
    //real_pos = column[0] + column[1]*p_Level->Size[0] + i*p_Level->Size[0]*p_Level->Size[1];
    gl_Logical2Real(column[0], column[1], i, &real_pos, p_Level);

    if (p_Level->Level[real_pos]) {
      p_real_posO = p_Level->Level[real_pos]->p_Object;

      if (p_real_posO->Class == 2)
        break;

      if (p_real_posO->Class == 1) {
        weight++;
        if (p_Level->Level[real_pos]->p_Back_Pack->item[7])
          weight++;
      }

      if ((p_real_posO->Class == 5) && (!p_real_posO->SubClass))
        weight += 2;

      if ((p_real_posO->Class == 5) && (p_real_posO->SubClass))
        weight++;

      if (p_real_posO->Class == 6)
        weight += 2;

      if (p_real_posO->Class == 7)
        weight += 2;

      if (p_real_posO->Class == 17)
        weight += 2;

      if (p_real_posO->Class == 13)
        weight++;
    }
    else
      break;
  }

  return weight;
}

int gl_Stlac_Tlacitko(ITEMDESC * pTlacitko, char *bPowerOn,
  LEVELINFO * p_Level)
{
  int p_run;
  int pos[3];
  int r;
  char off;

  *bPowerOn = -1;

  memcpy((void *) pos, (void *) pTlacitko->Pos, 3 * sizeof(int));
  pos[2]++;

  gl_Logical2Real(pos[0], pos[1], pos[2], &r, p_Level);

  if ((p_Level->Level[r] && pTlacitko->p_Back_Pack->item[0]) ||
    (!p_Level->Level[r] && !pTlacitko->p_Back_Pack->item[0]))
    return 0;

  if (p_Level->Level[r] && !pTlacitko->p_Back_Pack->item[0]) {
    off = 0;
    pTlacitko->p_Back_Pack->item[0] = 1;
    *bPowerOn = 1;
  }
  else {
    *bPowerOn = 0;
    off = 1;
    pTlacitko->p_Back_Pack->item[0] = 0;
  }

  if (!off)
    p_run =
      rani_aktivuj(am.sim_anim[43], &p_Level->TrashFlag, GK_REMOVE, 0, 0);
  else
    p_run =
      rani_aktivuj(am.sim_anim[44], &p_Level->TrashFlag, GK_REMOVE, 0, 0);


  rani_privaz_mesh(p_run, pTlacitko->Index_Of_Game_Mesh, 0);

  anmend_Tlacitko((int) pTlacitko, 0, (void *) p_Level);

  return 0;
}

//------------------------------------------------------------------------------------------------
// provede akci na tlacitko
//------------------------------------------------------------------------------------------------
int gl_Do_Button(long real_pos, char bUse, LEVELINFO * p_Level)
{
  int test_pos, ret, connection, connection1;
  int pos[3];
  int ret_value = 0;
  char bPowerOn;
  ITEMDESC *p_connection, *p_connection1;

  connection = p_Level->Level[real_pos]->Connection[0];
  connection1 = p_Level->Level[real_pos]->Connection[1];

  p_connection = p_Level->Level[connection];
  p_connection1 = p_Level->Level[connection1];

  ret_value = gl_Stlac_Tlacitko(p_Level->Level[real_pos], &bPowerOn, p_Level);

  if (!bPowerOn && p_connection)
    if (p_connection->p_Object->Class == 9)
      am_TurnOff_The_Teleport(p_connection, p_Level);

  if (!bPowerOn && p_connection)
    if (p_connection->p_Object->Class == 11)
      p_connection->p_Back_Pack->item[1] = 0;

  if (!bPowerOn && p_connection1)
    if (p_connection1->p_Object->Class == 11)
      p_connection1->p_Back_Pack->item[1] = 0;

  if (!bUse)
    return ret_value;

  // aktivuj teleport
  if (p_connection)
    if (p_connection->p_Object->Class == 9) {
      pos[0] = p_connection->Pos[0];
      pos[1] = p_connection->Pos[1];
      pos[2] = p_connection->Pos[2];

      //test_pos = pos[0] + pos[1]*p_Level->Size[0] + (pos[2]+1)*p_Level->Size[0]*p_Level->Size[1];
      if (bPowerOn == 1)
        am_TurnOn_The_Teleport(p_connection, p_Level);

      gl_Logical2Real(pos[0], pos[1], pos[2] + 1, &test_pos, p_Level);

      if (p_Level->Level[test_pos]) {
        ret = gl_Teleport_Item(connection, test_pos, p_Level);

        if (ret != -1)
          ret_value = 1;

      }
      return ret_value;
    }

  // aktivuj vytah
  if (p_connection)
    if (p_connection->p_Object->Class == 11) {
      pos[0] = p_connection->Pos[0];
      pos[1] = p_connection->Pos[1];
      pos[2] = p_connection->Pos[2];

      ret_value = gl_Do_Lift(connection, pos, p_Level);
      return ret_value;
    }

  if (p_connection1)
    if (p_connection1->p_Object->Class == 11) {
      pos[0] = p_connection1->Pos[0];
      pos[1] = p_connection1->Pos[1];
      pos[2] = p_connection1->Pos[2];

      ret_value = gl_Do_Lift(connection1, pos, p_Level);
      return ret_value;
    }

  return ret_value;
}

int gl_Lift_Is_Move_Possible(ITEMDESC * p_Lift, int *Beruska, int real_pos,
  LEVELINFO * p_Level, int *move)
{
  int pos[3], dist, i;
  int real;

  memcpy((void *) pos, (void *) p_Lift->Pos, 3 * sizeof(int));

  pos[2]++;

  while (pos[2] < p_Level->Size[2]) {
    gl_Logical2Real(pos[0], pos[1], pos[2], &real, p_Level);

    if (!p_Level->Level[real])
      break;

    if (p_Level->Level[real]->p_Object->Class == 1 &&
      !p_Level->Level[real]->p_Back_Pack->item[0])
      *Beruska = 1;

    pos[2] += 2;
  }

  dist = pos[2] - (p_Lift->Pos[2] + 1);

//      gl_Real2Logical(real_pos, pos, p_Level);

  if ((pos[2] + dist - 2) >= p_Level->Size[2]) {
    *move = ftoi(((pos[2] + dist - 2) - p_Level->Size[2]) / 2.0f);
    return 0;
  }
  else {
    int max = pos[2] + dist;

    if (max >= p_Level->Size[2])
      max = p_Level->Size[2];

    for (i = pos[2]; i < max; i++) {
      gl_Logical2Real(pos[0], pos[1], i, &real, p_Level);

      if (p_Level->Level[real])
        if (p_Level->Level[real]->p_Object->Class != 12) {
          *move = ftoi((i - pos[2]) / 2.0f);
          return 0;
        }
    }

    return 1;
  }
}

int gl_Is_There_Water(int *from, int *to, LEVELINFO * p_Level)
{
  int i;
  int r;

  for (i = from[2]; i < to[2]; i++) {
    gl_Logical2Real(from[0], from[1], i, &r, p_Level);

    if (i > -1)
      if (p_Level->Level[r])
        if (p_Level->Level[r]->p_Object->Class == 12)
          return i;
  }

  return -1;
}

int gl_Find_Top_Of_Lift_things(LEVELINFO * p_Level, int x, int y, int z)
{
  int r;
  int i;

  for (i = z; i < p_Level->Size[2]; i += 2) {
    gl_Logical2Real(x, y, i, &r, p_Level);

    if (!p_Level->Level[r])
      return i;
    else if (p_Level->Level[r]->p_Object->Class == 2)
      return i;
  }

  return i;
}

//------------------------------------------------------------------------------------------------
// provede akci na vytah
//------------------------------------------------------------------------------------------------
int gl_Do_Lift(int Lift, int *pos, LEVELINFO * p_Level)
{
  int test_pos, real_pos, test1_pos;
  int item, new_pos[3], move[3], old_Zpos;
  int i, j;
  char bMove;

  //int    Lift_Zpos;
  int voda;
  int vpos[3];
  int lpos[3];
  int VyskaSloupce = 0;
  int Beruska = 0;
  char SmerDolu;
  int liftanim;
  int LiftTop;
  int lmove = 0;

  ITEMDESC *p_LiftI = p_Level->Level[Lift];

  memcpy((void *) lpos, (void *) p_LiftI->Pos, 3 * sizeof(int));

  gl_Logical2Real(pos[0], pos[1], pos[2] + 1, &test_pos, p_Level);

  item = (int) p_Level->Level[test_pos];

  if (!p_LiftI->p_Object->SubClass) {
    if (item != p_LiftI->p_Back_Pack->item[2])
      bMove = 1;
    else
      return 0;
  }

  if (p_LiftI->p_Object->SubClass == 1) {
    //item = (long)p_Level->Level[test_pos];

    if ((!p_LiftI->p_Back_Pack->item[2] && item) ||
      (!item && p_LiftI->p_Back_Pack->item[2]))
      bMove = 1;
    else {
      p_LiftI->p_Back_Pack->item[2] = item;
      return 0;
    }
  }

  if (p_LiftI->p_Object->SubClass == 2) {
    if (p_LiftI->p_Back_Pack->item[1] < 10) {
      p_LiftI->p_Back_Pack->item[1]++;
      return 0;
    }
    else {
      if (!p_LiftI->p_Back_Pack->item[0])
        bMove = 1;
      p_LiftI->p_Back_Pack->item[1] = 0;
    }
  }

  if (p_LiftI->p_Object->SubClass == 3) {
    if (!p_LiftI->p_Back_Pack->item[1]) {
      p_LiftI->p_Back_Pack->item[1] = 1;
      bMove = 1;
    }
    else
      return 0;
  }

  if (bMove) {
    p_LiftI->p_Back_Pack->item[3] = 1;
    p_LiftI->p_Back_Pack->item[2] = item;

    // real_pos = pozice destinace vytahu
    if (p_LiftI->Connection[0] == Lift)
      real_pos = p_LiftI->Connection[1];
    else
      real_pos = p_LiftI->Connection[0];

    gl_Real2Logical(real_pos, new_pos, p_Level);
    gl_Get_Move(pos, new_pos, move);

    //vertikalni vytah
    if (!move[0] && !move[1]) {
      // jestlize je vytah navrchu, tak musim promazat zdi
      if (p_LiftI->p_Back_Pack->item[4]) {
        for (j = (new_pos[2] + 1); j < pos[2]; j += 2) {
          gl_Logical2Real(pos[0], pos[1], j, &test_pos, p_Level);
          p_Level->Level[test_pos] = 0;
        }

        SmerDolu = 1;
      }
      else
        SmerDolu = 0;

      if (!gl_Lift_Is_Move_Possible(p_LiftI, &Beruska, real_pos, p_Level,
          &lmove) && !SmerDolu) {
        //posun vytahu a veci na nem TAM A ZPET
        p_set->animation[p_set->last].p_anim = sim_vyrob_animaci(3, 0, 0);

        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 0, 0.0f,
          0.0f, 0.0f, 0);
        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 1, 0.0f,
          (float) (lmove * 2), 0.0f, abs(lmove * 5) - 1);
        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 2, 0.0f,
          0.0f, 0.0f, 2 * abs(lmove * 5) - 1);
        sim_interpoluj_animaci(p_set->animation[p_set->last].p_anim,
          2 * abs(lmove * 5), 0);

        p_set->animation[p_set->last].p_run =
          rani_aktivuj_cekej(p_set->animation[p_set->last].p_anim,
          &p_set->animation[p_set->last].flag);

        p_set->animation[p_set->last].p_matrix =
          rani_privaz_mesh(p_set->animation[p_set->last].p_run,
          p_LiftI->Index_Of_Game_Mesh, 0);

        gl_Add_Mesh(&p_set->animation[p_set->last],
          p_LiftI->Index_Of_Game_Mesh);
        //p_set->animation[p_set->last].mesh = p_LiftI->Index_Of_Game_Mesh;

        am_Set_Start_Function(&p_set->animation[p_set->last], anmend_Lift,
          2 * lmove, Lift, p_Level, 0);

        am_Set_aMaterial_Trigers(&p_set->animation[p_set->last], p_LiftI,
          p_Level);

        p_set->animation[p_set->last].flag = 0;

        //posun veci nad vytahem
        for (i = p_LiftI->Pos[2] + 1; i < p_Level->Size[2]; i += 2) {
          gl_Logical2Real(pos[0], pos[1], i, &test_pos, p_Level);

          if (!p_Level->Level[test_pos]) {
            p_set->last++;
            return 1;
          }
          else {
            if (p_Level->Level[test_pos]->p_Object->Class == 13) {
              //kom_svaz_meshe(p_LiftI->Index_Of_Game_Mesh, p_Level->Level[test_pos]->Index_Of_Game_Mesh);
              am_Set_Start_Function(&p_set->animation[p_set->last],
                anmend_Lift_Item, p_LiftI->Index_Of_Game_Mesh,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, p_Level, 1);

              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                animend_Lift_End,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0, 0);
            }
            else
              if (p_Level->Level[test_pos]->p_Object->Class == 5 ||
              p_Level->Level[test_pos]->p_Object->Class == 6 ||
              p_Level->Level[test_pos]->p_Object->Class == 1 ||
              p_Level->Level[test_pos]->p_Object->Class == 7) {
              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_Level->Level[test_pos]->Index_Of_Game_Mesh;
            }

            if (test_pos == p_Level->Actual_Item && p_Level->bPosouvatKameru)
              rani_privaz_kameru(p_set->animation[p_set->last].p_run);
          }
        }

        return 1;
      }

      vpos[0] = pos[0];
      vpos[1] = pos[1];
      vpos[2] = pos[2] - 1;

      if (SmerDolu) {
        gl_Find_Bottom(vpos, &voda, p_Level);

        if (Beruska && voda != -1)
          return 0;
      }
      else
        voda = gl_Is_There_Water(vpos, new_pos, p_Level);

      //posun vytahu a veci na nem
      liftanim = p_set->last;
      p_set->animation[p_set->last].p_anim = sim_vyrob_animaci(2, 0, 0);

      sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 0, 0.0f, 0.0f,
        0.0f, 0);
      sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 1, 0.0f,
        (float) (move[2] * 2), 0.0f, abs(move[2] * 5) - 1);
      sim_interpoluj_animaci(p_set->animation[p_set->last].p_anim,
        abs(move[2] * 5), 0);

      p_set->animation[p_set->last].p_run =
        rani_aktivuj_cekej(p_set->animation[p_set->last].p_anim,
        &p_set->animation[p_set->last].flag);

      p_set->animation[p_set->last].p_matrix =
        rani_privaz_mesh(p_set->animation[p_set->last].p_run,
        p_LiftI->Index_Of_Game_Mesh, 0);

      gl_Add_Mesh(&p_set->animation[p_set->last],
        p_LiftI->Index_Of_Game_Mesh);
      //p_set->animation[p_set->last].mesh = p_LiftI->Index_Of_Game_Mesh;

      am_Set_Start_Function(&p_set->animation[p_set->last], anmend_Lift,
        move[2], real_pos, p_Level, 0);

      am_Set_aMaterial_Trigers(&p_set->animation[p_set->last], p_LiftI,
        p_Level);

      p_set->animation[p_set->last].flag = 0;

      // posun vytahu
      p_Level->Level[real_pos] = p_Level->Level[Lift];
      p_Level->Level[Lift] = 0;
      p_LiftI = p_Level->Level[real_pos];

      old_Zpos = p_LiftI->Pos[2];

      p_LiftI->Pos[0] = new_pos[0];
      p_LiftI->Pos[1] = new_pos[1];
      p_LiftI->Pos[2] = new_pos[2];

      LiftTop =
        gl_Find_Top_Of_Lift_things(p_Level, pos[0], pos[1], old_Zpos + 1);

      //posun veci nad vytahem
      //-->nahoru for(i=LiftTop-2;i > old_Zpos;i-=2)
      //-->dolu   for(i=old_Zpos+1;i < LiftTop;i+=2)

      if (SmerDolu)
        for (i = old_Zpos + 1; i < LiftTop; i += 2) {
          gl_Logical2Real(pos[0], pos[1], i, &test_pos, p_Level);

          if (p_Level->Level[test_pos]) {
            VyskaSloupce++;

            new_pos[0] = p_LiftI->Pos[0];
            new_pos[1] = p_LiftI->Pos[1];
            new_pos[2] = i + move[2] * 2;

            gl_Logical2Real(new_pos[0] + move[0], new_pos[1] + move[1],
              i + move[2] * 2, &test1_pos, p_Level);

            if (p_Level->Level[test_pos]->p_Object->Class == 13) {
              //kom_svaz_meshe(p_LiftI->Index_Of_Game_Mesh, p_Level->Level[test_pos]->Index_Of_Game_Mesh);

              am_Set_Start_Function(&p_set->animation[p_set->last],
                anmend_Lift_Item, p_LiftI->Index_Of_Game_Mesh,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, p_Level, 1);

              /*am_Set_Start_Function(&p_set->animation[p_set->last], anmend_Lift_Item,
                 p_Level->Level[test_pos]->Index_Of_Game_Mesh, 
                 p_LiftI->Index_Of_Game_Mesh, 
                 p_Level, 1); */

              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                animend_Lift_End,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0, 0);
            }
            else {
              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
            }

            if (test_pos == p_Level->Actual_Item)
              p_Level->Actual_Item = test1_pos;

            if (p_Level->bPosouvatKameru)
              rani_privaz_kameru(p_set->animation[p_set->last].p_run);

            p_Level->Level[test1_pos] = p_Level->Level[test_pos];
            p_Level->Level[test_pos] = 0;
            p_Level->Level[test1_pos]->Pos[0] = new_pos[0];
            p_Level->Level[test1_pos]->Pos[1] = new_pos[1];
            p_Level->Level[test1_pos]->Pos[2] = new_pos[2];
          }
        }
      else
        for (i = LiftTop - 2; i > old_Zpos; i -= 2) {
          gl_Logical2Real(pos[0], pos[1], i, &test_pos, p_Level);

          if (p_Level->Level[test_pos]) {
            VyskaSloupce++;

            new_pos[0] = p_LiftI->Pos[0];
            new_pos[1] = p_LiftI->Pos[1];
            new_pos[2] = i + move[2] * 2;

            gl_Logical2Real(new_pos[0] + move[0], new_pos[1] + move[1],
              i + move[2] * 2, &test1_pos, p_Level);

            if (p_Level->Level[test_pos]->p_Object->Class == 13) {
              kom_svaz_meshe(p_LiftI->Index_Of_Game_Mesh,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                animend_Lift_End,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0, 0);
            }
            else {
              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
            }

            if (test_pos == p_Level->Actual_Item)
              p_Level->Actual_Item = test1_pos;

            if (p_Level->bPosouvatKameru)
              rani_privaz_kameru(p_set->animation[p_set->last].p_run);

            p_Level->Level[test1_pos] = p_Level->Level[test_pos];
            p_Level->Level[test_pos] = 0;
            p_Level->Level[test1_pos]->Pos[0] = new_pos[0];
            p_Level->Level[test1_pos]->Pos[1] = new_pos[1];
            p_Level->Level[test1_pos]->Pos[2] = new_pos[2];
          }
        }

      p_set->last++;

      if (p_LiftI->p_Back_Pack->item[4])
        p_LiftI->p_Back_Pack->item[4] = 0;
      else {
        //vytah byl dole -> vytvorit zdi pod vytahem
        p_LiftI->p_Back_Pack->item[4] = 1;

        for (j = old_Zpos + 1; j < p_LiftI->Pos[2]; j += 2) {
          gl_Logical2Real(new_pos[0], new_pos[1], j, &test_pos, p_Level);
          p_Level->Level[test_pos] = p_Level->pWall;
        }
      }

      if (voda != -1 && SmerDolu) {
        POINTERSTRUCTURE *pS =
          (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

        if (pS) {
          int r;
          int percent;

          gl_Logical2Real(pos[0], pos[1], voda + 1, &r, p_Level);

          pS->p_Level = p_Level;
          pS->iParam = (VyskaSloupce + 1) * 25;
          pS->bParam = 1;
          pS->viParam2[0] = 0;
          pS->viParam2[1] = p_LiftI->Index_Of_Game_Mesh;
          pS->viParam2[2] = 1;

          vpos[0] = pos[0];
          vpos[1] = pos[1];
          vpos[2] = voda + 1;

          memcpy((void *) pS->viParam1, (void *) vpos, 3 * sizeof(int));

          percent =
            ((lpos[2] - (voda + 1)) * 100) / (lpos[2] - p_LiftI->Pos[2]);

          am_Set_Triger_Function(&p_set->animation[liftanim], anmend_Water,
            2, r, pS, percent, 0);
        }
      }

      if (voda != -1 && !SmerDolu) {
        POINTERSTRUCTURE *pS =
          (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));
        int percent;

        percent =
          ((voda - (lpos[2] + VyskaSloupce)) * 100) / (p_LiftI->Pos[2] -
          lpos[2]);

        if (percent < 1)
          percent = 1;

        if (pS) {
          int r;

          gl_Logical2Real(pos[0], pos[1], voda + 1, &r, p_Level);

          pS->p_Level = p_Level;
          pS->iParam = (VyskaSloupce + 1) * 25;
          pS->bParam = -2;
          pS->viParam2[0] = 0;
          pS->viParam2[1] = 0;
          pS->viParam2[2] = 1;

          vpos[0] = pos[0];
          vpos[1] = pos[1];
          vpos[2] = voda + 1;

          memcpy((void *) pS->viParam1, (void *) vpos, 3 * sizeof(int));

          am_Set_Triger_Function(&p_set->animation[liftanim],
            anmend_WaterLift, p_LiftI->Index_Of_Game_Mesh, 0, pS, percent, 0);
        }

      }

      return 1;
    }
    else if (!move[2]) {
      //horizontalni vytah
      DDA dda;
      char bStop = 0;

      dda.x1 = pos[0];
      dda.y1 = pos[1];
      dda.x2 = new_pos[0];
      dda.y2 = new_pos[1];

      dda_First(&dda);
      dda_Next(&dda);

      do {
        gl_Logical2Real(dda.x, dda.y, pos[2] - 1, &test_pos, p_Level);

        /*if((pos[2]-1) >= 0)
           if(p_Level->Level[test_pos])
           {
           bStop = 1;
           break;
           } */

        gl_Logical2Real(dda.x, dda.y, pos[2] + 1, &test_pos, p_Level);
        if (p_Level->Level[test_pos]) {
          bStop = 1;
          break;
        }
      }
      while (dda_Next(&dda));

      if (bStop) {
        // neco je v ceste -> vetah pojede tam a zpet
        dda_Prev(&dda);

        if (dda.main_axe <= 0)
          return 0;

        new_pos[0] = dda.x;
        new_pos[1] = dda.y;
        new_pos[2] = pos[2];
        gl_Get_Move(pos, new_pos, move);

        p_set->animation[p_set->last].p_anim = sim_vyrob_animaci(3, 0, 0);

        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 0, 0.0f,
          0.0f, 0.0f, 0);

        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 1,
          (float) move[0] * 2, 0.0f, (float) move[1] * 2,
          (dda.main_axe * 5) - 1);

        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 2, 0.0f,
          0.0f, 0.0f, (dda.main_axe * 10) - 1);

        sim_interpoluj_animaci(p_set->animation[p_set->last].p_anim,
          dda.main_axe * 10, 0);

        p_set->animation[p_set->last].p_run =
          rani_aktivuj_cekej(p_set->animation[p_set->last].p_anim,
          &p_set->animation[p_set->last].flag);

        p_set->animation[p_set->last].p_matrix =
          rani_privaz_mesh(p_set->animation[p_set->last].p_run,
          p_LiftI->Index_Of_Game_Mesh, 0);

        gl_Add_Mesh(&p_set->animation[p_set->last],
          p_LiftI->Index_Of_Game_Mesh);
        //p_set->animation[p_set->last].mesh = p_LiftI->Index_Of_Game_Mesh;
        p_set->animation[p_set->last].flag = 0;

        am_Set_Start_Function(&p_set->animation[p_set->last], anmend_Lift,
          dda.main_axe * 2, Lift, p_Level, 0);

        am_Set_aMaterial_Trigers(&p_set->animation[p_set->last], p_LiftI,
          p_Level);

        for (i = pos[2] + 1; i < p_Level->Size[2]; i += 2) {
          gl_Logical2Real(pos[0], pos[1], i, &test_pos, p_Level);

          if (!p_Level->Level[test_pos]) {
            p_set->last++;
            return 1;
          }
          else if (p_Level->Level[test_pos]->p_Object->Class == 2) {
            p_set->last++;
            return 1;
          }
          else {

            if (p_Level->Level[test_pos]->p_Object->Class == 13) {
              kom_svaz_meshe(p_LiftI->Index_Of_Game_Mesh,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                animend_Lift_End,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0, 0);
            }
            else {
              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_Level->Level[test_pos]->Index_Of_Game_Mesh;
            }
          }
        }
      }
      else {
        //normalni posun
        p_set->animation[p_set->last].p_anim = sim_vyrob_animaci(2, 0, 0);

        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 0, 0.0f,
          0.0f, 0.0f, 0);

        sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 1,
          (float) (move[0] * 2), 0.0f, (float) (move[1] * 2),
          (dda.main_axe * 5) - 1);

        sim_interpoluj_animaci(p_set->animation[p_set->last].p_anim,
          dda.main_axe * 5, 0);

        p_set->animation[p_set->last].p_run =
          rani_aktivuj_cekej(p_set->animation[p_set->last].p_anim,
          &p_set->animation[p_set->last].flag);

        p_set->animation[p_set->last].p_matrix =
          rani_privaz_mesh(p_set->animation[p_set->last].p_run,
          p_LiftI->Index_Of_Game_Mesh, 0);

        gl_Add_Mesh(&p_set->animation[p_set->last],
          p_LiftI->Index_Of_Game_Mesh);
        //p_set->animation[p_set->last].mesh = p_LiftI->Index_Of_Game_Mesh;

        am_Set_Start_Function(&p_set->animation[p_set->last], anmend_Lift,
          dda.main_axe, real_pos, p_Level, 0);

        // posun vytahu
        p_Level->Level[real_pos] = p_Level->Level[Lift];
        p_Level->Level[Lift] = 0;
        p_LiftI = p_Level->Level[real_pos];

        old_Zpos = p_LiftI->Pos[2];

        p_LiftI->Pos[0] = new_pos[0];
        p_LiftI->Pos[1] = new_pos[1];
        p_LiftI->Pos[2] = new_pos[2];

        //posun veci nad vytahem
        for (i = old_Zpos + 1; i < p_Level->Size[2]; i += 2) {
          gl_Logical2Real(pos[0], pos[1], i, &test_pos, p_Level);

          if (!p_Level->Level[test_pos]) {
            p_set->last++;
            return 1;
          }
          else if (p_Level->Level[test_pos]->p_Object->Class == 2) {
            p_set->last++;
            return 1;
          }
          else {

            new_pos[0] = p_LiftI->Pos[0];
            new_pos[1] = p_LiftI->Pos[1];
            new_pos[2] = i;

            gl_Logical2Real(new_pos[0], new_pos[1], i, &test1_pos, p_Level);

            if (p_Level->Level[test_pos]->p_Object->Class == 13) {
              kom_svaz_meshe(p_LiftI->Index_Of_Game_Mesh,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
              rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
                animend_Lift_End,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0, 0);
            }
            else {
              p_set->animation[p_set->last].p_matrix =
                rani_privaz_mesh(p_set->animation[p_set->last].p_run,
                p_Level->Level[test_pos]->Index_Of_Game_Mesh, 0);

              gl_Add_Mesh(&p_set->animation[p_set->last],
                p_Level->Level[test_pos]->Index_Of_Game_Mesh);
              //p_set->animation[p_set->last].mesh = p_Level->Level[test_pos]->Index_Of_Game_Mesh;
            }

            if (test_pos == p_Level->Actual_Item)
              p_Level->Actual_Item = test1_pos;

            if (p_Level->bPosouvatKameru)
              rani_privaz_kameru(p_set->animation[p_set->last].p_run);


            p_Level->Level[test1_pos] = p_Level->Level[test_pos];
            p_Level->Level[test_pos] = 0;

            p_Level->Level[test1_pos]->Pos[0] = new_pos[0];
            p_Level->Level[test1_pos]->Pos[1] = new_pos[1];
            p_Level->Level[test1_pos]->Pos[2] = new_pos[2];
          }
        }
      }
    }
    else
      return 0;
    //     /\ spatnej vytah
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
// najde prvni stenu v mape
//------------------------------------------------------------------------------------------------
ITEMDESC *gl_Find_First_Wall(LEVELINFO * p_Level)
{
  int i;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if (p_Level->Level[i]->p_Object->Class == 2)
        return p_Level->Level[i];

  return NULL;
}

//------------------------------------------------------------------------------------------------
// prida steny pod vytah
//------------------------------------------------------------------------------------------------
void gl_Add_Walls_Bellow_Lifts(LEVELINFO * p_Level)
{
  int i, j, p1[3], p2[3], p3[3];
  ITEMDESC *p_lift;
  int real;


  p_Level->pWall = gl_Find_First_Wall(p_Level);

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i]) {
      if (p_Level->Level[i]->p_Object->Class == 11) {
        p_lift = p_Level->Level[i];

        gl_Real2Logical(p_lift->Connection[0], p1, p_Level);
        gl_Real2Logical(p_lift->Connection[1], p2, p_Level);
        gl_Real2Logical(i, p3, p_Level);

        if (((p1[2] > p2[2]) && (p3[2] == p1[2])) ||
          ((p2[2] > p1[2]) && (p3[2] == p2[2]))) {
          int p;

          if (p1[2] > p2[2]) {
            p = p1[2];
            p1[2] = p2[2];
            p2[2] = p;

          }

          //for(j=p1[2]+1;j<p2[2];j+=2)
          for (j = 1; j < p2[2]; j += 2) {
            gl_Logical2Real(p1[0], p1[1], j, &real, p_Level);
            p_Level->Level[real] = p_Level->pWall;
          }

          p_lift->p_Back_Pack->item[4] = 1;
        }
        else if (p1[2] != p2[2]) {
          int p;

          if (p1[2] < p2[2]) {
            p = p1[2];
            p1[2] = p2[2];
            p2[2] = p;
          }

          for (j = 1; j < p2[2]; j += 2) {
            gl_Logical2Real(p1[0], p1[1], j, &real, p_Level);
            p_Level->Level[real] = p_Level->pWall;
          }

          p_lift->p_Back_Pack->item[4] = 0;
        }
        else
          p_lift->p_Back_Pack->item[4] = 0;

        //nainicializuj vytah
        gl_Real2Logical(i, p3, p_Level);
        p3[2]++;
        gl_Logical2Real(p3[0], p3[1], p3[2], &real, p_Level);

        p_lift->p_Back_Pack->item[2] = (int) p_Level->Level[real];
      }
    }
}

void gl_Add_Walls_Bellow_LiftsH(LEVELINFO * p_Level)
{
  int i, p1[3], p2[3];
  ITEMDESC *p_lift;
  int real;
  DDA dda;

  p_Level->pWall = gl_Find_First_Wall(p_Level);

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if (p_Level->Level[i]->p_Object->Class == 11) {
        p_lift = p_Level->Level[i];

        gl_Real2Logical(p_lift->Connection[0], p1, p_Level);
        gl_Real2Logical(p_lift->Connection[1], p2, p_Level);

        if (p1[2] == p2[2]) {

          dda.x1 = p1[0];
          dda.y1 = p1[1];
          dda.x2 = p2[0];
          dda.y2 = p2[1];

          dda_First(&dda);

          do {
            gl_Logical2Real(dda.x, dda.y, p1[2] - 1, &real, p_Level);

            if ((p1[2] - 1) >= 0) {
              if (!p_Level->Level[real])
                p_Level->Level[real] = p_Level->pWall;
            }
            else
              break;
          }
          while (dda_Next(&dda));
        }

      }
}

void gl_Open_Door(ITEMDESC * p_DoorI, LEVELINFO * p_Level)
{
  float pos[3];
  int i, r = rand() % 2;

  lani_set(p_DoorI->Index_Of_Game_Mesh, 0, 0, &p_Level->TrashFlag, 0, 0, 10);

  p_DoorI->p_Back_Pack->item[1] = 1;
  p_DoorI->p_Back_Pack->item[0]++;

  kom_mesh_get_float(p_DoorI->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2],
    &i);
/*
	if(p_DoorI->p_Object->Specific[r].Index != -1)
		ap_Play_Sound(0,0,0, pos, p_DoorI->p_Object->Specific[r].Index, NULL, &ad);
	else
		if(p_DoorI->p_Object->Specific[0].Index != -1)
			ap_Play_Sound(0,0,0, pos, p_DoorI->p_Object->Specific[0].Index, NULL, &ad);
*/
  r = rand() % 3;
/*
	if(p_DoorI->p_Object->Reserved[r].Index != -1)
		ap_Play_Sound(0,0,0, pos, p_DoorI->p_Object->Reserved[r].Index, NULL, &ad);
	else
		if(p_DoorI->p_Object->Reserved[0].Index != -1)
			ap_Play_Sound(0,0,0, pos, p_DoorI->p_Object->Reserved[0].Index, NULL, &ad);
*/
}

//------------------------------------------------------------------------------------------------
// logika dveri
//------------------------------------------------------------------------------------------------
int gl_Gate_Keeper(int Door, int *Item, LEVELINFO * p_Level)
{
  int Real_Item;
  ITEMDESC *p_DoorI = p_Level->Level[Door];
  OBJECTDESC *p_Real_ItemO;

  if (!p_DoorI)
    return 0;

  gl_Logical2Real(Item[0], Item[1], Item[2], &Real_Item, p_Level);
  p_Real_ItemO = p_Level->Level[Real_Item]->p_Object;

  // jestlize jsou dvere otevrene, tak muze projit
  if (p_DoorI->p_Back_Pack->item[1])
    return 1;
  else {
    //na jeden pruchod a zavrene -> nelze prijit
    if ((p_DoorI->p_Object->SubClass == 22) &&
      (p_DoorI->p_Back_Pack->item[0]))
      return 0;
  }

  if (!p_DoorI->p_Object->SubClass) {
    gl_Open_Door(p_DoorI, p_Level);
    p_DoorI->p_Back_Pack->item[0]++;
    p_DoorI->p_Back_Pack->item[1] = 1;
    return 1;
  }

  if ((p_DoorI->p_Object->SubClass > 0) && (p_DoorI->p_Object->SubClass < 7)) {
    if (
      (p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->item[p_DoorI->
          p_Object->SubClass + 9]) || (((p_Real_ItemO->Class == 1)
          && (p_Real_ItemO->SubClass == p_DoorI->p_Object->SubClass - 1)
        )
        || ((p_DoorI->p_Object->SubClass == 6) && (p_Real_ItemO->SubClass > 4)
        )
      )
      ) {
      gl_Open_Door(p_DoorI, p_Level);
      p_DoorI->p_Back_Pack->item[0]++;
      p_DoorI->p_Back_Pack->item[1] = 1;
      return 1;
    }
    return 0;
  }

  if ((p_DoorI->p_Object->SubClass > 6) && (p_DoorI->p_Object->SubClass < 13)) {
    if (
      (p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->item[p_DoorI->
          p_Object->SubClass + 3]) || (((p_Real_ItemO->Class == 1)
          && (p_Real_ItemO->SubClass == p_DoorI->p_Object->SubClass - 7)
        )
        ||
        ((p_DoorI->p_Object->SubClass == 12) && (p_Real_ItemO->SubClass > 4)
        )
      )
      ) {
      gl_Open_Door(p_DoorI, p_Level);
      p_DoorI->p_Back_Pack->item[0]++;
      p_DoorI->p_Back_Pack->item[1] = 1;
      return 1;
    }
    return 0;
  }

  if ((p_DoorI->p_Object->SubClass > 12) &&
    (p_DoorI->p_Object->SubClass < 19)) {
    if (p_Real_ItemO->Class != 1)
      return 0;

    if (
      (p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->item[p_Level->
          Level[Door]->p_Object->SubClass - 3])
      || (((p_Real_ItemO->Class == 1)
          && (p_Real_ItemO->SubClass == p_DoorI->p_Object->SubClass - 13)
        )
        ||
        ((p_DoorI->p_Object->SubClass == 18) && (p_Real_ItemO->SubClass > 4)
        )
      )
      ) {
      gl_Open_Door(p_DoorI, p_Level);
      p_DoorI->p_Back_Pack->item[0]++;
      p_DoorI->p_Back_Pack->item[1] = 1;
      return 1;
    }
    return 0;
  }

  if ((p_DoorI->p_Object->SubClass == 19) ||
    (p_DoorI->p_Object->SubClass == 20)) {
    int move[3];

    gl_Get_Move(Item, p_DoorI->Pos, move);

    if (!move[0]) {
      if ((move[1] > 0) && (p_DoorI->Rotation != 2))
        return 0;
      if ((move[1] < 0) && (p_DoorI->Rotation))
        return 0;
    }
    else {
      if ((move[0] > 0) && (p_DoorI->Rotation != 3))
        return 0;
      if ((move[0] < 0) && (p_DoorI->Rotation != 1))
        return 0;
    }

    gl_Open_Door(p_DoorI, p_Level);
    p_DoorI->p_Back_Pack->item[0]++;
    p_DoorI->p_Back_Pack->item[1] = 1;
    return 1;
  }

  if (p_DoorI->p_Object->SubClass == 21) {
    int move[3];

    gl_Get_Move(Item, p_DoorI->Pos, move);

    if (move[0] && ((!p_DoorI->Rotation) || (p_DoorI->Rotation == 2)
      )
      )
      return 0;
    else if ((p_DoorI->Rotation == 1) || (p_DoorI->Rotation == 3))
      return 0;

    gl_Open_Door(p_DoorI, p_Level);
    p_DoorI->p_Back_Pack->item[0]++;
    p_DoorI->p_Back_Pack->item[1] = 1;
    return 1;
  }

  if (p_DoorI->p_Object->SubClass == 22) {
    if (p_DoorI->p_Back_Pack->item[0])
      return 0;

    gl_Open_Door(p_DoorI, p_Level);
    p_DoorI->p_Back_Pack->item[0]++;
    p_DoorI->p_Back_Pack->item[1] = 1;
    return 1;
  }

  return 1;
}

int gl_Find_Top_With_Water(int *pWater, ITEMDESC * pItem, LEVELINFO * p_Level)
{
  int i, iPos[3], r;

  (*pWater) = -1;

  if (!pItem)
    return -1;

  memcpy((void *) iPos, (void *) pItem->Pos, 3 * sizeof(int));

  for (i = pItem->Pos[2] + 2; i < p_Level->Size[2]; i += 2) {
    gl_Logical2Real(iPos[0], iPos[1], i, &r, p_Level);

    if (p_Level->Level[r])
      return i;

    gl_Logical2Real(iPos[0], iPos[1], i - 1, &r, p_Level);

    if (p_Level->Level[r])
      if (p_Level->Level[r]->p_Object->Class != 12)
        return i - 2;
      else {
        (*pWater) = i - 1;

        if (pItem->p_Object->Class == 5 && pItem->p_Object->SubClass == 3)
          return i;
        else if ((i + 2) < p_Level->Size[2]) {
          gl_Logical2Real(iPos[0], iPos[1], i + 1, &r, p_Level);

          if (p_Level->Level[r])
            return i;

          gl_Logical2Real(iPos[0], iPos[1], i + 2, &r, p_Level);

          if (p_Level->Level[r])
            return i;

          return i + 2;
        }
        else
          return i;
      }
  }

  return p_Level->Size[2] - 1;
}

void gl_Vyrob_Animaci(int move, int newL, int Water, int bWater,
  ITEMDESC * pItem, LEVELINFO * p_Level)
{
  POINTERSTRUCTURE *pS;
  int real, new_real;

  p_set->animation[p_set->last].p_anim = sim_vyrob_animaci(2, 0, 0);

  sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 0, 0.0f, 0.0f,
    0.0f, 0);

  sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 1, 0.0f,
    (float) move, 0.0f, (move * 2) - 1);

  sim_interpoluj_animaci(p_set->animation[p_set->last].p_anim, move * 2, 0);

  p_set->animation[p_set->last].p_run =
    rani_aktivuj_cekej(p_set->animation[p_set->last].p_anim,
    &p_set->animation[p_set->last].flag);

  p_set->animation[p_set->last].p_matrix =
    rani_privaz_mesh(p_set->animation[p_set->last].p_run,
    pItem->Index_Of_Game_Mesh, 0);

  gl_Add_Mesh(&p_set->animation[p_set->last], pItem->Index_Of_Game_Mesh);
  //p_set->animation[p_set->last].mesh = pItem->Index_Of_Game_Mesh;

  p_set->last++;

  gl_Logical2Real(pItem->Pos[0], pItem->Pos[1], pItem->Pos[2], &real,
    p_Level);
  gl_Logical2Real(pItem->Pos[0], pItem->Pos[1], newL, &new_real, p_Level);

  p_Level->Level[new_real] = p_Level->Level[real];
  p_Level->Level[real] = 0;

  if (Water != -1 && bWater) {
    pS = (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

    if (pS) {
      int percent;

      gl_Logical2Real(p_Level->Level[new_real]->Pos[0],
        p_Level->Level[new_real]->Pos[1], Water, &real, p_Level);

      pS->p_Level = p_Level;
      pS->iParam = 50;
      pS->bParam = 1;
      pS->viParam2[0] = 0;
      pS->viParam2[1] = 0;
      pS->viParam2[2] = 1;

      pS->viParam1[0] = p_Level->Level[new_real]->Pos[0];
      pS->viParam1[1] = p_Level->Level[new_real]->Pos[1];
      pS->viParam1[2] = Water;

      percent =
        ((Water - p_Level->Level[new_real]->Pos[2]) * 100) / (newL -
        p_Level->Level[new_real]->Pos[2]);

      am_Set_Triger_Function(&p_set->animation[p_set->last - 1], anmend_Water,
        2, real, pS, percent, 0);
    }
  }

  p_Level->Level[new_real]->Pos[2] = newL;
}

int gl_Check_Anim_Pontonky(ITEMDESC * pItem, LEVELINFO * p_Level)
{
  int real;
  int iPos[3];

  if (!pItem)
    return 0;

  iPos[0] = pItem->Pos[0];
  iPos[1] = pItem->Pos[1];
  iPos[2] = pItem->Pos[2];

  iPos[2]--;

  //musi pod ni byt voda
  gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

  if (p_Level->Level[real]) {
    if (p_Level->Level[real]->p_Object->Class != 12)
      return 0;
  }
  else
    return 0;

  //nesmi pod ni byt bedna
  iPos[2]--;

  if (iPos >= 0) {
    gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

    if (p_Level->Level[real])
      return 0;
  }

  iPos[2] += 3;

  if (iPos[2] < p_Level->Size[2]) {
    gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

    if (p_Level->Level[real])
      return 0;
  }

  iPos[2]++;

  if (iPos[2] < p_Level->Size[2]) {
    gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

    if (p_Level->Level[real])
      return 0;
  }

  return 1;
}

int gl_Vynor_Pontonky(LEVELINFO * p_Level)
{
  ITEMDESC *pItem;
  int c, i, r, v, Water, bWater;
  int ret = 0;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i]) {
      pItem = p_Level->Level[i];

      if (pItem->p_Object->Class == 5 &&
        pItem->p_Object->SubClass == 3 && p_Level->Square[i].bUnderWater) {
        c = gl_Count_Weight(pItem->Pos, p_Level);

        //pokud pontonka nese moc malou vahu, tak se vynori
        if (c < 3) {
          bWater = 1;

          gl_Logical2Real(pItem->Pos[0], pItem->Pos[1], pItem->Pos[2] + 2, &r,
            p_Level);

          if (pItem->Pos[2] + 2 < p_Level->Size[2])
            if (p_Level->Level[r]) {
              v = gl_Find_Top_With_Water(&Water, p_Level->Level[r], p_Level);

              if (v != p_Level->Level[r]->Pos[2]) {
                gl_Vyrob_Animaci(v - p_Level->Level[r]->Pos[2], v, Water,
                  bWater, p_Level->Level[r], p_Level);

                bWater = 0;
                ret = 1;
              }
            }

          v = gl_Find_Top_With_Water(&Water, pItem, p_Level);

          if (v != pItem->Pos[2]) {
            gl_Vyrob_Animaci(v - pItem->Pos[2], v, Water, bWater, pItem,
              p_Level);
            ret = 1;
          }
        }
      }
      else if (pItem->p_Object->Class == 5 && pItem->p_Object->SubClass == 3) {
        c = gl_Check_Anim_Pontonky(pItem, p_Level);

        if (!c && pItem->a_run != -1) {
          //rani_rozvaz(pItem->a_run,pItem->Index_Of_Game_Mesh);
          rani_zrus(pItem->a_run);
          pItem->a_run = -1;
          kom_umisti_prvek(pItem->Index_Of_Game_Mesh, pItem->Pos[0],
            pItem->Pos[2], pItem->Pos[1], pItem->Rotation);
        }
        else if (c && pItem->a_run == -1)
          am_Start_Animaci_Pontonky(pItem);
      }
    }

  return ret;
}

void gl_Close_Door(ITEMDESC * pDoor, LEVELINFO * p_Level)
{
  float pos[3];
  int r;

  pDoor->p_Back_Pack->item[1] = 0;
  lani_set(pDoor->Index_Of_Game_Mesh, 0, 0, &p_Level->TrashFlag, 0, 10, 21);

  kom_mesh_get_float(pDoor->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2],
    &r);
/*
	if(pDoor->p_Object->Specific[2].Index != -1)
		ap_Play_Sound(0,0,0, pos, pDoor->p_Object->Specific[2].Index, NULL, &ad);
*/
}

//------------------------------------------------------------------------------------------------
// zkontroluje logicke zavislosti: teleporty, propadla, tlacitka,...
//------------------------------------------------------------------------------------------------
int gl_Check_Logic_Dependences(LEVELINFO * p_Level)
{
  int x, y, z, i, m;

  //int real_pos, test_pos, ret, real_p;
  int test_pos, ret, real_p;
  int pos[3];
  int ret_value = 0;
  int r;
  ITEMDESC *p_Action_ItemI;

  // nuluju flagy vytahu
  for (m = 0; m < p_Level->Count_Of_Action_Items; m++) {
    if (p_Level->Action_Item[m] == -1)
      break;
    p_Action_ItemI = p_Level->Level[p_Level->Action_Item[m]];

    if (p_Action_ItemI)
      if (p_Action_ItemI->p_Object->Class == 11)
        p_Action_ItemI->p_Back_Pack->item[3] = 0;
  }

  for (m = 0; m < p_Level->Count_Of_Items; m++) {
    if (p_Level->Action_Item[m] == -1) {
      ret_value += gl_Vynor_Pontonky(p_Level);

      return ret_value;
    }

    p_Action_ItemI = p_Level->Level[p_Level->Action_Item[m]];

    if (p_Action_ItemI && !p_Action_ItemI->bDestroed) {
      x = p_Action_ItemI->Pos[0];
      y = p_Action_ItemI->Pos[1];
      z = p_Action_ItemI->Pos[2];

      // dvere
      if (p_Action_ItemI->p_Object->Class == 8) {
        if (((p_Action_ItemI->p_Object->SubClass > 6) &&
            (p_Action_ItemI->p_Object->SubClass < 13)) ||
          (p_Action_ItemI->p_Object->SubClass > 18)) {
          //zavri dvere
          gl_Logical2Real(x, y, z + 1, &test_pos, p_Level);

          if (!p_Level->Level[test_pos]
            && p_Action_ItemI->p_Back_Pack->item[1]) {
            gl_Close_Door(p_Action_ItemI, p_Level);
            //ret_value = 1;
          }
        }
      }

      //tlacitko
      if (p_Action_ItemI->p_Object->Class == 10) {
        gl_Logical2Real(x, y, z + 1, &test_pos, p_Level);

        if (p_Level->Level[test_pos])
          ret_value += gl_Do_Button(p_Level->Action_Item[m], 1, p_Level);
        else
          ret_value += gl_Do_Button(p_Level->Action_Item[m], 0, p_Level);
      }

      // teleport
      if ((p_Action_ItemI->p_Object->Class == 9) &&
        (!p_Action_ItemI->p_Object->SubClass)) {
        gl_Logical2Real(x, y, z + 1, &test_pos, p_Level);

        if (p_Level->Level[test_pos]) {
          ret = gl_Teleport_Item(p_Level->Action_Item[m], test_pos, p_Level);

          if (ret != -1)
            ret_value = 1;
        }
      }

      //exit
      if (p_Action_ItemI->p_Object->Class == 4) {
        gl_Logical2Real(x, y, z + 1, &real_p, p_Level);

        if (p_Level->Level[real_p])
          if (p_Level->Level[real_p]->p_Object->Class == 1) {
            p_Level->bExit = 1;
            ret_value = 1;
          }
      }

      // vytah
      if ((p_Action_ItemI->p_Object->Class == 11) &&
        (!p_Action_ItemI->p_Back_Pack->item[3])) {
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;

        if (p_Action_ItemI->p_Object->SubClass != 3)
          ret_value += gl_Do_Lift(p_Level->Action_Item[m], pos, p_Level);
      }

      //propadlo
      if (p_Action_ItemI)
        if (p_Action_ItemI->p_Object->Class == 15) {
          gl_Logical2Real(x, y, z + 1, &real_p, p_Level);

          if (p_Level->Level[real_p]) {
            pos[0] = x;
            pos[1] = y;
            pos[2] = z + 1;

            // zjistim vahu sloupce na propadlu
            ret = gl_Count_Weight(pos, p_Level);

            if (ret > 1) {
              // musim otestovat, zda to neodporuje jinym pravidlum
              ret = 0;
              for (i = z; i > 0; i -= 2) {
                gl_Logical2Real(x, y, i, &test_pos, p_Level);

                if (p_Level->Level[test_pos])
                  if ((p_Level->Level[test_pos]->p_Object->Class == 1) ||
                    (p_Level->Level[test_pos]->p_Object->Class == 6))
                    ret--;
              }

              if (ret >= 0) {
                float fpos[3];
                int rot, mat;

                mat =
                  gl_Get_Mesh_Material(p_Level->Level[p_Level->
                    Action_Item[m]]->Index_Of_Game_Mesh);
                kom_mesh_get_float(p_Level->Level[p_Level->Action_Item[m]]->
                  Index_Of_Game_Mesh, &fpos[0], &fpos[1], &fpos[2], &rot);
                pos[0] = x;
                pos[1] = y;
                pos[2] = z;

                r = rand() % 3;

                if (p_Action_ItemI->p_Object->Specific[rot].Index == -1)
                  rot = 0;
/*
									if(p_Action_ItemI->p_Object->Specific[rot].Index != -1)
										ap_Play_Sound(0,0,0, fpos, p_Action_ItemI->p_Object->Specific[rot].Index, NULL, &ad);								
*/
                gl_Destroy_Item(p_Level->Action_Item[m], 0, p_Level);

                gl_Do_Propadlo(fpos, p_Level, mat);

                gl_Throw_off(pos, p_Level);

                m = 0;
                ret_value = 1;
              }
            }
          }
        }
    }
  }

  ret_value += gl_Vynor_Pontonky(p_Level);

  return ret_value;
}

int gl_Can_Go_Into_Water(ITEMDESC * pItem)
{
  if (pItem->p_Object->Class != 1)
    return 1;
  else if (pItem->p_Object->SubClass < 6 && pItem->p_Back_Pack->item[0])
    return 1;
  else
    return 0;
}

int gl_Is_There_Beatle(int *iPos, ITEMDESC ** pItem, LEVELINFO * p_Level)
{
  int i, c = 0;
  int real;

  for (i = iPos[2]; i < p_Level->Size[2]; i += 2) {
    gl_Logical2Real(iPos[0], iPos[1], i, &real, p_Level);

    if (p_Level->Level[real]) {
      if (p_Level->Level[real]->p_Object->Class == 1) {
        *pItem = p_Level->Level[real];
        return c;
      }

      if (p_Level->Level[real]->p_Object->Class == 2 ||
        p_Level->Level[real]->p_Object->Class == 4 ||
        p_Level->Level[real]->p_Object->Class == 17 ||
        p_Level->Level[real]->p_Object->Class == 19) {
        *pItem = NULL;
        return c;
      }
    }
    else {
      *pItem = NULL;
      return c;
    }

    c++;
  }

  *pItem = NULL;
  return c;
}

int gl_Will_Go_Under_Water(int *iPos, int DontCountPos, int Plus, int *pVaha,
  LEVELINFO * p_Level)
{
  int i, iPocetBeden = Plus, cPos[3];
  char bUnderWater = 0;
  int real;

  if (pVaha)
    (*pVaha) = 0;

  for (i = iPos[2]; i > 0; i -= 2) {
    gl_Logical2Real(iPos[0], iPos[1], i, &real, p_Level);

    // mrknu se co je v prvni vrstve
    if (p_Level->Level[real]) {
      //brouka nepocitat
      if (p_Level->Level[real]->p_Object->Class != 1) {
        // jestlize je to zed a bedny sou ok, tak je to OK
        if (p_Level->Level[real]->p_Object->Class == 2 && iPocetBeden >= 0)
          return 0;

        //jestlize je to pontonka, tak zvaz vahu na ni (informativne)
        if (p_Level->Level[real]->p_Object->Class == 5 &&
          p_Level->Level[real]->p_Object->SubClass == 3 && pVaha) {
          cPos[0] = iPos[0];
          cPos[1] = iPos[1];
          cPos[2] = i;

          (*pVaha) = gl_Count_Weight(cPos, p_Level);
        }

        // jestlize nejsu pod vodou tak pripoctu bednu 
        //(Pokud to je prvni a nema se pocitat, tak ji nepocitam)
        if (DontCountPos != i && !bUnderWater)
          iPocetBeden++;

        // jestlize se jedna o pozici k nezapocteni a jsem pod vodou, tak
        // odecist, protoze ta bedna tam bude chybet
        if (DontCountPos == i && bUnderWater)
          iPocetBeden--;

        // jsem pod vodou, narazil jsem na neco a pocet beden je dostatecny
        // a dostal jsem se k bodu, kde sem vyhodil bednu
        if (bUnderWater && iPocetBeden >= 0 && i <= DontCountPos)
          return 0;
      }
    }
    else                        // jestlize jsu pod vodou a nic tam neni, odectu si jednu bednu
    if (bUnderWater)
      iPocetBeden--;

    // pokud mi dosli bedny, tak beruska pujde pod vodu
    if (iPocetBeden < 0)
      return 1;

    gl_Logical2Real(iPos[0], iPos[1], i - 1, &real, p_Level);

    //mrknu se na nultou vrstvu
    // - jestlize to neni voda a je to nad vodu tak ok
    // - jestlize je to pod vodou a neno to voda :) a pocet beden je dostatecny, tak OK
    if (p_Level->Level[real])
      if (p_Level->Level[real]->p_Object->Class != 12 && !bUnderWater)
        return 0;
      else
        if (p_Level->Level[real]->p_Object->Class != 12 && bUnderWater
        && iPocetBeden >= 0)
        return 1;
      else
        bUnderWater = 1;
  }

  //dojel jsem na dno levelu a pocet beden je ok
  if (iPocetBeden < 0)
    return 1;
  else
    return 0;
}

int gl_Check_Detonation_Pack_Water_Rules(int *iPos, LEVELINFO * p_Level)
{
  ITEMDESC *pItem = NULL;
  int pos[3], c;

  memcpy((void *) pos, (void *) iPos, 3 * sizeof(int));

  //pos[2] += 2;

  c = gl_Is_There_Beatle(pos, &pItem, p_Level);

  if (!pItem)
    return 1;

  //nastav pozici pod berusku
  pos[2] += c * 2;

  if (gl_Will_Go_Under_Water(pos, iPos[2], 0, NULL, p_Level)
    && !gl_Can_Go_Into_Water(pItem))
    return 0;

  return 1;
}

//------------------------------------------------------------------------------------------------
// use of detonation pack
//------------------------------------------------------------------------------------------------
void gl_Throw_Detonation_Pack(long item, LEVELINFO * p_Level)
{
  int pos[3], rot;
  float fpos[3];
  int real_pos;
  OBJECTDESC *p_real_posO;
  ITEMDESC *p_itemI = p_Level->Level[item];

  pos[0] = p_itemI->Pos[0];
  pos[1] = p_itemI->Pos[1];
  pos[2] = p_itemI->Pos[2];
  pos[2] -= 2;

  if (!p_itemI->Rotation)
    pos[1]++;
  if (p_itemI->Rotation == 1)
    pos[0]++;
  if (p_itemI->Rotation == 2)
    pos[1]--;
  if (p_itemI->Rotation == 3)
    pos[0]--;

  if ((pos[0] < 0) || (pos[0] > (p_Level->Size[0] - 1)))
    return;
  if ((pos[1] < 0) || (pos[1] > (p_Level->Size[1] - 1)))
    return;
  if (pos[2] < 1)
    return;

  gl_Logical2Real(pos[0], pos[1], pos[2], &real_pos, p_Level);

  if (!p_Level->Level[real_pos])
    return;

  p_real_posO = p_Level->Level[real_pos]->p_Object;


  if (!gl_Check_Detonation_Pack_Water_Rules(pos, p_Level))
    return;

  if (p_real_posO->Class == 7 ||
    p_real_posO->Class == 3 ||
    p_real_posO->Class == 16 ||
    p_real_posO->Class == 5 || p_real_posO->Class == 6) {
    kom_mesh_get_float(p_Level->Level[real_pos]->Index_Of_Game_Mesh,
      &fpos[0], &fpos[1], &fpos[2], &rot);

    gl_Destroy_Item(real_pos, 4, p_Level);
    p_Level->Level[real_pos] = 0;
  }
  else
    return;

  p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->item[4]--;
  gl_Throw_off(pos, p_Level);
  p_Level->Flip++;
  p_Level->status = 1;
  p_Level->bAllow_Key = 0;

  gl_Next_Queue_Set(p_Level);

  if (p_Level->Flip == 1)
    gl_Go_Animations();

  rot = rand() % 5;

  //ap_Play_Sound(0,0,0, fpos, rot + 14, NULL, &ad);
}


//------------------------------------------------------------------------------------------------
// kontrola pri odkladani predmetu
//------------------------------------------------------------------------------------------------
int gl_Is_Drop_Item_Possible(int *p_Pos, LEVELINFO * p_Level)
{
  int real_pos;
  int i;
  OBJECTDESC *p_real_posO;

  for (i = p_Pos[2]; i >= 0; i--) {
    gl_Logical2Real(p_Pos[0], p_Pos[1], i, &real_pos, p_Level);

    if (p_Level->Level[real_pos]) {
      p_real_posO = p_Level->Level[real_pos]->p_Object;
      if (p_real_posO->Class == 1 ||
        p_real_posO->Class == 13 || p_real_posO->Class == 19)
        return 0;
      else if (p_real_posO->Class != 12)
        return 1;
    }
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
// najde dno, na urcene pozici od urcene vysky
//------------------------------------------------------------------------------------------------
int gl_Find_Bottom(int *p_Pos, int *bVoda, LEVELINFO * p_Level)
{
  int real_pos;
  int i, bottom = p_Pos[2];

  *bVoda = -1;

  for (i = p_Pos[2]; i >= 0; i -= 2) {
    gl_Logical2Real(p_Pos[0], p_Pos[1], i, &real_pos, p_Level);

    if (p_Level->Level[real_pos])
      return bottom;

    gl_Logical2Real(p_Pos[0], p_Pos[1], i - 1, &real_pos, p_Level);

    if (p_Level->Level[real_pos])
      if (p_Level->Level[real_pos]->p_Object->Class != 12) {
        if (p_Level->Level[real_pos]->p_Object->Class != 8)
          return bottom - 2;
        else
          return bottom;
      }
      else
        *bVoda = bottom - 2;

    bottom -= 2;
  }

  return -1;
}

char gl_Check_Mesh_Sim(int iMesh)
{
  int i;

  for (i = 0; i < p_set_anim->last; i++)
    if (p_set_anim->animation[i].p_run != -1)
      if (rani_je_mesh(p_set_anim->animation[i].p_run, iMesh))
        return 1;

  for (i = 0; i < p_set->last; i++)
    if (p_set->animation[i].p_run != -1)
      if (rani_je_mesh(p_set->animation[i].p_run, iMesh))
        return 1;

  return 0;
}

//------------------------------------------------------------------------------------------------
// zpusti animace jednoho kroku
//------------------------------------------------------------------------------------------------
void gl_Go_Animations(void)
{
  int i, j;
  SIM_ANIMATION *p_anim;

  if (p_set_anim->bSetMesh) {
    kom_mesh_set_mesh(p_set_anim->iMesh, p_set_anim->iMeshID,
      p_set_anim->iRot);
    //kprintf(1, "Nastavuji mesh na rotaci %d a spoustim anim!", p_set_anim->iRot);
  }

  for (i = 0; i < 32; i++) {
    if (p_set_anim->animation[i].p_run != -1) {
      p_anim = &p_set_anim->animation[i];

      for (j = 0; j < 8; j++)
        if (Level.Actual_Item != -1)
          if ((p_anim->mesh[j] ==
              Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh)
            && Level.bPosouvatKameru)
            rani_privaz_kameru(p_anim->p_run);
    }
    else if (i < p_set_anim->last)
      p_set_anim->animation[i].flag = 101;


    if (p_set_anim->animation[i].p_run != -1
      && !p_set_anim->animation[i].bnext) {
      p_anim = &p_set_anim->animation[i];

      if (!p_set_anim->animation[i].bconnected)
        rani_go(p_anim->p_run, 0, 0, 0);

      /*for(j=0;j<8;j++)
         if((p_anim->mesh[i] == Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh) &&
         Level.bPosouvatKameru)
         {
         rani_privaz_kameru(p_anim->p_run);   
         break;
         } */

/*			else
				kprintf(1,"nespoustim animaci %d, protoze je privazana\n", p_anim->p_run);*/

      for (j = 0; j < 2; j++)
        if (p_anim->tStart[j].pProc) {
          p_anim->tStart[j].pProc(p_anim->tStart[j].iParam[0],
            p_anim->tStart[j].iParam[1], p_anim->tStart[j].pParam);

          p_anim->tStart[j].pProc = NULL;
        }

      for (j = 0; j < 3; j++)
        if (p_anim->taMateral[j].pProc) {
          p_anim->taMateral[j].pProc(p_anim->taMateral[j].iParam[0],
            p_anim->taMateral[j].iParam[1], p_anim->taMateral[j].pParam);

          p_anim->taMateral[j].pProc = NULL;
        }
    }
  }

  //privaz meshe
  for (i = 0; i < p_set_anim->pMesh; i++)
    kom_svaz_meshe(p_set_anim->PrivazanyMesh[i].TopMesh,
      p_set_anim->PrivazanyMesh[i].LowMesh);

  for (i = 0; i < 6; i++)
    if (p_set_anim->_3dsAnim[i].mesh != -1)
      lani_set(p_set_anim->_3dsAnim[i].mesh,
        i,
        p_set_anim->_3dsAnim[i].c_anim,
        p_set_anim->_3dsAnim[i].p_flag,
        p_set_anim->_3dsAnim[i].flag,
        p_set_anim->_3dsAnim[i].start, p_set_anim->_3dsAnim[i].stop);

}

void gl_Rozvaz_Meshe(void)
{
  int i;

  for (i = 0; i < p_set_anim->pMesh; i++)
    kom_rozvaz_mesh(p_set_anim->PrivazanyMesh[i].LowMesh);
}

//------------------------------------------------------------------------------------------------
// pokud nasatala udalost, spusti jeji funkci
//------------------------------------------------------------------------------------------------
void gl_Do_Triger(SIM_ANIMATION * p_animation)
{
  POINTERSTRUCTURE *pStruct;
  int j;

  for (j = 0; j < 5; j++)
    if (p_animation->tTriger[j].pProc)
      if (p_animation->flag >= p_animation->tTriger[j].flag ||
        p_animation->flag == -1) {
        pStruct = (POINTERSTRUCTURE *) p_animation->tTriger[j].pParam;

        p_animation->tTriger[j].pProc(p_animation->tTriger[j].iParam[0],
          p_animation->tTriger[j].iParam[1], p_animation->tTriger[j].pParam);

        p_animation->tTriger[j].pProc = NULL;
      }

  for (j = 0; j < 3; j++)
    if (p_animation->taMateral[j].pProc
      && p_animation->taMateral[j].flag > -1)
      if (p_animation->flag >= p_animation->taMateral[j].flag
        || p_animation->flag == -1) {
        p_animation->taMateral[j].pProc(p_animation->taMateral[j].iParam[0],
          p_animation->taMateral[j].iParam[1],
          p_animation->taMateral[j].pParam);

        p_animation->taMateral[j].pProc = NULL;
      }
}

void gl_Add_Mesh(SIM_ANIMATION * p_animation, int mesh)
{
  int i;

  for (i = 0; i < 8; i++)
    if (p_animation->mesh[i] == -1) {
      p_animation->mesh[i] = mesh;
      return;
    }
}

int gl_Is_Included_In(SIM_ANIMATION * p_animation, int mesh)
{
  int i;

  for (i = 0; i < 8; i++)
    if (p_animation->mesh[i] == mesh)
      return 1;

  return 0;
}

SIM_ANIMATION *gl_Is_Included_In_AnimSet(ANIMATION_QUEUE_SET * p_aset,
  int mesh)
{
  int j;

  for (j = 0; j < p_aset->last; j++)
    if (gl_Is_Included_In(&p_aset->animation[j], mesh))
      return &p_aset->animation[j];

  return NULL;
}

int gl_Include_Coincidence(SIM_ANIMATION * p_anim1, SIM_ANIMATION * p_anim2)
{
  int i;

  for (i = 0; i < 8; i++)
    if (gl_Is_Included_In(p_anim2, p_anim1->mesh[i])
      && p_anim1->mesh[i] != -1)
      return 1;

  return 0;
}

//------------------------------------------------------------------------------------------------
// vynuluje seznam animaci
//------------------------------------------------------------------------------------------------
void gl_Zero_Queue(void)
{
  int i, j;
  SIM_ANIMATION *p_animation;

  for (i = 0; i < 32; i++) {
    p_animation = &p_set_anim->animation[i];

    p_animation->flag = 0;

    for (j = 0; j < 8; j++)
      p_animation->mesh[j] = -1;

    p_animation->p_anim = -1;
    p_animation->p_matrix = -1;
    p_animation->p_run = -1;
    p_animation->bnext = 0;
    p_animation->tStart[0].pProc = NULL;
    p_animation->tStart[1].pProc = NULL;
    p_animation->tTriger[0].pProc = NULL;
    p_animation->tTriger[1].pProc = NULL;
    p_animation->tTriger[2].pProc = NULL;
    p_animation->tTriger[3].pProc = NULL;
    p_animation->tTriger[4].pProc = NULL;
    p_animation->taMateral[0].pProc = NULL;
    p_animation->taMateral[1].pProc = NULL;
    p_animation->taMateral[2].pProc = NULL;
    p_animation->bconnected = 0;
  }

  p_set_anim->pMesh = 0;

  for (i = 0; i < 6; i++)
    p_set_anim->_3dsAnim[i].mesh = -1;

  p_set_anim->bSetMesh = 0;
  p_set_anim->iMeshID = 0;
  p_set_anim->iMesh = 0;
  p_set_anim->last = 0;

}

//------------------------------------------------------------------------------------------------
// zkontroluje, zda jsou animace hotovy
//------------------------------------------------------------------------------------------------
int gl_Are_Animations_Done(LEVELINFO * p_Level)
{
  int i, min = 101, flag;
  char bCheck_Result = p_Level->bCheck_Result;
  char bCheck = p_Level->bCheck;

gl_Are_Animations_Done_TEST:

  for (i = 0; i < p_set_anim->last; i++) {
    am_Animation_Status(&p_set_anim->animation[i], &am);
    gl_Do_Triger(&p_set_anim->animation[i]);
    flag = p_set_anim->animation[i].flag;

    if ((flag > -1) && (flag < min))
      min = flag;
  }

  if (!bCheck_Result &&
    (min >= 90) && (p_Level->Flip < 2) && (!p_Level->bTriger))
    p_Level->bAllow_Key = 1;

  if (p_Level->bStone)
    p_Level->bAllow_Key = 0;

  if (min < 101)
    return 0;

  if (p_Level->Flip) {
    p_set_anim->last = 0;
    gl_Rozvaz_Meshe();
    gl_Zero_Queue();
    gl_Next_Anim_Queue_Set(p_Level);
    gl_Go_Animations();
    p_Level->Flip--;

    goto gl_Are_Animations_Done_TEST;
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
// set frame rate
//------------------------------------------------------------------------------------------------
void gl_Set_Frame_Rate(void)
{
  bSet_Frame_Rate = 1;
}


//------------------------------------------------------------------------------------------------
// Zero movement keys
//------------------------------------------------------------------------------------------------
void gl_Zero_Key_Buffer(CONTROL_KEYS * p_keys)
{
  key[p_keys->move_forward] = 0;
  key[p_keys->turn_back] = 0;
  key[p_keys->turn_left] = 0;
  key[p_keys->turn_right] = 0;

}

//------------------------------------------------------------------------------------------------
// Change destonation square which depends on Rotation
//------------------------------------------------------------------------------------------------
void gl_Change_Destonation(LEVELINFO * p_Level, int *pos)
{
  if (p_Level->Actual_Item == -1)
    return;

  switch (p_Level->Level[p_Level->Actual_Item]->Rotation) {
    case 0:
      pos[1]++;
      break;
    case 1:
      pos[0]++;
      break;
    case 2:
      pos[1]--;
      break;
    case 3:
      pos[0]--;
      break;
  }
}

//------------------------------------------------------------------------------------------------
// Returns index of object selected in inventory
//------------------------------------------------------------------------------------------------
int gl_Inventory_Item_To_Object(LEVELINFO * p_Level, int Item, char bDec)
{
  int result = 0;
  int counter = 0;
  int act_count = 0;
  int i;
  OBJECTDESC *pObject;
  BACK_PACK *p_Back_Pack;

  if (p_Level->Actual_Item == -1)
    return 0;

  p_Back_Pack = p_Level->Level[p_Level->Actual_Item]->p_Back_Pack;

  while (counter <= Item) {
    if (act_count != 3 && act_count != 5)
      counter += p_Back_Pack->item[act_count];
    act_count++;
  }
  act_count--;

  for (i = 0; i < p_Level->Count_Of_Objects; i++) {
    pObject = &p_Level->Object[i];

    if ((pObject->Class == 13) && (pObject->SubClass == act_count)) {
      if (bDec)
        p_Back_Pack->item[act_count]--;

      break;
    }
  }

  return i;
}

//------------------------------------------------------------------------------------------------
// Returns index of object selected in inventory
//------------------------------------------------------------------------------------------------
ITEMDESC *gl_Find_First_Destroed(LEVELINFO * p_Level, int Class, int SubClass)
{
  int i;
  ITEMDESC Item;

  for (i = 0; i < p_Level->Count_Of_Items; i++) {
    Item = p_Level->Item[i];
    if ((Item.bDestroed) && (Item.p_Object->Class == Class) &&
      (Item.p_Object->SubClass == SubClass))
      return &p_Level->Item[i];
  }

  return NULL;
}

//------------------------------------------------------------------------------------------------
// fill key, time and create new frame
//------------------------------------------------------------------------------------------------
int gl_Next_Demo_Frame(int Key, CONTROL_KEYS * p_control,
  char bOvladaniBerusek1, int *pos_o, int *pos_n, long item)
{
  long eplased;

  if (Key == control.menu)
    return 1;

  Demo.Finish = timeGetTime();

  eplased = demo_Eplased_Time(Demo.Start, Demo.Finish);

  if (bOvladaniBerusek1 &&
    (Key == p_control->move_forward ||
      Key == p_control->turn_back ||
      Key == p_control->turn_left || Key == p_control->turn_right))
    Demo.p_Last->Key = Level.Level[item]->Rotation + 100;
  else
    Demo.p_Last->Key = demo_Virual2Fuction(Key, p_control);

  Demo.p_Last->bOvladaniBerusek1 = bOvladaniBerusek1;
  Demo.p_Last->Time = eplased;

  if (pos_o) {
    Demo.p_Last->pos_o[0] = pos_o[0];
    Demo.p_Last->pos_o[1] = pos_o[1];
    Demo.p_Last->pos_o[2] = pos_o[2];

    Demo.p_Last->pos_n[0] = pos_n[0];
    Demo.p_Last->pos_n[1] = pos_n[1];
    Demo.p_Last->pos_n[2] = pos_n[2];
  }
  else {
    Demo.p_Last->pos_o[0] = -1;
    Demo.p_Last->pos_o[1] = -1;
    Demo.p_Last->pos_o[2] = -1;

    Demo.p_Last->pos_n[0] = -1;
    Demo.p_Last->pos_n[1] = -1;
    Demo.p_Last->pos_n[2] = -1;
  }


  if (demo_Create_Frame(&Demo) == NULL)
    return 0;

  return 1;
}

int gl_Rotation2Virtual(int initRot, int finishRot)
{
  int rot = initRot - finishRot;
  float t[3], fr, fi, di;
  int s, ss;

  kam_pol_get((BOD *) t, &fr, &fi, &di);

  fr = fr / 90.0f;

  s = ftoi(fr);

  s = s % 4;

  if (s > 0)
    ss = 4 - s;
  else
    ss = abs(s);


  rot = 4 - (ss - finishRot);

  if (rot > 3)
    rot = rot % 4;
  else
    rot = abs(rot);

  //kprintf(1, "keyrot = %d, rot = %d", finishRot, rot);

  if (!rot)
    return control.move_forward;

  if (rot == 3)
    return control.turn_left;

  if (rot == 1)
    return control.turn_right;

  if (rot == 2)
    return control.turn_back;

  kprintf(1,
    "FUCK gl_Rotation2Virtual, int initRot = %d, int finishRot = %d, rot = %d",
    initRot, finishRot, rot);
  return 0;
}

int gl_Rotation2Rotation(int initRot, int finishRot)
{
  int rot = initRot - finishRot;
  float t[3], fr, fi, di;
  int s, ss;

  kam_pol_get((BOD *) t, &fr, &fi, &di);

  fr = fr / 90.0f;

  s = ftoi(fr);

  s = s % 4;

  if (s > 0)
    ss = 4 - s;
  else
    ss = abs(s);

  rot = 4 - (ss - finishRot);

  if (rot > 3)
    rot = rot % 4;
  else
    rot = abs(rot);

  return rot;
}

//------------------------------------------------------------------------------------------------
// do next demo frame
//------------------------------------------------------------------------------------------------
int gl_Do_Next_Demo_Frame(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
  if (Demo.p_Last) {
    Demo.bOvladaniBerusek1 = Demo.p_Last->bOvladaniBerusek1;

    if (Demo.bOvladaniBerusek1 && Demo.p_Last->Key > 99) {
      if (Level.Flip
        && gl_Rotation2Virtual(Level.Level[Level.Actual_Item]->Rotation,
          Demo.p_Last->Key - 100) != Level.Level[Level.Actual_Item]->Rotation)
        return 0;

      key[gl_Rotation2Virtual(Level.Level[Level.Actual_Item]->Rotation,
          Demo.p_Last->Key - 100)] = 1;
    }
    else {
      if (Demo.p_Last->Key > 4 && Level.Flip)
        return 0;

      key[demo_Fuction2Virtual(Demo.p_Last->Key, &control)] = 1;
    }

    Demo.p_Last = Demo.p_Last->p_Next;
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
// provede polozeni predmetu
//------------------------------------------------------------------------------------------------
int gl_Drop_Item(int item, int *pos, LEVELINFO * p_Level)
{
  DataHandle Data;
  int obj;
  int mesh;
  int new_pos[3], move[3];
  int real_pos;
  ITEMDESC *p_real_posI;
  int Voda, vodapos[3];
  POINTERSTRUCTURE *pS;
  int real;

  if (!gl_Is_Drop_Item_Possible(pos, p_Level))
    return 0;

  gl_Logical2Real(pos[0], pos[1], pos[2], &real_pos, p_Level);

  obj = gl_Inventory_Item_To_Object(p_Level, item, 0);

  if (p_Level->Square[p_Level->Actual_Item].bUnderWater &&
    !p_Level->Object[obj].SubClass &&
    p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->item[0] < 2)
    return 0;

  obj = gl_Inventory_Item_To_Object(p_Level, item, 1);

  Data = kom_pridej_prvek_do_databaze(&(p_Level->Object[obj].Out_File));

  if (Data == -1) {
    //MessageBox(Level.hWnd,"Error while adding item in databaze","Error",MB_OK);
    kprintf(1, "Error while adding item in databaze");
  }

  mesh = kom_vloz_prvek_z_databaze(Data, pos[0], pos[2], pos[1],
    p_Level->Level[p_Level->Actual_Item]->Rotation,
    p_Level->Level[p_Level->Actual_Item]->p_Object->GUID);
  p_Level->Level[p_Level->Actual_Item]->p_Back_Pack->Count_of_Items--;

  new_pos[0] = pos[0];
  new_pos[1] = pos[1];
  new_pos[2] = gl_Find_Bottom(pos, &Voda, p_Level) + 2;

  gl_Get_Move(pos, new_pos, move);

  gl_Logical2Real(pos[0], pos[1], new_pos[2], &real_pos, p_Level);

  p_Level->Level[real_pos] =
    gl_Find_First_Destroed(p_Level, p_Level->Object[obj].Class,
    p_Level->Object[obj].SubClass);

  p_real_posI = p_Level->Level[real_pos];
  p_real_posI->bDestroed = 0;
  p_real_posI->Index_Of_Game_Mesh = mesh;
  p_real_posI->Pos[0] = pos[0];
  p_real_posI->Pos[1] = pos[1];
  p_real_posI->Pos[2] = new_pos[2];
  p_real_posI->Rotation = 0;

  if (move[2]) {
    int r = rand() % 2;
    BOD b(1, 1, 1);

    p_set->animation[p_set->last].p_anim = sim_vyrob_animaci(3, 3, 0);

    sim_vloz_pivot(p_set->animation[p_set->last].p_anim, 0.0f, 1.0f, 0.0f);

    sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 0, &b, 0, 0);

    if (r)
      sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 1, &b,
        (-1 * move[2]) * 20.0f, (-1 * move[2] * 2) - 1);
    else
      sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 1, &b,
        move[2] * 20.0f, (-1 * move[2] * 2) - 1);

    sim_vloz_klic_rotace(p_set->animation[p_set->last].p_anim, 2, &b, 0.0f,
      (-1 * move[2] * 2) + 4);

    sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 0, 0.0f, 0.0f,
      0.0f, 0);
    sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 1, 0.0f,
      (float) (move[2] * 2), 0.0f, (-1 * move[2] * 2) - 1);

    sim_vloz_klic_posun(p_set->animation[p_set->last].p_anim, 2, 0.0f,
      (float) (move[2] * 2), 0.0f, (-1 * move[2] * 2) + 4);

    sim_interpoluj_animaci(p_set->animation[p_set->last].p_anim,
      (-1 * move[2]) * 2 + 5, 0);

    p_set->animation[p_set->last].p_run =
      rani_aktivuj_cekej(p_set->animation[p_set->last].p_anim,
      &p_set->animation[p_set->last].flag);

    p_set->animation[p_set->last].p_matrix =
      rani_privaz_mesh(p_set->animation[p_set->last].p_run, mesh, 0);
    gl_Add_Mesh(&p_set->animation[p_set->last], mesh);
    //p_set->animation[p_set->last].mesh = mesh;

    if (new_pos[2] == 1
      && gl_Check_Throw_off_Water(new_pos, new_pos[2], p_Level))
      Voda = 0;

    if (Voda != -1) {
      pS = (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

      if (pS) {
        int percent, percent1;
        int x, y = pos[2] - new_pos[2];

        Voda++;

        gl_Logical2Real(pos[0], pos[1], Voda, &real, p_Level);

        pS->p_Level = p_Level;
        pS->iParam = 1;
        pS->bParam = (pos[2] - Voda) / 2;
        pS->viParam2[0] = 1;
        pS->viParam2[1] = 0;
        pS->viParam2[2] = 0;

        vodapos[0] = pos[0];
        vodapos[1] = pos[1];
        vodapos[2] = Voda;

        memcpy((void *) pS->viParam1, (void *) vodapos, 3 * sizeof(int));

        x =
          (int) floor(((pos[2] - (Voda + 1)) * 100) / (float) (pos[2] -
            new_pos[2]));
        y =
          (int) floor((((-1 * move[2] * 2) -
              1) * 100) / (float) ((-1 * move[2] * 2) + 4));

        am_Set_Triger_Function(&p_set->animation[p_set->last], anmend_Water,
          0, real, pS, (x * y) / 100, 1);

        percent = (x * y) / 100;

        x = (int) floor((100 / (float) (pos[2] - new_pos[2])));

        percent1 = (x * y) / 100;

        am_Set_Triger_Function(&p_set->animation[p_set->last], anmend_Cakanec,
          0, real, pS, percent + percent1, 2);
      }
    }

    am_Set_Triger_Function(&p_set->animation[p_set->last], anmend_Item_Fall,
      0, real_pos, p_Level,
      (int) floor((float) (((-1 * move[2]) * 2 +
            2) * 100) / (float) (((-1 * move[2]) * 2) + 5)), 0);

    rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
      anmend_Item_FallStartAnim, 0, real_pos, p_Level);

    p_set->last++;
    p_Level->Flip++;
    p_Level->status = 1;
    p_Level->bAllow_Key = 0;

    gl_Next_Queue_Set(p_Level);

    if (p_Level->Flip == 1)
      gl_Go_Animations();
  }
  else {
    anmend_Item_Fall(0, real_pos, p_Level);
    anmend_Item_FallStartAnim(0, real_pos, p_Level);
    p_Level->status = 2;
  }

  return 1;
}

char gl_Get_Correspond_Rotation(int Side, char bDemo)
{
  float t[3], fr, fi, di;
  int s, ss;

  kam_pol_get((BOD *) t, &fr, &fi, &di);

  fr = fr / 90.0f;

  s = ftoi(fr);

  s = s % 4;

  if (s > 0)
    ss = 4 - s;
  else
    ss = abs(s);

  ss += Side;

  if (ss > 3)
    ss -= 4;

  return ss;
}

//------------------------------------------------------------------------------------------------
// presune se na zadaneho brouka
//------------------------------------------------------------------------------------------------
int gl_Moveto_Beatle(int btl, LEVELINFO * p_Level)
{
  CAMERA_ANIMATION camera;

  int i = _3d_Is_There_Beetle(btl, p_Level);

  if (i == -1)
    return 0;

  if (p_Level->Beetle[i] != -1) {
    BOD b;
    float dist;
    int frame;
    int rot;
    int last_b;

    //p_camera->flag = 0;
    p_Level->Beetle_Index = i;
    last_b = Level.Actual_Item;
    p_Level->Actual_Item = gl_Select_Beatle(i, p_Level);

    if (p_Level->Actual_Item != -1 && last_b != Level.Actual_Item)
      gl_Select_BeatleFlek(p_Level->Level[p_Level->Actual_Item], p_Level);

    if (p_Level->Actual_Item == -1)
      return 0;

    if (p_Level->bPosouvatKameru) {
      kom_mesh_get_float(p_Level->Level[p_Level->Actual_Item]->
        Index_Of_Game_Mesh, &b.x, &b.y, &b.z, &rot);

      kam_pol_get((BOD *) & camera.finish.Target, &camera.finish.r,
        &camera.finish.fi, &camera.finish.Distance);
      dist = vzdal_bodu_bod(&b, (BOD *) & camera.finish.Target);
      frame = (long) ceil(dist * 3);
      kam_pol_anim(&b, camera.finish.r, camera.finish.fi,
        camera.finish.Distance, &cam_flag, GK_REMOVE, frame, 1.0f);
    }
  }

  return 1;
}

int gl_Screen_Shot(int i)
{
  char text[256], t[32];
  char z[] = "0000";
  FILE *f = NULL;

  do {
    strcpy(text, CurrentWorkingDirectory);
    //strcat(text,"\\screenshot"); - DIR
    strcat(text, "/screenshot");

    itoa(i, t, 10);

    strncat(text, z, 4 - strlen(t));
    strcat(text, t);
    strcat(text, ".bmp");
    f = fopen(text, "r");
    if (!f)
      break;

    i++;
    fclose(f);
  }
  while (f);

//      lsi_Make_Screenshot(hwconf.hDC, text);

  return i;
}

void gl_Init_Buttons(LEVELINFO * p_Level)
{
  int i, pos[3];
  int r;

  for (i = 0; i < p_Level->Size_of_Level; i++)
    if (p_Level->Level[i])
      if (p_Level->Level[i]->p_Object->Class == 10) {
        memcpy((void *) pos, (void *) p_Level->Level[i]->Pos,
          3 * sizeof(int));
        pos[2]++;

        gl_Logical2Real(pos[0], pos[1], pos[2], &r, p_Level);

        if (p_Level->Level[r])
          p_Level->Level[i]->p_Back_Pack->item[0] = 1;
        else
          p_Level->Level[i]->p_Back_Pack->item[0] = 0;
      }
}


void gl_Flip(AUDIO_DATA * pad, LEVELINFO * p_Level, int *Frame_Rate_Counter,
  char *bCursor, int *Cursor_Time_Out, CAMERA_ANIMATION * p_camera,
  unsigned long *NoKeyCounter, char *bBeetleAdded, char *bLastSunuti,
  CAMERA_STRUCT * pGetCamera, char *no_Menu, float *_3d_Scale_Factor,
  int *act_item, int *iReturn, char demo, char *bOvladaniBerusek1)
{
  float orientation[6];

	if(timeGetTime() - p_Level->dwLastMusicCheck > 10000)
	{
		if(!ogg_playing() && (pad->Music_Gain > 0.05f) && (!p_Level->bExit) && karmin_aktivni)
				ap_Play_Song(0,1,pad);
		
		p_Level->dwLastMusicCheck = timeGetTime();
	}

  (*Frame_Rate_Counter)++;

  if (bSet_Frame_Rate) {
    am.frame_per_quater = (float) (*Frame_Rate_Counter);
    bSet_Frame_Rate = 0;
    (*Frame_Rate_Counter) = 0;

		if(karmin_aktivni)
			as_Do_Ambient_Sounds(p_Level->Environment.id,pad,p_Level);

    if (*bCursor) {
      (*Cursor_Time_Out)++;
      if ((*Cursor_Time_Out) > 20) {
        (*bCursor) = 0;
        (*Cursor_Time_Out) = 0;
        sHint.bHint = 0;
      }
    }

    //zmena vetru
    if (!(rand() % 20))
      am_Change_Wind(p_Level);

  }

  //ap_Count_Environment(p_ad,&Level);

/*	if(!p_camera->anim_type)
		camera_Animate_Jumping(p_camera);
	else
		camera_Animate(p_camera);*/

  if (*NoKeyCounter && !(*bBeetleAdded) && !p_Level->status) {
    if (p_Level->Actual_Item != -1)
      if (am_Add_Beetle_Animation(p_Level->Level[p_Level->Actual_Item]->
          Index_Of_Game_Mesh, p_Level,
          p_Level->Level[p_Level->Actual_Item]->Rotation, 0))
        *bBeetleAdded = 1;

    *bLastSunuti = 0;
  }


  //zmen pozici posluchace podle kamery
  kam_get_float_ext(orientation, pGetCamera->Position);

  orientation[0] = -orientation[0];
  orientation[1] = -orientation[1];
  orientation[2] = -orientation[2];

//      adas_Set_Listener_Position(pGetCamera->Position);
//      adas_Set_Listener_Orientation(orientation);

  am_Animate_Items(&am, p_Level);
  am_Animate_Beetles(p_Level);
  am_Do_Zhave_castice(p_Level);
  am_Do_Flares(p_Level);
  am_Do_Lifts(p_Level);
  am_Do_Water(p_Level);
  am_Do_Water_KolaB(p_Level);
  am_Do_Bubliny_Berusek(p_Level);
  am_Do_BublVybuchy(p_Level);
  am_Do_Exit_Efects(p_Level);
  am_Do_Exit_Sparks(p_Level);
  am_Do_TelCSparks(p_Level);
  am_Do_Rain(&p_Level->Rain);
  am_Do_RainB(&p_Level->StreetRain);
  am_Do_Snow(&p_Level->Snow);
  am_Do_Nature_Effects(p_Level);
  am_Do_BarelSparks(p_Level);
  am_Do_BeatleSmokes(p_Level);
  am_Obsluha_Koure_Brouku(p_Level);
  am_Do_BublSystemE(p_Level);
  am_Do_BublSystemC(p_Level);
  am_Do_Street_Lights(p_Level);
  am_Do_Swamp_Lights(p_Level);
  am_Do_Fairies(p_Level);
  am_Do_Star_Lights(p_Level);
  am_Do_StarFall(&Level);
  am_Do_CandleSparks(&Level);
  am_Do_CandleSmoke(&Level);

  spracuj_spravy(0);

  kom_klavesy_hra();
  kom_kresli_level();

  if (*bCursor) {
    if (!*no_Menu)
      _3d_Draw_Menus(bCursor, Cursor_Time_Out, _3d_Scale_Factor, p_Level,
        *act_item, iReturn, demo);
    else
      flip();

    *act_item = iReturn[0];
    p_Level->bMenuButton = iReturn[1];
  }
  else {
    if (!*no_Menu)
      _3d_Draw_Menus(bCursor, Cursor_Time_Out, _3d_Scale_Factor, p_Level,
        *act_item, iReturn, demo);
    else
      flip();

    //*act_item = iReturn[0];
    p_Level->bMenuButton = iReturn[1];
  }
}

void gl_Beetle_Exit(LEVELINFO * p_Level)
{
  BOD b;
  float dist;
  int frame;
  int rot, iPos[3], i;
  CAMERA_ANIMATION camera;
  ANIMATION_QUEUE_SET *p_prev_set = gl_Get_Prev_Queue_SetA(2, p_Level);

  if (p_Level->Actual_Item == -1)
    return;

  if (p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek) {
    kom_flek_zrus(p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek);
    p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek = 0;
  }

  memcpy((void *) iPos, (void *) p_Level->Level[p_Level->Actual_Item]->Pos,
    3 * sizeof(int));

  iPos[2]--;

  gl_Logical2Real(iPos[0], iPos[1], iPos[2], &i, p_Level);

  if (p_prev_set->animation[1].p_run != -1)
    rani_pripoj_funkci(p_prev_set->animation[1].p_run, anmend_Exit,
      p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
      p_Level->Level[i]->Index_Of_Game_Mesh, p_Level);
  else if (p_prev_set->animation[0].p_run != -1)
    rani_pripoj_funkci(p_prev_set->animation[0].p_run, anmend_Exit,
      p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
      p_Level->Level[i]->Index_Of_Game_Mesh, p_Level);
  else
    anmend_Exit(p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
      p_Level->Level[i]->Index_Of_Game_Mesh, p_Level);

  am_Remove_Beetle_Animation(p_Level->Level[p_Level->Actual_Item]->
    Index_Of_Game_Mesh, p_Level);

  p_Level->Beetle[p_Level->Beetle_Index] = -1;
  p_Level->Level[p_Level->Actual_Item]->bDestroed = 1;
  p_Level->Level[p_Level->Actual_Item] = 0;
  p_Level->Actual_Item = -1;
  p_Level->Actual_Item = gl_Find_Next_Beatle(0, p_Level);

  if (p_Level->Actual_Item == -1)
    p_Level->Level_Exit = 1;
  else {
    gl_Create_SelectionFlek(p_Level->Level[p_Level->Actual_Item], p_Level);

    if (p_Level->bPosouvatKameru) {
//                      camera.flag = 0;

      kom_mesh_get_float(p_Level->Level[p_Level->Actual_Item]->
        Index_Of_Game_Mesh, &b.x, &b.y, &b.z, &rot);

      kam_pol_get((BOD *) & camera.finish.Target, &camera.finish.r,
        &camera.finish.fi, &camera.finish.Distance);
      dist = vzdal_bodu_bod(&b, (BOD *) & camera.finish.Target);
      frame = (long) ceil(dist * 2);
      kam_pol_anim(&b, camera.finish.r, camera.finish.fi,
        camera.finish.Distance, &cam_flag, GK_REMOVE, frame, 1.0f);
    }
  }

  p_Level->bExit = 0;
}

int gl_Get_Count_Of_Action_Items(LEVELINFO * p_Level)
{
  int i, x = 0;

  for (i = 0; i < p_Level->Count_Of_Items; i++)
    if (p_Level->Action_Item[i] == -1)
      return x;
    else
      x++;

  return x;
}

void gl_Udelej_Krychlicku(int *p, int *iPos, LEVELINFO * p_Level, float r,
  float g, float b)
{
  KOFOLOVA_CARA2 *pCara;
  float pos[3];

  kom_get_fyz_souradnice(iPos[0], iPos[2], iPos[1], (BOD *) pos);

  pCara = &p_Level->pCara[(*p)];

  pCara->p1.x = pos[0] - 1;
  pCara->p1.y = pos[1];
  pCara->p1.z = pos[2] - 1;

  pCara->p2.x = pos[0] + 1;
  pCara->p2.y = pos[1];
  pCara->p2.z = pos[2] - 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] + 1;
  pCara->p1.y = pos[1];
  pCara->p1.z = pos[2] - 1;

  pCara->p2.x = pos[0] + 1;
  pCara->p2.y = pos[1];
  pCara->p2.z = pos[2] + 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] + 1;
  pCara->p1.y = pos[1];
  pCara->p1.z = pos[2] + 1;

  pCara->p2.x = pos[0] - 1;
  pCara->p2.y = pos[1];
  pCara->p2.z = pos[2] + 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] - 1;
  pCara->p1.y = pos[1];
  pCara->p1.z = pos[2] + 1;

  pCara->p2.x = pos[0] - 1;
  pCara->p2.y = pos[1];
  pCara->p2.z = pos[2] - 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];

  pCara->p1.x = pos[0] - 1;
  pCara->p1.y = pos[1] + 2;
  pCara->p1.z = pos[2] - 1;

  pCara->p2.x = pos[0] + 1;
  pCara->p2.y = pos[1] + 2;
  pCara->p2.z = pos[2] - 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] + 1;
  pCara->p1.y = pos[1] + 2;
  pCara->p1.z = pos[2] - 1;

  pCara->p2.x = pos[0] + 1;
  pCara->p2.y = pos[1] + 2;
  pCara->p2.z = pos[2] + 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] + 1;
  pCara->p1.y = pos[1] + 2;
  pCara->p1.z = pos[2] + 1;

  pCara->p2.x = pos[0] - 1;
  pCara->p2.y = pos[1] + 2;
  pCara->p2.z = pos[2] + 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] - 1;
  pCara->p1.y = pos[1] + 2;
  pCara->p1.z = pos[2] + 1;

  pCara->p2.x = pos[0] - 1;
  pCara->p2.y = pos[1] + 2;
  pCara->p2.z = pos[2] - 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];

  pCara->p1.x = pos[0] - 1;
  pCara->p1.y = pos[1] + 2;
  pCara->p1.z = pos[2] - 1;

  pCara->p2.x = pos[0] - 1;
  pCara->p2.y = pos[1];
  pCara->p2.z = pos[2] - 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] + 1;
  pCara->p1.y = pos[1] + 2;
  pCara->p1.z = pos[2] - 1;

  pCara->p2.x = pos[0] + 1;
  pCara->p2.y = pos[1];
  pCara->p2.z = pos[2] - 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] - 1;
  pCara->p1.y = pos[1] + 2;
  pCara->p1.z = pos[2] + 1;

  pCara->p2.x = pos[0] - 1;
  pCara->p2.y = pos[1];
  pCara->p2.z = pos[2] + 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
  pCara = &p_Level->pCara[(*p)];
  pCara->p1.x = pos[0] + 1;
  pCara->p1.y = pos[1];
  pCara->p1.z = pos[2] + 1;

  pCara->p2.x = pos[0] + 1;
  pCara->p2.y = pos[1] + 2;
  pCara->p2.z = pos[2] + 1;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;

  (*p)++;
}

void gl_kresli_Caru(KOFOLOVA_CARA2 * pCara, ITEMDESC * pItem, int i,
  LEVELINFO * p_Level, float r, float g, float b)
{
  int iPos[3];

  kom_get_fyz_souradnice(pItem->Pos[0], pItem->Pos[2], pItem->Pos[1],
    &pCara->p1);
  pCara->p1.y++;

  gl_Real2Logical(pItem->Connection[i], iPos, p_Level);
  kom_get_fyz_souradnice(iPos[0], iPos[2], iPos[1], &pCara->p2);
  pCara->p2.y++;

  pCara->b1.r = r;
  pCara->b1.g = g;
  pCara->b1.b = b;

  pCara->b2.r = r;
  pCara->b2.g = g;
  pCara->b2.b = b;
}

void gl_Vytvor_Seznam_Car(LEVELINFO * p_Level)
{
//      KOFOLOVA_CARA2 *pCara;
  ITEMDESC *pItem;
  int i, x = gl_Get_Count_Of_Action_Items(p_Level);
  int p = 0, iPos[3];

  if (p_Level->pCara)
    free((void *) p_Level->pCara);

  p_Level->pCara =
    (KOFOLOVA_CARA2 *) malloc(((x * 25) + 4) * sizeof(KOFOLOVA_CARA2));

  if (!p_Level->pCara)
    return;

  for (i = 0; i < x; i++)
    if (p_Level->Level[p_Level->Action_Item[i]]) {
      pItem = p_Level->Level[p_Level->Action_Item[i]];

      if (pItem->p_Object->Class == 9 ||
        pItem->p_Object->Class == 10 || pItem->p_Object->Class == 11) {
        if (pItem->p_Object->Class == 10 &&
          p_Level->Level[pItem->Connection[0]]) {
          gl_kresli_Caru(&p_Level->pCara[p], pItem, 0, p_Level, 0, 0, 1);
          p++;
          gl_Udelej_Krychlicku(&p, pItem->Pos, p_Level, 0, 0, 1);

          gl_Real2Logical(pItem->Connection[0], iPos, p_Level);
          gl_Udelej_Krychlicku(&p, iPos, p_Level, 0, 0, 1);
        }
        else
          if (pItem->p_Object->Class == 10 &&
          p_Level->Level[pItem->Connection[1]]) {
          gl_kresli_Caru(&p_Level->pCara[p], pItem, 1, p_Level, 0, 0, 1);
          p++;
          gl_Udelej_Krychlicku(&p, pItem->Pos, p_Level, 0, 0, 1);

          gl_Real2Logical(pItem->Connection[1], iPos, p_Level);
          gl_Udelej_Krychlicku(&p, iPos, p_Level, 0, 0, 1);
        }
        else {
          float r, g, b;

          if (pItem->p_Object->Class == 9) {
            r = 0;
            g = 0;
            b = 1;
          }

          if (pItem->p_Object->Class == 11) {
            if (!pItem->p_Object->SubClass) {
              r = 1;
              g = 1;
              b = 0;
            }
            else if (pItem->p_Object->SubClass == 1) {
              r = 1;
              g = 0;
              b = 0;
            }
            else {
              r = 0;
              g = 0;
              b = 1;
            }
          }

          if (p_Level->Action_Item[i] != pItem->Connection[0])
            gl_kresli_Caru(&p_Level->pCara[p], pItem, 0, p_Level, r, g, b);
          else
            gl_kresli_Caru(&p_Level->pCara[p], pItem, 1, p_Level, r, g, b);

          p++;

          gl_Udelej_Krychlicku(&p, pItem->Pos, p_Level, r, g, b);

          if (p_Level->Action_Item[i] != pItem->Connection[0])
            gl_Real2Logical(pItem->Connection[0], iPos, p_Level);
          else
            gl_Real2Logical(pItem->Connection[1], iPos, p_Level);

          gl_Udelej_Krychlicku(&p, iPos, p_Level, r, g, b);
        }
      }
    }

  kom_set_linelist((KOFOLOVA_CARA *) p_Level->pCara, p);
}

int gl_Beetle2VirtualKey(int Beetle, CONTROL_KEYS * p_control)
{
  if (Beetle == 0)
    return p_control->beatle1;
  if (Beetle == 1)
    return p_control->beatle2;
  if (Beetle == 2)
    return p_control->beatle3;
  if (Beetle == 3)
    return p_control->beatle4;
  if (Beetle == 4)
    return p_control->beatle5;
  if (Beetle == 5)
    return p_control->beatle6;

  return 0;
}

int gl_Act_Item2VirtualKey(int Act_Item, CONTROL_KEYS * p_control)
{
  if (Act_Item == 0)
    return p_control->item1;
  if (Act_Item == 1)
    return p_control->item2;
  if (Act_Item == 2)
    return p_control->item3;
  if (Act_Item == 3)
    return p_control->item4;

  return 0;
}

void gl_Start_Level_Environment(LEVELINFO * p_Level, AUDIO_DATA * p_ad)
{
  WEATHERSTRUCT *p_Weather;
  NATUREEFFECTSTRUCT *pNE;
  int i, kvalita_castic;
  LEVEL_ENVIRONMENT *pEnv = kom_get_level_environment();

  kvalita_castic =
    GetPrivateProfileInt("hra", "kvalita_casticp", 0, ini_file);
  kvalita_castic++;

  p_Level->Rain.bRain = 0;
  p_Level->Snow.bSnow = 0;
  p_Level->Mist.bMist = 0;
  p_Level->Rain.bWaterCircles = 0;

  if (!pEnv) {
    kprintf(1, "kom_get_level_environment vratila NULL!!!");
    ap_Load_Environment("default.env", p_Level, p_ad);
    return;
  }

  if (strlen(pEnv->cEnv))
    ap_Load_Environment(pEnv->cEnv, p_Level, p_ad);
  else
    ap_Load_Environment("default.env", p_Level, p_ad);

  p_Level->bUnderWaterLevel = p_Level->Environment.bunderwater;

  if (!p_Level->iNSrart)
    p_Level->iFloorMaterial =
      ap_Load_Sound_List(p_ad, p_Level->Environment.extsoundf, 1);

  if (strlen(pEnv->cAnimEff))
    lsi_Load_Level_Script(p_Level, pEnv->cAnimEff);

  for (i = 0; i < 3; i++)
    if (pEnv->Weather[i].bUsed) {
      switch (pEnv->Weather[i].iWeather) {
        case 1:
          {
            p_Level->Rain.bRain = 1;
            p_Weather = &pEnv->Weather[i];

            am_Create_Rain(p_Level, &p_Level->Rain,
              p_Weather->fDist,
              (int) ceil(p_Weather->uiDensity / kvalita_castic),
              p_Weather->uiIntensity, p_Weather->fmaxDivergence,
              p_Weather->fSky);
          }
          break;
        case 2:
          {
            p_Level->Snow.bSnow = 1;
            p_Weather = &pEnv->Weather[i];

            am_Create_Snow(p_Level, &p_Level->Snow,
              p_Weather->fDist,
              (int) ceil(p_Weather->uiDensity / kvalita_castic),
              p_Weather->uiIntensity, p_Weather->fmaxDivergence,
              p_Weather->fSky);
          }
          break;
        case 3:
          {
            p_Level->Mist.bMist = 1;

            p_Weather = &pEnv->Weather[i];

            am_Create_Prizemni_Mlhu(p_Level, p_Weather->fDist,
              p_Weather->fSky);
          }
          break;
      }
    }

  for (i = 0; i < 10; i++)
    if (pEnv->NatureEffect[i].bUsed) {
      pNE = &pEnv->NatureEffect[i];

      am_Create_Nature_Effect(p_Level, pNE->EffectID, pNE->fRadius,
        pNE->uiDensity, pNE->uiIntensity, pNE->fSky, pNE->fLowPlane);
    }

  am_Create_Water_Effect(p_Level);

  p_Level->iNSrart++;
}

void gl_Mesh_Click(LEVELINFO * p_Level, char demo, AUDIO_DATA * p_ad)
{
  float pos[3];
  int i, bmesh, btl, rot;
  int c = 0;

  bmesh = kom_get_mesh_mys(12);

  if (bmesh == -1) {
    kprintf(1, "gl_Mesh_Click - kom_get_mesh_mys vratil -1");
    return;
  }

  for (i = 0; i < 6; i++)
    if (p_Level->BublSystem[i].mesh == bmesh) {
      btl = p_Level->Item[p_Level->BublSystem[i].iItem].p_Object->SubClass;

      if (!demo)
        gl_Next_Demo_Frame(gl_Beetle2VirtualKey(btl, &control), &control, 0,
          NULL, NULL, Level.Actual_Item);

      kom_zpruhlednovat_prvky_zmena_berusky();

      kom_mesh_get_float(p_Level->Item[p_Level->BublSystem[i].iItem].
        Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);
/*
			if(gl_Moveto_Beatle(btl, p_Level))
				ap_Play_Sound(0,1,0, pos, gl_Choose_Wave_Index(23), NULL, p_ad);
*/
      kprintf(1, "prepnito na berusku mesh = %d", bmesh);
      c++;
    }

  if (!c)
    kprintf(1, "nepodarilo se prepnout na berusku ... mesh = %d", bmesh);
}

int gl_CheckCamera(LEVELINFO * p_Level)
{
  if (p_Level->bPosouvatKameru && p_Level->Flip)
    return 0;
  else
    return 1;
}

void gl_Resetini(LEVELINFO * p_Level, char *bOvladaniBerusek1)
{
  p_Level->bPosouvatKameru =
    GetPrivateProfileInt("hra", "posouvat_kameru", 0, ini_file);
  *bOvladaniBerusek1 = GetPrivateProfileInt("hra", "ovladani", 0, ini_file);
  p_Level->bPohled_Berusky =
    GetPrivateProfileInt("hra", "pohled_berusky", 0, ini_file);
  Bind_Keys(&control);

  kom_reload_ini();
}

void gl_Change_Dir_To_Level(char *p_Level_Name)
{
  char ctext[MAX_PATH + 1];

  GetPrivateProfileString("game", "save_dir", "c:\\", ctext, MAX_PATH,
    ini_file);
  working_file_translate(ctext, MAX_PATH);

  chdir((ctext));
  chdir((p_Level_Name));
}

//------------------------------------------------------------------------------------------------
// Run level of selected number
// return               1  = Level dokoncen uspesne
//                              2  = Level restart
//                              3  = Level nedokoncen uspesne
//------------------------------------------------------------------------------------------------
int gl_Run_Level(char *p_Level_Name, char *p_Env_Name, AUDIO_DATA * p_ad,
  int iCPU)
{
  int iKeyLine[3];
  int iKeyCursor;
  int Return;
  int pos_n[3];
  int Frame_Rate_Counter;
  long _new;
  float position[3], orientation[6];
  float p_pos[3];
  char bCursor;
  int Cursor_Time_Out;
  float _3d_Scale_Factor[2];
  int act_item;
  int top_item;
  int iReturn[2];
  char Active_Beetle;
  char no_Menu;

//      CAMERA_ANIMATION        camera;
  char bLastSunuti;
  unsigned long NoKeyCounter;
  char bBeetleAdded;

//      GRAMMAR                         gr;
//      TRIGER_STRUCTURE        tri;
  CAMERA_STRUCT GetCamera;
  char bOvladaniBerusek1;

//      char                            bInitCameraSet;
  char LastRotation;
  char bVisibility;
  char bHint;
  int iMenuAnimation = 0;
  char cLevelName[64];
  char cDemoName[128];
  LEVEL_ENVIRONMENT *pEnv;
  int iRestart = 0;
  char bRestart = 0;
  char bPause = 0;

  //...................... *** oprava
  char bCamera;
  int cameraanim;
  static int flag;
  int bmpc = 0;

  //...................... *** oprava


  Level.iNSrart = 0;

  Xresolution = hwconf.xres;
  Yresolution = hwconf.yres;

  //      camera.last_move = -1;
  Level.bInventory = GetPrivateProfileInt("hra", "bInventory", 1, ini_file);
  Level.bTopLedge = GetPrivateProfileInt("hra", "bTopLedge", 1, ini_file);;

  _3d_Scale_Factor[0] = (float) Xresolution / 1024.0f;
  _3d_Scale_Factor[1] = (float) Yresolution / 768.0f;

  Level.Level_Exit = 0;
  Level.lCamera.Position[0] = 0;
  Level.lCamera.Position[1] = 0;
  Level.lCamera.Position[2] = 0;
  Level.lCamera.r = 0;
  Level.lCamera.fi = 0;
  Level.lCamera.Distance = 0;
  Level.iFloorMaterial = 0;

PLAY_LEVEL_START:

  tools_Parse_Command_Line(p_Level_Name, cLevelName, cDemoName, &demo);

  ZeroMemory(&sKuk, sizeof(KUK_STATE));
  _3d_Nahraj_Kuk();

  PRCameraFlag = 0;

  ZeroMemory(&sHint, sizeof(HINT_STATE));
  sKuk.bKUK = 0;

  // init values
  Frame_Rate_Counter = 0;
  position[0] = 0;
  position[1] = 0;
  position[2] = 0;
  orientation[0] = 0;
  orientation[1] = 1;
  orientation[2] = 0;
  orientation[3] = 0;
  orientation[4] = 0;
  orientation[5] = 1;
  p_pos[0] = 1;
  p_pos[1] = 1;
  p_pos[2] = 1;
  act_item = -1;
  top_item = -1;
  Active_Beetle = 0;
  iReturn[0] = 0;
  iReturn[1] = 0;
//      camera.flag                     = 0;
//      camera.bChange          = 1;
  Level.bSikminaSound = 0;
  Level.bSunuti = 0;
  bLastSunuti = 0;
  Level.bVypniFlek = 0;
  NoKeyCounter = 0;
  bBeetleAdded = 1;
  Level.bTriger = 0;
//      bInitCameraSet          = 0;
  LastRotation = -1;
  cameraflag = 0;
  PRCameraFlag = 0;
  PRCameraFlagChange = 0;
  Level.bMenuButton = 0;
  Level.bMenuRunning = 0;
  Level.pCara = NULL;
  Level.bSikminaUpExeption = 0;
  bHint = 0;
  bCamera = 1;
  Level.bB1ExtraCameraRot = 0;
  Level.lLevel_Exitc = 2000;
  Level.StreetRain.bRain = 0;
  Level.StreetRain.bWaterCircles = 0;
  Level.bResetini = 0;
  Level.bSaveDemo = 0;
  Level.bUnderWaterLevel = 0;
  Level.iNumOfSteps = 0;
  Level.dwStartTime = 0;
  Level.dwPlayTime = 0;
  Level.bStoneSound = 0;
  Level.iCursor = 0;
  Level.iKursorAnimation = K_CHYBA;
  Level.iKursorMesh = K_CHYBA;
  Level.bRestart = 0;
  Level.iMessageBoxReturn = -1;
  Level.bCancelMenu = 0;
  Level.bReturnToMenu = 0;
  Level.bDemoSaveExeption = 0;
  Level.bGameResume = 0;

  iKeyCursor = 0;

  ZeroMemory(iKeyLine, 3 * sizeof(int));

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      Level.bUnderWaterLevel = 1;

  am_Change_Wind(&Level);

  Level.bPosouvatKameru =
    GetPrivateProfileInt("hra", "posouvat_kameru", 0, ini_file);
  bOvladaniBerusek1 = GetPrivateProfileInt("hra", "ovladani", 0, ini_file);
  bVisibility = GetPrivateProfileInt("hra", "init_visibility", 0, ini_file);
  no_Menu = GetPrivateProfileInt("hra", "no_menu", 0, ini_file);
  Level.bPohled_Berusky =
    GetPrivateProfileInt("hra", "pohled_berusky", 0, ini_file);

  kprintf(1, "demo_Load...");
  if (demo) {
    char Demo_File[255];

    Level.bPosouvatKameru = 0;
    Level.bPohled_Berusky = 0;

    GetPrivateProfileString("game", "save_dir", "c:\\", Demo_File, 255,
      ini_file);
    working_file_translate(Demo_File, 255);

    //strcat(Demo_File,"\\"); - DIR?
    strcat(Demo_File, "/");
    strcat(Demo_File, cDemoName);

    if (!demo_Load(&Demo, Demo_File, &bOvladaniBerusek1, cLevelName,
        p_Env_Name)) {
      //MessageBox(NULL,"Chyba pri nahravani dema!", "Chyba", MB_OK);
      kprintf(1, "Chyba pri nahravani dema!");
//                      MyMessageBox(hwnd_hry, "##error_title", "##demo_load_error","");
      return 2;
    }

    Demo.p_Last = Demo.p_First;
  }

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  Bind_Keys(&control);

//      Level.hWnd = hwnd_hry;
  Level.p_ad = &ad;
//      ad                 = *p_ad;

  kprintf(1, "_3d_Scale_Factor: X = %f, Y = %f", _3d_Scale_Factor[0],
    _3d_Scale_Factor[1]);

  Level.Base_Priority = iCPU;

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  //LEVEL LOAD//////////////////////////////////////////////////
  if (!strcmp(p_Env_Name, "LOAD_GAME")) {
    BUNKA_LEVELU_DISK *b_l_d;
    int isize;
    char ctext[MAX_PATH];

    lsi_Get_Dir_Name(ctext, cLevelName);
    gl_Change_Dir_To_Level(ctext);

    kprintf(1, "lsi_Create_Level_Raw...");

    if (!lsi_Create_Level_Raw(cLevelName, &b_l_d, &isize)) {
//                      MyMessageBox(hwnd_hry, "##error_title", "##level_load_error","");
      return -1;
    }

    kprintf(1, "kom_load_level...");
    kom_load_level(cLevelName, 0, 0, b_l_d, isize);
    gl_Change_Dir_To_Level(ctext);

    kprintf(1, "free((void *) b_l_d);");
    free((void *) b_l_d);
  }
  else {
    BUNKA_LEVELU_DISK *b_l_d;
    int isize;
    char ctext[MAX_PATH];

    GetPrivateProfileString("game", "game_level_dir", "c:\\", ctext, 256,
      ini_file);
    working_file_translate(ctext, 256);
    chdir(ctext);
    lsi_Get_Dir_Name(ctext, cLevelName);
    chdir(ctext);

    kprintf(1, "lsi_Create_Level_Raw...");
    if (!lsi_Create_Level_Raw(cLevelName, &b_l_d, &isize))
      return -1;

    strcpy(Level.cLoadedFrom, cLevelName);
    kprintf(1, "kom_load_level...");
    kom_load_level(cLevelName, 1, bRestart, b_l_d, isize);
    GetPrivateProfileString("game", "game_level_dir", "c:\\", ctext, 256,
      ini_file);
    working_file_translate(ctext, 256);
    chdir(ctext);
    lsi_Get_Dir_Name(ctext, cLevelName);
    chdir(ctext);

    kprintf(1, "free((void *) b_l_d);");
    free((void *) b_l_d);
  }

  if (!no_Menu) {
    _3d_Create_Top_Ledge_Display_List(_3d_Scale_Factor);
    _3d_Create_Inventory_Display_List(_3d_Scale_Factor);
  }

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  kprintf(1, "lsi_Load_Items");
  if (lsi_Load_Items(&Level) == FALSE) {
    //MessageBox(hWnd,"Unable to load items","Error",MB_OK);
    kprintf(1, "Unable to load items");
//              MyMessageBox(hwnd_hry, "##error_title", "##level_load_error","");
    return -1;
  }

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  kprintf(1, "lsi_Load_Level");
  Return = lsi_Load_Level(cLevelName, &Level);

  if (lsi_Load_Saved_Level(cLevelName, &Level) == -2) {
    Level.Level_Exit = 2;
    Level.lLevel_Exitc = 0;
  }

  strcpy(Level.cLevelName, cLevelName);
  //_getcwd(Level.cLevelDir, MAX_PATH);

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  if (Return == -1) {
    lsi_Release_Items(&Level);
    //MessageBox(hWnd,"Unable to load level","Error",MB_OK);
    kprintf(1, "Unable to load level");
//              MyMessageBox(hwnd_hry, "##error_title", "##level_load_error","");
    return -1;
  }

  if (Return > 0) {
    char text[100];

    sprintf(text, "Unresolved GUIDs: %d", Return);
    //MessageBox(hWnd,text,"Warning",MB_OK);
    kprintf(1, text);
  }

  kprintf(1,
    "gl_Set_Water_Marks, w_Create_Water_Boundaries, gl_Add_Walls_Bellow_Lifts...");
  gl_Set_Water_Marks(&Level);
  w_Create_Water_Boundaries();
  gl_Add_Walls_Bellow_Lifts(&Level);
  //gl_Add_Walls_Bellow_LiftsH(&Level);

  if (!am_Init(&am, &Level)) {
    lsi_Release_Level(&Level);
    lsi_Release_Items(&Level);
    //MessageBox(hWnd,"Unable to init animation module","Error",MB_OK);
    kprintf(1, "Unable to init animation module");
    return -1;
  }
  else
    am.p_ad = p_ad;

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  kprintf(1, "fn_Set_Font...");

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if (!fn_Set_Font(cFontFile[1]))
    kprintf(1, "Unable to set font!");

  if (!fn_Load_Bitmaps())
    kprintf(1, "Unable to load font bitmaps");

  kprintf(1, "fn_Texture...");

  fn_Texture(1);
  _3d_Load_Animations(_3d_Scale_Factor);
  fn_Release_Font(1);

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  am_Init_Zhave_Castice(&Level);

  /*kprintf(1,"Load gramatiky trigeru...");
     gr_Load_Grammar("trigers_grammar.txt", &gr);
     kprintf(1,"Load trigeru...");
     trig_Load_Trigers(cLevelName,"trigers.txt", &tri, &gr); */

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  //kprintf(1,"1...");
  kprintf(1, "kom_post_init_level...");
  kom_post_init_level();

  kprintf(1, "kom_pridej_kurzor_do_levelu...");
  Level.iKursorMesh = kom_pridej_kurzor_do_levelu();

  if (Level.iKursorMesh == K_CHYBA)
    kprintf(1, "kom_pridej_kurzor_do_levelu == K_CHYBA");

  //kprintf(1,"2...");
  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  kprintf(1, "gl_Pripoj_Flek_k_Beruskam...");
  gl_Pripoj_Flek_k_Beruskam(&Level);
  kprintf(1, "gl_Pripoj_Flek_k_Predmetum...");
  gl_Pripoj_Flek_k_Predmetum(&Level);
  kprintf(1, "gl_Init_Buttons...");
  gl_Init_Buttons(&Level);
  kprintf(1, "am_Create_Sparks_To_Exits...");
  am_Create_Sparks_To_Exits(&Level);
  kprintf(1, "am_Start_Animace_Pontonek...");
  am_Start_Animace_Pontonek(&Level);
  kprintf(1, "am_Start_Barels...");
  am_Start_Barels(&Level);
  kprintf(1, "am_Create_Steps_System...");
  am_Create_Steps_System(&Level);
  am_Start_Joint_Animations();
  am_Create_Item_Lights(&Level);

  if (iActualScene == 5)
    am_Start_Voda_Kanal();

  //am_Start_Voda_Akvarko();

  //kprintf(1,"10...");

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //gl_TEST(&Level);
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  kprintf(1, "gl_Start_Level_Environment...");
  gl_Start_Level_Environment(&Level, p_ad);

  if (Level.bUnderWaterLevel) {
    kprintf(1, "gl_Set_Underwater_Level");
    gl_Set_Underwater_Level(&Level);
  }

  kprintf(1, "gl_Set_Underwater_Level_Snorchl...");
  gl_Set_Underwater_Level_Snorchl(&Level);
  //kprintf(1,"11...");

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  //kprintf(1,"12...");
  //nastav uvodni visibility
  kprintf(1, "kom_zpruhlednovat_prvky...");
  kom_zpruhlednovat_prvky(bVisibility);

  //kprintf(1,"13...");
  //load animacnich materialu
  kprintf(1, "kom_amat_mesh_get_num...");
  Level.iMaterialb = kom_amat_mesh_get_num();

  Level.pMaterialb =
    (MATERIAL_BUNKA_K *) malloc(Level.iMaterialb * sizeof(MATERIAL_BUNKA));
  if (!Level.pMaterialb)
    kprintf(1, "Nelze alokovat pamet pro animovane materialy");
  else {
    kom_amat_mesh_get_all((MATERIAL_BUNKA *) Level.pMaterialb);
    lsi_Bind_Materials2Items(&Level);
  }

  kprintf(1, "gl_Find_Next_Beatle...");
  am.frame_per_quater = 1;
  Level.Beetle_Index = -1;
  Level.Actual_Item = gl_Find_Next_Beatle(0, &Level);

  kprintf(1, "gl_Create_SelectionFlek...");
  //kprintf(1,"14...");
  if (Level.Actual_Item != -1)
    gl_Create_SelectionFlek(Level.Level[Level.Actual_Item], &Level);

   adas_Set_Listener_Environment(Level.Environment.room);
   as_Start(Level.Environment.id,p_ad,&Level);

  //schovej_konzoli();

  //kprintf(1,"15...");
  //adas_OGG_Set_Priority(Level.Base_Priority);

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);


  //kprintf(1,"16...");
  kam_get_float(orientation);
  adas_Set_Listener_Orientation(orientation);

  kprintf(1, "demo_Init...");
  if (!demo) {
    if (demo_Init(&Demo) == NULL) {
      //MessageBox(Level.hWnd,"Unable to create first demo frame","Error", MB_OK);
      kprintf(1, "Unable to create first demo frame");
    }
  }

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  Level.status = 0;
  Level.bAllow_Key = 1;
  Level.bCheck = 0;
  Level.Level_Exit = 0;
  Level.bExit = 0;
  Level.Item_Lock = 0;
  Level.Throw_off = 0;
  Level.Move_Exeption = 0;
  Level.Actual_Qset = 0;
  Level.Actual_Anim_Qset = 0;
  Level.Flip = 0;

  kprintf(1, "gl_Next_Anim_Queue_Set...");
  gl_Next_Anim_Queue_Set(&Level);
  gl_Next_Anim_Queue_Set(&Level);
  gl_Next_Anim_Queue_Set(&Level);
  gl_Zero_Queue();
  gl_Next_Anim_Queue_Set(&Level);
  gl_Zero_Queue();
  gl_Next_Anim_Queue_Set(&Level);
  gl_Zero_Queue();
  gl_Next_Queue_Set(&Level);
  gl_Next_Queue_Set(&Level);

  //kprintf(1,"17...");

  bCursor = 0;
  Cursor_Time_Out = 21;

  if (demo) {
    if (demo == 1)
      Demo.dwTimeCounter = 0;
    //Demo.Timer_ID = SetTimer(Level.hWnd, NEXTDEMOFRAMETIMERID, Demo.p_Last->Time, (TIMERPROC)gl_Do_Next_Demo_Frame);

  }
  else
    Demo.Start = timeGetTime();

  RunMenuLoadScreenAddProgress(-1);
  RunMenuLoadScreenDrawProgress(-1, -1);

  kprintf(1, "RunMenuLoadScreenRelease...");
  RunMenuLoadScreenRelease(3);

  kprintf(1, "kom_get_level_environment, kam_3ds_nahraj_animaci...");
  if (!demo) {
    char file[256];
    int camerarun;
    int icamanimrestart =
      GetPrivateProfileInt("hra", "camera_intro", 0, ini_file);

    if (bRestart)
      icamanimrestart = 0;

    GetPrivateProfileString("game", "game_data_dir", "c:\\", file, 255,
      ini_file);
    working_file_translate(file, 255);
    pEnv = kom_get_level_environment();
    if (pEnv) {
      //strcat(file,"\\"); - DIR?
      strcat(file, "/");
      strcat(file, pEnv->cCam);

      cameraanim = kam_3ds_nahraj_animaci(pGDataArchive, file);

      if (cameraanim != K_CHYBA) {
        kam_set_kino_screen(p_ber);
        kom_zpruhlednovac_off();
        Level.bInventory = 0;
        Level.bTopLedge = 0;
        camerarun = kam_start(cameraanim, &cameraflag, GK_REMOVE, 0, 0);
      }
      else
        cameraflag = -1;


      if (!icamanimrestart) {
        BOD p_p(0,10,0), p_t(0,0,0); // default camera values
        float p_roll = 0;

        if (kam_3ds_cti_klic(cameraanim, 1, &p_p, &p_t, &p_roll) != K_CHYBA) {
          kam_stop();
          kam_3ds_set(&p_p, &p_t, p_roll);
        }

        kam_set_normal_screen(p_ber);
        kom_zpruhlednovac_on();
        Level.bInventory =
          GetPrivateProfileInt("hra", "bInventory", 1, ini_file);
        Level.bTopLedge =
          GetPrivateProfileInt("hra", "bTopLedge", 1, ini_file);
        cameraflag = -1;
      }
    }
  }
/*	else
		cameraflag = -1;*/

  bRestart = 0;

  Level.dwStartTime = timeGetTime();

  kprintf(1, "ap_Play_Song...");
  ogg_gain(p_ad->Music_Gain);
  ap_Play_Song(0,1,p_ad);

  Level.dwLastMusicCheck = timeGetTime();
  kprintf(1, "ap_Play_Song DONE...");

  while (!Level.Level_Exit || Level.lLevel_Exitc > 0) {

    //brouk na exitu
    if (Level.bExit)
      gl_Beetle_Exit(&Level);

    if (!Level.bMenuRunning && !Level.Level_Exit) {

      if (key[K_F3] && !demo) {
        key[K_F3] = 0;

        if (cameraflag == -1) {
          WCHAR cText[128];

          PRCameraFlag = 0;
          ZeroMemory(cText, 128);
          RunMenuLoadGameLoad2("Mmload_game_load3d.txt", Level.p_ad, &Level, &am, 1, cText);
          PRCameraFlag = 1;

          if (wcslen(cText)) {
            Level.Level_Exit = 4;
            Level.lLevel_Exitc = 0;
            //TODO  <----------------------- cText = jmeno levelu na load

            ZeroMemory(Level.cLevelName, 64);
            lsi_Save_Exist(cText, Level.cLevelName);

            //WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, cText, wcslen(cText), Level.cLevelName, 63, NULL, NULL);
          }
        }
      }

      if (key[control.menu] && cameraflag == -1) {
        float fpos[3] = { 0, 0, 0 };

        sHint.bHint = 0;
        key[control.menu] = 0;

        Level.bLastTopLedge = Level.bTopLedge;
        Level.bLInventory = Level.bInventory;
        Level.bInventory = 1;
        Level.bTopLedge = 1;
        _3d_Load_From_Sys_To_Video(107);
        fn_Load_Textures_From_RAM();
        Level.bMenuRunning = 1;
        _3d_Start_Animation(14, NULL, &iMenuAnimation);
        ap_Play_Sound(0,1,0, fpos, gl_Choose_Wave_Index(23), NULL, p_ad);

        /*Level.Level_Exit = 2;
           Level.lLevel_Exitc = 0; */
      }

      if (key[control.pause]) {
        key[control.pause] = 0;

        if (demo) {
          if (!bPause)
            bPause = 1;
          else
            bPause = 0;
        }
      }

      if (key[control.item_lock]) {
        key[control.item_lock] = 0;

        if (!demo)
          gl_Next_Demo_Frame(control.item_lock, &control, 0, NULL, NULL,
            Level.Actual_Item);

        if (Level.Item_Lock)
          Level.Item_Lock = 0;
        else
          Level.Item_Lock = 1;
      }

      if (key[control.next_song]) {
        key[control.next_song] = 0;
        ap_Stop_Song(&ad);
      }

      if (key[control.visibility]) {
        key[control.visibility] = 0;

        if (bVisibility)
          bVisibility = 0;
        else
          bVisibility = 1;

        kom_zpruhlednovat_prvky(bVisibility);
      }

      if (key[control.highlight]) {
        key[control.highlight] = 0;

        kom_zvyraznovac();
      }

      if (key[control.hint]) {
        key[control.hint] = 0;

        if (bHint) {
          bHint = 0;
          kom_set_linelist(NULL, 0);
          free((void *) Level.pCara);
          Level.pCara = NULL;
        }
        else {
          gl_Vytvor_Seznam_Car(&Level);
          bHint = 1;
        }

      }

      if (key[control.disable_inventory]) {
        key[control.disable_inventory] = 0;

        if (Level.bInventory) {
          Level.bInventory = 0;
          WritePrivateProfileString("hra", "bInventory", "0", ini_file);
          _3d_Cancel_Animations(&Level);
        }
        else {
          Level.bInventory = 1;
          WritePrivateProfileString("hra", "bInventory", "1", ini_file);
        }
      }

      if (key[control.disable_top_ledge]) {
        key[control.disable_top_ledge] = 0;

        if (Level.bTopLedge) {
          WritePrivateProfileString("hra", "bTopLedge", "0", ini_file);
          Level.bTopLedge = 0;
          _3d_Stop_Menu_Select();
        }
        else {
          Level.bTopLedge = 1;
          WritePrivateProfileString("hra", "bTopLedge", "1", ini_file);
        }
      }
    }

//-------------------------------------------------------------------------------------------
    if ((Level.status == 5) || (Level.status == 3) || (Level.status == 1)) {
      if (gl_Are_Animations_Done(&Level))
        Level.status--;

      //if(Level.status < 3)
      if (!Level.status)
        Level.bAllow_Key = 1;
    }

    if (Level.status == 4) {
    _gl_Throw_off:

      if (Level.Throw_off) {
        if (!gl_Throw_off(Level.Column, &Level)) {
          if (Level.bColumn || Level.bSecColumn) {
            if (gl_Analyse_Column(Level.Column, &Level)) {
              gl_Throw_off(Level.Column, &Level);
              Level.Throw_off = 1;
              Level.status = 4;

              if (!Level.Flip && p_set_anim->last) {
                gl_Go_Animations();
                gl_Next_Queue_Set(&Level);
                Level.Flip++;
              }
              else if (p_set_anim->last) {
                gl_Next_Queue_Set(&Level);
                Level.Flip++;
              }
            }

            if (gl_Analyse_Column(Level.SecColumn, &Level)) {
              Level.SecColumn[2] += 2;

              gl_Throw_off(Level.SecColumn, &Level);
              Level.Throw_off = 1;
              Level.status = 4;

              if (!Level.Flip && p_set_anim->last) {
                gl_Go_Animations();
                gl_Next_Queue_Set(&Level);
                Level.Flip++;
              }
              else if (p_set_anim->last) {
                gl_Next_Queue_Set(&Level);
                Level.Flip++;
              }
            }
          }

          if (Level.Flip) {
            Level.Throw_off = 0;
            Level.status = 5;
          }
          else {
            Level.Throw_off = 0;
            Level.status = 2;
            goto _Check_Logic_Dependences;
          }
        }
        else {
          Level.status--;
          Level.Flip++;
          gl_Next_Queue_Set(&Level);
          gl_Go_Animations();
        }
      }
      else {
        Level.status = 2;
        goto _Check_Logic_Dependences;
      }
    }

    if (Level.status == 2) {
    _Check_Logic_Dependences:

      if (Level.Throw_off)
        if (gl_Analyse_Column(Level.Column, &Level)) {
          Level.Throw_off = 1;
          Level.status = 4;
          goto _gl_Throw_off;
        }
        else
          Level.Throw_off = 0;

      if (!Level.bCheck) {
        Level.bCheck = 0;

        if (!gl_Check_Logic_Dependences(&Level)) {
          Level.status = 0;
          Level.bAllow_Key = 1;
        }
        else {
          Level.status--;
          Level.Flip++;
          gl_Next_Queue_Set(&Level);
          gl_Go_Animations();
        }
      }
      else {
        Level.bCheck = 0;

        if (Level.bCheck_Result)
          Level.status--;
        else {
          Level.status = 0;
          Level.bAllow_Key = 1;
        }
      }
    }

    //jestlize nastala udalost, proved ji
/*	if(Level.bTriger && !Level.status)
	{
		gl_Do_Game_Trigers(&tri, &Level, p_ad);
		Level.bTriger = 0;
		Level.bAllow_Key = 1;
		gl_Zero_Key_Buffer(&control);
	}*/

//-------------------------------------------------------------------------------------------
    if (key[control.screenshot]) {
      key[control.screenshot] = 0;
      bmpc = gl_Screen_Shot(bmpc);
      bmpc++;
    }

    //if(camera.bChange && cameraflag == -1 && !Level.bMenuRunning)
    //if(cameraflag == -1 && !Level.bMenuRunning && gl_CheckCamera(&Level))
    if (cameraflag == -1 && !Level.bMenuRunning && !Level.Level_Exit) {
      if (key[control.camera_center]) {
        key[control.camera_center] = 0;

        camera_Center(&cameraflag, &Level, 2, 0, 0);
      }

/*		if (key[control.camera_move_right])
		{
			if (!demo)
				gl_Next_Demo_Frame(control.camera_move_right);
			camera_Move(control.camera_move_right, 0, -1, &camera, am.frame_per_quater);
		}

		if (key[control.camera_move_left])
		{
			if (!demo)
				gl_Next_Demo_Frame(control.camera_move_left);
			camera_Move(control.camera_move_left, 1, -1, &camera, am.frame_per_quater);
		}

		if (key[control.camera_move_forward])
		{
		if (!demo)
				gl_Next_Demo_Frame(control.camera_move_forward);
			camera_Move(control.camera_move_forward, -1, 0, &camera, am.frame_per_quater);
		}

		if (key[control.camera_move_back])
		{
			if (!demo)
				gl_Next_Demo_Frame(control.camera_move_back);
			camera_Move(control.camera_move_back, -1, 1, &camera, am.frame_per_quater);
		}*/

      if (key[control.camera_fast_turn_right])
        camera_Fast_Turn(control.camera_fast_turn_right, 0, &cameraflag,
          am.frame_per_quater);

      if (key[control.camera_fast_turn_left])
        camera_Fast_Turn(control.camera_fast_turn_left, 1, &cameraflag,
          am.frame_per_quater);
/* TODO?
		if (key[control.camera_zoom_in])
			camera_Zoom(control.camera_zoom_in, 0, &cameraflag, am.frame_per_quater);

		if (key[control.camera_zoom_out])
			camera_Zoom(control.camera_zoom_out, 1, &cameraflag, am.frame_per_quater);
*/
      /*if (key[control.camera_turn_right] && !bOvladaniBerusek1)
         {
         if (!demo)
         gl_Next_Demo_Frame(control.camera_turn_right);
         camera_Turn(control.camera_turn_right, 0, -1, &camera, am.frame_per_quater);
         }

         if (key[control.camera_turn_left] && !bOvladaniBerusek1)
         {
         if (!demo)
         gl_Next_Demo_Frame(control.camera_turn_left);
         camera_Turn(control.camera_turn_left, 1, -1, &camera, am.frame_per_quater);
         }

         if (key[control.camera_turn_up])
         {
         if (!demo)
         gl_Next_Demo_Frame(control.camera_turn_up);
         camera_Turn(control.camera_turn_up, -1, 0, &camera, am.frame_per_quater);
         }

         if (key[control.camera_turn_down])
         {
         if (!demo)
         gl_Next_Demo_Frame(control.camera_turn_down);
         camera_Turn(control.camera_turn_down, -1, 1, &camera, am.frame_per_quater);
         } */
    }

    //if(bCamera && cameraflag == -1 && !camera.flag)
    if (bCamera && cameraflag == -1) {
/*		float p_roll = 0;

		kam_3ds_get((BOD *)&camera.finish.Position, (BOD *)&camera.finish.Target, &camera.finish.Roll);
		kam_3ds_get((BOD *)&camera.real.Position, (BOD *)&camera.real.Target, &camera.real.Roll);
		kam_pol_get((BOD *)&camera.finish.Position, &camera.finish.r, &camera.finish.fi, &camera.finish.Distance);
		kam_pol_get((BOD *)&camera.real.Position, &camera.real.r, &camera.real.fi, &camera.real.Distance);*/

      kam_set_normal_screen(p_ber);

      Level.bInventory =
        GetPrivateProfileInt("hra", "bInventory", 1, ini_file);
      Level.bTopLedge = GetPrivateProfileInt("hra", "bTopLedge", 1, ini_file);
      kom_zpruhlednovac_on();

      if (iRestart)
        camera_Center(&cameraflag, &Level, 15, 1, 1);
      else
        camera_Center(&cameraflag, &Level, 15, 1, 0);

      if (iActualLevel >= 200 && iActualLevel < 300)
        RunMenuTutorial2("Mmtutor3D.txt", p_ad, &Level, &am);

      PRCameraFlag = 1;

      bCamera = 0;
    }

    //if(key[control.menu] && cameraflag != -1 && !camera.flag && bCamera)
    if (key[control.menu] && cameraflag != -1 && bCamera) {      
      BOD p_p(0,10,0), p_t(0,0,0); // default camera values
      float p_roll = 0;

      key[control.menu] = 0;

      if (!demo)
        gl_Next_Demo_Frame(control.menu, &control, 0, NULL, NULL,
          Level.Actual_Item);

      cameraflag = -1;

      if (kam_3ds_cti_klic(cameraanim, 1, &p_p, &p_t, &p_roll) != K_CHYBA) {
        kam_stop();
        kam_3ds_set(&p_p, &p_t, p_roll);
      }

/*		kam_3ds_get((BOD *)&camera.finish.Position, (BOD *)&camera.finish.Target, &camera.finish.Roll);
		kam_3ds_get((BOD *)&camera.real.Position, (BOD *)&camera.real.Target, &camera.real.Roll);
		kam_pol_get((BOD *)&camera.finish.Position, &camera.finish.r, &camera.finish.fi, &camera.finish.Distance);
		kam_pol_get((BOD *)&camera.real.Position, &camera.real.r, &camera.real.fi, &camera.real.Distance);*/

      kam_set_normal_screen(p_ber);
      kom_zpruhlednovac_on();
      Level.bInventory =
        GetPrivateProfileInt("hra", "bInventory", 1, ini_file);;
      Level.bTopLedge =
        GetPrivateProfileInt("hra", "bTopLedge", 1, ini_file);;

      if (iRestart)
        camera_Center(&cameraflag, &Level, 8, 1, 1);
      else
        camera_Center(&cameraflag, &Level, 8, 1, 0);

      if (iActualLevel >= 200 && iActualLevel < 300) {
        PRCameraFlag = 0;
        RunMenuTutorial2("Mmtutor3D.txt", p_ad, &Level, &am);
        PRCameraFlag = 1;
      }

      bCamera = 0;
    }

    NoKeyCounter++;

    //if (Level.bAllow_Key && cameraflag == -1 && !camera.flag && !Level.bMenuRunning)
    if (Level.bAllow_Key && cameraflag == -1 && !Level.bMenuRunning
      && !Level.Level_Exit) {
      PRCameraFlag = 1;

      if (key[K_F2] && !demo) {
        key[K_F2] = 0;

        if (cameraflag == -1) {
          WCHAR cText[128];

          sHint.bHint = 0;
          ZeroMemory(cText, 128);
          PRCameraFlag = 0;
          RunMenuLoadGameLoad2("Mmload_game_loads3d.txt", Level.p_ad, &Level, &am, 0, cText);
          PRCameraFlag = 1;

          if (wcslen(cText))
            lsi_Save_Level(cText, &Level);
        }
      }

      if (key[K_F1]) {
        key[K_F1] = 0;

        if (cameraflag == -1) {
          sHint.bHint = 0;

          PRCameraFlag = 0;
          RunMenuHelp2("mmhelp3d.txt", p_ad, &Level, &am);
          PRCameraFlag = 1;
          sHint.bHint = 0;
        }
      }

      // prehravani fast dema
      if (demo == 2 && !Level.Flip && !bPause) {
        if (!iKeyLine[iKeyCursor]) {
          if (!PRCameraFlagChange) {
            if (demo_Create_Sequence(iKeyLine, 3, Demo.p_Last,
                &bOvladaniBerusek1, Level.Level[Level.Actual_Item]->Rotation,
                &control, &Demo, Level.Level[Level.Actual_Item]->Pos)) {
              key[iKeyLine[0]] = 1;
              iKeyCursor = 1;
            }
            else {
              //NENI TO OVLADANI BERUSKY
            }
          }
        }
        else if (!PRCameraFlagChange) {
          key[iKeyLine[iKeyCursor]] = 1;
          iKeyCursor++;
        }
      }

      //drop item
/*		if((key[control.item1]) ||
		   (key[control.item2]) ||
		   (key[control.item3]) ||
		   (key[control.item4]))
		{
			int pos[3];
			long real_pos;
			ITEMDESC *p_Item;
		
			p_Item = Level.Level[Level.Actual_Item];

			pos[0] = p_Item->Pos[0];
			pos[1] = p_Item->Pos[1];
			pos[2] = p_Item->Pos[2];

			if (!p_Item->Rotation) pos[1]++;
			if (p_Item->Rotation == 1) pos[0]++;
			if (p_Item->Rotation == 2) pos[1]--;
			if (p_Item->Rotation == 3) pos[0]--;

			gl_Logical2Real(pos[0], pos[1], pos[2], &real_pos, &Level);

			if(key[control.item1])
			{
				if (!demo)
					gl_Next_Demo_Frame(control.item1, &control, 0);
				act_item = 0;
				key[control.item1] = 0;
			}
			else
				if(key[control.item2])
				{
					if (!demo)
						gl_Next_Demo_Frame(control.item2, &control, 0);
					act_item = 1;
					key[control.item2] = 0;
				}
				else
					if(key[control.item3])
					{
						if (!demo)
							gl_Next_Demo_Frame(control.item3, &control, 0);
						act_item = 2;
						key[control.item3] = 0;
					}
					else
						if(key[control.item4])
						{
							if (!demo)
								gl_Next_Demo_Frame(control.item4, &control, 0);
							act_item = 3;
							key[control.item4] = 0;
						}

			if((pos[0] >= 0) && (pos[0] < Level.Size[0]) &&
			   (pos[1] >= 0) && (pos[1] < Level.Size[1]) &&
			   (p_Item->p_Back_Pack->Count_of_Items) &&
			   !Level.Level[real_pos])
					if(act_item < Level.Level[Level.Actual_Item]->p_Back_Pack->Count_of_Items)
						gl_Drop_Item(act_item, pos, &Level);

			act_item = -1;
		}*/

/*		if(key[control.inventory])
		{
			int pos[3];
			long real_pos;
			ITEMDESC *p_Item;
		
			if (!demo)
				gl_Next_Demo_Frame(control.inventory, &control, 0);

			key[control.inventory] = 0;

			act_item = 0;
			bCursor = 0;
			Cursor_Time_Out = 0;

			p_Item = Level.Level[Level.Actual_Item];

			pos[0] = p_Item->Pos[0];
			pos[1] = p_Item->Pos[1];
			pos[2] = p_Item->Pos[2];

			if (!p_Item->Rotation) pos[1]++;
			if (p_Item->Rotation == 1) pos[0]++;
			if (p_Item->Rotation == 2) pos[1]--;
			if (p_Item->Rotation == 3) pos[0]--;
			
			gl_Logical2Real(pos[0], pos[1], pos[2], &real_pos, &Level);

			if((pos[0] >= 0) && (pos[0] < Level.Size[0]) &&
			   (pos[1] >= 0) && (pos[1] < Level.Size[1]) &&
			   (p_Item->p_Back_Pack->Count_of_Items) &&
			   !Level.Level[real_pos])
			{

			while(!key[control.inventory_cancel] && !mi.t2)
			{

				if (key[control.inventory_right])
				{
					if (!demo)
						gl_Next_Demo_Frame(control.inventory_right, &control, 0);

					key[control.inventory_right] = 0;

					act_item++;
					if(act_item > (p_Item->p_Back_Pack->Count_of_Items-1))
						act_item = p_Item->p_Back_Pack->Count_of_Items-1;
				}

				if (key[control.inventory_left])
				{
					if (!demo)
						gl_Next_Demo_Frame(control.inventory_left, &control, 0);

					key[control.inventory_left] = 0;

					act_item--;
					if(act_item < 0)
						act_item = 0;
				}

				if ((key[control.inventory_select] || mi.t1) && act_item != -1)
				{
					if (!demo)
						gl_Next_Demo_Frame(control.inventory_select, &control, 0);

					key[control.inventory_select] = 0;
					mi.t1 = 0;

					if(gl_Drop_Item(act_item, pos, &Level))
					{
						key[control.inventory_cancel] = 1;
						act_item = -1;
					}
				}

				gl_Flip(p_ad, &Level, &Frame_Rate_Counter, &bCursor, &Cursor_Time_Out,
						NULL, &NoKeyCounter, &bBeetleAdded, &bLastSunuti, &GetCamera,
						&no_Menu, _3d_Scale_Factor, &act_item, iReturn, demo);

			}
			}

			act_item = -1;
			key[control.inventory_cancel] = 0;
			mi.t2 = 0;
		}
*/
      if (key[control.blow_detonation_pack]) {
        if (!demo)
          gl_Next_Demo_Frame(control.blow_detonation_pack, &control, 0, NULL,
            NULL, Level.Actual_Item);

        key[control.blow_detonation_pack] = 0;
        if ((Level.Level[Level.Actual_Item]->p_Back_Pack->item[4]) &&
          (!Level.Item_Lock))
          gl_Throw_Detonation_Pack(Level.Actual_Item, &Level);
      }

      if (key[control.restart] && !Level.Flip && !Level.status) {
        Level.bRestart = 1;
        Level.iMessageBoxReturn = -1;

        key[control.restart] = 0;
        Level.bMenuRunning = 1;
        Level.bCancelMenu = 1;
        /*Level.Level_Exit = 3;
           Level.lLevel_Exitc = 0; */
      }

      if (key[control.next_beatle] && !Level.Flip && !Level.status) {
        BOD b;
        float dist;
        long frame;
        int rot;
        CAMERA_ANIMATION camera;
        int last_b;

        if (!demo)
          gl_Next_Demo_Frame(control.next_beatle, &control, 0, NULL, NULL,
            Level.Actual_Item);

        key[control.next_beatle] = 0;
//                      camera.flag = 0;

        last_b = Level.Actual_Item;
        Level.Actual_Item = gl_Find_Next_Beatle(Level.Actual_Item, &Level);

        if (Level.Actual_Item != -1 && last_b != Level.Actual_Item)
          gl_Select_BeatleFlek(Level.Level[Level.Actual_Item], &Level);

        am_Add_Beetle_Animation(Level.Level[last_b]->Index_Of_Game_Mesh,
          &Level, Level.Level[last_b]->Rotation, 0);

        kom_zpruhlednovat_prvky_zmena_berusky();

        if (Level.bPosouvatKameru) {
          kom_mesh_get_float(Level.Level[Level.Actual_Item]->
            Index_Of_Game_Mesh, &b.x, &b.y, &b.z, &rot);

          kam_pol_get((BOD *) & camera.finish.Target, &camera.finish.r,
            &camera.finish.fi, &camera.finish.Distance);

          dist = vzdal_bodu_bod(&b, (BOD *) & camera.finish.Target);
          frame = (long) ceil(dist * 2);
          kam_pol_anim(&b, camera.finish.r, camera.finish.fi,
            camera.finish.Distance, &cam_flag, GK_REMOVE, frame, 1.0f);
        }

      }

      if (key[control.move_forward]) {
        gl_Zero_Key_Buffer(&control);

        Level.lLastKeyCounter = NoKeyCounter;
        NoKeyCounter = 0;
        bBeetleAdded = 0;

        p_set->pMesh = 0;
        Level.bSetStep = 0;

        //kprintf(1,"K_NAHORU");

        if (bOvladaniBerusek1) {
          int rot;

          rot = gl_Get_Correspond_Rotation(0, 0);

          if (PRCameraFlagChange != 1) {
            if (!Level.Flip
              && Level.Level[Level.Actual_Item]->Rotation != rot) {
              kom_umisti_prvek(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, Level.Level[Level.Actual_Item]->Pos[0],
                Level.Level[Level.Actual_Item]->Pos[2],
                Level.Level[Level.Actual_Item]->Pos[1], rot);
              Level.Level[Level.Actual_Item]->Rotation = rot;

              am_Correct_Beetle_Animation(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, &Level,
                Level.Level[Level.Actual_Item]->Rotation);

              Level.bB1ExtraCameraRot = 1;
              goto POSUN_DOPREDU;
            }
            else if (Level.Level[Level.Actual_Item]->Rotation == rot) {
              Level.bB1ExtraCameraRot = 0;
              goto POSUN_DOPREDU;
            }
          }
          else {
            Level.bB1ExtraCameraRot = 0;
            goto POSUN_DOPREDU;
          }
        }
        else {
          int pos_o[3];

        POSUN_DOPREDU:

          PRCameraFlagChange = 0;

          gl_Zero_Key_Buffer(&control);
//                              camera.flag = 0;

          pos_o[0] = pos_n[0] = Level.Level[Level.Actual_Item]->Pos[0];
          pos_o[1] = pos_n[1] = Level.Level[Level.Actual_Item]->Pos[1];
          pos_o[2] = pos_n[2] = Level.Level[Level.Actual_Item]->Pos[2];

          gl_Change_Destonation(&Level, pos_n);
          Level.bColumn = 0;
          Level.bSecColumn = 0;
          Level.Sikmina_Flag = 0;
          Level.bCheckDependencesNeeded = 0;
          Level.bExtraSoundSunuti = 0;
          Level.bSikminaDown = 0;

          if (gl_Is_Move_Possible(Level.Level[Level.Actual_Item]->Pos, pos_n,
              &Level)) {
            Level.bSikminaMoveExeption = 0;

            p_set->animation[p_set->last].p_run =
              rani_aktivuj_cekej(am.sim_anim[Level.Level[Level.Actual_Item]->
                Rotation], &p_set->animation[p_set->last].flag);

            /*if(Level.bPosouvatKameru)
               rani_privaz_kameru(p_set->animation[p_set->last].p_run); */

            /*if (!demo)
               gl_Next_Demo_Frame(control.move_forward, &control, bOvladaniBerusek1, pos_o, pos_n); */

            Level.bStone = 0;
            Level.bDemoSaveExeption = 0;

            _new =
              gl_Move_Item(Level.Level[Level.Actual_Item]->Pos, pos_n,
              Level.Actual_Item, &Level);

            if ((_new > -1 || Level.bDemoSaveExeption) && !demo) {
              if (_new > -1)
                gl_Next_Demo_Frame(control.move_forward, &control,
                  bOvladaniBerusek1, pos_o, pos_n, _new);
              else
                gl_Next_Demo_Frame(control.move_forward, &control,
                  bOvladaniBerusek1, pos_o, pos_n, Level.Actual_Item);
            }

            if (_new > -1) {
              POINTERSTRUCTURE *pS =
                (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));
              int i;

              Level.iNumOfSteps++;

              kom_flek_setflag(Level.SelectionFlek.pFlek, 1);

              if (Level.iKursorAnimation != K_CHYBA) {
                rani_zrus(Level.iKursorAnimation);
                Level.iKursorAnimation = K_CHYBA;
              }

              kom_prvek_viditelnost(Level.iKursorMesh, 0);

              Level.bAllow_Key = 0;
              Level.Flip++;
              Level.Actual_Item = _new;
              Level.status = 5;

              /*if (!demo)
                 gl_Next_Demo_Frame(control.move_forward, &control, bOvladaniBerusek1); */

              if (p_set->animation[p_set->last].p_matrix == -1 &&
                p_set->animation[p_set->last].p_run != -1) {
                //na toto bacha .... nejsu si tim jistej!
                rani_zrus(p_set->animation[p_set->last].p_run);
                //----------------------------------------------------
                p_set->last--;
              }
              else if (p_set->animation[p_set->last].p_run != -1)
                p_set->last++;

              Level.Sikmina_FlagOld = Level.bSikminaSound;

              if (p_set->last == 1 && !Level.bSikminaSound)
                rani_pripoj_funkci(p_set->animation[0].p_run,
                  anmend_Are_Animations_Done, 0, 0, &Level);

              //Level.bTriger = trig_Check_ifPos_Trigers(&tri, &Level);

              am_Remove_Beetle_Animation(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, &Level);
              gl_Set_3ds_Mesh(1, 0,
                Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
                Level.Level[Level.Actual_Item]->Rotation);

              if (Level.Flip == 1)
                Level.iWalkAnimation = rand() % 2;

              //privare 3ds animaci
              if (!Level.bSunuti) {
/*							if(Level.lLastKeyCounter > 1 || bLastSunuti)
								gl_Set_3ds_Mesh(1, 0, Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
												Level.Level[Level.Actual_Item]->Rotation);*/

                gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->
                  Index_Of_Game_Mesh, 0, Level.iWalkAnimation,
                  &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);
              }
              else {
/*							if(Level.lLastKeyCounter > 1 || !bLastSunuti)
								gl_Set_3ds_Mesh(1, 1, Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
												Level.Level[Level.Actual_Item]->Rotation);*/

                gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->
                  Index_Of_Game_Mesh, 0, 2,
                  &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);

                if (Level.bSikminaSound) {
                  for (i = 0; i < Level.bSikminaSound; i++)
                    if (!i && Level.bSikminaUp && !Level.bSikminaUpExeption)
                      gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->
                        Index_Of_Game_Mesh, i + 1, 2,
                        &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);
                    else
                      gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->
                        Index_Of_Game_Mesh, i + 1, Level.iWalkAnimation,
                        &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);
                }
              }

              Level.bSikminaUpExeption = 0;

              gl_Connect_Animations(&Level);

              bLastSunuti = Level.bSunuti;
              Level.bSunuti = 0;
              Level.Level[Level.Actual_Item]->_3ds_flag = 0;

              if (pS) {
                pS->p_Level = &Level;
                pS->iParam = pos_n[2];
                pS->bRCamera = 0;

                pS->viParam1[0] = Level.Actual_Item;
                pS->viParam1[1] = 0;
                pS->viParam1[2] = bLastSunuti;

                pS->viParam2[0] = Level.bExtraSoundSunuti;

                pS->bParam = Level.bSikminaSound;

                if (Level.bVypniFlek) {
                  am_Set_Start_Function(&p_set->animation[0],
                    anmend_StepsSikmina, pos_n[0], pos_n[1], (void *) pS, 0);

                  //am_Set_Triger_Function(&p_set->animation[0], am_Do_Step, 1, 0, (void *) &Level, 8, 3);
                  //am_Set_Triger_Function(&p_set->animation[0], am_Do_Step, 1, 0, (void *) &Level, 62, 4);
                }
                else {
                  am_Set_Start_Function(&p_set->animation[0], anmend_Steps,
                    pos_n[0], pos_n[1], (void *) pS, 0);

                  am_Set_Triger_Function(&p_set->animation[0], am_Do_Step, 0,
                    0, (void *) &Level, 8, 3);
                  am_Set_Triger_Function(&p_set->animation[0], am_Do_Step, 0,
                    0, (void *) &Level, 62, 4);
                }

                Level.bVypniFlek = 0;
                Level.bSikminaSound = 0;
              }

              gl_Next_Queue_Set(&Level);

              if (Level.Flip == 1)
                gl_Go_Animations();
              else
                Level.status = 5;

              if (!Level.Throw_off) {
                Level.bCheck_Result = gl_Check_Logic_Dependences(&Level);

                if (Level.bCheck_Result) {
                  gl_Next_Queue_Set(&Level);
                  Level.Flip++;
                }

                Level.bCheck = 1;
              }
              else {
                bLastSunuti = 0;
                Level.bCheck = 0;
                Level.bCheck_Result = 1;
              }
            }
            else if (!Level.bStone) {
              if (p_set->animation[0].p_run != -1 && !Level.Throw_off) {
                rani_zrus(p_set->animation[0].p_run);
                p_set->animation[0].p_run = -1;
              }

              if (p_set->animation[0].p_matrix == -1 && Level.Throw_off &&
                p_set->animation[0].p_run != -1) {
                rani_zrus(p_set->animation[0].p_run);
                p_set->animation[0].p_run = -1;
                Level.Throw_off = 0;
              }

              if (Level.Throw_off) {
                Level.status = 1;
                Level.Flip++;
                gl_Next_Queue_Set(&Level);

                if (Level.Flip == 1)
                  gl_Go_Animations();

                Level.bAllow_Key = 0;
              }
              else {
                p_set->animation[0].flag = 0;
                p_set->last = 0;
              }

              if (Level.bCheckDependencesNeeded)
                Level.status = 5;

            }
          }
        }

        if (!demo)
          Demo.Start = timeGetTime();
      }

      if (key[control.turn_back]) {
        POINTERSTRUCTURE *pS;

        Level.lLastKeyCounter = NoKeyCounter;
        NoKeyCounter = 0;
        bBeetleAdded = 0;

        gl_Zero_Key_Buffer(&control);
//                      camera.flag = 0;

/*			if (!demo && !bOvladaniBerusek1)
				gl_Next_Demo_Frame(control.turn_back, &control, 0, NULL, NULL);*/

        Level.Sikmina_Flag = 0;

        if (bOvladaniBerusek1) {
          int rot = gl_Get_Correspond_Rotation(2, 0);

          if (PRCameraFlagChange != 1) {

            if (!Level.Flip
              && Level.Level[Level.Actual_Item]->Rotation != rot) {
              kom_umisti_prvek(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, Level.Level[Level.Actual_Item]->Pos[0],
                Level.Level[Level.Actual_Item]->Pos[2],
                Level.Level[Level.Actual_Item]->Pos[1], rot);
              Level.Level[Level.Actual_Item]->Rotation = rot;

              am_Correct_Beetle_Animation(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, &Level,
                Level.Level[Level.Actual_Item]->Rotation);

              Level.bB1ExtraCameraRot = 1;
              goto POSUN_DOPREDU;
            }
            else if (Level.Level[Level.Actual_Item]->Rotation == rot)
              goto POSUN_DOPREDU;
          }
          else
            goto POSUN_DOPREDU;

        }
        else
          if ((Level.bSikminaMoveExeption && !Level.Flip)
          || !Level.bSikminaMoveExeption) {
          Level.bSikminaMoveExeption = 0;

          pS = (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

          pos_n[0] = Level.Level[Level.Actual_Item]->Pos[0];
          pos_n[1] = Level.Level[Level.Actual_Item]->Pos[1];
          pos_n[2] = Level.Level[Level.Actual_Item]->Pos[2];

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[5],
            &p_set->animation[p_set->last].flag);
          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh;
          p_set->last++;

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[5],
            &p_set->animation[p_set->last].flag);
          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh;

          p_set->animation[p_set->last].bnext = 1;
          rani_next_animace(p_set->animation[p_set->last - 1].p_run,
            p_set->animation[p_set->last].p_run, 0, 0, 0);
          p_set->last++;

          rani_pripoj_funkci(p_set->animation[1].p_run,
            anmend_Are_Animations_Done, 0, 0, &Level);


          gl_Set_3ds_Mesh(1, 0,
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
            Level.Level[Level.Actual_Item]->Rotation);

          Level.Level[Level.Actual_Item]->Rotation += 2;
          if (Level.Level[Level.Actual_Item]->Rotation > 3)
            Level.Level[Level.Actual_Item]->Rotation =
              Level.Level[Level.Actual_Item]->Rotation - 4;

          Level.Flip++;

          //if(Level.Flip == 1)
          am_Remove_Beetle_Animation(Level.Level[Level.Actual_Item]->
            Index_Of_Game_Mesh, &Level);

          //privare 3ds animaci
          gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
            0, Level.iWalkAnimation,
            &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);

          gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
            1, Level.iWalkAnimation,
            &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);

          Level.Level[Level.Actual_Item]->_3ds_flag = 0;

          if (pS) {
            pS->p_Level = &Level;
            pS->iParam = Level.Level[Level.Actual_Item]->Pos[2];
            pS->bParam = 1;
            if (Level.bPohled_Berusky)
              pS->bRCamera = 3;
            else
              pS->bRCamera = 0;

            pS->viParam1[0] = Level.Actual_Item;
            pS->viParam1[1] = 1;
            pS->viParam1[2] = 0;

            am_Set_Start_Function(&p_set->animation[0], anmend_Steps,
              Level.Level[Level.Actual_Item]->Pos[0],
              Level.Level[Level.Actual_Item]->Pos[1], pS, 0);
          }

          Level.status = 5;
          Level.bAllow_Key = 0;
          Level.bCheck_Result = 0;

          gl_Next_Queue_Set(&Level);

          if (Level.Flip == 1)
            gl_Go_Animations();
        }

        if (!demo)
          Demo.Start = timeGetTime();
      }

      if (key[control.turn_left]) {
        POINTERSTRUCTURE *pS;

        Level.Sikmina_Flag = 0;

        Level.lLastKeyCounter = NoKeyCounter;
        NoKeyCounter = 0;
        bBeetleAdded = 0;

/*			if (!demo && !bOvladaniBerusek1)
				gl_Next_Demo_Frame(control.turn_left, &control, 0, NULL, NULL);*/

        gl_Zero_Key_Buffer(&control);
//                      camera.flag = 0;

        if (bOvladaniBerusek1) {
          int rot = gl_Get_Correspond_Rotation(3, 0);

          if (PRCameraFlagChange != 1 || bLastSunuti) {

            if (!Level.Flip
              && Level.Level[Level.Actual_Item]->Rotation != rot) {
              kom_umisti_prvek(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, Level.Level[Level.Actual_Item]->Pos[0],
                Level.Level[Level.Actual_Item]->Pos[2],
                Level.Level[Level.Actual_Item]->Pos[1], rot);

              Level.Level[Level.Actual_Item]->Rotation = rot;

              am_Correct_Beetle_Animation(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, &Level,
                Level.Level[Level.Actual_Item]->Rotation);

              Level.bB1ExtraCameraRot = 1;
              goto POSUN_DOPREDU;
            }
            else if (Level.Level[Level.Actual_Item]->Rotation == rot)
              goto POSUN_DOPREDU;
          }
          else
            goto POSUN_DOPREDU;

        }
        else
          if ((Level.bSikminaMoveExeption && !Level.Flip)
          || !Level.bSikminaMoveExeption) {
          Level.bSikminaMoveExeption = 0;
          pS = (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

          pos_n[0] = Level.Level[Level.Actual_Item]->Pos[0];
          pos_n[1] = Level.Level[Level.Actual_Item]->Pos[1];
          pos_n[2] = Level.Level[Level.Actual_Item]->Pos[2];

          assert(!p_set->last);

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[5],
            &p_set->animation[p_set->last].flag);
          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh, 0);

          gl_Add_Mesh(&p_set->animation[p_set->last],
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh;

          rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
            anmend_Are_Animations_Done, 0, 0, &Level);

          gl_Set_3ds_Mesh(1, 0,
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
            Level.Level[Level.Actual_Item]->Rotation);

          Level.Level[Level.Actual_Item]->Rotation--;
          if (Level.Level[Level.Actual_Item]->Rotation < 0)
            Level.Level[Level.Actual_Item]->Rotation = 3;

          Level.Flip++;

          //if(Level.Flip == 1)
          am_Remove_Beetle_Animation(Level.Level[Level.Actual_Item]->
            Index_Of_Game_Mesh, &Level);

          //privare 3ds animaci
          gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
            0, Level.iWalkAnimation,
            &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);

          Level.Level[Level.Actual_Item]->_3ds_flag = 0;

          if (pS) {
            pS->p_Level = &Level;
            pS->iParam = Level.Level[Level.Actual_Item]->Pos[2];
            pS->bParam = 0;

            if (Level.bPohled_Berusky)
              pS->bRCamera = 2;
            else
              pS->bRCamera = 0;

            pS->viParam1[0] = Level.Actual_Item;
            pS->viParam1[1] = 0;
            pS->viParam1[2] = 0;

            am_Set_Start_Function(&p_set->animation[0], anmend_Steps,
              Level.Level[Level.Actual_Item]->Pos[0],
              Level.Level[Level.Actual_Item]->Pos[1], pS, 0);
          }

          p_set->last++;
          Level.status = 5;
          Level.bAllow_Key = 0;
          Level.bCheck_Result = 0;

          gl_Next_Queue_Set(&Level);

          if (Level.Flip == 1)
            gl_Go_Animations();
        }

        if (!demo)
          Demo.Start = timeGetTime();
      }

      if (key[control.turn_right]) {
        POINTERSTRUCTURE *pS;

        Level.lLastKeyCounter = NoKeyCounter;
        NoKeyCounter = 0;
        bBeetleAdded = 0;

        Level.Sikmina_Flag = 0;

/*			if (!demo && !bOvladaniBerusek1)
				gl_Next_Demo_Frame(control.turn_right, &control, 0, NULL, NULL);*/

        gl_Zero_Key_Buffer(&control);
//                      camera.flag = 0;

        if (bOvladaniBerusek1) {
          int rot = gl_Get_Correspond_Rotation(1, 0);

          if (PRCameraFlagChange != 1) {

            if (!Level.Flip
              && Level.Level[Level.Actual_Item]->Rotation != rot) {
              kom_umisti_prvek(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, Level.Level[Level.Actual_Item]->Pos[0],
                Level.Level[Level.Actual_Item]->Pos[2],
                Level.Level[Level.Actual_Item]->Pos[1], rot);

              Level.Level[Level.Actual_Item]->Rotation = rot;

              am_Correct_Beetle_Animation(Level.Level[Level.Actual_Item]->
                Index_Of_Game_Mesh, &Level,
                Level.Level[Level.Actual_Item]->Rotation);

              Level.bB1ExtraCameraRot = 1;
              goto POSUN_DOPREDU;
            }
            else if (Level.Level[Level.Actual_Item]->Rotation == rot)
              goto POSUN_DOPREDU;
          }
          else
            goto POSUN_DOPREDU;

        }
        else
          if ((Level.bSikminaMoveExeption && !Level.Flip)
          || !Level.bSikminaMoveExeption) {
          Level.bSikminaMoveExeption = 0;
          pS = (POINTERSTRUCTURE *) malloc(sizeof(POINTERSTRUCTURE));

          pos_n[0] = Level.Level[Level.Actual_Item]->Pos[0];
          pos_n[1] = Level.Level[Level.Actual_Item]->Pos[1];
          pos_n[2] = Level.Level[Level.Actual_Item]->Pos[2];

          assert(!p_set->last);

          p_set->animation[p_set->last].p_run =
            rani_aktivuj_cekej(am.sim_anim[4],
            &p_set->animation[p_set->last].flag);
          p_set->animation[p_set->last].p_matrix =
            rani_privaz_mesh(p_set->animation[p_set->last].p_run,
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh, 0);


          gl_Add_Mesh(&p_set->animation[p_set->last],
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh);
          //p_set->animation[p_set->last].mesh = Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh;

          rani_pripoj_funkci(p_set->animation[p_set->last].p_run,
            anmend_Are_Animations_Done, 0, 0, &Level);

          gl_Set_3ds_Mesh(1, 0,
            Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
            Level.Level[Level.Actual_Item]->Rotation);

          Level.Level[Level.Actual_Item]->Rotation++;
          if (Level.Level[Level.Actual_Item]->Rotation > 3)
            Level.Level[Level.Actual_Item]->Rotation = 0;


          Level.Flip++;

          //if(Level.Flip == 1)
          am_Remove_Beetle_Animation(Level.Level[Level.Actual_Item]->
            Index_Of_Game_Mesh, &Level);

          //privare 3ds animaci
          gl_Set_3ds_Anim(Level.Level[Level.Actual_Item]->Index_Of_Game_Mesh,
            0, Level.iWalkAnimation,
            &Level.Level[Level.Actual_Item]->_3ds_flag, 0, 0, 0);

          Level.Level[Level.Actual_Item]->_3ds_flag = 0;

          if (pS) {
            pS->p_Level = &Level;
            pS->iParam = Level.Level[Level.Actual_Item]->Pos[2];
            pS->bParam = 0;

            if (Level.bPohled_Berusky)
              pS->bRCamera = 1;
            else
              pS->bRCamera = 0;

            pS->viParam1[0] = Level.Actual_Item;
            pS->viParam1[1] = 0;
            pS->viParam1[2] = 0;

            am_Set_Start_Function(&p_set->animation[0], anmend_Steps,
              Level.Level[Level.Actual_Item]->Pos[0],
              Level.Level[Level.Actual_Item]->Pos[1], pS, 0);
          }

          p_set->last++;
          Level.status = 5;
          Level.bAllow_Key = 0;
          Level.bCheck_Result = 0;

          gl_Next_Queue_Set(&Level);

          if (Level.Flip == 1)
            gl_Go_Animations();
        }

        if (!demo)
          Demo.Start = timeGetTime();
      }

      //vyber brouka horkou klavesou
      if (((key[control.beatle1]) ||
          (key[control.beatle2]) ||
          (key[control.beatle3]) ||
          (key[control.beatle4]) ||
          (key[control.beatle5]) || (key[control.beatle6]))
        && !Level.Flip) {
        int btl;

        if (key[control.beatle1]) {
          if (!demo)
            gl_Next_Demo_Frame(control.beatle1, &control, 0, NULL, NULL,
              Level.Actual_Item);

          btl = 0;
          key[control.beatle1] = 0;
        }
        else if (key[control.beatle2]) {
          if (!demo)
            gl_Next_Demo_Frame(control.beatle2, &control, 0, NULL, NULL,
              Level.Actual_Item);

          btl = 1;
          key[control.beatle2] = 0;
        }
        else if (key[control.beatle3]) {
          if (!demo)
            gl_Next_Demo_Frame(control.beatle3, &control, 0, NULL, NULL,
              Level.Actual_Item);

          btl = 2;
          key[control.beatle3] = 0;
        }
        else if (key[control.beatle4]) {
          if (!demo)
            gl_Next_Demo_Frame(control.beatle4, &control, 0, NULL, NULL,
              Level.Actual_Item);

          btl = 3;
          key[control.beatle4] = 0;
        }
        else if (key[control.beatle5]) {
          if (!demo)
            gl_Next_Demo_Frame(control.beatle5, &control, 0, NULL, NULL,
              Level.Actual_Item);

          btl = 4;
          key[control.beatle5] = 0;
        }
        else if (key[control.beatle6]) {
          if (!demo)
            gl_Next_Demo_Frame(control.beatle6, &control, 0, NULL, NULL,
              Level.Actual_Item);

          btl = 5;
          key[control.beatle6] = 0;
        }

        kom_zpruhlednovat_prvky_zmena_berusky();

        gl_Moveto_Beatle(btl, &Level);
      }

      // click
      if (mi.t2 && !demo) {
        _3d_Display_Hint(&Level, _3d_Scale_Factor);

        mi.t2 = 0;
      }


      if (Level.bMenuButton && Level.bTopLedge && mi.t1) {
        float fpos[3] = { 0, 0, 0 };

        Level.bLastTopLedge = Level.bTopLedge;
        Level.bLInventory = Level.bInventory;
        sHint.bHint = 0;
        Level.bInventory = 1;
        Level.bTopLedge = 1;
        _3d_Load_From_Sys_To_Video(107);
        fn_Load_Textures_From_RAM();
        Level.bMenuRunning = 1;
        _3d_Start_Animation(14, NULL, &iMenuAnimation);
        //ap_Play_Sound(0,1,0, fpos, gl_Choose_Wave_Index(23), NULL, p_ad);
      }

      if (mi.t1 && !demo) {
        float cx, cy;
        char btlchange = 0;
        float fpos[3] = { 0, 0, 0 };
        int btl;

        btl = _3d_Check_Beatle_Select(_3d_Scale_Factor);

        if (btl != -1 && Level.bTopLedge && !Level.Flip && !Level.status) {
          if (!demo)
            gl_Next_Demo_Frame(gl_Beetle2VirtualKey(btl, &control), &control,
              0, NULL, NULL, Level.Actual_Item);

          kom_zpruhlednovat_prvky_zmena_berusky();

          gl_Moveto_Beatle(btl, &Level);
/*			
				if(gl_Moveto_Beatle(btl, &Level))
					ap_Play_Sound(0,1,0, fpos, gl_Choose_Wave_Index(23), NULL, p_ad);
*/
          btlchange = 1;
        }

        if (!btlchange && !Level.Flip && !Level.status)
          gl_Mesh_Click(&Level, demo, p_ad);


        cx = (float) mi.x + _3dCur.iaddx;
        cy = (float) mi.y + _3dCur.iaddy;

        if (cx > 745 * _3d_Scale_Factor[0] && cx < 809 * _3d_Scale_Factor[0]
          && cy > 42 * _3d_Scale_Factor[1] && cy < 106 * _3d_Scale_Factor[1]
          && Level.bTopLedge) {
          sHint.bHint = 0;
          PRCameraFlag = 0;
          RunMenuHelp2("mmhelp3d.txt", p_ad, &Level, &am);
          PRCameraFlag = 1;
          sHint.bHint = 0;
        }
      }

      mi.t1 = 0;
    }
    else                        // nemuze se pouzivat pr ovladani kamery
      PRCameraFlag = 0;

    if (mi.t1 && (Level.bRestart || Level.bReturnToMenu)) {
      if (Level.bRestart && Level.iMessageBoxReturn != -1) {
        if (!Level.iMessageBoxReturn) {
          Level.bRestart = 0;
          Level.iMessageBoxReturn = -1;

          if (Level.bCancelMenu)
            Level.bMenuRunning = 0;
        }
        else {
          Level.bRestart = 0;
          Level.iMessageBoxReturn = -1;
          Level.Level_Exit = 3;
          Level.lLevel_Exitc = 0;
          Level.bMenuRunning = 0;

          if (!Level.bCancelMenu)
            _3d_Cancel_Menu(&Level);
        }
      }

      if (Level.bReturnToMenu && Level.iMessageBoxReturn != -1) {
        if (!Level.iMessageBoxReturn) {
          Level.bReturnToMenu = 0;
          Level.iMessageBoxReturn = -1;

          if (Level.bCancelMenu)
            Level.bMenuRunning = 0;
        }
        else {
          Level.bReturnToMenu = 0;
          Level.iMessageBoxReturn = -1;
          Level.Level_Exit = 2;
          Level.lLevel_Exitc = 0;
          Level.bMenuRunning = 0;

          if (!Level.bCancelMenu)
            _3d_Cancel_Menu(&Level);
        }
      }

      mi.t1 = 0;
    }

    if (demo)
      PRCameraFlag = 1;

    if (iMenuAnimation) {
      iMenuAnimation = 0;
      Level.bMenuRunning = 2;
      _3d_Run_Cibule();
    }

    if (PRCameraFlagChange == 2)
      PRCameraFlagChange = 0;

    if (!Level.status)
      NoKeyCounter++;

    if (Level.bResetini) {
      Level.bResetini = 0;
      gl_Resetini(&Level, &bOvladaniBerusek1);
    }

    Level.bGameResume = 0;

    gl_Flip(p_ad, &Level, &Frame_Rate_Counter, &bCursor, &Cursor_Time_Out,
      NULL, &NoKeyCounter, &bBeetleAdded, &bLastSunuti, &GetCamera,
      &no_Menu, _3d_Scale_Factor, &act_item, iReturn, demo,
      &bOvladaniBerusek1);

    if (Level.Level_Exit)
      Level.lLevel_Exitc -= ber.TimeLastFrame;


    if (MenuCheckBossExit()) {
      Level.Level_Exit = 5;
      Level.lLevel_Exitc = 0;
      gl_Kofola_End(0);
    }

/*if(MenuCheckSuccessExit())
  {
	Level.Level_Exit = 1;
	Level.lLevel_Exitc = 0;
  }*/

/*	bmpc++;
	gl_Screen_Shot(bmpc);*/
  }
//---------------------------------------------------------------------------------------------------

  demo = 0;

  Level.dwPlayTime += timeGetTime() - Level.dwStartTime;

  kprintf(1, "kam_pol_get...");

  kam_pol_get((BOD *) & Level.lCamera.Position, &Level.lCamera.r,
    &Level.lCamera.fi, &Level.lCamera.Distance);

  kprintf(1, "Save Demo?");
  if (Level.bSaveDemo) {
    WCHAR cText[128];

    ZeroMemory(cText, 128);
    if (RunMenuLoadGameLoad2("Mmload_game_loads3d.txt", NULL /*Level.p_ad */ ,
        &Level, &am, 2, cText))
      if (wcslen(cText))
        demo_SaveWC(&Demo, cText, bOvladaniBerusek1, cLevelName, p_Env_Name);
  }

//      lsi_Save_Level(Level.cLevelName, &Level);

  kprintf(1, "RunMenuLevelStats2");

  if (Level.Level_Exit == 1)
    RunMenuLevelStats2("Mmstats3D.txt", p_ad, &Level, &am);

  kprintf(1, "lsi_Get_Save_Info");
  if (Level.Level_Exit == 4)
    lsi_Get_Save_Info(Level.cLevelName, &iActualLevel, &iActualScene);

  if (Level.Level_Exit == 3 || Level.Level_Exit == 4) {
    kprintf(1, "RunMenuLoadScreen2");
    RunMenuLoadScreen2();
    kprintf(1, "RunMenuLoadScreenInitBar");
    RunMenuLoadScreenInitBar(14);
  }

  //kprintf(1,"demo_Release");
  kprintf(1, "am_Do_Street_Lights_Release");
  am_Do_Street_Lights_Release(&Level);
  kprintf(1, "am_Do_Swamp_Lights_Release");
  am_Do_Swamp_Lights_Release(&Level);
  kprintf(1, "am_Do_Star_Lights_Release");
  am_Do_Star_Lights_Release(&Level);
  kprintf(1, "adas_Release_Source");
  adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
  adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE);

  kprintf(1, "adas_Is_OGG_Stream_Playing");
  if (Level.Level_Exit != 5)

	if(ogg_playing())
	{
		float f = p_ad->Music_Gain;
		while(f >= 0.05f)
		{
			f -= 0.05f;
			ogg_gain(f);
			Sleep(25);
		}
	}

  kprintf(1, "ap_Stop_Song");
  ap_Stop_Song(&ad);
  kprintf(1, "demo_Release");
  demo_Release(&Demo);
  kprintf(1, "adas_Set_Listener_Environment");
  adas_Set_Listener_Environment(EAX_ENVIRONMENT_GENERIC);
  kprintf(1, "am_Stop_Beetle_Animations");
  am_Stop_Beetle_Animations(&Level);
  kprintf(1, "am_Release_Animate_Items");
  am_Release_Animate_Items(&am, &Level);
  kprintf(1, "am_Release");
  am_Release(&am, &Level);
  kprintf(1, "free((void *) Level.pMaterialb)");
  free((void *) Level.pMaterialb);
  kprintf(1, "w_Release_Water_Boundaries");
  w_Release_Water_Boundaries();
  kprintf(1, "am_Release_Steps");
  am_Release_Steps(&Level);
  kprintf(1, "lsi_Release_Level");
  lsi_Release_Level(&Level);
  kprintf(1, "lsi_Release_Items");
  lsi_Release_Items(&Level);
  kprintf(1, "fn_Release_3d_Textures_Full");
  fn_Release_3d_Textures_Full();
  kprintf(1, "_3d_Release_Kuk");
  _3d_Release_Kuk();
  kprintf(1, "kom_zpruhlednovat_prvky");
  kom_zpruhlednovat_prvky(0);

  kprintf(1, "kom_zrus_level");

  if (Level.Level_Exit == 3)
    kom_zrus_level(1);
  else
    kom_zrus_level(0);

  kprintf(1, "_3d_Release_Display_Lists");
  if (!no_Menu)
    _3d_Release_Display_Lists();

  if (Level.Level_Exit == 3 || Level.Level_Exit == 4) {
    kprintf(1, "RunMenuLoadScreenAddProgress");
    RunMenuLoadScreenAddProgress(-1);
    kprintf(1, "RunMenuLoadScreenDrawProgress");
    RunMenuLoadScreenDrawProgress(-1, -1);
  }

  iRestart++;

  if (Level.Level_Exit == 3) {
    kprintf(1, "goto PLAY_LEVEL_START ");
    bRestart = 1;
    //kom_prvnotni_init();RUN_LEVEL

    if (!strcmp(p_Env_Name, "LOAD_GAME")) {
      strcpy(p_Env_Name, "RUN_LEVEL");
      strcpy(Level.cLevelName, Level.cLoadedFrom);
      strcpy(p_Level_Name, Level.cLevelName);
    }

    goto PLAY_LEVEL_START;
  }

  if (Level.Level_Exit == 4) {
    strcpy(p_Level_Name, Level.cLevelName);
    strcat(p_Level_Name, ".lv6");
    strcpy(p_Env_Name, "LOAD_GAME");
    iRestart = 0;

    //kom_prvnotni_init();
    kprintf(1, "goto PLAY_LEVEL_START ");
    goto PLAY_LEVEL_START;
  }

  Return = Level.Level_Exit;

  kprintf(1, "adas_Release_Loaded_Data");
  adas_Release_Loaded_Data();
  kprintf(1, "ap_Load_Sound_List");
  ap_Load_Sound_List(p_ad, "basicset.dat", 0);

  if (Level.Level_Exit == 3 || Level.Level_Exit == 4) {
    kprintf(1, "RunMenuLoadScreenRelease");
    RunMenuLoadScreenRelease(3);
  }

  adas_Set_Listener_Environment(EAX_ENVIRONMENT_GENERIC);

  if (Level.Level_Exit == 5)
    exit(0);

  return Return;
}

void gl_Kofola_Minimalize(void)
{
  Level.bGameResume = 0;
  kprintf(1, "game suspended");
  adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
  adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE);

	if(ogg_playing())
		ap_Stop_Song(&ad);

  adas_Set_Listener_Environment(EAX_ENVIRONMENT_GENERIC);
}

void gl_Kofola_Maximalize(void)
{
  Level.bGameResume = 1;
  kprintf(1, "game resumed");
//      ap_Play_Song(0,1,&ad);
//      as_Start(Level.Environment.id, &ad, &Level);
//      adas_Set_Listener_Environment(Level.Environment.room);
}

int gl_Allow_Key(int iScanCode)
{
  int i = demo_Virual2Fuction(iScanCode, &control);

  if (!demo)
    return 1;

  if (i >= 10 && i <= 24)
    return 1;

  if (i >= 38 && i <= 42)
    return 1;

  if (i >= 44 && i <= 47)
    return 1;

  //komatova vyjimka
  if (iScanCode == K_PLUS || iScanCode == K_MINUS)
    return 1;

  return 0;
}

void gl_Kofola_End(int DirectX)
{
  if (!DirectX) {
    kprintf(1, "am_Do_Street_Lights_Release");
    am_Do_Street_Lights_Release(&Level);
    kprintf(1, "am_Do_Swamp_Lights_Release");
    am_Do_Swamp_Lights_Release(&Level);
    kprintf(1, "am_Do_Star_Lights_Release");
    am_Do_Star_Lights_Release(&Level);
    kprintf(1, "adas_Release_Source");

    kprintf(1, "demo_Release");
    demo_Release(&Demo);
    kprintf(1, "adas_Set_Listener_Environment");
    adas_Set_Listener_Environment(EAX_ENVIRONMENT_GENERIC);
    kprintf(1, "am_Stop_Beetle_Animations");
    am_Stop_Beetle_Animations(&Level);
    kprintf(1, "am_Release_Animate_Items");
    am_Release_Animate_Items(&am, &Level);
    kprintf(1, "am_Release");
    am_Release(&am, &Level);
    kprintf(1, "free((void *) Level.pMaterialb)");
    free((void *) Level.pMaterialb);
    kprintf(1, "w_Release_Water_Boundaries");
    w_Release_Water_Boundaries();
    kprintf(1, "am_Release_Steps");
    am_Release_Steps(&Level);
    kprintf(1, "lsi_Release_Level");
    lsi_Release_Level(&Level);
    kprintf(1, "lsi_Release_Items");
    lsi_Release_Items(&Level);
    kprintf(1, "fn_Release_3d_Textures_Full");
    fn_Release_3d_Textures_Full();
    kprintf(1, "_3d_Release_Kuk");
    _3d_Release_Kuk();
    kprintf(1, "kom_zpruhlednovat_prvky");
    kom_zpruhlednovat_prvky(0);

    kprintf(1, "kom_zrus_level");
    kom_zrus_level(0);

    kprintf(1, "_3d_Release_Display_Lists");
    _3d_Release_Display_Lists();

    //kprintf(1, "adas_Release_Loaded_Data");
    //adas_Release_Loaded_Data();

    kprintf(1, "_3d_Release_Hints");
    _3d_Release_Hints(pHintTexture, 26);

    kprintf(1, "_3d_Release");
    _3d_Release();
  }
  else {
    ddxRelease();
    FreeDirectDraw();
  }

  kprintf(1, "apakclose pGDataArchive");
  apakclose(pGDataArchive);
  kprintf(1, "apakclose pDataArchive");
  apakclose(pDataArchive);
  kprintf(1, "apakclose pBmpArchive");
  apakclose(pBmpArchive);
  kprintf(1, "apakclose pSndArchive");
  apakclose(pSndArchive);
  kprintf(1, "apakclose p3DMArchive");
  apakclose(p3DMArchive);
  kprintf(1, "apakclose pControlsArchive");
  apakclose(pControlsArchive);

  //kprintf(1, "apakclose FONT");
  //fn_Release_Font();

  adas_Set_Listener_Environment(EAX_ENVIRONMENT_GENERIC);
  adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
  adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 

	kprintf(1, "ap_Stop_Song");
	ap_Stop_Song(&ad);
	kprintf(1, "gi_Release_Sound_Engine");
	gi_Release_Sound_Engine();
	kprintf(1, "ap_Release_Play_List");
	ap_Release_Play_List(&ad);
	kprintf(1, "ap_Release_Material_List");
	ap_Release_Material_List(&ad);
	kprintf(1, "ap_Release");
	ap_Release(&ad);

//      ChangeDisplaySettings(NULL,0);
//      ShowWindow(hwnd_hry, SW_MAXIMIZE);
  spracuj_spravy(0);
  ShowCursor(TRUE);
  spracuj_spravy(0);

//      DestroyWindow(hwnd_hry);
  exit(0);
}
