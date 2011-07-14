#include "..\\komat\\mss_on.h"
#include <windows.h>
#include <commctrl.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include "..\\Komat\resource.h"
#include "Editor Levelu.h"

#pragma comment(lib,"ComCtl32.lib")

LEVEL_ENVIRONMENT *p_LevelEnvironment;

int iCountOfWeather, iCountOfNature, iIndex;

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT OnInitDialog(HWND hDlg);
HRESULT OnInitDialogWeather(HWND hDlg);
HRESULT OnInitDialogNature(HWND hDlg);

void kof_Init_Level_Environment(LEVEL_ENVIRONMENT * p_LevEnv)
{
  p_LevelEnvironment = p_LevEnv;
  ZeroMemory((void *) p_LevEnv, sizeof(LEVEL_ENVIRONMENT));
}

void kof_Set_Level_Environment(LEVEL_ENVIRONMENT * p_LevEnv, HINSTANCE hInst,
  HWND hWnd)
{
  p_LevelEnvironment = p_LevEnv;
  DialogBox(hInst, MAKEINTRESOURCE(IDD_LEVEL_ENVIRONMENT), hWnd, MainDlgProc);
}

BOOL CALLBACK MainDlgProcWeather(HWND hDlg, UINT msg, WPARAM wParam,
  LPARAM lParam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_HUSTOTA:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_HUSTOTA, text, 32);
            p_LevelEnvironment->Weather[iIndex].uiDensity = atoi(text);
          }
          break;
        case IDC_INTEZITA:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_INTEZITA, text, 32);
            p_LevelEnvironment->Weather[iIndex].uiIntensity = atoi(text);
          }
          break;
        case IDC_ROZPTYL:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_ROZPTYL, text, 32);
            p_LevelEnvironment->Weather[iIndex].fmaxDivergence =
              (float) atof(text);
          }
          break;
        case IDC_VZDALENOST:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_VZDALENOST, text, 32);
            p_LevelEnvironment->Weather[iIndex].fDist = (float) atof(text);
          }
          break;
        case IDC_SKY:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_SKY, text, 32);
            p_LevelEnvironment->Weather[iIndex].fSky = (float) atof(text);
          }
          break;
        case IDC_WEATHER:
          if (HIWORD(wParam) == CBN_SELCHANGE)
            p_LevelEnvironment->Weather[iIndex].iWeather =
              (char) SendMessage(GetDlgItem(hDlg, IDC_WEATHER), CB_GETCURSEL,
              0, 0);
          break;
        case IDCANCEL:
          EndDialog(hDlg, IDCANCEL);
          break;
        default:
          return FALSE;         // Neobslouzil message
      }
      break;
    case WM_NOTIFY:
      {
      }
      break;
    case WM_INITDIALOG:
      OnInitDialogWeather(hDlg);
      break;
    case WM_DESTROY:
      EndDialog(hDlg, IDCANCEL);
      break;
    default:
      return FALSE;             // Neobslouzil message
  }

  return TRUE;                  // Obslouzil message
}

int Find_Free_Weather(void)
{
  int i;

  for (i = 0; i < 3; i++)
    if (!p_LevelEnvironment->Weather[i].bUsed)
      return i;

  return -1;
}

HRESULT OnInitDialogWeather(HWND hDlg)
{
  char text[256];
  HWND hCombo;

  HINSTANCE hInst = (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE);
  HICON hIcon = LoadIcon(hInst, IDI_APPLICATION);

  hCombo = GetDlgItem(hDlg, IDC_WEATHER);
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Žádné");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Déš");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Sníh");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Pøízemní Mlha");
  SendMessage(hCombo, CB_SETCURSEL,
    p_LevelEnvironment->Weather[iIndex].iWeather, 0);

  SetDlgItemText(hDlg, IDC_HUSTOTA,
    itoa(p_LevelEnvironment->Weather[iIndex].uiDensity, text, 10));
  SetDlgItemText(hDlg, IDC_INTEZITA,
    itoa(p_LevelEnvironment->Weather[iIndex].uiIntensity, text, 10));

  sprintf(text, "%f", p_LevelEnvironment->Weather[iIndex].fmaxDivergence);
  SetDlgItemText(hDlg, IDC_ROZPTYL, text);
  sprintf(text, "%f", p_LevelEnvironment->Weather[iIndex].fDist);
  SetDlgItemText(hDlg, IDC_VZDALENOST, text);
  sprintf(text, "%f", p_LevelEnvironment->Weather[iIndex].fSky);
  SetDlgItemText(hDlg, IDC_SKY, text);

  return S_OK;
}

BOOL CALLBACK MainDlgProcNature(HWND hDlg, UINT msg, WPARAM wParam,
  LPARAM lParam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_HUSTOTA:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_HUSTOTA, text, 32);
            p_LevelEnvironment->NatureEffect[iIndex].uiDensity = atoi(text);
          }
          break;
        case IDC_INTEZITA:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_INTEZITA, text, 32);
            p_LevelEnvironment->NatureEffect[iIndex].uiIntensity = atoi(text);
          }
          break;
        case IDC_VZDALENOST:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_VZDALENOST, text, 32);
            p_LevelEnvironment->NatureEffect[iIndex].fRadius =
              (float) atof(text);
          }
          break;
        case IDC_SKY:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_SKY, text, 32);
            p_LevelEnvironment->NatureEffect[iIndex].fSky =
              (float) atof(text);
          }
          break;
        case IDC_PLANE_LOW:
          {
            char text[32];

            GetDlgItemText(hDlg, IDC_PLANE_LOW, text, 32);
            p_LevelEnvironment->NatureEffect[iIndex].fLowPlane =
              (float) atof(text);
          }
          break;
        case IDC_NATUREEFFECT:
          if (HIWORD(wParam) == CBN_SELCHANGE)
            p_LevelEnvironment->NatureEffect[iIndex].EffectID =
              (char) SendMessage(GetDlgItem(hDlg, IDC_NATUREEFFECT),
              CB_GETCURSEL, 0, 0);
          break;
        case IDCANCEL:
          EndDialog(hDlg, IDCANCEL);
          break;
        default:
          return FALSE;         // Neobslouzil message
      }
      break;
    case WM_NOTIFY:
      {
      }
      break;
    case WM_INITDIALOG:
      OnInitDialogNature(hDlg);
      break;
    case WM_DESTROY:
      EndDialog(hDlg, IDCANCEL);
      break;
    default:
      return FALSE;             // Neobslouzil message
  }

  return TRUE;                  // Obslouzil message
}

int Find_Free_Nature(void)
{
  int i;

  for (i = 0; i < 10; i++)
    if (!p_LevelEnvironment->NatureEffect[i].bUsed)
      return i;

  return -1;
}

HRESULT OnInitDialogNature(HWND hDlg)
{
  char text[256];
  HWND hCombo;

  HINSTANCE hInst = (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE);
  HICON hIcon = LoadIcon(hInst, IDI_APPLICATION);

  hCombo = GetDlgItem(hDlg, IDC_NATUREEFFECT);

  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Žádný");
  SendMessage(hCombo, CB_ADDSTRING, 0,
    (LPARAM) (LPCTSTR) "Chmíøí Pampelišek");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Chmíøí I");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Chmíøí II");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Muška I");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Muška II");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "List I");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "List II");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "List III");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "List IV");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "List V");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "List VI");
  SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Svìtluška");
  SendMessage(hCombo, CB_SETCURSEL,
    p_LevelEnvironment->NatureEffect[iIndex].EffectID, 0);

  SetDlgItemText(hDlg, IDC_HUSTOTA,
    itoa(p_LevelEnvironment->NatureEffect[iIndex].uiDensity, text, 10));
  SetDlgItemText(hDlg, IDC_INTEZITA,
    itoa(p_LevelEnvironment->NatureEffect[iIndex].uiIntensity, text, 10));

  sprintf(text, "%f", p_LevelEnvironment->NatureEffect[iIndex].fRadius);
  SetDlgItemText(hDlg, IDC_VZDALENOST, text);
  sprintf(text, "%f", p_LevelEnvironment->NatureEffect[iIndex].fSky);
  SetDlgItemText(hDlg, IDC_SKY, text);
  sprintf(text, "%f", p_LevelEnvironment->NatureEffect[iIndex].fLowPlane);
  SetDlgItemText(hDlg, IDC_PLANE_LOW, text);

  return S_OK;
}

void i2weather(int iIndex, char *cText)
{
  switch (iIndex) {
    case 0:
      strcpy(cText, "Žádné");
      break;
    case 1:
      strcpy(cText, "Déš");
      break;
    case 2:
      strcpy(cText, "Sníh");
      break;
    case 3:
      strcpy(cText, "Pøízemní Mlha");
      break;
  }
}

void i2nature(int iIndex, char *cText)
{
  switch (iIndex) {
    case 0:
      strcpy(cText, "Žádné");
      break;
    case 1:
      strcpy(cText, "Chmíøí Pampelišek");
      break;
    case 2:
      strcpy(cText, "Chmíøí I");
      break;
    case 3:
      strcpy(cText, "Chmíøí II");
      break;
    case 4:
      strcpy(cText, "Muška I");
      break;
    case 5:
      strcpy(cText, "Muška II");
      break;
    case 6:
      strcpy(cText, "List I");
      break;
    case 7:
      strcpy(cText, "List II");
      break;
    case 8:
      strcpy(cText, "List III");
      break;
    case 9:
      strcpy(cText, "List IV");
      break;
    case 10:
      strcpy(cText, "List V");
      break;
    case 11:
      strcpy(cText, "List VI");
      break;
    case 12:
      strcpy(cText, "Svìtluška");
      break;
  }
}

void WDelete(HWND hDlg)
{
  HWND hCombo;
  DWORD Select;

  hCombo = GetDlgItem(hDlg, IDC_WEATHERLIST);
  Select = SendMessage(hCombo, LB_GETCURSEL, 0, 0);
  iIndex = SendMessage(hCombo, LB_GETITEMDATA, Select, 0);

  if (iIndex > -1) {
    SendMessage(hCombo, LB_DELETESTRING, Select, 0);
    p_LevelEnvironment->Weather[iIndex].bUsed = 0;
    iCountOfWeather--;
    PostMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP1), PBM_SETPOS,
      (LONG) iCountOfWeather, 0);
  }
}

void WEdit(HWND hDlg)
{
  char text[256];
  HWND hCombo;
  DWORD Select;

  hCombo = GetDlgItem(hDlg, IDC_WEATHERLIST);
  Select = SendMessage(hCombo, LB_GETCURSEL, 0, 0);
  iIndex = SendMessage(hCombo, LB_GETITEMDATA, Select, 0);

  if (iIndex > -1) {
    SendMessage(hCombo, LB_DELETESTRING, Select, (LPARAM) (LPCTSTR) text);

    DialogBox((HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE),
      MAKEINTRESOURCE(IDD_WEATHER), hDlg, MainDlgProcWeather);

    i2weather(p_LevelEnvironment->Weather[iIndex].iWeather, text);

    Select = SendMessage(hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text);
    SendMessage(hCombo, LB_SETITEMDATA, Select, iIndex);
  }
}

void WAdd(HWND hDlg)
{
  char text[256];
  DWORD WordString;
  HWND hCombo;

  iIndex = Find_Free_Weather();

  if (iIndex > -1) {
    DialogBox((HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE),
      MAKEINTRESOURCE(IDD_WEATHER), hDlg, MainDlgProcWeather);
    p_LevelEnvironment->Weather[iIndex].bUsed = 1;
    iCountOfWeather++;
    PostMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP1), PBM_SETPOS,
      (LONG) iCountOfWeather, 0);

    hCombo = GetDlgItem(hDlg, IDC_WEATHERLIST);

    i2weather(p_LevelEnvironment->Weather[iIndex].iWeather, text);

    WordString =
      SendMessage(hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text);
    SendMessage(hCombo, LB_SETITEMDATA, WordString, iIndex);
  }
}

void NEDelete(HWND hDlg)
{
  HWND hCombo;
  DWORD Select;

  hCombo = GetDlgItem(hDlg, IDC_NATURELIST);
  Select = SendMessage(hCombo, LB_GETCURSEL, 0, 0);
  iIndex = SendMessage(hCombo, LB_GETITEMDATA, Select, 0);

  if (iIndex > -1) {
    SendMessage(hCombo, LB_DELETESTRING, Select, 0);
    p_LevelEnvironment->NatureEffect[iIndex].bUsed = 0;
    iCountOfNature--;
    PostMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP2), PBM_SETPOS,
      (LONG) iCountOfNature, 0);
  }
}

void NEEdit(HWND hDlg)
{
  char text[256];
  HWND hCombo;
  DWORD Select;

  hCombo = GetDlgItem(hDlg, IDC_NATURELIST);
  Select = SendMessage(hCombo, LB_GETCURSEL, 0, 0);
  iIndex = SendMessage(hCombo, LB_GETITEMDATA, Select, 0);

  if (iIndex > -1) {
    SendMessage(hCombo, LB_DELETESTRING, Select, (LPARAM) (LPCTSTR) text);

    DialogBox((HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE),
      MAKEINTRESOURCE(IDD_NATUREEFFECT), hDlg, MainDlgProcNature);

    i2nature(p_LevelEnvironment->NatureEffect[iIndex].EffectID, text);

    Select = SendMessage(hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text);
    SendMessage(hCombo, LB_SETITEMDATA, Select, iIndex);
  }
}

void NEAdd(HWND hDlg)
{
  char text[256];
  DWORD WordString;
  HWND hCombo;

  iIndex = Find_Free_Nature();

  if (iIndex > -1) {
    DialogBox((HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE),
      MAKEINTRESOURCE(IDD_NATUREEFFECT), hDlg, MainDlgProcNature);
    p_LevelEnvironment->NatureEffect[iIndex].bUsed = 1;
    iCountOfNature++;
    PostMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP2), PBM_SETPOS,
      (LONG) iCountOfNature, 0);

    hCombo = GetDlgItem(hDlg, IDC_NATURELIST);

    i2nature(p_LevelEnvironment->NatureEffect[iIndex].EffectID, text);

    WordString =
      SendMessage(hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text);
    SendMessage(hCombo, LB_SETITEMDATA, WordString, iIndex);
  }
}

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_WDELETE:
          WDelete(hDlg);
          break;
        case IDC_WEATHERLIST:
          if (HIWORD(wParam) == LBN_DBLCLK)
            WEdit(hDlg);
          break;
        case IDC_WEATHER:
          WAdd(hDlg);
          break;
        case IDC_NATURE:
          NEAdd(hDlg);
          break;
        case IDC_NDELETE:
          NEDelete(hDlg);
          break;
        case IDC_NATURELIST:
          if (HIWORD(wParam) == LBN_DBLCLK)
            NEEdit(hDlg);
          break;
        case IDC_ANIMACE_KAMERY:
          GetDlgItemText(hDlg, IDC_ANIMACE_KAMERY, p_LevelEnvironment->cCam,
            32);
          break;
        case IDC_ENVIRONMENT:
          GetDlgItemText(hDlg, IDC_ENVIRONMENT, p_LevelEnvironment->cEnv, 32);
          break;
        case IDC_ANIMATEANDEFFECT:
          GetDlgItemText(hDlg, IDC_ANIMATEANDEFFECT,
            p_LevelEnvironment->cAnimEff, 32);
          break;
        case IDCANCEL:
          EndDialog(hDlg, IDCANCEL);
          break;
        default:
          return FALSE;         // Neobslouzil message
      }
      break;
    case WM_NOTIFY:
      {
      }
      break;
    case WM_INITDIALOG:
      OnInitDialog(hDlg);
      break;
    case WM_DESTROY:
      EndDialog(hDlg, IDCANCEL);
      break;
    default:
      return FALSE;             // Neobslouzil message
  }

  return TRUE;                  // Obslouzil message
}

HRESULT OnInitDialog(HWND hDlg)
{
  int i;
  char game_datadir[256], sound_dir[256], text[256];
  long Done, error;
  struct _finddata_t Data;
  HWND hCombo;
  long WordString;

  HINSTANCE hInst = (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE);
  HICON hIcon = LoadIcon(hInst, IDI_APPLICATION);

  iCountOfWeather = 0;
  iCountOfNature = 0;

  GetPrivateProfileString("game", "game_data_dir", "c:\\", game_datadir, 256,
    ini_file);
  GetPrivateProfileString("soundengine", "sound_dir", "c:\\", sound_dir, 256,
    ini_file);

  PostMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM) hIcon);
  PostMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);

  hCombo = GetDlgItem(hDlg, IDC_ANIMACE_KAMERY);
  _chdir(game_datadir);
  Done = _findfirst("*.sik", &Data);
  error = Done;

  while (error != -1) {
    if (error != -1) {
      SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) Data.name);
      error = _findnext(Done, &Data);
    }
  }
  _findclose(Done);

  SetDlgItemText(hDlg, IDC_ANIMACE_KAMERY, p_LevelEnvironment->cCam);

  hCombo = GetDlgItem(hDlg, IDC_ENVIRONMENT);
  _chdir(sound_dir);
  Done = _findfirst("*.env", &Data);
  error = Done;

  while (error != -1) {
    if (error != -1) {
      SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) Data.name);
      error = _findnext(Done, &Data);
    }
  }
  _findclose(Done);

  SetDlgItemText(hDlg, IDC_ENVIRONMENT, p_LevelEnvironment->cEnv);

  hCombo = GetDlgItem(hDlg, IDC_ANIMATEANDEFFECT);
  _chdir(game_datadir);
  Done = _findfirst("*.lsc", &Data);
  error = Done;

  while (error != -1) {
    if (error != -1) {
      SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) Data.name);
      error = _findnext(Done, &Data);
    }
  }
  _findclose(Done);

  SetDlgItemText(hDlg, IDC_ANIMATEANDEFFECT, p_LevelEnvironment->cAnimEff);

  hCombo = GetDlgItem(hDlg, IDC_WEATHERLIST);

  for (i = 0; i < 3; i++)
    if (p_LevelEnvironment->Weather[i].bUsed) {
      i2weather(p_LevelEnvironment->Weather[i].iWeather, text);

      WordString =
        SendMessage(hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text);
      SendMessage(hCombo, LB_SETITEMDATA, WordString, i);
    }

  hCombo = GetDlgItem(hDlg, IDC_NATURELIST);

  for (i = 0; i < 10; i++)
    if (p_LevelEnvironment->NatureEffect[i].bUsed) {
      i2nature(p_LevelEnvironment->NatureEffect[i].EffectID, text);

      WordString =
        SendMessage(hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text);
      SendMessage(hCombo, LB_SETITEMDATA, WordString, i);
    }

  SendMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP1), PBM_SETRANGE, TRUE,
    MAKELONG(0, 3));
  SendMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP2), PBM_SETRANGE, TRUE,
    MAKELONG(0, 10));

  for (i = 0; i < 3; i++)
    if (p_LevelEnvironment->Weather[i].bUsed)
      iCountOfWeather++;

  PostMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP1), PBM_SETPOS,
    (LONG) iCountOfWeather, 0);

  for (i = 0; i < 10; i++)
    if (p_LevelEnvironment->NatureEffect[i].bUsed)
      iCountOfNature++;

  PostMessage(GetDlgItem(hDlg, IDD_LEVEL_ENVIRONMENTP2), PBM_SETPOS,
    (LONG) iCountOfNature, 0);

  return S_OK;
}
