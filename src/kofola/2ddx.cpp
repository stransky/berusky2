#define DIRECTINPUT_VERSION 0x0800

#include <math.h>
#include "ddutil.h"
#include "2d_graphic.h"
#include "2ddx.h"
#include "apak.h"
#include "..\\Komat\\di.h"
#include "adas.h"
#include "tools.h"

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dinput8.lib")

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

const NUM_OF_BITMAPS = 600;

typedef struct
{
  CSurface *g_pSurface;
  int x;
  int y;
  char bLoad;
} SURFACESTRUCT;

typedef struct
{
  int bm_count;
  char bm_dir[256];
  SURFACESTRUCT *surface;
} _2DX_DATA;

extern "C" void kprintf(byte log, byte * p_text, ...);
extern "C" byte ini_file[300];
extern "C" APAK_HANDLE * pBmpArchive;
extern "C" APAK_HANDLE * pControlsArchive;
extern "C" APAK_HANDLE * p3DMArchive;
extern "C" APAK_HANDLE * pSndArchive;
extern "C" RECT_LINE rline;
extern "C" HINSTANCE hinst;
extern "C" MOUSE_INFO dim;
extern "C" int bWindowMenu;

_2DX_DATA ddx;
CDisplay *g_pDisplay = NULL;
static int i_Cursor[2];
static int i_CursorDDX = 0;
LPDIRECTINPUT8 i_pDInput = NULL;
LPDIRECTINPUTDEVICE8 g_pMouse = NULL;
DIMOUSESTATE MouseState;
extern "C" char bDXAktivni;

// These variables are set in StorePixelFormat and should be defined
// elsewhere(global, member variable, etc...)
WORD NumberRedBits, NumberGreenBits, NumberBlueBits;
WORD LowRedBit, LowGreenBit, LowBlueBit;
static char bFlip = 1;
static char bDrawCursor = 1;
static unsigned long bLastGameState = 1;

extern "C" unsigned long karmin_aktivni;
extern "C" HWND hwnd_hry;

extern "C" int spracuj_spravy(int priorita);
extern "C" void gi_Release_Sound_Engine(void);
extern "C" char MenuCheckBossExit(void);
extern "C" void gl_Kofola_End(int DirectX);

extern "C" char cCheckMusicExeption;

//static HHOOK                          hKBHook = 0;

int start_x = 0;
int start_y = 0;

extern "C" void ddxSaveSurface(int idx)
{
  char cfile[256];
  FILE *file;

  sprintf(cfile, "c:\\bmp_debug\\%d.bmp", idx);

  file = fopen(cfile, "wb");

  if (!file)
    return;
  else {
    ddx.surface[idx].g_pSurface->Save(file, ddx.surface[idx].x,
      ddx.surface[idx].y);
    fclose(file);
  }
}

extern "C" void ddxSaveSurfaces(void)
{
  int i;
  FILE *file;

  file = fopen("c:\\bmp_debug\\front.bmp", "wb");

  if (file) {
    g_pDisplay->Save(file, 1024, 768, 1);
    fclose(file);
  }

  file = fopen("c:\\bmp_debug\\back.bmp", "wb");

  if (file) {
    g_pDisplay->Save(file, 1024, 768, 0);
    fclose(file);
  }

  for (i = 0; i < NUM_OF_BITMAPS; i++)
    if (ddx.surface[i].bLoad)
      ddxSaveSurface(i);
}

extern "C" void ddxSetFlip(char bSwitch)
{
  bFlip = bSwitch;
}

extern "C" void ddxSetCursor(char bSwitch)
{
  bDrawCursor = bSwitch;
}

void ddxTiskniError(char *ctext, HRESULT hr)
{
/*	#ifdef _DEBUG
		return;
	#endif*/

  switch (hr) {
    case DDERR_GENERIC:
      kprintf(1, (unsigned char *) "%s - DDERR_GENERIC ", ctext);
      break;
    case DDERR_INVALIDCLIPLIST:
      kprintf(1, (unsigned char *) "%s - DDERR_INVALIDCLIPLIST  ", ctext);
      break;
    case DDERR_INVALIDOBJECT:
      kprintf(1, (unsigned char *) "%s - DDERR_INVALIDOBJECT  ", ctext);
      break;
    case DDERR_INVALIDPARAMS:
      kprintf(1, (unsigned char *) "%s - DDERR_INVALIDPARAMS  ", ctext);
      break;
    case DDERR_INVALIDRECT:
      kprintf(1, (unsigned char *) "%s - DDERR_INVALIDRECT  ", ctext);
      break;
    case DDERR_NOALPHAHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NOALPHAHW  ", ctext);
      break;
    case DDERR_NOBLTHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NOBLTHW  ", ctext);
      break;
    case DDERR_NOCLIPLIST:
      kprintf(1, (unsigned char *) "%s - DDERR_NOCLIPLIST  ", ctext);
      break;
    case DDERR_NODDROPSHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NODDROPSHW  ", ctext);
      break;
    case DDERR_NOMIRRORHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NOMIRRORHW  ", ctext);
      break;
    case DDERR_NORASTEROPHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NORASTEROPHW  ", ctext);
      break;
    case DDERR_NOROTATIONHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NOROTATIONHW  ", ctext);
      break;
    case DDERR_NOSTRETCHHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NOSTRETCHHW  ", ctext);
      break;
    case DDERR_NOZBUFFERHW:
      kprintf(1, (unsigned char *) "%s - DDERR_NOZBUFFERHW   ", ctext);
      break;
    case DDERR_SURFACEBUSY:
      kprintf(1, (unsigned char *) "%s - DDERR_SURFACEBUSY   ", ctext);
      break;
    case DDERR_SURFACELOST:
      kprintf(1, (unsigned char *) "%s - DDERR_SURFACELOST   ", ctext);
      break;
    case DDERR_UNSUPPORTED:
      kprintf(1, (unsigned char *) "%s - DDERR_UNSUPPORTED   ", ctext);
      break;
    case DDERR_WASSTILLDRAWING:
      kprintf(1, (unsigned char *) "%s - DDERR_WASSTILLDRAWING   ", ctext);
      break;
    default:
      kprintf(1, (unsigned char *) "%s - TAK TO JE NEJAKA PICOVINA   ",
        ctext);
      break;
  }
}

// Small utility function to find the LowBit and Number of Bits in a supplied Mask
void ProcessBits(DWORD Mask, WORD * LowBit, WORD * NumBits)
{
  DWORD TestMask = 1;

  for (*LowBit = 0; *LowBit < 32; (*LowBit)++) {
    if (Mask & TestMask)
      break;
    TestMask <<= 1;
  }

  TestMask <<= 1;
  for (*NumBits = 1; *NumBits < 32; (*NumBits)++) {
    if (!(Mask & TestMask))
      break;
    TestMask <<= 1;
  }
}

BOOL StorePixelFormat()
{
  // Define a PixelFormat variable, clear it and set the dwSize variable, as usual.
  DDPIXELFORMAT DDPixelFormat;

  ZeroMemory(&DDPixelFormat, sizeof(DDPixelFormat));
  DDPixelFormat.dwSize = sizeof(DDPixelFormat);

  // Fill the PixelFormat from the information for the Primary Surface
  if (FAILED((g_pDisplay->GetBackBuffer())->GetPixelFormat(&DDPixelFormat))) {
    // Fatal error. The program should exit nicely
    return FALSE;
  }

  // Save the Low Bit and Number of Bits
  ProcessBits(DDPixelFormat.dwRBitMask, &LowRedBit, &NumberRedBits);
  ProcessBits(DDPixelFormat.dwGBitMask, &LowGreenBit, &NumberGreenBits);
  ProcessBits(DDPixelFormat.dwBBitMask, &LowBlueBit, &NumberBlueBits);

  return TRUE;
}

HRESULT TurnonCursor(VOID)
{
  HRESULT hr;

  if (!g_pMouse)
    return (E_POINTER);

  if (FAILED(hr = g_pMouse->Acquire()))
    return hr;

  return S_OK;
}

HRESULT TurnoffCursor(VOID)
{
  HRESULT hr;

  if (!g_pMouse)
    return (E_POINTER);

  if (FAILED(hr = g_pMouse->Unacquire()))
    return hr;

  return S_OK;
}

/*extern "C" HRESULT ddxUpdateMouse(VOID)
{
	HRESULT hr;

	if(!g_pMouse)
		return E_POINTER;

	if( FAILED( hr = g_pMouse->GetDeviceState(sizeof(MouseState), &MouseState) ) )
	{
		TurnonCursor();
		if( FAILED( hr = g_pMouse->GetDeviceState(sizeof(MouseState), &MouseState) ) )
			return hr;
	}

	dim.x += (dim.dx = MouseState.lX);
	
	if(dim.x < dim.x_min)
		dim.x = dim.x_min;
	else
		if(dim.x > dim.x_max)
			dim.x = dim.x_max;

	dim.y += (dim.dy = MouseState.lY);
	
	if(dim.y < dim.y_min)
		dim.y = dim.y_min;
	else
		if(dim.y > dim.y_max)
			dim.y = dim.y_max;

	dim.t1 = dim.dt1 = (MouseState.rgbButtons[0] & 0x80) ? 1 : 0;
	dim.t2 = dim.dt2 = (MouseState.rgbButtons[1] & 0x80) ? 1 : 0;

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

	return S_OK;
}*/

extern "C" HRESULT ddxUpdateMouse(VOID)
{
  spracuj_spravy(1);

  dim.dx = mi.dx;
  dim.dy = mi.dy;

  dim.x = mi.x;
  dim.y = mi.y;

  dim.rx = mi.x - start_x;
  dim.ry = mi.y - start_y;
/*
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

  if (!dim.t1)
    dim.lt1 = 0;

  if (!dim.t2)
    dim.lt2 = 0;

  if (dim.t1 && dim.lt1)
    dim.t1 = 0;

  if (dim.t2 && dim.lt2)
    dim.t2 = 0;

  if (!dim.lt1 && dim.t1) {
    dim.lt1 = 1;
    dim.tf1 = 1;
  }

  if (!dim.lt2 && dim.t2) {
    dim.lt2 = 1;
    dim.tf2 = 1;
  }

  return S_OK;
}


VOID FreeDirectInput(VOID)
{
  TurnoffCursor();

  if (g_pMouse) {
    g_pMouse->Release();
    g_pMouse = NULL;
  }

  if (i_pDInput) {
    i_pDInput->Release();
    i_pDInput = NULL;
  }
}

HRESULT InitDirectInput(HWND hWnd)
{
/*	HRESULT hr;

	if(FAILED(hr = DirectInput8Create((HINSTANCE) GetWindowLong( hWnd, GWL_HINSTANCE ), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&i_pDInput, NULL) ) )
		return hr;

	if(FAILED(hr = i_pDInput->CreateDevice(GUID_SysMouse, &g_pMouse, NULL) ) )
	{
		FreeDirectInput();
		return hr;
	}

	if(FAILED(hr = g_pMouse->SetDataFormat(&c_dfDIMouse) ) )
	{
		FreeDirectInput();
		return hr;
	}

	if(FAILED(hr = g_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) ) )
	{
		FreeDirectInput();
		return hr;
	}*/

  dim.x_min = 0;
  dim.x_max = 1024;
  dim.y_min = 0;
  dim.y_max = 768;
  dim.x = dim.y = dim.t1 = dim.t2 = 0;
  dim.lt1 = dim.lt2 = 0;

/*	if( FAILED( TurnonCursor( ) ) )
	{
		FreeDirectInput();
		return hr;
	}*/

  return S_OK;
}

extern "C" HRESULT DisplayFramePure()
{
  HRESULT hr;

  if (FAILED(hr = g_pDisplay->Present()))
    return hr;

  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DisplayFrame()
// Desc: Blts a the sprites to the back buffer, then flips the 
//       back buffer onto the primary buffer.
//-----------------------------------------------------------------------------
extern "C" HRESULT DisplayFrame()
{
  RECT r;
  HRESULT hr;
  int i_Act = g_pDisplay->GetActBuffer();

  // We are in fullscreen mode, so perform a flip and return 
  // any errors like DDERR_SURFACELOST
  r.left = 0;
  r.top = 0;
  r.right = ddxGetWidth(i_CursorDDX);
  r.bottom = ddxGetHight(i_CursorDDX);

  if (dim.rx + r.right >= 1024)
    r.right -= dim.rx + r.right - 1024;

  if (dim.y + r.bottom >= 768)
    r.bottom -= dim.ry + r.bottom - 768;

  ddxBitBlt(i_Cursor[i_Act], 0, 0, r.right, r.bottom, HDC2DD, dim.rx, dim.ry);

  if (bDrawCursor) {
    ddx.surface[i_CursorDDX].g_pSurface->SetColorKey(TRANSCOLOR);

    g_pDisplay->ColorKeyBlt(dim.rx, dim.ry,
      ddx.surface[i_CursorDDX].g_pSurface->GetDDrawSurface(), &r);
  }

  g_pDisplay->SetCursorX(i_Act, dim.rx);
  g_pDisplay->SetCursorY(i_Act, dim.ry);

  if (FAILED(hr = g_pDisplay->Present()))
    return hr;

  if (!g_pDisplay->IsWindowed()) {
    g_pDisplay->AddBufferCounter();
    i_Act = g_pDisplay->GetActBuffer();
  }

  r.left = 0;
  r.top = 0;
  r.right = ddxGetWidth(i_CursorDDX);
  r.bottom = ddxGetHight(i_CursorDDX);

  if (g_pDisplay->GetCursorX(i_Act) + r.right >= 1024)
    r.right -= g_pDisplay->GetCursorX(i_Act) + r.right - 1024;

  if (g_pDisplay->GetCursorY(i_Act) + r.bottom >= 768)
    r.bottom -= g_pDisplay->GetCursorY(i_Act) + r.bottom - 768;


  g_pDisplay->Blt(g_pDisplay->GetCursorX(i_Act),
    g_pDisplay->GetCursorY(i_Act), ddx.surface[i_Cursor[i_Act]].g_pSurface,
    &r);

  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDirectDraw()
// Desc: Create the DirectDraw object, and init the surfaces
//-----------------------------------------------------------------------------
extern "C" HRESULT InitDirectDraw(HWND hWnd, int x, int y, int bpp)
{
  int i;
  HRESULT hr;

  g_pDisplay = new CDisplay();

  int bWindowINI =
    !GetPrivateProfileInt("hra", "fullscreen", 1, (const char *) ini_file);

  if (bWindowINI) {
    if (FAILED(hr = g_pDisplay->CreateWindowedDisplay(hWnd, x, y))) {
      char text[256];

      sprintf(text, "%dx%dx%d.", x, y, bpp);

      MyMessageBox(hwnd_hry, "##error_title", "##dxinit_error", text);

      return hr;
    }
  }
  else if (FAILED(hr = g_pDisplay->CreateFullScreenDisplay(hWnd, x, y, bpp))) {
    char text[256];

    sprintf(text, "%dx%dx%d.", x, y, bpp);

    MyMessageBox(hwnd_hry, "##error_title", "##dxinit_error", text);

    return hr;
  }

  g_pDisplay->Clear(0);
  g_pDisplay->Present();
  g_pDisplay->Clear(0);

  if (!StorePixelFormat()) {
    FreeDirectDraw();
    return FALSE;
  }

  kprintf(1, (unsigned char *) "PixelFormat = %d(%d), %d(%d), %d(%d)",
    NumberRedBits, LowRedBit, NumberGreenBits, LowGreenBit, NumberBlueBits,
    LowBlueBit);

  if (FAILED(hr = InitDirectInput(hWnd))) {
    MessageBox(hWnd, TEXT("Unable to init directinput. "),
      TEXT("Berusky 2"), MB_ICONERROR | MB_OK);
    return hr;
  }

  i = g_pDisplay->CheckFlipMode();

  if (i == 1)
    kprintf(1, (unsigned char *) "FLIP MODE detected ...");
  else if (i == 2)
    kprintf(1, (unsigned char *) "TRIPLE FLIP MODE detected ...");
  else
    kprintf(1, (unsigned char *) "BLIT MODE detected ...");

  bDXAktivni = 1;

  return S_OK;
}

extern "C" int ddxGetMode(void)
{
  return g_pDisplay->IsWindowed();
}

//-----------------------------------------------------------------------------
// Name: FreeDirectDraw()
// Desc: Release all the DirectDraw objects
//-----------------------------------------------------------------------------
extern "C" VOID FreeDirectDraw()
{
  kprintf(1, (unsigned char *) "FreeDirectDraw...");
  //FreeDirectInput();
  SAFE_DELETE(g_pDisplay);
  kprintf(1, (unsigned char *) "FreeDirectDraw DONE");
  bDXAktivni = 0;
}

//------------------------------------------------------------------------------------------------
// init 2d
//------------------------------------------------------------------------------------------------
extern "C" int ddxInit(void)
{
  //int i;

  kprintf(1, (unsigned char *) "Kofola - init bitmap -");

  ddx.bm_count = NUM_OF_BITMAPS;

  ddx.surface =
    (SURFACESTRUCT *) malloc((ddx.bm_count) * sizeof(SURFACESTRUCT));
  if (!ddx.surface) {
    kprintf(1, (unsigned char *) "Unable to allocate memory for bitmaps");
    return 0;
  }
  else
    ZeroMemory(ddx.surface, (ddx.bm_count) * sizeof(SURFACESTRUCT));

  /*for(i=0;i<ddx.bm_count;i++)
     {
     ddx.surface[i].bLoad = 0;
     ddx.surface[i].g_pSurface = NULL;
     } */

  GetPrivateProfileString("game", "bitmap_dir", "c:\\", ddx.bm_dir, 256,
    (const char *) ini_file);

  ddxSetCursor(1);
  ddxSetCursorSurface(0);
  ddxSetFlip(1);

  return 1;
}

//------------------------------------------------------------------------------------------------
// 2d release
//------------------------------------------------------------------------------------------------
extern "C" void ddxRelease(void)
{
  int i;

  kprintf(1, (unsigned char *) "Kofola: - Release Bitmap...");
  for (i = 0; i < ddx.bm_count; i++)
    if (ddx.surface[i].bLoad) {
      kprintf(1, (unsigned char *) "Release Bitmapy[%d]...", i);
      SAFE_DELETE(ddx.surface[i].g_pSurface);
      ddx.surface[i].bLoad = 0;
    }

  kprintf(1, (unsigned char *) "free ((void *) ddx.surface);");

  if (ddx.surface) {
    free((void *) ddx.surface);
    ddx.surface = NULL;
  }

  kprintf(1, (unsigned char *) "release bitmap hotov");
}

//------------------------------------------------------------------------------------------------
// finds first free index
//------------------------------------------------------------------------------------------------
extern "C" int ddxFindFreeSurface(void)
{
  int i;

  for (i = 0; i < ddx.bm_count; i++)
    if (!ddx.surface[i].bLoad)
      return i;

  return -1;
}

//------------------------------------------------------------------------------------------------
// release bitmap
//------------------------------------------------------------------------------------------------
extern "C" int ddxReleaseBitmap(int iSurface)
{
  if (iSurface < 0)
    return 1;

  if (iSurface > (ddx.bm_count - 1))
    return 0;

  kprintf(1, (unsigned char *) "Release Bitmapy[%d]...", iSurface);

  if (ddx.surface[iSurface].bLoad) {
    SAFE_DELETE(ddx.surface[iSurface].g_pSurface);
    ddx.surface[iSurface].bLoad = 0;
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
// load list of bitmaps from APAK
//------------------------------------------------------------------------------------------------
extern "C" int ddxLoadList(char *pFileName, int bProgress)
{
  //HRESULT         hr;
  int c = 0;
  char text[256];
  FILE *file = 0;
  DWORD Eplased = 0;
  DWORD Start, Stop;
  RECT r = { 0, 753, 0, 768 };

  if (bProgress) {
    ddxSetCursor(0);
    ddxSetFlip(0);
  }

  kprintf(1, (unsigned char *) "ddxLoadList - achdir");
  achdir(pBmpArchive, ddx.bm_dir);

  kprintf(1, (unsigned char *) "ddxLoadList - aopen");
  file = aopen(pBmpArchive, pFileName, "rb");

  kprintf(1, (unsigned char *) "Kofola: - Load bitmap pro herni menu");

  if (!file) {
    kprintf(1, (unsigned char *) "File not found : %s", pFileName);
    return 0;
  }
  else {
    Start = timeGetTime();

    while (!aeof(file)) {
      agets(text, 256, file);
      if (!aeof(file)) {
        text[strlen(text) - 1] = '\0';

        kprintf(1, (unsigned char *) "Loading bitmap %s ...", text);

        ddxLoadBitmap(text, pBmpArchive);
        /*hr = g_pDisplay->CreateSurfaceFromBitmapArchive(&ddx.surface[c].g_pSurface, text, pBmpArchive);

           if(hr == S_OK)
           {
           LPDDSURFACEDESC2     p_DDSD = NULL;

           ddx.surface[c].bLoad = 1;
           p_DDSD = ddx.surface[c].g_pSurface->GetDDrawSurfaceDesc();

           if(p_DDSD)
           {
           ddx.surface[c].x = p_DDSD->dwWidth;
           ddx.surface[c].y = p_DDSD->dwHeight;
           }
           } */
      }

      c++;

      if (bProgress) {
        r.right = (int) floor(c * (1024 / 214.0f));

        ddxFillRectDisplay(&r, RGB(0, 47, 81));
        DisplayFramePure();
      }
    }

    aclose(file);

    Stop = timeGetTime();
    Eplased = Stop - Start;

    kprintf(1, (unsigned char *)
      "--------------Total load time %.1f s -----------------",
      Eplased / 1000.0f);

    if (bProgress) {
      ddxSetCursor(1);
      ddxSetFlip(1);
    }

    i_Cursor[0] = ddxFindFreeSurface();
    ddxCreateSurface(ddxGetWidth(0), ddxGetHight(0), i_Cursor[0]);

    i_Cursor[1] = ddxFindFreeSurface();
    ddxCreateSurface(ddxGetWidth(0), ddxGetHight(0), i_Cursor[1]);

    return 1;
  }

  if (bProgress) {
    //ddxSetCursor(1);
    ddxSetFlip(1);
  }
}

extern "C" void ddxResizeCursorBack(int iSurface)
{
  int idx1, idx2;

  idx1 = ddxFindFreeSurface();
  ddxCreateSurface(ddxGetWidth(iSurface), ddxGetHight(iSurface), idx1);

  idx2 = ddxFindFreeSurface();
  ddxCreateSurface(ddxGetWidth(iSurface), ddxGetHight(iSurface), idx2);

  ddxReleaseBitmap(i_Cursor[0]);
  ddxReleaseBitmap(i_Cursor[1]);

  i_Cursor[0] = idx1;
  i_Cursor[1] = idx2;

  ddxBitBlt(i_Cursor[0], 0, 0, ddxGetWidth(i_Cursor[0]),
    ddxGetHight(i_Cursor[0]), HDC2DD, g_pDisplay->GetCursorX(0),
    g_pDisplay->GetCursorY(0));
  ddxBitBlt(i_Cursor[1], 0, 0, ddxGetWidth(i_Cursor[1]),
    ddxGetHight(i_Cursor[1]), HDC2DD, g_pDisplay->GetCursorX(1),
    g_pDisplay->GetCursorY(1));
}

extern "C" void ddxSetCursorSurface(int iSurface)
{
  i_CursorDDX = iSurface;
}

void ddxCreateSurfaceError(HRESULT hr)
{
  char text[256];

  strcpy(text, "Protoze: ");

  switch (hr) {
    case DDERR_INCOMPATIBLEPRIMARY:
      strcat(text, "DDERR_INCOMPATIBLEPRIMARY");
      break;
    case DDERR_INVALIDCAPS:
      strcat(text, "DDERR_INVALIDCAPS ");
      break;
    case DDERR_INVALIDOBJECT:
      strcat(text, "DDERR_INVALIDOBJECT ");
      break;
    case DDERR_INVALIDPARAMS:
      strcat(text, "DDERR_INVALIDPARAMS ");
      break;
    case DDERR_INVALIDPIXELFORMAT:
      strcat(text, "DDERR_INVALIDPIXELFORMAT ");
      break;
    case DDERR_NOALPHAHW:
      strcat(text, "DDERR_NOALPHAHW ");
      break;
    case DDERR_NOCOOPERATIVELEVELSET:
      strcat(text, "DDERR_NOCOOPERATIVELEVELSET ");
      break;
    case DDERR_NODIRECTDRAWHW:
      strcat(text, "DDERR_NODIRECTDRAWHW ");
      break;
    case DDERR_NOEMULATION:
      strcat(text, "DDERR_NOEMULATION ");
      break;
    case DDERR_NOEXCLUSIVEMODE:
      strcat(text, "DDERR_NOEXCLUSIVEMODE ");
      break;
    case DDERR_NOFLIPHW:
      strcat(text, "DDERR_NOFLIPHW ");
      break;
    case DDERR_NOMIPMAPHW:
      strcat(text, "DDERR_NOMIPMAPHW ");
      break;
    case DDERR_NOOVERLAYHW:
      strcat(text, "DDERR_NOOVERLAYHW ");
      break;
    case DDERR_NOZBUFFERHW:
      strcat(text, "DDERR_NOZBUFFERHW ");
      break;
    case DDERR_OUTOFMEMORY:
      strcat(text, "DDERR_OUTOFMEMORY ");
      break;
    case DDERR_OUTOFVIDEOMEMORY:
      strcat(text, "DDERR_OUTOFVIDEOMEMORY ");
      break;
    case DDERR_PRIMARYSURFACEALREADYEXISTS:
      strcat(text, "DDERR_PRIMARYSURFACEALREADYEXISTS ");
      break;
    case DDERR_UNSUPPORTEDMODE:
      strcat(text, "DDERR_UNSUPPORTEDMODE ");
      break;
    default:
      strcat(text, "neznama chyba");
      break;
  }

  kprintf(1, (unsigned char *) "%s", text);
}

//------------------------------------------------------------------------------------------------
// load bitmap from APAK
//------------------------------------------------------------------------------------------------
extern "C" int ddxLoadBitmap(char *pFileName, APAK_HANDLE * pHandle)
{
  HRESULT hr;
  int c = ddxFindFreeSurface();

  if (c < 0)
    return -1;

  hr =
    g_pDisplay->CreateSurfaceFromBitmapArchive(&ddx.surface[c].g_pSurface,
    pFileName, pHandle);

  if (hr == S_OK) {
    LPDDSURFACEDESC2 p_DDSD = NULL;

    ddx.surface[c].bLoad = 1;
    p_DDSD = ddx.surface[c].g_pSurface->GetDDrawSurfaceDesc();

    if (p_DDSD) {
      ddx.surface[c].x = p_DDSD->dwWidth;
      ddx.surface[c].y = p_DDSD->dwHeight;
    }
  }
  else {
    kprintf(1, (unsigned char *)
      "Nepodarilo se vytvorit surface/nahrat bmp (ddxLoadBitmap '%s') idx = %d",
      pFileName, c);
    ddxCreateSurfaceError(hr);
    exit(0);
    return -1;
  }

  return c;
}

//------------------------------------------------------------------------------------------------
// Create Surface
//------------------------------------------------------------------------------------------------
extern "C" int ddxCreateSurface(int x, int y, int idx)
{
  HRESULT hr;

  if (idx < 0) {
    kprintf(1,
      (unsigned char *) "Neplatni index pro vytvoreni surfacy !!!! idx =  %d",
      idx);
    exit(0);
  }
  else
    kprintf(1, (unsigned char *) "ddxCreateSurface = %d", idx);

  hr = g_pDisplay->CreateSurface(&ddx.surface[idx].g_pSurface, x, y);

  if (hr == S_OK) {
    ddx.surface[idx].g_pSurface->Clear(TRANSCOLOR);
    ddx.surface[idx].bLoad = 1;
    ddx.surface[idx].x = x;
    ddx.surface[idx].y = y;
  }
  else {
    kprintf(1,
      (unsigned char *) "Nepodarilo se vytvorit surface %dx%x idx = %d", x, y,
      idx);
    ddxCreateSurfaceError(hr);
    exit(0);
    return 0;
  }

  return idx;
}

extern "C" void ddxAddRectItem(RECT_LINE * p_rl, RECT rect, int iLayer)
{
  if (p_rl->rlast < DRAW_RECT_NUM) {
    p_rl->rect[p_rl->rlast].rect = rect;
    p_rl->rect[p_rl->rlast].iLayer = iLayer;
    p_rl->rect[p_rl->rlast].bUsed = 1;

    p_rl->rlast++;
  }
}


//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
extern "C" void ddxDrawDisplay(int *com, int layer)
{
  HRESULT hr;

  RECT r;

  r.left = 0;
  r.top = 0;
  r.right = ddx.surface[com[1]].x;
  r.bottom = ddx.surface[com[1]].y;

  hr = g_pDisplay->Blt(com[2], com[3], ddx.surface[com[1]].g_pSurface, &r);

  if (!g_pDisplay->IsWindowed() && bFlip) {
    DisplayFrame();

    hr = g_pDisplay->Blt(com[2], com[3], ddx.surface[com[1]].g_pSurface, &r);
  }

  if (hr != DD_OK)
    ddxTiskniError("ddxDrawDisplay", hr);

/*    if( hr == DDERR_SURFACELOST )
    {
		kprintf(1, (unsigned char *) "ddxDrawDisplay DDERR_SURFACELOST");
    }
	else
		if(hr != DD_OK)
			kprintf(1, (unsigned char *) "ddxTransparentBltDisplay DDERR");*/

  r.left = com[2];
  r.top = com[3];

  ddxAddRectItem(&rline, r, layer);
}

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
extern "C" void ddxDrawDisplayColorKey(int *com, int layer, COLORREF color)
{
  HRESULT hr;

  RECT r;

  r.left = 0;
  r.top = 0;
  r.right = ddx.surface[com[1]].x;
  r.bottom = ddx.surface[com[1]].y;

  ddx.surface[com[1]].g_pSurface->SetColorKey(color);

  hr =
    g_pDisplay->ColorKeyBlt(com[2], com[3],
    ddx.surface[com[1]].g_pSurface->GetDDrawSurface(), &r);

  if (!g_pDisplay->IsWindowed() && bFlip) {
    DisplayFrame();

    hr =
      g_pDisplay->ColorKeyBlt(com[2], com[3],
      ddx.surface[com[1]].g_pSurface->GetDDrawSurface(), &r);
  }

  if (hr != DD_OK)
    ddxTiskniError("ddxDrawDisplayColorKey", hr);

/*    if( hr == DDERR_SURFACELOST )
    {
		kprintf(1, (unsigned char *) "ddxDrawDisplay DDERR_SURFACELOST");
    }
	else
		if(hr != DD_OK)
			kprintf(1, (unsigned char *) "ddxTransparentBltDisplay DDERR");*/

  r.left = com[2];
  r.top = com[3];

  ddxAddRectItem(&rline, r, layer);
}

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
extern "C" void ddxDrawSurface(int iSurface, int *com, int layer)
{
  HRESULT hr;
  CSurface *g_pSurface = ddx.surface[iSurface].g_pSurface;
  RECT r;

  r.left = 0;
  r.top = 0;
  r.right = ddx.surface[com[1]].x;
  r.bottom = ddx.surface[com[1]].y;

  hr = g_pSurface->Blt(com[2], com[3], ddx.surface[com[1]].g_pSurface, &r);

  //kprintf(1, (unsigned char *) "ddxDrawSurface %d, %d, %d", com[1], com[2], com[3]);

  if (hr != DD_OK)
    ddxTiskniError("ddxDrawSurface", hr);

/*    if( hr == DDERR_SURFACELOST )
    {
		kprintf(1, (unsigned char *) "ddxDrawSurface DDERR_SURFACELOST");
    }
	else
		if(hr != DD_OK)
			kprintf(1, (unsigned char *) "ddxTransparentBltDisplay DDERR");*/

  r.left = com[2];
  r.top = com[3];

  ddxAddRectItem(&rline, r, layer);
}

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
extern "C" void ddxDrawSurfaceColorKey(int iSurface, int *com, int layer,
  COLORREF color)
{
  HRESULT hr;
  CSurface *g_pSurface = ddx.surface[iSurface].g_pSurface;
  RECT r;

  r.left = 0;
  r.top = 0;
  r.right = ddx.surface[com[1]].x;
  r.bottom = ddx.surface[com[1]].y;

  ddx.surface[com[1]].g_pSurface->SetColorKey(color);

  hr =
    g_pSurface->ColorKeyBlt(com[2], com[3],
    ddx.surface[com[1]].g_pSurface->GetDDrawSurface(), &r);

  if (hr != DD_OK)
    ddxTiskniError("ddxDrawSurfaceColorKey", hr);

/*    if( hr == DDERR_SURFACELOST )
    {
		kprintf(1, (unsigned char *) "ddxDrawSurface DDERR_SURFACELOST");
	}
 	else
		if(hr != DD_OK)
			kprintf(1, (unsigned char *) "ddxTransparentBltDisplay DDERR");*/

  r.left = com[2];
  r.top = com[3];

  ddxAddRectItem(&rline, r, layer);
}

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
extern "C" BOOL ddxTransparentBlt(int dcDestSurface,    // handle to Dest DC
  int nXOriginDest,             // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc,              // y-coord of source upper-left corner
  int nWidthSrc,                // width of source rectangle
  int nHeightSrc,               // height of source rectangle
  UINT crTransparent            // color to make transparent
  )
{
  HRESULT hr;
  CSurface *g_pSurfaceDest;
  CSurface *g_pSurfaceSrc;
  RECT r;

  if (dcDestSurface == HDC2DD)
    return ddxTransparentBltDisplay(nXOriginDest, nYOriginDest, nWidthDest,
      nHeightDest, dcSrcSurface, nXOriginSrc, nYOriginSrc, nWidthSrc,
      nHeightSrc, crTransparent);

  if (dcSrcSurface == -1)
    return 0;

  g_pSurfaceDest = ddx.surface[dcDestSurface].g_pSurface;
  g_pSurfaceSrc = ddx.surface[dcSrcSurface].g_pSurface;

  r.left = nXOriginSrc;
  r.top = nYOriginSrc;
  r.right = nXOriginSrc + nWidthSrc;
  r.bottom = nYOriginSrc + nHeightSrc;

  g_pSurfaceSrc->SetColorKey(crTransparent);

  hr =
    g_pSurfaceDest->ColorKeyBlt(nXOriginDest, nYOriginDest,
    g_pSurfaceSrc->GetDDrawSurface(), &r);

  if (hr != DD_OK) {
    ddxTiskniError("ddxTransparentBlt", hr);
    kprintf(1, (unsigned char *)
      "xs = %d, ys = %d, xd = %d, yd = %d, dx = %d, dy = %d", nXOriginSrc,
      nYOriginSrc, nXOriginDest, nYOriginDest, nWidthSrc, nHeightSrc);
  }

  return TRUE;
}

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
extern "C" BOOL ddxTransparentBltDisplay(int nXOriginDest,      // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc,              // y-coord of source upper-left corner
  int nWidthSrc,                // width of source rectangle
  int nHeightSrc,               // height of source rectangle
  UINT crTransparent            // color to make transparent
  )
{
  HRESULT hr;
  CSurface *g_pSurfaceSrc = ddx.surface[dcSrcSurface].g_pSurface;
  RECT r;

  if (dcSrcSurface == -1)
    return 0;

  r.left = nXOriginSrc;
  r.top = nYOriginSrc;
  r.right = nXOriginSrc + nWidthSrc;
  r.bottom = nYOriginSrc + nHeightSrc;

  g_pSurfaceSrc->SetColorKey(crTransparent);

  hr =
    g_pDisplay->ColorKeyBlt(nXOriginDest, nYOriginDest,
    g_pSurfaceSrc->GetDDrawSurface(), &r);

  if (!g_pDisplay->IsWindowed() && bFlip) {
    DisplayFrame();

    hr =
      g_pDisplay->ColorKeyBlt(nXOriginDest, nYOriginDest,
      g_pSurfaceSrc->GetDDrawSurface(), &r);
  }

  if (hr != DD_OK) {
    ddxTiskniError("ddxTransparentBltDisplay", hr);
    kprintf(1, (unsigned char *)
      "xs = %d, ys = %d, xd = %d, yd = %d, dx = %d, dy = %d", nXOriginSrc,
      nYOriginSrc, nXOriginDest, nYOriginDest, nWidthSrc, nHeightSrc);
  }

  return TRUE;
}

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
extern "C" BOOL ddxBitBlt(int dcDestSurface,    // handle to Dest DC
  int nXOriginDest,             // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc               // y-coord of source upper-left corner
  )
{
  HRESULT hr;
  CSurface *g_pSurfaceDest;
  CSurface *g_pSurfaceSrc;
  RECT r;

  if (dcDestSurface == HDC2DD)
    return ddxBitBltDisplay(nXOriginDest, nYOriginDest, nWidthDest,
      nHeightDest, dcSrcSurface, nXOriginSrc, nYOriginSrc);

  g_pSurfaceDest = ddx.surface[dcDestSurface].g_pSurface;

  r.left = nXOriginSrc;
  r.top = nYOriginSrc;
  r.right = nXOriginSrc + nWidthDest;
  r.bottom = nYOriginSrc + nHeightDest;

  if (dcSrcSurface == HDC2DD)
    hr =
      g_pSurfaceDest->Blt(nXOriginDest, nYOriginDest,
      g_pDisplay->GetBackBuffer(), &r);
  else {
    g_pSurfaceSrc = ddx.surface[dcSrcSurface].g_pSurface;
    hr =
      g_pSurfaceDest->Blt(nXOriginDest, nYOriginDest,
      g_pSurfaceSrc->GetDDrawSurface(), &r);
  }

  if (hr != DD_OK) {
    ddxTiskniError("ddxBitBlt", hr);

    kprintf(1, (unsigned char *)
      "xs = %d, ys = %d, xd = %d, yd = %d, dx = %d, dy = %d", nXOriginSrc,
      nYOriginSrc, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest);
  }

  return TRUE;
}

extern "C" BOOL ddxBitBltDisplay(int nXOriginDest,      // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc               // y-coord of source upper-left corner
  )
{
  HRESULT hr;
  CSurface *g_pSurfaceSrc = ddx.surface[dcSrcSurface].g_pSurface;
  RECT r;

  if (dcSrcSurface == -1)
    return 0;

  r.left = nXOriginSrc;
  r.top = nYOriginSrc;
  r.right = nXOriginSrc + nWidthDest;
  r.bottom = nYOriginSrc + nHeightDest;

  hr =
    g_pDisplay->Blt(nXOriginDest, nYOriginDest,
    g_pSurfaceSrc->GetDDrawSurface(), &r);

  if (!g_pDisplay->IsWindowed() && bFlip) {
    DisplayFrame();

    hr =
      g_pDisplay->Blt(nXOriginDest, nYOriginDest,
      g_pSurfaceSrc->GetDDrawSurface(), &r);
  }

  if (hr != DD_OK) {
    ddxTiskniError("ddxBitBltDisplay", hr);

    kprintf(1, (unsigned char *)
      "xs = %d, ys = %d, xd = %d, yd = %d, dx = %d, dy = %d", nXOriginSrc,
      nYOriginSrc, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest);
  }

  return TRUE;
}

extern "C" int ddxGetWidth(int iSurface)
{
  return ddx.surface[iSurface].x;
}

extern "C" int ddxGetHight(int iSurface)
{
  return ddx.surface[iSurface].y;
}

extern "C" void ddxCleareSurface(int iSurface)
{
  ddx.surface[iSurface].g_pSurface->Clear(RGB(255, 0, 255));
}

extern "C" void ddxCleareSurfaceColor(int iSurface, COLORREF color)
{
  HRESULT hr = ddx.surface[iSurface].g_pSurface->Clear(color);

  if (hr != DD_OK)
    kprintf(1, (unsigned char *) "Clear iSurface = %d vyvolal CHYBU!!!",
      iSurface);
}

extern "C" void ddxCleareSurfaceColorDisplay(COLORREF color)
{
  HRESULT hr = g_pDisplay->Clear(color);

  if (hr != DD_OK)
    kprintf(1, (unsigned char *) "Clear g_pDisplay vyvolal CHYBU!!!");
}

extern "C" void ddxFillRect(int iSurface, RECT * rect, COLORREF color)
{
  ddx.surface[iSurface].g_pSurface->FillRect(rect, color);
}

extern "C" void ddxFillRectDisplay(RECT * rect, COLORREF color)
{
  g_pDisplay->FillRect(rect, color);
}

extern "C" HRESULT ddxStretchBltDisplay(RECT * rDest, int iSurface,
  RECT * rSource)
{
  return g_pDisplay->StretchBlt(rDest, ddx.surface[iSurface].g_pSurface,
    rSource);
}

extern "C" HRESULT ddxStretchBlt(int iSDest, RECT * rDest, int iSSource,
  RECT * rSource)
{
  if (iSDest < 0 || iSSource < 0)
    return 0;

  return ddx.surface[iSDest].g_pSurface->MyStretchBlt(rDest,
    ddx.surface[iSSource].g_pSurface, rSource);
}

extern "C" BOOL ddxRestore(int *p_CompositDC, int *p_FontDC, int *p_BackDC,
  int *p_iCompositDC, int *p_iFontDC, int *p_iBackDC,
  char *p_cBrutalRestart, AUDIO_DATA * p_ad)
{
  int i;

  /*if(key[K_S])
     {
     key[K_S] = 0;
     ddxSaveSurfaces();
     } */

  if (karmin_aktivni && timeGetTime() - dwLastMenuMusicCheck > 20000) {
    if (!ogg_playing() && !cCheckMusicExeption)
      ap_Play_Song(0, 0, p_ad);

    dwLastMenuMusicCheck = timeGetTime();
  }

  if (bLastGameState != karmin_aktivni) {
    bLastGameState = karmin_aktivni;

    if (bLastGameState) {
      if (!bWindowMenu) {
        //restoruju surfacy;
        g_pDisplay->GetDirectDraw()->RestoreAllSurfaces();

        // release vsech surfacu
        for (i = 0; i < ddx.bm_count; i++)
          if (ddx.surface[i].bLoad) {
            SAFE_DELETE(ddx.surface[i].g_pSurface);
            memset(&ddx.surface[i], 0, sizeof(SURFACESTRUCT));

            //ddx.surface[i].bLoad = 0;
          }

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

      if (!ogg_playing())
        ap_Play_Song(0, 0, p_ad);

      if (bWindowMenu) {
        BringWindowToTop(hwnd_hry);
        SetForegroundWindow(hwnd_hry);
        SetFocus(hwnd_hry);
      }

      if (!bWindowMenu)
        return TRUE;
      else
        return FALSE;
    }
    else {
      adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
      adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES, UNDEFINED_VALUE);
      ap_Stop_Song(p_ad);
    }
  }

  if (MenuCheckBossExit()) {
    gl_Kofola_End(1);
  }

  return FALSE;
}
