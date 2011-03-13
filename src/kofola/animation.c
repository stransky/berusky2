//------------------------------------------------------------------------------------------------
// version 1.0.1
//------------------------------------------------------------------------------------------------
#include <math.h>

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Berusky3d_kofola2d.h"
#include "game_logic.h"
#include "animation.h"
#include "animend_functions.h"
#include "water.h"
#include "3d_math.h"
#include "3D_graphic.h"
#include "menu_def.h"
#include "3D_menus.h"
#include "Apak.h"

#define randf()      ((float)rand())

extern APAK_HANDLE	*pDataArchive;
extern LEVELINFO			Level;
extern _3D_CURSOR _3dCur;
extern	G_KONFIG ber;
extern  AUDIO_DATA ad;
void am_Do_Vodni_Cakanec2(float *pos, int VyskaPadu, int predmet, LEVELINFO *p_Level);
int am_Find_Water(int i, float *f, LEVELINFO *p_Level);

extern int	gl_Choose_Wave_Index(int Plus);
extern void gl_Do_Strepiny_Na_Vode(float *pos, LEVELINFO *p_Level, float fycor);
static	int mx = 0;
static	int my = 0;

RECT	rDrawRect[DRAW_RECT_NUM];

float am_vzdal_bodu(float x1, float y1, float x2, float y2)
{
  return((float)sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
}

float am_vzdal_bodu_single(float x1, float x2)
{
  return((float)sqrt((x2-x1)*(x2-x1)));
}


//------------------------------------------------------------------------------------------------
// init animation module
//------------------------------------------------------------------------------------------------
int	am_Init(ANIMATION_MODULE *p_am, LEVELINFO *p_Level)
{
	FILE *file = NULL;
	char text[256];
	int i;

	pDataArchive->pActualNode = pDataArchive->pRootNode->pNextNode;

	p_am->Size_of_Anim = 100;

	p_am->p_animation = (SIM_ANIMATION *) malloc((p_am->Size_of_Anim) * sizeof(SIM_ANIMATION));		
	if (!p_am->p_animation)
	{
		//MessageBox(p_am->hWnd,"Unable to allocate memory for animatin module","Error",MB_OK);
		kprintf(1,"Unable to allocate memory for animatin module");
		return 0;
	}
	else
		ZeroMemory(p_am->p_animation, (p_am->Size_of_Anim) * sizeof(SIM_ANIMATION));

	for(i=0;i<p_am->Size_of_Anim;i++)
	{
		p_am->p_animation[i].flag = 0;
		p_am->p_animation[i].p_run = -1;
		p_am->p_animation[i].p_matrix = -1;
	}
	
	GetPrivateProfileString("game","data_dir","c:\\",text,256,ini_file);
	_chdir(text);

	pDataArchive->pActualNode = pDataArchive->pRootNode->pNextNode;

	file = aopen(pDataArchive, "animload.dat","r");
	if(!file)
	{
		kprintf(1, "animload.dat not found");
		return 0;
	}

	for(i=0;i<110;i++)
		p_am->sim_anim[i] = -1;

	i = 0;

	kprintf(1, "Kofola - Nahravam animace...");
	
	while(!aeof(file))
	{
		if(!agets(text, 256, file))
			break;

		text[strlen(text)-1] = '\0';
		//kprintf(1, "%s", text);
		p_am->sim_anim[i] = sim_nahraj_animaci(pDataArchive, text, 0);
		i++;
	}
	
	aclose(file);

	return 1;
}

//------------------------------------------------------------------------------------------------
// release animation module
//------------------------------------------------------------------------------------------------
void am_Release(ANIMATION_MODULE *p_am, LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<110;i++)
		if(p_am->sim_anim[i] != -1)
		{
			kprintf(1, "sim_zrus_animaci[%d], sim = %d", i, p_am->sim_anim[i]);

			sim_zrus_animaci(p_am->sim_anim[i]);
		}

	free ((void *) p_am->p_animation);
	p_am->Size_of_Anim = 0;

	for(i=0;i<10;i++)
		if(p_Level->ExitSparks[i].System != -1)
		{
			kprintf(1, "ExitSparks[%d], par_zrus %d", i, p_Level->ExitSparks[i].System);

			par_zrus(p_Level->ExitSparks[i].System);
			p_Level->ExitSparks[i].System = -1;	
		}

}

//------------------------------------------------------------------------------------------------
// find free animation item
//------------------------------------------------------------------------------------------------
int am_Find_Free_Animation_Item(ANIMATION_MODULE *p_am)
{
	int i;

	for(i=0;i<p_am->Size_of_Anim;i++)
		if(p_am->p_animation[i].flag == -1)
			return i;

	return -1;
}

//------------------------------------------------------------------------------------------------
// get animaton status
//------------------------------------------------------------------------------------------------
int am_Animation_Status(SIM_ANIMATION *p_animation, ANIMATION_MODULE *p_am)
{
	if (p_animation->flag == -1)
	{
		if (p_animation->p_run != -1)
		{
			//rani_rozvaz(p_animation->p_run,p_animation->p_matrix);
			rani_zrus(p_animation->p_run);
			p_animation->p_run = -1;
			p_animation->p_matrix = -1;
		}
		return 0;
	}
	else
		return 1;
}

//------------------------------------------------------------------------------------------------
// pripoji funkci na start animace
//------------------------------------------------------------------------------------------------
void am_Set_Start_Function(SIM_ANIMATION *p_animation, END_FUNKCE pStartf,
						   int iParam1, int iParam2, void *pParam, int index)
{
	p_animation->tStart[index].pProc = pStartf;
	p_animation->tStart[index].pParam = pParam;
	p_animation->tStart[index].iParam[0] = iParam1;
	p_animation->tStart[index].iParam[1] = iParam2;
}

//------------------------------------------------------------------------------------------------
// nastavi spusteni funkce na ucrite misto animace
//------------------------------------------------------------------------------------------------
void am_Set_Triger_Function(SIM_ANIMATION *p_animation, END_FUNKCE pTrigerf,
						   int iParam1, int iParam2, void *pParam, int flag, int index)
{
	p_animation->tTriger[index].pProc = pTrigerf;
	p_animation->tTriger[index].pParam = pParam;
	p_animation->tTriger[index].iParam[0] = iParam1;
	p_animation->tTriger[index].iParam[1] = iParam2;
	p_animation->tTriger[index].flag = flag;
}

//------------------------------------------------------------------------------------------------
// nastavi spusteni funkce na ucrite misto animace
//------------------------------------------------------------------------------------------------
void am_Set_aMaterial_Function(SIM_ANIMATION *p_animation, int i, END_FUNKCE taMateral,
						   int iParam1, int iParam2, void *pParam, int flag)
{
	p_animation->taMateral[i].pProc = taMateral;
	p_animation->taMateral[i].pParam = pParam;
	p_animation->taMateral[i].iParam[0] = iParam1;
	p_animation->taMateral[i].iParam[1] = iParam2;
	p_animation->taMateral[i].flag = flag;
}

//------------------------------------------------------------------------------------------------
// odebere predmet ke konstantni animaci
//------------------------------------------------------------------------------------------------
void am_Remove_Animate_item(long item, LEVELINFO *p_Level)
{
	int i = 0;

	while(p_Level->Anim_Item[i] != p_Level->Level[item]->iItem)
		i++;


	if(p_Level->Item[p_Level->Anim_Item[i]].a_run != -1)
	{
		rani_zrus(p_Level->Item[p_Level->Anim_Item[i]].a_run);
		p_Level->Item[p_Level->Anim_Item[i]].a_run = -1;
		p_Level->Item[p_Level->Anim_Item[i]].a_flag = -1;
	}	

	i++;

	while(p_Level->Anim_Item[i] != -1)
	{
		p_Level->Anim_Item[i-1] = p_Level->Anim_Item[i];
		i++;
	}
	
	if(i > 0)
		p_Level->Anim_Item[i-1] = p_Level->Anim_Item[i];
	else
		p_Level->Anim_Item[0] = -1;
}

//------------------------------------------------------------------------------------------------
// odebere predmet ke konstantni animaci
//------------------------------------------------------------------------------------------------
void am_Remove_Animate_itemB(int iItem, LEVELINFO *p_Level)
{
	int i = 0;

	while(p_Level->Anim_Item[i] != iItem)
		i++;


	if(p_Level->Item[p_Level->Anim_Item[i]].a_run != -1)
	{
		rani_zrus(p_Level->Item[p_Level->Anim_Item[i]].a_run);
		p_Level->Item[p_Level->Anim_Item[i]].a_run = -1;
		p_Level->Item[p_Level->Anim_Item[i]].a_flag = -1;
	}	

	i++;

	while(p_Level->Anim_Item[i] != -1)
	{
		p_Level->Anim_Item[i-1] = p_Level->Anim_Item[i];
		i++;
	}
	
	if(i > 0)
		p_Level->Anim_Item[i-1] = p_Level->Anim_Item[i];
	else
		p_Level->Anim_Item[0] = -1;
}

//------------------------------------------------------------------------------------------------
// prida predmet ke konstantni animaci
//------------------------------------------------------------------------------------------------
void am_Add_Animate_Item(long item, LEVELINFO *p_Level)
{
//	int iPos[3];
//	float pos[3];
	int i = 0;//, rot;

	while(p_Level->Anim_Item[i] != -1)
		i++;

	p_Level->Anim_Item[i] = p_Level->Level[item]->iItem;
	p_Level->Anim_Item[i+1] = -1;

/*	memcpy((void *) iPos, (void *) p_Level->Level[item]->Pos, 3*sizeof(int));

	iPos[2]--;

	gl_Logical2Real(iPos[0], iPos[1], iPos[2], &rot, p_Level);

	if(p_Level->Level[rot])
		if(p_Level->Level[rot]->p_Object->Class == 12)
		{
			kom_mesh_get_float(p_Level->Level[item]->Index_Of_Game_Mesh,
							   &pos[0], &pos[1], &pos[2], &rot);

			am_Kola_na_VodeB(pos, 1, -1, 1, p_Level);
		}*/
}

//------------------------------------------------------------------------------------------------
// prida predmet ke konstantni animaci
//------------------------------------------------------------------------------------------------
void am_Add_Animate_ItemB(int iItem, LEVELINFO *p_Level)
{
//	int iPos[3];
//	float pos[3];
	int i = 0;//, rot;

	while(p_Level->Anim_Item[i] != -1)
		i++;

	p_Level->Anim_Item[i] = iItem;
	p_Level->Anim_Item[i+1] = -1;

/*	memcpy((void *) iPos, (void *) p_Level->Item[iItem].Pos, 3*sizeof(int));

	iPos[2]--;

	gl_Logical2Real(iPos[0], iPos[1], iPos[2], &rot, p_Level);

	if(p_Level->Level[rot])
		if(p_Level->Level[rot]->p_Object->Class == 12)
		{
			kom_mesh_get_float(p_Level->Item[iItem].Index_Of_Game_Mesh,
							   &pos[0], &pos[1], &pos[2], &rot);

			am_Kola_na_VodeB(pos, 1, -1, 1, p_Level);
		}*/
}

//------------------------------------------------------------------------------------------------
// animuje predmety
//------------------------------------------------------------------------------------------------
void am_Animate_Items(ANIMATION_MODULE *p_am, LEVELINFO *p_Level)
{
	int i = 0;
	ITEMDESC *p_item;

	while(p_Level->Anim_Item[i] != -1)
	{
		if(p_Level->Item[p_Level->Anim_Item[i]].a_flag == -1)
		{
			p_item = &p_Level->Item[p_Level->Anim_Item[i]];

			if(p_item->a_run != -1)
			{
				rani_zrus(p_item->a_run);
				p_item->a_run = -1;
			}

			p_item->a_run = rani_aktivuj(p_am->sim_anim[34], &p_item->a_flag,GK_LOOP,0,0);
		
			if(p_item->a_run != -1)
			{
				rani_privaz_mesh(p_item->a_run, p_item->Index_Of_Game_Mesh,0);

				p_item->a_flag = 0;
			}
		}

		i++;

		if(i > p_Level->Count_Of_Items)
			return;
	}
}

//------------------------------------------------------------------------------------------------
// animuje predmety
//------------------------------------------------------------------------------------------------
void am_Release_Animate_Items(ANIMATION_MODULE *p_am, LEVELINFO *p_Level)
{
	int i = 0;
	ITEMDESC *p_item;

	while(p_Level->Anim_Item[i] != -1)
	{
		kprintf(1, "am_Release_Animate_Items - p_Level->Anim_Item[%d]", i);

		p_item = &p_Level->Item[p_Level->Anim_Item[i]];

		if(p_item->a_run != -1)
		{
			kprintf(1, "rani_zrus...");
			rani_zrus(p_item->a_run);
			p_item->a_run = -1;
		}

		i++;
	}
}

void am_Set_aMaterial_Trigers(SIM_ANIMATION *p_animation, ITEMDESC *p_Item, LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<3;i++)
		if(p_Item->p_Object->MatEvent[i].cType == 1)
			am_Set_aMaterial_Function(p_animation, i, anmend_Send_Event, 
									  p_Item->mMaterial->handle_materialove_animace,0,0,
									  p_Item->p_Object->MatEvent[i].iFlag);
		else
			if(p_Item->p_Object->MatEvent[i].cType == 2)
				am_Set_aMaterial_Function(p_animation, i, anmend_Send_Event, 
										  p_Item->mMaterial->handle_materialove_animace,0,0,-1);
			else
				if(!p_Item->p_Object->MatEvent[i].cType)
					am_Set_aMaterial_Function(p_animation, i, NULL, 0,0,0,-1);
}

void am_Set_Beetle_Switching(LEVELINFO *p_Level, int mesh, int bSwitch)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->BeetleAnim[i].Mesh == mesh)
		{
			p_Level->BeetleAnim[i].bSwitch = bSwitch;
			return;
		}

	return;
}

void am_Play_Beatle_Sound(BEETLEANIMATION *p_anim, int mesh, int idx)
{
	float pos[3];
	int	rot;

	kom_mesh_get_float(mesh, &pos[0], &pos[1], &pos[2], &rot);

//	p_anim->iSound = ap_Play_Sound(0, 0, 0, pos, idx + (rand()%3), NULL, &ad);
	p_anim->dwTime = timeGetTime();
}

void am_Do_Beatle_Cakanec(int mesh, LEVELINFO *p_Level, int id)
{
	int iPos[3], r;
	float pos[3], fPos[3];
	long real;

	if(p_Level->bUnderWaterLevel)
		return;

	kom_mesh_get_float(mesh, &pos[0], &pos[1], &pos[2], &r);

	iPos[0] = (int)((pos[0] - ber.x_start - X_PRVEK/2) / X_PRVEK);
	iPos[2] = (int)(((pos[1] - ber.y_start - Y_PRVEK/2) / Y_PRVEK) * 2);
	iPos[1] = (int)((pos[2] - ber.z_start - Z_PRVEK/2) / Z_PRVEK);

	if(iPos[0] < 0 || iPos[1] < 0 || iPos[2] < 0 ||
	   iPos[0] >= p_Level->Size[0] ||
	   iPos[1] >= p_Level->Size[1] ||
	   iPos[2] >= p_Level->Size[2])
		return;

	gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

	if(p_Level->Level[real])
		if(p_Level->Level[real]->p_Object->Class == 12)
		{
			fPos[0] = pos[0];
			fPos[1] = pos[1] - 1;
			fPos[2] = pos[2];

			if(!id)
			{
				am_Kola_na_Vode(fPos, 1, 4, 0, p_Level);
//				ap_Play_Sound(0,0,0, pos, 74+(rand()%3), NULL, &ad);
			}
			else
			{
				pos[1] += -0.7f;
	
				am_Do_Vodni_Cakanec1(pos, 4, 0, p_Level);
				//am_Do_Vodni_Cakanec2(pos, 4, 0, p_Level);

//				ap_Play_Sound(0,0,0, pos, 40+(rand()%4), NULL, &ad);
			}
		}
}

void am_Animate_Beetle(LEVELINFO *p_Level, int i)
{
	int r;

	switch(p_Level->BeetleAnim[i].iID)
	{
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		lani_set(p_Level->BeetleAnim[i].Mesh, 0, 0, &p_Level->BeetleAnim[i].Flag,0,0,0);
		p_Level->BeetleAnim[i].iAnimCount++;
		return;
	case 2:
		r = rand()%8;
		lani_set(p_Level->BeetleAnim[i].Mesh, 0, 2 + r, &p_Level->BeetleAnim[i].Flag,0,0,0);
		p_Level->BeetleAnim[i].iAnimCount++;
		return;
	case 0:
		r = rand()%12;
		lani_set(p_Level->BeetleAnim[i].Mesh, 0, 3 + r, &p_Level->BeetleAnim[i].Flag,0,0,0);

		if(p_Level->BeetleAnim[i].iAnimCount)
		{
			if(r == 4) // pada
				am_Play_Beatle_Sound(&p_Level->BeetleAnim[i], p_Level->BeetleAnim[i].Mesh, 165);
			else
				if(r == 5) // salto
				{
					am_Play_Beatle_Sound(&p_Level->BeetleAnim[i], p_Level->BeetleAnim[i].Mesh, 168);
					p_Level->BeetleAnim[i].iEffect = 1;
					p_Level->BeetleAnim[i].iEffectCounter = 0;
				}
			else
				if(r == 11) // meditace
					am_Play_Beatle_Sound(&p_Level->BeetleAnim[i], p_Level->BeetleAnim[i].Mesh, 171);
		}

		p_Level->BeetleAnim[i].iAnimCount++;

		return;
	}

	kprintf(1, "Nemuzu animovat brouka! ID = %d", p_Level->BeetleAnim[i].iID);
}

void am_Animate_Beetles(LEVELINFO *p_Level)
{
	DWORD	time = timeGetTime();
	int i, r, x;

	for(i=0;i<6;i++)
		if(p_Level->BeetleAnim[i].Mesh != -1 && p_Level->BeetleAnim[i].Flag == -1)
		{
			//jedu na starem cekani ... a tak to musi zustat
			/*if(p_Level->BeetleAnim[i].iID == 2)
			{
				kom_mesh_set_mesh(p_Level->BeetleAnim[i].Mesh, 2, p_Level->BeetleAnim[i].iRot);

				r = rand()%8;
				lani_set(p_Level->BeetleAnim[i].Mesh, 0, 2 + r, &p_Level->BeetleAnim[i].Flag,0,0,0);
			}				
			else*/
				if(gl_Check_Mesh_Sim(p_Level->BeetleAnim[i].Mesh))	//brouk se animuje -> nemuzu prepinat meshe
					am_Animate_Beetle(p_Level, i);
				else
				{
					r = (rand()%10);

					/*
						r <0-6>	nejcastejsi cekani
						r <7-8> ridke cekani
						r <9>	stare cekani
					*/

					if(time - p_Level->BeetleAnim[i].dwTime > 180000 && !(rand()%2))
					{
						p_Level->BeetleAnim[i].dwTime = time;
						r = 7;
					}

					if(r < 7)
					{
						x = (rand()%6);

						kom_mesh_set_mesh(p_Level->BeetleAnim[i].Mesh, 3 + x, p_Level->BeetleAnim[i].iRot);
						p_Level->BeetleAnim[i].iID = 3 + x;

						lani_set(p_Level->BeetleAnim[i].Mesh, 0, 0, &p_Level->BeetleAnim[i].Flag,0,0,0);
						p_Level->BeetleAnim[i].iAnimCount++;
					}
					else
					if(r > 6)
					{
						kom_mesh_set_mesh(p_Level->BeetleAnim[i].Mesh, 0, p_Level->BeetleAnim[i].iRot);
						p_Level->BeetleAnim[i].iID = 0;

						x = rand()%12;

						lani_set(p_Level->BeetleAnim[i].Mesh, 0, 3 + x, &p_Level->BeetleAnim[i].Flag,0,0,0);

						if(p_Level->BeetleAnim[i].iAnimCount)
						{
							if(x == 4) // pada
								am_Play_Beatle_Sound(&p_Level->BeetleAnim[i], p_Level->BeetleAnim[i].Mesh, 165);
							else
							if(x == 5) // salto
							{
								am_Play_Beatle_Sound(&p_Level->BeetleAnim[i], p_Level->BeetleAnim[i].Mesh, 168);
								p_Level->BeetleAnim[i].iEffect = 1;
								p_Level->BeetleAnim[i].iEffectCounter = 0;
							}
							else
							if(x == 11) // meditace
								am_Play_Beatle_Sound(&p_Level->BeetleAnim[i], p_Level->BeetleAnim[i].Mesh, 171);
						}

						p_Level->BeetleAnim[i].iAnimCount++;
					}
					/*else
					{
						kom_mesh_set_mesh(p_Level->BeetleAnim[i].Mesh, 2, p_Level->BeetleAnim[i].iRot);
						p_Level->BeetleAnim[i].iID = 2;

						x = rand()%8;
						lani_set(p_Level->BeetleAnim[i].Mesh, 0, 2 + x, &p_Level->BeetleAnim[i].Flag,0,0,0);
					}*/
				}
		}
		else
			if(p_Level->BeetleAnim[i].Mesh != -1 && p_Level->BeetleAnim[i].Flag != -1 && p_Level->BeetleAnim[i].iEffect)
			{
				if(!p_Level->BeetleAnim[i].iEffectCounter)
				{
					am_Do_Beatle_Cakanec(p_Level->BeetleAnim[i].Mesh, p_Level, 0);
					p_Level->BeetleAnim[i].iEffectCounter++;
				}
				else
					if(p_Level->BeetleAnim[i].Flag >= 10)
					{
						p_Level->BeetleAnim[i].iEffect = 0;
						am_Do_Beatle_Cakanec(p_Level->BeetleAnim[i].Mesh, p_Level, 1);
						p_Level->BeetleAnim[i].iEffectCounter = 0;
					}
			}
}

void am_Remove_Beetle_Animation(int mesh, LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->BeetleAnim[i].Mesh == mesh)
		{
			lani_set(p_Level->BeetleAnim[i].Mesh, 0, K_CHYBA, 
					&p_Level->BeetleAnim[i].Flag,0,0,0);
			
			kom_mesh_set_mesh(mesh, 0, p_Level->BeetleAnim[i].iRot);

			//kprintf(1, "am_Remove_Beetle_Animation set na  je %d", p_Level->BeetleAnim[i].iRot);

			p_Level->BeetleAnim[i].Mesh = -1;
			p_Level->BeetleAnim[i].iID = 0;
/*
			if(timeGetTime() - p_Level->BeetleAnim[i].dwTime < 1000 && p_Level->BeetleAnim[i].iSound != -1)
				adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, p_Level->BeetleAnim[i].iSound);
*/
			p_Level->BeetleAnim[i].iSound = -1;

			return;
		}
}

void am_Correct_Beetle_Animation(int mesh, LEVELINFO *p_Level, int rot)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->BeetleAnim[i].Mesh == mesh)
		{
			p_Level->BeetleAnim[i].iRot = rot;
			return;
		}
}

int am_Add_Beetle_Animation(int mesh, LEVELINFO *p_Level, int iRot, char bExeption)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->BeetleAnim[i].Mesh == mesh)
			return 0;

	if(gl_Check_Mesh_Sim(mesh) && !bExeption)
		return 0;

	for(i=0;i<6;i++)
		if(p_Level->BeetleAnim[i].Mesh == -1)
		{
			lani_set(mesh, 0, K_CHYBA, &p_Level->BeetleAnim[i].Flag,0,0,0);
			p_Level->BeetleAnim[i].Mesh = mesh;
			p_Level->BeetleAnim[i].Flag = -1;
			p_Level->BeetleAnim[i].iRot = iRot;
//			kom_mesh_set_mesh(mesh, 2, iRot);
			p_Level->BeetleAnim[i].dwTime = timeGetTime();
			p_Level->BeetleAnim[i].iAnimCount = 1;
			p_Level->BeetleAnim[i].iSound = -1;
			p_Level->BeetleAnim[i].iEffect = 0;
			p_Level->BeetleAnim[i].iEffectCounter = 0;
			return 1;
		}

	return 0;
}

void am_Stop_Beetle_Animations(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->BeetleAnim[i].Mesh != -1)
		{
			//kprintf(1, "lani_set[%d], mesh = %d", i, p_Level->BeetleAnim[i].Mesh);

			lani_set(p_Level->BeetleAnim[i].Mesh, 0, K_CHYBA, 
					&p_Level->BeetleAnim[i].Flag,0,0,0);

			p_Level->BeetleAnim[i].Mesh = -1;
		}
}

void am_Init_Zhave_Castice(LEVELINFO *p_Level)
{
	int i,j;

	p_Level->BublSystemE.System.System = -1;
	p_Level->BublSystemE.System.pCastice = NULL;

	p_Level->BublSystemC.System.System = -1;
	p_Level->BublSystemC.System.pCastice = NULL;

	for(i=0;i<10;i++)
	{
		p_Level->KourUst[i].pCastice = NULL;
		p_Level->KourUst[i].System = -1;

		for(j=0;j<64;j++)
			p_Level->BublVybuch[i].hHnizdo[j] = 0;
	}

	for(i=0;i<20;i++)
	{
		p_Level->TeleportSparks[i].pCastice = NULL;
		p_Level->TeleportSparks[i].System = -1;
		p_Level->ZhaveCastice[i].System.pCastice = NULL;
		p_Level->KourovaStopa[i].pCastice = NULL;
		p_Level->KourovaStopa[i].System = -1;
		p_Level->Kour[i].pCastice = NULL;
		p_Level->Kour[i].System = -1;
		p_Level->KourKameni[i].pCastice = NULL;
		p_Level->KourKameni[i].System = -1;
		p_Level->BublVybuch[i].pCastice = NULL;
		p_Level->BublVybuch[i].System = -1;

		for(j=0;j<64;j++)
		{
			p_Level->ZhaveCastice[i].hSvetlo[j] = -1;
			p_Level->ZhaveCastice[i].hFlare[j] = -1;
			p_Level->KourovaStopa[i].hHnizdo[j] = 0;
			p_Level->Kour[i].hHnizdo[j] = 0;
			p_Level->KourKameni[i].hHnizdo[j] = 0;
			p_Level->BublVybuch[i].hHnizdo[j] = 0;
			p_Level->TeleportSparks[i].hHnizdo[j] = 0;
		}
	}

	for(i=0;i<30;i++)
	{
		p_Level->BarelSparks[i].pCastice = NULL;
		p_Level->BarelSparks[i].System = -1;
		
		for(j=0;j<64;j++)
			p_Level->BarelSparks[i].hHnizdo[j] = 0;
	}

	for(i=0;i<10;i++)
	{
		p_Level->LiftParticles[i].pCastice = NULL;
		p_Level->LiftParticles[i].System = -1;

		p_Level->VodniKola[i].pCastice = NULL;
		p_Level->VodniKola[i].System = -1;

		p_Level->LiftVParticles[i].pCastice = NULL;
		p_Level->LiftVParticles[i].System = -1;

		p_Level->ExitEfect[i].hSvetlo = -1;
		p_Level->ExitEfect[i].System.System = -1;
		p_Level->ExitEfect[i].hEXSvetlo[0] = -1;
		p_Level->ExitEfect[i].hEXSvetlo[1] = -1;
		p_Level->ExitEfect[i].System.hHnizdo[0] = 0;
		p_Level->ExitEfect[i].System.hHnizdo[1] = 0;

		p_Level->ExitSparks[i].pCastice = NULL;
		p_Level->ExitSparks[i].System = -1;

		p_Level->TelCSparks[i].pCastice = NULL;
		p_Level->TelCSparks[i].System = -1;

		p_Level->NatureESystem[i].pCastice = NULL;
		p_Level->NatureESystem[i].pSystem = -1;
		p_Level->NatureESystem[i].hHnizdo = 0;
		p_Level->NatureESystem[i].EffectID = 0;

		for(j=0;j<64;j++)
		{
			p_Level->LiftParticles[i].hHnizdo[j] = 0;
			p_Level->LiftVParticles[i].hHnizdo[j] = 0;
			p_Level->VodniKola[i].hHnizdo[j] = 0;
			p_Level->ExitSparks[i].hHnizdo[j] = 0;
			p_Level->TelCSparks[i].hHnizdo[j] = 0;
		}
	}

	for(i=0;i<100;i++)
	{
		p_Level->VodniKolaB[i].pCastice = NULL;
		p_Level->VodniKolaB[i].System = -1;

		for(j=0;j<64;j++)
			p_Level->VodniKola[i].hHnizdo[j] = 0;
	}

	for(i=0;i<6;i++)
	{
		p_Level->BublSystem[i].System.System = -1;
		p_Level->BublSystem[i].iStart = (int)ceil((randf() / (float)RAND_MAX) * 3000);

		for(j=0;j<64;j++)
		{
			p_Level->BublSystem[i].System.hHnizdo[j] = 0;
		}
	}

	for(i=0;i<8;i++)
		memset(&p_Level->StreetL[i], 0, sizeof(STREET_LIGHT_EFFECT));

	for(i=0;i<10;i++)
		memset(&p_Level->SwampL[i], 0, sizeof(STREET_LIGHT_EFFECT));

	for(i=0;i<200;i++)
		memset(&p_Level->StarL[i], 0, sizeof(STREET_LIGHT_EFFECT));

	for(i=0;i<2;i++)
	{
		memset(&p_Level->StarFall[i], 0, sizeof(FAIRY_EFFECT));
		p_Level->StarFall[i].pSystem = -1;
	}

	for(i=0;i<6;i++)
	{
		memset(&p_Level->CandleEffect[i], 0, sizeof(CANDLE_EFFECT));
		p_Level->CandleEffect[i].pSystem = -1;
		p_Level->CandleEffect[i].hSvetlo = -1;
		memset(&p_Level->CandleSmoke[i], 0, sizeof(CANDLE_EFFECT));
		p_Level->CandleSmoke[i].pSystem = -1;
		p_Level->CandleSmoke[i].hSvetlo = -1;
	}

	for(i=0;i<4;i++)
	{
		memset(&p_Level->FairyEffect[i], 0, sizeof(FAIRY_EFFECT));
		p_Level->FairyEffect[i].pSystem = -1;
	}
}

void am_Obsluha_Koure(int i, LEVELINFO *p_Level)
{
	DWORD dwTime, dwEplased;
	int j;

	if(p_Level->Kour[i].System != -1)
	{
		dwTime = timeGetTime();
		dwEplased = dwTime - p_Level->Kour[i].dwStart;

		if(dwEplased > 1000)
			for(j=0;j<p_Level->Kour[i].Sizeof;j++)
				if(p_Level->Kour[i].hHnizdo[j])
				{
					par_vloz_hnizdo_pivot(p_Level->Kour[i].hHnizdo[j], NULL);
					p_Level->Kour[i].hHnizdo[j] = 0;
				}

		if(!par_get_hnizda(p_Level->Kour[i].System))
		{
			par_zrus(p_Level->Kour[i].System);
			p_Level->Kour[i].System = -1;				
		}
	}
}

void am_Obsluha_Koure_Kameni(int i, LEVELINFO *p_Level)
{
	DWORD dwTime, dwEplased;
	int j;

	if(p_Level->KourKameni[i].System != -1)
	{
		dwTime = timeGetTime();
		dwEplased = dwTime - p_Level->KourKameni[i].dwStart;

		if(dwEplased > 1000)
			for(j=0;j<p_Level->KourKameni[i].Sizeof;j++)
				if(p_Level->KourKameni[i].hHnizdo[j])
				{
					par_vloz_hnizdo_pivot(p_Level->KourKameni[i].hHnizdo[j], NULL);
					p_Level->KourKameni[i].hHnizdo[j] = 0;
				}

		if(!par_get_hnizda(p_Level->KourKameni[i].System))
		{
			par_zrus(p_Level->KourKameni[i].System);
			p_Level->KourKameni[i].System = -1;				
		}
	}
}

void am_Do_Zhave_castice(LEVELINFO *p_Level)
{
	int i,j;
	SYSTEMZHAVYCHCASTIC	*pSystem;
	DWORD dwTime, dwEplased;

	for(i=0;i<20;i++)
	{
		if(p_Level->ZhaveCastice[i].System.pCastice)
		{
			pSystem = &p_Level->ZhaveCastice[i];
			
			for(j=0;j<pSystem->System.Sizeof;j++)
				if(!pSystem->System.pCastice[j].aktivni)
				{
					if(pSystem->hSvetlo[j] != -1)
					{
						sdl_svetlo_zrus(pSystem->hSvetlo[j]);
						pSystem->hSvetlo[j] = -1;
					}

					if(pSystem->hFlare[j] != -1)
					{
						kom_flare_zrus(pSystem->hFlare[j]);
						pSystem->hFlare[j] = -1;
					}

					if(pSystem->iKourStopa >= 0)
					{
						if(p_Level->KourovaStopa[pSystem->iKourStopa].hHnizdo[j])
						{
							dwTime = timeGetTime();
							dwEplased = dwTime - p_Level->KourovaStopa[pSystem->iKourStopa].dwStart;

							if(dwEplased < 100)
								par_zrus_hnizdo(p_Level->KourovaStopa[pSystem->iKourStopa].System, p_Level->KourovaStopa[pSystem->iKourStopa].hHnizdo[j]);
							else
								par_vloz_hnizdo_pivot(p_Level->KourovaStopa[pSystem->iKourStopa].hHnizdo[j], NULL);

							p_Level->KourovaStopa[pSystem->iKourStopa].hHnizdo[j] = 0;
						}
					}
				}
				else
					sdl_svetlo_set_pos(pSystem->hSvetlo[j], 
									   (BOD *)&pSystem->System.pCastice[j].p);
		}

		if(p_Level->KourovaStopa[i].System != -1)
			if(!par_get_hnizda(p_Level->KourovaStopa[i].System))
			{
				par_zrus(p_Level->KourovaStopa[i].System);
				p_Level->KourovaStopa[i].System = -1;
			}

		//obslouzeni kouru
		am_Obsluha_Koure(i, p_Level);

		//obslouzeni koure do kamenech
		am_Obsluha_Koure_Kameni(i, p_Level);
	}
}

void am_Release_Zhave_castice(SYSTEMZHAVYCHCASTIC *pSystem)
{
	int i;

	pSystem->System.pCastice = NULL;

	for(i=0;i<32;i++)
	{
		if(pSystem->hSvetlo[i] != -1)
		{
			sdl_svetlo_zrus(pSystem->hSvetlo[i]);
			pSystem->hSvetlo[i] = -1;
		}
		
		if(pSystem->hFlare[i] != -1)
		{
			kom_flare_zrus(pSystem->hFlare[i]);
			pSystem->hFlare[i] = -1;
		}
	}
}

void am_Release_Kourove_Castice(SYSTEMKOUROVYCHCASTIC *pSystem)
{
	int i;

	pSystem->pCastice = NULL;

	for(i=0;i<64;i++)
	{
		if(pSystem->hHnizdo[i])
		{
			par_vloz_hnizdo_pivot(pSystem->hHnizdo[i], NULL);
			pSystem->hHnizdo[i] = 0;
		}
	}
}

void am_Change_Wind(LEVELINFO *p_Level)
{
	float f;

	p_Level->fVitr[0] = rand()&0x1 ? randf() : -randf();
	p_Level->fVitr[1] = rand()&0x1 ? randf() : -randf();
	p_Level->fVitr[2] = rand()&0x1 ? randf() : -randf();
	vektor_norm((BOD *)p_Level->fVitr);

	f = (randf()/(float)(RAND_MAX)) * (_3DKOREKCE / 2.0f);
	
	p_Level->fVitr[0] *= f / 30.0f;
	p_Level->fVitr[1] *= f / 50.0f;
	p_Level->fVitr[2] *= f / 30.0f;
}

void am_Obsluha_Teleportacnich_Jisker(int i, LEVELINFO *p_Level)
{
	PAR_KOUR_STOPA *pCastice;
	DWORD dwTime, dwEplased;
	int j , r;
	float f;

	if(p_Level->TeleportSparks[i].System != -1)
	{
		dwTime = timeGetTime();
		dwEplased = dwTime - p_Level->TeleportSparks[i].dwStart;
		
		if(!p_Level->bGameResume)
			p_Level->TeleportSparks[i].dwTime += ber.TimeLastFrame;

		if(dwEplased > 30000)
			for(j=0;j<p_Level->TeleportSparks[i].Sizeof;j++)
				if(p_Level->TeleportSparks[i].hHnizdo[j])
				{
					par_zrus_hnizdo(p_Level->TeleportSparks[i].System,
									p_Level->TeleportSparks[i].hHnizdo[j]);
					p_Level->TeleportSparks[i].hHnizdo[j] = 0;
				}

		//mihotani jisker
		if(p_Level->TeleportSparks[i].dwTime > 100)
		{
			for(j=0;j<p_Level->TeleportSparks[i].Sizeof;j++)
				if(p_Level->TeleportSparks[i].hHnizdo[j])
				{
					pCastice = 	par_cti_hnizdo_castice(p_Level->TeleportSparks[i].hHnizdo[j]);

					if(pCastice)
					{
						while(pCastice)
						{
							if(pCastice->rychlost_x > 0.01f &&
							   pCastice->rychlost_y > 0.01f)
							{
								r = rand() %100;

								if(r < 10)
								{
									f = (randf()/(float)(RAND_MAX)) / 7.5f;
									pCastice->rychlost_x += f;
									pCastice->rychlost_y += f;
								}
							}
							else
								pCastice->a = 0;

							pCastice = pCastice->p_next;
						}
					}
				}

			p_Level->TeleportSparks[i].dwTime = 0;
		}

		if(!par_get_hnizda(p_Level->TeleportSparks[i].System))
		{
			par_zrus(p_Level->TeleportSparks[i].System);
			p_Level->TeleportSparks[i].System = -1;				
		}
	}
}

void am_Do_Flares(LEVELINFO *p_Level)
{
	int i,j;
	LENS_FLARE *pFlare;
	DWORD dwTime, dwEplased;
	float koef = ber.TimeLastFrame/1000.0f;

	dwTime = timeGetTime();
	
	for(i=0;i<20;i++)
		if(p_Level->Flare[i].pFlare)
		{
			dwEplased = dwTime - p_Level->Flare[i].dwStart;

			if(dwEplased > 1000)
			{
				//pokud se napodarilo prvek presunout v terminu (500 ms) udelej to ted
				if(p_Level->Flare[i].Item)
				{
					(anmend_Teleport)((int)p_Level->Flare[i].Item, 
									p_Level->Flare[i].Teleport, p_Level->Flare[i].pPointer);
					p_Level->Flare[i].Item = NULL;
					p_Level->Flare[i].Teleport = -1;
				}

				kom_flare_zrus(p_Level->Flare[i].pFlare);
				p_Level->Flare[i].pFlare = 0;
				
				if(p_Level->Flare[i].hSvetlo != -1)
				{
					sdl_svetlo_zrus(p_Level->Flare[i].hSvetlo);
					p_Level->Flare[i].hSvetlo = -1;
				}

				if(p_Level->Flare[i].hEXSvetlo != -1)
				{
					edl_svetlo_zrus(p_Level->Flare[i].hEXSvetlo);
					p_Level->Flare[i].hEXSvetlo = -1;
				}
			
			}
			else
				if(dwEplased > 500)
				{
					//proved teleportaci, pokud je zapsan predmet
					if(p_Level->Flare[i].Item)
					{
						(anmend_Teleport)((int)p_Level->Flare[i].Item, 
										p_Level->Flare[i].Teleport, p_Level->Flare[i].pPointer);
						p_Level->Flare[i].Item = NULL;
						p_Level->Flare[i].Teleport = -1;
					}

					pFlare = kom_flare_get_pointer(p_Level->Flare[i].pFlare);

					pFlare->dx -= p_Level->Flare[i].mSpeed * koef;
					pFlare->dy -= p_Level->Flare[i].mSpeed * koef;

					for(j=0;j<14;j++)
						if(pFlare->p_sloz[j].vzdal)
						{
							pFlare->p_sloz[j].dx -= p_Level->Flare[i].Speed[j] * koef;
							pFlare->p_sloz[j].dy -= p_Level->Flare[i].Speed[j] * koef;
						}
						else
							break;
				}
				else
				{
					pFlare = kom_flare_get_pointer(p_Level->Flare[i].pFlare);

					pFlare->dx += p_Level->Flare[i].mSpeed * koef;
					pFlare->dy += p_Level->Flare[i].mSpeed * koef;

					for(j=0;j<14;j++)
						if(pFlare->p_sloz[j].vzdal)
						{
							pFlare->p_sloz[j].dx += p_Level->Flare[i].Speed[j] * koef;
							pFlare->p_sloz[j].dy += p_Level->Flare[i].Speed[j] * koef;
						}
						else
							break;
				}
		}

	for(i=0;i<20;i++)
		am_Obsluha_Teleportacnich_Jisker(i, p_Level);
}

void am_Do_LiftVParticies(int i, LEVELINFO *p_Level)
{
	int r;
	float f, pos[3];
	DWORD dwTime, dwEplased;
	PARMETAC_HNIZDO *pHnizdo;

	dwTime = timeGetTime();

	if(p_Level->LiftVParticles[i].System != -1)
	{
		dwEplased = dwTime - p_Level->LiftVParticles[i].dwStart;

		if(dwEplased > 200000)
		{
			par_zrus(p_Level->LiftVParticles[i].System);
			p_Level->LiftVParticles[i].System = -1;			
		}
		else
		{
			if(!p_Level->bGameResume)
				p_Level->LiftVParticles[i].dwTime += ber.TimeLastFrame;

			if(p_Level->LiftVParticles[i].dwTime > 500)
			{
				p_Level->LiftVParticles[i].dwTime = 0;
					
				if(p_Level->LiftVParticles[i].hHnizdo[0])
				{
					pHnizdo = par_cti_hnizdo(p_Level->LiftVParticles[i].hHnizdo[0]);
					pHnizdo->time_interval += 10;
				}
			}
				
			if(p_Level->LiftVParticles[i].hHnizdo[0])
			{
				kom_mesh_get_float(p_Level->LiftVParticles[i].hHnizdo[1],
								   &pos[0], &pos[1], &pos[2], &r);
					
				memcpy((void *)p_Level->LiftVParticles[i].pivot[0], (void *)pos, 3*sizeof(float));

				p_Level->LiftVParticles[i].pivot[0][0] += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				p_Level->LiftVParticles[i].pivot[0][1] --;
				p_Level->LiftVParticles[i].pivot[0][2] += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);

				p_Level->LiftVParticles[i].dir[0][0] = 0;
				p_Level->LiftVParticles[i].dir[0][1] = ((-randf()/(float)(RAND_MAX)) / 7.0f) * _3DKOREKCE;

				if(p_Level->LiftVParticles[i].dir[0][1] > (-0.1f) * _3DKOREKCE)
					p_Level->LiftVParticles[i].dir[0][1] = (-0.1f) * _3DKOREKCE;

				p_Level->LiftVParticles[i].dir[0][2] = 0;

				f = (randf()/(float)(RAND_MAX)) / 8.0f;
				
				par_vloz_hnizdo_scale(p_Level->LiftVParticles[i].hHnizdo[0], f, f, 0, 0);
			}
		}
	}
}

void am_Do_Lifts(LEVELINFO *p_Level)
{
	int i,r;
	float f, pos[3];
	DWORD dwTime, dwEplased;

	dwTime = timeGetTime();
	
	for(i=0;i<10;i++)
	{
		if(p_Level->LiftParticles[i].System != -1)
		{
			dwEplased = dwTime - p_Level->LiftParticles[i].dwStart;

			if(dwEplased > p_Level->LiftParticles[i].dwStop)
			{
				if(!par_get_hnizda(p_Level->LiftParticles[i].System))
				{
					par_zrus(p_Level->LiftParticles[i].System);
					p_Level->LiftParticles[i].System = -1;			
				}
				else
					par_vloz_hnizdo_pivot(p_Level->LiftParticles[i].hHnizdo[0], NULL);
			}
			else
			{
				if(!p_Level->bGameResume)
					p_Level->LiftParticles[i].dwTime += ber.TimeLastFrame;

				if(p_Level->LiftParticles[i].dwTime > 100)
				{
					p_Level->LiftParticles[i].dwTime = 0;
				
					r = rand()%100;

					if(r < 50 && p_Level->LiftParticles[i].hHnizdo[0])
					{
						//nacteni pozice vytahu
						kom_mesh_get_float(p_Level->LiftParticles[i].hHnizdo[1],
										   &pos[0], &pos[1], &pos[2], &r);
					
						memcpy((void *)p_Level->LiftParticles[i].pivot[0], (void *)pos, 3*sizeof(float));

						p_Level->LiftParticles[i].pivot[0][0] += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
						p_Level->LiftParticles[i].pivot[0][1] --;
						p_Level->LiftParticles[i].pivot[0][2] += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);

						p_Level->LiftParticles[i].dir[0][0] = 0;
						p_Level->LiftParticles[i].dir[0][1] = ((-randf()/(float)(RAND_MAX)) / 10.0f) * _3DKOREKCE;

						if(p_Level->LiftParticles[i].dir[0][1] > (-0.03f) * _3DKOREKCE)
							p_Level->LiftParticles[i].dir[0][1] = (-0.03f) * _3DKOREKCE;

						p_Level->LiftParticles[i].dir[0][2] = 0;

						f = (randf()/(float)(RAND_MAX)) / 25.0f;
					
						par_vloz_hnizdo_scale(p_Level->LiftParticles[i].hHnizdo[0], f, f, 0, 0);
					
						par_vloz_hnizdo_pust_castici(p_Level->LiftParticles[i].hHnizdo[0]);
					}
				}
			}
		}

		am_Do_LiftVParticies(i, p_Level);
	}
}

void am_Do_Water_KolaB(LEVELINFO *p_Level)
{
	int i, rot;
	float a, pos[3];
	DWORD dwTime, dwEplased;
	PARMETAC_HNIZDO *pHnizdo;
	WATER_BOUNDARY	Boundary;

	dwTime = timeGetTime();
	
	for(i=0;i<100;i++)
		if(p_Level->VodniKolaB[i].System != -1)
		{
			dwEplased = dwTime - p_Level->VodniKolaB[i].dwStart - ber.TimeLastFrame;

			if(p_Level->VodniKolaB[i].dwExpire)
			{
				if(!p_Level->bGameResume)
					p_Level->VodniKolaB[i].dwExpireTime += ber.TimeLastFrame;

			   if(p_Level->VodniKolaB[i].dwExpireTime >= p_Level->VodniKolaB[i].dwExpire &&
				  p_Level->VodniKolaB[i].hHnizdo[2] != -1)
					p_Level->VodniKolaB[i].hHnizdo[2] = -1;
			}

			if(p_Level->VodniKolaB[i].hHnizdo[2] != -1)
			{
				kom_mesh_get_float(p_Level->VodniKolaB[i].hHnizdo[2],
								   &pos[0], &pos[1], &pos[2], &rot);

				if(!p_Level->VodniKolaB[i].dwExpire)
					w_Choose_Best_Boundary(&Boundary, pos[0], pos[2], pos[1] - 1, 
										   WATER_BOUNDARY_METHOD_SIZE_HEURISTICS);
				else
					w_Choose_Best_Boundary(&Boundary, pos[0], pos[2], 
										   p_Level->VodniKolaB[i].pivot[0][1] - 0.67f, 
										   WATER_BOUNDARY_METHOD_SIZE_HEURISTICS);


				par_vloz_hnizdo_clip(p_Level->VodniKolaB[i].hHnizdo[0],
									 Boundary.Left, Boundary.Bottom,
									 Boundary.Right, Boundary.Top);				
				pos[1] -= 0.33f;

				p_Level->VodniKolaB[i].pivot[0][0] = pos[0];
				
				if(!p_Level->VodniKolaB[i].dwExpire)
					p_Level->VodniKolaB[i].pivot[0][1] = pos[1];

				p_Level->VodniKolaB[i].pivot[0][2] = pos[2];
			}
			
			if(dwEplased > p_Level->VodniKolaB[i].dwStop)
			{
				if(!par_get_hnizda(p_Level->VodniKolaB[i].System))
				{
					par_zrus(p_Level->VodniKolaB[i].System);
					p_Level->VodniKolaB[i].System = -1;	
				}
				else
				{
					pHnizdo = par_cti_hnizdo(p_Level->VodniKolaB[i].hHnizdo[0]);

					if(pHnizdo)
					{
						if(!p_Level->bGameResume)
							p_Level->VodniKolaB[i].dwTime += ber.TimeLastFrame;
					
						if(p_Level->VodniKolaB[i].dwTime > 250)
						{
							p_Level->VodniKolaB[i].dwTime = 0;

							if(pHnizdo->a > 0)
							{
								a = pHnizdo->a - 0.05f;
					
								if(a <= 0)
									a = 0;

								par_vloz_hnizdo_diff(p_Level->VodniKolaB[i].hHnizdo[0],
													 pHnizdo->r,pHnizdo->g,pHnizdo->b,a,
													 pHnizdo->dr,pHnizdo->dg,pHnizdo->db,
													 pHnizdo->da, pHnizdo->ka);
							}
							else
								if(p_Level->VodniKolaB[i].hHnizdo[0])
									par_vloz_hnizdo_pivot(p_Level->VodniKolaB[i].hHnizdo[0], NULL);
						}
					}
				}
			}
		}
}

void am_Do_Water(LEVELINFO *p_Level)
{
	int i;
	float a;
	DWORD dwTime, dwEplased;
	PARMETAC_HNIZDO *pHnizdo;

	dwTime = timeGetTime();
	
	for(i=0;i<10;i++)
		if(p_Level->VodniKola[i].System != -1)
		{
			dwEplased = dwTime - p_Level->VodniKola[i].dwStart - ber.TimeLastFrame;

			if(dwEplased > p_Level->VodniKola[i].dwStop)
			{
/*				if(!p_Level->VodniKola[i].hHnizdo[1] && p_Level->VodniKola[i].dwStop > 0)
				{
					am_Do_Vodni_Cakanec2(p_Level->VodniKola[i].pivot[1], 
										 p_Level->VodniKola[i].hHnizdo[2],
										 p_Level->VodniKola[i].hHnizdo[3],
										 p_Level);

					p_Level->VodniKola[i].hHnizdo[1] = 1;
				}*/

				if(!par_get_hnizda(p_Level->VodniKola[i].System))
				{
					par_zrus(p_Level->VodniKola[i].System);
					p_Level->VodniKola[i].System = -1;	
				}
				else
				{
					pHnizdo = par_cti_hnizdo(p_Level->VodniKola[i].hHnizdo[0]);

					if(pHnizdo)
					{
						if(!p_Level->bGameResume)
							p_Level->VodniKola[i].dwTime += ber.TimeLastFrame;
					
						if(p_Level->VodniKola[i].dwTime > 250)
						{
							p_Level->VodniKola[i].dwTime = 0;

							if(pHnizdo->a > 0)
							{
								a = pHnizdo->a - 0.05f;
					
								if(a <= 0)
									a = 0;

/*								if(p_Level->VodniKola[i].dwStop > 0)
									par_vloz_hnizdo_scale(p_Level->VodniKola[i].hHnizdo[0], 0.01f, 0.01f, 
														  pHnizdo->utlum_x, pHnizdo->utlum_y);*/
								
								par_vloz_hnizdo_diff(p_Level->VodniKola[i].hHnizdo[0],
													 pHnizdo->r,pHnizdo->g,pHnizdo->b,a,
													 pHnizdo->dr,pHnizdo->dg,pHnizdo->db,
													 pHnizdo->da, pHnizdo->ka);
							}
							else
								if(p_Level->VodniKola[i].hHnizdo[0])
									par_vloz_hnizdo_pivot(p_Level->VodniKola[i].hHnizdo[0], NULL);
						}
					}
				}
			}
		}
}


int am_Get_Free_VodniKola(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->VodniKola[i].System == -1)
			return i;

	return -1;
}

int am_Kola_na_Vode(float *pos, int VyskaSloupce, int VyskaPadu, int predmet, LEVELINFO *p_Level)
{
	int k,m;
	PAR_KOUR_STOPA	*pKourovaS;
	PARMETAC_HNIZDO *pHnizdo;
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	WATER_BOUNDARY	Boundary;
	int size = (int)(100 / p_Level->KvalitaCastic);

	k = am_Get_Free_VodniKola(p_Level);

	if(k == -1)
		return -1;
	
	pSystem = &p_Level->VodniKola[k];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return -1;
	
	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;
	pSystem->dwStart = timeGetTime();

	m = kom_najdi_material("cvkolo1");
	
	if(m == -1)
		kprintf(1,"Nelze najit material cvkolo1");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = 1.5f;
	pKourovaS[0].utlum_x = 2.75f + ((VyskaPadu-1) * 0.25f);
	pKourovaS[0].utlum_y = 2.75f + ((VyskaPadu-1) * 0.25f);

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 0.6f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = -0.15f;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_3D | TPAR_SCALE_ADD | 
				  TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_2DCLIP, (BOD *)pos, NULL);

	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 550, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 550, -550);
	
	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][1] += 0.7f;

	if(pSystem->hHnizdo[0])
		pHnizdo = par_cti_hnizdo(pSystem->hHnizdo[0]);

	pSystem->nx[0][0] = 1;
	pSystem->nx[0][1] = 0;
	pSystem->nx[0][2] = 0;
	pSystem->ny[0][0] = 0;
	pSystem->ny[0][1] = 0;
	pSystem->ny[0][2] = 1;

	pHnizdo->p_nx = (BOD *) pSystem->nx[0];
	pHnizdo->p_ny = (BOD *) pSystem->ny[0];

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwExpire = 0;
	pSystem->dwExpireTime = 0;
	pSystem->dwStop = VyskaSloupce * 10;

	memcpy((void *) pSystem->pivot[1], (void *)pos, 3 * sizeof(float));

//	pSystem->hHnizdo[1] = 0;
	pSystem->hHnizdo[2] = VyskaPadu;
	pSystem->hHnizdo[3] = predmet;

	w_Choose_Best_Boundary(&Boundary, pos[0], pos[2], pos[1], WATER_BOUNDARY_METHOD_SIZE_HEURISTICS);
	par_vloz_hnizdo_clip(pSystem->hHnizdo[0], Boundary.Left, Boundary.Bottom,
						 Boundary.Right, Boundary.Top);

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);

	return k;
}

void am_Do_Vodni_Cakanec1(float *pos, int vyska, int Predmet, LEVELINFO *p_Level)
{
	int i,r;
	PAR_STREPINA	*pCastice;
	int pocercastic;
	int ph;
	int material;
	float v;
	float fpos[3];

	memcpy((void *) fpos, (void *)pos, 3* sizeof(float));

	fpos[1] += 0.7f;

	for(i=0;i<10;i++)
	{
		r = rand()%10;
		if(p_Level->VodniCakanec1[r].flag == -1)
			break;
	}
	
	if(i == 10)
		return;

	p_Level->VodniCakanec1[r].dwStart = timeGetTime();

	if(Predmet)
		pocercastic = p_Level->VodniCakanec1[r].Sizeof / 2;
	else
		pocercastic = p_Level->VodniCakanec1[r].Sizeof;

	pCastice = (PAR_STREPINA *) malloc(pocercastic * sizeof(PAR_STREPINA));

	if(!pCastice)
		return;

	memcpy((void *) pCastice, (void *)p_Level->VodniCakanec1[r].pCastice, pocercastic * sizeof(PAR_STREPINA));
	
	//pregeneruj parametry podle vysky padu
	for(i=0;i<pocercastic;i++)
	{
		v = (pCastice[i].vel1 + pCastice[i].vel2 + pCastice[i].vel3 + pCastice[i].vel4)/2.0f;
			
		if(Predmet)
		{
			pCastice[i].rychlost = (((randf()/(float)(RAND_MAX)) * v) * (8.0f + (vyska*0.25f))) * _3DKOREKCE;
			
			if(pCastice[i].rychlost < 0.1f * _3DKOREKCE)
					pCastice[i].rychlost = 0.1f * _3DKOREKCE;
			
			pCastice[i].p.z = ((rand()&0x1 ? randf() : -randf())/(float)RAND_MAX) / 5.0f;
			pCastice[i].p.x = ((rand()&0x1 ? randf() : -randf())/(float)RAND_MAX) / 5.0f;
		}
		else
		{
			pCastice[i].rychlost = (((randf()/(float)(RAND_MAX)) * v) * (8.0f + (vyska*0.5f))) * _3DKOREKCE;
			
			if(pCastice[i].rychlost < 0.1f * _3DKOREKCE)
					pCastice[i].rychlost = 0.1f * _3DKOREKCE;
		}
	}
	
	ph = par_vyrob();

	material = kom_najdi_material("ckapka2");
	
	if(material == -1)
		kprintf(1,"Nelze najit material ckapka2");

	par_set_param(ph, material, TPAR_YPLANE_LOW | TPAR_HTEST | TPAR_AUTOREMOVE, (BOD *)fpos, NULL);

	par_set_y_plane(ph, fpos[1] - 0.1f);

	par_vloz_strepy(ph,pCastice,pocercastic);

	par_pripoj_funkci(ph,anmend_ZrusCastice3,0,0,(void *) pCastice);

	par_go(ph, &p_Level->VodniCakanec1[r].flag, 0, 0);
}

void am_Do_Vodni_Cakanec2(float *pos, int VyskaPadu, int predmet, LEVELINFO *p_Level)
{
	int i,r;
	PAR_STREPINA	*pCastice;
	int pocercastic;
	int ph;
	int material;
	float v;
	float fpos[3];

	memcpy((void *) fpos, (void *)pos, 3* sizeof(float));

	fpos[1] += 0.7f;

	for(i=0;i<10;i++)
	{
		r = rand()%10;
		if(p_Level->VodniCakanec2[r].flag == -1)
			break;
	}
	
	if(i == 10)
	{
		for(i=0;i<10;i++)
		{
			if(p_Level->VodniCakanec2[i].flag == -1)
				break;
		}

		if(i == 10)
			return;

		r = i;
	}

	if(predmet)
		pocercastic = p_Level->VodniCakanec1[r].Sizeof / 2;
	else
		pocercastic = p_Level->VodniCakanec1[r].Sizeof;

	p_Level->VodniCakanec2[r].dwStart = timeGetTime();

	pCastice = (PAR_STREPINA *) malloc(pocercastic * sizeof(PAR_STREPINA));

	if(!pCastice)
		return;

	memcpy((void *) pCastice, (void *)p_Level->VodniCakanec2[r].pCastice, pocercastic * sizeof(PAR_STREPINA));
	
	//pregeneruj parametry podle vysky padu
	for(i=0;i<pocercastic;i++)
	{
		v = (pCastice[i].vel1 + pCastice[i].vel2 + pCastice[i].vel3 + pCastice[i].vel4)/2.0f;
			
		if(!predmet)
			pCastice[i].rychlost = (((randf()/(float)(RAND_MAX)) * v) * (8.0f + VyskaPadu)) * _3DKOREKCE;
		else
			pCastice[i].rychlost = (((randf()/(float)(RAND_MAX)) * v) * (8.0f + (VyskaPadu / 2.0f))) * _3DKOREKCE;
			
		if(pCastice[i].rychlost < 0.1f * _3DKOREKCE)
			pCastice[i].rychlost = 0.1f * _3DKOREKCE;
	}
	
	ph = par_vyrob();

	if(!ph)
		return;

	material = kom_najdi_material("ckapka2");

	if(material == -1)
		kprintf(1,"Nelze najit material ckapka2");
	
	par_set_param(ph, material, TPAR_YPLANE_LOW | TPAR_HTEST | TPAR_AUTOREMOVE, (BOD *)fpos, NULL);

	par_set_y_plane(ph, fpos[1] - 0.1f);

	par_vloz_strepy(ph,pCastice,pocercastic);

	par_pripoj_funkci(ph,anmend_ZrusCastice3,0,0,(void *) pCastice);

	par_go(ph, &p_Level->VodniCakanec2[r].flag, 0, 0);
}

int am_Get_Free_Lift_VParticles(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->LiftVParticles[i].System == -1)
			return i;

	return -1;
}

void am_Do_Lift_VParticles(float *pos, int mesh, LEVELINFO *p_Level)
{
	int k,m;
	PAR_KOUR_STOPA	*pKourovaS;
	int size = (int)(500 / p_Level->KvalitaCastic);

	k = am_Get_Free_Lift_VParticles(p_Level);

	if(k == -1)
		return;

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	p_Level->LiftVParticles[k].pCastice = pKourovaS;
	p_Level->LiftVParticles[k].Sizeof = size;
	p_Level->LiftVParticles[k].dwStart = timeGetTime();

	m = kom_najdi_material("ckapka1");
	
	if(m == -1)
		kprintf(1,"Nelze najit material ckapka1");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX));
	pKourovaS[0].utlum_x = 0;
	pKourovaS[0].utlum_y = 0;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 1.0f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	p_Level->LiftVParticles[k].System = par_vyrob();

	par_set_param(p_Level->LiftVParticles[k].System, m, TPAR_NO_FOG | TPAR_YPLANE_LOW | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR, (BOD *)pos, NULL);
	par_vloz_kour_stopu(p_Level->LiftVParticles[k].System, pKourovaS, p_Level->LiftVParticles[k].Sizeof);

	p_Level->LiftVParticles[k].hHnizdo[0] = par_vloz_hnizdo(p_Level->LiftVParticles[k].System);

	par_vloz_hnizdo_komplet(p_Level->LiftVParticles[k].hHnizdo[0], 1, (BOD *)p_Level->LiftVParticles[k].pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(p_Level->LiftVParticles[k].hHnizdo[0], 1, -1);
	
	memcpy((void *)p_Level->LiftVParticles[k].pivot[0], (void *)pos, 3*sizeof(float));

	p_Level->LiftParticles[k].pivot[0][0] += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
	p_Level->LiftParticles[k].pivot[0][1] --;
	p_Level->LiftParticles[k].pivot[0][2] += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);

	p_Level->LiftParticles[k].dir[0][0] = 0;
	p_Level->LiftParticles[k].dir[0][1] = ((-randf()/(float)(RAND_MAX)) / 10.0f)  * _3DKOREKCE;
	
	if(p_Level->LiftVParticles[k].dir[0][1] > (-0.03f) * _3DKOREKCE)
		p_Level->LiftVParticles[k].dir[0][1] = (-0.03f) * _3DKOREKCE;

	p_Level->LiftVParticles[k].dir[0][2] = 0;

	par_vloz_hnizdo_dir(p_Level->LiftVParticles[k].hHnizdo[0], (BOD *)p_Level->LiftVParticles[k].dir[0]);

	par_vloz_hnizdo_y_plane(p_Level->LiftVParticles[k].hHnizdo[0], pos[1] + 0.7f);
	
	p_Level->LiftVParticles[k].dwStart = timeGetTime();
	p_Level->LiftVParticles[k].dwTime = 0;

	par_pripoj_funkci(p_Level->LiftVParticles[k].System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	p_Level->LiftVParticles[k].hHnizdo[1] = mesh;
	par_go(p_Level->LiftVParticles[k].System, &p_Level->LiftVParticles[k].flag, 0, 0);
}

int am_Get_Free_VodniKolaB(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<100;i++)
		if(p_Level->VodniKolaB[i].System == -1)
			return i;

	return -1;
}

int am_Find_Corresponding_VodniKolaB(int mesh, LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<100;i++)
		if(p_Level->VodniKolaB[i].System != -1 && 
		   p_Level->VodniKolaB[i].hHnizdo[2] == mesh)
			return i;

	return -1;
}

int am_Kola_na_VodeB(float *pos, int Beruska, int mesh, int infinity, LEVELINFO *p_Level)
{
	int k,m;
	PAR_KOUR_STOPA	*pKourovaS;
	PARMETAC_HNIZDO *pHnizdo;
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	WATER_BOUNDARY	Boundary;
	int size = (int)(100 / p_Level->KvalitaCastic);

	pos[1] -= 0.5f;

	k = am_Find_Corresponding_VodniKolaB(mesh, p_Level);

	if(k != -1)
	{
		p_Level->VodniKolaB[k].dwStart = timeGetTime();
		par_vloz_hnizdo_diff(p_Level->VodniKolaB[k].hHnizdo[0],
							 1.0f,1.0f,1.0f,0.6f,
							 -0.25f,-0.25f,-0.25f,
							 -0.25f, 0);

		par_vloz_hnizdo_pivot(p_Level->VodniKolaB[k].hHnizdo[0], 
							  (BOD *)p_Level->VodniKolaB[k].pivot[0]);

		w_Choose_Best_Boundary(&Boundary, pos[0], pos[2], pos[1] - 0.5f, 
							   WATER_BOUNDARY_METHOD_SIZE_HEURISTICS);

		par_vloz_hnizdo_clip(p_Level->VodniKolaB[k].hHnizdo[0],
							 Boundary.Left, Boundary.Bottom,
							 Boundary.Right, Boundary.Top);

		return -1;
	}

	k = am_Get_Free_VodniKolaB(p_Level);

	if(k == -1)
		return -1;
	
	pSystem = &p_Level->VodniKolaB[k];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return -1;
	
	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;
	pSystem->dwStart = timeGetTime();

	m = kom_najdi_material("cvkolo1");
	
	if(m == -1)
		kprintf(1,"Nelze najit material cvkolo1");

	if(!Beruska)
		pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = 1.5f;
	else
		pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = 0.5f;

	pKourovaS[0].utlum_x = 1.75f;
	pKourovaS[0].utlum_y = 1.75f;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 0.6f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = -0.25f;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_3D | TPAR_SCALE_ADD | 
				  TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_2DCLIP, (BOD *)pos, NULL);
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 400, (BOD *)pSystem->pivot[0], pKourovaS);

	if(p_Level->bSikminaDown)
		par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 400, 500);
	else
		par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 400, -400);
	
	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][1] += 0.7f;

	if(pSystem->hHnizdo[0])
		pHnizdo = par_cti_hnizdo(pSystem->hHnizdo[0]);

	pSystem->nx[0][0] = 1;
	pSystem->nx[0][1] = 0;
	pSystem->nx[0][2] = 0;
	pSystem->ny[0][0] = 0;
	pSystem->ny[0][1] = 0;
	pSystem->ny[0][2] = 1;

	pHnizdo->p_nx = (BOD *) pSystem->nx[0];
	pHnizdo->p_ny = (BOD *) pSystem->ny[0];

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwExpire = 0;
	pSystem->dwExpireTime = 0;
	
	if(!infinity)
		pSystem->dwStop = 500;
	else
		pSystem->dwStop = 7200000;

	pSystem->hHnizdo[1] = Beruska;
	pSystem->hHnizdo[2] = mesh;

	//if(infinity)
	w_Choose_Best_Boundary(&Boundary, pos[0], pos[2], pos[1] - 0.5f, WATER_BOUNDARY_METHOD_SIZE_HEURISTICS);
	par_vloz_hnizdo_clip(pSystem->hHnizdo[0], Boundary.Left, Boundary.Bottom,
						 Boundary.Right, Boundary.Top);
	
	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);

	return k;
}

void am_Release_BublSystem(int i, LEVELINFO *p_Level)
{
	par_zrus(p_Level->BublSystem[i].System.System);
	p_Level->BublSystem[i].System.System = -1;	
}

void am_Do_BublSystem(int i, LEVELINFO *p_Level)
{
	float pos[3], f;
	int	  rot;

	SYSTEMKOUROVYCHCASTIC	*pSystem = &p_Level->BublSystem[i].System;

	if(!p_Level->bGameResume)
		pSystem->dwTime += ber.TimeLastFrame;

	if(pSystem->dwTime > pSystem->dwStop)
	{
		pSystem->dwTime = 0;
		

		if(p_Level->BublSystem[i].bOn)
		{
			p_Level->BublSystem[i].bOn = 0;
			par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 100000, 0);
			pSystem->dwStop = 3000;
		}
		else
		{
			pSystem->dwStop = 1000 + p_Level->BublSystem[i].iStart;
			p_Level->BublSystem[i].bOn = 1;
			par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 1, p_Level->BublSystem[i].iStart);
			p_Level->BublSystem[i].iStart = 0;
		}
	}

	kom_mesh_get_float(p_Level->BublSystem[i].mesh, &pos[0], &pos[1], &pos[2], &rot);

	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][0] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;
	pSystem->pivot[0][1] += 0.70f;
	pSystem->pivot[0][2] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;

	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 5.0f;
	
	if(pSystem->dir[0][1] < 1.0f)
		pSystem->dir[0][1] = 1.0f;

	f = (randf()/(float)(RAND_MAX)) / 7.0f;
	par_vloz_hnizdo_scale(pSystem->hHnizdo[0], f, f, 0, 0);

	if(am_Find_Water(i, &f, p_Level))
		par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], f);
}

int am_Find_Water(int i, float *f, LEVELINFO *p_Level)
{
	long r;
	int j;
	int iPos[3];
	float pos[3];

	memcpy((void *) iPos, (void *)p_Level->Item[p_Level->BublSystem[i].iItem].Pos, 3 * sizeof(int));

	iPos[2]++;

	for(j = iPos[2];j<p_Level->Size[2];j+=2)
	{
		gl_Logical2Real(iPos[0], iPos[1], j, &r, p_Level);

		if(p_Level->Level[r])
			if(p_Level->Level[r]->p_Object->Class == 12)
			{
				kom_get_fyz_souradnice(iPos[0], j, iPos[1], (BOD *)pos);
				*f = pos[1] + 0.6f;

				return 1;
			}

      gl_Logical2Real(iPos[0], iPos[1], j+1, &r, p_Level);

      if(p_Level->Level[r])
			  if(p_Level->Level[r]->p_Object->Class == 2)
			  {
				  kom_get_fyz_souradnice(iPos[0], j, iPos[1], (BOD *)pos);
				  *f = pos[1];

				  return 1;
			  }
	}	

	*f = ber.y_start + 50;

	return p_Level->bUnderWaterLevel;
}

int am_Find_WaterB(int* viPos, float *f, LEVELINFO *p_Level)
{
	long r;
	int j;
	int iPos[3];
	float pos[3];

	memcpy((void *) iPos, (void *)viPos, 3 * sizeof(int));

	iPos[2]++;

	for(j = iPos[2];j<p_Level->Size[2];j+=2)
	{
		gl_Logical2Real(iPos[0], iPos[1], j, &r, p_Level);

		if(p_Level->Level[r])
			if(p_Level->Level[r]->p_Object->Class == 12)
			{
				kom_get_fyz_souradnice(iPos[0], j, iPos[1], (BOD *)pos);
				*f = pos[1] + 0.6f;

				return 1;
			}
	}	

	*f = ber.y_start + 50;

	return p_Level->bUnderWaterLevel;
}

void am_Create_BublSystem(int i, LEVELINFO *p_Level)
{
	float pos[3], f;
	int m, rot;
	PAR_KOUR_STOPA	*pKourovaS;
	SYSTEMKOUROVYCHCASTIC	*pSystem = &p_Level->BublSystem[i].System;
	int size = (int)(500 / p_Level->KvalitaCastic);

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;

	m = kom_najdi_material("cbublina1");
	
	if(m == -1)
		kprintf(1,"Nelze najit material cbublina1");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 7.0f;
	
	pKourovaS[0].utlum_x = 0;
	pKourovaS[0].utlum_y = 0;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 0.8f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	kom_mesh_get_float(p_Level->BublSystem[i].mesh, &pos[0], &pos[1], &pos[2], &rot);
	pos[1]++;
	
	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR, (BOD *)pos, NULL);
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 100000, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 100000, -100000);
	
	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][0] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;
	pSystem->pivot[0][1] += 0.70f;
	pSystem->pivot[0][2] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;

	pSystem->dir[0][0] = 0;
	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 5.0f;
	
	if(pSystem->dir[0][1] < 1.0f)
		pSystem->dir[0][1] = 1.0f;

	pSystem->dir[0][2] = 0;

	par_vloz_hnizdo_dir(pSystem->hHnizdo[0], (BOD *)pSystem->dir[0]);

	if(am_Find_Water(i, &f, p_Level))
		par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], f);

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwStop = 2000;
	p_Level->BublSystem[i].bOn = 0;

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);

}

void am_Do_Bubliny_Berusek(LEVELINFO *p_Level)
{
	long real;

	int i, j;

	for(i=0;i<6;i++)
		if(p_Level->BublSystem[i].iItem != -1)
		{
			j = p_Level->BublSystem[i].iItem;

			gl_Logical2Real(p_Level->Item[j].Pos[0], p_Level->Item[j].Pos[1],
							p_Level->Item[j].Pos[2], &real, p_Level);
			
			if(p_Level->Square[real].bUnderWater &&
			   p_Level->BublSystem[i].System.System == -1)
				am_Create_BublSystem(i, p_Level);
			else
				if(p_Level->Square[real].bUnderWater &&
				   p_Level->BublSystem[i].System.System != -1)
					am_Do_BublSystem(i, p_Level);
				else
					if(!p_Level->Square[real].bUnderWater &&
					   p_Level->BublSystem[i].System.System != -1)
						am_Release_BublSystem(i, p_Level);
		}
}


void am_Do_BublVybuchy(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<20;i++)
	{
		if(p_Level->BublVybuch[i].System != -1)
		{
			if(p_Level->BublVybuch[i].hHnizdo[0])
			{
				par_vloz_hnizdo_pivot(p_Level->BublVybuch[i].hHnizdo[0], NULL);

				if(!par_get_hnizda(p_Level->BublVybuch[i].System))
				{
					par_zrus(p_Level->BublVybuch[i].System);
					p_Level->BublVybuch[i].System = -1;	
				}
			}
		}
	}
}

int am_Get_Free_BublVybuch(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<20;i++)
		if(p_Level->BublVybuch[i].System == -1)
			return i;

	return -1;
}

void am_Do_Vybuch_Bublin(int *iPos, float *fPos, LEVELINFO *p_Level)
{
	float f;
	int m;
	PAR_KOUR_STOPA	*pKourovaS;
	PAR_KOUR_STOPA	*pCastice;
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	int size = (int)(500 / p_Level->KvalitaCastic);
	
	m = am_Get_Free_BublVybuch(p_Level);

	if(m == -1)
		return;

	pSystem = &p_Level->BublVybuch[m];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;

	m = kom_najdi_material("cbublina1");

	if(m == -1)
		kprintf(1,"Nelze najit material cbublina1");
	
	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 4.5f;
		
	pKourovaS[0].utlum_x = 0;
	pKourovaS[0].utlum_y = 0;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 0.8f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR, (BOD *)fPos, NULL);
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 1, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 1, -1);
	
	memcpy((void *)pSystem->pivot[0], (void *)fPos, 3*sizeof(float));

	if(am_Find_WaterB(iPos, &f, p_Level))
	{
		float fpos[3];

		par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], f);

		fpos[0] = pSystem->pivot[0][0];
		fpos[2] = pSystem->pivot[0][2];
		//fpos[1] = pSystem->pivot[0][1] + (f - 0.6f);
		fpos[1] = f;

		if(!p_Level->bUnderWaterLevel)
			gl_Do_Strepiny_Na_Vode(fpos, p_Level, -0.7f);
	}

	pCastice = par_vloz_hnizdo_pust_castice(pSystem->System, pSystem->hHnizdo[0], size);

	while(pCastice)
	{

		pCastice->p.x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX));
		pCastice->p.y += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX));
		pCastice->p.z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX));

		pCastice->dir.x = 0;
		pCastice->dir.y = (randf()/(float)(RAND_MAX)) * 5.0f;

		if(pCastice->dir.y < 1.0f)
			pCastice->dir.y = 1.0f;
	
		pCastice->dir.z = 0;

		pCastice->rychlost_x = pCastice->rychlost_y = (randf()/(float)(RAND_MAX)) / 4.5f;

		pCastice = pCastice->p_next;
	}
	
	pSystem->dir[0][0] = 0;
	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 5.0f;
	
	if(pSystem->dir[0][1] < 1.0f)
		pSystem->dir[0][1] = 1.0f;

	pSystem->dir[0][2] = 0;

	par_vloz_hnizdo_dir(pSystem->hHnizdo[0], (BOD *)pSystem->dir[0]);

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwStop = 0;

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);
}

void am_Do_Exit_Efects(LEVELINFO *p_Level)
{
	EXITEFECT	*pEEfect;
	int i;

	for(i=0;i<10;i++)
		if(p_Level->ExitEfect[i].System.System != -1)
		{
			pEEfect = &p_Level->ExitEfect[i];

			if(!p_Level->bGameResume)
				pEEfect->System.dwTime += ber.TimeLastFrame;

			if(pEEfect->System.dwTime > pEEfect->System.dwStop)
			{
				if(pEEfect->System.hHnizdo[0])
					par_vloz_hnizdo_pivot(pEEfect->System.hHnizdo[0], NULL);
			
				if(pEEfect->System.hHnizdo[1])
					par_vloz_hnizdo_pivot(pEEfect->System.hHnizdo[1], NULL);
			}

			if(par_get_hnizda(pEEfect->System.System) < 2)
			{
				par_zrus(pEEfect->System.System);
				pEEfect->System.System = -1;	

				if(pEEfect->hSvetlo != -1)
				{
					sdl_svetlo_zrus(pEEfect->hSvetlo);
					pEEfect->hSvetlo = -1;
				}

				if(pEEfect->hEXSvetlo[0] != -1)
				{
					edl_svetlo_zrus(pEEfect->hEXSvetlo[0]);
					pEEfect->hEXSvetlo[0] = -1;
				}

				if(pEEfect->hEXSvetlo[1] != -1)
				{
					edl_svetlo_zrus(pEEfect->hEXSvetlo[1]);
					pEEfect->hEXSvetlo[1] = -1;
				}

				lani_set(pEEfect->mesh, 0, K_CHYBA, &p_Level->TrashFlag,0,0,0);
				kom_zrus_prvek(pEEfect->mesh);
			}
		}
}

int am_Get_Free_ExitSystem(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->ExitEfect[i].hSvetlo == -1)
			return i;

	return -1;
}

void am_Do_Exit(int Bmesh, int Emesh, LEVELINFO *p_Level)
{
	int m,k;
	PAR_KOUR_STOPA	*pKourovaS;
	PARMETAC_HNIZDO *pHnizdo;
	SYSTEMKOUROVYCHCASTIC	*pSystem;

	EXITEFECT	*pEEfect;
	int i, hSvetlo;
	float pos[3];
	int	  rot;

	i = am_Get_Free_ExitSystem(p_Level);

	if( i == -1)
		return;

	pEEfect = &p_Level->ExitEfect[i];
	pEEfect->mesh = Bmesh;

	hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);

	if(hSvetlo != -1)
	{
		kom_mesh_get_float(Bmesh, &pos[0], &pos[1], &pos[2], &rot);

		sdl_svetlo_set_pos(hSvetlo, (BOD *)pos);
		sdl_svetlo_set_diff(hSvetlo, 1, 1, 1, 4, 10, 10);

		sdl_anim_vyrob(hSvetlo, 15,0,0,2);
		sdl_anim_vloz_klic_vzdal(hSvetlo,0,10,4,10,0);
		sdl_anim_vloz_klic_vzdal(hSvetlo,1,4,4,4,14);
		sdl_anim_start(hSvetlo, &p_Level->TrashFlag,0,0,0);

		pEEfect->hSvetlo = hSvetlo;
	}
	else
		pEEfect->hSvetlo = -1;
	
	pEEfect->hEXSvetlo[0] = edl_svetlo_vyrob(EDL_BODOVE | EDL_GOURAND | EDL_SPEC_ADD | 
										     EDL_DOSAH | EDL_UTLUM_LIN, 0);

	hSvetlo = pEEfect->hEXSvetlo[0];

	if(hSvetlo != -1)
	{
		kom_mesh_get_float(Emesh, &pos[0], &pos[1], &pos[2], &rot);

		pos[1] += 1.0f;

		edl_svetlo_set_pos(hSvetlo, (BOD *)pos, (BOD *)pos);
		edl_svetlo_set_spec(hSvetlo, 1, 1, 1);
		edl_svetlo_set_par(hSvetlo, 1, 3, 0, 4.0f);
	}

	pEEfect->hEXSvetlo[1] = edl_svetlo_vyrob(EDL_MESH_LIGHT | EDL_PRUHL_LIGHT | EDL_ALFA_SET | 
										     EDL_DOSAH | EDL_UTLUM_KVAD | EDL_PLOSNE_Y, 1);

	hSvetlo = pEEfect->hEXSvetlo[1];

	if(hSvetlo != -1)
	{
		edl_svetlo_pridej_mesh(hSvetlo, Bmesh);
		edl_svetlo_uzavri_meshe(hSvetlo);

		kom_mesh_get_float(Bmesh, &pos[0], &pos[1], &pos[2], &rot);

		edl_svetlo_set_pos(hSvetlo, (BOD *)pos, (BOD *)pos);
		edl_svetlo_set_diff(hSvetlo, 1);
		edl_svetlo_set_par(hSvetlo, 1, 0, 0, 0);

		edl_anim_vyrob(hSvetlo, 20, 0, 0, 0, 0, 2);
		edl_anim_vloz_klic_vzdal(hSvetlo, 0, 0, 0, 1, 0);
		edl_anim_vloz_klic_vzdal(hSvetlo, 1, 1, 0, 1, 19);
		edl_anim_start(hSvetlo, &p_Level->TrashFlag, 0, 0, 0);
	}

//	pEEfect->hEXSvetlo[1] = -1;
	
	kom_mesh_get_float(Bmesh, &pos[0], &pos[1], &pos[2], &rot);

	pSystem = &pEEfect->System;

	pKourovaS = (PAR_KOUR_STOPA *) malloc(100 * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = 100;
	pSystem->dwStart = timeGetTime();

	m = kom_najdi_material("flare11");
	
	if(m == -1)
		kprintf(1,"Nelze najit material flare11");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = 1.0f;
	pKourovaS[0].utlum_x = 0;
	pKourovaS[0].utlum_y = 0;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 1.0f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_LOW | TPAR_DIR | TPAR_3D | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	
	for(k=0;k<2;k++)
	{
		pSystem->hHnizdo[k] = par_vloz_hnizdo(pSystem->System);

		par_vloz_hnizdo_komplet(pSystem->hHnizdo[k], 1000, (BOD *)pSystem->pivot[k], pKourovaS);
		par_vloz_hnizdo_timer(pSystem->hHnizdo[k], 1000, -1000);

		memcpy((void *)pSystem->pivot[k], (void *)pos, 3*sizeof(float));

		pSystem->dir[k][0] = 0;
		
		if(k == 0)
			pSystem->dir[k][1] = 1;
		else
		{
			pSystem->dir[k][1] = -1;
				
			par_vloz_hnizdo_y_plane(pSystem->hHnizdo[k], pos[1] - 0.8f);
		}

		pSystem->dir[k][2] = 0;
		vektor_norm((BOD *)pSystem->dir[k]);

		par_vloz_hnizdo_dir(pSystem->hHnizdo[k], (BOD *)pSystem->dir[k]);

		if(pSystem->hHnizdo[k])
			pHnizdo = par_cti_hnizdo(pSystem->hHnizdo[k]);

		pSystem->nx[k][0] = 1;
		pSystem->nx[k][1] = 0;
		pSystem->nx[k][2] = 0;
		pSystem->ny[k][0] = 0;
		pSystem->ny[k][1] = 0;
		pSystem->ny[k][2] = 1;

		pHnizdo->p_nx = (BOD *) pSystem->nx[k];
		pHnizdo->p_ny = (BOD *) pSystem->ny[k];

		pSystem->dwStart = timeGetTime();
		pSystem->dwTime = 0;
		pSystem->dwStop = 250;
	}

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);

	kom_mesh_get_float(Emesh, &pos[0], &pos[1], &pos[2], &rot);

	rot = rand()%3;

//	ap_Play_Sound(0, 0, 0, pos, 81 + rot, NULL, &ad);
}

int am_Get_Free_ExitSparksSystem(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->ExitSparks[i].System == -1)
			return i;

	return -1;
}

void am_Do_Exit_Sparks(LEVELINFO *p_Level)
{
	ITEMDESC *pItem;
	PAR_KOUR_STOPA *pCastice;
	int i, r;
	float f, pos[3];

	for(i=0;i<10;i++)
		if(p_Level->ExitSparks[i].System != -1)
		{
			if(!p_Level->bGameResume)
				p_Level->ExitSparks[i].dwTime += ber.TimeLastFrame;

			//mihotani jisker
			if(p_Level->ExitSparks[i].dwTime > 100)
			{
				if(p_Level->ExitSparks[i].hHnizdo[0])
				{
					pCastice = 	par_cti_hnizdo_castice(p_Level->ExitSparks[i].hHnizdo[0]);

					if(pCastice)
					{
						while(pCastice)
						{
							r = rand() %100;

							if(r < 10)
							{
								f = (randf()/(float)(RAND_MAX)) / 7.5f;
								pCastice->rychlost_x += f;
								pCastice->rychlost_y += f;
							}

							pCastice = pCastice->p_next;
						}
					}
				}

				p_Level->ExitSparks[i].dwTime = 0;
			}

			pItem = (ITEMDESC *) p_Level->ExitSparks[i].hHnizdo[1];

			kom_mesh_get_float(pItem->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &r);

			p_Level->ExitSparks[i].pivot[0][0] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
			p_Level->ExitSparks[i].pivot[0][2] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
			norm_vect_2D(&p_Level->ExitSparks[i].pivot[0][0], &p_Level->ExitSparks[i].pivot[0][2]);

			f = (randf()/(float)(RAND_MAX)) / 1.35f;

			p_Level->ExitSparks[i].pivot[0][0] *= f;
			p_Level->ExitSparks[i].pivot[0][2] *= f;

			p_Level->ExitSparks[i].pivot[0][0] += pos[0];
			p_Level->ExitSparks[i].pivot[0][2] += pos[2];

			f = (randf()/(float)(RAND_MAX)) / 10.0f;
			par_vloz_hnizdo_scale(p_Level->ExitSparks[i].hHnizdo[0], f,f, -0.5f, -0.5f);
			
		}
}

void am_Do_TelCSparks(LEVELINFO *p_Level)
{
	ITEMDESC *pItem;
	int i, r;
	float f, pos[3];
	PAR_KOUR_STOPA *pCastice;

	for(i=0;i<10;i++)
		if(p_Level->TelCSparks[i].System != -1)
		{
			// test na vypnuti efektu
			if(!par_get_hnizda(p_Level->TelCSparks[i].System))
			{
				par_zrus(p_Level->TelCSparks[i].System);
				p_Level->TelCSparks[i].System = -1;	
				sdl_svetlo_zrus(p_Level->TelCSparks[i].hHnizdo[2]);
				p_Level->TelCSparks[i].hHnizdo[2] = -1;
				continue;
			}

			pCastice = 	par_cti_hnizdo_castice(p_Level->TelCSparks[i].hHnizdo[0]);

			// test na velikost castic. Pokud je moc mala, tak pric s ni
			if(pCastice)
			{
				while(pCastice)
				{
					if(pCastice->rychlost_x < 0.001f ||
					   pCastice->rychlost_y < 0.001f)
						pCastice->a = 0;

					pCastice = pCastice->p_next;
				}
			}
			
			pItem = (ITEMDESC *) p_Level->TelCSparks[i].hHnizdo[1];

			if(pItem)
			{
				if((rand()%3))
					kom_mesh_get_float(pItem->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &r);
				else
				{
					int iPos[3];

					gl_Real2Logical(pItem->Connection[0], iPos, p_Level);
					kom_get_fyz_souradnice(iPos[0], iPos[2], iPos[1], (BOD *) pos);
				}

				p_Level->TelCSparks[i].pivot[0][0] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				p_Level->TelCSparks[i].pivot[0][2] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				norm_vect_2D(&p_Level->TelCSparks[i].pivot[0][0], &p_Level->TelCSparks[i].pivot[0][2]);

				f = (randf()/(float)(RAND_MAX)) * 0.75f;

				p_Level->TelCSparks[i].pivot[0][0] *= f;
				p_Level->TelCSparks[i].pivot[0][2] *= f;

				p_Level->TelCSparks[i].pivot[0][0] += pos[0];
				p_Level->TelCSparks[i].pivot[0][2] += pos[2];

				f = (randf()/(float)(RAND_MAX)) / 2.0f;
				par_vloz_hnizdo_scale(p_Level->TelCSparks[i].hHnizdo[0], f,f, -2.0f, -2.0f);

				p_Level->TelCSparks[i].dir[0][1] = (randf()/(float)(RAND_MAX)) * 5;
			}
		}
}

void am_Release_BarelSparks(LEVELINFO *p_Level, ITEMDESC *pBarel)
{
	int i;

	for(i=0;i<30;i++)
		if(p_Level->BarelSparks[i].System != -1 &&
		   p_Level->BarelSparks[i].hHnizdo[1] == (int)pBarel)
		{
			par_zrus(p_Level->BarelSparks[i].System);
			p_Level->BarelSparks[i].System = -1;	
			sdl_svetlo_zrus(p_Level->BarelSparks[i].hHnizdo[2]);
			p_Level->BarelSparks[i].hHnizdo[2] = -1;
			return;
		}
}

void am_Do_BarelSparks(LEVELINFO *p_Level)
{
	ITEMDESC *pItem;
	int i, r;
	float f, pos[3];
	PAR_KOUR_STOPA *pCastice;

	for(i=0;i<30;i++)
		if(p_Level->BarelSparks[i].System != -1)
		{
			// test na vypnuti efektu
			if(!par_get_hnizda(p_Level->BarelSparks[i].System))
			{
				par_zrus(p_Level->BarelSparks[i].System);
				p_Level->BarelSparks[i].System = -1;	
				sdl_svetlo_zrus(p_Level->BarelSparks[i].hHnizdo[2]);
				p_Level->BarelSparks[i].hHnizdo[2] = -1;
				continue;
			}

			pCastice = 	par_cti_hnizdo_castice(p_Level->BarelSparks[i].hHnizdo[0]);

			// test na velikost castic. Pokud je moc mala, tak pric s ni
			if(pCastice)
			{
				while(pCastice)
				{
					if(pCastice->rychlost_x < 0.001f ||
					   pCastice->rychlost_y < 0.001f)
						pCastice->a = 0;

					pCastice = pCastice->p_next;
				}
			}
			
			pItem = (ITEMDESC *) p_Level->BarelSparks[i].hHnizdo[1];

			if(pItem)
			{
				kom_mesh_get_float(pItem->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &r);

				p_Level->BarelSparks[i].pivot[0][0] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				p_Level->BarelSparks[i].pivot[0][1] = pos[1]+1;
				p_Level->BarelSparks[i].pivot[0][2] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				norm_vect_2D(&p_Level->BarelSparks[i].pivot[0][0], &p_Level->BarelSparks[i].pivot[0][2]);

				f = (randf()/(float)(RAND_MAX)) * 0.75f;

				p_Level->BarelSparks[i].pivot[0][0] *= f;
				p_Level->BarelSparks[i].pivot[0][2] *= f;

				p_Level->BarelSparks[i].pivot[0][0] += pos[0];
				p_Level->BarelSparks[i].pivot[0][2] += pos[2];

				f = (randf()/(float)(RAND_MAX)) / 2.0f;
				par_vloz_hnizdo_scale(p_Level->BarelSparks[i].hHnizdo[0], f,f, -2.0f, -2.0f);

				p_Level->BarelSparks[i].dir[0][1] = (randf()/(float)(RAND_MAX)) * 1;

				pos[1] += 1.25f;

				sdl_svetlo_set_pos(p_Level->BarelSparks[i].hHnizdo[2], (BOD *)pos);
			}
		}
}

void am_Create_Exit_Sparks(ITEMDESC *pExit, char Down, LEVELINFO *p_Level)
{
	int m,k;
	PAR_KOUR_STOPA	*pKourovaS;
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	float pos[3];
	int rot;
	int size = (int)(100 / p_Level->KvalitaCastic);

	k = am_Get_Free_ExitSparksSystem(p_Level);

	if( k == -1)
		return;

	pSystem = &p_Level->ExitSparks[k];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	kom_mesh_get_float(pExit->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;
	pSystem->dwStart = timeGetTime();

	m = kom_najdi_material("flare12");
	
	if(m == -1)
		kprintf(1,"Nelze najit material flare12");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 10.0f;
	pKourovaS[0].utlum_x = -0.5f;
	pKourovaS[0].utlum_y = -0.5f;

	pKourovaS[0].r = 0.98f;
	pKourovaS[0].g = 0.91f;
	pKourovaS[0].b = 0.33f;

	pKourovaS[0].a = 1.0f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	if(Down)
		par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_LOW | TPAR_DIR | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);
	else
		par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_DIR | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);
	
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 10, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 10, -10);

	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

	if(Down)
		pSystem->pivot[0][1] += 0.8f;
	else
		pSystem->pivot[0][1] -= 0.8f;

	pSystem->dir[0][0] = 0;
	
	if(Down)
		pSystem->dir[0][1] = -1;
	else
		pSystem->dir[0][1] = 1;

	pSystem->dir[0][2] = 0;				

	if(Down)
		par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], pos[1] - 0.8f);
	else
		par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], pos[1] + 0.8f);

	par_vloz_hnizdo_dir(pSystem->hHnizdo[0], (BOD *)pSystem->dir[0]);

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwStop = 0;
	pSystem->hHnizdo[1] = (int)pExit;

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);
}

int am_Get_Free_TelCSparksSystem(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->TelCSparks[i].System == -1)
			return i;

	return -1;
}

int am_Create_TelCSparks(ITEMDESC *pTel, LEVELINFO *p_Level)
{
	int m,k;
	PAR_KOUR_STOPA	*pKourovaS;
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	float pos[3];
	int rot;
	int size = (int)(300 / p_Level->KvalitaCastic);

	k = am_Get_Free_TelCSparksSystem(p_Level);

	if( k == -1)
		return -1;

	pSystem = &p_Level->TelCSparks[k];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return -1;
	
	kom_mesh_get_float(pTel->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;
	pSystem->dwStart = timeGetTime();

	m = kom_najdi_material("flare104");
	
	if(m == -1)
		kprintf(1,"Nelze najit material flare104");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 3.0f;
	pKourovaS[0].utlum_x = -2;
	pKourovaS[0].utlum_y = -2;

	pKourovaS[0].r = 0;
	pKourovaS[0].g = 0.8f;
	pKourovaS[0].b = 0.97f;

	pKourovaS[0].a = 1.0f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_DIR | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);
	
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 10, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 10, -10);

	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][1]--;

	pSystem->dir[0][0] = 0;
	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 5;
	pSystem->dir[0][2] = 0;				

	par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], pos[1] + 3);

	par_vloz_hnizdo_dir(pSystem->hHnizdo[0], (BOD *)pSystem->dir[0]);

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwStop = 0;
	pSystem->hHnizdo[1] = (int)pTel;

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);

	return k;
}

void am_TurnOn_The_Teleport(ITEMDESC *pTel, LEVELINFO *p_Level)
{
	float pos[3];
	int hSvetlo, rot;
	int i = am_Create_TelCSparks(pTel, p_Level);
	int rnd = rand()%3;

	if (i == -1)
		return;

	hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_KVADRATIC);

	kom_mesh_get_float(pTel->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

	if(hSvetlo != -1)
	{
		pos[1]++;
		sdl_svetlo_set_pos(hSvetlo, (BOD *)pos);
		sdl_svetlo_set_diff(hSvetlo, 1, 1, 1, 2, 3, 3);

		sdl_anim_vyrob(hSvetlo, 20,0,0,2);
		sdl_anim_vloz_klic_vzdal(hSvetlo,0,0,0,0,0);
		sdl_anim_vloz_klic_vzdal(hSvetlo,1,3,2,3,19);
		sdl_anim_start(hSvetlo, &p_Level->TrashFlag,0,0,0);

		p_Level->TelCSparks[i].hHnizdo[2] = hSvetlo;
	}
	else
		p_Level->TelCSparks[i].hHnizdo[2] = -1;

//	ap_Play_Sound(0,0,0, pos, rnd+71, NULL, &ad);	
}

int am_Find_TelCSparks(ITEMDESC * pTel, LEVELINFO *p_Level)
{
	ITEMDESC *pItem;
	int i;

	for(i=0;i<10;i++)
		if(p_Level->TelCSparks[i].System != -1)
		{
			pItem = (ITEMDESC *) p_Level->TelCSparks[i].hHnizdo[1];

			if(pItem)
				if(pItem->iItem == pTel->iItem)
					return i;
		}

	return -1;
}

void am_TurnOff_The_Teleport(ITEMDESC *pTel, LEVELINFO *p_Level)
{
	int hSvetlo;
	float pos[3];
	int i = am_Find_TelCSparks(pTel, p_Level);
	int rnd = rand()%3;

	if (i == -1)
		return;

	hSvetlo = p_Level->TelCSparks[i].hHnizdo[2];

	if(hSvetlo != -1)
	{
		sdl_anim_vyrob(hSvetlo, 20,0,0,2);
		sdl_anim_vloz_klic_vzdal(hSvetlo,0,3,2,3,0);
		sdl_anim_vloz_klic_vzdal(hSvetlo,1,0,0,0,19);
		sdl_anim_start(hSvetlo, &p_Level->TrashFlag,0,0,0);
	}

	if(p_Level->TelCSparks[i].hHnizdo[0])
		par_vloz_hnizdo_pivot(p_Level->TelCSparks[i].hHnizdo[0], NULL);

	p_Level->TelCSparks[i].hHnizdo[1] = 0;

	kom_get_fyz_souradnice(pTel->Pos[0], pTel->Pos[2], pTel->Pos[1], (BOD *) pos);

//	ap_Play_Sound(0,0,0, pos, rnd+68, NULL, &ad);	
}

void am_Create_Sparks_To_Exits(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<p_Level->Size_of_Level;i++)
		if(p_Level->Level[i])
		{
			if(p_Level->Level[i]->p_Object->Class == 4)
			{
				am_Create_Exit_Sparks(p_Level->Level[i], 1, p_Level);
				am_Create_Exit_Sparks(p_Level->Level[i], 0, p_Level);
			}

			if(p_Level->Level[i]->p_Object->Class == 9 && 
			   !p_Level->Level[i]->p_Object->SubClass)
				am_Create_TelCSparks(p_Level->Level[i], p_Level);
		}
}

int am_Counet_Waters(LEVELINFO *p_Level)
{
	int i, c = 0;

	for(i=0;i<p_Level->Size_of_Level;i++)
		if(p_Level->Level[i])
			if(p_Level->Level[i]->p_Object->Class == 12)
				c++;

	return c;
}

ITEMDESC *am_Find_Next_Water(int *LastIndex, LEVELINFO *p_Level)
{
	int i,x = (*LastIndex) + 1;

	for(i=x;i<p_Level->Size_of_Level;i++)
		if(p_Level->Level[i])
			if(p_Level->Level[i]->p_Object->Class == 12)
			{
				(*LastIndex) = i;
				return p_Level->Level[i];
			}

	(*LastIndex) = -1;
	return NULL;
}

void am_Do_Rain(RAINSYSTEM *pRain)
{
	float dir[6], pos[3];
	int i;

	// pregeneruj pivoty hnizd vodnich kol
	if(pRain->bWaterCircles)
	for(i=0;i<pRain->SizeofWHnizda;i++)
	{
		memcpy((void *) &pRain->Wpivot[i], (void *) &pRain->Wpos[i], sizeof(BOD));

		pRain->Wpivot[i].x += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
		pRain->Wpivot[i].z += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
		pRain->Wpivot[i].y -= 0.3f;
	}

	// pregeneruj pivoty hnizd deste
	if(pRain->bRain)
	{
		kam_get_float_ext(dir, pos);

		//kprintf(1,"y: %f + %f = %f | floor: %f", pos[1], pRain->fSky, pos[1] + pRain->fSky, ber.y_start);

		memcpy((void *)&pRain->RainCenter, (void *)pos, sizeof(BOD));

		for(i=0;i<pRain->SizeofHnizda;i++)
		{
			memcpy((void *)&pRain->pivot[i], (void *)&pRain->RainCenter, sizeof(BOD));

			pRain->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pRain->fDist;
			pRain->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pRain->fDist;
			//pRain->pivot[i].y = pRain->fSky;
			//pRain->pivot[i].y = pos[1] + 1.0f;
			pRain->pivot[i].y = pos[1] + pRain->fSky;
		}
	}
}

void am_Do_Snow(SNOWSYSTEM *pSnow)
{
	float dir[6], pos[3];
	int i;

	// pregeneruj pivoty hnizd snehu
	if(pSnow->bSnow)
	{
		kam_get_float_ext(dir, pos);

		memcpy((void *)&pSnow->SnowCenter, (void *)pos, sizeof(BOD));

		for(i=0;i<pSnow->SizeofHnizda;i++)
		{
			memcpy((void *)&pSnow->pivot[i], (void *)&pSnow->SnowCenter, sizeof(BOD));

			pSnow->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pSnow->fDist;
			pSnow->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pSnow->fDist;
			//pSnow->pivot[i].y = pSnow->fSky;
			//pSnow->pivot[i].y = pos[1] + 1.0f;
			pSnow->pivot[i].y = pos[1] + pSnow->fSky;
			assert(fabs(pSnow->pivot[i].x) < 1000);
		}
	}
}

void am_Do_NESChmiri(NATUREEFFECTSYSTEM *pNESystem)
{
	float dir[6], pos[3];
	int j;

	if(!Level.bGameResume)
		pNESystem->dwTime += ber.TimeLastFrame;

	if(pNESystem->dwTime > pNESystem->dwRegTime)
	{
		pNESystem->dwTime = 0;
			
		kam_get_float_ext(dir, pos);

		for(j=0; j<pNESystem->SizeofHnizda; j++)
		{
			memcpy((void *)&pNESystem->pivot[j], (void *)pos, sizeof(BOD));

			pNESystem->pivot[j].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pNESystem->fRadius;
			pNESystem->pivot[j].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pNESystem->fRadius;
			//pNESystem->pivot[j].y = pNESystem->fSky;
			//pNESystem->pivot.y = pos[1] + 1.0f;
			pNESystem->pivot[j].y = pos[1] + pNESystem->fSky;

			pNESystem->dir[j].x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			//pNESystem->dir[j].y = -0.15f;
			pNESystem->dir[j].y = (-randf()/(float)(RAND_MAX)) * 0.25f;
			pNESystem->dir[j].z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
		}
	}
}

void am_Do_NESSvetluska(NATUREEFFECTSYSTEM *pNESystem)
{
	float dir[6], pos[3];
	int j;

	if(!Level.bGameResume)
		pNESystem->dwTime += ber.TimeLastFrame;

	if(!Level.bGameResume)
		pNESystem->dwGenTimeCounter += ber.TimeLastFrame;

	if(pNESystem->dwGenTimeCounter > pNESystem->dwGenTime)
	{
		pNESystem->dwGenTimeCounter = 0;

		if(!(rand()%pNESystem->iGenProbability))
		{
			int r = rand()%pNESystem->SizeofHnizda;

			kam_get_float_ext(dir, pos);

			memcpy((void *)&pNESystem->pivot[r], (void *)pos, sizeof(BOD));

			pNESystem->pivot[r].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pNESystem->fRadius;
			pNESystem->pivot[r].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pNESystem->fRadius;
			pNESystem->pivot[r].y = pNESystem->fLowPlain;

			par_vloz_hnizdo_pust_castici(pNESystem->hHnizdo[r]);
		}

		return;
	}

	if(pNESystem->dwTime > pNESystem->dwRegTime)
	{
		pNESystem->dwTime = 0;
			
		for(j=0; j<pNESystem->SizeofHnizda; j++)
		{
			if(!(rand()%pNESystem->iGenProbability))
				par_vloz_hnizdo_pust_castici(pNESystem->hHnizdo[j]);

			pNESystem->dir[j].x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			pNESystem->dir[j].y = (randf()/(float)(RAND_MAX)) * 0.5f;
			pNESystem->dir[j].z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
		}
	}
}

void am_Do_NESMusku(NATUREEFFECTSYSTEM *pNESystem)
{
	PAR_KOUR_STOPA *pCastice;
	int j;

	if(!Level.bGameResume)
		pNESystem->dwTime += ber.TimeLastFrame;

	if(pNESystem->dwTime > pNESystem->dwRegTime)
	{
		pNESystem->dwTime = 0;
			
		for(j=0; j<pNESystem->SizeofHnizda; j++)
		{
			if(!(rand()%2))
			{
				pNESystem->dir[j].x = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				pNESystem->dir[j].y = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.15f;
				pNESystem->dir[j].z = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				norm_vect(&pNESystem->dir[j].x, &pNESystem->dir[j].y, &pNESystem->dir[j].z);

				memcpy((void *)&pNESystem->nx[j], (void *)&pNESystem->dir[j], sizeof(BOD));

				pCastice = par_cti_hnizdo_castice(pNESystem->hHnizdo[j]);

				if(pCastice)
					memcpy((void *)&pCastice->nx, (void *)&pNESystem->dir[j], sizeof(BOD));

				pNESystem->dir[j].x *= 4;
				pNESystem->dir[j].z *= 4;
			}
		}
	}
}

void am_Do_NESList(NATUREEFFECTSYSTEM *pNESystem, LEVELINFO *p_Level)
{
	PAR_KOUR_STOPA *pCastice;
	float dir[6], pos[3];
	int j;

	if(!p_Level->bGameResume)
		pNESystem->dwTime += ber.TimeLastFrame;

	if(!p_Level->bGameResume)
		pNESystem->dwGenTimeCounter += ber.TimeLastFrame;

	if(pNESystem->dwGenTimeCounter > pNESystem->dwGenTime)
	{
		pNESystem->dwGenTimeCounter = 0;

		if(!(rand()%pNESystem->iGenProbability))
		{
			int r = rand()%pNESystem->SizeofHnizda;

			kam_get_float_ext(dir, pos);

			memcpy((void *)&pNESystem->pivot[r], (void *)pos, sizeof(BOD));

			pNESystem->pivot[r].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pNESystem->fRadius;
			pNESystem->pivot[r].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pNESystem->fRadius;
			//pNESystem->pivot[r].y = pNESystem->fSky;
			//pNESystem->pivot.y = pos[1] + 1.0f;
			pNESystem->pivot[r].y = pos[1] + pNESystem->fSky;

			par_vloz_hnizdo_pust_castici(pNESystem->hHnizdo[r]);
		}

		return;
	}

	if(pNESystem->dwTime > pNESystem->dwRegTime)
	{
		pNESystem->dwTime = 0;
			
		for(j=0; j<pNESystem->SizeofHnizda; j++)
		{
			if(!(rand()%pNESystem->iGenProbability))
				par_vloz_hnizdo_pust_castici(pNESystem->hHnizdo[j]);

			pNESystem->dir[j].x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.5f;
			pNESystem->dir[j].y = -3;
			pNESystem->dir[j].z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.5f;

			pNESystem->dir[j].x += pNESystem->AddDir.x;
			pNESystem->dir[j].y += pNESystem->AddDir.y;
			pNESystem->dir[j].z += pNESystem->AddDir.z;

			pNESystem->dir[j].x += p_Level->fVitr[0];
			pNESystem->dir[j].y += p_Level->fVitr[1];
			pNESystem->dir[j].z += p_Level->fVitr[2];

			memcpy((void *)&pNESystem->nx[j], (void *)&pNESystem->dir[j], sizeof(BOD));
			norm_vect(&pNESystem->nx[j].x, &pNESystem->nx[j].y, &pNESystem->nx[j].z);

			pCastice = par_cti_hnizdo_castice(pNESystem->hHnizdo[j]);

			while(pCastice)
			{
				memcpy((void *)&pCastice->nx, (void *)&pNESystem->nx[j], sizeof(BOD));
				pCastice = pCastice->p_next;
			}
		}
	}
}

void am_Do_Nature_Effects(LEVELINFO *p_Level)
{
	NATUREEFFECTSYSTEM *pNESystem;
	int i;

	for(i=0;i<10;i++)
		if(p_Level->NatureESystem[i].pSystem != -1)
		{
			pNESystem = &p_Level->NatureESystem[i];
			
			switch(pNESystem->EffectID)
			{
				case 1:
				case 2:
				case 3:
					am_Do_NESChmiri(pNESystem);
					break;
				case 4:
				case 5:
					am_Do_NESMusku(pNESystem);
					break;
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
					am_Do_NESList(pNESystem, p_Level);
					break;
				case 12:
					am_Do_NESSvetluska(pNESystem);
					break;
			}
		}
}

int am_Create_Water_Circles(LEVELINFO *p_Level, RAINSYSTEM *pRain, float fRadius, 
							unsigned int uiDensity, unsigned int uiIntensity, 
							float fmaxDivergence)
{
	PARMETAC_HNIZDO *pWHnizdo;
	ITEMDESC *pItem;
	float	pos[3] = {0,0,0};
	PAR_KOUR_STOPA	*pCastice;
	int cWaters, i, material, LastItem = 0, rot;
	int intensity, delay;

	pRain->hWHnizdo = NULL;
	pRain->Wpivot = NULL;
	pRain->Wpos = NULL;

	cWaters = am_Counet_Waters(p_Level);

	if(!cWaters)
	{
		pRain->bWaterCircles = 0;
		return 1;
	}

	pRain->SizeofWHnizda = cWaters;

	// pamet na hnizda
	pRain->hWHnizdo = (int *)malloc(cWaters * sizeof(int));

	if(!pRain->hWHnizdo)
	{
		pRain->bWaterCircles = 0;
		return 0;
	}

	// pamet na pivoty
	pRain->Wpivot = (BOD *)malloc(cWaters * sizeof(BOD));

	if(!pRain->Wpivot)
	{
		free((void *) pRain->hWHnizdo);
		pRain->bWaterCircles = 0;
		return 0;
	}

	// pamet na pozice vody
	pRain->Wpos = (BOD *)malloc(cWaters * sizeof(BOD));

	if(!pRain->Wpos)
	{
		free((void *) pRain->hWHnizdo);
		free((void *) pRain->Wpivot);
		pRain->bWaterCircles = 0;
		return 0;
	}

	// vypocet potrebne velikosti pameti na castice
	pRain->SizeofWCastice = cWaters * 10;

	// pamet na castice
	pRain->pWCastice = (PAR_KOUR_STOPA *)malloc(pRain->SizeofWCastice * sizeof(PAR_KOUR_STOPA));

	if(!pRain->pWCastice)
	{
		free((void *) pRain->hWHnizdo);
		free((void *) pRain->Wpivot);
		free((void *) pRain->Wpos);
		pRain->bWaterCircles = 0;
		return 0;
	}	

	material = kom_najdi_material("ckolo1");

	if(material == -1)
		kprintf(1,"Nelze najit material ckolo1");

	pCastice = &pRain->pWCastice[0];

	pCastice->rychlost_x = 0.01f;
	pCastice->rychlost_y = 0.01f;

	pCastice->utlum_x = pCastice->utlum_y = 3.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 0.8f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = -3.0f;
	pCastice->ka = 0.0f;

	pRain->Wnx.x = 1;
	pRain->Wnx.y = 0;
	pRain->Wnx.z = 0;
	pRain->Wny.x = 0;
	pRain->Wny.y = 0;
	pRain->Wny.z = 1;

	pRain->pWSystem = par_vyrob();

	if(pRain->pWSystem == -1)
	{
		free((void *) pRain->hWHnizdo);
		free((void *) pRain->Wpivot);
		free((void *) pRain->Wpos);
		free((void *) pRain->pWCastice);
		pRain->bWaterCircles = 0;
		return 0;
	}

	par_set_param(pRain->pWSystem, material, TPAR_NO_FOG | TPAR_3D | TPAR_SCALE_ADD | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);

	pCastice = pRain->pWCastice;

	par_vloz_kour_stopu(pRain->pWSystem, pCastice, pRain->SizeofWCastice);

	for(i=0;i<pRain->SizeofWHnizda;i++)
	{
		pRain->hWHnizdo[i] = par_vloz_hnizdo(pRain->pWSystem);

		if(pRain->hWHnizdo[i] != -1)
		{
			pItem = am_Find_Next_Water(&LastItem, p_Level);

			if(pItem)
			{
				kom_mesh_get_float(pItem->Index_Of_Game_Mesh, &pRain->Wpos[i].x,
								   &pRain->Wpos[i].y, &pRain->Wpos[i].z, &rot);

				memcpy((void *) &pRain->Wpivot[i], (void *) &pRain->Wpos[i], sizeof(BOD));

				pRain->Wpivot[i].x += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				pRain->Wpivot[i].z += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				pRain->Wpivot[i].y -= 0.3f;

				intensity = (int)ceil((uiIntensity / 2.0f) + ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * ((uiIntensity * fmaxDivergence) / 2.0f));
			
				delay = rand()%uiIntensity;

				par_vloz_hnizdo_timer(pRain->hWHnizdo[i], intensity, delay);

				par_vloz_hnizdo_komplet(pRain->hWHnizdo[i], intensity, &pRain->Wpivot[i], pCastice);
				
				pWHnizdo = par_cti_hnizdo(pRain->hWHnizdo[i]);

				if(pWHnizdo)
				{
					pWHnizdo->p_nx = &pRain->Wnx;
					pWHnizdo->p_ny = &pRain->Wny;
				}
			}
		}
	}

	par_go(pRain->pWSystem, &pRain->flag, 0, 0);

	pRain->bWaterCircles = 1;

	return 1;
}

int am_Create_Rain(LEVELINFO *p_Level, RAINSYSTEM *pRain, float fRadius, 
				   unsigned int uiDensity, unsigned int uiIntensity, float fmaxDivergence, 
				   float	fSky)
{
	PARMETAC_HNIZDO *pHnizdo;

	float	fvPos[3];
	PAR_KOUR_STOPA	*pCastice;
	int i, material, intensity, delay;

	
	kom_get_fyz_souradnice((int)floor(p_Level->Size[0] / 2.0f), 0, (int)floor(p_Level->Size[2] / 2.0f), (BOD *)fvPos);
	
	fvPos[1] = ber.y_start;

	pRain->fDist = fRadius;				// radius deste
	pRain->uiDensity = uiDensity;		// hustota deste (pocet hnizd)
	pRain->uiIntensity = uiIntensity;	// intenzita deste (casovy interval knizd)
	pRain->fSky = fSky;					// vyska oblohy (y souradnice, odkud kapky padaji)
	pRain->fLowPlain = ber.y_start;		// dno levelu

	memcpy((void *)&pRain->RainCenter, (void *)fvPos, 3 * sizeof(float));

	// pamet na hnizda
	pRain->hHnizdo = (int *)malloc(uiDensity * sizeof(int));

	if(!pRain->hHnizdo)
	{
		pRain->bRain = 0;
		return 0;
	}
	else
		pRain->SizeofHnizda = uiDensity;

	// pamet na pivoty
	pRain->pivot = (BOD *)malloc(uiDensity * sizeof(BOD));

	if(!pRain->pivot)
	{
		free((void *) pRain->hHnizdo);
		pRain->bRain = 0;
		return 0;
	}

	// vypocet potrebne velikosti pameti na castice
	pRain->SizeofCastice = (int)ceil((uiDensity * 2) + (fRadius * fRadius));

	// pamet na castice
	pRain->pCastice = (PAR_KOUR_STOPA *)malloc(pRain->SizeofCastice * sizeof(PAR_KOUR_STOPA));

	if(!pRain->pCastice)
	{
		free((void *) pRain->hHnizdo);
		free((void *) pRain->pivot);
		pRain->bRain = 0;
		return 0;
	}

	// prvotni nagenerovani deste
	material = kom_najdi_material("ckapka3");

	if(material == -1)
		kprintf(1,"Nelze najit material ckapka3");

	pCastice = &pRain->pCastice[0];
	
	pCastice->rychlost_x = 0.3f;
	pCastice->rychlost_y = 0.02f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 1.0f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pRain->dir.x = (rand()&0x1 ? randf() : -randf())/(float)(2 * RAND_MAX);
	pRain->dir.y = -27.0f;
	pRain->dir.z = (rand()&0x1 ? randf() : -randf())/(float)(2 * RAND_MAX);

	memcpy((void *)&pRain->nx, (void *)&pRain->dir, sizeof(BOD));
	norm_vect(&pRain->nx.x, &pRain->nx.y, &pRain->nx.z);

	pRain->pSystem = par_vyrob();

	if(pRain->pSystem == -1)
	{
		free((void *) pRain->hHnizdo);
		free((void *) pRain->pivot);
		free((void *) pRain->pCastice);
		pRain->bRain = 0;
		return 0;
	}

	par_set_param(pRain->pSystem, material, TPAR_3D | TPAR_HALF_LIFE | TPAR_YPLANE_LOW | 
				  TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR | TPAR_NO_FOG, 
				  (BOD *)fvPos, NULL);	

	pCastice = pRain->pCastice;

	par_vloz_kour_stopu(pRain->pSystem, pCastice, pRain->SizeofCastice);
	
	for(i=0;i<pRain->SizeofHnizda;i++)
	{
		pRain->hHnizdo[i] = par_vloz_hnizdo(pRain->pSystem);

		if(pRain->hHnizdo[i] != -1)
		{
			intensity = (int)ceil(uiIntensity + ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * (uiIntensity * fmaxDivergence));
			
			delay = rand()%uiIntensity;

			par_vloz_hnizdo_timer(pRain->hHnizdo[i], intensity, delay);

			par_vloz_hnizdo_komplet(pRain->hHnizdo[i], intensity, &pRain->pivot[i], pCastice);

			memcpy((void *)&pRain->pivot[i], (void *)fvPos, 3 * sizeof(float));

			pRain->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pRain->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pRain->pivot[i].y = ber.y_start + fSky;

			par_vloz_hnizdo_dir(pRain->hHnizdo[i], &pRain->dir);
			par_vloz_hnizdo_y_plane(pRain->hHnizdo[i], pRain->fLowPlain);

			pHnizdo = par_cti_hnizdo(pRain->hHnizdo[i]);

			if(pHnizdo)
			{
				pHnizdo->p_nx = &pRain->nx;
				pHnizdo->p_ny = &pRain->ny;
			}
		}
	}

	par_go(pRain->pSystem, &pRain->flag, 0, 0);

	pRain->bRain = 1;

	am_Create_Water_Circles(p_Level, pRain, fRadius, uiDensity, uiIntensity, fmaxDivergence);

	return 1;
}

int am_Create_Snow(LEVELINFO *p_Level, SNOWSYSTEM *pSnow, float fRadius, 
				   unsigned int uiDensity, unsigned int uiIntensity, float fmaxDivergence, 
				   float fSky)
{
	float	fvPos[3];
	PAR_KOUR_STOPA	*pCastice;
	int i, material, intensity, delay;

	
	kom_get_fyz_souradnice((int)floor(p_Level->Size[0] / 2.0f), 0, (int)floor(p_Level->Size[2] / 2.0f), (BOD *)fvPos);
	
	fvPos[1] = ber.y_start;

	pSnow->fDist = fRadius;				// radius snehu
	pSnow->uiDensity = uiDensity;		// hustota snehu (pocet hnizd)
	pSnow->uiIntensity = uiIntensity;	// intenzita snehu (casovy interval knizd)
	pSnow->fSky = fSky;					// vyska oblohy (y souradnice, odkud kapky padaji)
	pSnow->fLowPlain = ber.y_start;		// dno levelu

	memcpy((void *)&pSnow->SnowCenter, (void *)fvPos, 3 * sizeof(float));

	// pamet na hnizda
	pSnow->hHnizdo = (int *)malloc(uiDensity * sizeof(int));

	if(!pSnow->hHnizdo)
	{
		pSnow->bSnow = 0;
		return 0;
	}
	else
		pSnow->SizeofHnizda = uiDensity;

	// pamet na pivoty
	pSnow->pivot = (BOD *)malloc(uiDensity * sizeof(BOD));

	if(!pSnow->pivot)
	{
		free((void *) pSnow->hHnizdo);
		pSnow->bSnow = 0;
		return 0;
	}

	// vypocet potrebne velikosti pameti na castice
	pSnow->SizeofCastice = (int)ceil((uiDensity * 20) + (fRadius * fRadius));

	// pamet na castice
	pSnow->pCastice = (PAR_KOUR_STOPA *)malloc(pSnow->SizeofCastice * sizeof(PAR_KOUR_STOPA));

	if(!pSnow->pCastice)
	{
		free((void *) pSnow->hHnizdo);
		free((void *) pSnow->pivot);
		pSnow->bSnow = 0;
		return 0;
	}

	// prvotni nagenerovani deste
	material = kom_najdi_material("cvlocka1");

	if(material == -1)
		kprintf(1,"Nelze najit material cvlocka1");

	pCastice = &pSnow->pCastice[0];
	
	pCastice->rychlost_y = 0.02f;
	pCastice->rychlost_y = 0.02f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 1.0f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pSnow->dir.x = (rand()&0x1 ? randf() : -randf())/(float)(6 * RAND_MAX);
	pSnow->dir.y = -0.25f;
	pSnow->dir.z = (rand()&0x1 ? randf() : -randf())/(float)(6 * RAND_MAX);

	pSnow->pSystem = par_vyrob();

	if(pSnow->pSystem == -1)
	{
		free((void *) pSnow->hHnizdo);
		free((void *) pSnow->pivot);
		free((void *) pSnow->pCastice);
		pSnow->bSnow = 0;
		return 0;
	}

	par_set_param(pSnow->pSystem, material, TPAR_YPLANE_LOW | TPAR_VETSI | TPAR_AUTOREMOVE | 
				  TPAR_DIR | TPAR_NO_FOG, (BOD *)fvPos, NULL);	

	pCastice = pSnow->pCastice;

	par_vloz_kour_stopu(pSnow->pSystem, pCastice, pSnow->SizeofCastice);
	
	for(i=0;i<pSnow->SizeofHnizda;i++)
	{
		pSnow->hHnizdo[i] = par_vloz_hnizdo(pSnow->pSystem);

		if(pSnow->hHnizdo[i] != -1)
		{
			intensity = (int)ceil(uiIntensity + ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * (uiIntensity * fmaxDivergence));
			
			delay = rand()%(uiIntensity * 1000);

			par_vloz_hnizdo_timer(pSnow->hHnizdo[i], intensity, delay);

			par_vloz_hnizdo_komplet(pSnow->hHnizdo[i], intensity, &pSnow->pivot[i], pCastice);

			memcpy((void *)&pSnow->pivot[i], (void *)fvPos, 3 * sizeof(float));

			pSnow->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pSnow->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pSnow->pivot[i].y = fSky;



			par_vloz_hnizdo_dir(pSnow->hHnizdo[i], &pSnow->dir);
			par_vloz_hnizdo_y_plane(pSnow->hHnizdo[i], pSnow->fLowPlain);

			//pregenerovani nekterych parametru
			pCastice[0].rychlost_y = pCastice[0].rychlost_x = randf()/(float)(50 * RAND_MAX);

			if(pCastice[0].rychlost_y < 0.005f)
				pCastice[0].rychlost_y = pCastice[0].rychlost_x = 0.005f;
		}
	}

	par_go(pSnow->pSystem, &pSnow->flag, 0, 0);

	pSnow->bSnow = 1;

	return 1;
}

void am_Error_NESRelease(NATUREEFFECTSYSTEM *pNESystem)
{
	if(pNESystem->hHnizdo)
		free((void *) pNESystem->hHnizdo);

	if(pNESystem->pCastice)
		free((void *) pNESystem->pCastice);
	
	if(pNESystem->pivot)
		free((void *) pNESystem->pivot);
	
	if(pNESystem->dir)
		free((void *) pNESystem->dir);

	if(pNESystem->nx)
		free((void *) pNESystem->nx);

	if(pNESystem->ny)
		free((void *) pNESystem->ny);

	pNESystem->hHnizdo = NULL;
	pNESystem->pCastice = NULL;
	pNESystem->pivot = NULL;
	pNESystem->dir = NULL;
	pNESystem->nx = NULL;
	pNESystem->ny = NULL;
}

int am_Create_Chmiri_Pampelisek(LEVELINFO *p_Level, NATUREEFFECTSYSTEM *pNESystem, 
					            float fRadius, unsigned int uiDensity, 
					            unsigned int uiIntensity, float fSky, float fLowPlane,
								int iID)
{
	float	fvPos[3];
	PAR_KOUR_STOPA	*pCastice;
	int material, i;

	kom_get_fyz_souradnice((int)floor(p_Level->Size[0] / 2.0f), 0, (int)floor(p_Level->Size[2] / 2.0f), (BOD *)fvPos);
	
	fvPos[1] = ber.y_start;

	pNESystem->EffectID = iID;
	pNESystem->fRadius = fRadius;
	pNESystem->uiDensity = uiDensity;
	pNESystem->uiIntensity = uiIntensity;
	pNESystem->fSky = fSky;
	pNESystem->fLowPlain = ber.y_start + fLowPlane;
	
	pNESystem->SizeofCastice = uiDensity;
	pNESystem->SizeofHnizda = uiDensity;
	pNESystem->SizeofDir = uiDensity;
	pNESystem->SizeofPivot = uiDensity;

	pNESystem->hHnizdo = NULL;
	pNESystem->pCastice = NULL;
	pNESystem->pivot = NULL;
	pNESystem->dir = NULL;
	pNESystem->nx = NULL;
	pNESystem->ny = NULL;

	// pamet na castice
	pNESystem->pCastice = (PAR_KOUR_STOPA *)malloc(pNESystem->SizeofCastice * sizeof(PAR_KOUR_STOPA));

	if(!pNESystem->pCastice)
		return 0;

	pNESystem->hHnizdo = (int *)malloc(pNESystem->SizeofHnizda * sizeof(int));

	if(!pNESystem->hHnizdo)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->pivot = (BOD *)malloc(pNESystem->SizeofPivot * sizeof(BOD));

	if(!pNESystem->pivot)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->dir = (BOD *)malloc(pNESystem->SizeofDir * sizeof(BOD));

	if(!pNESystem->dir)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}


	material = kom_najdi_material("cchmiri1");

	if(material == -1)
		kprintf(1,"Nelze najit material cchmiri1");

	pCastice = &pNESystem->pCastice[0];
	
	pCastice->rychlost_x = 0.16f;
	pCastice->rychlost_y = 0.08f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 1.0f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pNESystem->pSystem = par_vyrob();

	if(pNESystem->pSystem == -1)
	{
		free((void *) pNESystem->pCastice);
		return 0;
	}

	par_set_param(pNESystem->pSystem, material, TPAR_YPLANE_LOW | TPAR_VETSI | 
				  TPAR_AUTOREMOVE | TPAR_DIR | TPAR_VITR | TPAR_NO_FOG, (BOD *)fvPos, NULL);	

	pCastice = pNESystem->pCastice;

	par_vloz_kour_stopu(pNESystem->pSystem, pCastice, pNESystem->SizeofCastice);

	for(i=0; i<pNESystem->SizeofHnizda; i++)
	{
		pNESystem->hHnizdo[i] = par_vloz_hnizdo(pNESystem->pSystem);

		if(pNESystem->hHnizdo[i] != -1)
		{
			par_vloz_hnizdo_komplet(pNESystem->hHnizdo[i], uiIntensity, &pNESystem->pivot[i], pCastice);
			par_vloz_hnizdo_timer(pNESystem->hHnizdo[i], uiIntensity, -1 * uiIntensity);

			memcpy((void *)&pNESystem->pivot[i], (void *)fvPos, 3 * sizeof(float));

			pNESystem->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].y = fSky;

			pNESystem->dir[i].x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			//pNESystem->dir[i].y = -0.15f;
			pNESystem->dir[i].y = (-randf()/(float)(RAND_MAX)) * 0.25f;
			pNESystem->dir[i].z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;

			par_vloz_hnizdo_dir(pNESystem->hHnizdo[i], &pNESystem->dir[i]);
			par_vloz_hnizdo_y_plane(pNESystem->hHnizdo[i], pNESystem->fLowPlain);
			par_vloz_hnizdo_vitr(pNESystem->hHnizdo[i], (BOD *)p_Level->fVitr);
		}
	}

	par_go(pNESystem->pSystem, &pNESystem->flag, 0, 0);

	pNESystem->dwTime = 0;
	pNESystem->dwRegTime = uiIntensity;

	return 1;
}

int am_Create_Chmiri(LEVELINFO *p_Level, NATUREEFFECTSYSTEM *pNESystem, 
					 float fRadius, unsigned int uiDensity, 
					 unsigned int uiIntensity, float fSky, float fLowPlane,
					 char *cMaterial, int iID)
{
	float	fvPos[3];
	PAR_KOUR_STOPA	*pCastice;
	int material, i;

	kom_get_fyz_souradnice((int)floor(p_Level->Size[0] / 2.0f), 0, (int)floor(p_Level->Size[2] / 2.0f), (BOD *)fvPos);
	
	fvPos[1] = ber.y_start;

	pNESystem->EffectID = iID;
	pNESystem->fRadius = fRadius;
	pNESystem->uiDensity = uiDensity;
	pNESystem->uiIntensity = uiIntensity;
	pNESystem->fSky = fSky;
	pNESystem->fLowPlain = ber.y_start + fLowPlane;
	
	pNESystem->SizeofCastice = uiDensity;
	pNESystem->SizeofHnizda = uiDensity;
	pNESystem->SizeofDir = uiDensity;
	pNESystem->SizeofPivot = uiDensity;

	pNESystem->hHnizdo = NULL;
	pNESystem->pCastice = NULL;
	pNESystem->pivot = NULL;
	pNESystem->dir = NULL;
	pNESystem->nx = NULL;
	pNESystem->ny = NULL;

	// pamet na castice
	pNESystem->pCastice = (PAR_KOUR_STOPA *)malloc(pNESystem->SizeofCastice * sizeof(PAR_KOUR_STOPA));

	if(!pNESystem->pCastice)
		return 0;

	pNESystem->hHnizdo = (int *)malloc(pNESystem->SizeofHnizda * sizeof(int));

	if(!pNESystem->hHnizdo)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->pivot = (BOD *)malloc(pNESystem->SizeofPivot * sizeof(BOD));

	if(!pNESystem->pivot)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->dir = (BOD *)malloc(pNESystem->SizeofDir * sizeof(BOD));

	if(!pNESystem->dir)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}


	material = kom_najdi_material(cMaterial);

	if(material == -1)
		kprintf(1,"Nelze najit material %s", cMaterial);

	pCastice = &pNESystem->pCastice[0];
	
	pCastice->rychlost_x = 0.06f;
	pCastice->rychlost_y = 0.06f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 1.0f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pNESystem->pSystem = par_vyrob();

	if(pNESystem->pSystem == -1)
	{
		free((void *) pNESystem->pCastice);
		return 0;
	}

	par_set_param(pNESystem->pSystem, material, TPAR_YPLANE_LOW | TPAR_VETSI | 
				  TPAR_AUTOREMOVE | TPAR_DIR | TPAR_VITR | TPAR_NO_FOG, (BOD *)fvPos, NULL);	

	pCastice = pNESystem->pCastice;

	par_vloz_kour_stopu(pNESystem->pSystem, pCastice, pNESystem->SizeofCastice);

	for(i=0; i<pNESystem->SizeofHnizda; i++)
	{
		pNESystem->hHnizdo[i] = par_vloz_hnizdo(pNESystem->pSystem);

		if(pNESystem->hHnizdo[i] != -1)
		{
			par_vloz_hnizdo_komplet(pNESystem->hHnizdo[i], uiIntensity, &pNESystem->pivot[i], pCastice);
			par_vloz_hnizdo_timer(pNESystem->hHnizdo[i], uiIntensity, -1 * uiIntensity);

			memcpy((void *)&pNESystem->pivot[i], (void *)fvPos, 3 * sizeof(float));

			pNESystem->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].y = fSky;

			pNESystem->dir[i].x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			//pNESystem->dir[i].y = -0.15f;
			pNESystem->dir[i].y = (-randf()/(float)(RAND_MAX)) * 0.25f;
			pNESystem->dir[i].z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;

			par_vloz_hnizdo_dir(pNESystem->hHnizdo[i], &pNESystem->dir[i]);
			par_vloz_hnizdo_y_plane(pNESystem->hHnizdo[i], pNESystem->fLowPlain);
			par_vloz_hnizdo_vitr(pNESystem->hHnizdo[i], (BOD *)p_Level->fVitr);
		}
	}

	par_go(pNESystem->pSystem, &pNESystem->flag, 0, 0);

	pNESystem->dwTime = 0;
	pNESystem->dwRegTime = uiIntensity;

	return 1;
}

int am_Create_Svetlusky(LEVELINFO *p_Level, NATUREEFFECTSYSTEM *pNESystem, 
					    float fRadius, unsigned int uiDensity, 
					    unsigned int uiIntensity, float fSky, float fLowPlane,
					    char *cMaterial, int iID)
{
	float	fvPos[3],r;
	PAR_KOUR_STOPA	*pCastice;
	int material, i;

	kom_get_fyz_souradnice((int)floor(p_Level->Size[0] / 2.0f), 0, (int)floor(p_Level->Size[2] / 2.0f), (BOD *)fvPos);
	
	fvPos[1] = ber.y_start;

	pNESystem->EffectID = iID;
	pNESystem->fRadius = fRadius;
	pNESystem->uiDensity = uiDensity;
	pNESystem->uiIntensity = uiIntensity;
	pNESystem->fSky = ber.y_start + fSky;
	pNESystem->fLowPlain = ber.y_start;
	
	pNESystem->SizeofCastice = uiDensity;
	pNESystem->SizeofHnizda = uiDensity;
	pNESystem->SizeofDir = uiDensity;
	pNESystem->SizeofPivot = uiDensity;

	pNESystem->hHnizdo = NULL;
	pNESystem->pCastice = NULL;
	pNESystem->pivot = NULL;
	pNESystem->dir = NULL;
	pNESystem->nx = NULL;
	pNESystem->ny = NULL;

	// pamet na castice
	pNESystem->pCastice = (PAR_KOUR_STOPA *)malloc(pNESystem->SizeofCastice * sizeof(PAR_KOUR_STOPA));

	if(!pNESystem->pCastice)
		return 0;

	pNESystem->hHnizdo = (int *)malloc(pNESystem->SizeofHnizda * sizeof(int));

	if(!pNESystem->hHnizdo)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->pivot = (BOD *)malloc(pNESystem->SizeofPivot * sizeof(BOD));

	if(!pNESystem->pivot)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->dir = (BOD *)malloc(pNESystem->SizeofDir * sizeof(BOD));

	if(!pNESystem->dir)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}


	material = kom_najdi_material(cMaterial);

	if(material == -1)
		kprintf(1,"Nelze najit material %s", cMaterial);

	pCastice = &pNESystem->pCastice[0];
	
	pCastice->rychlost_x = 0.06f;
	pCastice->rychlost_y = 0.06f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	pCastice->r = 0.99f;
	pCastice->g = 0.96f;
	pCastice->b = 0.66f;
	pCastice->a = 1.0f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pNESystem->pSystem = par_vyrob();

	if(pNESystem->pSystem == -1)
	{
		free((void *) pNESystem->pCastice);
		return 0;
	}

	par_set_param(pNESystem->pSystem, material, TPAR_YPLANE_TOP | TPAR_VETSI | 
				  TPAR_AUTOREMOVE | TPAR_DIR | TPAR_VITR | TPAR_NO_FOG, (BOD *)fvPos, NULL);	

	pCastice = pNESystem->pCastice;

	par_vloz_kour_stopu(pNESystem->pSystem, pCastice, pNESystem->SizeofCastice);

	for(i=0; i<pNESystem->SizeofHnizda; i++)
	{
		r = (randf()/(float)(RAND_MAX)) * 0.1f;
		pCastice->rychlost_x = 0.05f + r;
		pCastice->rychlost_y = 0.05f + r;

		pNESystem->hHnizdo[i] = par_vloz_hnizdo(pNESystem->pSystem);

		if(pNESystem->hHnizdo[i] != -1)
		{
			par_vloz_hnizdo_komplet(pNESystem->hHnizdo[i], uiIntensity, &pNESystem->pivot[i], pCastice);
			par_vloz_hnizdo_timer(pNESystem->hHnizdo[i], uiIntensity, -1 * uiIntensity);

			memcpy((void *)&pNESystem->pivot[i], (void *)fvPos, 3 * sizeof(float));

			pNESystem->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].y = ber.y_start;

			pNESystem->dir[i].x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			//pNESystem->dir[i].y = -0.15f;
			pNESystem->dir[i].y = (randf()/(float)(RAND_MAX)) * 0.5f;
			pNESystem->dir[i].z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;

			par_vloz_hnizdo_dir(pNESystem->hHnizdo[i], &pNESystem->dir[i]);
			par_vloz_hnizdo_y_plane(pNESystem->hHnizdo[i], pNESystem->fSky);
			par_vloz_hnizdo_vitr(pNESystem->hHnizdo[i], &pNESystem->dir[i]);
		}
	}

	par_go(pNESystem->pSystem, &pNESystem->flag, 0, 0);

	pNESystem->dwTime = 0;
	pNESystem->dwGenTimeCounter = 0;
	pNESystem->dwRegTime = 500;
	pNESystem->dwGenTime = rand()%(uiIntensity * 10);
	pNESystem->iGenProbability = uiIntensity;


	return 1;
}

int am_Create_Musku(LEVELINFO *p_Level, NATUREEFFECTSYSTEM *pNESystem, 
					 float fRadius, unsigned int uiDensity, 
					 unsigned int uiIntensity, float fSky, float fLowPlane,
					 char *cMaterial, int iID)
{
	PARMETAC_HNIZDO *pHnizdo;
	float	fvPos[3];
	PAR_KOUR_STOPA	*pCastice;
	int material, i;

	kom_get_fyz_souradnice((int)floor(p_Level->Size[0] / 2.0f), 0, (int)floor(p_Level->Size[2] / 2.0f), (BOD *)fvPos);
	
	fvPos[1] = ber.y_start;

	pNESystem->EffectID = iID;
	pNESystem->fRadius = fRadius;
	pNESystem->uiDensity = uiDensity;
	pNESystem->uiIntensity = uiIntensity;
	pNESystem->fSky = ber.y_start + fSky;
	pNESystem->fLowPlain = ber.y_start + fLowPlane;
	
	pNESystem->SizeofCastice = uiDensity;
	pNESystem->SizeofHnizda = uiDensity;
	pNESystem->SizeofDir = uiDensity;
	pNESystem->SizeofPivot = uiDensity;
	pNESystem->SizeofNX = uiDensity;
	pNESystem->SizeofNY = uiDensity;

	pNESystem->hHnizdo = NULL;
	pNESystem->pCastice = NULL;
	pNESystem->pivot = NULL;
	pNESystem->dir = NULL;
	pNESystem->nx = NULL;
	pNESystem->ny = NULL;

	// pamet na castice
	pNESystem->pCastice = (PAR_KOUR_STOPA *)malloc(pNESystem->SizeofCastice * sizeof(PAR_KOUR_STOPA));

	if(!pNESystem->pCastice)
		return 0;

	pNESystem->hHnizdo = (int *)malloc(pNESystem->SizeofHnizda * sizeof(int));

	if(!pNESystem->hHnizdo)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->pivot = (BOD *)malloc(pNESystem->SizeofPivot * sizeof(BOD));

	if(!pNESystem->pivot)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->dir = (BOD *)malloc(pNESystem->SizeofDir * sizeof(BOD));

	if(!pNESystem->dir)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->nx = (BOD *)malloc(pNESystem->SizeofNX * sizeof(BOD));

	if(!pNESystem->nx)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->ny = (BOD *)malloc(pNESystem->SizeofNY * sizeof(BOD));

	if(!pNESystem->ny)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	material = kom_najdi_material(cMaterial);

	if(material == -1)
		kprintf(1,"Nelze najit material %s", cMaterial);

	pCastice = &pNESystem->pCastice[0];
	
	pCastice->rychlost_x = 0.06f;
	pCastice->rychlost_y = 0.06f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 1.0f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pNESystem->pSystem = par_vyrob();

	if(pNESystem->pSystem == -1)
	{
		free((void *) pNESystem->pCastice);
		return 0;
	}

	par_set_param(pNESystem->pSystem, material, TPAR_3D | TPAR_HALF_LIFE | TPAR_NO_FOG |
				  TPAR_YPLANE_LOW | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_VITR, 
				  (BOD *)fvPos, NULL);	

	pCastice = pNESystem->pCastice;

	par_vloz_kour_stopu(pNESystem->pSystem, pCastice, pNESystem->SizeofCastice);

	for(i=0; i<pNESystem->SizeofHnizda; i++)
	{
		pNESystem->hHnizdo[i] = par_vloz_hnizdo(pNESystem->pSystem);

		if(pNESystem->hHnizdo[i] != -1)
		{
			par_vloz_hnizdo_komplet(pNESystem->hHnizdo[i], uiIntensity, &pNESystem->pivot[i], pCastice);
			par_vloz_hnizdo_timer(pNESystem->hHnizdo[i], uiIntensity, -1 * uiIntensity);

			memcpy((void *)&pNESystem->pivot[i], (void *)fvPos, 3 * sizeof(float));

			pNESystem->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].y = fSky;

			pNESystem->dir[i].x = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
			pNESystem->dir[i].y = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.15f;
			pNESystem->dir[i].z = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
			norm_vect(&pNESystem->dir[i].x, &pNESystem->dir[i].y, &pNESystem->dir[i].z);

			par_vloz_hnizdo_vitr(pNESystem->hHnizdo[i], &pNESystem->dir[i]);
			par_vloz_hnizdo_y_plane(pNESystem->hHnizdo[i], pNESystem->fLowPlain);

			memcpy((void *)&pNESystem->nx[i], (void *)&pNESystem->dir[i], sizeof(BOD));
			norm_vect(&pNESystem->nx[i].x, &pNESystem->nx[i].y, &pNESystem->nx[i].z);
		
			pHnizdo = par_cti_hnizdo(pNESystem->hHnizdo[i]);

			if(pHnizdo)
			{
				pHnizdo->p_nx = &pNESystem->nx[i];
				pHnizdo->p_ny = &pNESystem->ny[i];
			}	
		}
	}

	par_go(pNESystem->pSystem, &pNESystem->flag, 0, 0);

	pNESystem->dwTime = 0;
	pNESystem->dwRegTime = 250;

	return 1;
}

int am_Create_Padajici_Listi(LEVELINFO *p_Level, NATUREEFFECTSYSTEM *pNESystem, 
							 float fRadius, unsigned int uiDensity, 
							 unsigned int uiIntensity, float fSky, float fLowPlane,
							 char *cMaterial, int iID)
{
	PARMETAC_HNIZDO *pHnizdo;
	float	fvPos[3];
	PAR_KOUR_STOPA	*pCastice;
	int material, i;

	kom_get_fyz_souradnice((int)floor(p_Level->Size[0] / 2.0f), 0, (int)floor(p_Level->Size[2] / 2.0f), (BOD *)fvPos);
	
	fvPos[1] = ber.y_start;

	pNESystem->EffectID = iID;
	pNESystem->fRadius = fRadius;
	pNESystem->uiDensity = uiDensity;
	pNESystem->uiIntensity = uiIntensity;
	pNESystem->fSky = fSky * 2;
	pNESystem->fLowPlain = ber.y_start + fLowPlane;
	
	pNESystem->SizeofCastice = (uiDensity * 10);
	pNESystem->SizeofHnizda = uiDensity;
	pNESystem->SizeofDir = uiDensity;
	pNESystem->SizeofPivot = uiDensity;
	pNESystem->SizeofNX = uiDensity;
	pNESystem->SizeofNY = uiDensity;

	pNESystem->hHnizdo = NULL;
	pNESystem->pCastice = NULL;
	pNESystem->pivot = NULL;
	pNESystem->dir = NULL;
	pNESystem->nx = NULL;
	pNESystem->ny = NULL;

	// pamet na castice
	pNESystem->pCastice = (PAR_KOUR_STOPA *)malloc(pNESystem->SizeofCastice * sizeof(PAR_KOUR_STOPA));

	if(!pNESystem->pCastice)
		return 0;

	pNESystem->hHnizdo = (int *)malloc(pNESystem->SizeofHnizda * sizeof(int));

	if(!pNESystem->hHnizdo)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->pivot = (BOD *)malloc(pNESystem->SizeofPivot * sizeof(BOD));

	if(!pNESystem->pivot)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->dir = (BOD *)malloc(pNESystem->SizeofDir * sizeof(BOD));

	if(!pNESystem->dir)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}
	
	pNESystem->nx = (BOD *)malloc(pNESystem->SizeofNX * sizeof(BOD));

	if(!pNESystem->nx)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}

	pNESystem->ny = (BOD *)malloc(pNESystem->SizeofNY * sizeof(BOD));

	if(!pNESystem->ny)
	{
		am_Error_NESRelease(pNESystem);
		return 0;
	}
	material = kom_najdi_material(cMaterial);

	if(material == -1)
		kprintf(1,"Nelze najit material %s", cMaterial);

	pCastice = &pNESystem->pCastice[0];
	
	pCastice->rychlost_x = 0.2f;
	pCastice->rychlost_y = 0.2f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 1.0f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pNESystem->pSystem = par_vyrob();

	if(pNESystem->pSystem == -1)
	{
		free((void *) pNESystem->pCastice);
		return 0;
	}

	par_set_param(pNESystem->pSystem, material, TPAR_3D | TPAR_HALF_LIFE | TPAR_YPLANE_LOW
				  | TPAR_VETSI | TPAR_VITR | TPAR_NO_FOG, (BOD *)fvPos, NULL);	

	pCastice = pNESystem->pCastice;

	par_vloz_kour_stopu(pNESystem->pSystem, pCastice, pNESystem->SizeofCastice);

	for(i=0; i<pNESystem->SizeofHnizda; i++)
	{
		pNESystem->hHnizdo[i] = par_vloz_hnizdo(pNESystem->pSystem);

		if(pNESystem->hHnizdo[i] != -1)
		{
			par_vloz_hnizdo_komplet(pNESystem->hHnizdo[i], 3600000, &pNESystem->pivot[i], pCastice);
			par_vloz_hnizdo_timer(pNESystem->hHnizdo[i], 3600000, 0);

			memcpy((void *)&pNESystem->pivot[i], (void *)fvPos, 3 * sizeof(float));

			pNESystem->pivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
			pNESystem->pivot[i].y = fSky;

			pNESystem->dir[i].x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.5f;
			pNESystem->dir[i].y = -1;
			pNESystem->dir[i].z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.5f;
			norm_vect(&pNESystem->dir[i].x, &pNESystem->dir[i].y, &pNESystem->dir[i].z);

			par_vloz_hnizdo_vitr(pNESystem->hHnizdo[i], &pNESystem->dir[i]);
			par_vloz_hnizdo_y_plane(pNESystem->hHnizdo[i], pNESystem->fLowPlain);

			pCastice->rychlost_x = pCastice->rychlost_y = (randf()/(float)(RAND_MAX));

			if(pCastice->rychlost_x < 0.05f)
				pCastice->rychlost_x = pCastice->rychlost_y = 0.05f;

			if(pCastice->rychlost_x > 0.3f)
				pCastice->rychlost_x = pCastice->rychlost_y = 0.3f;

			par_vloz_hnizdo_scale(pNESystem->hHnizdo[i], pCastice->rychlost_x, 
								  pCastice->rychlost_y, 0, 0);

			memcpy((void *)&pNESystem->nx[i], (void *)&pNESystem->dir[i], sizeof(BOD));
			norm_vect(&pNESystem->nx[i].x, &pNESystem->nx[i].y, &pNESystem->nx[i].z);
		
			pHnizdo = par_cti_hnizdo(pNESystem->hHnizdo[i]);

			if(pHnizdo)
			{
				pHnizdo->p_nx = &pNESystem->nx[i];
				pHnizdo->p_ny = &pNESystem->ny[i];
			}	
		}
	}

	pNESystem->AddDir.x = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 3;
	pNESystem->AddDir.y = (randf()/(float)(RAND_MAX)) * 2;
	pNESystem->AddDir.z = ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 3;
	//norm_vect(&pNESystem->AddDir.x, &pNESystem->AddDir.y, &pNESystem->AddDir.z);

	par_go(pNESystem->pSystem, &pNESystem->flag, 0, 0);

	pNESystem->dwTime = 0;
	pNESystem->dwGenTimeCounter = 0;
	pNESystem->dwRegTime = 500;
	pNESystem->dwGenTime = rand()%(uiIntensity * 10);
	pNESystem->iGenProbability = uiIntensity;

	return 1;
}

int am_Get_Free_Nature_Effect_System(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->NatureESystem[i].pSystem == -1)
			return i;

	return -1;
}


int am_Create_Nature_Effect(LEVELINFO *p_Level, int iEffectID, float fRadius, 
							unsigned int uiDensity, unsigned int uiIntensity, 
							float fSky, float fLowPlane)
{
	int k = am_Get_Free_Nature_Effect_System(p_Level);
	
	if(k == -1)
		return 0;

	switch(iEffectID)
	{
	case 1:
		return am_Create_Chmiri_Pampelisek(p_Level, &p_Level->NatureESystem[k], fRadius, 
										   uiDensity, uiIntensity, fSky, fLowPlane, 1);
	case 2:
		return am_Create_Chmiri(p_Level, &p_Level->NatureESystem[k], fRadius, 
							    uiDensity, uiIntensity, fSky, fLowPlane,
								"cchmiri2", 2);
	case 3:
		return am_Create_Chmiri(p_Level, &p_Level->NatureESystem[k], fRadius, 
							    uiDensity, uiIntensity, fSky, fLowPlane,
								"cchmiri3", 3);
	case 4:
		return am_Create_Musku(p_Level, &p_Level->NatureESystem[k], fRadius, 
							    uiDensity, uiIntensity, fSky, fLowPlane,
								"cmuska1", 4);
	case 5:
		return am_Create_Musku(p_Level, &p_Level->NatureESystem[k], fRadius, 
							    uiDensity, uiIntensity, fSky, fLowPlane,
								"cmuska2", 5);
	case 6:
		return am_Create_Padajici_Listi(p_Level, &p_Level->NatureESystem[k], fRadius, 
										uiDensity, uiIntensity, fSky, fLowPlane,
										"clist1", 6);
	case 7:
		return am_Create_Padajici_Listi(p_Level, &p_Level->NatureESystem[k], fRadius, 
										uiDensity, uiIntensity, fSky, fLowPlane,
										"clist2", 7);
	case 8:
		return am_Create_Padajici_Listi(p_Level, &p_Level->NatureESystem[k], fRadius, 
										uiDensity, uiIntensity, fSky, fLowPlane,
										"clist3", 8);
	case 9:
		return am_Create_Padajici_Listi(p_Level, &p_Level->NatureESystem[k], fRadius, 
										uiDensity, uiIntensity, fSky, fLowPlane,
										"clist4", 9);
	case 10:
		return am_Create_Padajici_Listi(p_Level, &p_Level->NatureESystem[k], fRadius, 
										uiDensity, uiIntensity, fSky, fLowPlane,
										"clist5", 10);
	case 11:
		return am_Create_Padajici_Listi(p_Level, &p_Level->NatureESystem[k], fRadius, 
										uiDensity, uiIntensity, fSky, fLowPlane,
										"clist6", 11);
	case 12:
		return am_Create_Svetlusky(p_Level, &p_Level->NatureESystem[k], fRadius, 
										uiDensity, uiIntensity, fSky, fLowPlane,
										"csvetluska1", 12);
	default:
		return 0;
	}
}

void am_Create_Water_Effect(LEVELINFO *p_Level)
{
	long guid;
	BOD p={0,0,0};
	WATERDESC *pWater;
	int i, c = 0;

	pWater = &p_Level->Water;

	for(i=0;i<p_Level->Size_of_Level;i++)
		if(p_Level->Level[i])
			if(p_Level->Level[i]->p_Object->Class == 12)
			{
				c++;
				guid = p_Level->Level[i]->p_Object->GUID;
			}

	if(!c)
	{
		pWater->bWater = 0;
		return;
	}

	pWater->hWater = vod_vyrob(c);

	for(i=0;i<p_Level->Size_of_Level;i++)
		if(p_Level->Level[i])
			if(p_Level->Level[i]->p_Object->Class == 12)
				vod_pridej_mesh(pWater->hWater, p_Level->Level[i]->Index_Of_Game_Mesh);

	vod_uzavri_meshe(pWater->hWater);

	if(guid == 12000)
		pWater->hVodnik[0] = vod_vloz_vodnika(pWater->hWater,&p, VOD_AKTIVNI | VOD_Z_PLANE |
							 VOD_DIFF_ADD | VOD_SPEC_ADD, 0.04f, 100, 1, 0, 0, 200);
	else
		pWater->hVodnik[0] = vod_vloz_vodnika(pWater->hWater,&p, VOD_AKTIVNI | VOD_Z_PLANE |
							 VOD_DIFF_ADD | VOD_SPEC_ADD, 0.01f, 100, 1, 0, 0, 200);
	
	if(guid == 12000)
	{
		vod_vloz_vodnika_spec(pWater->hVodnik[0], 0.1f, 0.1f, 0.1f);
		vod_vloz_vodnika_add_spec(pWater->hVodnik[0], 0, 0, 0);
	
		vod_vloz_vodnika_diff(pWater->hVodnik[0], 0.05f, 0.05f, 0.05f, 0.125f);
		vod_vloz_vodnika_add_diff(pWater->hVodnik[0], 0, 0, 0, 0);
	}
	else
	{
		vod_vloz_vodnika_spec(pWater->hVodnik[0], 0.05f, 0.05f, 0.05f);
		vod_vloz_vodnika_add_spec(pWater->hVodnik[0], 0, 0, 0);
	
		vod_vloz_vodnika_diff(pWater->hVodnik[0], 0.025f, 0.025f, 0.025f, 0.05f);
		vod_vloz_vodnika_add_diff(pWater->hVodnik[0], 0, 0, 0, 0);
	}

	if(guid == 12000)
	{
		pWater->hVodnik[1] = vod_vloz_vodnika(pWater->hWater, &p, VOD_AKTIVNI | VOD_X_PLANE, 
											  0.04f, 100, 1, 0, 0, 200);

		pWater->hVodnik[2] = vod_vloz_vodnika(pWater->hWater, &p, VOD_AKTIVNI | VOD_BOD,
											  0.04f, 200, 1, 0, 0, 300);
	}
	else
	{
		pWater->hVodnik[1] = vod_vloz_vodnika(pWater->hWater, &p, VOD_AKTIVNI | VOD_X_PLANE, 
											  0.01f, 100, 1, 0, 0, 200);

		pWater->hVodnik[2] = vod_vloz_vodnika(pWater->hWater, &p, VOD_AKTIVNI | VOD_BOD,
											  0.01f, 200, 1, 0, 0, 300);
	}

	pWater->bWater = 1;
}

void am_Create_Prizemni_Mlhu(LEVELINFO *p_Level, float fDensity, float fHeight)
{
	float pos[3];
	int hSvetlo;

	hSvetlo = edl_svetlo_vyrob(EDL_PLOSNE_Y |  EDL_SPEC_ADD | EDL_SCENA |
							   EDL_DOSAH | EDL_UTLUM_LIN, 0);

	if(hSvetlo != -1)
	{
		pos[0] = 0;
		pos[1] = ber.y_start;
		pos[2] = 0;

		edl_svetlo_set_pos(hSvetlo, (BOD *)pos, (BOD *)pos);
		edl_svetlo_set_spec(hSvetlo, 0.5f, 0.5f, 0.5f);
		edl_svetlo_set_par(hSvetlo, 1, fHeight, 0, 0);
		p_Level->Mist.hSvetlo = hSvetlo;
	}
	else
		p_Level->Mist.bMist = 0;
}

void am_Start_Animaci_Pontonky(ITEMDESC *pItem)
{
	int guid = pItem->p_Object->GUID == 5007;
	BOD b = {(float)(rand()%2),0,(float)(rand()%2)};
	int anim, i, f1, f2;
	float r;

	if(pItem->a_run != -1)
		return;

	if(!b.x && !b.z)
	{
		b.x = 1.0f;
		b.z = 1.0f;
	}

	anim = sim_vyrob_animaci(4,4,0);
	sim_vloz_pivot(anim,0.0f, 1.0f, 0.0f);
	
	sim_vloz_klic_posun(anim,0,0.0f,0.0f,0.0f,0);

	i = rand()%2;

	if(guid == 5007)
		r = rand()/(float)(RAND_MAX * 5);
	else
		r = rand()/(float)(RAND_MAX * 40);

	if(r < 0.05f)
		r = 0.05f;

	if(guid == 5007)
		f1 = (int)ceil(r * 200);
	else
		f1 = (int)ceil(r * 400);

	if(i) r *= -1;

	sim_vloz_klic_posun(anim,1,0.0f,r,0.0f,f1);
	
	if(guid == 5007)
		r = rand()/(float)(RAND_MAX * 5);
	else
		r = rand()/(float)(RAND_MAX * 40);

	if(r < 0.05f)
		r = 0.05f;

	if(guid == 5007)
		f2 = (int)ceil((r * 200) + f1);
	else
		f2 = (int)ceil((r * 400) + f1);

	if(!i) r *= -1;

	sim_vloz_klic_posun(anim,2,0.0f,r,0.0f,f1 + f2);
	sim_vloz_klic_posun(anim,3,0.0f,0.0f,0.0f,2*f1 + f2);

	sim_vloz_klic_rotace(anim,0,&b,0,0);

	i = rand()%2;

	if(guid == 5007)
		r = (rand()/(float)(RAND_MAX)) * 10;
	else
		r = (rand()/(float)(RAND_MAX)) * 2;

	if(i) r *= -1;

	sim_vloz_klic_rotace(anim,1,&b,r,f1);
	
	if(guid == 5007)
		r = (rand()/(float)(RAND_MAX)) * 10;
	else
		r = (rand()/(float)(RAND_MAX)) * 2;

	if(!i) r *= -1;
	
	sim_vloz_klic_rotace(anim,2,&b,r,f1 + f2);
	sim_vloz_klic_rotace(anim,3,&b,0,2*f1 + f2);

	sim_interpoluj_animaci(anim,2*f1 + f2 + 1,1);
						
	pItem->a_run = rani_aktivuj(anim, &pItem->a_flag,GK_LOOP,0,0);
						
	rani_privaz_mesh(pItem->a_run, pItem->Index_Of_Game_Mesh, 0);
}

void am_Start_Animace_Pontonek(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<p_Level->Count_Of_Items;i++)
		if(p_Level->Item[i].p_Object)
		{
		if(p_Level->Item[i].p_Object->Class == 5 &&
		   p_Level->Item[i].p_Object->SubClass == 3)
		   if(gl_Check_Anim_Pontonky(&p_Level->Item[i], p_Level))
				am_Start_Animaci_Pontonky(&p_Level->Item[i]);
		}
		else
		kprintf(1,"p_Level->Item[%d].p_Object == NULL", i); 
}

int am_Get_Free_BarelSparksSystem(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<30;i++)
		if(p_Level->BarelSparks[i].System == -1)
			return i;

	return -1;
}

int am_Create_BarelSparksSystem(ITEMDESC *pTel, LEVELINFO *p_Level)
{
	int m,k;
	PAR_KOUR_STOPA	*pKourovaS;
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	float pos[3];
	int rot;
	int size = (int)(10 / p_Level->KvalitaCastic);

	k = am_Get_Free_BarelSparksSystem(p_Level);

	if( k == -1)
		return -1;

	pSystem = &p_Level->BarelSparks[k];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return -1;
	
	kom_mesh_get_float(pTel->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;
	pSystem->dwStart = timeGetTime();

	//m = kom_najdi_material("flare12");
	m = kom_najdi_material("flare100");
	
	if(m == -1)
		kprintf(1,"Nelze najit material flare12");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 1.0f;
	pKourovaS[0].utlum_x = -2;
	pKourovaS[0].utlum_y = -2;

	pKourovaS[0].r = 0;
	pKourovaS[0].g = 1;
	pKourovaS[0].b = 0.5f;

	pKourovaS[0].a = 1.0f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();

	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_DIR | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);
	
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 10, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 10, -10);

	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][1]++;

	pSystem->dir[0][0] = 0;
	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 1;
	pSystem->dir[0][2] = 0;				

	par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], pos[1] + 3);

	par_vloz_hnizdo_dir(pSystem->hHnizdo[0], (BOD *)pSystem->dir[0]);

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwStop = 0;
	pSystem->hHnizdo[1] = (int)pTel;

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);

	return k;
}

void am_Start_Barels(LEVELINFO *p_Level)
{
	float pos[3];
	int hSvetlo, rot;
	int rnd = rand()%3;

	int i, k;

	for(i=0;i<p_Level->Count_Of_Items;i++)
		if(p_Level->Item[i].p_Object)
		{
		if(p_Level->Item[i].p_Object->Class == 6 &&
		   p_Level->Item[i].p_Object->GUID == 6014)
    {
		hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);
		
		kom_mesh_get_float(p_Level->Item[i].Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

	    if(hSvetlo != -1)
	    {
		    pos[1] += 1.25f;

		    sdl_svetlo_set_pos(hSvetlo, (BOD *)pos);
		    sdl_svetlo_set_diff(hSvetlo, 0, 1, 0.5f, 1, 4, 4);

			k = am_Create_BarelSparksSystem(&p_Level->Item[i], p_Level);

			if(k > -1)
				p_Level->BarelSparks[k].hHnizdo[2] = (int)hSvetlo;
			else
				sdl_svetlo_zrus(hSvetlo);
	    }
    }
		}
		else
			kprintf(1,"p_Level->Item[%d].p_Object == NULL", i);
}

int gl_Get_Free_Kour_Pusa(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->KourUst[i].System == -1)
			return i;

	return -1;
}

void am_Do_Mouth_Smoke(int iItem, float *pos, LEVELINFO *p_Level)
{
	int k,m,i;
	PAR_KOUR_STOPA	*pKourovaS;
	float f;

	k = gl_Get_Free_Kour_Pusa(p_Level);

	if(k == -1)
		return;

	pKourovaS = (PAR_KOUR_STOPA *) malloc(20 * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	p_Level->KourUst[k].pCastice = pKourovaS;
	p_Level->KourUst[k].Sizeof = 1;
	p_Level->KourUst[k].dwStart = timeGetTime();

	m = kom_najdi_material("mrak1_1");

	if(m == -1)
		kprintf(1,"Nelze najit material mrak1_1");

	pKourovaS[0].rychlost_x = 0.02f;
	pKourovaS[0].rychlost_y = 0.02f;
	pKourovaS[0].utlum_x = 2.7f;
	pKourovaS[0].utlum_y = 2.7f;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 0.95f;
	pKourovaS[0].a = 0.3f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = -0.2f;
	pKourovaS[0].ka = 0.0f;

	p_Level->KourUst[k].System = par_vyrob();

	par_set_param(p_Level->KourUst[k].System, m, TPAR_NO_FOG|TPAR_SCALE|TPAR_VETSI|TPAR_AUTOREMOVE|TPAR_DIR|TPAR_VITR, (BOD *)pos, NULL);
	par_vloz_kour_stopu(p_Level->KourUst[k].System, pKourovaS, 20);

	for(i=0;i<p_Level->KourUst[k].Sizeof;i++)
	{
		p_Level->KourUst[k].hHnizdo[i] = par_vloz_hnizdo(p_Level->KourUst[k].System);

		par_vloz_hnizdo_komplet(p_Level->KourUst[k].hHnizdo[i], 100, (BOD *)p_Level->KourUst[k].pivot[i], pKourovaS);
		par_vloz_hnizdo_timer(p_Level->KourUst[k].hHnizdo[i], 100, -100);
	
		//vygeneruj dir
		p_Level->KourUst[k].dir[i][0] = (rand()&0x1 ? randf() : -randf());
		p_Level->KourUst[k].dir[i][1] = (randf());
		p_Level->KourUst[k].dir[i][2] = (rand()&0x1 ? randf() : -randf());
		vektor_norm((BOD *)p_Level->KourUst[k].dir[i]);

		f = (randf()/(float)(RAND_MAX))* _3DKOREKCE;
	
		p_Level->KourUst[k].dir[i][0] *= f / 100.0f;
		p_Level->KourUst[k].dir[i][1] *= f / 100.0f;
		p_Level->KourUst[k].dir[i][2] *= f / 100.0f;

		memcpy((void *)p_Level->KourUst[k].pivot[i], (void *)pos, 3*sizeof(float));

		par_vloz_hnizdo_dir(p_Level->KourUst[k].hHnizdo[i], (BOD *)p_Level->KourUst[k].dir[i]);

		par_vloz_hnizdo_vitr(p_Level->KourUst[k].hHnizdo[i], (BOD *)p_Level->fVitr);
	}

	p_Level->KourUst[k].dwStart = timeGetTime();
	p_Level->KourUst[k].hHnizdo[1] = iItem;

	par_pripoj_funkci(p_Level->KourUst[k].System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(p_Level->KourUst[k].System, &p_Level->KourUst[k].flag, 0, 0);
}

void am_Do_BeatleSmokes(LEVELINFO *p_Level)
{
	float pos[3], f;
	int	  rot, i;
	long  real;
	BEATLE_SMOKE_SYSTEM *pBeatle;

	if(!p_Level->Snow.bSnow)
			return;

	for(i=0;i<10;i++)
		if(p_Level->KourUst[i].System != -1)
	{
		//vygeneruj dir
		p_Level->KourUst[i].dir[0][0] = (rand()&0x1 ? randf() : -randf());
		p_Level->KourUst[i].dir[0][1] = (randf());
		p_Level->KourUst[i].dir[0][2] = (rand()&0x1 ? randf() : -randf());
		vektor_norm((BOD *)p_Level->KourUst[i].dir[0]);

		f = (randf()/(float)(RAND_MAX))* _3DKOREKCE;
	
		p_Level->KourUst[i].dir[0][0] *= f / 100.0f;
		p_Level->KourUst[i].dir[0][1] *= f / 100.0f;
		p_Level->KourUst[i].dir[0][2] *= f / 100.0f;

		kom_mesh_get_float(p_Level->Item[p_Level->KourUst[i].hHnizdo[1]].Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);
				
		pos[1] += 0.70f;

		switch(p_Level->Item[p_Level->KourUst[i].hHnizdo[1]].Rotation)
		{
			case 0:
				pos[2]+=0.5f;						
				break;
			case 1:
				pos[0]+=0.5f;
				break;
			case 2:
				pos[2]-=0.5f;
				break;
			case 3:
				pos[0]-=0.5f;
				break;
		}

		memcpy((void *)p_Level->KourUst[i].pivot[0], (void *)pos, 3*sizeof(float));
	}

	for(i=0;i<6;i++)
	{
		pBeatle = &p_Level->BeatleSmoke[i];

		if(pBeatle->iItem != -1)
		{		
			if(!p_Level->bGameResume)
				pBeatle->dwRealTime += ber.TimeLastFrame;

			if(pBeatle->dwRealTime > pBeatle->dwExpTime)
			{
				kom_mesh_get_float(p_Level->Item[pBeatle->iItem].Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);
					
				pos[1] += 0.70f;
				
				pBeatle->dwRealTime = 0;

				gl_Logical2Real(p_Level->Item[pBeatle->iItem].Pos[0], p_Level->Item[pBeatle->iItem].Pos[1],
								p_Level->Item[pBeatle->iItem].Pos[2], &real, p_Level);
			
				if(!p_Level->Square[real].bUnderWater)
					am_Do_Mouth_Smoke(pBeatle->iItem, pos, p_Level);
			}
		}
	}
}

void am_Obsluha_Koure_Brouku(LEVELINFO *p_Level)
{
	DWORD dwTime, dwEplased;
	int j,i;

	if(!p_Level->Snow.bSnow)
		return;

	for(i=0;i<10;i++)
	if(p_Level->KourUst[i].System != -1)
	{
		dwTime = timeGetTime();
		dwEplased = dwTime - p_Level->KourUst[i].dwStart;

		if(dwEplased > 1500)
			for(j=0;j<p_Level->KourUst[i].Sizeof;j++)
				if(p_Level->KourUst[i].hHnizdo[j])
				{
					par_vloz_hnizdo_pivot(p_Level->KourUst[i].hHnizdo[j], NULL);
					p_Level->KourUst[i].hHnizdo[j] = 0;
				}

		if(!par_get_hnizda(p_Level->KourUst[i].System))
		{
			par_zrus(p_Level->KourUst[i].System);
			p_Level->KourUst[i].System = -1;				
		}
	}
}

void am_Create_Steps_System(LEVELINFO *p_Level)
{
	int i,m;
	PAR_KOUR_STOPA	*pKourovaS;
	PARMETAC_HNIZDO *pHnizdo;
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	int size = 100, rot;
	float pos[3];


	for(i=0;i<6;i++)
	if(p_Level->BeatleSmoke[i].iItem != -1)
	{
		pSystem = &p_Level->BeatleSmoke[i].sStopy;

		kom_mesh_get_float(p_Level->Item[p_Level->BeatleSmoke[i].iItem].Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

		pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

		if(!pKourovaS)
			return;
	
		pSystem->pCastice = pKourovaS;
		pSystem->Sizeof = size;
		pSystem->dwStart = timeGetTime();

		m = kom_najdi_material("csteps0");
	
		if(m == -1)
		{
			kprintf(1,"Nelze najit material csteps0");
			free((void *) pKourovaS);
			return;
		}

		pKourovaS[0].rychlost_x = 0.5f;
		pKourovaS[0].rychlost_y = 0.5f;
		pKourovaS[0].utlum_x = 0;
		pKourovaS[0].utlum_y = 0;

		pKourovaS[0].r = 0.14f;
		pKourovaS[0].g = 0.47f;
		pKourovaS[0].b = 0.65f;

		pKourovaS[0].a = 0.75f;
		pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = 0;
		pKourovaS[0].da = -0.02f;
		pKourovaS[0].ka = 0;

		pSystem->System = par_vyrob();

		par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_3D |  
					  TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);

		par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

		pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

		par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 1000000, (BOD *)pSystem->pivot[0], pKourovaS);
		par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 1000000, 0);
	
		memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

		pSystem->pivot[0][1] -= 0.95f;
		
		if(pSystem->hHnizdo[0])
			pHnizdo = par_cti_hnizdo(pSystem->hHnizdo[0]);

		pSystem->nx[0][0] = 1;
		pSystem->nx[0][1] = 0;
		pSystem->nx[0][2] = 0;
		pSystem->ny[0][0] = 0;
		pSystem->ny[0][1] = 0;
		pSystem->ny[0][2] = 1;

		pHnizdo->p_nx = (BOD *) pSystem->nx[0];
		pHnizdo->p_ny = (BOD *) pSystem->ny[0];

		pSystem->dwStart = timeGetTime();
		pSystem->dwTime = 0;
		pSystem->dwExpire = 0;
		pSystem->dwExpireTime = 0;
		pSystem->dwStop = 0;

//		pSystem->hHnizdo[1] = 0;

		par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

		par_go(pSystem->System, &pSystem->flag, 0, 0);
	}

	return;
}

void am_Change_NXNZ(LEVELINFO *p_Level, ITEMDESC *pItem, SYSTEMKOUROVYCHCASTIC *pSystem)
{
	int iPos[3];
	long real;

	memcpy((void *)iPos, pItem->Pos, 3*sizeof(int));

	switch(pItem->Rotation)
	{
	case 0:
		iPos[1]--;
		break;
	case 1:
		iPos[0]--;
		break;
	case 2:
		iPos[1]++;
		break;
	case 3:
		iPos[0]++;
		break;
	}

	gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);

	if(p_Level->Level[real])
	{
		if(p_Level->Level[real]->p_Object->Class != 19)
			iPos[2]-=2;
	}
	else
		iPos[2]-=2;


	gl_Logical2Real(iPos[0], iPos[1], iPos[2], &real, p_Level);


	if(p_Level->Level[real])
	{
		if(p_Level->Level[real]->p_Object->Class != 19)
			return;
	}
	else
		return;

	switch(p_Level->Level[real]->Rotation)
	{
	case 3:
		pSystem->nx[0][0] = 1;
		pSystem->nx[0][1] = 1;
		pSystem->nx[0][2] = 0;
		pSystem->ny[0][0] = 0;
		pSystem->ny[0][1] = 0;
		pSystem->ny[0][2] = 1;
		break;
	case 1:
		pSystem->nx[0][0] = -1;
		pSystem->nx[0][1] = 1;
		pSystem->nx[0][2] = 0;
		pSystem->ny[0][0] = 0;
		pSystem->ny[0][1] = 0;
		pSystem->ny[0][2] = -1;
		break;
	case 2:
		pSystem->nx[0][0] = 1;
		pSystem->nx[0][1] = 0;
		pSystem->nx[0][2] = 0;
		pSystem->ny[0][0] = 0;
		pSystem->ny[0][1] = 1;
		pSystem->ny[0][2] = 1;
		break;
	case 0:
		pSystem->nx[0][0] = -1;
		pSystem->nx[0][1] = 0;
		pSystem->nx[0][2] = 0;
		pSystem->ny[0][0] = 0;
		pSystem->ny[0][1] = 1;
		pSystem->ny[0][2] = -1;
		break;
	}
}
void am_Set_Step(int iFoot, LEVELINFO *p_Level)
{
	int i, rot;
	float pos[3];
	SYSTEMKOUROVYCHCASTIC	*pSystem;
	PARMETAC_HNIZDO *pHnizdo;
	
	if(p_Level->Actual_Item == -1)
		return;

	for(i=0;i<6;i++)
		if(p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh == 
		   p_Level->Item[p_Level->BeatleSmoke[i].iItem].Index_Of_Game_Mesh && p_Level->BeatleSmoke[i].iItem != -1)
		{
			if(p_Level->BeatleSmoke[i].iStepc)
			{
				kom_mesh_get_float(p_Level->Item[p_Level->BeatleSmoke[i].iItem].Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

				pSystem = &p_Level->BeatleSmoke[i].sStopy;

				memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));

				if(!iFoot)
				{
					pSystem->pivot[0][1] -= 0.95f;
					
					pSystem->nx[0][0] = 1;
					pSystem->nx[0][1] = 0;
					pSystem->nx[0][2] = 0;
					pSystem->ny[0][0] = 0;
					pSystem->ny[0][1] = 0;
					pSystem->ny[0][2] = 1;
				}
				else
				{
					/*am_Change_NXNZ(p_Level, p_Level->Level[p_Level->Actual_Item], pSystem);
					
					if(!(p_Level->BeatleSmoke[i].iStepc%2))
						pSystem->pivot[0][1] -= 0.7f;
					else
						pSystem->pivot[0][1] += 0.2f;*/
				}

				pHnizdo = par_cti_hnizdo(pSystem->hHnizdo[0]);

				if(pHnizdo)
				{
					pHnizdo->rotace = p_Level->Level[p_Level->Actual_Item]->Rotation;

					switch(pHnizdo->rotace)
					{
					case 0:
						if(!(p_Level->BeatleSmoke[i].iStepc%2))
							pSystem->pivot[0][0] += 0.4f;
						else
							pSystem->pivot[0][0] -= 0.4f;
						break;
					case 2:
						if(!(p_Level->BeatleSmoke[i].iStepc%2))
							pSystem->pivot[0][0] -= 0.4f;
						else
							pSystem->pivot[0][0] += 0.4f;
						break;
					case 1:
						if(!(p_Level->BeatleSmoke[i].iStepc%2))
							pSystem->pivot[0][2] -= 0.4f;
						else
							pSystem->pivot[0][2] += 0.4f;
						break;
					case 3:
						if(!(p_Level->BeatleSmoke[i].iStepc%2))
							pSystem->pivot[0][2] += 0.4f;
						else
							pSystem->pivot[0][2] -= 0.4f;
						break;
					}
				}

				par_vloz_hnizdo_pust_castici(pSystem->hHnizdo[0]);
			}

			p_Level->BeatleSmoke[i].iStepc++;

			return;
		}
}


void am_Zero_Step(LEVELINFO *p_Level)
{
	int i;

	if(p_Level->Actual_Item == -1)
		return;

	p_Level->bSetStep = 0;

	for(i=0;i<6;i++)
		if(p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh == 
		   p_Level->Item[p_Level->BeatleSmoke[i].iItem].Index_Of_Game_Mesh && p_Level->BeatleSmoke[i].iItem != -1)
		{
			p_Level->BeatleSmoke[i].iStepc=0;

			return;
		}
}

void am_Release_Steps(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->BeatleSmoke[i].iItem != -1 && p_Level->BeatleSmoke[i].sStopy.System != -1)
		{
			kprintf(1, "p_Level->BeatleSmoke[%d],  par_zrus %d" ,i, p_Level->BeatleSmoke[i].sStopy.System);

			par_zrus(p_Level->BeatleSmoke[i].sStopy.System);
			p_Level->BeatleSmoke[i].sStopy.System = -1;
		}
}

void am_Do_Step(int param, int param2, void *p_param)
{
	LEVELINFO *p_Level = (LEVELINFO *) p_param;

	if(!p_Level->bSetStep)
		return;

	am_Set_Step(param, p_Level);

//	p_Level->bSetStep = 0;
}

void am_Destroy_Steps(ITEMDESC *pItem, LEVELINFO *p_Level)
{
	float pos[3];
	int	  rot, i;
	PAR_KOUR_STOPA  *pks;

	if(!pItem)
		return;

	kom_mesh_get_float(pItem->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

	pos[1] += 1.05f;

	for(i=0;i<6;i++)
		if(p_Level->BeatleSmoke[i].iItem != -1 && p_Level->BeatleSmoke[i].sStopy.System != -1)
		{
			pks = par_cti_hnizdo_castice(p_Level->BeatleSmoke[i].sStopy.hHnizdo[0]);

			if(pks)
			{
				while(pks)
				{
					if(pks->p.x >= pos[0] - 1 &&  pks->p.x <= pos[0] + 1 &&
					   pks->p.z >= pos[2] - 1 &&  pks->p.z <= pos[2] + 1 &&
					   pks->p.y >= pos[1] - 0.5f && pks->p.y <= pos[1] + 0.5f)
						pks->a = 0;

					pks = pks->p_next;
				}
			}
		}
	
}

void am_Create_BublSystem_Effect1(float *pos, LEVELINFO *p_Level, int iSize)
{
	int m;
	PAR_KOUR_STOPA	*pKourovaS;
	SYSTEMKOUROVYCHCASTIC	*pSystem = &p_Level->BublSystemE.System;
	int size = (int)(iSize / p_Level->KvalitaCastic);

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;

	m = kom_najdi_material("cbublina1");
	
	if(m == -1)
		kprintf(1,"Nelze najit material cbublina1");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 7.0f;
	
	pKourovaS[0].utlum_x = 0;
	pKourovaS[0].utlum_y = 0;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 0.8f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();
	
	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR, (BOD *)pos, NULL);
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 100000, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 100000, -100000);
	
	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));
	memcpy((void *)pSystem->pivot[1], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][0] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;
	pSystem->pivot[0][2] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;

	pSystem->dir[0][0] = 0;
	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 5.0f;
	
	if(pSystem->dir[0][1] < 1.0f)
		pSystem->dir[0][1] = 1.0f;

	pSystem->dir[0][2] = 0;

	par_vloz_hnizdo_dir(pSystem->hHnizdo[0], (BOD *)pSystem->dir[0]);
	par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], 50.0f);

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwStop = 2000;
	p_Level->BublSystemE.bOn = 0;

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);
}

void am_Create_BublSystem_Effect2(float *pos, LEVELINFO *p_Level, int iSize, float yPlane)
{
	int m;
	PAR_KOUR_STOPA	*pKourovaS;
	SYSTEMKOUROVYCHCASTIC	*pSystem = &p_Level->BublSystemC.System;
	int size = (int)(iSize / p_Level->KvalitaCastic);

	pKourovaS = (PAR_KOUR_STOPA *) malloc(size * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return;
	
	pSystem->pCastice = pKourovaS;
	pSystem->Sizeof = size;

	m = kom_najdi_material("cbublina1");
	
	if(m == -1)
		kprintf(1,"Nelze najit material cbublina1");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 7.0f;
	
	pKourovaS[0].utlum_x = 0;
	pKourovaS[0].utlum_y = 0;

	pKourovaS[0].r = pKourovaS[0].g = pKourovaS[0].b = 1.0f;

	pKourovaS[0].a = 0.8f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->System = par_vyrob();
	
	par_set_param(pSystem->System, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_VETSI | TPAR_AUTOREMOVE | TPAR_DIR, (BOD *)pos, NULL);
	par_vloz_kour_stopu(pSystem->System, pKourovaS, pSystem->Sizeof);

	pSystem->hHnizdo[0] = par_vloz_hnizdo(pSystem->System);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo[0], 100000, (BOD *)pSystem->pivot[0], pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 100000, -100000);
	
	memcpy((void *)pSystem->pivot[0], (void *)pos, 3*sizeof(float));
	memcpy((void *)pSystem->pivot[1], (void *)pos, 3*sizeof(float));

	pSystem->pivot[0][0] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;
	pSystem->pivot[0][2] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) / 5.0f;

	pSystem->dir[0][0] = 0;
	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 5.0f;
	
	if(pSystem->dir[0][1] < 1.0f)
		pSystem->dir[0][1] = 1.0f;

	pSystem->dir[0][2] = 0;

	par_vloz_hnizdo_dir(pSystem->hHnizdo[0], (BOD *)pSystem->dir[0]);
	par_vloz_hnizdo_y_plane(pSystem->hHnizdo[0], yPlane);

	pSystem->dwStart = timeGetTime();
	pSystem->dwTime = 0;
	pSystem->dwStop = 2000;
	p_Level->BublSystemC.bOn = 0;

	par_pripoj_funkci(pSystem->System,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->System, &pSystem->flag, 0, 0);
}

void am_Do_BublSystemE(LEVELINFO *p_Level)
{
	float f;

	SYSTEMKOUROVYCHCASTIC	*pSystem = &p_Level->BublSystemE.System;

	if(p_Level->BublSystemE.System.System == -1)
		return;

	if(!p_Level->bGameResume)
		pSystem->dwTime += ber.TimeLastFrame;

	if(pSystem->dwTime > pSystem->dwStop)
	{
		pSystem->dwTime = 0;

		if(p_Level->BublSystemE.bOn)
		{
			p_Level->BublSystemE.bOn = 0;
			par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 100000, 0);
			pSystem->dwStop = 3000 + (int)ceil((randf()/(float)(RAND_MAX)) * 1000);
		}
		else
		{
			pSystem->dwStop = 2000;
			p_Level->BublSystemE.bOn = 1;
			par_vloz_hnizdo_timer(pSystem->hHnizdo[0], 1, 0);
//			ap_Play_Sound(0,0,1, pSystem->pivot[1], gl_Choose_Wave_Index(102), NULL, &ad);
		}
	}

	memcpy((void *)pSystem->pivot[0], (void *)pSystem->pivot[1], 3*sizeof(float));

	pSystem->pivot[0][0] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX));
	pSystem->pivot[0][1] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX));
	pSystem->pivot[0][2] += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX));

	pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 10.0f;
	
	if(pSystem->dir[0][1] < 4.0f)
		pSystem->dir[0][1] = 4.0f + (randf()/(float)(RAND_MAX)) * 5.0f;

	f = (randf()/(float)(RAND_MAX)) / 2.0f;
	par_vloz_hnizdo_scale(pSystem->hHnizdo[0], f, f, 0, 0);
}

void am_Do_BublSystemC(LEVELINFO *p_Level)
{
	float f;

	SYSTEMKOUROVYCHCASTIC	*pSystem = &p_Level->BublSystemC.System;

	if(p_Level->BublSystemC.System.System == -1)
		return;

	if(!p_Level->bGameResume)
		pSystem->dwTime += ber.TimeLastFrame;

	if(pSystem->dwTime > pSystem->dwStop)
	{
		pSystem->dwTime = 0;

		p_Level->BublSystemC.bOn = 0;
		par_vloz_hnizdo_pust_castici(pSystem->hHnizdo[0]);
		pSystem->dwStop = 3000 + (int)ceil((randf()/(float)(RAND_MAX)) * 1000);

		pSystem->dir[0][1] = (randf()/(float)(RAND_MAX)) * 10.0f;
	
		if(pSystem->dir[0][1] < 4.0f)
			pSystem->dir[0][1] = 4.0f + (randf()/(float)(RAND_MAX)) * 5.0f;

		f = (randf()/(float)(RAND_MAX)) * 1.5f;
		
		par_vloz_hnizdo_scale(pSystem->hHnizdo[0], f, f, 0, 0);
	}
}

void am_Start_Joint_Animations(void)
{   
  ExMeshHandle    mh, next;
  ChapadloHandle *p_handle = NULL;
  byte           *p_jmeno;
  int             chapadel;
  static int      flag;
  int             i,j,objektu;

  /* -----------------------------------------------------------
     Tuto cast musis delat nekde na zacatku - trva to dlouho
     (nacteni animaci objektu)
  */

  /* Smycka - hleda a vraci postupne vsechny meshe ve scene ktere
     maji joint-animace
  */
  next = 0;
  while((mh = kom_najdi_mesh_joint(&next)) != K_CHYBA) {
    /* U nalezeneho meshe nacte pocet objektu ktere maji joint-animace
    */
    objektu = chani_mesh_cti_objekty(mh);

    /* Projede objekty meshe (pocet objektu s animacema = objektu)
       a spusti jejich prvni animaci
    */
    for(i = 0; i < objektu; i++) {     
    
      /* Nacni animace i-teho objektu v poradi
      */
      chani_mesh_cti_chapadelniky(mh,i,&p_handle,&chapadel);

      /* chapadel = pocet animaci objektu
         p_handle = pole handlu na tyto animace
      */

      /* Vrati jmena animaci objektu
      */
      for(j = 0; j < chapadel; j++) {
        p_jmeno = chani_cti_jmeno(p_handle[j]);
      }

      /* -----------------------------------------------------------
        Tuto cast muzes delat behem hry - je to rychle 
      */

      /* Vyberu si 1. animaci v objektu a tu zaradim do seznamu animaci
         (!!! pozor !!! pri zarazovani animace nekontroluju jestli ta animace 
           tam uz je nebo ne, tak to musis volat jen 1x! nejlepe pred spustenim
           animace)
      */
      j = 0;
      chani_zarad(p_handle[j]);

      /* Zarazenou animaci spustim
      */
      chani_go(p_handle[j],&flag,GK_LOOP,0,0);

      /* Uvolnim pole handlu (je to pole integeru, alokuje se 
         pri volani chani_mesh_cti_chapadelniky pomoci malloc
         a ty to musis uvolnit kdyz to pole nepotrebujes
         To pole si klidne muzes ulozit do nejake sve struktury,
         ja ho k nicemu nepozuivam a je urceny jen pro tebe
      */
      free(p_handle);
    }
  }  


  /* Postup pri ruseni bezici animace:

  1) Zastavim animaci
  chani_stop(p_handle[j]);

  2) Vymazu ji z fronty animaci
  chani_zrus(p_handle[j]);

  */
}

int find_free_street_light(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<8;i++)
		if(!p_Level->StreetL[i].bUsed)
			return i;

	return -1;
}

void am_Create_Street_Light(LEVELINFO *p_Level, int iStart, int iPause, float *pos)
{
	STREET_LIGHT_EFFECT	*psle;

	int idx = find_free_street_light(p_Level);

	if(idx < 0)
		return;

	psle = &p_Level->StreetL[idx];

	psle->bUsed = 1;
	psle->eCounter = 0;
	psle->ePause = iPause;
	psle->epCounter = iPause;
	psle->eStart = iStart;
	psle->hFlare = -1;
	psle->dwEfTime = 0;
	psle->hSvetlo = -1;
	psle->pos[0] = pos[0];
	psle->pos[1] = pos[1];
	psle->pos[2] = pos[2];
	psle->Speed = 10;
	psle->hFlare = -1;
}

void am_Do_Street_Lights(LEVELINFO *p_Level)
{
	STREET_LIGHT_EFFECT	*psle;
	int i, hSvetlo;
	DWORD e = ber.TimeLastFrame;
	float k = ber.TimeLastFrame / 1000.0f;
	LENS_FLARE *pFlare;
	DWORD t = timeGetTime();
	DWORD ep;

	for(i=0;i<8;i++)
		if(p_Level->StreetL[i].bUsed)
		{
			psle = &p_Level->StreetL[i];

			if(psle->eCounter < psle->eStart)
				psle->eCounter+=e;
			else
				if(psle->epCounter < psle->ePause)
				{
					psle->epCounter+=e;
					
					if(psle->hFlare != -1)
					{
						ep = t - psle->dwEfTime;

						if(ep < 250)
						{
							pFlare = kom_flare_get_pointer(psle->hFlare);

							pFlare->dx += psle->Speed * k;
							pFlare->dy += psle->Speed * k;

							if(pFlare->dx > 2.5f)
							{
								pFlare->dx = pFlare->dy = 2.5f;
							}
						}
						else
							if(ep > 1000 && ep < 1250)
							{
								pFlare = kom_flare_get_pointer(psle->hFlare);

								pFlare->dx -= psle->Speed * k;
								pFlare->dy -= psle->Speed * k;

								if(pFlare->dx < 0)
								{
									pFlare->dy = 0;
									pFlare->dx = 0;
								}
							}
							else
								if(ep >= 1250)
								{
									kom_flare_zrus(psle->hFlare);
									psle->hFlare = -1;
								}
					}
				}
				else
				{
					/*if(!i)
						kprintf(1, "Svetlo [%d] na %f, %f, %f", i, psle->pos[0], psle->pos[1], psle->pos[2]);*/

					psle->epCounter = 0;

					hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);

					if(hSvetlo != -1)
					{
						sdl_svetlo_set_pos(hSvetlo, (BOD *)psle->pos);
						sdl_svetlo_set_diff(hSvetlo, 0.996f, 0.617f, 0, 0, 0, 0);

						sdl_anim_vyrob(hSvetlo, 30,0,0,4);
						sdl_anim_vloz_klic_vzdal(hSvetlo,0,0,0,0,0);
						sdl_anim_vloz_klic_vzdal(hSvetlo,1,70,10,70,5);
						sdl_anim_vloz_klic_vzdal(hSvetlo,2,70,10,70,25);
						sdl_anim_vloz_klic_vzdal(hSvetlo,3,0,0,0,29);
						sdl_anim_start(hSvetlo, &p_Level->TrashFlag,GK_REMOVE,0,0);

						psle->hSvetlo = hSvetlo;
					}

					psle->dwEfTime = timeGetTime();

					if(psle->hFlare != K_CHYBA)
						kom_flare_zrus(psle->hFlare);

					psle->hFlare = kom_flare_vyrob(FLR_NO_ZTEST);

					if(psle->hFlare != -1)
					{
						int m = kom_najdi_material("flare4");

						if(m != -1)
						{
							kom_flare_set_param(psle->hFlare, 0.996f, 0.617f, 0, 0.75f, 0.001f, 0.001f, m, GL_ONE);
							kom_flare_set_pivot(psle->hFlare, (BOD *) &psle->pos);
						}
					}
				}
		}
}

void am_Do_Street_Lights_Release(LEVELINFO *p_Level)
{
	int i;
	STREET_LIGHT_EFFECT	*psle;

	for(i=0;i<8;i++)
		if(p_Level->StreetL[i].bUsed)
		{
			kprintf(1, "am_Do_Street_Lights_Release[%d] ...", i);

			psle = &p_Level->StreetL[i];

			if(psle->hFlare != -1)
			{
				kprintf(1, "kom_flare_zrus ...");
				kom_flare_zrus(psle->hFlare);
			}

			if(psle->hSvetlo != -1)
			{
				kprintf(1, "sdl_svetlo_zrus ...");
				sdl_svetlo_zrus(psle->hSvetlo);
			}
		}
}

void am_Do_RainB(RAINSYSTEM *pRain)
{
	int i;

	for(i=0;i<pRain->SizeofWHnizda;i++)
	{
		memcpy((void *) &pRain->Wpivot[i], (void *) &pRain->Wpos[i], sizeof(BOD));

		pRain->Wpivot[i].x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pRain->fDist;
		pRain->Wpivot[i].z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * pRain->fDist;
		//pRain->Wpivot[i].y -= 0.3f;
	}
}

int am_Create_Water_CirclesB(LEVELINFO *p_Level, RAINSYSTEM *pRain, float fRadius, 
							unsigned int uiDensity, unsigned int uiIntensity, 
							float fmaxDivergence, float *rPos)
{
	PARMETAC_HNIZDO *pWHnizdo;
	float	pos[3] = {0,0,0};
	PAR_KOUR_STOPA	*pCastice;
	int cWaters, i, material;
	int intensity, delay;

	pRain->hWHnizdo = NULL;
	pRain->Wpivot = NULL;
	pRain->Wpos = NULL;

	cWaters = 20;

	pRain->SizeofWHnizda = cWaters;

	// pamet na hnizda
	pRain->hWHnizdo = (int *)malloc(cWaters * sizeof(int));

	if(!pRain->hWHnizdo)
	{
		pRain->bWaterCircles = 0;
		return 0;
	}

	// pamet na pivoty
	pRain->Wpivot = (BOD *)malloc(cWaters * sizeof(BOD));

	if(!pRain->Wpivot)
	{
		free((void *) pRain->hWHnizdo);
		pRain->bWaterCircles = 0;
		return 0;
	}

	// pamet na pozice vody
	pRain->Wpos = (BOD *)malloc(cWaters * sizeof(BOD));

	if(!pRain->Wpos)
	{
		free((void *) pRain->hWHnizdo);
		free((void *) pRain->Wpivot);
		pRain->bWaterCircles = 0;
		return 0;
	}

	// vypocet potrebne velikosti pameti na castice
	pRain->SizeofWCastice = cWaters * 10;

	// pamet na castice
	pRain->pWCastice = (PAR_KOUR_STOPA *)malloc(pRain->SizeofWCastice * sizeof(PAR_KOUR_STOPA));

	if(!pRain->pWCastice)
	{
		free((void *) pRain->hWHnizdo);
		free((void *) pRain->Wpivot);
		free((void *) pRain->Wpos);
		pRain->bWaterCircles = 0;
		return 0;
	}	

	material = kom_najdi_material("ckolo1");

	if(material == -1)
		kprintf(1,"Nelze najit material ckolo1");

	pCastice = &pRain->pWCastice[0];

	pCastice->rychlost_x = 0.01f;
	pCastice->rychlost_y = 0.01f;

	pCastice->utlum_x = pCastice->utlum_y = 20.0f;
	pCastice->r = pCastice->g = pCastice->b = 1.0f;
	pCastice->a = 0.8f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = -3.0f;
	pCastice->ka = 0.0f;

	pRain->Wnx.x = 1;
	pRain->Wnx.y = 0;
	pRain->Wnx.z = 0;
	pRain->Wny.x = 0;
	pRain->Wny.y = 0;
	pRain->Wny.z = 1;

	pRain->pWSystem = par_vyrob();

	if(pRain->pWSystem == -1)
	{
		free((void *) pRain->hWHnizdo);
		free((void *) pRain->Wpivot);
		free((void *) pRain->Wpos);
		free((void *) pRain->pWCastice);
		pRain->bWaterCircles = 0;
		return 0;
	}

	par_set_param(pRain->pWSystem, material, TPAR_NO_FOG | TPAR_3D | TPAR_SCALE_ADD | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);

	pCastice = pRain->pWCastice;

	par_vloz_kour_stopu(pRain->pWSystem, pCastice, pRain->SizeofWCastice);

	for(i=0;i<pRain->SizeofWHnizda;i++)
	{
		pRain->hWHnizdo[i] = par_vloz_hnizdo(pRain->pWSystem);

		if(pRain->hWHnizdo[i] != -1)
		{
			//pItem = am_Find_Next_Water(&LastItem, p_Level);

			//if(pItem)
			{
				/*kom_mesh_get_float(pItem->Index_Of_Game_Mesh, &pRain->Wpos[i].x,
								   &pRain->Wpos[i].y, &pRain->Wpos[i].z, &rot);*/

				pRain->Wpos[i].x = rPos[0];
				pRain->Wpos[i].y = rPos[1];
				pRain->Wpos[i].z = rPos[2];

				memcpy((void *) &pRain->Wpivot[i], (void *) &pRain->Wpos[i], sizeof(BOD));

				pRain->Wpivot[i].x += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				pRain->Wpivot[i].z += (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
				pRain->Wpivot[i].y -= 0.3f;

				intensity = (int)ceil((uiIntensity / 2.0f) + ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * ((uiIntensity * fmaxDivergence) / 2.0f));
			
				delay = rand()%uiIntensity;

				par_vloz_hnizdo_timer(pRain->hWHnizdo[i], intensity, delay);

				par_vloz_hnizdo_komplet(pRain->hWHnizdo[i], intensity, &pRain->Wpivot[i], pCastice);
				
				pWHnizdo = par_cti_hnizdo(pRain->hWHnizdo[i]);

				if(pWHnizdo)
				{
					pWHnizdo->p_nx = &pRain->Wnx;
					pWHnizdo->p_ny = &pRain->Wny;
				}
			}
		}
	}

	par_go(pRain->pWSystem, &pRain->flag, 0, 0);

	pRain->fDist = fRadius;
	pRain->bWaterCircles = 1;

	return 1;
}


void am_Flip(LEVELINFO *p_Level, ANIMATION_MODULE *p_am, char bAll, RECT_LINE *rline, int rsize, int bTutor, int bTText, int ty)
{
	RECT	r;

	char c;
	int i, x = 0;
	float s[2];
	float pos[4];

	s[0] = (float)hwconf.xres / 1024.0f;
	s[1] = (float)hwconf.yres / 768.0f;

	am_Animate_Items(p_am, p_Level);
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
	am_Do_Star_Lights(p_Level);
	am_Do_StarFall(p_Level);
	am_Do_CandleSparks(p_Level);
	am_Do_CandleSmoke(p_Level);

	r.left = mx;
	r.top = my;
	r.right = _3dCur.idx;
	r.bottom = _3dCur.idy;

	if(rline)
	{
		ddx2AddRectItem(rline, r, 0);

		if(!rline->rlast)
			ddx2SetRect(NULL, 0);
		else
		{
			ZeroMemory(rDrawRect, sizeof(RECT) * DRAW_RECT_NUM);

			for(i=0;i<rline->rlast;i++)
				if(!_2d_Is_InRectLine(rDrawRect, &rline->rect[i].rect, x))
				{
					memcpy(&rDrawRect[x], &rline->rect[i].rect, sizeof(RECT));

					x++;
				}

			ddx2SetRect(rDrawRect, x);
		}
	}

	p_Level->bGameResume = 0;

	spracuj_spravy(0);

	kom_klavesy_hra();
	kom_kresli_level();

	_3d_Begin_Draw();

	if(!bAll)
		_3d_Draw_MenusB(p_Level, -1, &c, s, bTutor, bTText, ty);

	_3d_End_Draw();

	if(bAll)
		ddx2RenderujVse(p_ber);
	else
		ddx2RenderujMenu(p_ber);

	_3d_Begin_Draw();
	_3d_Set_Smooth();

	if(p_Level->bRestart)
		p_Level->iMessageBoxReturn = _3d_Draw_MessageBox(5, s);

	if(p_Level->bReturnToMenu)
		p_Level->iMessageBoxReturn = _3d_Draw_MessageBox(7, s);

	pos[0] = (float)mi.x + _3dCur.iaddx;
	pos[1] = (float)mi.y + _3dCur.iaddy;
	pos[2] = (float)mi.x + _3dCur.idx + _3dCur.iaddx;
	pos[3] = (float)mi.y + _3dCur.idy + _3dCur.iaddy;
	_3d_Draw_Box(p_Level->iCursor, pos);
	_3d_End_Draw();

	flip();

	_2d_Clear_RectLine(rline);

	mx = mi.x;
	my = mi.y;
}

void am_FlipA(LEVELINFO *p_Level, ANIMATION_MODULE *p_am, char bAll, RECT_LINE *rline, int rsize, int bTutor, int bTText, int ty)
{
	char c;
	float s[2];
	float pos[4];

	s[0] = (float)hwconf.xres / 1024.0f;
	s[1] = (float)hwconf.yres / 768.0f;

	am_Animate_Items(p_am, p_Level);
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
	am_Do_Star_Lights(p_Level);
	am_Do_StarFall(p_Level);
	am_Do_CandleSparks(p_Level);
	am_Do_CandleSmoke(p_Level);

	p_Level->bGameResume = 0;
	spracuj_spravy(0);

	kom_klavesy_hra();
	kom_kresli_level();

	_3d_Begin_Draw();

	_3d_Draw_MenusB(p_Level, -1, &c, s, bTutor, bTText, ty);

	if(!p_Level->iCursor)
	{
		pos[0] = (float)mi.x + _3dCur.iaddx;
		pos[1] = (float)mi.y + _3dCur.iaddy;
		pos[2] = (float)mi.x + _3dCur.idx + _3dCur.iaddx;
		pos[3] = (float)mi.y + _3dCur.idy + _3dCur.iaddy;
	}
	else
	{
		pos[0] = (float)mi.x;
		pos[1] = (float)mi.y;
		pos[2] = (float)mi.x + 32;
		pos[3] = (float)mi.y + 32;
	}

	_3d_Draw_Box(p_Level->iCursor, pos);
	_3d_End_Draw();

	flip();

	mx = mi.x;
	my = mi.y;
}

void am_Start_Gen_Animation(char *cMesh, LEVELINFO *p_Level)
{
	int iAnimation = K_CHYBA, iMesh = K_CHYBA;
	BOD b = {(float)(rand()%2),0,(float)(rand()%2)};
	BOD p;
	int anim, i, f1, f2;
	float r;


	iMesh = kom_pridej_mesh_do_levelu(cMesh);

	if(iMesh == K_CHYBA)
		return;

	if(!b.x && !b.z)
	{
		b.x = 1.0f;
		b.z = 1.0f;
	}

	anim = sim_vyrob_animaci(1,4,0);
	kom_mesh_get_pivot(iMesh, &p);

	sim_vloz_pivot(anim, p.x, p.y, p.z);

	sim_vloz_klic_posun(anim, 0, 0, 0, 0, 0);

	r = rand()/(float)(RAND_MAX * 40);

	if(r < 0.05f)
		r = 0.05f;

	f1 = (int)ceil(r * 800);

	r = rand()/(float)(RAND_MAX * 40);

	if(r < 0.05f)
		r = 0.05f;
	
	f2 = (int)ceil((r * 800) + f1);

	sim_vloz_klic_rotace(anim,0,&b,0,0);

	i = rand()%2;

	r = (rand()/(float)(RAND_MAX)) * 2;

	if(i) r *= -1;

	sim_vloz_klic_rotace(anim,1,&b,r,f1);
	
	r = (rand()/(float)(RAND_MAX)) * 2;

	if(!i) r *= -1;
	
	sim_vloz_klic_rotace(anim,2,&b,r,f1 + f2);
	sim_vloz_klic_rotace(anim,3,&b,0,2*f1 + f2);

	sim_interpoluj_animaci(anim,2*f1 + f2 + 1,1);
						
	iAnimation = rani_aktivuj(anim, &p_Level->TrashFlag,GK_LOOP,0,0);

	if(iAnimation == K_CHYBA)
		return;

	rani_privaz_mesh(iAnimation, iMesh, 0);
}


int find_free_swamp_light(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(!p_Level->SwampL[i].bUsed)
			return i;

	return -1;
}

void am_Create_Swamp_Light(LEVELINFO *p_Level, int iStart, int iPause, float *pos)
{
	STREET_LIGHT_EFFECT	*psle;
	int hSvetlo;
	int idx = find_free_swamp_light(p_Level);

	if(idx < 0)
		return;

	psle = &p_Level->SwampL[idx];

	ZeroMemory(psle, sizeof(STREET_LIGHT_EFFECT));

	psle->bUsed = 1;
	psle->eCounter = 0;
	psle->ePause = iPause;
	psle->epCounter = iPause;
	psle->eStart = iStart;
	psle->hFlare = -1;
	psle->dwEfTime = 0;
	psle->hSvetlo = -1;
	psle->pos[0] = pos[0];
	psle->pos[1] = pos[1];
	psle->pos[2] = pos[2];
	psle->Speed = (randf() / (float)RAND_MAX) * 0.3f;
	psle->fdx = (randf() / (float)RAND_MAX) * 0.3f;

	psle->dwRiseTime = (int)ceil(rand() / (float)RAND_MAX) * 150;

	if(psle->dwRiseTime < 50)
		psle->dwRiseTime = 50;


	psle->dwLightTime = (int)ceil(rand() / (float)RAND_MAX) * 250;

	if(psle->dwLightTime < psle->dwRiseTime)
		psle->dwLightTime = psle->dwRiseTime + 50;

	hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);

	if(hSvetlo != -1)
	{
		sdl_svetlo_set_pos(hSvetlo, (BOD *)psle->pos);
		sdl_svetlo_set_diff(hSvetlo, 0.996f, 0.617f, 0, 1, psle->Speed * 200, psle->Speed * 200);

		psle->hSvetlo = hSvetlo;
	}

	psle->hFlare = kom_flare_vyrob(0);

	if(psle->hFlare != -1)
	{
		int m = kom_najdi_material("flare4");

		if(m != -1)
		{
				kom_flare_set_param(psle->hFlare, 0.996f, 0.617f, 0, 0.5f, psle->fdx, psle->fdx, m, GL_ONE);
				kom_flare_set_pivot(psle->hFlare, (BOD *) &psle->pos);
		}
	}
}

void am_Do_Swamp_Lights(LEVELINFO *p_Level)
{
	STREET_LIGHT_EFFECT	*psle;
	int i;
	DWORD e = ber.TimeLastFrame;
	float k = ber.TimeLastFrame / 1000.0f;
	LENS_FLARE *pFlare;
	DWORD t = timeGetTime();
	DWORD ep;

	if(p_Level->bGameResume)
	{
		e = 0;
		k = 0;
	}

	for(i=0;i<10;i++)
		if(p_Level->SwampL[i].bUsed)
		{
			psle = &p_Level->SwampL[i];

			if(psle->eCounter < psle->eStart)
				psle->eCounter+=e;
			else
				if(psle->epCounter < psle->ePause)
				{
					psle->epCounter+=e;
					
					if(psle->hFlare != -1)
					{
						ep = t - psle->dwEfTime;

						if(ep < psle->dwRiseTime)
						{
							pFlare = kom_flare_get_pointer(psle->hFlare);

							pFlare->dx += psle->Speed * k;
							pFlare->dy += psle->Speed * k;

							if(pFlare->dx > 0.5f)
							{
								pFlare->dx = psle->fdx;
								pFlare->dy = psle->fdx;
							}
						}
						else
							if(ep >= psle->dwRiseTime && ep <= 2*psle->dwRiseTime)
							{
								pFlare = kom_flare_get_pointer(psle->hFlare);

								pFlare->dx -= psle->Speed * k;
								pFlare->dy -= psle->Speed * k;

								if(pFlare->dx < psle->fdx)
								{
									pFlare->dy = psle->fdx;
									pFlare->dx = psle->fdx;
								}
							}
					}
				}
				else
				{
					psle->epCounter = 0;
					psle->dwEfTime = timeGetTime();

					psle->dwRiseTime = (int)ceil(rand() / (float)RAND_MAX) * 150;

					if(psle->dwRiseTime < 50)
						psle->dwRiseTime = 50;

					psle->dwLightTime = (int)ceil(rand() / (float)RAND_MAX) * 250;

					if(psle->dwLightTime < psle->dwRiseTime)
						psle->dwLightTime = psle->dwRiseTime + 50;

					psle->ePause = (2 * psle->dwRiseTime);// + psle->dwLightTime;
				}
		}
}

void am_Do_Swamp_Lights_Release(LEVELINFO *p_Level)
{
	int i;
	STREET_LIGHT_EFFECT	*psle;

	for(i=0;i<10;i++)
		if(p_Level->SwampL[i].bUsed)
		{
			kprintf(1, "am_Do_Swamp_Lights_Release[%d] ...", i);
			psle = &p_Level->SwampL[i];

			if(psle->hFlare != -1)
			{
				kprintf(1, "kom_flare_zrus[%d] ...", i);
				kom_flare_zrus(psle->hFlare);
			}

			if(psle->hSvetlo != -1)
			{
				kprintf(1, "sdl_svetlo_zrus[%d] ...", i);
				sdl_svetlo_zrus(psle->hSvetlo);
			}
		}
}

void am_Do_Star_Lights_Release(LEVELINFO *p_Level)
{
	int i;
	STREET_LIGHT_EFFECT	*psle;

	for(i=0;i<200;i++)
		if(p_Level->StarL[i].bUsed)
		{
			kprintf(1, "am_Do_Star_Lights_Release[%d] ...", i);

			psle = &p_Level->StarL[i];

			if(psle->hFlare != -1)
			{
				kprintf(1, "kom_flare_zrus[%d] ...", i);
				kom_flare_zrus(psle->hFlare);
			}

			if(psle->hSvetlo != -1)
			{
				kprintf(1, "sdl_svetlo_zrus[%d] ...", i);
				sdl_svetlo_zrus(psle->hSvetlo);
			}
		}
}

void am_Gen_Swamp_Lights(LEVELINFO *p_Level, RECT *r)
{
	BOD p1, p2;
	float p[3];
	int i,t;
	int done = 0;

	kom_get_level_size(&p1, &p2);

	p1.x -= 25.0f;
	p1.z -= 25.0f;

	p2.x += 25.0f;
	p2.z += 25.0f;

	//generuj staticke msvetilka
	for(i=0;i<10;i++)
	{
		t = (int)(int)((randf() / (float)RAND_MAX) * 1000.0f);

		done = 0;

		while(!done)
		{
			p[0] = r->left + (float)(rand() / (float)RAND_MAX) * am_vzdal_bodu_single((float)r->left, (float)r->right);		
			p[1] = p_ber->y_start + 1 + ((rand() / (float)RAND_MAX) * 40);
			p[2] = r->top + (rand() / (float)RAND_MAX) * am_vzdal_bodu_single((float)r->top, (float)r->bottom);

			if((p[0] < p1.x || p[0] > p2.x) && (p[2] < p1.z || p[2] > p2.z))
				done = 1;
		}	
		
		am_Create_Swamp_Light(p_Level, t, t, p);
	}
}

int find_used_swamp_light(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->SwampL[i].bUsed && !p_Level->SwampL[i].bConnected)
			return i;

	return -1;
}

int am_Create_Fairy(LEVELINFO *p_Level, FAIRY_EFFECT *pF, float pos[3], float fLowPlain, char *cMaterial,
					float fRadius, float fStartY, RECT	*pScene)
{
	PAR_KOUR_STOPA	*pCastice;
	int material;
	int iSwampL = find_used_swamp_light(p_Level);

	if(iSwampL == -1)
		return 0;

	ZeroMemory(pF, sizeof(FAIRY_EFFECT));

	pF->hHnizdo = -1;
	pF->pSystem = -1;
//	pF->iSound = ap_Play_Sound(2, 0, 1, pos, 139, NULL, &ad);

	memcpy(&pF->rScene, pScene, sizeof(RECT));

	pF->csize = 300;
	pF->pCastice = (PAR_KOUR_STOPA *)malloc(pF->csize * sizeof(PAR_KOUR_STOPA));
	pF->fLowPlain = fLowPlain;

	if(!pF->pCastice)
		return 0;

	pF->faire_dir[0] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
	pF->faire_dir[2] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);

	norm_vect(&pF->faire_dir[0], &pF->faire_dir[1], &pF->faire_dir[2]);

	pF->speed = 20 + ((randf()/(float)(RAND_MAX)) * 60);

	memcpy((void *)&pF->pos, (void *)pos, 3 * sizeof(float));
	
	if(p_Level->SwampL[iSwampL].hFlare != K_CHYBA)
		kom_flare_set_pivot(p_Level->SwampL[iSwampL].hFlare, &pF->pivot);


	if(p_Level->SwampL[iSwampL].hSvetlo != K_CHYBA)
		sdl_svetlo_set_pos(p_Level->SwampL[iSwampL].hSvetlo, &pF->pivot);

	p_Level->SwampL[iSwampL].bConnected = 1;
	pF->iSwampLight = iSwampL;

	material = kom_najdi_material(cMaterial);

	if(material == -1)
	{
		kprintf(1,"Nelze najit material %s", cMaterial);
		free((void *) pF->pCastice);
		return 0;
	}

	//gen sinu

	pF->falngle[0] = (randf() / (float)RAND_MAX) * 4;
	pF->falngle[1] = (randf() / (float)RAND_MAX) * 4;
	pF->falngle[2] = (randf() / (float)RAND_MAX) * 4;

	pF->fsinspeed[0] = (randf() / (float)RAND_MAX) * 2;
	pF->fsinspeed[1] = (randf() / (float)RAND_MAX) * 2;
	pF->fsinspeed[2] = (randf() / (float)RAND_MAX) * 2;

	pF->fmult[0] = (randf() / (float)RAND_MAX) * 10;
	pF->fmult[1] = (randf() / (float)RAND_MAX) * 10;
	pF->fmult[2] = (randf() / (float)RAND_MAX) * 10;

	pCastice = &pF->pCastice[0];

	pCastice->rychlost_x = 0.06f;
	pCastice->rychlost_y = 0.06f;

	pCastice->utlum_x = pCastice->utlum_y = 0.0f;
	
	pCastice->r = 0.98f;
	pCastice->g = 0.91f;
	pCastice->b = 0.33f;
	
	pCastice->a = 0.75f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pF->pSystem = par_vyrob();

	if(pF->pSystem == -1)
	{
		free((void *) pF->pCastice);
		return 0;
	}

	memcpy((void *)&pF->pivot, (void *)pos, 3 * sizeof(float));

	par_set_param(pF->pSystem, material, TPAR_NO_FOG | TPAR_YPLANE_LOW | TPAR_DIR | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, 
				  &pF->pivot, &pF->pivot);	

	pCastice = pF->pCastice;

	par_vloz_kour_stopu(pF->pSystem, pCastice, pF->csize);

	pF->hHnizdo = par_vloz_hnizdo(pF->pSystem);

	if(pF->hHnizdo != -1)
	{
		par_vloz_hnizdo_komplet(pF->hHnizdo, 5, &pF->pivot, pCastice);
		par_vloz_hnizdo_timer(pF->hHnizdo, 5, 0);

		memcpy((void *)&pF->pivot, (void *)pos, 3 * sizeof(float));

		pF->pivot.x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
		pF->pivot.z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
		pF->pivot.y = fStartY;

		memcpy((void *)&pF->dir, (void *)pos, 3 * sizeof(float));

		pF->dir.x = 0;
		pF->dir.y = -1;
		pF->dir.z = 0;

		par_vloz_hnizdo_dir(pF->hHnizdo, &pF->dir);
		par_vloz_hnizdo_y_plane(pF->hHnizdo, pF->fLowPlain);
	}

	par_pripoj_funkci(pF->pSystem,anmend_ZrusCastice3,0,0,(void *) pF->pCastice);

	par_go(pF->pSystem, &pF->flag, 0, 0);


	pF->dwTime = 0;
	pF->dwRegTime = 250;

	return 1;
}

int find_free_fairy_system(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<5;i++)
		if(p_Level->FairyEffect[i].pSystem == -1)
			return i;

	return -1;
}

int am_Create_Fairies(LEVELINFO *p_Level, RECT *r)
{
	//RECT	r = {-200, -200, 200, 200};
	int i, idx;

	float pos[3], low_plane, radius, start;

	for(i=0;i<5;i++)
	{
		idx = find_free_fairy_system(p_Level);

		if(idx == -1)
			return 0;
	
		radius = 10;
		low_plane = p_ber->y_start;
		start = p_ber->y_start + 5 + ((rand() / (float)RAND_MAX) * 25);

		pos[0] = r->left + (float)(rand() / (float)RAND_MAX) * am_vzdal_bodu_single((float)r->left, (float)r->right);		
		pos[1] = p_ber->y_start + 25 + ((rand() / (float)RAND_MAX) * 25);
		pos[2] = r->top + (rand() / (float)RAND_MAX) * am_vzdal_bodu_single((float)r->top, (float)r->bottom);

		am_Create_Fairy(p_Level, &p_Level->FairyEffect[idx], pos, low_plane, "flare12", radius, start, r);
	}

	return 1;
}

void am_move_Fiere(FAIRY_EFFECT *pF, LEVELINFO *p_Level)
{
	float f = ber.TimeLastFrame / 1000.0f;
	int c = 0;

	memcpy((void *)&pF->pivot, (void *)pF->pos, 3 * sizeof(float));

	pF->pivot.x += pF->faire_dir[0] * pF->speed * f;
	pF->pivot.z += pF->faire_dir[2] * pF->speed * f;

	if(pF->pivot.x < pF->rScene.left)
	{
		pF->pivot.x = (float)pF->rScene.left;
		c++;
	}
	  
	if(pF->pivot.x > pF->rScene.right)
	{
		pF->pivot.x = (float)pF->rScene.right;
		c++;
	}
	
	if(pF->pivot.z < pF->rScene.top)
	{
		pF->pivot.z = (float)pF->rScene.top;
		c++;
	}

	if(pF->pivot.z > pF->rScene.bottom)
	{
		pF->pivot.z = (float)pF->rScene.bottom;
		c++;
	}

	if(c)
	{
		pF->faire_dir[0] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);
		pF->faire_dir[2] = (rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX);

		norm_vect(&pF->faire_dir[0], &pF->faire_dir[1], &pF->faire_dir[2]);

		pF->speed = 20 + ((randf()/(float)(RAND_MAX)) * 60);

		pF->fsinspeed[0] = (randf() / (float)RAND_MAX) * 2;
		pF->fsinspeed[1] = (randf() / (float)RAND_MAX) * 2;
		pF->fsinspeed[2] = (randf() / (float)RAND_MAX) * 2;

		pF->fmult[0] = (randf() / (float)RAND_MAX) * 10;
		pF->fmult[1] = (randf() / (float)RAND_MAX) * 10;
		pF->fmult[2] = (randf() / (float)RAND_MAX) * 10;
	}

	memcpy((void *)pF->pos, (void *)&pF->pivot, 3 * sizeof(float));

	//modulace pozice;
	pF->falngle[0] += pF->fsinspeed[0] * f;
	pF->falngle[1] += pF->fsinspeed[1] * f;
	pF->falngle[2] += pF->fsinspeed[2] * f;

	pF->pivot.x += ((float)sin(pF->falngle[0]) * pF->fmult[0]);
	pF->pivot.y += ((float)sin(pF->falngle[1]) * pF->fmult[1]);
	pF->pivot.z += ((float)sin(pF->falngle[2]) * pF->fmult[2]);

	//kprintf(1, "f = %f", (float)sin(pF->falngle[0]) * pF->fmult[0]);

	if(p_Level->SwampL[pF->iSwampLight].hSvetlo != K_CHYBA)
		sdl_svetlo_set_pos(p_Level->SwampL[pF->iSwampLight].hSvetlo, &pF->pivot);

	if(pF->iSound != -1)
	{
		float ftmp[3];
	
		memcpy((void *)ftmp, (void *)&pF->pivot, 3 * sizeof(float));
		//adas_Set_Source_Position(PARTICULAR_SOUND_SOURCE, 2, pF->iSound, ftmp);
	}

}

void am_Do_Fairies(LEVELINFO *p_Level)
{
	FAIRY_EFFECT *pF;
	int i;
	float f;

	for(i=0;i<4;i++)
	if(p_Level->FairyEffect[i].pSystem != -1)
	{
		pF = &p_Level->FairyEffect[i];

		//pF->dwTime += ber.TimeLastFrame;

		pF->dir.y = ((-randf()/(float)(RAND_MAX)) * 60.0f);

		if(pF->dir.y > -10)
			pF->dir.y = -10 + ((-randf()/(float)(RAND_MAX)) * 20.0f);

		f = (randf()/(float)(RAND_MAX)) * 1.0f;
		par_vloz_hnizdo_scale(pF->hHnizdo, f,f, 0, 0);

		am_move_Fiere(pF, p_Level);
	}
}

int find_free_star_system(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<200;i++)
		if(!p_Level->StarL[i].bUsed)
			return i;

	return -1;
}


void am_Gen_Star_Light(LEVELINFO *p_Level, int iStart, int iPause, float *pos, int iNoLight)
{
	STREET_LIGHT_EFFECT	*psle;
	int hSvetlo;
	int idx = find_free_star_system(p_Level);

	if(idx < 0)
		return;

	psle = &p_Level->StarL[idx];

	ZeroMemory(psle, sizeof(STREET_LIGHT_EFFECT));

	psle->bUsed = 1;
	psle->eCounter = 0;
	psle->ePause = iPause;
	psle->epCounter = iPause;
	psle->eStart = iStart;
	psle->hFlare = -1;
	psle->dwEfTime = 0;
	psle->hSvetlo = -1;
	psle->pos[0] = pos[0];
	psle->pos[1] = pos[1];
	psle->pos[2] = pos[2];
	psle->Speed = (randf() / (float)RAND_MAX) * 0.3f;

	if(!iNoLight)
		psle->fdx = 0.1f + (randf() / (float)RAND_MAX) * 0.3f;
	else
		psle->fdx = 0.01f + (randf() / (float)RAND_MAX) * 0.1f;

	psle->dwRiseTime = (int)ceil(rand() / (float)RAND_MAX) * 150;

	if(psle->dwRiseTime < 50)
		psle->dwRiseTime = 50;


	psle->dwLightTime = (int)ceil(rand() / (float)RAND_MAX) * 250;

	if(psle->dwLightTime < psle->dwRiseTime)
		psle->dwLightTime = psle->dwRiseTime + 50;

	if(!iNoLight)
	{
		hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);

		if(hSvetlo != -1)
		{
			sdl_svetlo_set_pos(hSvetlo, (BOD *)psle->pos);
			sdl_svetlo_set_diff(hSvetlo, 0.765f, 0.89f, 0.89f, 1, psle->Speed * 1000, psle->Speed * 1000);

			psle->hSvetlo = hSvetlo;
		}
	}

	psle->hFlare = kom_flare_vyrob(0);

	if(psle->hFlare != -1)
	{
		int m = kom_najdi_material("flare4");

		if(m != -1)
		{
				kom_flare_set_param(psle->hFlare, 0.765f, 0.89f, 0.89f, 0.75f, psle->fdx, psle->fdx, m, GL_ONE);
				kom_flare_set_pivot(psle->hFlare, (BOD *) &psle->pos);
		}
	}
}

void am_Gen_Star_Lights(LEVELINFO *p_Level, float *pos)
{
	BOD p1, p2;
	int done = 0;
	float p[3];
	int i,t, s;

	kom_get_level_size(&p1, &p2);

	p1.x -= 50.0f;
	p1.z -= 50.0f;

	p2.x += 50.0f;
	p2.z += 50.0f;
	
	for(i=0;i<200;i++)
	{
		s = (int)(int)((randf() / (float)RAND_MAX) * 10000.0f);
		t = (int)(int)((randf() / (float)RAND_MAX) * 1000.0f);

		done = 0;

		while(!done)
		{
			p[0] = pos[0] + (rand() / (float)RAND_MAX) * am_vzdal_bodu_single((float)pos[0], (float)pos[3]);		
			p[1] = -50 + pos[1] + (rand() / (float)RAND_MAX) * am_vzdal_bodu_single((float)pos[1], (float)pos[4]);
			p[2] = pos[2] + (rand() / (float)RAND_MAX) * am_vzdal_bodu_single((float)pos[2], (float)pos[5]);
		
			if((p[0] < p1.x || p[0] > p2.x) && (p[2] < p1.z || p[2] > p2.z))
				done = 1;
		}	

		am_Gen_Star_Light(p_Level, s, t, p, i);
	}
}

void am_Do_Star_Lights(LEVELINFO *p_Level)
{
	STREET_LIGHT_EFFECT	*psle;
	int i;
	DWORD e = ber.TimeLastFrame;
	float k = ber.TimeLastFrame / 1000.0f;
	LENS_FLARE *pFlare;
	DWORD t = timeGetTime();
	DWORD ep;

	for(i=0;i<200;i++)
		if(p_Level->StarL[i].bUsed)
		{
			psle = &p_Level->StarL[i];

			if(psle->eCounter < psle->eStart)
				psle->eCounter+=e;
			else
				if(psle->epCounter < psle->ePause)
				{
					psle->epCounter+=e;
					
					if(psle->hFlare != -1)
					{
						ep = t - psle->dwEfTime;

						if(ep < psle->dwRiseTime)
						{
							pFlare = kom_flare_get_pointer(psle->hFlare);

							pFlare->dx += psle->Speed * k;
							pFlare->dy += psle->Speed * k;

							if(pFlare->dx > 0.2f)
							{
								pFlare->dx = psle->fdx;
								pFlare->dy = psle->fdx;
							}
						}
						else
							if(ep >= psle->dwRiseTime && ep <= 2*psle->dwRiseTime)
							{
								pFlare = kom_flare_get_pointer(psle->hFlare);

								pFlare->dx -= psle->Speed * k;
								pFlare->dy -= psle->Speed * k;

								if(pFlare->dx < psle->fdx)
								{
									pFlare->dy = psle->fdx;
									pFlare->dx = psle->fdx;
								}
							}
					}
				}
				else
				{
					psle->epCounter = 0;
					psle->dwEfTime = timeGetTime();

					psle->dwRiseTime = (int)ceil(rand() / (float)RAND_MAX) * 150;

					if(psle->dwRiseTime < 50)
						psle->dwRiseTime = 50;

					psle->dwLightTime = (int)ceil(rand() / (float)RAND_MAX) * 250;

					if(psle->dwLightTime < psle->dwRiseTime)
						psle->dwLightTime = psle->dwRiseTime + 50;

					psle->ePause = (2 * psle->dwRiseTime);// + psle->dwLightTime;
				}
		}
}

int am_Create_Falling_Star(LEVELINFO *p_Level, FAIRY_EFFECT *pF, float pos[3], float fLowPlain, char *cMaterial,
							float fRadius, float fStartY, float	*pScene)
{
	float pos1[3], pos2[3];
	PAR_KOUR_STOPA	*pCastice;
	int material;
	int iStarL = 0;

	if(iStarL == -1)
		return 0;

	ZeroMemory(pF, sizeof(FAIRY_EFFECT));

	pF->hHnizdo = -1;
	pF->pSystem = -1;
	pF->iSound = -1;
	
	memcpy(&pF->fScene, pScene, 6 * sizeof(float));

	pF->csize = 7000;
	pF->pCastice = (PAR_KOUR_STOPA *)malloc(pF->csize * sizeof(PAR_KOUR_STOPA));
	pF->fLowPlain = fLowPlain;

	if(!pF->pCastice)
		return 0;

	pos1[0] = pScene[0] + ((randf()/(float)(RAND_MAX)) * am_vzdal_bodu_single(pScene[3], pScene[0]));
	pos1[1] = pScene[4];
	pos1[2] = pScene[2] + ((randf()/(float)(RAND_MAX)) * am_vzdal_bodu_single(pScene[2], pScene[5]));

	pos2[0] = pScene[0] + ((randf()/(float)(RAND_MAX)) * am_vzdal_bodu_single(pScene[3], pScene[0]));
	pos2[1] = pScene[1];
	pos2[2] = pScene[2] + ((randf()/(float)(RAND_MAX)) * am_vzdal_bodu_single(pScene[2], pScene[5]));

	Make_Vector(pos1, pos2, pF->faire_dir);

	norm_vect(&pF->faire_dir[0], &pF->faire_dir[1], &pF->faire_dir[2]);

	pF->speed = 40 + ((randf()/(float)(RAND_MAX)) * 100);

	memcpy((void *)&pF->pos, (void *)pos1, 3 * sizeof(float));
	
	if(p_Level->StarL[iStarL].hFlare != K_CHYBA)
		kom_flare_set_pivot(p_Level->StarL[iStarL].hFlare, &pF->pivot);

	if(p_Level->StarL[iStarL].hSvetlo != K_CHYBA)
		sdl_svetlo_set_pos(p_Level->StarL[iStarL].hSvetlo, &pF->pivot);

	p_Level->StarL[iStarL].bConnected = 1;
	pF->iSwampLight = iStarL;

	material = kom_najdi_material(cMaterial);

	if(material == -1)
	{
		kprintf(1,"Nelze najit material %s", cMaterial);
		free((void *) pF->pCastice);
		return 0;
	}

	//gen sinu

	pF->falngle[0] = (randf() / (float)RAND_MAX) * 4;
	pF->falngle[1] = (randf() / (float)RAND_MAX) * 4;
	pF->falngle[2] = (randf() / (float)RAND_MAX) * 4;

	pF->fsinspeed[0] = (randf() / (float)RAND_MAX) * 2;
	pF->fsinspeed[1] = (randf() / (float)RAND_MAX) * 2;
	pF->fsinspeed[2] = (randf() / (float)RAND_MAX) * 2;

	pF->fmult[0] = (randf() / (float)RAND_MAX) * 10;
	pF->fmult[1] = (randf() / (float)RAND_MAX) * 10;
	pF->fmult[2] = (randf() / (float)RAND_MAX) * 10;

	pCastice = &pF->pCastice[0];

	pCastice->rychlost_x = 0.06f;
	pCastice->rychlost_y = 0.06f;

	pCastice->utlum_x = pCastice->utlum_y = -0.1f;
	
	pCastice->r = 0.765f;
	pCastice->g = 0.89f;
	pCastice->b = 0.89f;
	
	pCastice->a = 0.75f;
	pCastice->da = pCastice->dr = pCastice->dg = pCastice->db = 0.0f;

	pF->pSystem = par_vyrob();

	if(pF->pSystem == -1)
	{
		free((void *) pF->pCastice);
		return 0;
	}

	memcpy((void *)&pF->pivot, (void *)pos, 3 * sizeof(float));

	par_set_param(pF->pSystem, material, TPAR_NO_FOG | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, 
				  &pF->pivot, &pF->pivot);	

	pCastice = pF->pCastice;

	par_vloz_kour_stopu(pF->pSystem, pCastice, pF->csize);

	pF->hHnizdo = par_vloz_hnizdo(pF->pSystem);

	if(pF->hHnizdo != -1)
	{
		par_vloz_hnizdo_komplet(pF->hHnizdo, 5, &pF->pivot, pCastice);
		par_vloz_hnizdo_timer(pF->hHnizdo, 5, 0);

		memcpy((void *)&pF->pivot, (void *)pos1, 3 * sizeof(float));

/*		pF->pivot.x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
		pF->pivot.z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * fRadius;
		pF->pivot.y = fStartY;*/

/*		memcpy((void *)&pF->dir, (void *)pos, 3 * sizeof(float));

		pF->dir.x = 0;
		pF->dir.y = -1;
		pF->dir.z = 0;

		par_vloz_hnizdo_dir(pF->hHnizdo, &pF->dir);*/
		//par_vloz_hnizdo_y_plane(pF->hHnizdo, pF->fLowPlain);
	}

	par_pripoj_funkci(pF->pSystem,anmend_ZrusCastice3,0,0,(void *) pF->pCastice);

	par_go(pF->pSystem, &pF->flag, 0, 0);

	pF->dwTime = 0;
	pF->dwRegTime = 250;

	return 1;
}

void am_move_Star(FAIRY_EFFECT *pF, LEVELINFO *p_Level)
{
	float pos1[3], pos2[3];
	float f = ber.TimeLastFrame / 1000.0f;
	int c = 0;

	if(p_Level->bGameResume)
	{
		f= 0;
	}

	memcpy((void *)&pF->pivot, (void *)pF->pos, 3 * sizeof(float));

	pF->pivot.x += pF->faire_dir[0] * pF->speed * f;
	pF->pivot.y += pF->faire_dir[1] * pF->speed * f;
	pF->pivot.z += pF->faire_dir[2] * pF->speed * f;

	if(pF->pivot.y < pF->fScene[1])
		c++;

	if(c)
	{
		pos1[0] = pF->fScene[0] + ((randf()/(float)(RAND_MAX)) * pF->fScene[3]);
		pos1[1] = pF->fScene[4];
		pos1[2] = pF->fScene[2] + ((randf()/(float)(RAND_MAX)) * pF->fScene[5]);

		pos2[0] = pF->fScene[0] + ((randf()/(float)(RAND_MAX)) * pF->fScene[3]);
		pos2[1] = pF->fScene[1];
		pos2[2] = pF->fScene[2] + ((randf()/(float)(RAND_MAX)) * pF->fScene[5]);

		Make_Vector(pos1, pos2, pF->faire_dir);

		norm_vect(&pF->faire_dir[0], &pF->faire_dir[1], &pF->faire_dir[2]);

		pF->speed = 40 + ((randf()/(float)(RAND_MAX)) * 100);

		memcpy((void *)&pF->pivot, (void *)pos1, 3 * sizeof(float));
	}

	memcpy((void *)pF->pos, (void *)&pF->pivot, 3 * sizeof(float));

	if(p_Level->StarL[pF->iSwampLight].hSvetlo != K_CHYBA)
		sdl_svetlo_set_pos(p_Level->StarL[pF->iSwampLight].hSvetlo, &pF->pivot);
}

void am_Do_StarFall(LEVELINFO *p_Level)
{
	PAR_KOUR_STOPA *pCastice;
	FAIRY_EFFECT *pF;
	int i;
	float f, f1;

	for(i=0;i<2;i++)
	if(p_Level->StarFall[i].pSystem != -1)
	{
		pF = &p_Level->StarFall[i];

		f = (randf()/(float)(RAND_MAX)) * 4.0f;

		f1 = 0.025f + (randf()/(float)(RAND_MAX)) * 2.0f;

		par_vloz_hnizdo_scale(pF->hHnizdo, f,f, -f1, -f1);

		am_move_Star(pF, p_Level);

		pCastice = 	par_cti_hnizdo_castice(pF->hHnizdo);

		// test na velikost castic. Pokud je moc mala, tak pric s ni
		if(pCastice)
		{
			while(pCastice)
			{
				if(pCastice->rychlost_x < 0.01f ||
				   pCastice->rychlost_y < 0.01f)
					pCastice->a = 0;

				pCastice = pCastice->p_next;
			}
		}

	}
}

int am_Create_Falling_Stars(LEVELINFO *p_Level, float *r)
{
	int i, idx;

	float pos[3], low_plane, radius, start;

	for(i=0;i<1;i++)
	{
		idx = 0;

		if(idx == -1)
			return 0;
	
		radius = 10;
		low_plane = r[1];
		start = r[4];

		pos[0] = r[0] + randf() * r[3];
		pos[1] = r[4];
		pos[2] = r[2] + randf() * r[5];

		am_Create_Falling_Star(p_Level, &p_Level->StarFall[idx], pos, low_plane, "flare100", radius, start, r);
	}

	return 1;
}

int am_Get_Free_CandleSystem(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->CandleEffect[i].pSystem == -1)
			return i;

	return -1;
}

int am_Create_CandleSystem(float *pos, LEVELINFO *p_Level)
{
	int m,k, hSvetlo;
	PAR_KOUR_STOPA	*pKourovaS;
	CANDLE_EFFECT	*pSystem;

	k = am_Get_Free_CandleSystem(p_Level);

	if( k == -1)
		return -1;

	pSystem = &p_Level->CandleEffect[k];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(200 * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return -1;

	pSystem->pCastice = pKourovaS;
	pSystem->csize = 200;

	m = kom_najdi_material("flare5");
	
	if(m == -1)
		kprintf(1,"Nelze najit material flare5");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 1.0f;
	pKourovaS[0].utlum_x = -2;
	pKourovaS[0].utlum_y = -2;

	pKourovaS[0].r = 1;
	pKourovaS[0].g = 1;
	pKourovaS[0].b = 0.5f;

	pKourovaS[0].a = 0.75f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = pKourovaS[0].da = 0;
	pKourovaS[0].ka = 0;

	pSystem->pSystem = par_vyrob();

	par_set_param(pSystem->pSystem, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_DIR | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);
	
	par_vloz_kour_stopu(pSystem->pSystem, pKourovaS, pSystem->csize);

	pSystem->hHnizdo = par_vloz_hnizdo(pSystem->pSystem);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo, 10, &pSystem->pivot, pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo, 10, -10);

	memcpy((void *)&pSystem->pivot, (void *)pos, 3*sizeof(float));
	memcpy((void *)&pSystem->pos, (void *)pos, 3*sizeof(float));

	pSystem->dir.x = 0;
	pSystem->dir.y = (randf()/(float)(RAND_MAX)) * 1;
	pSystem->dir.z = 0;				

	par_vloz_hnizdo_y_plane(pSystem->hHnizdo, pos[1] + 10);

	par_vloz_hnizdo_dir(pSystem->hHnizdo, &pSystem->dir);

	par_pripoj_funkci(pSystem->pSystem,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->pSystem, &pSystem->flag, 0, 0);

	hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);

	if(hSvetlo != -1)
	{
		int cfames = 50 * ((rand()%9) + 1);
		int	cklicu = (int) floor(cfames / 4.0f);
		float dosah;
		int	frm;

		sdl_svetlo_set_pos(hSvetlo, (BOD *)pSystem->pos);
		sdl_svetlo_set_diff(hSvetlo, 0.25f, 0.25f, 0.197f, 0, 0, 0);
		
		sdl_anim_vyrob(hSvetlo, cfames,0,0,cklicu);
		
		sdl_anim_vloz_klic_vzdal(hSvetlo,0,30,0,30,0);
		sdl_anim_vloz_klic_vzdal(hSvetlo,cklicu-1,30,0,30,cfames-1);

		for(m=1;m<cklicu-1;m++)
		{
			dosah = 30 + (((rand()&0x1 ? randf() : -randf()) / RAND_MAX) * 25.0f);

			frm = (m * (int)floor(cfames / (float) cklicu));

			sdl_anim_vloz_klic_vzdal(hSvetlo, m, dosah, 0, dosah, frm);
		}
		
		sdl_anim_start(hSvetlo, &p_Level->TrashFlag,GK_LOOP,0,0);
	}

	pSystem->hSvetlo = hSvetlo;

	return k;
}

void am_Do_CandleSparks(LEVELINFO *p_Level)
{
	int i;
	float f;
	PAR_KOUR_STOPA *pCastice;
	CANDLE_EFFECT	*pSystem;

	for(i=0;i<6;i++)
		if(p_Level->CandleEffect[i].pSystem != -1)
		{
			pCastice = 	par_cti_hnizdo_castice(p_Level->CandleEffect[i].hHnizdo);
			pSystem = &p_Level->CandleEffect[i];

			// test na velikost castic. Pokud je moc mala, tak pric s ni
			if(pCastice)
			{
				while(pCastice)
				{
					if(pCastice->rychlost_x < 0.001f ||
					   pCastice->rychlost_y < 0.001f)
						pCastice->a = 0;

					pCastice = pCastice->p_next;
				}
			}
			
			memcpy((void *)&pSystem->pivot, (void *)&pSystem->pos, 3*sizeof(float));

			pSystem->pivot.x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			pSystem->pivot.y = pSystem->pos[1];
			pSystem->pivot.z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			//norm_vect_2D(&pSystem->pivot.x, &pSystem->pivot.y);

			f = (randf()/(float)(RAND_MAX)) * 2;

			if(f < 1)
				f = 1;

			par_vloz_hnizdo_scale(pSystem->hHnizdo, f,f, -2.0f, -2.0f);

			pSystem->dir.y = (randf()/(float)(RAND_MAX)) * 2;
		}
}

int am_Get_Free_SmokeSystem(LEVELINFO *p_Level)
{
	int i;

	for(i=0;i<6;i++)
		if(p_Level->CandleSmoke[i].pSystem == -1)
			return i;

	return -1;
}

int am_Create_CandleSmokeSystem(float *pos, LEVELINFO *p_Level)
{
	int m,k;
	PAR_KOUR_STOPA	*pKourovaS;
	CANDLE_EFFECT	*pSystem;

	k = am_Get_Free_CandleSystem(p_Level);

	if( k == -1)
		return -1;

	pSystem = &p_Level->CandleSmoke[k];

	pKourovaS = (PAR_KOUR_STOPA *) malloc(300 * sizeof(PAR_KOUR_STOPA));

	if(!pKourovaS)
		return -1;

	pSystem->pCastice = pKourovaS;
	pSystem->csize = 300;

	m = kom_najdi_material("mrak1_1");
	
	if(m == -1)
		kprintf(1,"Nelze najit material mrak1_1");

	pKourovaS[0].rychlost_x = pKourovaS[0].rychlost_y = (randf()/(float)(RAND_MAX)) / 10.0f;
	pKourovaS[0].utlum_x = 1;
	pKourovaS[0].utlum_y = 1;

	pKourovaS[0].r = 0.75f;
	pKourovaS[0].g = 0.75f;
	pKourovaS[0].b = 0.75f;

	pKourovaS[0].a = 0.5f;
	pKourovaS[0].dr = pKourovaS[0].dg = pKourovaS[0].db = 0;
	pKourovaS[0].da = -0.1f;
	pKourovaS[0].ka = 0;

	pSystem->pSystem = par_vyrob();

	par_set_param(pSystem->pSystem, m, TPAR_NO_FOG | TPAR_YPLANE_TOP | TPAR_DIR | TPAR_SCALE | TPAR_VETSI | TPAR_AUTOREMOVE, (BOD *)pos, NULL);
	
	par_vloz_kour_stopu(pSystem->pSystem, pKourovaS, pSystem->csize);

	pSystem->hHnizdo = par_vloz_hnizdo(pSystem->pSystem);

	par_vloz_hnizdo_komplet(pSystem->hHnizdo, 10, &pSystem->pivot, pKourovaS);
	par_vloz_hnizdo_timer(pSystem->hHnizdo, 10, -10);

	memcpy((void *)&pSystem->pivot, (void *)pos, 3*sizeof(float));
	memcpy((void *)&pSystem->pos, (void *)pos, 3*sizeof(float));

	pSystem->dir.x = 0;
	pSystem->dir.y = (randf()/(float)(RAND_MAX)) * 1;
	pSystem->dir.z = 0;				

	par_vloz_hnizdo_y_plane(pSystem->hHnizdo, pos[1] + 10);

	par_vloz_hnizdo_dir(pSystem->hHnizdo, &pSystem->dir);

	par_pripoj_funkci(pSystem->pSystem,anmend_ZrusCastice3,0,0,(void *) pKourovaS);

	par_go(pSystem->pSystem, &pSystem->flag, 0, 0);

	return k;
}

void am_Do_CandleSmoke(LEVELINFO *p_Level)
{
	int i;
	float f;
	CANDLE_EFFECT	*pSystem;

	for(i=0;i<6;i++)
		if(p_Level->CandleSmoke[i].pSystem != -1)
		{
			pSystem = &p_Level->CandleSmoke[i];
		
			memcpy((void *)&pSystem->pivot, (void *)&pSystem->pos, 3*sizeof(float));

			pSystem->pivot.x += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			pSystem->pivot.y = pSystem->pos[1];
			pSystem->pivot.z += ((rand()&0x1 ? randf() : -randf())/(float)(RAND_MAX)) * 0.25f;
			//norm_vect_2D(&pSystem->pivot.x, &pSystem->pivot.y);

			f = (randf()/(float)(RAND_MAX)) * 0.01f;

			par_vloz_hnizdo_scale(pSystem->hHnizdo, f,f, 1.0f, 1.0f);

			pSystem->dir.y = (randf()/(float)(RAND_MAX)) * 2;
		}
}

void am_Start_Voda_Kanal(void)
{
  BOD p = {0,0,0};

  MeshHandle mh;
  VodnikHandle voh;
  VodaHandle vh = vod_vyrob(5);

  mh = kom_pridej_mesh_do_levelu("voda.b2m_Voda1");
  vod_pridej_mesh(vh, mh);
  mh = kom_pridej_mesh_do_levelu("voda.b2m_Voda2");
  vod_pridej_mesh(vh, mh);
  mh = kom_pridej_mesh_do_levelu("voda.b2m_Voda3");
  vod_pridej_mesh(vh, mh);
  mh = kom_pridej_mesh_do_levelu("voda.b2m_Voda4");
  vod_pridej_mesh(vh, mh);
  mh = kom_pridej_mesh_do_levelu("voda.b2m_Voda5");
  vod_pridej_mesh(vh, mh);
  vod_uzavri_meshe(vh);
  
  voh = vod_vloz_vodnika(vh,&p, VOD_AKTIVNI | VOD_Z_PLANE |  VOD_DIFF_ADD | VOD_SPEC_ADD, 1, 50, 1, 0, 0, 200);
    
  vod_vloz_vodnika_spec(voh, 0.1f, 0.1f, 0.1f);
  vod_vloz_vodnika_add_spec(voh, 0, 0, 0);
  
  vod_vloz_vodnika_diff(voh, 0.05f, 0.05f, 0.05f, 0.125f);
  vod_vloz_vodnika_add_diff(voh, 0, 0, 0, 0);

  p.x = 100;
  p.z = 100;

  voh = vod_vloz_vodnika(vh,&p, VOD_AKTIVNI | VOD_X_PLANE |  VOD_DIFF_ADD | VOD_SPEC_ADD, 0.5f, 50, 1, 0, 0, 200);

  voh = vod_vloz_vodnika(vh,&p, VOD_AKTIVNI | VOD_BOD |  VOD_DIFF_ADD | VOD_SPEC_ADD, 0.25f, 50, 1, 0, 0, 200);
}

void am_Create_Item_Lights(LEVELINFO *p_Level)
{
	float pos[3];
	int i, hSvetlo, rot;

	for(i=0;i<p_Level->Size_of_Level;i++)
		if(p_Level->Level[i])
			if(p_Level->Level[i]->p_Object->Class == 13)
			{
				hSvetlo = sdl_svetlo_vyrob(SDL_UTLUM_LINEAR);

				if(hSvetlo != -1)
				{
					kom_mesh_get_float(p_Level->Level[i]->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

					sdl_svetlo_set_pos(hSvetlo, (BOD *)pos);
					sdl_svetlo_set_diff(hSvetlo, 1.0f, 1.0f, 0.75f, 0, 0, 0);
		
					sdl_anim_vyrob(hSvetlo, 40,0,0,4);
		
					sdl_anim_vloz_klic_vzdal(hSvetlo,0,1,0,1,0);
					sdl_anim_vloz_klic_vzdal(hSvetlo,1,1.5f,0,1.5f,9);
					sdl_anim_vloz_klic_vzdal(hSvetlo,2,2,0,2,19);
					sdl_anim_vloz_klic_vzdal(hSvetlo,3,1,0,1,29);
		
					sdl_anim_start(hSvetlo, &p_Level->TrashFlag,GK_LOOP,0,0);
				}

				p_Level->Level[i]->hSvetlo = hSvetlo;
			}
}


void am_Start_Voda_Akvarko(void)
{
  BOD p = {0,0,0};

  MeshHandle mh;
  VodnikHandle voh;
  VodaHandle vh = vod_vyrob(1);

  mh = kom_pridej_mesh_do_levelu("akvarko_voda.b2m");
  vod_pridej_mesh(vh, mh);
  vod_uzavri_meshe(vh);
  
  voh = vod_vloz_vodnika(vh,&p, VOD_AKTIVNI | VOD_Z_PLANE |  VOD_DIFF_ADD | VOD_SPEC_ADD, 0.1f, 50, 1, 0, 0, 200);
    
  vod_vloz_vodnika_spec(voh, 0.1f, 0.1f, 0.1f);
  vod_vloz_vodnika_add_spec(voh, 0, 0, 0);
  
  vod_vloz_vodnika_diff(voh, 0.05f, 0.05f, 0.05f, 0.125f);
  vod_vloz_vodnika_add_diff(voh, 0, 0, 0, 0);

  p.x = 100;
  p.z = 100;

  voh = vod_vloz_vodnika(vh,&p, VOD_AKTIVNI | VOD_X_PLANE |  VOD_DIFF_ADD | VOD_SPEC_ADD, 0.1f, 50, 1, 0, 0, 200);

  voh = vod_vloz_vodnika(vh,&p, VOD_AKTIVNI | VOD_BOD |  VOD_DIFF_ADD | VOD_SPEC_ADD, 0.1f, 50, 1, 0, 0, 200);
}
