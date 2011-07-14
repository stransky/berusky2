//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "3d_all.h"
#include "Demo.h"
#include "Bind.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky_universal.h"
#include "profiles.h"

extern PLAYER_PROFILE pPlayerProfile;
extern int iActualLevel;
extern int iActualScene;
extern int iLanguageVersion;

void demo_Set_Scene_Level(char *cDemo, int *pScene, int *pLevel)
{
  char odir[MAX_PATH + 1];
  char dir[MAX_PATH + 1];
  FILE *file;
  DEMOFILEHEADER FileHeader;

  getcwd(odir, MAX_PATH);

  GetPrivateProfileString("game", "save_dir", "c:\\", dir, MAX_PATH,
    ini_file);
  chdir((dir));

  file = fopen(cDemo, "rb");

  if (!file) {
    chdir((odir));
    return;
  }

  fread(&FileHeader, sizeof(DEMOFILEHEADER), 1, file);
  fclose(file);

  (*pScene) = FileHeader.iScene;
  (*pLevel) = FileHeader.iLevel;

  chdir((odir));
}

int demo_Check_Owner(WCHAR * wPlayer, char *cDemo, WCHAR * wDemoName)
{
/*
	WCHAR	wc[128];
	char	text[256];

	DEMOFILEHEADER	FileHeader;
	FILE	*file;

	file = fopen(cDemo,"rb");

	if(!file)
		return 0;

	fread(&FileHeader,sizeof(DEMOFILEHEADER),1,file);
	fclose(file);

	if(FileHeader.iDemoID != 123456789 ||
	   FileHeader.iHiVer != DEMO_HIVERSION ||
	   FileHeader.iLoVer != DEMO_LOVERSION)
		return 0;

	strcpy(text,"ANAKREON_DEMO_ANAKREON");

	MultiByteToWideChar( CP_ACP, 0, text, strlen(text)+1, wc, sizeof(wc)/sizeof(wc[0]));


	if(!wcscmp(wc, FileHeader.cPlayerName))
	{
		wcscpy(wDemoName, FileHeader.cFileName);
		return 1;
	}

	if(wcscmp(wPlayer, FileHeader.cPlayerName))
		return 0;
	else
	{
		if(iLanguageVersion == 4)
		{
			sprintf(text, "scene%d", FileHeader.iScene);
		
			if(!GetPrivateProfileInt("Internet", text, 0, ini_file))
				return 0;
		}

		wcscpy(wDemoName, FileHeader.cFileName);
		return 1;
	}
  */
  return 0;
}

//------------------------------------------------------------------------------------------------
// init demo saving
//------------------------------------------------------------------------------------------------
DEMOKEYFRAME *demo_Init(DEMOSTRUCTURE * p_Demo)
{
  p_Demo->Frame_Counter = 0;

  p_Demo->p_First = (DEMOKEYFRAME *) malloc(sizeof(DEMOKEYFRAME));
  if (!p_Demo->p_First)
    return NULL;
  else {
    p_Demo->p_Last = p_Demo->p_First;
    p_Demo->p_First->p_Next = NULL;
    return p_Demo->p_First;
  }
}

//------------------------------------------------------------------------------------------------
// Release demo key frame line
//------------------------------------------------------------------------------------------------
void demo_Release(DEMOSTRUCTURE * p_Demo)
{
  DEMOKEYFRAME *p_Frame, *p_Next;

  p_Frame = p_Demo->p_First;
  if (!p_Frame)
    return;

  p_Next = p_Demo->p_First->p_Next;
  while (p_Frame) {
    kprintf(1, "Release Demo Frame Vkey = %d", p_Frame->Key);

    free((void *) p_Frame);
    p_Frame = p_Next;

    if (p_Frame)
      p_Next = p_Frame->p_Next;
  }
}

//------------------------------------------------------------------------------------------------
// add new demo key frame into the line
//------------------------------------------------------------------------------------------------
DEMOKEYFRAME *demo_Create_Frame(DEMOSTRUCTURE * p_Demo)
{
  p_Demo->p_Last->p_Next = NULL;

  p_Demo->p_Last->p_Next = (DEMOKEYFRAME *) malloc(sizeof(DEMOKEYFRAME));
  if (!p_Demo->p_Last->p_Next)
    return NULL;
  else {
    p_Demo->Frame_Counter++;
    p_Demo->p_Last = p_Demo->p_Last->p_Next;
    p_Demo->p_Last->p_Next = NULL;
    p_Demo->Start = timeGetTime();
    return p_Demo->p_Last;
  }
}

//------------------------------------------------------------------------------------------------
// count eplaset time in millisecods
//------------------------------------------------------------------------------------------------
DWORD demo_Eplased_Time(DWORD Start, DWORD Finish)
{
  DWORD epl = Finish - Start;

  return epl;
}

//------------------------------------------------------------------------------------------------
// save demo
//------------------------------------------------------------------------------------------------
int demo_Save(DEMOSTRUCTURE * p_Demo, char *p_File_Name, char Ovladani,
  char *cLevelName)
{
  DEMOFILEHEADER FileHeader;
  FILE *file;
  int i;
  DEMOKEYFRAME *p_Frame;


  file = fopen(p_File_Name, "wb");

  if (!file)
    return 0;

  ZeroMemory(&FileHeader, sizeof(FileHeader));

  FileHeader.iHiVer = DEMO_HIVERSION;
  FileHeader.iLoVer = DEMO_LOVERSION;
  FileHeader.bOvladaniBerusek1 = Ovladani;
  //wcscpy(FileHeader.cPlayerName, pPlayerProfile.cName);
  strcpy(FileHeader.cLevelName, cLevelName);
  FileHeader.iDemoID = 123456789;
  FileHeader.iLevel = iActualLevel;
  FileHeader.iScene = iActualScene;

  fwrite(&FileHeader, sizeof(DEMOFILEHEADER), 1, file);

  fwrite(p_Demo, sizeof(DEMOSTRUCTURE), 1, file);

  p_Frame = p_Demo->p_First;

  for (i = 0; i < p_Demo->Frame_Counter; i++) {
    fwrite(p_Frame, sizeof(DEMOKEYFRAME), 1, file);
    p_Frame = p_Frame->p_Next;
  }

  fclose(file);
  return 1;
}

int demo_Exist(WCHAR * wName, char *cFile)
{
/*
	DEMOFILEHEADER	FileHeader;
	FILE *file;
	long Done, error;
	struct _finddata_t	Data;

	Done = _findfirst("*.dem",&Data);
	error = Done;
			
	while(error != -1)
	{
		if(error != -1)
		{
			file = fopen(Data.name, "rb");

			if(file)
			{
				fread(&FileHeader, sizeof(DEMOFILEHEADER), 1, file);
				fclose(file);

				if(!wcscmp(FileHeader.cFileName, wName) && !wcscmp(FileHeader.cPlayerName, pPlayerProfile.cName))
				{
					strcpy(cFile, Data.name);
					_findclose(Done); 
					return 1;
				}
			}

			error = _findnext(Done,&Data);
		}
	}
	_findclose(Done); 
*/
  return 0;
}

int demo_SaveWC(DEMOSTRUCTURE * p_Demo, WCHAR * wcName, char Ovladani,
  char *cLevelName, char *cLoadSignature)
{
  char cText[1024];
  char cFile[1024];
  char pom[128];
  char pom2[128];

  DEMOFILEHEADER FileHeader;
  FILE *file;
  int i;
  DEMOKEYFRAME *p_Frame;

  //strdate(pom);

  for (i = 0; i < (signed) strlen(pom); i++)
    if (pom[i] == '/')
      pom[i] = '_';

  //strtime(pom2);

  for (i = 0; i < (signed) strlen(pom2); i++)
    if (pom2[i] == ':')
      pom2[i] = '_';

  sprintf(cFile, "demo_[%s]_[%s].dem", pom, pom2);

  GetPrivateProfileString("game", "save_dir", "c:\\", cText, 255, ini_file);
  working_file_translate(cText, 255);
  chdir((cText));

  //strcat(cText, "\\");
  //strcat(cText, cFile);

  demo_Exist(wcName, cFile);

  file = fopen(cFile, "wb");

  if (!file)
    return 0;

  ZeroMemory(&FileHeader, sizeof(FileHeader));

  FileHeader.iHiVer = DEMO_HIVERSION;
  FileHeader.iLoVer = DEMO_LOVERSION;
  FileHeader.bOvladaniBerusek1 = Ovladani;
  wcscpy(FileHeader.cPlayerName, pPlayerProfile.cName);
  wcscpy(FileHeader.cFileName, wcName);
  strcpy(FileHeader.cLevelName, cLevelName);
  FileHeader.iDemoID = 123456789;
  FileHeader.iLevel = iActualLevel;
  FileHeader.iScene = iActualScene;

  if (!strcmp(cLoadSignature, "LOAD_GAME"))
    FileHeader.iLoadedLevel = 1;
  else
    FileHeader.iLoadedLevel = 0;

  fwrite(&FileHeader, sizeof(DEMOFILEHEADER), 1, file);

  fwrite(p_Demo, sizeof(DEMOSTRUCTURE), 1, file);

  p_Frame = p_Demo->p_First;

  for (i = 0; i < p_Demo->Frame_Counter; i++) {
    fwrite(p_Frame, sizeof(DEMOKEYFRAME), 1, file);
    p_Frame = p_Frame->p_Next;
  }

  fclose(file);
  return 1;
}

//------------------------------------------------------------------------------------------------
// load demo
//------------------------------------------------------------------------------------------------
int demo_Load(DEMOSTRUCTURE * p_Demo, char *p_File_Name, char *bOvladani,
  char *cLevel_Name, char *cLoadedSignature)
{
  DEMOFILEHEADER FileHeader;

  FILE *file;
  int i;
  DEMOKEYFRAME *p_Frame;
  DEMOKEYFRAME *p_Last;


  file = fopen(p_File_Name, "rb");

  if (!file) {
    kprintf(1, "Demo file not found");
    return 0;
  }

  fread(&FileHeader, sizeof(DEMOFILEHEADER), 1, file);

  if (FileHeader.iDemoID != 123456789 ||
    FileHeader.iHiVer != DEMO_HIVERSION ||
    FileHeader.iLoVer != DEMO_LOVERSION) {
    kprintf(1, "Demo Version mismatch! - Expected: v%d.%d - File v%d.%d",
      DEMO_HIVERSION, DEMO_LOVERSION, FileHeader.iHiVer, FileHeader.iLoVer);
    fclose(file);
    return 0;
  }

  iActualLevel = FileHeader.iLevel;
  iActualScene = FileHeader.iScene;
  *bOvladani = FileHeader.bOvladaniBerusek1;
  strcpy(cLevel_Name, FileHeader.cLevelName);

  if (FileHeader.iLoadedLevel)
    strcpy(cLoadedSignature, "LOAD_GAME");

  fread(p_Demo, sizeof(DEMOSTRUCTURE), 1, file);

  p_Frame = (DEMOKEYFRAME *) malloc(sizeof(DEMOKEYFRAME));
  if (!p_Frame)
    return 0;

  fread(p_Frame, sizeof(DEMOKEYFRAME), 1, file);

  p_Demo->bOvladaniBerusek1 = FileHeader.bOvladaniBerusek1;
  p_Demo->p_First = p_Frame;
  p_Last = p_Demo->p_First;
  p_Last->p_Next = NULL;

  for (i = 0; i < p_Demo->Frame_Counter - 1; i++) {
    p_Frame = (DEMOKEYFRAME *) malloc(sizeof(DEMOKEYFRAME));
    if (!p_Frame)
      return 0;
    ZeroMemory(p_Frame, sizeof(DEMOKEYFRAME));

    fread(p_Frame, sizeof(DEMOKEYFRAME), 1, file);
    p_Last->p_Next = p_Frame;

    p_Last = p_Frame;
    p_Last->p_Next = NULL;
  }

  fclose(file);
  return 1;
}


int demo_Virual2Fuction(int iVirtualKey, CONTROL_KEYS * p_keys)
{
  if (iVirtualKey == p_keys->move_forward)
    return 1;

  if (iVirtualKey == p_keys->turn_back)
    return 2;

  if (iVirtualKey == p_keys->turn_left)
    return 3;

  if (iVirtualKey == p_keys->turn_right)
    return 4;

  if (iVirtualKey == p_keys->next_beatle)
    return 5;

  /*if(iVirtualKey == p_keys->inventory)
     return 6;

     if(iVirtualKey == p_keys->inventory_select)
     return 7;

     if(iVirtualKey == p_keys->inventory_left)
     return 8;

     if(iVirtualKey == p_keys->inventory_right)
     return 9;

     if(iVirtualKey == p_keys->inventory_cancel)
     return 10;

     if(iVirtualKey == p_keys->camera_move_left)
     return 11;

     if(iVirtualKey == p_keys->camera_move_right)
     return 12;

     if(iVirtualKey == p_keys->camera_move_forward)
     return 13;

     if(iVirtualKey == p_keys->camera_move_back)
     return 14; */

  if (iVirtualKey == p_keys->camera_zoom_in)
    return 15;

  if (iVirtualKey == p_keys->camera_zoom_out)
    return 16;

  /*if(iVirtualKey == p_keys->camera_turn_left)
     return 17;

     if(iVirtualKey == p_keys->camera_turn_right)
     return 18;

     if(iVirtualKey == p_keys->camera_turn_up)
     return 19;

     if(iVirtualKey == p_keys->camera_turn_down)
     return 20; */

  if (iVirtualKey == p_keys->camera_fast_turn_left)
    return 21;

  if (iVirtualKey == p_keys->camera_fast_turn_right)
    return 22;

  if (iVirtualKey == p_keys->camera_center)
    return 23;

  if (iVirtualKey == p_keys->menu)
    return 24;

  if (iVirtualKey == p_keys->item_lock)
    return 25;

  if (iVirtualKey == p_keys->blow_detonation_pack)
    return 26;

  if (iVirtualKey == p_keys->beatle1)
    return 27;

  if (iVirtualKey == p_keys->beatle2)
    return 28;

  if (iVirtualKey == p_keys->beatle3)
    return 29;

  if (iVirtualKey == p_keys->beatle4)
    return 30;

  if (iVirtualKey == p_keys->beatle5)
    return 31;

  if (iVirtualKey == p_keys->beatle6)
    return 32;

  /*if(iVirtualKey == p_keys->item1)
     return 33;

     if(iVirtualKey == p_keys->item2)
     return 34;

     if(iVirtualKey == p_keys->item3)
     return 35;

     if(iVirtualKey == p_keys->item4)
     return 36; */

  if (iVirtualKey == p_keys->next_song)
    return 37;

  if (iVirtualKey == p_keys->disable_inventory)
    return 38;

  if (iVirtualKey == p_keys->disable_top_ledge)
    return 39;

  if (iVirtualKey == p_keys->screenshot)
    return 40;

  if (iVirtualKey == p_keys->visibility)
    return 41;

  if (iVirtualKey == p_keys->hint)
    return 42;

  if (iVirtualKey == p_keys->restart)
    return 43;

  if (iVirtualKey == p_keys->highlight)
    return 44;

  if (iVirtualKey == p_keys->camera_move)
    return 45;

  if (iVirtualKey == p_keys->camera_rotation)
    return 46;

  if (iVirtualKey == p_keys->pause)
    return 47;

  return 0;
}

int demo_Fuction2Virtual(int iFunctionKey, CONTROL_KEYS * p_keys)
{
  switch (iFunctionKey) {
    case 1:
      return p_keys->move_forward;
    case 2:
      return p_keys->turn_back;
    case 3:
      return p_keys->turn_left;
    case 4:
      return p_keys->turn_right;
    case 5:
      return p_keys->next_beatle;
    case 6:
      return p_keys->inventory;
    case 7:
      return p_keys->inventory_select;
    case 8:
      return p_keys->inventory_left;
    case 9:
      return p_keys->inventory_right;
    case 10:
      return p_keys->inventory_cancel;
    case 11:
      return p_keys->camera_move_left;
    case 12:
      return p_keys->camera_move_right;
    case 13:
      return p_keys->camera_move_forward;
    case 14:
      return p_keys->camera_move_back;
    case 15:
      return p_keys->camera_zoom_in;
    case 16:
      return p_keys->camera_zoom_out;
    case 17:
      return p_keys->camera_turn_left;
    case 18:
      return p_keys->camera_turn_right;
    case 19:
      return p_keys->camera_turn_up;
    case 20:
      return p_keys->camera_turn_down;
    case 21:
      return p_keys->camera_fast_turn_left;
    case 22:
      return p_keys->camera_fast_turn_right;
    case 23:
      return p_keys->camera_center;
    case 24:
      return p_keys->menu;
    case 25:
      return p_keys->item_lock;
    case 26:
      return p_keys->blow_detonation_pack;
    case 27:
      return p_keys->beatle1;
    case 28:
      return p_keys->beatle2;
    case 29:
      return p_keys->beatle3;
    case 30:
      return p_keys->beatle4;
    case 31:
      return p_keys->beatle5;
    case 32:
      return p_keys->beatle6;
    case 33:
      return p_keys->item1;
    case 34:
      return p_keys->item2;
    case 35:
      return p_keys->item3;
    case 36:
      return p_keys->item4;
    case 37:
      return p_keys->next_song;
    case 38:
      return p_keys->disable_inventory;
    case 39:
      return p_keys->disable_top_ledge;
    case 40:
      return p_keys->screenshot;
    case 41:
      return p_keys->visibility;
    case 42:
      return p_keys->hint;
    case 43:
      return p_keys->restart;
    case 44:
      return p_keys->highlight;
    case 45:
      return p_keys->camera_move;
    case 46:
      return p_keys->camera_rotation;
    case 47:
      return p_keys->pause;
    default:
      return 0;
  }
}

int demo_Get_Move(int *pos_o, int *pos_n)
{
  int x = pos_o[0] - pos_n[0];
  int y = pos_o[1] - pos_n[1];

  if (x < 0)
    return 1;
  else if (x > 0)
    return 3;
  else if (y < 0)
    return 0;
  else
    return 2;
}

int demo_Rotation(int initRot, int finishRot)
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

  return rot;
}

int demo_Create_Sequence(int *pKeyLine, int iKLSize, DEMOKEYFRAME * pFrame,
  char *pBerusky1, int Rotation, CONTROL_KEYS * p_cont, DEMOSTRUCTURE * pDemo,
  int *pos_a)
{
  int i, c = 0;

  if (!pFrame)
    return 0;

  for (i = 0; i < 3; i++)
    if (pFrame->pos_n[i] != -1)
      c++;

  for (i = 0; i < 3; i++)
    if (pFrame->pos_o[i] != -1)
      c++;

  ZeroMemory(pKeyLine, iKLSize * sizeof(int));

  //nejedna se o pokus o pohyb
  if (!c) {
    pKeyLine[0] = demo_Fuction2Virtual(pFrame->Key, p_cont);
    pDemo->p_Last = pFrame->p_Next;
    return 1;
  }

  if (pFrame->bOvladaniBerusek1) {
    int rot =
      demo_Rotation(Rotation, demo_Get_Move(pFrame->pos_o, pFrame->pos_n));
    *pBerusky1 = 1;

    if (!rot)
      pKeyLine[0] = p_cont->move_forward;
    else if (rot == 1)
      pKeyLine[0] = p_cont->turn_right;
    else if (rot == 2)
      pKeyLine[0] = p_cont->turn_back;
    else
      pKeyLine[0] = p_cont->turn_left;

    pDemo->p_Last = pFrame->p_Next;
  }
  else {
    int move = demo_Get_Move(pFrame->pos_o, pFrame->pos_n);
    int rot = Rotation - move;

    *pBerusky1 = 0;

    if (!rot)
      pKeyLine[0] = p_cont->move_forward;
    else if (abs(rot) == 2) {
      pKeyLine[0] = p_cont->turn_back;
      pKeyLine[1] = p_cont->move_forward;
    }
    else if (Rotation - 1 == move || (!Rotation && move == 3)) {
      pKeyLine[0] = p_cont->turn_left;
      pKeyLine[1] = p_cont->move_forward;
    }
    else {
      pKeyLine[0] = p_cont->turn_right;
      pKeyLine[1] = p_cont->move_forward;
    }

    pDemo->p_Last = pFrame->p_Next;
  }

  return 1;
}
