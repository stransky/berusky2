//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Object.h"
#include "game_logic.h"
#include "animation.h" 
#include "animend_functions.h"

typedef struct
{
	SIM_ANIMATION	animation[32];
	int				last;
} ANIMATION_QUEUE_SET_;

extern AUDIO_DATA ad;
extern int		  cameraflag;
extern LEVELINFO  Level;
extern ANIMATION_QUEUE_SET_ *p_set_anim;
extern int gl_Get_Column_Height(int x, int y, int z, LEVELINFO *p_Level);

void anmend_Play_PadBedny(ITEMDESC	*p_itm, LEVELINFO *p_Level)
{
	float pos[3];
	int	  rot;

	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &rot);

	rot = rand()%3;
//	ap_Play_Sound(0,0,0, pos, rot + 49, NULL, &ad);

	gl_Do_Prach(pos, p_Level);
}

void anmend_Play_Sound_PadBerusky(float *pos, int material)
{
	switch(material)
	{
	case 9:
//		ap_Play_Sound(0,0,0, pos, 58, NULL, &ad);
		break;
	default:
//		ap_Play_Sound(0,0,0, pos, 57, NULL, &ad);
		break;
	}
}

void anmend_Play_PadBerusky(ITEMDESC *p_itm, LEVELINFO *p_Level)
{
	float pos[3];
	int	  rot;
	int ipos[3];

	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &rot);

	memcpy((void *) ipos, (void *) p_itm->Pos, 3 * sizeof(int));

	ipos[2]--;

	gl_Logical2Real(ipos[0], ipos[1], ipos[2], (long *)&rot, p_Level);

	if(p_Level->Level[rot])
		anmend_Play_Sound_PadBerusky(pos, p_Level->Level[rot]->p_Object->Material);
	else
		{
			ipos[2]--;

			if(ipos[2] >= 0)
			{
				gl_Logical2Real(ipos[0], ipos[1], ipos[2], (long *)&rot, p_Level);

				if(p_Level->Level[rot])
					anmend_Play_Sound_PadBerusky(pos, p_Level->Level[rot]->p_Object->Material);
			}
			else
				anmend_Play_Sound_PadBerusky(pos, -1);
		}
}

void anmend_PadBedny(int param, int param2, void *p_param)
{
	LEVELINFO	*p_Level = (LEVELINFO *)param;
	ITEMDESC	*p_itm = (ITEMDESC *)p_param;

	if(p_itm->p_Object->Class != 1)
	{
		anmend_Play_PadBedny(p_itm, p_Level);
	}
	else
		anmend_Play_PadBerusky(p_itm, p_Level);
}

void anmend_Add_Beetle_Animation(int param, int param2, void *p_param)
{
	am_Add_Beetle_Animation(param, p_param, param2, 1);
}

void anmend_kom_mesh_set_mesh(int param, int param2, void *p_param)
{
	kom_mesh_set_mesh(param, param2, (int)p_param);
	lani_set(param, 0, 0, &Level.TrashFlag, 0, 0, 0);
}

int anmend_Find_Item(int Mesh, LEVELINFO  *p_Level)
{
	int i;

/*	kprintf(1,"anmend_Find_Item Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/
	
	for(i=0;i<p_Level->Count_Of_Items;i++)
		if(p_Level->Item[i].Index_Of_Game_Mesh == Mesh)
			return i;

	return -1;
}

void anmend_ExplozeBedny(int param, int param2, void *p_param)
{
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;
	ITEMDESC	*p_itm = p_Level->Level[param2];

	float pos[3], pos1[3];
	int	  ipos[3], vipos[3];
	int	  rot,i;
	long  s;
	FLEK_K *pFlek;
	char	cFlek[32];
	int		mat[2];
	char	bSvetlo = 1;

/*	kprintf(1,"anmend_ExplozeBedny Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/
	gl_i2FStr(rand()%11,cFlek);

	if(p_Level->Actual_Item == -1)
		return;

	memcpy(ipos, p_Level->Level[p_Level->Actual_Item]->Pos, 3*sizeof(int));

	gl_Change_Destonation(p_Level, ipos);
	
	gl_Logical2Real(ipos[0], ipos[1], ipos[2], &s, p_Level);
		
	if(!p_Level->Level[s])
	{
		ipos[2]-=2;
		gl_Logical2Real(ipos[0], ipos[1], ipos[2], &s, p_Level);

		if(!p_Level->Level[s])
			return;
	}
	
	kom_mesh_get_float(p_Level->Level[s]->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &rot);
	
	memcpy((void *)vipos, (void *) p_Level->Level[s]->Pos, 3*sizeof(int));

	if(p_Level->Square[s].bUnderWater)
		bSvetlo = 0;

	pFlek = &p_Level->Level[s]->Square.Flek;

	switch(p_Level->Level[s]->Rotation)
	{
	case 3:
		pFlek->nx[0] = 1;
		pFlek->nx[1] = 1;
		pFlek->nx[2] = 0;
		pFlek->nz[0] = 0;
		pFlek->nz[1] = 0;
		pFlek->nz[2] = 1;
		break;
	case 1:
		pFlek->nx[0] = -1;
		pFlek->nx[1] = 1;
		pFlek->nx[2] = 0;
		pFlek->nz[0] = 0;
		pFlek->nz[1] = 0;
		pFlek->nz[2] = -1;
		break;
	case 2:
		pFlek->nx[0] = 1;
		pFlek->nx[1] = 0;
		pFlek->nx[2] = 0;
		pFlek->nz[0] = 0;
		pFlek->nz[1] = 1;
		pFlek->nz[2] = 1;
		break;
	case 0:
		pFlek->nx[0] = -1;
		pFlek->nx[1] = 0;
		pFlek->nx[2] = 0;
		pFlek->nz[0] = 0;
		pFlek->nz[1] = 1;
		pFlek->nz[2] = -1;
		break;
	}

	pFlek->Flag = K_CHYBA;
	memcpy(pFlek->pos, pos, 3*sizeof(float));
	pFlek->Material = kom_najdi_material(cFlek);

	if(pFlek->Material == -1)
		kprintf(1,"Nelze najit material %s", cFlek);

	p_Level->Level[s]->Square.iUroven = 0;
	pFlek->dx = 1;
	pFlek->dy = 1;
	pFlek->Rotation = 0;
				
	if(pFlek->pFlek)
	{
		kom_flek_zrus(pFlek->pFlek);
		pFlek->pFlek = 0;
	}

	kom_flek_pridej(pFlek->Flag,(BOD *)&pFlek->pos,(BOD *)&pFlek->nx,
				    (BOD *)&pFlek->nz, pFlek->dx,pFlek->dy,pFlek->Rotation,
					p_Level->Level[s]->Square.iUroven,pFlek->Material);


	memcpy(ipos, p_itm->Pos, 3*sizeof(int));

	kom_mesh_get_float(param, &pos1[0], &pos1[1], &pos1[2], &rot);

	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &rot);

	i = p_itm->Index_Of_Game_Mesh;

	mat[0] = gl_Get_Mesh_Material(param);
	mat[1] = gl_Get_Mesh_Material(p_itm->Index_Of_Game_Mesh);

	
	rot = anmend_Find_Item(param, p_Level);

/*	kprintf(1,"anmend_Find_Item RETURN Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/

	if(rot != -1)
		if(p_Level->Item[rot].Square.Flek.pFlek)
		{
			kom_flek_zrus(p_Level->Item[rot].Square.Flek.pFlek);
			p_Level->Item[rot].Square.Flek.pFlek = 0;
		}

	kom_zrus_prvek(param);

	if(p_itm->Square.Flek.pFlek)
		kom_flek_zrus(p_itm->Square.Flek.pFlek);

	kom_zrus_prvek(p_itm->Index_Of_Game_Mesh);

	gl_Do_Strepiny(pos1,p_Level, mat[0],0);
	gl_Do_Strepiny(pos,p_Level, mat[1], bSvetlo);

	if(!bSvetlo)
		am_Do_Vybuch_Bublin(vipos, pos, p_Level);

	gl_Destroy_Item(param2,1,p_Level);
	gl_Throw_offAnim(ipos, p_Level);

  p_Level->Column[0] = ipos[0];
  p_Level->Column[1] = ipos[1];
  p_Level->Column[2] = ipos[2] + (gl_Get_Column_Height(ipos[0], ipos[1], ipos[2]-1, p_Level) - 2) * 2;

  if(p_Level->Column[2] < 0)
     p_Level->Column[2] = 0;
  
  p_Level->bColumn++;
  p_Level->bCheckDependencesNeeded = 1;

  p_Level->Flip++;
	gl_Next_Queue_Set(p_Level);
	rot = rand()%5;

//	ap_Play_Sound(0,0,0, pos, rot + 14, NULL, &ad);
}

void anmend_ExplozeBednyZaSikminou(int param, int param2, void *p_param)
{
  int lc;
	POINTERSTRUCTURE	*pStruct = (POINTERSTRUCTURE *)p_param;
	ANIMATION_QUEUE_SET_ *pSet = (ANIMATION_QUEUE_SET_ *)pStruct->pParam;

/*	kprintf(1,"anmend_ExplozeBednyZaSikminou Act_Item = %d, -> %d\n",
			pStruct->p_Level->Actual_Item,
			pStruct->p_Level->Level[pStruct->p_Level->Actual_Item]);*/
	// znic vybusnimu
	gl_Destroy_Item(param,4,pStruct->p_Level);
	gl_Throw_offAnim(pStruct->viParam1,pStruct->p_Level);

  pStruct->p_Level->Column[0] = pStruct->viParam1[0];
  pStruct->p_Level->Column[1] = pStruct->viParam1[1];
  pStruct->p_Level->Column[2] = pStruct->viParam1[2] + (gl_Get_Column_Height(pStruct->viParam1[0], pStruct->viParam1[1], pStruct->viParam1[2]-1, pStruct->p_Level) - 2) * 2;

  if(pStruct->p_Level->Column[2] < 0)
      lc = 0;
   else
      lc = pStruct->p_Level->Column[2];

  pStruct->p_Level->bColumn++;

	// znic bednu
	gl_Destroy_Item(param2,3,pStruct->p_Level);
	gl_Throw_offAnim(pStruct->viParam2,pStruct->p_Level);

  pStruct->p_Level->Column[2] = lc;

  pStruct->p_Level->SecColumn[0] = pStruct->viParam2[0];
	pStruct->p_Level->SecColumn[1] = pStruct->viParam2[1];
	pStruct->p_Level->SecColumn[2] = pStruct->viParam2[2] + (gl_Get_Column_Height(pStruct->viParam2[0], pStruct->viParam2[1], pStruct->viParam2[2]-1, pStruct->p_Level) - 2) * 2;
	pStruct->p_Level->bSecColumn++;

  pStruct->p_Level->bCheckDependencesNeeded = 1;
	free((void *)pStruct);
}

int anmend_Find_Lift_PSystem(int mesh, LEVELINFO  *p_Level)
{
	int i;

	for(i=0;i<10;i++)
		if(p_Level->LiftParticles[i].System != -1 &&
		   p_Level->LiftParticles[i].hHnizdo[1] == mesh)
			return i;

	return -1;
}

void anmend_Lift(int param, int param2, void *p_param)
{
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;
	ITEMDESC	*p_itm = p_Level->Level[param2];
	float pos[3];
	int i,s;

	if(p_Level->Actual_Item == -1)
		return;

	am_Add_Beetle_Animation(p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, p_Level,
							p_Level->Level[p_Level->Actual_Item]->Rotation, 1);
	
	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &i);

	if(param > 0)
		gl_Do_Lift_Particles(pos, p_itm->Index_Of_Game_Mesh, p_Level);

//	s = ap_Play_Sound(0,0,0, pos, p_itm->p_Object->Specific[0].Index, NULL, &ad);

	if(s >= 0)
	{

		for(i=0;i<(abs(param)-1);i++)
			adas_Queue_Sound(s, p_itm->p_Object->Specific[1].Index);

		adas_Queue_Sound(s, p_itm->p_Object->Specific[2].Index);
	}

	if(param > 0)
	{
		s = anmend_Find_Lift_PSystem(p_itm->Index_Of_Game_Mesh, p_Level);

		if(s < 0)
			return;

		p_Level->LiftParticles[s].dwStop = abs(param) * 250;
	}
}

void animend_Lift_End(int param, int param2, void *p_param)
{
	kom_rozvaz_mesh(param);
}

void anmend_Play_Sunuti(int param, int param2, void *p_param)
{
	float pos[3];
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;

	if(p_Level->Actual_Item == -1)
		return;

	kom_get_fyz_souradnice(p_Level->Level[p_Level->Actual_Item]->Pos[0],
						   p_Level->Level[p_Level->Actual_Item]->Pos[1],
						   p_Level->Level[p_Level->Actual_Item]->Pos[2],
						   (BOD *) pos);

	switch(param)
	{
	case 10:	//snih
//		ap_Play_Sound(0,0,0, pos, 123, NULL, &ad);
		break;
	case 9://zelezo
//		ap_Play_Sound(0,0,0, pos, 124, NULL, &ad);
		break;
	case 8://drevo
//		ap_Play_Sound(0,0,0, pos, 127, NULL, &ad);	
		break;
	case 5://kamen
//		ap_Play_Sound(0,0,0, pos, 125, NULL, &ad);
		break;
	default:
//		ap_Play_Sound(0,0,0, pos, 125, NULL, &ad);
		break;
	}
}

void PlayStepsA(float *pos, char bQueue, int Num, int iWave, char bStoneSound)
{
	int s,i;

//	s = ap_Play_Sound(0,0,0, pos, iWave, NULL, &ad);

	if(bStoneSound && s != -1)
		adas_Queue_Sound(s, 164);

	if(bQueue && s != -1)
		for(i=0;i<Num;i++)
			adas_Queue_Sound(s, iWave);
}

int GetStep(LEVELINFO  *p_Level)
{
	switch(p_Level->iFloorMaterial)
	{
	case 12:	//pisek
		am_Zero_Step(p_Level);
		return 161;		
		break;
	case 11:	//trava
		am_Zero_Step(p_Level);
		return 149;		
		break;
	case 10:	//snih
		p_Level->bSetStep = 1;
		return 120 + (rand()%2);
		break;
	case 9://zelezo
		am_Zero_Step(p_Level);
		return 36;
		break;
	case 8://drevo
		am_Zero_Step(p_Level);
		return 35;
		break;
	case 5://kamen
		am_Zero_Step(p_Level);
		return 119;
	default:
		return 35;
		break;
	}
}

void PlaySteps(LEVELINFO  *p_Level, OBJECTDESC *pObj, float *pos, char bQueue, int Num, char bSunuti, char bESunuti)
{
//	int s,i;

	if(pObj->Class == 12)
	{
		int rnd = rand()%4;

//		ap_Play_Sound(0,0,0, pos, rnd+77, NULL, &ad);
		return;
	}

	switch(pObj->Material)
	{
	case 10:	//snih
		//am_Set_Step(p_Level);
		p_Level->bSetStep = 1;
		PlayStepsA(pos, bQueue, Num, 120 + (rand()%2), p_Level->bStoneSound);

		if(bESunuti == 1)
			PlayStepsA(pos, bQueue, 0, 126, p_Level->bStoneSound);
		else
		if(bESunuti == 2)
			PlayStepsA(pos, bQueue, 0, 122, p_Level->bStoneSound);
		else
		if(bSunuti)
			PlayStepsA(pos, bQueue, Num, 123, p_Level->bStoneSound);
		break;
	case 9://zelezo
		am_Zero_Step(p_Level);
		PlayStepsA(pos, bQueue, Num, 36, p_Level->bStoneSound);

		if(bESunuti == 1)
			PlayStepsA(pos, bQueue, 0, 126, p_Level->bStoneSound);
		else		
		if(bESunuti == 2)
			PlayStepsA(pos, bQueue, 0, 122, p_Level->bStoneSound);
		else
		if(bSunuti)
			PlayStepsA(pos, bQueue, Num, 124, p_Level->bStoneSound);
		break;
	case 8://drevo
		am_Zero_Step(p_Level);
		PlayStepsA(pos, bQueue, Num, 35, p_Level->bStoneSound);

		if(bESunuti == 1)
			PlayStepsA(pos, bQueue, 0, 126, p_Level->bStoneSound);
		else
		if(bESunuti == 2)
			PlayStepsA(pos, bQueue, 0, 122, p_Level->bStoneSound);
		else
		if(bSunuti)
			PlayStepsA(pos, bQueue, Num, 127, p_Level->bStoneSound);		
		break;
	case 5://kamen
		am_Zero_Step(p_Level);
		PlayStepsA(pos, bQueue, Num, 119, p_Level->bStoneSound);

		if(bESunuti == 1)
			PlayStepsA(pos, bQueue, 0, 126, p_Level->bStoneSound);
		else		
		if(bESunuti == 2)
			PlayStepsA(pos, bQueue, 0, 122, p_Level->bStoneSound);
		else
		if(bSunuti)
			PlayStepsA(pos, bQueue, Num, 125, p_Level->bStoneSound);
		break;
	default:
		am_Zero_Step(p_Level);
		PlayStepsA(pos, bQueue, Num, 35, p_Level->bStoneSound);

		if(bESunuti == 1)
			PlayStepsA(pos, bQueue, 0, 126, p_Level->bStoneSound);
		else
		if(bESunuti == 2)
			PlayStepsA(pos, bQueue, 0, 122, p_Level->bStoneSound);
		else
		if(bSunuti)
			PlayStepsA(pos, bQueue, Num, 125, p_Level->bStoneSound);
		break;
	}

	p_Level->bStoneSound = 0;
}

int anmend_Is_On_Water(long item, LEVELINFO  *p_Level)
{
	int pos[3];
	long r;

	if(p_Level->Level[item])
	{

		memcpy((void *) pos, (void *) p_Level->Level[item]->Pos, 3*sizeof(int));

		pos[2]--;

		gl_Logical2Real(pos[0], pos[1], pos[2], &r, p_Level);

		if(p_Level->Level[r])
			if(p_Level->Level[r]->p_Object->Class == 12)
				return 1;

	}

	return 0;
}

void anmend_Steps(int param, int param2, void *p_param)
{
	POINTERSTRUCTURE	*pStruct = (POINTERSTRUCTURE *)p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	int   p[3], r, s, i, kanim;
	float pos[3],fr,fi,vzdal;
	char bSunuti, bESunuti;
	char bOnWater = 0;


	p[0] = param;
	p[1] = param2;
	p[2] = pStruct->iParam;

	bSunuti = pStruct->viParam1[2];
	bESunuti = pStruct->viParam2[0];

	if(anmend_Is_On_Water(pStruct->viParam1[0], p_Level))
	{
		kom_mesh_get_float(p_Level->Level[pStruct->viParam1[0]]->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &r);
		
		if(p_Level->Level[pStruct->viParam1[0]]->p_Object->Class != 1)
			am_Kola_na_VodeB(pos, 0, p_Level->Level[pStruct->viParam1[0]]->Index_Of_Game_Mesh, 0, p_Level);
		else
			am_Kola_na_VodeB(pos, 1, p_Level->Level[pStruct->viParam1[0]]->Index_Of_Game_Mesh, 0, p_Level);

		bOnWater = 1;
	}

/*   if(p_set_anim->last && Level.bPosouvatKameru && 
	  !(p_Level->bB1ExtraCameraRot && p_Level->bPohled_Berusky))
		rani_privaz_kameru(p_set_anim->animation[p_set_anim->last-1].p_run);*/
	
	kam_pol_get((BOD *)pos, &fr, &fi, &vzdal);
	
	if(p_Level->bB1ExtraCameraRot && p_Level->bPohled_Berusky)
	{
		kom_get_fyz_souradnice(p_Level->Level[pStruct->viParam1[0]]->Pos[0],
							   p_Level->Level[pStruct->viParam1[0]]->Pos[2],
							   p_Level->Level[pStruct->viParam1[0]]->Pos[1],
							   (BOD *)pos);
		switch(p_Level->Level[pStruct->viParam1[0]]->Rotation)
		{
		case 0:
				kam_pol_anim((BOD *)pos, 0, fi, vzdal, &p_Level->TrashFlag, GK_REMOVE, 11, 0);
			break;
		case 1:
				kam_pol_anim((BOD *)pos, -90, fi, vzdal, &p_Level->TrashFlag, GK_REMOVE, 11, 0);
			break;
		case 2:
				kam_pol_anim((BOD *)pos, 180, fi, vzdal, &p_Level->TrashFlag, GK_REMOVE, 11, 0);
			break;
		case 3:
				kam_pol_anim((BOD *)pos, 90, fi, vzdal, &p_Level->TrashFlag, GK_REMOVE, 11, 0);
			break;
		}
	}

	switch(pStruct->bRCamera)
	{
	case 1:
		fr -= 90;
		kam_pol_anim((BOD *)pos, fr, fi, vzdal, &p_Level->TrashFlag, GK_REMOVE, 11, 0);
		break;
	case 2:
		fr += 90;
		kam_pol_anim((BOD *)pos, fr, fi, vzdal, &p_Level->TrashFlag, GK_REMOVE, 11, 0);
		break;
	case 3:
		//kam_pol_anim((BOD *)pos, fr, fi, vzdal, &p_Level->TrashFlag, 0, 22, 0);
		kanim = kam_pol_vyrob_animaci(22,0,3,0,1);

		if(kanim != K_CHYBA)
		{
			kam_pol_vloz_klic_rotace(kanim, 0, fr, fi, 0);
			kam_pol_vloz_klic_rotace(kanim, 1, fr + 90, fi, 10);
			kam_pol_vloz_klic_rotace(kanim, 2, fr + 180, fi, 21);
			kam_start(kanim, &p_Level->TrashFlag, GK_REMOVE, 0, 0);
		}

		fr += 180;
		break;
	}
	
	kom_get_fyz_souradnice(p[0], p[2], p[1], (BOD *) pos);

	p[2]--;
	gl_Logical2Real(p[0], p[1], p[2], &r, p_Level);
		
	if(p_Level->Level[r])
		PlaySteps(p_Level, p_Level->Level[r]->p_Object, pos, pStruct->bParam,pStruct->bParam - pStruct->viParam1[1], 
				  bSunuti, bESunuti);
	else
	{
		p[2]--;
		if(p[2]>=0)
		{
			gl_Logical2Real(p[0], p[1], p[2], &r, p_Level);

			if(p_Level->Level[r])
				PlaySteps(p_Level, p_Level->Level[r]->p_Object, pos, pStruct->bParam,pStruct->bParam - pStruct->viParam1[1], 
							bSunuti, bESunuti);
			else
			{
				p[2]--;
				if(p[2]>=0)
				{
					gl_Logical2Real(p[0], p[1], p[2], &r, p_Level);
	
					if(p_Level->Level[r])
						PlaySteps(p_Level, p_Level->Level[r]->p_Object, pos, pStruct->bParam,pStruct->bParam - pStruct->viParam1[1], 
									bSunuti, bESunuti);
				}
			}
		}
		else
		{
			int idx = GetStep(p_Level);
//			s = ap_Play_Sound(0,0,0, pos, idx, NULL, &ad);

			if(p_Level->bStoneSound && s != -1)
				adas_Queue_Sound(s, 164);

			if(pStruct->bParam && s != -1)
				for(i=0;i<(pStruct->bParam - pStruct->viParam1[1]);i++)
					adas_Queue_Sound(s, idx);
		}
	}

	p_Level->bStoneSound = 0;
	
	if(bOnWater || p_Level->Square[pStruct->viParam1[0]].bUnderWater)
		p_Level->bSetStep = 0;

	free((void *)pStruct);
}

void anmend_Steps2(int param, int param2, void *p_param)
{
//	ANIMATION_QUEUE_SET_	*p_aset;
	POINTERSTRUCTURE	*pStruct = (POINTERSTRUCTURE *)p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	int   p[3], r, s;
	float pos[3];

/*	kprintf(1,"anmend_Steps2 Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/
	p[0] = param;
	p[1] = param2;
	p[2] = pStruct->iParam;

 /*  p_aset = gl_Get_Prev_Queue_Set(p_Level);

   if(p_aset->last && Level.bPosouvatKameru)
		rani_privaz_kameru(p_aset->animation[p_aset->last-1].p_run);*/

/*	if(p_set_anim->last && Level.bPosouvatKameru)
		rani_privaz_kameru(p_set_anim->animation[p_set_anim->last-1].p_run);*/

	kom_get_fyz_souradnice(p[0], p[2], p[1], (BOD *) pos);

	p[2]--;
	gl_Logical2Real(p[0], p[1], p[2], &r, p_Level);
		
	if(p_Level->Level[r])
		PlaySteps(p_Level, p_Level->Level[r]->p_Object, pos, pStruct->bParam,1, 0, 0);
	else
	{
		p[2]--;
		if(p[2]>=0)
		{
			gl_Logical2Real(p[0], p[1], p[2], &r, p_Level);

			if(p_Level->Level[r])
				PlaySteps(p_Level, p_Level->Level[r]->p_Object, pos, pStruct->bParam,1, 0, 0);
		}
		else
		{
//			s = ap_Play_Sound(0,0,0, pos, 35, NULL, &ad);
			if(pStruct->bParam && s != -1)
				adas_Queue_Sound(s, 35);
		}
	}

	free((void *)pStruct);
}

void anmend_StepsSikmina(int param, int param2, void *p_param)
{
//	ANIMATION_QUEUE_SET_	*p_aset;
	POINTERSTRUCTURE	*pStruct = (POINTERSTRUCTURE *)p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	int   p[3], r, s, i;
	float pos[3];

/*	kprintf(1,"anmend_StepsSikmina Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/

	p[0] = param;
	p[1] = param2;
	p[2] = pStruct->iParam;

	if(anmend_Is_On_Water(pStruct->viParam1[0], p_Level))
	{
		kom_mesh_get_float(p_Level->Level[pStruct->viParam1[0]]->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &r);
		
		if(p_Level->Level[pStruct->viParam1[0]]->p_Object->Class != 1)
			am_Kola_na_VodeB(pos, 0, p_Level->Level[pStruct->viParam1[0]]->Index_Of_Game_Mesh, 0, p_Level);
		else
			am_Kola_na_VodeB(pos, 1, p_Level->Level[pStruct->viParam1[0]]->Index_Of_Game_Mesh, 0, p_Level);
	}
	
	kom_get_fyz_souradnice(p[0], p[2], p[1], (BOD *) pos);

	p[2]--;
	gl_Logical2Real(p[0], p[1], p[2], &r, p_Level);
		
	if(p_Level->Level[r])
		PlaySteps(p_Level, p_Level->Level[r]->p_Object, pos, pStruct->bParam,pStruct->bParam, 0, 0);
	else
	{
		p[2]--;
		if(p[2]>=0)
		{
			gl_Logical2Real(p[0], p[1], p[2], &r, p_Level);

			if(p_Level->Level[r])
				PlaySteps(p_Level, p_Level->Level[r]->p_Object, pos, pStruct->bParam,pStruct->bParam, 0, 0);
		}
		else
		{
//			s = ap_Play_Sound(0,0,0, pos, 35, NULL, &ad);

			if(p_Level->bStoneSound && s != -1)
				adas_Queue_Sound(s, 164);

			if(pStruct->bParam && s != -1)
				for(i=0;i<(pStruct->bParam);i++)
					adas_Queue_Sound(s, 35);
		}
	}

	if(p_Level->Actual_Item != -1)
		kom_flek_setflag(p_Level->Level[p_Level->Actual_Item]->Square.Flek.pFlek, 1);

	p_Level->bStoneSound = 0;

	free((void *)pStruct);
}

void anmend_Water(int param, int param2, void *p_param)
{
	float fvpos[3];
	int i;
	POINTERSTRUCTURE	*pStruct = (POINTERSTRUCTURE *)p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	ITEMDESC	*p_itm = p_Level->Level[param2];
	
	int rnd = rand()%3;
	int rndp = rand()%2;

	float pos[3] = {(float)p_itm->Pos[0]*2, 
					(float)p_itm->Pos[1]*2, 
					(float)p_itm->Pos[2]*2};

	kom_get_fyz_souradnice(pStruct->viParam1[0], pStruct->viParam1[2], 
						   pStruct->viParam1[1], (BOD *) fvpos);

	am_Kola_na_Vode(fvpos, pStruct->iParam, pStruct->bParam, pStruct->viParam2[0], p_Level);
	am_Do_Vodni_Cakanec1(fvpos, pStruct->bParam, pStruct->viParam2[0], p_Level);

	switch(param)
	{
	case 0:
//		ap_Play_Sound(0,0,0, pos, rnd+37, NULL, &ad);	
//		ap_Play_Sound(0,0,0, pos, rnd+40+rndp, NULL, &ad);	
		break;
	case 1:
//		ap_Play_Sound(0,0,0, pos, rnd+37, NULL, &ad);	
		break;
	case 2:
//		ap_Play_Sound(0,0,0, pos, rnd+40+rndp, NULL, &ad);	
		break;
	}

	if(pStruct->viParam2[1])
		for(i=0;i<10;i++)
			if(p_Level->LiftVParticles[i].hHnizdo[1] == pStruct->viParam2[1])
			{
				par_zrus(p_Level->LiftVParticles[i].System);
				p_Level->LiftVParticles[i].System = -1;			
			}

	if(pStruct->viParam2[2])
		free((void *)pStruct);
}

void anmend_Cakanec(int param, int param2, void *p_param)
{
	float fvpos[3];
	POINTERSTRUCTURE	*pStruct = (POINTERSTRUCTURE *)p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	ITEMDESC	*p_itm = p_Level->Level[param2];
	
	int rnd = rand()%3;
	
	float pos[3] = {(float)p_itm->Pos[0]*2, 
					(float)p_itm->Pos[1]*2, 
					(float)p_itm->Pos[2]*2};

	kom_get_fyz_souradnice(pStruct->viParam1[0], pStruct->viParam1[2], 
						   pStruct->viParam1[1], (BOD *) fvpos);

	am_Do_Vodni_Cakanec2(fvpos, pStruct->bParam, pStruct->viParam2[0], p_Level);

	free((void *)pStruct);
}

void anmend_WaterLift(int param, int param2, void *p_param)
{
	int i;
	float fvpos[3];
	int rnd = rand()%3;

	POINTERSTRUCTURE	*pStruct = (POINTERSTRUCTURE *)p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	
	kom_get_fyz_souradnice(pStruct->viParam1[0], pStruct->viParam1[2], 
						   pStruct->viParam1[1], (BOD *) fvpos);

	i = am_Kola_na_Vode(fvpos, pStruct->iParam, pStruct->bParam, pStruct->viParam2[0], p_Level);
	am_Do_Vodni_Cakanec1(fvpos, pStruct->bParam, pStruct->viParam2[0], p_Level);
	
	//kom_mesh_get_float(param, &fvpos[0], &fvpos[1], &fvpos[2], &r);

	am_Do_Lift_VParticles(fvpos, param, p_Level);

/*	if(i != -1)
		p_Level->VodniKola[i].hHnizdo[1] = 1;*/

//	ap_Play_Sound(0,0,0, fvpos, rnd+74, NULL, &ad);

	free((void *)pStruct);
}

void anmend_Item_Fall(int param, int param2, void *p_param)
{
	int rot;
	float pos[3];

	LEVELINFO  *p_Level = (LEVELINFO *)p_param;
	ITEMDESC	*p_itm = p_Level->Level[param2];
	
/*	kprintf(1,"anmend_Item_Fall Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/
	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, &pos[0], &pos[1], &pos[2], &rot);

	rot = rand()%2;
	switch(p_itm->p_Object->SubClass)
	{
	case 0:
//		ap_Play_Sound(0,0,0, pos, 48, NULL, &ad);	
		break;
	case 1:
//		ap_Play_Sound(0,0,0, pos, 44, NULL, &ad);	
		break;
	case 4:
//		ap_Play_Sound(0,0,0, pos, 45, NULL, &ad);	
		break;
	case 7:
//		ap_Play_Sound(0,0,0, pos, 47, NULL, &ad);	
		break;
	case 8:
//		ap_Play_Sound(0,0,0, pos, 46, NULL, &ad);	
		break;
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
//		ap_Play_Sound(0,0,0, pos, 100+rot, NULL, &ad);	
		break;
	}

	gl_Pripoj_Flek_k_Predmenu(p_itm, p_Level);
}

void anmend_Item_FallStartAnim(int param, int param2, void *p_param)
{
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;
	
/*	kprintf(1,"anmend_Item_FallStartAnim Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/
	p_Level->Level[param2]->a_flag = -1;
	am_Add_Animate_Item(param2, p_Level);

	p_Level->bCheck_Result = gl_Check_Logic_Dependences(p_Level);
	p_Level->bCheck = 1;

	if(p_Level->bCheck_Result)
	{
			p_Level->Flip++;
			gl_Next_Queue_Set(p_Level);
			if(p_Level->Flip == 1)
				gl_Go_Animations();
	}
}

void anmend_Send_Event(int param, int param2, void *p_param)
{
//	kprintf(1,"anmend_Send_Event \n");
	kom_amat_event(param);
}

void anmend_Set_Flek_Flag(int param, int param2, void *p_param)
{
//	kprintf(1,"anmend_Set_Flek_Flag \n");
	kom_flek_setflag(param, param2);
}

void anmend_Set_Flek_Flag_Anim(int param, int param2, void *p_param)
{
	LEVELINFO  *p_Level = (LEVELINFO *)&Level;

//	kprintf(1,"anmend_Set_Flek_Flag \n");
	kom_flek_setflag(param, param2);

	if(p_Level->Actual_Item == -1)
		return;

	am_Add_Beetle_Animation(p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, &Level, 
							p_Level->Level[p_Level->Actual_Item]->Rotation, 0);
}

void anmend_Take_Item(int param, int param2, void *p_param)
{
	POINTERSTRUCTURE *pStruct = (POINTERSTRUCTURE *)p_param;
	float pos[3];
	int rot;

/*	kprintf(1,"anmend_Take_Item Act_Item = %d, -> %d\n",
			pStruct->p_Level->Actual_Item,
			pStruct->p_Level->Level[pStruct->p_Level->Actual_Item]);*/

	kom_mesh_get_float(param, &pos[0], &pos[1], &pos[2], &rot);
/*
	if(param2 == 3 || param2 == 5)
		ap_Play_Sound(0,0,0, pos, 53, NULL, &ad);
	else
		ap_Play_Sound(0,0,0, pos, 52, NULL, &ad);
*/
	if(pStruct->p_Level->Item[pStruct->iParam].Square.Flek.pFlek)
	{
		kom_flek_zrus(pStruct->p_Level->Item[pStruct->iParam].Square.Flek.pFlek);
		pStruct->p_Level->Item[pStruct->iParam].Square.Flek.pFlek = 0;
	}

	if(pStruct->p_Level->Item[pStruct->iParam].hSvetlo != -1)
	{
		sdl_anim_zrus(pStruct->p_Level->Item[pStruct->iParam].hSvetlo);
		sdl_svetlo_zrus(pStruct->p_Level->Item[pStruct->iParam].hSvetlo);
		pStruct->p_Level->Item[pStruct->iParam].hSvetlo = -1;
	}

	am_Remove_Animate_itemB(pStruct->iParam, pStruct->p_Level);
	kom_zrus_prvek(param);
	free((void *)pStruct);
}

void anmend_Kamen(int param, int param2, void *p_param)
{
	float fpos[3];
	int	  pos[3];
	int rot;
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;
	ITEMDESC	*p_itm = p_Level->Level[param];
	int mat = gl_Get_Mesh_Material(p_itm->Index_Of_Game_Mesh);
				
/*	kprintf(1,"anmend_Kamen Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/
	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, &fpos[0],
					   &fpos[1], &fpos[2], &rot);

	memcpy((void *)pos, (void *)p_itm->Pos, 3*sizeof(int));

	rot = rand()%3;

	if(p_itm->p_Object->Specific[rot].Index == -1)
		rot = 0;

	//if(p_itm->p_Object->Specific[rot].Index != -1)
		//ap_Play_Sound(0,0,0, fpos, p_itm->p_Object->Specific[rot].Index, NULL, &ad);								

	
	gl_Destroy_Item(param,0,p_Level);
	gl_Do_Kameni(fpos, p_Level, mat);

	mat = kom_najdi_material("cvytah1");
	gl_Do_Krompac(fpos, p_Level, mat);

	gl_Throw_off(pos,p_Level);
}

void anmend_ZrusCastice(int param, int param2, void *p_param)
{
	SYSTEMZHAVYCHCASTIC *pSystem = (SYSTEMZHAVYCHCASTIC *)param2;

//	kprintf(1,"anmend_ZrusCastice Act_Item = %d, -> %d\n");

	free(p_param);
	if(param != -1)
		sdl_svetlo_zrus(param);

	if(pSystem)
		am_Release_Zhave_castice(pSystem);
}

void anmend_ZrusCastice2(int param, int param2, void *p_param)
{
	SYSTEMZHAVYCHCASTIC *pSystem = (SYSTEMZHAVYCHCASTIC *)param2;
	SYSTEMKOUROVYCHCASTIC *pSystemK = (SYSTEMKOUROVYCHCASTIC *)param;

	free(p_param);

	if(pSystem)
		am_Release_Zhave_castice(pSystem);

	if(pSystemK)
		am_Release_Kourove_Castice(pSystemK);
}

void anmend_ZrusCastice3(int param, int param2, void *p_param)
{
	free(p_param);
}

void anmend_Are_Animations_Done(int param, int param2, void *p_param)
{
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;
/*	kprintf(1,"anmend_Are_Animations_Done Act_Item = %d, -> %d\n",
			p_Level->Actual_Item,
			p_Level->Level[p_Level->Actual_Item]);*/

	gl_Are_Animations_Done(p_Level);
}

void anmend_Tlacitko(int param, int param2, void *p_param)
{
	float	pos[3];
	int		rot;
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;
	ITEMDESC	*p_itm = (ITEMDESC *)param;

	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, &pos[0],
					   &pos[1], &pos[2], &rot);

	rot = rand()%3;

	if(p_itm->p_Object->Specific[rot].Index == -1)
		rot = 0;

	if(p_itm->p_Object->Specific[rot].Index == -1)
		return;

//	ap_Play_Sound(0,0,0, pos, p_itm->p_Object->Specific[rot].Index, NULL, &ad);
}

void anmend_Teleport(int param, int param2, void *p_param)
{
	float	pos[3], target[3], r, fi, dist;
	int		rot;
	int		newr;
	POINTERSTRUCTURE *pStruct = (POINTERSTRUCTURE *) p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	ITEMDESC	*p_itm = (ITEMDESC *)param;
	ITEMDESC	*p_itmNEW;
	ITEMDESC	*p_tel = (ITEMDESC *)p_Level->Level[param2];

	if(p_Level->Actual_Item == -1)
		return;

	if(p_itm->p_Object->Class == 1)
		am_Remove_Beetle_Animation(p_itm->Index_Of_Game_Mesh, p_Level);

	pStruct->viParam1[0] = p_itm->Pos[0];
	pStruct->viParam1[1] = p_itm->Pos[1];
	pStruct->viParam1[2] = p_itm->Pos[2];
	
	gl_Logical2Real(pStruct->viParam2[0], pStruct->viParam2[1], pStruct->viParam2[2],
					&rot, p_Level);

	p_Level->Level[rot] = p_itm;
	p_itmNEW = p_Level->Level[rot];

	newr = rot;

	gl_Logical2Real(pStruct->viParam1[0], pStruct->viParam1[1], pStruct->viParam1[2],
					&rot, p_Level);

	if(rot == p_Level->Actual_Item)
		p_Level->Actual_Item = newr;

	p_Level->Level[rot] = 0;

	p_itmNEW->Pos[0] = pStruct->viParam2[0];
	p_itmNEW->Pos[1] = pStruct->viParam2[1];
	p_itmNEW->Pos[2] = pStruct->viParam2[2];

	p_itm = p_itmNEW;

	if(p_itm->Square.pExtFlek && p_itm->Square.pExtFlek->pFlek)
	{
		kom_flek_zrus(p_itm->Square.pExtFlek->pFlek);
		p_itm->Square.pExtFlek->pFlek = 0;
		p_itm->Square.pExtFlek = NULL;
	}

	if(p_itm->p_Object->Class == 13)
	{
		kom_flek_zrus(p_itm->Square.Flek.pFlek);
		p_itm->Square.Flek.pFlek = 0;
	}

	kprintf(1, "kom_umisti_prvek");
	kom_umisti_prvek(p_itm->Index_Of_Game_Mesh,p_itm->Pos[0], p_itm->Pos[2],
					 p_itm->Pos[1], p_itm->Rotation);

	kprintf(1, "kom_umisti_prvek DONE");

	if(p_itm->p_Object->Class == 13)
		gl_Pripoj_Flek_k_Predmenu(p_itm, p_Level);

	kom_mesh_get_float(p_itm->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &rot);

	if(p_itm->p_Object->Class == 1 && 
	   p_Level->Level[p_Level->Actual_Item]->iItem == p_itm->iItem &&
	   p_Level->bPosouvatKameru)
	{
		kam_pol_get((BOD *)&target, &r, &fi, &dist);	
	
		kam_pol_anim((BOD *)pos, r, fi, dist, &cameraflag,GK_REMOVE,10,1);

		kom_flek_setflag(p_itm->Square.Flek.pFlek, 0);
	}

	kom_mesh_get_float(p_tel->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &rot);

	gl_Do_Teleport_Sparks(pos, p_Level);

	gl_Throw_off(p_tel->Pos, p_Level);
	p_Level->Flip++;
	gl_Next_Queue_Set(p_Level);

	if(p_Level->Flip == 1)
		gl_Go_Animations();

	p_Level->Status = 5;
	free((void *) pStruct);

	if(p_itm->p_Object->Class == 1)
		am_Add_Beetle_Animation(p_itm->Index_Of_Game_Mesh, p_Level, p_itm->Rotation, 1);
}

void anmend_TeleportStart(int param, int param2, void *p_param)
{
	float	pos[3];
	int		rot;
	POINTERSTRUCTURE *pStruct = (POINTERSTRUCTURE *) p_param;
	LEVELINFO  *p_Level = pStruct->p_Level;
	ITEMDESC	*p_itm = (ITEMDESC *)param;
	ITEMDESC	*p_tel = (ITEMDESC *)p_Level->Level[param2];

	kom_mesh_get_float(p_tel->Index_Of_Game_Mesh, 
					   &pos[0], &pos[1], &pos[2], &rot);
	
	rot = gl_Do_Teleport_Flare(pos, p_Level);

	if(rot != -1)
	{
		p_Level->Flare[rot].mesh = p_itm->Index_Of_Game_Mesh;
		p_Level->Flare[rot].hSvetlo = gl_Do_Teleport_Light(pos, p_Level, rot);
		p_Level->Flare[rot].Teleport = -1;
		p_Level->Flare[rot].Item = NULL;
	}

	kom_get_fyz_souradnice(pStruct->viParam2[0],
						   pStruct->viParam2[2],
						   pStruct->viParam2[1],
						   (BOD *)pos);

	rot = gl_Do_Teleport_Flare(pos, p_Level);

	if(rot != -1)
	{
		p_Level->Flare[rot].hEXSvetlo = -1;
		p_Level->Flare[rot].hSvetlo = gl_Do_Teleport_Light(pos, p_Level, -1);
		p_Level->Flare[rot].Teleport = param2;
		if(p_itm->p_Object->Class == 13 || !p_Level->Flip)
			p_Level->Flare[rot].Item = p_itm;
		else
			p_Level->Flare[rot].Item = NULL;

		p_Level->Flare[rot].pPointer = (void *)pStruct;
	}

	rot = rand()%3;

	if(p_tel->p_Object->Specific[rot].Index == -1)
		rot = 0;

	if(p_tel->p_Object->Specific[rot].Index == -1)
		return;

//	ap_Play_Sound(0,0,0, pos, p_tel->p_Object->Specific[rot].Index, NULL, &ad);
}

void anmend_Exit(int param, int param2, void *p_param)
{
	LEVELINFO  *p_Level = (LEVELINFO *)p_param;

	am_Do_Exit(param, param2, p_Level);

//	lani_set(param, 0, K_CHYBA, &p_Level->TrashFlag,0,0,0);
//	kom_zrus_prvek(param);
}

void anmend_Lift_Item(int param, int param2, void *p_param)
{
	kom_svaz_meshe(param, param2);
}

void anmend_KamenZaSikmonou(int param, int param2, void *p_param)
{
	int mat = kom_najdi_material("cvytah1");

	POINTERSTRUCTURE *pStruct = (POINTERSTRUCTURE *) p_param;

	//rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_Kamen, Ldest, 0, (void *)p_Level);
						
	/*rani_pripoj_funkci(p_set->animation[p_set->last].p_run, anmend_kom_mesh_set_mesh, 
					     p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh, 0, (void *)p_Level->Level[p_Level->Actual_Item]->Rotation);*/
	
	anmend_Kamen(pStruct->viParam1[0], pStruct->viParam1[1], pStruct->p_Level);
	anmend_kom_mesh_set_mesh(pStruct->viParam2[0], pStruct->viParam2[1], (void *)pStruct->viParam2[2]);

	free((void *)pStruct);
}

void anmend_PlayKrumpac(int param, int param2, void *p_param)
{
}

void anmend_kom_mesh_set_meshK(int param, int param2, void *p_param)
{
	float pos[3];
	int rot;

	kom_mesh_get_float(param, &pos[0], &pos[1], &pos[2], &rot);
	
//	ap_Play_Sound(0,0,0, pos, 162 + (rand()%2), NULL, &ad);
	kom_mesh_set_mesh(param, param2, (int)p_param);
	lani_set(param, 0, 0, &Level.TrashFlag, 0, 0, 0);
}
