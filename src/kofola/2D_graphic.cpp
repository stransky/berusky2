//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "3d_all.h"
#include "Berusky_universal.h"
#include "2D_graphic.h"
#include "Apak.h"
#include "Berusky3d_ini.h"

const int NUM_OF_BITMAPS = 700;

static _2D_DATA _2dd;

extern APAK_HANDLE *pBmpArchive;
extern int iWinVer;
extern int iActualScene;

//#pragma comment(lib,"msimg32.lib")

//HMODULE g_hMSIModule = NULL; 
//LPTRANSPARENTBLT g_pTransparentBlt = NULL; 

extern char CurrentWorkingDirectory[256];

//------------------------------------------------------------------------------------------------
// init 2d
//------------------------------------------------------------------------------------------------
int _2d_Init(void)
{
  int i;

  kprintf(1, "Kofola - init bitmap -");

  _2dd.bm_count = NUM_OF_BITMAPS;
  //_2dd.hDC = GetDC(hwnd_hry);

  //kprintf(1,"GetDC() returned %d", GetLastError());

  //_2dd.hWnd = hwnd_hry;
/*
	if(!_2dd.hDC)
		kprintf(1,"Nepodarilo se nacist DC");
*/
  _2dd.bitmap =
    (BITMAPSTRUCT *) malloc((_2dd.bm_count) * sizeof(BITMAPSTRUCT));
  if (!_2dd.bitmap) {
    //MessageBox(hWnd,"Unable to allocate memory for bitmaps","Error",MB_OK);
    kprintf(1, "Unable to allocate memory for bitmaps");
    return 0;
  }

  for (i = 0; i < _2dd.bm_count; i++) {
    //_2dd.bitmap[i].bitmapDC = NULL;
    _2dd.bitmap[i].bLoad = 0;
  }

  GetPrivateProfileString("game", "bitmap_dir", "c:\\", _2dd.bm_dir, 256,
    ini_file);
  working_file_translate(_2dd.bm_dir, 256);
  return 1;
}

//------------------------------------------------------------------------------------------------
// 2d release
//------------------------------------------------------------------------------------------------
void _2d_Release(void)
{
/*
	int i;

	kprintf(1,"Kofola: - Release Bitmap...");
	for(i=0;i<_2dd.bm_count;i++)
		if(_2dd.bitmap[i].bLoad)
		{
			SelectObject(_2dd.bitmap[i].bitmapDC,_2dd.bitmap[i].old_hbitmap);	
			// Every time you select an object into a device context
    	    DeleteObject(_2dd.bitmap[i].hbitmap); 
			// Delete the HBITMAP we created -- Notice we didn't create "old_hbitmap" 
			DeleteDC(_2dd.bitmap[i].bitmapDC);
			//kprintf(1, " GetLastError() returned %d", GetLastError());
			_2dd.bitmap[i].bLoad = 0;
			_2dd.bitmap[i].bitmapDC = NULL;
		}

	if(_2dd.bitmap)
	{
		free ((void *) _2dd.bitmap);
		_2dd.bitmap = NULL;
	}

	if(_2dd.hDC)
	{
		ReleaseDC(_2dd.hWnd, _2dd.hDC);
		//kprintf(1, " ReleaseDC() returned %d", GetLastError());
		_2dd.hDC = NULL;
	}
*/
}

//------------------------------------------------------------------------------------------------
// finds first free index
//------------------------------------------------------------------------------------------------
int _2d_Find_Free_Surface(void)
{
  int i;

  for (i = 0; i < _2dd.bm_count; i++)
    if (!_2dd.bitmap[i].bLoad)
      return i;

  return -1;
}

//------------------------------------------------------------------------------------------------
// load bitmap
//------------------------------------------------------------------------------------------------
int _2d_Load_Bitmap(char *p_File_Name)
{
  //HBITMAP hbitmap;
  int pointer;

  pointer = _2d_Find_Free_Surface();

  if (pointer == -1)
    return -1;
/* TODO
	_2dd.bitmap[pointer].hbitmap = (HBITMAP)LoadImage(hinst,p_File_Name,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	
	_2dd.bitmap[pointer].bitmapDC = CreateCompatibleDC(_2dd.hDC);
	if(!_2dd.bitmap[pointer].bitmapDC)
	{
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	}

	if(!(_2dd.bitmap[pointer].old_hbitmap = 
		SelectObject(_2dd.bitmap[pointer].bitmapDC, _2dd.bitmap[pointer].hbitmap)))
	{
		DeleteDC(_2dd.bitmap[pointer].bitmapDC);
		_2dd.bitmap[pointer].bitmapDC = NULL;
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	} 
*/
/*
	GetObject(hbitmap, sizeof(BITMAP), &_2dd.bitmap[pointer].bitmap);
	DeleteObject(hbitmap);	
*/

  _2dd.bitmap[pointer].bLoad = 1;

  return pointer;
}

/*int _2d_APAK_Load_Bitmap(char *p_File_Name, char bSeek)
{
	long				size;
	APAK_FILE_HANDLE	*fHandle;
	BITMAPINFO			bmpInfo;
	HBITMAP				hDIB;
	char				*pBmp, *pDIB;
	BITMAPINFOHEADER	bmpInfoHeader;
	BITMAPFILEHEADER	bmpFileHeader;
	FILE	*file;
	int		pointer;

	pointer = _2d_Find_Free_Surface();

	if(pointer == -1)
		return -1;

	file = aopen(pBmpArchive, p_File_Name, "rb", bSeek);
	
	if(pBmpArchive->bError)
		kprintf(1,pBmpArchive->cError);

	if(!file)
		return -1;

	fHandle = (APAK_FILE_HANDLE *)file;

	aread(1, &bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file, 0);
	aread(1, &bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file, 0);

	size = bmpFileHeader.bfSize - bmpFileHeader.bfOffBits;
	pBmp = (char *) malloc(size);

	aread(1, pBmp, size, 1, file, 0);

	aclose(1, file);

	_2dd.bitmap[pointer].bitmapDC = CreateCompatibleDC(_2dd.hDC);
	kprintf(1, "CreateCompatibleDC returned %d", GetLastError());

	if(!_2dd.bitmap[pointer].bitmapDC)
	{
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	}
	
	bmpInfoHeader.biBitCount = 24;
	bmpInfo.bmiHeader = bmpInfoHeader;
	
	hDIB = CreateDIBSection(_2dd.bitmap[pointer].bitmapDC,
							&bmpInfo,
							DIB_RGB_COLORS,
							(void **)&pDIB,
							NULL, 0);
	
	SetDIBits(_2dd.bitmap[pointer].bitmapDC, hDIB, 
			  0, bmpInfoHeader.biHeight,
			  (CONST VOID *)pBmp,
			  &bmpInfo,
			  DIB_RGB_COLORS);

	if(!SelectObject(_2dd.bitmap[pointer].bitmapDC, hDIB))
		kprintf(1,"!SelectObject(hDIB) v _2d_APAK_Load_Bitmap");

	free((void *) pBmp);
	
	if(!DeleteObject(hDIB))
		kprintf(1,"!DeleteObject(hDIB) v _2d_APAK_Load_Bitmap");

	_2dd.bitmap[pointer].bitmap.bmType = 0;
	_2dd.bitmap[pointer].bitmap.bmWidth = bmpInfoHeader.biWidth;
	_2dd.bitmap[pointer].bitmap.bmHeight = bmpInfoHeader.biHeight;
	_2dd.bitmap[pointer].bLoad = 1;
	
	return pointer;
}
*/

/*int _2d_APAK_Load_Bitmap(char *p_File_Name, char bSeek)
{
	long				size;
	APAK_FILE_HANDLE	*fHandle;
	BITMAPINFO			bmpInfo;
	HBITMAP				hDIB;
	char				*pBmp, *pDIB;
	BITMAPINFOHEADER	bmpInfoHeader;
	BITMAPFILEHEADER	bmpFileHeader;
	FILE	*file;
	int		pointer;
	BITMAP				bmp;

	pointer = _2d_Find_Free_Surface();

	if(pointer == -1)
		return -1;

	file = aopen(pBmpArchive, p_File_Name, "rb", bSeek);
	
	if(pBmpArchive->bError)
		kprintf(1,pBmpArchive->cError);

	if(!file)
		return -1;

	fHandle = (APAK_FILE_HANDLE *)file;

	aread(1, &bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file, 0);
	aread(1, &bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file, 0);

	size = bmpFileHeader.bfSize - bmpFileHeader.bfOffBits;
	pBmp = (char *) malloc(size);

	aread(1, pBmp, size, 1, file, 0);

	aclose(1, file);

	_2dd.bitmap[pointer].bitmapDC = CreateCompatibleDC(_2dd.hDC);
	kprintf(1, "CreateCompatibleDC returned %d", GetLastError());

	if(!_2dd.bitmap[pointer].bitmapDC)
	{
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	}
	
	bmpInfo.bmiHeader = bmpInfoHeader;

	hDIB = CreateCompatibleBitmap(_2dd.hDC, 
								  bmpInfoHeader.biWidth,
								  bmpInfoHeader.biHeight);

	if(!hDIB)
	{
		kprintf(1, "CreateCompatibleBitmap returned %d", GetLastError());
		DeleteDC(_2dd.bitmap[pointer].bitmapDC);
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	}

	if(!SelectObject(_2dd.bitmap[pointer].bitmapDC, hDIB))
		kprintf(1,"!SelectObject(hDIB) v _2d_APAK_Load_Bitmap");

	StretchDIBits(_2dd.bitmap[pointer].bitmapDC,
				  0,
				  0,
				  bmpInfoHeader.biWidth,
				  bmpInfoHeader.biHeight,
				  0,
				  0,
				  bmpInfoHeader.biWidth,
				  bmpInfoHeader.biHeight,
				  (CONST VOID *)pBmp,
				  &bmpInfo,
				  DIB_RGB_COLORS,
				  SRCCOPY);

	free((void *) pBmp);
	
	if(!DeleteObject(hDIB))
		kprintf(1,"!DeleteObject(hDIB) v _2d_APAK_Load_Bitmap");

	_2dd.bitmap[pointer].bitmap.bmType = 0;
	_2dd.bitmap[pointer].bitmap.bmWidth = bmpInfoHeader.biWidth;
	_2dd.bitmap[pointer].bitmap.bmHeight = bmpInfoHeader.biHeight;
	_2dd.bitmap[pointer].bLoad = 1;
	
	return pointer;
}
*/

int _2d_APAK_Load_Bitmap(char *p_File_Name, APAK_HANDLE * pHandle)
{
/*
	long				size;
	APAK_FILE_HANDLE	*fHandle;
	BITMAPINFO			bmpInfo;
	//HBITMAP				hDIB;
	char				*pBmp;
	BITMAPINFOHEADER	bmpInfoHeader;
	BITMAPFILEHEADER	bmpFileHeader;
	FILE	*file;
	int		pointer;
	pointer = _2d_Find_Free_Surface();

	if(pointer == -1)
		return -1;

	file = aopen(pHandle, p_File_Name, "rb");
	
	if(pBmpArchive->bError)
		kprintf(1,"%s: %s", p_File_Name, pBmpArchive->cError);

	if(!file)
		return -1;

	fHandle = (APAK_FILE_HANDLE *)file;

	aread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	aread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	size = bmpFileHeader.bfSize - bmpFileHeader.bfOffBits;
	pBmp = (char *) malloc(size);

	aread(pBmp, size, 1, file);

	aclose(file);

	_2dd.bitmap[pointer].bitmapDC = CreateCompatibleDC(_2dd.hDC);
	//kprintf(1, "CreateCompatibleDC returned %d", GetLastError());

	if(!_2dd.bitmap[pointer].bitmapDC)
	{
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	}
	
	bmpInfo.bmiHeader = bmpInfoHeader;

	_2dd.bitmap[pointer].hbitmap = CreateCompatibleBitmap(_2dd.hDC, 
														  bmpInfoHeader.biWidth,
													      bmpInfoHeader.biHeight);

	if(!_2dd.bitmap[pointer].hbitmap)
	{
		kprintf(1, "CreateCompatibleBitmap returned %d", GetLastError());
		DeleteDC(_2dd.bitmap[pointer].bitmapDC);
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	}

	if(!(_2dd.bitmap[pointer].old_hbitmap = 
		SelectObject(_2dd.bitmap[pointer].bitmapDC, _2dd.bitmap[pointer].hbitmap)))
	{
		DeleteDC(_2dd.bitmap[pointer].bitmapDC);
		DeleteObject(_2dd.bitmap[pointer].hbitmap);
		_2dd.bitmap[pointer].bitmapDC = NULL;
		_2dd.bitmap[pointer].bLoad = 0;
		kprintf(1,"Can't load %s",p_File_Name);
		return -1;
	} 

	SetDIBits(_2dd.bitmap[pointer].bitmapDC, 
			  _2dd.bitmap[pointer].hbitmap, 
			  0, bmpInfoHeader.biHeight,
			  (CONST VOID *)pBmp,
			  &bmpInfo,
			  DIB_RGB_COLORS);

	free((void *) pBmp);
*/
/*	if(!DeleteObject(hDIB))
		kprintf(1,"!DeleteObject(hDIB) v _2d_APAK_Load_Bitmap");*/
/*
	_2dd.bitmap[pointer].bitmap.bmType = 0;
	_2dd.bitmap[pointer].bitmap.bmWidth = bmpInfoHeader.biWidth;
	_2dd.bitmap[pointer].bitmap.bmHeight = bmpInfoHeader.biHeight;
	_2dd.bitmap[pointer].bLoad = 1;
  
	return pointer;
*/
  assert(0);
}


/*
void lsi_Make_Screenshot(HDC hdc, char *pFile)
{
	HDC					hdcMem;
	BITMAPINFO			*pbmiRGB = (BITMAPINFO *) NULL; 
	HBITMAP				hbmRGB  = (HBITMAP) NULL;
	PBYTE				pjBitsRGB;
	BITMAPFILEHEADER	bmpHeader;
	FILE				*bmpFile = NULL;

	hdcMem  = CreateCompatibleDC(hdc);

	pbmiRGB = (BITMAPINFO *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, sizeof(BITMAPINFO) );
	
	if (!pbmiRGB)
		MessageBox(NULL,"!pbmiRGB","!!!", MB_OK);
	
	pbmiRGB->bmiHeader.biSize	       = sizeof(BITMAPINFOHEADER);     
	pbmiRGB->bmiHeader.biWidth		   = Xresolution;     
	pbmiRGB->bmiHeader.biHeight        = Yresolution;     
	pbmiRGB->bmiHeader.biPlanes        = 1;     
	pbmiRGB->bmiHeader.biBitCount      = 24;     
	pbmiRGB->bmiHeader.biCompression   = BI_RGB;     
	pbmiRGB->bmiHeader.biSizeImage     = pbmiRGB->bmiHeader.biWidth
										 * abs(pbmiRGB->bmiHeader.biHeight) * 3; 

	hbmRGB = CreateDIBSection(hdcMem, pbmiRGB, DIB_RGB_COLORS,
							  (PVOID *) &pjBitsRGB, NULL, 0);
	
	if (!hbmRGB) 
		MessageBox(NULL,"!hbmRGB","!!!", MB_OK);

	if (!SelectObject(hdcMem, hbmRGB))
		MessageBox(NULL,"!SelectObject(hdcMem, hbmRGB)","!!!", MB_OK);

  
	if (!BitBlt(hdcMem, 0,0, Xresolution, Yresolution, hdc, 0,0,SRCCOPY)) 
		MessageBox(NULL,"!BitBlt","!!!", MB_OK);

	pjBitsRGB = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbmiRGB->bmiHeader.biSizeImage);

	if (!pjBitsRGB) 
		MessageBox(NULL,"!pjBitsRGB","!!!", MB_OK);

	if (!GetDIBits(hdcMem, hbmRGB, 0, (UINT)pbmiRGB->bmiHeader.biHeight, pjBitsRGB, 
				   (LPBITMAPINFO)pbmiRGB, DIB_RGB_COLORS))
		MessageBox(NULL,"!GetDIBits","!!!", MB_OK);
		
	bmpHeader.bfType = 'MB';
	bmpHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + pbmiRGB->bmiHeader.biSizeImage;
	bmpHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;

	bmpFile = fopen(pFile,"wb");

	if (!bmpFile)
		MessageBox(NULL,"!bmpFile","!!!", MB_OK);

	fwrite(&bmpHeader,sizeof(BITMAPFILEHEADER),1,bmpFile);
	fwrite(&pbmiRGB->bmiHeader,sizeof(BITMAPINFOHEADER),1,bmpFile);
	fwrite(pjBitsRGB,sizeof(BYTE),pbmiRGB->bmiHeader.biSizeImage,bmpFile);

	fclose(bmpFile);

	DeleteDC(hdcMem);          
	DeleteObject(hbmRGB);
	LocalFree(pbmiRGB);
	GlobalFree((HGLOBAL)pjBitsRGB);
}
*/

//------------------------------------------------------------------------------------------------
// release bitmap
//------------------------------------------------------------------------------------------------
int _2d_Release_Bitmap(int pointer)
{
/*
	if(pointer < 0)
		return 1;

	if(pointer > (_2dd.bm_count - 1))
		return 0;

	if(_2dd.bitmap[pointer].bLoad)
	{
		SelectObject(_2dd.bitmap[pointer].bitmapDC,_2dd.bitmap[pointer].old_hbitmap);	
		// Every time you select an object into a device context
    	DeleteObject(_2dd.bitmap[pointer].hbitmap); 
		// Delete the HBITMAP we created -- Notice we didn't create "old_hbitmap" 
		DeleteDC(_2dd.bitmap[pointer].bitmapDC);
		_2dd.bitmap[pointer].bLoad = 0;
		_2dd.bitmap[pointer].bitmapDC = NULL;
	}
*/
  return 1;
}

//------------------------------------------------------------------------------------------------
// load list of bitmaps
//------------------------------------------------------------------------------------------------
int _2d_Load_List(char *p_File_Name)
{
  char text[256];
  FILE *file = 0;

  chdir((_2dd.bm_dir));

  file = fopen(p_File_Name, "r");

  kprintf(1, "Kofola: - Load bitmap pro herni menu");

  if (!file) {
    //MessageBox(_3dd.hWnd,"File not found","Error",MB_OK);
    kprintf(1, "File not found : %s", p_File_Name);
    return 0;
  }
  else {
    while (!feof(file)) {
      fgets(text, 256, file);
      if (!feof(file)) {
        newline_cut(text);
        _2d_Load_Bitmap(text);
        //kprintf(1,"Bitmap %s loaded.",text);
      }
    }
    fclose(file);

    return 1;
  }
}

int _2d_APAK_Load_List(char *p_File_Name)
{
  char text[256] = "";
  FILE *file = 0;
  DWORD Eplased = 0;
  DWORD Start, Stop;

  achdir(pBmpArchive, _2dd.bm_dir);

  file = aopen(pBmpArchive, p_File_Name, "rb");

  kprintf(1, "Kofola: - Load bitmap pro herni menu");

  if (!file) {
    //MessageBox(_3dd.hWnd,"File not found","Error",MB_OK);
    kprintf(1, "File not found : %s", p_File_Name);
    return 0;
  }
  else {
    Start = timeGetTime();

    while (!aeof(file)) {
      if (agets(text, 256, file) && !aeof(file)) {
        newline_cut(text);
        _2d_APAK_Load_Bitmap(text, pBmpArchive);
        //kprintf(1,"Bitmap %s loaded.",text);
      }
    }

    aclose(file);

    Stop = timeGetTime();
    Eplased = Stop - Start;
    kprintf(1, "--------------Total load time %.1f s -----------------",
      Eplased / 1000.0f);

    return 1;
  }

/*	DWORD	Eplased = 0;
	DWORD	Start, Stop;
	int		c = 0, i;
	TEXT_STRUCT	*pText;
	char	text[256];
	FILE	*file = 0;

	_2d_Init_Load_Progress_Bar(353);

	achdir(pBmpArchive, _2dd.bm_dir);

	if(pBmpArchive->bError)
		kprintf(1, "Change dir: %s", pBmpArchive->cError);

	file = aopen(pBmpArchive, p_File_Name,"rb");

	kprintf(1,"Kofola: - Load bitmap pro herni menu");

	if(!file)
	{
		//MessageBox(_3dd.hWnd,"File not found","Error",MB_OK);
		kprintf(1,"File not found : %s",p_File_Name);
		return 0;
	}

	do
	{
		agets(text, 256, file);
		c++;
	}
	while(!aeof(file));

	_2d_Add_Progress(-1);
	_2d_Draw_Progress(1024,768);

	aseek(file, 0, SEEK_SET);	
	
	_2d_Add_Progress(-1);
	_2d_Draw_Progress(1024,768);

	pText = (TEXT_STRUCT *) malloc(c * sizeof(TEXT_STRUCT));

	if(!pText)
	{
		aclose(file);
		return 0;
	}

	c = 0;

	do
	{
		agets(text, 256, file);
		text[strlen(text)-2] = '\0';
		
		strcpy(pText[c].cText, text);
		c++;
	}
	while(!aeof(file));

	aclose(file);

	_2d_Add_Progress(-1);
	_2d_Draw_Progress(1024,768);


	for(i=0;i<c;i++)
	{
		Start = timeGetTime();

		if(!i)
		{
			if(_2d_APAK_Load_Bitmap(pText[i].cText, 1) == -1)
			{
				kprintf(1,"Bitmap %s NOT loaded!",pText[i].cText);
			}
		}
		else
		{
			if(_2d_APAK_Load_Bitmap(pText[i].cText, 0) == -1)
			{
				kprintf(1,"Bitmap %s NOT loaded!",pText[i].cText);
			}
		}

		Stop = timeGetTime();

		//kprintf(1, "%s loaded in %d ms", pText[i].cText, Stop - Start);

		Eplased += (Stop - Start);

		_2d_Add_Progress(-1);
		_2d_Draw_Progress(1024,768);

	}

	free((void *)pText);

	kprintf(1, "--------------Total load time %.1f s -----------------", Eplased / 1000.0f);
	return 1;*/
}

void _2d_Draw_Load_Screen(void)
{
  int i;

  //schovej_konzoli();

  _2d_Init();
  _2d_Blackness();
  chdir((_2dd.bm_dir));
  achdir(pBmpArchive, _2dd.bm_dir);

  if (iActualScene < 13) {
    char text[256];

    sprintf(text, "scene%d.bmp", iActualScene);

    i = _2d_APAK_Load_Bitmap(text, pBmpArchive);
  }
  else
    i = _2d_APAK_Load_Bitmap("LoadScreen.bmp", pBmpArchive);
/*
	SetStretchBltMode(_2dd.hDC, HALFTONE);
	//SetBrushOrgEx(_2dd.hDC, 0, 0, NULL);
	StretchBlt(_2dd.hDC, 0, 0, hwconf.xres, hwconf.yres, _2dd.bitmap[i].bitmapDC,0,0,1024, 768, SRCCOPY);
*/
  _2d_Release_Bitmap(i);
  //_2dd.ProgressBmp = _2d_Load_Bitmap("LoadScreen3.bmp");

  //_2d_Release();
}

void _2d_Init_Load_Progress_Bar(int iNumOfItems)
{
  _2dd.ProgressStatus = 0;
  _2dd.ProgressPlus = 100.0f / (float) iNumOfItems;
}

void _2d_Add_Progress(float fPercent)
{
  if (fPercent >= 0)
    _2dd.ProgressStatus += fPercent;
  else
    _2dd.ProgressStatus += _2dd.ProgressPlus;

  if(_2dd.ProgressStatus > 100.0f)
    _2dd.ProgressStatus = 100.0f;  
}

void _2d_Draw_Progress(int x, int y)
{
  assert(0);
/*
	HPEN	LastPen;
	HPEN	MePen;
	int i;
	int Width;
	int RWidth;

	Width = ftoi((1024 * _2dd.ProgressStatus) / 100.0f);

	if(x < 0 || y < 0)
		RWidth = ftoi((hwconf.xres * _2dd.ProgressStatus) / 100.0f);
	else
		RWidth = ftoi((x * _2dd.ProgressStatus) / 100.0f);
	
	//SetStretchBltMode(_2dd.hDC, COLORONCOLOR);
	//StretchBlt(_2dd.hDC, 0, 0, RWidth, hwconf.yres, 
			   //_2dd.bitmap[_2dd.ProgressBmp].bitmapDC,0,0,Width, 768, SRCCOPY);

	MePen = CreatePen(PS_SOLID,1,RGB(255,0,0));
	LastPen = (HPEN)SelectObject(_2dd.hDC,MePen);

	if(x < 0 || y < 0)
		for(i=hwconf.yres - 5;i<hwconf.yres;i++)
		{
			MoveToEx(_2dd.hDC,0, i, NULL);
			LineTo(_2dd.hDC,RWidth, i);
		}
	else
		for(i=y - 5;i<y;i++)
		{
			MoveToEx(_2dd.hDC,0, i, NULL);
			LineTo(_2dd.hDC,RWidth, i);
		}

	SelectObject(_2dd.hDC,LastPen);
	DeleteObject(MePen);
  */
}

void _2d_Release_Progress(void)
{
  _2d_Release();
}

void _2d_Blackness(void)
{
  assert(0);
//  BitBlt(_2dd.hDC, 0, 0, hwconf.xres, hwconf.yres, NULL, 0, 0, BLACKNESS);
}

void _2d_Clear_RectLine(RECT_LINE * p_rl)
{
  memset(p_rl, 0, sizeof(RECT_LINE));
  p_rl->rlast = 0;
}

int _2d_Is_InRectLine(RECT * rline, RECT * p_r, int size)
{
  int i;

  for (i = 0; i < size; i++) {
    if (rline[i].bottom == p_r->bottom &&
        rline[i].left == p_r->left &&
        rline[i].right == p_r->right && 
        rline[i].top == p_r->top)
    {
      return 1;
    }
  }

  return 0;
}

void _2d_Add_RectItem_IfNPresent(RECT_LINE * p_rl, RECT rect, int iLayer)
{
  int i;

  for (i = 0; i < p_rl->rlast; i++)
    if (p_rl->rect[i].rect.bottom == rect.bottom &&
      p_rl->rect[i].rect.left == rect.left &&
      p_rl->rect[i].rect.right == rect.right &&
      p_rl->rect[i].rect.top == rect.top)
      return;

  if (p_rl->rlast < DRAW_RECT_NUM) {
    p_rl->rect[p_rl->rlast].rect = rect;
    p_rl->rect[p_rl->rlast].iLayer = iLayer;
    p_rl->rect[p_rl->rlast].bUsed = 1;

    p_rl->rlast++;
  }
}

void _2d_Add_RectItem(RECT_LINE * p_rl, RECT rect, int iLayer)
{
  if (p_rl->rlast < DRAW_RECT_NUM) {
    p_rl->rect[p_rl->rlast].rect = rect;
    p_rl->rect[p_rl->rlast].iLayer = iLayer;
    p_rl->rect[p_rl->rlast].bUsed = 1;

    p_rl->rlast++;
  }
}

/*
BOOL TransparentBltU98(
     HDC dcDest,         // handle to Dest DC
     int nXOriginDest,   // x-coord of destination upper-left corner
     int nYOriginDest,   // y-coord of destination upper-left corner
     int nWidthDest,     // width of destination rectangle
     int nHeightDest,    // height of destination rectangle
     HDC dcSrc,          // handle to source DC
     int nXOriginSrc,    // x-coord of source upper-left corner
     int nYOriginSrc,    // y-coord of source upper-left corner
     int nWidthSrc,      // width of source rectangle
     int nHeightSrc,     // height of source rectangle
     UINT crTransparent  // color to make transparent
  )
{
	HDC dc;
	HBITMAP bitmap;
	HBITMAP oldBitmap;
	HDC maskDC;
	HBITMAP maskBitmap;
	HBITMAP oldMask;
	HDC newMaskDC;
	HBITMAP newMask;
	HBITMAP oldNewMask;
	HDC newImageDC;
	HBITMAP newImage;
	HBITMAP oldNewImage;
	HDC	tmpImageDC;
	HBITMAP tmpImage;
	HBITMAP oldTmpImage;

     if (nWidthDest < 1) return FALSE;
     if (nWidthSrc < 1) return FALSE; 
     if (nHeightDest < 1) return FALSE;
     if (nHeightSrc < 1) return FALSE;

	 tmpImageDC = CreateCompatibleDC(_2dd.hDC);
     tmpImage = CreateBitmap(nWidthSrc, nHeightSrc, 1, GetDeviceCaps(tmpImageDC, BITSPIXEL), NULL);

     if (tmpImage == NULL)
         return FALSE;

     oldTmpImage = (HBITMAP)SelectObject(tmpImageDC, tmpImage);
	 BitBlt(tmpImageDC, 0, 0, nWidthDest, nHeightDest, dcDest, nXOriginDest, nYOriginDest, SRCCOPY);

     dc = CreateCompatibleDC(_2dd.hDC);
     bitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, GetDeviceCaps(dc, BITSPIXEL), NULL);

     if (bitmap == NULL)
         return FALSE;

     oldBitmap = (HBITMAP)SelectObject(dc, bitmap);

     if (!BitBlt(dc, 0, 0, nWidthSrc, nHeightSrc, dcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY))
         return FALSE;

     maskDC = CreateCompatibleDC(_2dd.hDC);
     maskBitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, 1, NULL);

     if (maskBitmap == NULL)
         return FALSE;

     oldMask =  (HBITMAP)SelectObject(maskDC, maskBitmap);
 
     SetBkColor(maskDC, RGB(0,0,0));
     SetTextColor(maskDC, RGB(255,255,255));
     if (!BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,NULL,0,0,BLACKNESS))
         return FALSE;

     SetBkColor(dc, crTransparent);
     BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,dc,0,0,SRCINVERT);

     SetBkColor(dc, RGB(0,0,0));
     SetTextColor(dc, RGB(255,255,255));
     BitBlt(dc, 0,0,nWidthSrc,nHeightSrc,maskDC,0,0,SRCAND); 

     newMaskDC = CreateCompatibleDC(_2dd.hDC);

     newMask = CreateBitmap(nWidthDest, nHeightDest, 1, GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newMask == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         SelectObject(maskDC, oldMask);
         DeleteDC(maskDC);
         DeleteDC(newMaskDC);
         return FALSE;
     }

     SetStretchBltMode(newMaskDC, COLORONCOLOR);
     oldNewMask = (HBITMAP) SelectObject(newMaskDC, newMask);

     StretchBlt(newMaskDC, 0, 0, nWidthDest, nHeightDest, maskDC, 0, 0, nWidthSrc, nHeightSrc, SRCCOPY);

     SelectObject(maskDC, oldMask);
     DeleteDC(maskDC);
    
     newImageDC = CreateCompatibleDC(_2dd.hDC);
     newImage = CreateBitmap(nWidthDest, nHeightDest, 1, GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newImage == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         DeleteDC(newMaskDC);
         return FALSE;
     }

     oldNewImage = (HBITMAP)SelectObject(newImageDC, newImage);
     
	 StretchBlt(newImageDC, 0, 0, nWidthDest, nHeightDest, dc, 0, 0, nWidthSrc, nHeightSrc, SRCCOPY);

     SelectObject(dc, oldBitmap);
     DeleteDC(dc);

     BitBlt( tmpImageDC, 0, 0, nWidthDest, nHeightDest, newMaskDC, 0, 0, SRCAND);

     BitBlt( tmpImageDC, 0, 0, nWidthDest, nHeightDest, newImageDC, 0, 0, SRCPAINT);

	 BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, tmpImageDC, 0, 0, SRCCOPY);

     //BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, newMaskDC, 0, 0, SRCAND);

     //BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, newImageDC, 0, 0, SRCPAINT);
 
	 SelectObject(tmpImageDC, oldTmpImage);
	 DeleteDC(tmpImageDC);
     SelectObject(newImageDC, oldNewImage);
     DeleteDC(newImageDC);
     SelectObject(newMaskDC, oldNewMask);
     DeleteDC(newMaskDC);
 
	 DeleteObject(tmpImage);
	 DeleteObject(maskBitmap);
	 DeleteObject(bitmap);
	 DeleteObject(newMask);
	 DeleteObject(newImage);

     return TRUE;
}
*/
/*
void _2d_Fill_Rect(RECT rect, COLORREF color)
{
	HBRUSH hbr = CreateSolidBrush(color);

	FillRect(hdc, &rect, hbr);

	DeleteObject(hbr);
}

BOOL BitBltU(
  HDC hdcDest, // handle to destination DC
  int nXDest,  // x-coord of destination upper-left corner
  int nYDest,  // y-coord of destination upper-left corner
  int nWidth,  // width of destination rectangle
  int nHeight, // height of destination rectangle
  HDC hdcSrc,  // handle to source DC
  int nXSrc,   // x-coordinate of source upper-left corner
  int nYSrc,   // y-coordinate of source upper-left corner
  DWORD dwRop  // raster operation code
)
{
	RECT r;

	r.left = nXDest - 1;
	r.top = nYDest - 1;
	r.right = nXDest + nWidth + 1;
	r.bottom = nYDest + nHeight + 1;
		
	_2d_Fill_Rect(hdcDest, r, RGB(255, 0, 255));

	return TRUE;
}
*/

int LoadTransparentBlt(void)
{
/*	char lib[1024];
	// I try to locate the function 
	// Do this early in your program 

	strcpy(lib, CurrentWorkingDirectory);
	strcat(lib, "\\");
	strcat(lib, "Msimg32.dll");

	g_hMSIModule = LoadLibrary(lib); 

	if (g_hMSIModule) 
	{ 
		g_pTransparentBlt = (LPTRANSPARENTBLT) GetProcAddress(g_hMSIModule, "TransparentBlt"); 
	}
	else
	{
		kprintf(1, "Unable to load Msimg32.dll");
		return 0;
	}

// if the pointer is valid, you may use it 
// see Documentation of this function - not hard to use 
	if(g_pTransparentBlt)
	{
		kprintf(1, "TransparentBlt succesfully wraped");
		return 1;
	}
	else
	{
		kprintf(1, "Unable to wrap TransparentBlt");
		return 0;
	}*/

  return 1;
}

void UnloadTransparentBlt(void)
{
/*	if(g_hMSIModule) 
	  FreeLibrary(g_hMSIModule);*/
}
