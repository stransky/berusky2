#ifndef __ANMEND_
#define __ANMEND_

#include "Object.h"

typedef struct
{
  LEVELINFO *p_Level;
  void *pParam;
  int iParam;
  char bParam;
  char bRCamera;
  int viParam1[3];
  int viParam2[3];
} POINTERSTRUCTURE;

void anmend_PadBedny(int param, int param2, void *pParam);
void anmend_ExplozeBedny(int param, int param2, void *pParam);
void anmend_Lift(int param, int param2, void *p_param);
void anmend_Steps(int param, int param2, void *p_param);
void anmend_StepsSikmina(int param, int param2, void *p_param);
void anmend_Steps2(int param, int param2, void *p_param);
void anmend_Water(int param, int param2, void *p_param);
void anmend_Cakanec(int param, int param2, void *p_param);
void anmend_Item_Fall(int param, int param2, void *p_param);
void anmend_Item_FallStartAnim(int param, int param2, void *p_param);
void animend_Lift_End(int param, int param2, void *p_param);
void anmend_Send_Event(int param, int param2, void *p_param);
void anmend_ExplozeBednyZaSikminou(int param, int param2, void *p_param);
void anmend_Set_Flek_Flag(int param, int param2, void *p_param);
void anmend_Take_Item(int param, int param2, void *p_param);
void anmend_Kamen(int param, int param2, void *p_param);
void anmend_ZrusCastice(int param, int param2, void *p_param);
void anmend_ZrusCastice2(int param, int param2, void *p_param);
void anmend_ZrusCastice3(int param, int param2, void *p_param);
void anmend_Are_Animations_Done(int param, int param2, void *p_param);
void anmend_Tlacitko(int param, int param2, void *p_param);
void anmend_Teleport(int param, int param2, void *p_param);
void anmend_TeleportStart(int param, int param2, void *p_param);
void anmend_Add_Beetle_Animation(int param, int param2, void *p_param);
void anmend_WaterLift(int param, int param2, void *p_param);
void anmend_Exit(int param, int param2, void *p_param);
void anmend_kom_mesh_set_mesh(int param, int param2, void *p_param);
void anmend_Play_Sunuti(int param, int param2, void *p_param);
void anmend_Set_Flek_Flag_Anim(int param, int param2, void *p_param);
void anmend_Lift_Item(int param, int param2, void *p_param);
void anmend_KamenZaSikmonou(int param, int param2, void *p_param);
void anmend_PlayKrumpac(int param, int param2, void *p_param);
void anmend_kom_mesh_set_meshK(int param, int param2, void *p_param);

#endif
