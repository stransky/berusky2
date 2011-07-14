#include<sys/time.h>

#include "3d_all.h"
#include "ini.h"

#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Berusky3d_kamery.h"

#include "Berusky3d_kofola2d.h"

void Sleep(int ms)
{
  SDL_Delay(ms);
}

int ftoi(float t)
{
  return((int)roundf(t));  
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
// ddx2 interface
//-----------------------------------------------------------------------------
extern APAK_HANDLE		  *pBmpArchive;
char                    bmp_dir[MAX_PATH];
DeviceHandle            ddxDevice;
int                     i_CursorDDX = 0;
int                     ddxInitDone = FALSE;
int                     bFlip;
static long		          bLastGameState = 1;
extern int				      bWindowMenu;
extern MOUSE_INFO       dim;
int		start_x = 0;
int		start_y = 0;

//-----------------------------------------------------------------------------
// Name: DisplayFrame()
// Desc: Blts a the sprites to the back buffer, then flips the 
//       back buffer onto the primary buffer.
//-----------------------------------------------------------------------------
int DisplayFrame()
{
/*
  RECT r;
  HRESULT hr;
  int i_Act = g_pDisplay->GetActBuffer();
  
    // We are in fullscreen mode, so perform a flip and return 
    // any errors like DDERR_SURFACELOST
  r.left = 0;
  r.top = 0;
  r.right = ddxGetWidth(i_CursorDDX);
  r.bottom = ddxGetHight(i_CursorDDX);
  
  if(dim.rx + r.right >= 1024)
    r.right -= dim.rx + r.right - 1024;
  
  if(dim.y + r.bottom >= 768)
    r.bottom -= dim.ry + r.bottom - 768;
  
  ddxBitBlt(i_Cursor[i_Act], 0, 0, r.right, r.bottom, HDC2DD, 
        dim.rx, 
        dim.ry);
  
  if(bDrawCursor)
  {
    ddx.surface[i_CursorDDX].g_pSurface->SetColorKey( TRANSCOLOR );
  
    g_pDisplay->ColorKeyBlt(dim.rx, dim.ry, ddx.surface[i_CursorDDX].g_pSurface->GetDDrawSurface(), &r);
  }
  
  g_pDisplay->SetCursorX(i_Act, dim.rx);
  g_pDisplay->SetCursorY(i_Act, dim.ry);
  
    if( FAILED( hr = g_pDisplay->Present() ) )
        return hr;
  
  if(!g_pDisplay->IsWindowed())
  {
    g_pDisplay->AddBufferCounter();
    i_Act = g_pDisplay->GetActBuffer();
  }
  
  r.left = 0;
  r.top = 0;
  r.right = ddxGetWidth(i_CursorDDX);
  r.bottom = ddxGetHight(i_CursorDDX);
  
  if(g_pDisplay->GetCursorX(i_Act) + r.right >= 1024)
    r.right -= g_pDisplay->GetCursorX(i_Act) + r.right - 1024;
  
  if(g_pDisplay->GetCursorY(i_Act) + r.bottom >= 768)
    r.bottom -= g_pDisplay->GetCursorY(i_Act) + r.bottom - 768;
  
  
  g_pDisplay->Blt(g_pDisplay->GetCursorX(i_Act), g_pDisplay->GetCursorY(i_Act), ddx.surface[i_Cursor[i_Act]].g_pSurface, &r);
    
  return S_OK;
  */
  ddxPublish();
}

void ddxSetCursorSurface(int iSurface)
{
	i_CursorDDX = iSurface;
}

int ddxInit(void)
{  
  if(!ddxInitDone) {
    ddx2Init(10000, RGB(255,0,255));
  
    GetPrivateProfileString("game","bitmap_dir","c:\\",bmp_dir,MAX_PATH,(const char *) ini_file);
  
    ddxSetCursor(1);
    ddxSetCursorSurface(0);
    ddxSetFlip(1);
  
    ddxDevice = ddx2DeviceCreate(TRUE, 32);
    ddx2DeviceSetActive(ddxDevice);
  
    ddx2DeviceSetBackBufferSize(1024, 768);
    ddx2DeviceSetBackBufferRect(0, 0, 1024, 768);
    ddx2DeviceSetTextRenderRec(0, 0, 1024, 768);
    ddx2DeviceSetScreenRec(0, 0, 1024, 768);
    ddx2DeviceSetRender(TRUE);
  
    ddxInitDone = TRUE;
  }
  return(TRUE);
}

void ddxRelease(void)
{
  if(ddxInitDone) {
    ddx2DeviceRemove(ddxDevice);
    ddx2Release();
    ddxInitDone = FALSE;
  }
}

/*
void am_Flip(LEVELINFO *p_Level, ANIMATION_MODULE *p_am, char bAll, RECT_LINE *rline, int rsize, int bTutor, int bTText, int ty)
*/
void ddxPublish(void)
{
  int i, x = 0;
  float s[2];
  
  s[0] = (float)hwconf.xres / 1024.0f;
  s[1] = (float)hwconf.yres / 768.0f;
  
  char bAll = TRUE;
  /*
  RECT	r;
  
  r.left = mx;
  r.top = my;
  r.right = _3dCur.idx;
  r.bottom = _3dCur.idy;
  
  ddx2AddRectItem(rline, r, 0);
  */
  if(!rline.rlast)
    ddx2SetRect(NULL, 0);
  else
  {
    ZeroMemory(rDrawRect, sizeof(RECT) * DRAW_RECT_NUM);
    for(i=0;i<rline.rlast;i++) {
      if(!_2d_Is_InRectLine(rDrawRect, &rline.rect[i].rect, x))
      {
        memcpy(&rDrawRect[x], &rline.rect[i].rect, sizeof(RECT));
        x++;
      }
    }
    ddx2SetRect(rDrawRect, x);
  }
  
  spracuj_spravy(0);
  
  if(bAll)
    ddx2RenderujVse(p_ber);
  else
    ddx2RenderujMenu(p_ber);
  
  flip();
  
  _2d_Clear_RectLine(&rline);
}

int ddxFindFreeSurface(void)
{
  return(ddx2FindFreeSurface());
}

int ddxReleaseBitmap(int iSurface)
{
  return(ddx2ReleaseBitmap(iSurface));
}

int ddxLoadList(char *pFileName, int bProgress)
{
  if(bProgress)
  {
    ddxSetCursor(0);
    ddxSetFlip(0);
  }
  
  kprintf(1,"Kofola: - Load bitmap pro herni menu");
  ddx2LoadList(pFileName, pBmpArchive, bmp_dir);
  
  if(bProgress)
  {
    ddxSetCursor(1);
    ddxSetFlip(1);
  }
  return(1);
}

int ddxLoadBitmap(char *pFileName, APAK_HANDLE *pHandle)
{
  return(ddx2LoadBitmap(pFileName, pHandle));
}

int ddxCreateSurface(int x, int y, int idx)
{
  return(ddx2CreateSurface(x, y, idx));
}

void ddxDrawSurfaceColorKey(int iSurface, int *com, int layer, COLORREF color)
{
  ddx2DrawSurfaceColorKey(iSurface, com, layer, color);
}
void ddxDrawDisplayColorKey(int *com, int layer, COLORREF color)
{
  ddx2DrawDisplayColorKey(com, layer, color);
}
void ddxDrawDisplay(int *com, int layer)
{
  ddx2DrawDisplay(com, layer);  
}
void ddxDrawSurface(int iSurface, int *com, int layer)
{
  ddx2DrawSurface(iSurface, com, layer);
}

/*
BOOL ddx2TransparentBltFull(SurfaceHandle dst, int dx, int dy, 
                            SurfaceHandle src, dword barva);

BOOL ddx2BitBltFull(SurfaceHandle dst, int dx, int dy, SurfaceHandle src);
*/

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
  bool ret = ddx2TransparentBlt(
                dcDestSurface,
                nXOriginDest,
                nYOriginDest,
                nWidthDest,
                nHeightDest,
                dcSrcSurface,
                nXOriginSrc,
                nYOriginSrc,
                nWidthSrc,
                nHeightSrc,
                crTransparent);
  return(ret);
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
  bool ret = ddx2TransparentBltDisplay(
                nXOriginDest,
                nYOriginDest,
                nWidthDest,
                nHeightDest,
                dcSrcSurface,
                nXOriginSrc,
                nYOriginSrc,
                nWidthSrc,
                nHeightSrc,
                crTransparent);
  return(ret);
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
  return ddx2BitBlt(
            dcDestSurface,
            nXOriginDest,
            nYOriginDest,
            nWidthDest,
            nHeightDest,
            dcSrcSurface,
            nXOriginSrc,
            nYOriginSrc);
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
  int ret = ddx2BitBltDisplay(
                nXOriginDest,
                nYOriginDest,
                nWidthDest,
                nHeightDest,
                dcSrcSurface,
                nXOriginSrc,
                nYOriginSrc);
  return(ret);
}

int ddxGetWidth(int iSurface)
{
  return(ddx2GetWidth(iSurface));
}
int ddxGetHight(int iSurface)
{
  return(ddx2GetHeight(iSurface));
}

void ddxCleareSurface(int iSurface)
{
  ddx2CleareSurface(iSurface);
}
void ddxCleareSurfaceColor(int iSurface, COLORREF color)
{
  ddx2CleareSurfaceColor(iSurface, color);
}
void ddxFillRect(int iSurface, RECT *rect, COLORREF color)
{
  ddx2FillRect(iSurface, rect, color);
}
void ddxFillRectDisplay(RECT *rect, COLORREF color)
{
  ddx2FillRect(DDX2_BACK_BUFFER, rect, color);
}
void ddxAddRectItem(void *p_rl, RECT rect, int iLayer)
{
  ddx2AddRectItem((RECT_LINE *)p_rl, rect, iLayer);
}

int  ddxStretchBltDisplay(RECT *rDest, int iSurface, RECT *rSource)
{
}
int  ddxStretchBlt(int iSDest, RECT *rDest, int iSSource, RECT *rSource)
{
}

int ddxUpdateMouse(void)
{
	spracuj_spravy(1);

	dim.dx = mi.dx;
	dim.dy = mi.dy;

	dim.x = mi.x;
	dim.y = mi.y;
	
	dim.rx = mi.x - start_x;
	dim.ry = mi.y - start_y;

/* TODO?
	if(dim.x < dim.x_min)
		dim.x = dim.x_min;
	else
		if(dim.x > dim.x_max)
			dim.x = dim.x_max;	
	
	if(dim.y < dim.y_min)
		dim.y = dim.y_min;
	else
		if(dim.y > dim.y_max)
			dim.y = dim.y_max;
*/

	dim.t1 = dim.dt1 = mi.t1;
	dim.t2 = dim.dt2 = mi.t2;

	if(!dim.t1)
		dim.lt1 = 0;

	if(!dim.t2)
		dim.lt2 = 0;

	if(dim.t1 && dim.lt1)
		dim.t1 = 0;

	if(dim.t2 && dim.lt2)
		dim.t2 = 0;

	if(!dim.lt1 && dim.t1)
	{
		dim.lt1 = 1;
		dim.tf1 = 1;
	}

	if(!dim.lt2 && dim.t2)
	{
		dim.lt2 = 1;
		dim.tf2 = 1;
	}

	return TRUE;
}

void ddxSetFlip(char bSwitch)
{
  bFlip = bSwitch;
}
void ddxSetCursor(char bSwitch)
{
  //SDL_ShowCursor(bSwitch ? SDL_ENABLE : SDL_DISABLE);
}
void ddxResizeCursorBack(int iSurface)
{
}

BOOL ddxRestore(int *p_CompositDC, int *p_FontDC, int *p_BackDC, int *p_iCompositDC, int *p_iFontDC, int *p_iBackDC, 
                char *p_cBrutalRestart, AUDIO_DATA *p_ad)
{
	int i;

	/*if(key[K_S])
	{
		key[K_S] = 0;
		ddxSaveSurfaces();
	}*/
/*
	if(karmin_aktivni && timeGetTime() - dwLastMenuMusicCheck > 20000)
	{
		if(!ogg_playing() && !cCheckMusicExeption)
			ap_Play_Song(0, 0, p_ad);

		dwLastMenuMusicCheck = timeGetTime();
	}
*/

	if(bLastGameState != karmin_aktivni)
	{
		bLastGameState = karmin_aktivni;
	
    // Game is restored
		if(bLastGameState)
		{
			if(!bWindowMenu)
			{
/*
				//restoruju surfacy;
				g_pDisplay->GetDirectDraw()->RestoreAllSurfaces();

				// release vsech surfacu
				for(i=0;i<ddx.bm_count;i++) {
					if(ddx.surface[i].bLoad)
					{
						SAFE_DELETE( ddx.surface[i].g_pSurface );
						memset(&ddx.surface[i], 0, sizeof(SURFACESTRUCT));

						//ddx.surface[i].bLoad = 0;
					}
        }
*/
        assert(0);
      
				ddxLoadList("2d_load.dat", 0);
				*p_iCompositDC = ddxFindFreeSurface();
				*p_CompositDC = ddxCreateSurface(1024, 768, *p_iCompositDC);
				*p_iFontDC = ddxFindFreeSurface();
				*p_FontDC = ddxCreateSurface(1024, 768, *p_iFontDC);
				*p_iBackDC = ddxFindFreeSurface();
				*p_BackDC = ddxCreateSurface(1024, 768, *p_iBackDC);

				*p_cBrutalRestart = 1;

				SetCursor(FALSE);
				ddxSetFlip(TRUE);
				ddxSetCursor(TRUE);
				ddxSetCursorSurface(0);
				ddxResizeCursorBack(0);
			}
/*
			if(!ogg_playing())
				ap_Play_Song(0, 0, p_ad);
*/
			if(bWindowMenu)
			{
				//BringWindowToTop(hwnd_hry);
				//SetForegroundWindow(hwnd_hry);
				//SetFocus(hwnd_hry);
			}

			if(!bWindowMenu)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
    /*
			adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
			adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 
			ap_Stop_Song(p_ad);
    */
		}
	}

	if(MenuCheckBossExit())
	{
		gl_Kofola_End(1);
	}

	return FALSE;
}

void ddxSaveSurface(int idx)
{
}

void ddxCleareSurfaceColorDisplay(COLORREF color)
{
}

// Return always "windowed" mode
int ddxGetMode(void)
{
  return(TRUE);
}

void InitDirectDraw( HWND hWnd , int x, int y, int bpp)
{
	bDXAktivni = 1;
}

void FreeDirectDraw()
{
	bDXAktivni = 0;
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
  //SDL_ShowCursor(state ? SDL_ENABLE : SDL_DISABLE);
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
  wchar_t *p_tmp = (wchar_t *)mmalloc(bytes_in_len*2);
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

char current_working_dir[PATH_MAX];
char current_working_dir_file[PATH_MAX];

void working_dir_init(void)
{
  getcwd(current_working_dir,PATH_MAX);
}

char * working_file_get(const char *p_file)
{
  return(return_path_ext(p_file, current_working_dir,
                         current_working_dir_file, PATH_MAX));
}

char * working_file_get(const char *p_file, char *p_target, int size)
{
  return(return_path_ext(p_file, current_working_dir,
                         p_target, size));
}

void working_file_translate(char *p_file, int size)
{
  return_path_ext(p_file, current_working_dir, current_working_dir_file, PATH_MAX);
  assert(size <= PATH_MAX);
  strcpy(p_file, current_working_dir_file);
}
