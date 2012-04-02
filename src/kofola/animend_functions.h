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

void anmend_PadBedny(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_ExplozeBedny(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Lift(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Steps(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_StepsSikmina(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Steps2(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Water(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Cakanec(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Item_Fall(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Item_FallStartAnim(size_ptr param, size_ptr param2, size_ptr param3);
void animend_Lift_End(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Send_Event(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_ExplozeBednyZaSikminou(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Set_Flek_Flag(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Take_Item(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Kamen(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_ZrusCastice(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_ZrusCastice2(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_ZrusCastice3(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Are_Animations_Done(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Tlacitko(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Teleport(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_TeleportStart(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Add_Beetle_Animation(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_WaterLift(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Exit(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_kom_mesh_set_mesh(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_kom_mesh_set_mesh(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Play_Sunuti(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Set_Flek_Flag_Anim(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_Lift_Item(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_KamenZaSikmonou(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_PlayKrumpac(size_ptr param, size_ptr param2, size_ptr param3);
void anmend_kom_mesh_set_meshK(size_ptr param, size_ptr param2, size_ptr param3);

#endif
