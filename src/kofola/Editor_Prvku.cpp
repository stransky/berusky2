#include "..\\komat\\mss_on.h"
#include <windows.h>
#include <commctrl.h>
#include <io.h>
#include <direct.h>
//#include <d3d.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "..\\komat\\resource.h"
#include "..\\komat\\objekt_editor_all.h"
#include "Object.h"

#pragma comment(lib,"ComCtl32.lib")
#pragma comment(lib,"winmm.lib")

typedef struct
{
  TCHAR Name[30];
  INT Index;
} WAVEFILEDESC;

const NUMOFOBJECTS = 1000;
const NUMOFCLASSES = 20;

FILE *file;
WAVEFILEDESC *WaveFile;
OBJECTDESC *Object;
SECONDDATADESC *SecondData;
TCHAR Directory[256];
TCHAR Directory3[256];
int NumOfFiles, i, ActualItem;
char ActualName[30];


BOOL CALLBACK ko_MainDlgProc(HWND hDlg, UINT msg, WPARAM wParam,
  LPARAM lParam);
HRESULT ko_OnInitDialog(HWND hDlg);

int ko_GetLayer(int Guid)
{
  int i;

  i = Guid / 1000;

  if ((i == 3) ||
    (i == 4) ||
    (i == 8) ||
    (i == 9) || (i == 10) || (i == 11) || (i == 12) || (i == 15) || (i == 16))
    return 0;
  else
    return 1;
}

INT GetFreeObject()
{
  for (i = 0; i < NUMOFOBJECTS; i++)
    if (Object[i].Class == 0)
      return i;
  return i;
}

INT TranslateWave(char *pFileName)
{
  for (i = 0; i < NumOfFiles; i++)
    if (strcmp(WaveFile[i].Name, pFileName) == 0)
      return WaveFile[i].Index;
  return -1;
}

char *TranslateIndex(int FileIndex)
{
  for (i = 0; i < NumOfFiles; i++)
    if (WaveFile[i].Index == FileIndex)
      return WaveFile[i].Name;
  return NULL;
}

INT CountObjects()
{
  INT Count;

  Count = 0;
  for (i = 0; i < NUMOFOBJECTS; i++)
    if (Object[i].Class != 0)
      Count++;
  return Count;
}

BOOL CheckGuid(INT g)
{
  INT j;

  for (j = 0; j < NUMOFOBJECTS; j++)
    if ((Object[j].GUID == g) && (Object[j].Class != 0))
      return TRUE;
  return FALSE;
}

void NaplnListBox(HWND hWnd, int Class)
{
  int i;
  DWORD WordString;

  HWND hCombo = GetDlgItem(hWnd, IDC_LISTOFITEMS);

  SendMessage(hCombo, LB_RESETCONTENT, 0, 0);

  for (i = 0; i < NUMOFOBJECTS; i++)
    if ((Object[i].GUID >= Class) && (Object[i].GUID < (Class + 1000))) {
      WordString =
        SendMessage(hCombo, LB_ADDSTRING, 0,
        (LPARAM) (LPCTSTR) Object[i].Name);
      SendMessage(hCombo, LB_SETITEMDATA, WordString, i);
    }
}

OBJECTDESC *ko_Nahraj_herni_prvky(void)
{
  char FileName[256];
  char Text[256];
  long error;
  int Count;
  int m;
  struct _finddata_t Data;
  long Done;

  GetPrivateProfileString("soundengine", "sound_dir", "c:\\", Directory, 256,
    ini_file);
  GetPrivateProfileString("game", "prvky_dir", "c:\\", Directory3, 256,
    ini_file);

  WaveFile = (WAVEFILEDESC *) malloc(1000 * sizeof(WAVEFILEDESC));
  if (WaveFile == NULL)
    MessageBox(NULL, "Nelze alokovat pamet pro WAVEFILEDESC", "!!!", MB_OK);

  strcpy(FileName, Directory);
  strcat(FileName, "\\Index.dat");


  file = fopen(FileName, "r");
  if (file) {
    fgets(Text, 30, file);
    NumOfFiles = atoi(Text);
    for (i = 0; i < NumOfFiles; i++) {
      fgets(Text, 30, file);
      Text[strlen(Text) - 1] = '\0';
      strcpy(WaveFile[i].Name, Text);
      fgets(Text, 30, file);
      WaveFile[i].Index = atoi(Text);
    }
    fclose(file);
  }
  else
    MessageBox(NULL, "Index.dat nenalezen", "Nahraj herni prvky", MB_OK);

  Object = (OBJECTDESC *) malloc(NUMOFOBJECTS * sizeof(OBJECTDESC));
  if (Object == NULL)
    MessageBox(NULL, "Nelze alokovat pamet pro OBJECTDESC", "!!!", MB_OK);

  for (i = 0; i < NUMOFOBJECTS; i++)
    Object[i].Class = 0;

  SecondData =
    (SECONDDATADESC *) malloc(NUMOFOBJECTS * sizeof(SECONDDATADESC));
  if (SecondData == NULL)
    MessageBox(NULL, "Nelze alokovat pamet pro SECONDDATADESC", "!!!", MB_OK);

  file = 0;

  strcpy(FileName, Directory3);
  strcat(FileName, "\\*.itm");

  Count = 0;
  Done = _findfirst(FileName, &Data);
  error = Done;

  while (error != -1) {
    Count++;
    error = _findnext(Done, &Data);
  }

  _findclose(Done);

  Done = _findfirst(FileName, &Data);
  error = Done;
  m = 0;

  if (error != -1) {
    do {
      strcpy(FileName, Directory3);
      strcat(FileName, "\\");
      strcat(FileName, Data.name);
      file = fopen(FileName, "rb");
      if (file) {
        fread(Object + m, sizeof(OBJECTDESC), 1, file);
        fseek(file, 1000, SEEK_SET);
        fread(SecondData + m, sizeof(SECONDDATADESC), 1, file);
        fclose(file);
      }
      else
        MessageBox(NULL, "Soubor nenalezen", "Nahraj herni prvky", MB_OK);

      error = _findnext(Done, &Data);
      m++;
    }
    while (error != -1);
  }
  _findclose(Done);

  return Object;
}

void ko_Uvolni_herni_prvky(void)
{
  free((void *) Object);
  free((void *) WaveFile);
  free((void *) SecondData);
}
