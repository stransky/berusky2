//------------------------------------------------------------------------------------------------
// version 0.0.1
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "Apak.h"

//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------

#include "game_init.h"

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky_universal.h"

extern HW_KONFIG hwconf;
extern APAK_HANDLE	*pSndArchive;

int		iWinVer = 0;
extern HWND	hwnd_hry;

void gi_Set_Win_Version(void)
{
/*
   OSVERSIONINFO           osvi;
    
   ZeroMemory( &osvi, sizeof( osvi ) );
   osvi.dwOSVersionInfoSize = sizeof( osvi );
        
   if(GetVersionEx( &osvi ))
	   iWinVer = (osvi.dwMajorVersion * 100) + osvi.dwMinorVersion;

   kprintf(1, "Sytem info: Windows v%d.%d build %d - platform ID %d", 
		   osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.dwPlatformId);
*/
}

int gi_Open_Archive(char *cFile, APAK_HANDLE **pAHandle, char *cAppName, char *cKeyName)
{
	int	e;
	char text[256];

	GetPrivateProfileString(cAppName,cKeyName,"c:\\",text,256,ini_file);
	chdir(text);

  apak_dir_correction(text);
  (*pAHandle) = apakopen(cFile, text, &e);
	
	if(!(*pAHandle))
	{
		kprintf(1, "Unable to open archive %s", cFile);

		switch(e)
		{
		case APAK_FILE_NOT_FOUND:
			kprintf(1, "Reason: File not found");
			break;
		case APAK_UNABLE_TO_READ:
			kprintf(1, "Reason: Unable to read from file");
			break;
		case APAK_VERSION_MISMATCH:
			kprintf(1, "Reason: Version mismatch");
			break;
		case APAK_OUT_OF_MEMORY:
			kprintf(1, "Reason: Out of memory");
			break;
		}

		kprintf(1,"Unable to open archive %s", cFile);
		return 0;
	}

	kprintf(1, "APAK: %s", cFile);
	kprintf(1, "Velikost AFAT: %.1fKB", (*pAHandle)->FileHeader.apuISizeofFAT / 1000.0f);
	kprintf(1, "Velikost Archivu: %.1fMB", (*pAHandle)->FileHeader.apuLSizeofPAK / 1000000.0f);
	kprintf(1, "Souboru: %d", (*pAHandle)->FileHeader.apuICountofFiles);
	kprintf(1, "Adresaru: %d", (*pAHandle)->FileHeader.apuICountofDirectiories);
	kprintf(1, "Uzlu: %d", (*pAHandle)->FileHeader.apuICountofNodes);

	return 1;
}

int gi_Get_Max_Resolution_Frequency(int xPels, int yPels, int Bits)
{
	int i=0, f=0;
/*
	DEVMODE dmSettings;
	
	memset(&dmSettings,0,sizeof(dmSettings));
	dmSettings.dmSize = sizeof(DEVMODE);
	
	while(EnumDisplaySettings(NULL,i,&dmSettings))
	{
		if(dmSettings.dmBitsPerPel == (unsigned)Bits &&
		   dmSettings.dmPelsWidth == (unsigned)xPels &&
		   dmSettings.dmPelsHeight == (unsigned)yPels)
				f = dmSettings.dmDisplayFrequency;

		memset(&dmSettings,0,sizeof(dmSettings));
		dmSettings.dmSize = sizeof(DEVMODE);
		i++;
	}
*/
	return f;
}

int gi_Get_Max_Resolution_Bpp(int xPels, int yPels)
{
	int i=0, f=0;
/*
	DEVMODE dmSettings;
	
	memset(&dmSettings,0,sizeof(dmSettings));
	dmSettings.dmSize = sizeof(DEVMODE);
	
	while(EnumDisplaySettings(NULL,i,&dmSettings))
	{
		if(dmSettings.dmPelsWidth == (unsigned)xPels &&
		   dmSettings.dmPelsHeight == (unsigned)yPels &&
		   f < (signed)dmSettings.dmBitsPerPel)
				f = dmSettings.dmBitsPerPel;

		memset(&dmSettings,0,sizeof(dmSettings));
		dmSettings.dmSize = sizeof(DEVMODE);
		i++;
	}
*/
	return f;
}

int gi_Change_Grafic_Test(int xPels, int yPels, int Bits, int freq)
{
/*
	long l;
	DEVMODE	devmode;
	ZeroMemory(&devmode, sizeof(DEVMODE));

	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmBitsPerPel = Bits;
	devmode.dmPelsWidth = xPels;
	devmode.dmPelsHeight = yPels;
	devmode.dmDisplayFrequency = freq;
	devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
	
	l = ChangeDisplaySettings(&devmode, CDS_TEST);

	kprintf(1, "Grafic_Test %dx%dx%d %dHz ...", xPels, yPels, Bits, freq);

	switch(l)
	{
	case DISP_CHANGE_SUCCESSFUL:
		kprintf(1, "DISP_CHANGE_SUCCESSFUL");
		break;
	case DISP_CHANGE_RESTART:
		kprintf(1, "DISP_CHANGE_RESTART");
		break;
	case DISP_CHANGE_BADFLAGS:
		kprintf(1, "DISP_CHANGE_BADFLAGS");
		break;
	case DISP_CHANGE_BADPARAM:
		kprintf(1, "DISP_CHANGE_BADPARAM");
		break;
	case DISP_CHANGE_FAILED:
		kprintf(1, "DISP_CHANGE_FAILED");
		break;
	case DISP_CHANGE_BADMODE:
		kprintf(1, "DISP_CHANGE_BADMODE");
		break;
	case DISP_CHANGE_NOTUPDATED:
		kprintf(1, "DISP_CHANGE_NOTUPDATED");
		break;
	}

	if(l != DISP_CHANGE_SUCCESSFUL)
		return 0;
	else
		return 1;
*/
}

int gi_Get_Max_Freq(int xPels, int yPels, int bpp)
{
	int i=0, f[100], c=0;
/*
	DEVMODE dmSettings;

	
	memset(&dmSettings,0,sizeof(dmSettings));
	memset(&f,0,sizeof(int)*100);

	dmSettings.dmSize = sizeof(DEVMODE);
	
	while(EnumDisplaySettings(NULL,i,&dmSettings))
	{
		if(dmSettings.dmPelsWidth == (unsigned)xPels &&
		   dmSettings.dmPelsHeight == (unsigned)yPels &&
		   dmSettings.dmBitsPerPel == (unsigned)bpp)
		{
			if(gi_Change_Grafic_Test(xPels, yPels, bpp, dmSettings.dmDisplayFrequency))
			{
				f[c] = dmSettings.dmDisplayFrequency;
				c++;
			}
		}

		memset(&dmSettings,0,sizeof(dmSettings));
		dmSettings.dmSize = sizeof(DEVMODE);
		i++;
	}

	c = 0;

	for(i=0;i<100;i++)
		if(!f[i])
			if(c-1 < 0)
				return f[0];
			else
				return f[c-1];
		else
			c++;

	return f[c];
*/
}

int gi_Change_Grafic_Mode(int xPels, int yPels, int Bits, int freq, DWORD dwflags)
{
/*
	long l;
	DEVMODE	devmode;

	ZeroMemory(&devmode, sizeof(DEVMODE));

	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmBitsPerPel = Bits;
	devmode.dmPelsWidth = xPels;
	devmode.dmPelsHeight = yPels;
	devmode.dmDisplayFrequency = freq;
	devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	
	if(freq > 0)
		 devmode.dmFields |= DM_DISPLAYFREQUENCY;

	l = ChangeDisplaySettings(&devmode, dwflags);

	kprintf(1, "Grafic_Mode %dx%dx%d %dHz ...", xPels, yPels, Bits, freq);

	switch(l)
	{
	case DISP_CHANGE_SUCCESSFUL:
		kprintf(1, "DISP_CHANGE_SUCCESSFUL");
		break;
	case DISP_CHANGE_RESTART:
		kprintf(1, "DISP_CHANGE_RESTART");
		break;
	case DISP_CHANGE_BADFLAGS:
		kprintf(1, "DISP_CHANGE_BADFLAGS");
		break;
	case DISP_CHANGE_BADPARAM:
		kprintf(1, "DISP_CHANGE_BADPARAM");
		break;
	case DISP_CHANGE_FAILED:
		kprintf(1, "DISP_CHANGE_FAILED");
		break;
	case DISP_CHANGE_BADMODE:
		kprintf(1, "DISP_CHANGE_BADMODE");
		break;
	case DISP_CHANGE_NOTUPDATED:
		kprintf(1, "DISP_CHANGE_NOTUPDATED");
		break;
	}

	if(l != DISP_CHANGE_SUCCESSFUL)
		return 0;
	else
		return 1;
*/
}
/*
int gi_EnumDisplaySettings(DEVMODE *pdevmode)
{
	return EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, pdevmode);
}
*/

void gi_APAK_Load(unsigned long mem, struct _finddata_t* pData)
{
/*
	char *pMem;
	apuLong size;
	FILE *file;
	
	if(!pData)
		return;

	file = aopen(pSndArchive, pData->name,"rb");

	if(!file)
	{
		kprintf(1, "%s: %s", pData->name, pSndArchive->cError);
		return;
	}

	agetbuffer(file, &pMem, &size);

	if(!mem)
		adas_Load_FirstMemory("index.dat",pMem,pData->name);
	else
		adas_Load_NextMemory(pMem, pData->name);

	aclose(file);
*/
}

//------------------------------------------------------------------------------------------------
// nahodi sound engine
//------------------------------------------------------------------------------------------------
/*
void gi_Init_Sound_Engine(HWND hWnd, AUDIO_DATA *p_ad)
{
	char				text[256];
	ADAS_INIT_DATA		init_data;
	unsigned long		max_mem, mem = 0;
//	long				Done,error;
//	struct _finddata_t	Data;
	float listenerPos[]={0.0,0.0,0.0};
	float listenerVel[]={0.0,0.0,0.0};
	float listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};
	int		iValue;
	char	*cString;
	int		iLowV, iHiV;

	init_data.Channels = GetPrivateProfileInt("soundengine","channels",8,ini_file);
	init_data.Implementation = "DirectSound3D";

	p_ad->Max_Sources = init_data.Channels;
	p_ad->hWnd = hWnd;
	ap_Init(p_ad);

	max_mem = GetPrivateProfileInt("soundengine","pre_load",0,ini_file);
	
	GetPrivateProfileString("soundengine","sound_dir","c:\\",init_data.Sound_Dir,256,ini_file);
	GetPrivateProfileString("soundengine","sound_dir","c:\\",p_ad->Sound_Dir,256,ini_file);
	GetPrivateProfileString("soundengine","music_dir","c:\\",p_ad->Music_Dir,256,ini_file);
	
	iValue = GetPrivateProfileInt("soundengine","soundvolume",0,ini_file);
		p_ad->Sound_Gain = (float)(iValue/100.0f);

	iValue = GetPrivateProfileInt("soundengine","musicvolume",0,ini_file);
		p_ad->Music_Gain = (float)(iValue/100.0f);

	iValue = GetPrivateProfileInt("soundengine","ambientvolume",0,ini_file);
		p_ad->Ambient_Gain = (float)(iValue/100.0f);

	init_data.Scale_Factor = 0.005f;

	iValue = GetPrivateProfileInt("hra","no_sound",0,ini_file);
	
	adas_OGG_Init();

	if(!iValue)
	{
		adas_Reset_Last_Error();
		kprintf(1,"Init Sound Engine...");
		adas_Init(&init_data);
		if (adas_Get_Last_Error(text,256))
		{
			//MessageBox(hWnd,text,"Error",MB_OK);
			kprintf(1,text);
			//MyMessageBox(hwnd_hry, "##error_title", "##adas_init_error","");
			p_ad->bAudio = 0;
		}
		else
		{
			if (adas_Get_Last_Warning(text,256))
			{
				//MessageBox(hWnd,text,"Error",MB_OK);
				kprintf(1,text);
			}
		
			adas_Get_Version(&iHiV, &iLowV);
			kprintf(1, "AnakreoN Digital Audio System v%d.%d", iHiV, iLowV);
			
			cString = alGetString(AL_VENDOR);
			kprintf(1, "Vendor: %s", cString);			
			cString = alGetString(AL_VERSION);
			kprintf(1, "Version: %s", cString);			
			cString = alGetString(AL_RENDERER);
			kprintf(1, "Renderer: %s", cString);			
			cString = alGetString(AL_EXTENSIONS);
			kprintf(1, "Extensions: %s", cString);			

			p_ad->bAudio = 1;
			chdir(init_data.Sound_Dir);
			achdir(pSndArchive, init_data.Sound_Dir);

			ap_Load_Sound_List(p_ad, "basicset.dat", 0);
*/
/*			kprintf(1,"Memory for waves: %.1fMB", max_mem / 1000000.0f);
			kprintf(1,"Loading waves...");
			Done = afindfirst(pSndArchive, "*.wav",&Data);
			error = Done;
			
			while ((error != -1) && (mem < max_mem))
			{
				if ((error != -1) && ((mem + Data.size) < max_mem))
					{
						adas_Reset_Last_Error();
						
						gi_APAK_Load(mem, &Data);

						if (adas_Get_Last_Error(text,256))
						{
							kprintf(1,"File not found: index.dat");
						}
						else
							mem += Data.size;
					}
				error = afindnext(Done,&Data);
			}
			afindclose(Done);*/
/*	
			kprintf(1,"Distance Model: AL_INVERSE_DISTANCE_CLAMPED");
			adas_Set_Distance_Model(AL_INVERSE_DISTANCE_CLAMPED);
			kprintf(1,"Doppler Factor: 1.0");
			adas_Set_Doppler_Factor(1.0f);
			kprintf(1,"Doppler Velocity: 343.0");
			adas_Set_Doppler_Velocity(343.0f);
			
			kprintf(1,"Setting Listener...");
			adas_Reset_Last_Error();
			adas_Set_Listener_Position(listenerPos);
			adas_Set_Listener_Velocity(listenerVel);
			adas_Set_Listener_Orientation(listenerOri);
			adas_Set_Listener_Environment(0);
			if (adas_Get_Last_Error(text,256))
				{
				//MyMessageBox(hwnd_hry, "##error_title", "##adas_init_error","");
					//MessageBox(hWnd,text,"Error",MB_OK);
					kprintf(1,text);
				}
		}
	}
	else
		p_ad->bAudio = 0;
}
*/

//------------------------------------------------------------------------------------------------
// ukonci sound engine
//------------------------------------------------------------------------------------------------
void gi_Release_Sound_Engine(void)
{
/*
	kprintf(1,"Release loaded waves...");
	adas_Release_Loaded_Data();
	kprintf(1,"Release sound engine...");
	adas_Exit();
*/
}
