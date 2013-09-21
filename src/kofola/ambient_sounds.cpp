//------------------------------------------------------------------------------------------------
// version 0.0.1
//------------------------------------------------------------------------------------------------
#include "compat_mini.h"
#include "adas.h"
#include "audio_plug-in.h"
#include "ambient_sounds.h"

#include <math.h>

#include "3d_all.h"
#include "Object.h"

static long amb_counter[3];

void as_Start_Env23(int i, AUDIO_DATA * p_ad, float *pos, char bRelative)
{
  int r = rand() % 2;

  ap_Play_Sound(2, bRelative, 1, pos, i + r, NULL, p_ad);
}

//------------------------------------------------------------------------------------------------
// starts environment
//------------------------------------------------------------------------------------------------
void as_Start(int Env_ID, AUDIO_DATA * p_ad, void *p_Level)
{
  float pos[3] = { 0.0f, 0.0f, 0.0f };
  int source;
  LEVELINFO *p_L = (LEVELINFO *) p_Level;

  //EAX_DATA Eax;

  for (source = 0; source < 3; source++)
    amb_counter[source] = 0;

  if (p_L->Environment.bwall_occlusion)
    p_ad->Eax.wall_occlusion = p_L->Environment.wall_occlusion;
  else
    p_ad->Eax.wall_occlusion = UNDEFINED_VALUE;

  if (p_L->Environment.bwall_occlusion_room)
    p_ad->Eax.wall_occlusion_room = p_L->Environment.wall_occlusion_room;
  else
    p_ad->Eax.wall_occlusion_room = (float) UNDEFINED_VALUE;

  if (p_L->Environment.bwall_occlusionLF)
    p_ad->Eax.wall_occlusionLF = p_L->Environment.wall_occlusionLF;
  else
    p_ad->Eax.wall_occlusionLF = (float) UNDEFINED_VALUE;

  if (p_L->Environment.bout_room)
    p_ad->Eax.out_room = p_L->Environment.out_room;
  else
    p_ad->Eax.out_room = UNDEFINED_VALUE;

  if (p_L->Environment.bout_room_rolloff)
    p_ad->Eax.out_room_rolloff = p_L->Environment.out_room_rolloff;
  else
    p_ad->Eax.out_room_rolloff = (float) UNDEFINED_VALUE;

  if (p_L->Environment.bout_roomHF)
    p_ad->Eax.out_roomHF = p_L->Environment.out_roomHF;
  else
    p_ad->Eax.out_roomHF = UNDEFINED_VALUE;

  if (p_L->Environment.bout_air_absorbtion)
    p_ad->Eax.out_air_absorbtion = p_L->Environment.out_air_absorbtion;
  else
    p_ad->Eax.out_air_absorbtion = (float) UNDEFINED_VALUE;

  p_ad->Eax.Obstruction = UNDEFINED_VALUE;
  p_ad->Eax.ObstructionLF = (float) UNDEFINED_VALUE;

  switch (Env_ID) {
    case 0:
      ap_Play_Sound(2, 1, 1, pos, 4, &(p_ad->Eax), p_ad);
      break;
    case 2:
      as_Start_Env23(90, p_ad, pos, 1);
      break;
    case 3:
      ap_Play_Sound(2, 1, 1, pos, (rand() % 3) + 111, NULL, p_ad);
      break;
    case 4:
      {
        pos[0] = 10;
        pos[1] = 0;
        pos[2] = -40;
        as_Start_Env23(92, p_ad, pos, 0);

        p_ad->Eax.Obstruction = p_ad->Eax.wall_occlusion;
        p_ad->Eax.ObstructionLF = p_ad->Eax.wall_occlusionLF;

        p_ad->Eax.wall_occlusion = UNDEFINED_VALUE;
        p_ad->Eax.wall_occlusionLF = (float) UNDEFINED_VALUE;
        p_ad->Eax.wall_occlusion_room = (float) UNDEFINED_VALUE;
      }
      break;
    case 10:
      ap_Play_Sound(2, 1, 1, pos, 4, &(p_ad->Eax), p_ad);
      break;
    case 9:
      as_Start_Env23(129, p_ad, pos, 1);
      as_Start_Env23(131, p_ad, pos, 1);
      break;
    case 11:
      ap_Play_Sound(2, 1, 1, pos, 128, &(p_ad->Eax), p_ad);
      break;
    case 13:
      ap_Play_Sound(2, 1, 1, pos, 150, &(p_ad->Eax), p_ad);
      break;
  }
}

//------------------------------------------------------------------------------------------------
// Thunder
//------------------------------------------------------------------------------------------------
void as_Thunder(AUDIO_DATA * p_ad, void *p_Level)
{
  int index, rnd;
  float pos[3];

//      LEVELINFO *p_L = (LEVELINFO *)p_Level;

  index = rand() % 4;

  pos[0] = (float) (rand() % 500);
  rnd = rand() % 2;
  if (rnd)
    pos[0] *= -1;

  pos[1] = (float) (rand() % 500);

  pos[2] = (float) (rand() % 500);
  rnd = rand() % 2;
  if (rnd)
    pos[0] *= -1;

  ap_Play_Sound(1, 0, 1, pos, index, &(p_ad->Eax), p_ad);
}

//------------------------------------------------------------------------------------------------
// Bird
//------------------------------------------------------------------------------------------------
void as_Bird(int Bird, AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd;
  float pos[3];

//      LEVELINFO *p_L = (LEVELINFO *)p_Level;

  pos[0] = (float) (rand() % 200);
  rnd = rand() % 2;
  if (rnd)
    pos[0] *= -1;

  pos[1] = (float) (rand() % 20);

  pos[2] = (float) (rand() % 200);
  rnd = rand() % 2;
  if (rnd)
    pos[0] *= -1;

  ap_Play_Sound(1, 0, 1, pos, Bird, &(p_ad->Eax), p_ad);
}

//------------------------------------------------------------------------------------------------
// Rat
//------------------------------------------------------------------------------------------------
void as_Rat(int Rat, AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd;
  float pos[3];

  pos[0] = (float) (rand() % 20);
  rnd = rand() % 2;
  if (rnd)
    pos[0] *= -1;

  pos[1] = 0;

  pos[2] = (float) (rand() % 20);
  rnd = rand() % 2;
  if (rnd)
    pos[0] *= -1;

  ap_Play_Sound(1, 0, 1, pos, Rat, NULL, p_ad);
}

//------------------------------------------------------------------------------------------------
// Thunder
//------------------------------------------------------------------------------------------------
void as_Bell(AUDIO_DATA * p_ad, void *p_Level)
{
  float pos[3] = { -50.0f, 5.0f, 20.0f };
//      LEVELINFO *p_L = (LEVELINFO *)p_Level;

  amb_counter[0]++;
  if (amb_counter[0] > 2400) {
    amb_counter[0] = 0;
    amb_counter[1]++;
  }

  if (amb_counter[1]) {
    if (amb_counter[1] > 5) {
      amb_counter[1] = 0;
      amb_counter[2] = 0;
    }
    amb_counter[2]++;
    if (amb_counter[2] > 12) {
      amb_counter[2] = 0;
      amb_counter[1]++;
      ap_Play_Sound(1, 0, 1, pos, 13, &(p_ad->Eax), p_ad);
    }
  }
}

//------------------------------------------------------------------------------------------------
// Bird
//------------------------------------------------------------------------------------------------
void as_Car(int Car, AUDIO_DATA * p_ad, void *p_Level, float fPos2)
{
  float pos[3];

  pos[0] = 0.0f;

  pos[1] = 0.0f;
  pos[2] = fPos2;

  ap_Play_Sound(1, 1, 1, pos, Car, &(p_ad->Eax), p_ad);
}

void as_Car4(int Car, AUDIO_DATA * p_ad, void *p_Level)
{
  float pos[3];

  pos[0] = 0.0f;

  pos[1] = 200.0f;
  pos[2] = 0.0f;

  ap_Play_Sound(1, 1, 1, pos, Car, &(p_ad->Eax), p_ad);
}

//------------------------------------------------------------------------------------------------
//pokoj + balkon
//------------------------------------------------------------------------------------------------
void ad_Do_Environment1(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 5;

  if (rnd < 10)
    as_Thunder(p_ad, p_Level);

  rnd = rand() % 400;

  if (rnd < 7)
    switch (rnd1) {
      case 0:
        as_Bird(8, p_ad, p_Level);
        break;
      case 1:
        as_Bird(9, p_ad, p_Level);
        break;
      case 2:
        as_Bird(10, p_ad, p_Level);
        break;
      case 3:
        as_Bird(11, p_ad, p_Level);
        break;
      case 4:
        as_Bird(12, p_ad, p_Level);
        break;
    }

  as_Bell(p_ad, p_Level);

  rnd = rand() % 400;

  if (rnd < 10)
    switch (rnd1) {
      case 0:
        as_Car(5, p_ad, p_Level, -1000.0f);
        break;
      case 1:
        as_Car(6, p_ad, p_Level, -1000.0f);
        break;
      case 2:
        as_Car(7, p_ad, p_Level, -1000.0f);
        break;
      case 3:
        as_Car(138, p_ad, p_Level, -1000.0f);
        break;
    }
}

void ad_Do_Environment2(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 5;

  if (rnd < 3)
    as_Thunder(p_ad, p_Level);

  as_Bell(p_ad, p_Level);

  rnd = rand() % 400;

  if (rnd < 10)
    switch (rnd1) {
      case 0:
        as_Car(5, p_ad, p_Level, -1000.0f);
        break;
      case 1:
        as_Car(6, p_ad, p_Level, -1000.0f);
        break;
      case 2:
        as_Car(7, p_ad, p_Level, -1000.0f);
        break;
      case 3:
        as_Car(138, p_ad, p_Level, -1000.0f);
        break;
    }
}

void ad_Do_Environment2b(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 5;

  as_Bell(p_ad, p_Level);

  if (rnd < 15)
    switch (rnd1) {
      case 0:
        as_Car(5, p_ad, p_Level, -1000.0f);
        break;
      case 1:
        as_Car(6, p_ad, p_Level, -1000.0f);
        break;
      case 2:
        as_Car(7, p_ad, p_Level, -1000.0f);
        break;
      case 3:
        as_Car(138, p_ad, p_Level, -1000.0f);
        break;
    }
}

//------------------------------------------------------------------------------------------------
//pokoj + balkon
//------------------------------------------------------------------------------------------------
void ad_Do_Environment5(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd;
  int rnd1 = rand() % 5;

  as_Bell(p_ad, p_Level);

  rnd = rand() % 400;

  if (rnd < 10)
    switch (rnd1) {
      case 0:
        as_Car(5, p_ad, p_Level, -1000.0f);
        break;
      case 1:
        as_Car(6, p_ad, p_Level, -1000.0f);
        break;
      case 2:
        as_Car(7, p_ad, p_Level, -1000.0f);
        break;
      case 3:
        as_Car(138, p_ad, p_Level, -1000.0f);
        break;
    }
}

//------------------------------------------------------------------------------------------------
// zima (tutorial)
//------------------------------------------------------------------------------------------------
void ad_Do_Environment3(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd;
  int rnd1 = rand() % 3;

  rnd = rand() % 400;

  if (rnd < 7)
    switch (rnd1) {
      case 0:
        as_Bird(105, p_ad, p_Level);
        break;
      case 1:
        as_Bird(106, p_ad, p_Level);
        break;
      case 2:
        as_Bird(107, p_ad, p_Level);
        break;
    }
}

void ad_Do_Environment6(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd;
  int rnd1 = rand() % 5;

  rnd = rand() % 400;

  if (rnd < 7)
    switch (rnd1) {
      case 0:
        as_Bird(8, p_ad, p_Level);
        break;
      case 1:
        as_Bird(9, p_ad, p_Level);
        break;
      case 2:
        as_Bird(10, p_ad, p_Level);
        break;
      case 3:
        as_Bird(11, p_ad, p_Level);
        break;
      case 4:
        as_Bird(12, p_ad, p_Level);
        break;
    }

  as_Bell(p_ad, p_Level);
}

void ad_Do_Environment3a(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd;
  int rnd1 = rand() % 3;

  rnd = rand() % 1000;

  if (rnd < 7)
    switch (rnd1) {
      case 0:
        as_Bird(108, p_ad, p_Level);
        break;
      case 1:
        as_Bird(109, p_ad, p_Level);
        break;
      case 2:
        as_Bird(110, p_ad, p_Level);
        break;
    }

}

//------------------------------------------------------------------------------------------------
// kanal
//------------------------------------------------------------------------------------------------
void ad_Do_Environment4(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 3;

  if (rnd < 10)
    as_Rat(87 + rnd1, p_ad, p_Level);

  rnd = rand() % 400;

  if (rnd < 10)
    as_Rat(84 + rnd1, p_ad, p_Level);

  rnd = rand() % 400;

  if (rnd < 10)
    as_Car4(5 + rnd1, p_ad, p_Level);

  //as_Bell(p_ad,p_Level);

  return;
}

void ad_Do_Environment9(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 5;

  as_Bell(p_ad, p_Level);

  if (rnd < 25)
    switch (rnd1) {
      case 0:
        as_Car(5, p_ad, p_Level, -500.0f);
        break;
      case 1:
        as_Car(6, p_ad, p_Level, -500.0f);
        break;
      case 2:
        as_Car(7, p_ad, p_Level, -500.0f);
        break;
      case 3:
        as_Car(138, p_ad, p_Level, -500.0f);
        break;
    }
}

void ad_Do_Environment10(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 5;

  as_Bell(p_ad, p_Level);

  if (rnd < 10)
    as_Thunder(p_ad, p_Level);

  if (rnd < 15)
    switch (rnd1) {
      case 0:
        as_Car(5, p_ad, p_Level, -250.0f);
        break;
      case 1:
        as_Car(6, p_ad, p_Level, -250.0f);
        break;
      case 2:
        as_Car(7, p_ad, p_Level, -250.0f);
        break;
      case 3:
        as_Car(138, p_ad, p_Level, -250.0f);
        break;
    }
}

void ad_Do_Environment11(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd;
  int rnd1 = rand() % 5;

  rnd = rand() % 400;

  if (rnd < 65)
    switch (rnd1) {
      case 0:
        as_Bird(133, p_ad, p_Level);
        break;
      case 1:
        as_Bird(134, p_ad, p_Level);
        break;
      case 2:
        as_Bird(135, p_ad, p_Level);
        break;
      case 3:
        as_Bird(136, p_ad, p_Level);
        break;
      case 4:
        as_Bird(137, p_ad, p_Level);
        break;
    }
}

//------------------------------------------------------------------------------------------------
//pokoj + balkon
//------------------------------------------------------------------------------------------------
void ad_Do_Environment12(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 5;

  if (rnd < 7)
    switch (rnd1) {
      case 0:
        as_Bird(8, p_ad, p_Level);
        break;
      case 1:
        as_Bird(9, p_ad, p_Level);
        break;
      case 2:
        as_Bird(10, p_ad, p_Level);
        break;
      case 3:
        as_Bird(11, p_ad, p_Level);
        break;
      case 4:
        as_Bird(12, p_ad, p_Level);
        break;
    }
}

//------------------------------------------------------------------------------------------------
//pokoj + balkon
//------------------------------------------------------------------------------------------------
void ad_Do_Environment13(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 5;

  if (rnd < 15)
    switch (rnd1) {
      case 0:
        as_Bird(140, p_ad, p_Level);
        break;
      case 1:
        as_Bird(141, p_ad, p_Level);
        break;
      case 2:
        as_Bird(142, p_ad, p_Level);
        break;
      case 3:
        as_Bird(143, p_ad, p_Level);
        break;
      case 4:
        as_Bird(144, p_ad, p_Level);
        break;
    }
}

//------------------------------------------------------------------------------------------------
//pokoj + balkon
//------------------------------------------------------------------------------------------------
void ad_Do_Environment14(AUDIO_DATA * p_ad, void *p_Level)
{
  int rnd = rand() % 400;
  int rnd1 = rand() % 10;

  if (rnd < 15)
    switch (rnd1) {
      case 0:
        as_Bird(151, p_ad, p_Level);
        break;
      case 1:
        as_Bird(152, p_ad, p_Level);
        break;
      case 2:
        as_Bird(153, p_ad, p_Level);
        break;
      case 3:
        as_Bird(154, p_ad, p_Level);
        break;
      case 4:
        as_Bird(155, p_ad, p_Level);
        break;
      case 5:
        as_Bird(156, p_ad, p_Level);
        break;
      case 6:
        as_Bird(157, p_ad, p_Level);
        break;
      case 7:
        as_Bird(158, p_ad, p_Level);
        break;
      case 8:
        as_Bird(159, p_ad, p_Level);
        break;
      case 9:
        as_Bird(160, p_ad, p_Level);
        break;
    }
}

//------------------------------------------------------------------------------------------------
// starts environment
//------------------------------------------------------------------------------------------------
void as_Do_Ambient_Sounds(int Env_ID, AUDIO_DATA * p_ad, void *p_Level)
{
  if (!p_ad->bAudio)
    return;

  switch (Env_ID) {
    case 0:
      ad_Do_Environment1(p_ad, p_Level);
      break;
    case 1:
      ad_Do_Environment2(p_ad, p_Level);
      break;
    case 2:
      ad_Do_Environment3(p_ad, p_Level);
      break;
    case 3:
      ad_Do_Environment3a(p_ad, p_Level);
      break;
    case 4:
      ad_Do_Environment4(p_ad, p_Level);
      break;
    case 5:
      ad_Do_Environment5(p_ad, p_Level);
      break;
    case 6:
      ad_Do_Environment6(p_ad, p_Level);
      break;
    case 8:
      ad_Do_Environment2b(p_ad, p_Level);
      break;
    case 9:
      ad_Do_Environment9(p_ad, p_Level);
      break;
    case 10:
      ad_Do_Environment10(p_ad, p_Level);
      break;
    case 11:
      ad_Do_Environment11(p_ad, p_Level);
      break;
    case 12:
      ad_Do_Environment12(p_ad, p_Level);
      break;
    case 13:
      ad_Do_Environment13(p_ad, p_Level);
      break;
    case 14:
      ad_Do_Environment14(p_ad, p_Level);
      break;
  }
}
