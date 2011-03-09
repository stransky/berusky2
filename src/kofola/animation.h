#ifndef __ANIMATION_INCLUDE_
#define __ANIMATION_INCLUDE_

#include <windows.h>
#include "audio_plug-in.h"
#include "2d_graphic.h"

typedef struct
{
	void		*pParam;
	int			iParam[2];
	END_FUNKCE	pProc;
	int			flag;
} ANIM_TRIGER;

typedef struct
{
	int			flag;
	int			p_run;
	int			p_matrix;
	int			p_anim;
	int			mesh[8];
	char		bnext;
	ANIM_TRIGER	tStart[2];
	ANIM_TRIGER	tTriger[5];
	ANIM_TRIGER	taMateral[3];
	char		bconnected;
} SIM_ANIMATION;

typedef struct
{
	int				sim_anim[110];
	SIM_ANIMATION	*p_animation;
	int				Size_of_Anim;
	HWND			hWnd;
	AUDIO_DATA		*p_ad;
	float			frame_per_quater;
} ANIMATION_MODULE;

	//init animation module
	int	am_Init(ANIMATION_MODULE *p_am, LEVELINFO *p_Level);

	// release animation module
	void am_Release(ANIMATION_MODULE *p_am, LEVELINFO *p_Level);

	// get animaton status
	int am_Animation_Status(SIM_ANIMATION *p_animation, ANIMATION_MODULE *p_am);

	void am_Set_Start_Function(SIM_ANIMATION *p_animation, END_FUNKCE pStartf,
							   int iParam1, int iParam2, void *pParam, int index);

	void am_Set_Triger_Function(SIM_ANIMATION *p_animation, END_FUNKCE pTrigerf,
						   int iParam1, int iParam2, void *pParam, int flag, int index);

	void am_Set_aMaterial_Function(SIM_ANIMATION *p_animation, int i, END_FUNKCE taMateral,
						           int iParam1, int iParam2, void *pParam, int flag);

	void am_Animate_Items(ANIMATION_MODULE *p_am, LEVELINFO *p_Level);
	void am_Remove_Animate_item(long item, LEVELINFO *p_Level);
	void am_Remove_Animate_itemB(int iItem, LEVELINFO *p_Level);
	void am_Add_Animate_Item(long item, LEVELINFO *p_Level);
	void am_Add_Animate_ItemB(int iItem, LEVELINFO *p_Level);
	void am_Release_Animate_Items(ANIMATION_MODULE *p_am, LEVELINFO *p_Level);
	void am_Stop_Beetle_Animations(LEVELINFO *p_Level);
	void am_Set_aMaterial_Trigers(SIM_ANIMATION *p_animation, ITEMDESC *p_Item, LEVELINFO *p_Level);
	void am_Animate_Beetles(LEVELINFO *p_Level);
	void am_Remove_Beetle_Animation(int mesh, LEVELINFO *p_Level);
	int am_Add_Beetle_Animation(int mesh, LEVELINFO *p_Level, int iRot, char bExeption);

	void am_Init_Zhave_Castice(LEVELINFO *p_Level);
	void am_Do_Zhave_castice(LEVELINFO *p_Level);
	void am_Release_Zhave_castice(SYSTEMZHAVYCHCASTIC *pSystem);
	void am_Release_Kourove_Castice(SYSTEMKOUROVYCHCASTIC *pSystem);

	void am_Change_Wind(LEVELINFO *p_Level);
	void am_Do_Flares(LEVELINFO *p_Level);

	void am_Do_Lifts(LEVELINFO *p_Level);

	int am_Kola_na_Vode(float *pos, int VyskaSloupce, int VyskaPadu, int predmet, LEVELINFO *p_Level);
	void am_Do_Water(LEVELINFO *p_Level);
	void am_Do_Vodni_Cakanec1(float *pos, int vyska, int Predmet, LEVELINFO *p_Level);
	void am_Do_Vodni_Cakanec2(float *pos, int VyskaPadu, int predmet, LEVELINFO *p_Level);

	void am_Do_Lift_VParticles(float *pos, int mesh, LEVELINFO *p_Level);

	void am_Do_Water_KolaB(LEVELINFO *p_Level);
	int am_Kola_na_VodeB(float *pos, int Beruska, int mesh, int infinity, LEVELINFO *p_Level);

	void am_Do_Bubliny_Berusek(LEVELINFO *p_Level);

	void am_Do_Vybuch_Bublin(int *iPos, float *fPos, LEVELINFO *p_Level);

	void am_Do_BublVybuchy(LEVELINFO *p_Level);
	void am_Do_Exit(int Bmesh, int Emesh, LEVELINFO *p_Level);
	void am_Do_Exit_Efects(LEVELINFO *p_Level);
	void am_Create_Sparks_To_Exits(LEVELINFO *p_Level);
	void am_Do_Exit_Sparks(LEVELINFO *p_Level);

	void am_Do_TelCSparks(LEVELINFO *p_Level);
	void am_TurnOn_The_Teleport(ITEMDESC *pTel, LEVELINFO *p_Level);
	void am_TurnOff_The_Teleport(ITEMDESC *pTel, LEVELINFO *p_Level);

	int am_Create_Rain(LEVELINFO *p_Level, RAINSYSTEM *pRain, float fRadius, 
					   unsigned int uiDensity, unsigned int uiIntensity,  
					   float fmaxDivergence, float fSky);
	void am_Do_Rain(RAINSYSTEM *pRain);

	int am_Create_Snow(LEVELINFO *p_Level, SNOWSYSTEM *pSnow, float fRadius, 
					   unsigned int uiDensity, unsigned int uiIntensity, 
					   float fmaxDivergence, float	fSky);
	void am_Do_Snow(SNOWSYSTEM *pSnow);

	int am_Create_Nature_Effect(LEVELINFO *p_Level, int iEffectID, float fRadius, 
								unsigned int uiDensity, unsigned int uiIntensity, 
								float fSky, float fLowPlane);
	void am_Do_Nature_Effects(LEVELINFO *p_Level);

	void am_Create_Water_Effect(LEVELINFO *p_Level);

	void am_Create_Prizemni_Mlhu(LEVELINFO *p_Level, float fDensity, float fHeight);

	void am_Start_Animace_Pontonek(LEVELINFO *p_Level);
	void am_Start_Animaci_Pontonky(ITEMDESC *pItem);
	void am_Start_Barels(LEVELINFO *p_Level);
	void am_Do_BarelSparks(LEVELINFO *p_Level);
	void am_Release_BarelSparks(LEVELINFO *p_Level, ITEMDESC *pBarel);

	void am_Do_BeatleSmokes(LEVELINFO *p_Level);
	void am_Obsluha_Koure_Brouku(LEVELINFO *p_Level);
	void am_Create_Steps_System(LEVELINFO *p_Level);
	void am_Set_Step(int iFoot, LEVELINFO *p_Level);
	void am_Release_Steps(LEVELINFO *p_Level);

	void am_Zero_Step(LEVELINFO *p_Level);
	void am_Do_Step(int param, int param2, void *p_param);

	void am_Destroy_Steps(ITEMDESC *pItem, LEVELINFO *p_Level);

	void am_Do_BublSystemE(LEVELINFO *p_Level);
	void am_Create_BublSystem_Effect1(float *pos, LEVELINFO *p_Level, int iSize);

	void am_Create_BublSystem_Effect2(float *pos, LEVELINFO *p_Level, int iSize, float yPlane);
	void am_Do_BublSystemC(LEVELINFO *p_Level);

	void am_Start_Joint_Animations(void);
	void am_Create_Street_Light(LEVELINFO *p_Level, int iStart, int iPause, float *pos);
	void am_Do_Street_Lights(LEVELINFO *p_Level);
	void am_Do_Street_Lights_Release(LEVELINFO *p_Level);

	void am_Do_RainB(RAINSYSTEM *pRain);
	int am_Create_Water_CirclesB(LEVELINFO *p_Level, RAINSYSTEM *pRain, float fRadius, 
								unsigned int uiDensity, unsigned int uiIntensity, 
								float fmaxDivergence, float *rPos);

	void am_Flip(LEVELINFO *p_Level, ANIMATION_MODULE *p_am, char bAll, RECT_LINE *rline, int rsize, int bTutor, int bTText, int ty);
	void am_FlipA(LEVELINFO *p_Level, ANIMATION_MODULE *p_am, char bAll, RECT_LINE *rline, int rsize, int bTutor, int bTText, int ty);

	void am_Start_Gen_Animation(char *cMesh, LEVELINFO *p_Level);

	void am_Create_Swamp_Light(LEVELINFO *p_Level, int iStart, int iPause, float *pos);
	void am_Do_Swamp_Lights(LEVELINFO *p_Level);
	void am_Do_Swamp_Lights_Release(LEVELINFO *p_Level);
	void am_Gen_Swamp_Lights(LEVELINFO *p_Level, RECT *r);

	int am_Create_Fairies(LEVELINFO *p_Level, RECT *r);
	void am_Do_Fairies(LEVELINFO *p_Level);

	void am_Do_Star_Lights_Release(LEVELINFO *p_Level);
	void am_Gen_Star_Lights(LEVELINFO *p_Level, float *pos);
	void am_Do_Star_Lights(LEVELINFO *p_Level);
	void am_Do_StarFall(LEVELINFO *p_Level);
	int am_Create_Falling_Stars(LEVELINFO *p_Level, float *r);

	int am_Create_CandleSystem(float *pos, LEVELINFO *p_Level);
	void am_Do_CandleSparks(LEVELINFO *p_Level);

	int am_Create_CandleSmokeSystem(float *pos, LEVELINFO *p_Level);
	void am_Do_CandleSmoke(LEVELINFO *p_Level);

	void am_Start_Voda_Kanal(void);

	void am_Correct_Beetle_Animation(int mesh, LEVELINFO *p_Level, int rot);

	void am_Set_Beetle_Switching(LEVELINFO *p_Level, int mesh, int bSwitch);

	void am_Create_Item_Lights(LEVELINFO *p_Level);
	
	void am_Start_Voda_Akvarko(void);

#endif