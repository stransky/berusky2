#ifndef __GAMA_LOGIC_
#define __GAMA_LOGIC_

#include "3d_all.h"
#include "Object.h"

	// spusti level
	int gl_Run_Level(char *p_Level_Name, char *p_Env_Name, HWND hWnd, AUDIO_DATA *p_ad, int iCPU);
	
	// set_ frame rate
	void gl_Set_Frame_Rate(void);

	int gl_Throw_off(int *column, LEVELINFO *p_Level);
	int gl_Throw_offAnim(int *column, LEVELINFO *p_Level);

	void gl_Destroy_Item(long Item, int Type, LEVELINFO *p_Level);

	void gl_Logical2Real(int x, int y, int z, int *Real_Pos, LEVELINFO *p_Level);
	void gl_Next_Queue_Set(LEVELINFO *p_Level);
	void gl_Change_Destonation(LEVELINFO *p_Level, int *pos);
	void gl_i2FStr(int i, char *cStr);
	void gl_Pripoj_Flek_k_Predmenu(ITEMDESC *pItem, LEVELINFO *p_Level);
	void gl_Do_Strepiny(float *pos, LEVELINFO *p_Level, int material, char bSvetlo);
	int gl_Get_Mesh_Material(int Mesh);
	void gl_Do_Kameni(float *pos, LEVELINFO *p_Level, int material);
	void gl_Go_Animations(void);
	int gl_Check_Logic_Dependences(LEVELINFO *p_Level);
	int gl_Are_Animations_Done(LEVELINFO *p_Level);
	void gl_Do_Prach(float *pos, LEVELINFO *p_Level);
	void gl_Do_Teleport_Sparks(float *pos, LEVELINFO *p_Level);
	int gl_Do_Teleport_Light(float *pos, LEVELINFO *p_Level, int iFlare);
	int gl_Do_Teleport_Flare(float *pos, LEVELINFO *p_Level);
	void gl_Do_Lift_Particles(float *pos, int mesh, LEVELINFO *p_Level);
	int gl_Check_Anim_Pontonky(ITEMDESC *pItem, LEVELINFO *p_Level);
	void gl_Do_Krompac(float *pos, LEVELINFO *p_Level, int material);
	char gl_Check_Mesh_Sim(int iMesh);

#endif
