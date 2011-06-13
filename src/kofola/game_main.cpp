//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <time.h>
#include "game_init.h"
#include "game_logic.h"
#include <math.h>
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "2D_graphic.h"
#include "3D_graphic.h"
#include "3D_menus.h"
#include "menu.h"
#include "Comics.h"
#include "font.h"
#include "Apak.h"
#include "profiles.h"
#include "Menu.h"
#include "Menu2.h"

APAK_HANDLE		*pBmpArchive = NULL;
APAK_HANDLE		*pControlsArchive = NULL;
APAK_HANDLE		*p3DMArchive = NULL;
APAK_HANDLE		*pSndArchive = NULL;
APAK_HANDLE		*pDataArchive = NULL;
APAK_HANDLE		*pGDataArchive = NULL;

char			CurrentWorkingDirectory[256];
int				iMaxBpp = 32;
int				iMaxFreq = 0;
int				bWindowMenu;
int				iLanguageVersion;

extern int				iWinVer;
extern HINT_TEXTURE		pHintTexture[26];
extern PLAYER_PROFILE	pPlayerProfile;
extern AUDIO_DATA			ad;
extern int			iActualScene;
extern int			iActualLevel;
char				bDXAktivni = 0;

char cFontFile[5][64];

void winmain_Test(void)
{
	DWORD s, f, e;
	FILE	*file;

	s = timeGetTime();

	file = aopen(pBmpArchive, "d:\\Berusky II\\bitmap\\LoadScreen.bmp","rb");
	
	f = timeGetTime();
	e = f - s;
}

int winmain_Check_Window_Menu(void)
{
/*
	DEVMODE dmSettings;

	if(!GetPrivateProfileInt("hra","fullscreen", 1, (const char *) ini_file))
	{
		memset(&dmSettings,0,sizeof(dmSettings));
		dmSettings.dmSize = sizeof(DEVMODE);

		if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings))
			return 1;
	
		if(dmSettings.dmPelsWidth < 1024 || dmSettings.dmPelsHeight < 768)  
		{
			MessageBox(NULL, "Desktop resolution must be at least 1024x768.", "Error", MB_OK);
			return 0;
		}
	}
*/
	return 1;
}

/*
int winmain_Check_Window_Menu(void)
{
	DEVMODE dmSettings;

	if(GetPrivateProfileInt("hra","windowmenu", 0, (const char *) ini_file))
		WritePrivateProfileString("hra","fullscreen","0",ini_file);

	if(!GetPrivateProfileInt("hra","fullscreen", 0, (const char *) ini_file))
		WritePrivateProfileString("hra","windowmenu","1",ini_file);

	if(GetPrivateProfileInt("hra","fullscreen", 0, (const char *) ini_file))
	{
		memset(&dmSettings,0,sizeof(dmSettings));
		dmSettings.dmSize = sizeof(DEVMODE);
		
		if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings))
			return 1;

		if(dmSettings.dmPelsWidth < 1024 || dmSettings.dmPelsHeight < 768 &&
		   GetPrivateProfileInt("hra","windowmenu", 0, (const char *) ini_file))
		{
			MessageBox(NULL, "Desktop resolution must be at least 1024x768.", "Error", MB_OK);
			return 0;
		}
	}

	return 1;
}*/

//------------------------------------------------------------------------------------------------
// kostra behu hry
//------------------------------------------------------------------------------------------------
int	winmain_Game_Run(char *p_Level_Name)
{
	char	dir[256];
	unsigned int Timer_ID;
	int		cpu;
	char	bGame = strlen(p_Level_Name);
	char	bConsole = GetPrivateProfileInt("debug","start-konzole",0,ini_file);

	cpu = sizeof(AUDIO_DATA);

	kprintf(1,"Kofola - verze zdrojaku: MASTER %d.%d", VERZEHI, VERZELO);

	if(!winmain_Check_Window_Menu())
		return 0;

	bWindowMenu = !GetPrivateProfileInt("hra","fullscreen", 1, ini_file);

	gi_Set_Win_Version();
	getcwd(CurrentWorkingDirectory, 256);

	srand( (unsigned) time( NULL ) );

	GetPrivateProfileString("game","bitmap_pak","c:\\",dir,256,ini_file);
  working_file_translate(dir,256);

	GetPrivateProfileString("game","font_pak1","c:\\",cFontFile[0],64,ini_file);
	kprintf(1, "font_pak1 = %s", cFontFile[0]);

	GetPrivateProfileString("game","font_pak2","c:\\",cFontFile[1],64,ini_file);
	kprintf(1, "font_pak2 = %s", cFontFile[1]);

	GetPrivateProfileString("game","font_pak3","c:\\",cFontFile[2],64,ini_file);
	kprintf(1, "font_pak3 = %s", cFontFile[2]);

	GetPrivateProfileString("game","font_pak4","c:\\",cFontFile[3],64,ini_file);
	kprintf(1, "font_pak4 = %s", cFontFile[3]);

	GetPrivateProfileString("game","font_pak5","c:\\",cFontFile[4],64,ini_file);
	kprintf(1, "font_pak5 = %s", cFontFile[4]);

	iLanguageVersion = GetPrivateProfileInt("game","languageid", 0, ini_file);
	kprintf(1, "Language ID = %d", iLanguageVersion);

	if(!gi_Open_Archive(dir, &pBmpArchive,"game","bitmap_dir"))
	{
		return 0;
	}

#ifndef __DEMO
	if(!gi_Open_Archive("bitmap.pak", &pBmpArchive,"game","bitmap_dir"))
	{
		return 0;
	}
#endif

#ifdef __DEMO
	if(!gi_Open_Archive("bitmap_demo.pak", &pBmpArchive,"game","bitmap_dir"))
	{
		return 0;
	}
#endif

	if(!gi_Open_Archive("controls.pak", &pControlsArchive,"game","bitmap_dir"))
	{
		apakclose(pBmpArchive);
		return 0;
	}
/*
#ifndef __DEMO
	if(!gi_Open_Archive("sound.pak", &pSndArchive,"soundengine","sound_dir"))
	{
		apakclose(pBmpArchive);
		apakclose(pControlsArchive);
		return 0;
	}
#endif

#ifdef __DEMO
	if(!gi_Open_Archive("sound_demo.pak", &pSndArchive,"soundengine","sound_dir"))
	{
		apakclose(pBmpArchive);
		apakclose(pControlsArchive);
		return 0;
	}
#endif
*/
	GetPrivateProfileString("game","3dmenu_pak","c:\\",dir,256,ini_file);
  working_file_translate(dir,256);

	if(!gi_Open_Archive(dir, &p3DMArchive,"game","bitmap_dir"))
	{
		apakclose(pControlsArchive);
		apakclose(pBmpArchive);
		apakclose(pSndArchive);
		return 0;
	}

	GetPrivateProfileString("game","data_pak","c:\\",dir,256,ini_file);
  working_file_translate(dir,256);

	if(!gi_Open_Archive(dir, &pDataArchive,"game","data_dir"))
	{
		apakclose(p3DMArchive);
		apakclose(pControlsArchive);
		apakclose(pBmpArchive);
		apakclose(pSndArchive);
		return 0;
	}

#ifndef __DEMO
	if(!gi_Open_Archive("data.pak", &pDataArchive,"game","data_dir"))
	{
		apakclose(p3DMArchive);
		apakclose(pControlsArchive);
		apakclose(pBmpArchive);
		apakclose(pSndArchive);
		return 0;
	}
#endif

#ifdef __DEMO
	if(!gi_Open_Archive("data_demo.pak", &pDataArchive,"game","data_dir"))
	{
		apakclose(p3DMArchive);
		apakclose(pControlsArchive);
		apakclose(pBmpArchive);
		apakclose(pSndArchive);
		return 0;
	}
#endif

#ifndef __DEMO
	if(!gi_Open_Archive("game_data.pak", &pGDataArchive,"game","game_data_dir"))
	{
		apakclose(pDataArchive);
		apakclose(p3DMArchive);
		apakclose(pControlsArchive);
		apakclose(pBmpArchive);
		apakclose(pSndArchive);
		return 0;
	}
#endif

#ifdef __DEMO
	if(!gi_Open_Archive("game_data_demo.pak", &pGDataArchive,"game","game_data_dir"))
	{
		apakclose(pDataArchive);
		apakclose(p3DMArchive);
		apakclose(pControlsArchive);
		apakclose(pBmpArchive);
		apakclose(pSndArchive);
		return 0;
	}
#endif

//	cpu = (int)ceil(tools_Base_Priority(tools_Get_CPU_Speed()));

	kprintf(1,"OGG Decompression Thread Priority: %d",cpu);
/*
	gi_Init_Sound_Engine(&ad);
	chdir(ad.Music_Dir);
	ap_Load_Play_List("Play_List.dat",&ad);
	chdir(ad.Sound_Dir);
*/
	ap_Load_Material_List("Material.dat",&ad);
	//ap_Play_Song(22,0,&ad);

	/*ogg_open("c:\\AnakreoN\\Berusky II\\Music\\Freemind.ogg");

   	if(!ogg_playback())
           return 0;
                    
    while(ogg_playing())
    {
		Sleep(10);
    }
                    
	ogg_release();*/

	iMaxBpp = gi_Get_Max_Resolution_Bpp(1024,768);
	iMaxFreq = gi_Get_Max_Freq(1024,768, iMaxBpp);

	kprintf(1, "iMaxBpp = %d , iMaxFreq = %d", iMaxBpp, iMaxFreq);

	if(!bGame)
	{
#ifndef __DEMO

		if(GetPrivateProfileInt("hra","logo",1,ini_file) && !bWindowMenu && iLanguageVersion != 4)
		{
			cmcs_Game_Down();
			cmcs_Play_Video("AnakreoN.mpg", 16000, &ad);
			cmcs_Game_Up();
		}
#endif
/*
		if(FAILED(InitDirectDraw(1024, 768, GetPrivateProfileInt("hra", "menu_bpp", 16, ini_file))))
			return 0;
*/		
		spracuj_spravy(0);
/*
		if(!bWindowMenu)
			ShowWindow(SW_MAXIMIZE);
		else		
			ShowWindow(SW_SHOWNORMAL);
*/  
		//maximalizuj_okno(hwnd_hry);
	}

	if(bGame)
	{
		if(!kom_graf_init())
		{
			kprintf(1, "kom_graf_init -  nepodarilo se nahodit");
			exit(0);
		}

		//kom_prvnotni_init();
		
		//_2d_Init(hWnd);

		//fn_Convert_Rect("", 1024, 1024);
		
		//_2d_Release();

		iActualScene = 0;
		RunMenuLoadScreen2();
		RunMenuLoadScreenInitBar(15);
		RunMenuLoadScreenAddProgress(-1);
		RunMenuLoadScreenDrawProgress(-1,-1);
		_3d_Init();
		_3d_Load_List("3D_load.dat");

		//_3d_Gen_Hints(pHintTexture, 26);

		if(!fn_Set_Font("font3d.pak"))
		{
			kprintf(1, "Unable to set font!");
			return 0;
		}

		if(!fn_Load_Bitmaps())
			kprintf(1,"Unable to load font bitmaps");

		RunMenuLoadScreenAddProgress(-1);
		RunMenuLoadScreenDrawProgress(-1,-1);
	}

	if(!bGame)
	{
		//schovej_konzoli();
		SetCursor(NULL);
		ddxInit();
	
		//Sleep(2000);
		RunMenuCinemax();

		ap_Play_Song(0,0,&ad);

		ddxLoadList("2d_load.dat", 1);
		
		if(!fn_Set_Font(cFontFile[0]))
		{
			//MessageBox("Unable to set font!", "Error", MB_OK);
			return 0;
		}

		if(!fn_Load_Bitmaps())
			kprintf(1,"Unable to load font bitmaps");

	}

	//adas_OGG_Set_Priority(cpu);

	if(bGame)
	{
		pr_ReadProfile("Default", &pPlayerProfile);

		SetCursor(NULL);
		Timer_ID = SetTimer(NULL, 0, 250, (TIMERPROC)gl_Set_Frame_Rate);
		_3d_Load_Indikace();		
		iActualScene = 0;
		gl_Run_Level(p_Level_Name,"default.env",&ad, cpu);
		_3d_Release_Hints(pHintTexture, 26);
		KillTimer(NULL,Timer_ID);
	}
	else
	{
		//fn_Convert_Rect("", 768, 576);
		SetCursor(NULL);
		ShowCursor(FALSE);
		//RunMenu("mainmenu.txt", hwnd_hry, &ad, cpu);

#ifdef __DEMO
		{
			int i = GetPrivateProfileInt("game","languageid", 0, ini_file);

			if(i != 2 && i != 4)
				RunMenuDrawDemoEndScreen();
		}
#endif
	}

	if(ogg_playing())
		ap_Stop_Song(&ad);

	if(!bGame)
	{
		/*_2d_Blackness();
		_2d_Release();*/
		ddxRelease();
		//FreeDirectDraw();
	}
	else
	{
		//fn_Release_Font();
		_3d_Release();
		kom_graf_konec(0);		
	}

    //kprintf(1, "cmcs_Play_Intro");
	//cmcs_Play_Intro("gamelogo.txt", hWnd, &ad);

	kprintf(1, "apakclose pGDataArchive");
	apakclose(pGDataArchive);
	kprintf(1, "apakclose pDataArchive");
	apakclose(pDataArchive);
	kprintf(1, "apakclose pBmpArchive");
	apakclose(pBmpArchive);
	kprintf(1, "apakclose pSndArchive");
	apakclose(pSndArchive);
	kprintf(1, "apakclose p3DMArchive");
	apakclose(p3DMArchive);
 	kprintf(1, "apakclose pControlsArchive");
	apakclose(pControlsArchive);
	
	//kprintf(1, "apakclose FONT");
	//fn_Release_Font();

	kprintf(1, "gi_Release_Sound_Engine");
	gi_Release_Sound_Engine();
	kprintf(1, "ap_Release_Play_List");
	ap_Release_Play_List(&ad);
	kprintf(1, "ap_Release_Material_List");
	ap_Release_Material_List(&ad);
	kprintf(1, "ap_Release");
	ap_Release(&ad);

	//ChangeDisplaySettings(NULL,0);
	//ShowWindow(hwnd_hry, SW_MAXIMIZE);
	spracuj_spravy(0);
    ShowCursor(TRUE);
	spracuj_spravy(0);
	
	//UnloadTransparentBlt();

	//MSS_LOG_BLOCK_LIST;

    //DestroyWindow(hwnd_hry);
    exit(0);

	return 0;
}
