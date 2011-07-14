//------------------------------------------------------------------------------------------------
// version 0.0.1
//------------------------------------------------------------------------------------------------

#ifndef _DEMO_
#define _DEMO_

#include <time.h>
#include "Bind.h"
#include "Di.h"

#define DEMO_HIVERSION 1
#define DEMO_LOVERSION 1

typedef struct _DEMOKEYFRAME
{
  char Key;                     // key, that was pressed
  long Time;                    // time to next key
  struct _DEMOKEYFRAME *p_Next; // pointer to next DEMOKEYFRAME
  int bOvladaniBerusek1;
  int pos_o[3];
  int pos_n[3];
} DEMOKEYFRAME;

typedef struct
{
  DEMOKEYFRAME *p_First;        // pointer to first DEMOKEYFRAME
  DEMOKEYFRAME *p_Last;         // pointer to first DEMOKEYFRAME
  long Frame_Counter;           // stores number of created DEMOKEYFRAMEs+1
  DWORD Start;                  // start time
  DWORD Finish;                 // finist time
  SDL_TimerID Timer_ID;         // Timer ID for next frame
  int bOvladaniBerusek1;
  long dwTimeCounter;
} DEMOSTRUCTURE;

typedef struct
{
  int iHiVer;
  int iLoVer;
  WCHAR cPlayerName[32];
  WCHAR cFileName[64];
  int iDemoID;
  int bOvladaniBerusek1;
  char cLevelName[32];
  int iLoadedLevel;
  int iReserved[100];
  int iLevel;
  int iScene;
} DEMOFILEHEADER;

        // init demo saving
DEMOKEYFRAME *demo_Init(DEMOSTRUCTURE * p_Demo);

        // add new demo key frame into the line
DEMOKEYFRAME *demo_Create_Frame(DEMOSTRUCTURE * p_Demo);

        // Release demo key frame line
void demo_Release(DEMOSTRUCTURE * p_Demo);

        // count eplaset time in millisecods
DWORD demo_Eplased_Time(DWORD Start, DWORD Finish);

        // save demo
int demo_Save(DEMOSTRUCTURE * p_Demo, char *p_File_Name, char Ovladani,
  char *cLevelName);

        // load demo
int demo_Load(DEMOSTRUCTURE * p_Demo, char *p_File_Name, char *bOvladani,
  char *cLevel_Name, char *cLoadedSignature);

int demo_Virual2Fuction(int iVirtualKey, CONTROL_KEYS * p_keys);

int demo_Fuction2Virtual(int iFunctionKey, CONTROL_KEYS * p_keys);

int demo_Create_Sequence(int *pKeyLine, int iKLSize, DEMOKEYFRAME * pFrame,
  char *pBerusky1, int Rotation, CONTROL_KEYS * p_cont, DEMOSTRUCTURE * pDemo,
  int *pos_a);

int demo_Check_Owner(WCHAR * wPlayer, char *cDemo, WCHAR * wDemoName);

int demo_SaveWC(DEMOSTRUCTURE * p_Demo, WCHAR * wcName, char Ovladani,
  char *cLevelName, char *cLoadSignature);

int demo_Exist(WCHAR * wName, char *cFile);

void demo_Set_Scene_Level(char *cDemo, int *pScene, int *pLevel);

#endif
