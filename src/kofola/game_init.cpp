//------------------------------------------------------------------------------------------------
// version 0.0.1
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <errno.h>
#include "Apak.h"
#include "adas.h"

//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------

#include "compat_mini.h"
#include "game_init.h"

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky_universal.h"

extern HW_KONFIG hwconf;
extern APAK_HANDLE *pSndArchive;

int iWinVer = 0;

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

int gi_Open_Archive(char *cFile, APAK_HANDLE ** pAHandle, char *p_dir)
{
  int e;

  if (chdir(p_dir)) {
    kprintf(1, "Unable to change directory to %s: %s",
	    p_dir, strerror(errno));
    return 0;
  }
  (*pAHandle) = apakopen(cFile, p_dir, &e);

  if (!(*pAHandle)) {
    kprintf(1, "Unable to open archive %s", cFile);

    switch (e) {
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

    assert(0);
    abort();
  }

  kprintf(1, "APAK: %s", cFile);
  kprintf(1, "Velikost AFAT: %.1fKB", (*pAHandle)->FileHeader.apuISizeofFAT / 1000.0f);
  kprintf(1, "Velikost Archivu: %.1fMB", (*pAHandle)->FileHeader.apuLSizeofPAK / 1000000.0f);
  kprintf(1, "Souboru: %d", (*pAHandle)->FileHeader.apuICountofFiles);
  kprintf(1, "Adresaru: %d", (*pAHandle)->FileHeader.apuICountofDirectiories);
  kprintf(1, "Uzlu: %d", (*pAHandle)->FileHeader.apuICountofNodes);

  return 1;
}

/*
int gi_EnumDisplaySettings(DEVMODE *pdevmode)
{
	return EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, pdevmode);
}
*/
/*
int gi_APAK_Load(unsigned long mem, struct _finddata_t *pData)
{
	char *pMem;
	apuInt size;
	FILE *file;
	
	if(!pData)
		return(FALSE);

	file = aopen(pSndArchive, pData->name,"rb");
	if(!file)	{
		kprintf(1, "%s: %s", pData->name, pSndArchive->cError);
		return(FALSE);
	}

	agetbuffer(file, &pMem, &size);

	int ret = (!mem) ? adas_Load_FirstMemory("index.dat",pMem,size,pData->name) :
                     adas_Load_NextMemory(pMem, size, pData->name);
	aclose(file);
  return(ret);
}
*/
//------------------------------------------------------------------------------------------------
// nahodi sound engine
//------------------------------------------------------------------------------------------------
void gi_Init_Sound_Engine(AUDIO_DATA *p_ad)
{
	char				text[MAX_FILENAME];
	ADAS_INIT_DATA		init_data;
	//unsigned long	max_mem;
  //unsigned long mem = 0;
//	long				Done,error;
//	struct _finddata_t	Data;
	float listenerPos[]={0.0,0.0,0.0};
	float listenerVel[]={0.0,0.0,0.0};
	float listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};
	int		iValue;
	const char	*cString;
	int		iLowV, iHiV;

	init_data.Channels = GetPrivateProfileInt("soundengine","channels",8,ini_file);
#ifdef WINDOWS
	init_data.Implementation = "DirectSound3D";
#else
	init_data.Implementation = NULL;
#endif

	p_ad->Max_Sources = init_data.Channels;
	//p_ad->hWnd = hWnd;
	ap_Init(p_ad);

	//max_mem = GetPrivateProfileInt("soundengine","pre_load",0,ini_file);
		
	iValue = GetPrivateProfileInt("soundengine","soundvolume",0,ini_file);
		p_ad->Sound_Gain = (float)(iValue/100.0f);

	iValue = GetPrivateProfileInt("soundengine","musicvolume",0,ini_file);
		p_ad->Music_Gain = (float)(iValue/100.0f);

	iValue = GetPrivateProfileInt("soundengine","ambientvolume",0,ini_file);
		p_ad->Ambient_Gain = (float)(iValue/100.0f);

	init_data.Scale_Factor = 0.005f;

	iValue = GetPrivateProfileInt("game","no_sound",0,ini_file);
	
	adas_OGG_Init();

	if(!iValue)
	{
		adas_Reset_Last_Error();
		kprintf(1,"Init Sound Engine...");
		adas_Init(&init_data);
		if (adas_Get_Last_Error(text,MAX_FILENAME))
		{
			//MessageBox(hWnd,text,"Error",MB_OK);
			kprintf(1,text);
			//MyMessageBox(hwnd_hry, "##error_title", "##adas_init_error","");
			p_ad->bAudio = 0;
		}
		else
		{
			if (adas_Get_Last_Warning(text,MAX_FILENAME))
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
			if (chdir(p_ber->dir.sound_dir)) {
				kprintf(1, "Cannot change directory to %s: %s",
					p_ber->dir.sound_dir, strerror, errno);
				return;
			}
			achdir(pSndArchive, p_ber->dir.sound_dir);

			ap_Load_Sound_List(p_ad, "basicset.dat", 0);

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

						if (adas_Get_Last_Error(text,MAX_FILENAME))
						{
							kprintf(1,"File not found: index.dat");
						}
						else
							mem += Data.size;
					}
				error = afindnext(Done,&Data);
			}
			afindclose(Done);*/
	
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
			adas_Set_Listener_Environment(EAX_ENVIRONMENT_GENERIC);

			if (adas_Get_Last_Error(text,MAX_FILENAME))
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

//------------------------------------------------------------------------------------------------
// ukonci sound engine
//------------------------------------------------------------------------------------------------
void gi_Release_Sound_Engine(void)
{
	kprintf(1,"Release loaded waves...");
	adas_Release_Loaded_Data();
	kprintf(1,"Release sound engine...");
	adas_Exit();
}
