#include<sys/time.h>

#include "3d_all.h"
#include "ini.h"

void Sleep(int ms)
{
}

int ftoi(float t)
{
  return((int)t);
}

static int timeGetTimeSet = 0;

void timeGetTimeInit(void)
{
  timeGetTimeSet = 1;
}

unsigned int timeGetTime(void)
{
  assert(timeGetTimeSet);
  return(current_time_get());
}

//-----------------------------------------------------------------------------
// Name: DisplayFrame()
// Desc: Blts a the sprites to the back buffer, then flips the 
//       back buffer onto the primary buffer.
//-----------------------------------------------------------------------------
int DisplayFrame()
{
}

int ddxInit(void)
{
}

void ddxRelease(void)
{
}

int ddxFindFreeSurface(void)
{
  return(0);
}

int ddxReleaseBitmap(int iSurface)
{
}

int ddxLoadList(char *pFileName, int bProgress)
{
}

int ddxLoadBitmap(char *pFileName, APAK_HANDLE *pHandle)
{
}

int ddxCreateSurface(int x, int y, int idx)
{
}

void ddxDrawSurfaceColorKey(int iSurface, int *com, int layer, COLORREF color)
{
}
void ddxDrawDisplayColorKey(int *com, int layer, COLORREF color)
{
}
void ddxDrawDisplay(int *com, int layer)
{
}
void ddxDrawSurface(int iSurface, int *com, int layer)
{
}

BOOL ddxTransparentBlt(
    int dcDestSurface,  // handle to Dest DC
    int nXOriginDest,   // x-coord of destination upper-left corner
    int nYOriginDest,   // y-coord of destination upper-left corner
    int nWidthDest,     // width of destination rectangle
    int nHeightDest,    // height of destination rectangle
    int dcSrcSurface,   // handle to source DC
    int nXOriginSrc,    // x-coord of source upper-left corner
    int nYOriginSrc,    // y-coord of source upper-left corner
    int nWidthSrc,      // width of source rectangle
    int nHeightSrc,     // height of source rectangle
    UINT crTransparent  // color to make transparent
  )
{
}

BOOL ddxTransparentBltDisplay(
    int nXOriginDest,   // x-coord of destination upper-left corner
    int nYOriginDest,   // y-coord of destination upper-left corner
    int nWidthDest,     // width of destination rectangle
    int nHeightDest,    // height of destination rectangle
    int dcSrcSurface,   // handle to source DC
    int nXOriginSrc,    // x-coord of source upper-left corner
    int nYOriginSrc,    // y-coord of source upper-left corner
    int nWidthSrc,      // width of source rectangle
    int nHeightSrc,     // height of source rectangle
    UINT crTransparent  // color to make transparent
  )
{
}

BOOL ddxBitBlt(
    int dcDestSurface,  // handle to Dest DC
    int nXOriginDest,   // x-coord of destination upper-left corner
    int nYOriginDest,   // y-coord of destination upper-left corner
    int nWidthDest,     // width of destination rectangle
    int nHeightDest,    // height of destination rectangle
    int dcSrcSurface,   // handle to source DC
    int nXOriginSrc,    // x-coord of source upper-left corner
    int nYOriginSrc     // y-coord of source upper-left corner
  )
{
}

BOOL ddxBitBltDisplay(
    int nXOriginDest,   // x-coord of destination upper-left corner
    int nYOriginDest,   // y-coord of destination upper-left corner
    int nWidthDest,     // width of destination rectangle
    int nHeightDest,    // height of destination rectangle
    int dcSrcSurface,   // handle to source DC
    int nXOriginSrc,    // x-coord of source upper-left corner
    int nYOriginSrc     // y-coord of source upper-left corner
  )
{
}

int ddxGetWidth(int iSurface)
{
}
int ddxGetHight(int iSurface)
{
}

void ddxCleareSurface(int iSurface)
{
}
void ddxCleareSurfaceColor(int iSurface, COLORREF color)
{
}
void ddxFillRect(int iSurface, RECT *rect, COLORREF color)
{
}
void ddxFillRectDisplay(RECT *rect, COLORREF color)
{
}
void ddxAddRectItem(void *p_rl, RECT rect, int iLayer)
{
}
int  ddxStretchBltDisplay(RECT *rDest, int iSurface, RECT *rSource)
{
}
int  ddxStretchBlt(int iSDest, RECT *rDest, int iSSource, RECT *rSource)
{
}

int ddxUpdateMouse(void)
{
}

void ddxSetFlip(char bSwitch)
{
}
void ddxSetCursor(char bSwitch)
{
}
void ddxResizeCursorBack(int iSurface)
{
}
void ddxSetCursorSurface(int iSurface)
{
}

BOOL ddxRestore(int *p_CompositDC, int *p_FontDC, int *p_BackDC, int *p_iCompositDC, int *p_iFontDC, int *p_iBackDC, 
        char *p_cBrutalRestart, AUDIO_DATA *p_ad)
{
}

void ddxSaveSurface(int idx)
{
}

void ddxCleareSurfaceColorDisplay(COLORREF color)
{
}

int ddxGetMode(void)
{
}

int ogg_playing(void)
{
}

char * strlwr(char *cFile)
{
  char *tmp = cFile;
  while(*tmp) {
    *tmp = tolower(*tmp);
    tmp++;
  }
  return(cFile);
}

void dbgprintf(char *p_tmp, ...)
{
}

int MultiByteToWideChar(
  int CodePage,
  int dwFlags,
  char * lpMultiByteStr,
  int cbMultiByte,
  WCHAR * lpWideCharStr,
  int cchWideChar
)
{  
  int ret = mbstowcs(lpWideCharStr, lpMultiByteStr, cchWideChar);
  assert(ret && ret < cchWideChar);
  return(ret);
}

int WideCharToMultiByte(
  int CodePage,
  int dwFlags,
  wchar_t * lpWideCharStr,
  int cchWideChar,
  char *lpMultiByteStr,
  int cbMultiByte,
  char * lpDefaultChar,
  int * lpUsedDefaultChar
)
{
  return(wcstombs(lpMultiByteStr, lpWideCharStr, cbMultiByte));
}

// Audio interface
// Init
void ap_Init(AUDIO_DATA *p_ad)
{
}

// Release
void ap_Release(AUDIO_DATA *p_ad)
{
}

// loades play list
int ap_Load_Play_List(char *p_File_Name, AUDIO_DATA *p_ad)
{
}

// Releases play list
void ap_Release_Play_List(AUDIO_DATA *p_ad)
{
}

// playes ogg song
int ap_Play_Song(int Index, char Random, AUDIO_DATA *p_ad)
{
}
int ap_Setup_and_Play_Song(int Index, char Random, AUDIO_DATA *p_ad)
{
}

// stops ogg song
void ap_Stop_Song(AUDIO_DATA *p_ad)
{
}

// Plays sound
int ap_Play_Sound(int Type, char Relative, char bAmbient, float *p_Pos, int Wave_Index, void *p_eax, AUDIO_DATA *p_ad)
{
}

// Releases materail list
void ap_Release_Material_List(AUDIO_DATA *p_ad)
{
}

// loades material list
int ap_Load_Material_List(char *p_File_Name, AUDIO_DATA *p_ad)
{
}

// count environment
void ap_Count_Environment(AUDIO_DATA *p_ad, void *p_Level)
{
}

// load environment
int ap_Load_Environment(char *p_Env_Name, void *p_Level, AUDIO_DATA *p_ad)
{
}

int ap_Load_Sound_List(AUDIO_DATA *p_ad, char *cFile, int iStart)
{
}

void ShowCursor(bool state)
{
}

void SetCursor(void *tmp)
{
}

char * strupr(char * string)
{
}

void GetPrivateProfileString(
  const char * lpAppName, // section name
  const char * lpKeyName, // key name
  const char * lpDefault, // default key value
  char * lpReturnedString, 
  int nSize,
  const char * lpFileName // ini file name
)
{
  // lpAppName -> unused
  ini_read_string(lpFileName, lpKeyName, lpReturnedString, nSize, lpDefault);
}

int WritePrivateProfileString(
  const char * lpAppName, // section name
  const char * lpKeyName,
  const char * lpString,
  const char * lpFileName
)
{

}

int GetPrivateProfileInt(
  const char * lpAppName, // section name
  const char * lpKeyName,
  int nDefault,
  const char * lpFileName
)
{
  // lpAppName -> unused
  return(ini_read_int(lpFileName, lpKeyName, nDefault));
}

void wchar_windows_to_linux(word *p_in, int bytes_in_len, wchar_t *p_out)
{
  int i;
  for(i = 0; i < bytes_in_len/2; i++)
    *p_out++ = *p_in++;
}

// in place replacement
wchar_t * wchar_windows_to_linux(word *p_in, int bytes_in_len)
{
  wchar_t *p_tmp = (wchar_t *)mmemcpy(p_in, bytes_in_len*2);
  wchar_windows_to_linux(p_in, bytes_in_len, p_tmp);
  return(p_tmp);
}

/* type definition for the "new" timer callback function */
Uint32 callback(Uint32 interval, void* param)
{
  TIMERPROC func = (TIMERPROC)param;
  func(NULL,0,0,interval);
  return(interval);
}

TIMER_ID SetTimer(HWND hWnd, TIMER_ID nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc)
{
  return(SDL_AddTimer(uElapse, callback, (void*)lpTimerFunc));
}

UINT KillTimer(HWND hWnd, TIMER_ID uIDEvent)
{
  return(SDL_RemoveTimer(uIDEvent));
}
