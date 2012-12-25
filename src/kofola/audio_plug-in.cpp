//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>

#include "3d_all.h"
#include "adas.h"
#include "audio_plug-in.h"
#include "Object.h"
#include "Apak.h"
#include "Tools.h"

extern APAK_HANDLE *pSndArchive;
//extern HWND hwnd_hry;

//------------------------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------------------------
void ap_Init(AUDIO_DATA * p_ad)
{
  int i;

  p_ad->bAudio = 0;
  p_ad->Music_Dir[0] = '\0';
  p_ad->Sound_Dir[0] = '\0';
  p_ad->Music_Gain = 1.0f;
  p_ad->p_Play_List = 0;
  p_ad->p_Material = 0;
  p_ad->Size_of_Play_List = 0;
  p_ad->Size_of_Material_List = 0;
  p_ad->Sound_Gain = 1.0f;

  p_ad->p_Source = (_SOURCE *) malloc((p_ad->Max_Sources) * sizeof(_SOURCE));
  if (!p_ad->p_Source) {
    //MessageBox(p_ad->hWnd,"Unable to allocate memory for active source database","Error",MB_OK);
    kprintf(1, "Unable to allocate memory for active source database");
    return;
  }

  for (i = 0; i < p_ad->Max_Sources; i++) {
    p_ad->p_Source[i].Source = -1;
    p_ad->p_Source[i].flag = 1;
  }
}

//------------------------------------------------------------------------------------------------
// Release
//------------------------------------------------------------------------------------------------
void ap_Release(AUDIO_DATA * p_ad)
{
  free((void *) p_ad->p_Source);
}

//------------------------------------------------------------------------------------------------
// Load sample from APAK
//------------------------------------------------------------------------------------------------
int ap_Load_Sample(int iCount, char *cFile)
{
  char *pMem;
  apuInt size;
  FILE *file;

  if (!cFile)
    return(FALSE);

  file = aopen(pSndArchive, cFile, "rb");
  if (!file) {
    kprintf(1, "%s: %s", cFile, pSndArchive->cError);
    return(FALSE);
  }

  agetbuffer(file, &pMem, &size);

  int ret = (!iCount) ? adas_Load_FirstMemory("index.dat", pMem, size, cFile):
                        adas_Load_NextMemory(pMem, size, cFile);
  aclose(file);
  return(ret);
}

//------------------------------------------------------------------------------------------------
// Loads sample List
//------------------------------------------------------------------------------------------------
int ap_Load_Sound_List(AUDIO_DATA * p_ad, char *cFile, int iStart)
{
  FILE *file;
  char text[256];
  int c = iStart;
  int iMaterial = 0;

  chdir(p_ad->Sound_Dir);

  if (!strlen(cFile))
    return 0;
  
  achdir(pSndArchive, p_ad->Sound_Dir);
  file = aopen(pSndArchive, cFile, "r");
  if (!file) {
    kprintf(1, "Play list file not found");
    return 0;
  }

  agets(text, 255, file);

  iMaterial = atoi(text);

  while (!aeof(file)) {
    memset(text, 0, 256);
    agets(text, 255, file);

    if (!strlen(text))
      break;
    else {
      newline_cut(text);
      if(!ap_Load_Sample(c, text)) {
        // Loading failed - stop it
        break;
      }
      c++;
    }
  }

  aclose(file);

  return iMaterial;
}

//------------------------------------------------------------------------------------------------
// loades play list
//------------------------------------------------------------------------------------------------
int ap_Load_Play_List(char *p_File_Name, AUDIO_DATA * p_ad)
{
  FILE *file = 0;
  char text[30];
  int i;

  if (p_ad->p_Play_List)
    return 0;

  chdir(p_ad->Music_Dir);

  file = fopen(p_File_Name, "r");
  if (!file) {
    //MessageBox(p_ad->hWnd,"Play list file not found","Error", MB_OK);
    MyMessageBox(NULL, "##error_title", "##play_list_error", "");
    kprintf(1, "Play list file not found");
    return 0;
  }

  fgets(text, 30, file);
  p_ad->Size_of_Play_List = atoi(text);

  p_ad->p_Play_List = (PLAY_LIST_ITEM *) malloc((p_ad->Size_of_Play_List) * sizeof(PLAY_LIST_ITEM));
  if (!p_ad->p_Play_List) {
    //MessageBox(p_ad->hWnd,"Unable to allocate memory for play list","Error",MB_OK);
    kprintf(1, "Unable to allocate memory for play list");
    MyMessageBox(NULL, "##error_title", "##play_list_error", "");
    return 0;
  }

  for (i = 0; i < p_ad->Size_of_Play_List; i++) {
    char *p_name = p_ad->p_Play_List[i].Song_Name;
    fgets(p_name, 30, file);
    newline_cut(p_name);
  }
  fclose(file);
  return 1;
}

//------------------------------------------------------------------------------------------------
// Releases play list
//------------------------------------------------------------------------------------------------
void ap_Release_Play_List(AUDIO_DATA * p_ad)
{
  free((void *) p_ad->p_Play_List);
  p_ad->p_Play_List = 0;
  p_ad->Size_of_Play_List = 0;
}

//------------------------------------------------------------------------------------------------
// Playes ogg song
//------------------------------------------------------------------------------------------------
int ap_Play_Song(int Index, char Random, AUDIO_DATA * p_ad)
{
//      char text[256];
  char song_name[256];
  int song = 0;
  FILE *file;
  int c = 0;

  if (!p_ad->bAudio)
    return 1;
  if (p_ad->Music_Gain < 0.05f)
    return 1;

CHOOSE_SONG:

  if (Random)
    song = (rand() % (p_ad->Size_of_Play_List - 12)) + 12;
  else
    song = Index;

  strcpy(song_name, p_ad->Music_Dir); 
  strcat(song_name, DIR_SLASH_STRING);
  strcat(song_name, p_ad->p_Play_List[song].Song_Name);

  file = fopen(song_name, "r");

  if (!file) {
    if (Random) {
      c++;

      if (c > 10)
        return 0;

      goto CHOOSE_SONG;
    }
  }
  else
    fclose(file);

  ogg_open(song_name, p_ad->Music_Gain);

  if (!ogg_playback())
    return 0;

  return 1;

/*	adas_Reset_Last_Error();
	adas_OGG_Open_Stream(song_name);
	
	if (adas_Get_Last_Error(text,256))
	{
		MessageBox(p_ad->hWnd,text,"Error",MB_OK);
		kprintf(1,"%s",text);
		return 0;
	}
	else
	{
		float pos[3] = {0.0f,0.0f,0.0f};
		
		adas_OGG_Set_Pitch(1.0f);
		
		adas_OGG_Set_Source_Relative(1);
		adas_OGG_Set_Position(pos);
		adas_OGG_Set_Velocity(pos);
		adas_Reset_Last_Error();
		adas_OGG_Play_Stream();
		if (adas_Get_Last_Error(text,256))
		{
			MessageBox(p_ad->hWnd,text,"Error",MB_OK);
			kprintf(1,text);
			return 0;
		}
		else
			return 1;
	}*/
}

//------------------------------------------------------------------------------------------------
// Playes ogg song
//------------------------------------------------------------------------------------------------
int ap_Setup_and_Play_Song(int Index, char Random, AUDIO_DATA * p_ad)
{
/*	int flag = 0;
	char text[256];
	char song_name[256];
	int song = 0;

	if (!p_ad->bAudio) return 1;
	if (p_ad->Music_Gain < 0.05f) return 1;

	if (Random)
		while(song < 2)
			song = rand()%p_ad->Size_of_Play_List;
	else
		song = Index;

	strcpy(song_name, p_ad->Music_Dir);
	strcat(song_name, "\\");
	strcat(song_name, p_ad->p_Play_List[song].Song_Name);
	adas_Reset_Last_Error();
	adas_OGG_Open_Stream(song_name);
	
	if (adas_Get_Last_Error(text,256))
	{
		MessageBox(p_ad->hWnd,text,"Error",MB_OK);
		kprintf(1,"%s",text);
		return 0;
	}
	else
	{
		float pos[3] = {0.0f,0.0f,0.0f};
		
		adas_OGG_Set_Gain(p_ad->Music_Gain);
		adas_OGG_Set_Pitch(1.0f);
		
		adas_OGG_Set_Source_Relative(1);
		adas_OGG_Set_Position(pos);
		adas_OGG_Set_Velocity(pos);
		adas_Reset_Last_Error();
		
		adas_OGG_Setup_Stream(&flag);
		if (adas_Get_Last_Error(text,256))
		{
			MessageBox(p_ad->hWnd,text,"Error",MB_OK);
			kprintf(1,text);
			return 0;
		}
		else
		{
			while(!flag)
				Sleep(100);

			return 1;
		}
	}*/

  return 1;
}

//------------------------------------------------------------------------------------------------
// stops ogg song
//------------------------------------------------------------------------------------------------
void ap_Stop_Song(AUDIO_DATA * p_ad)
{
  if (!p_ad->bAudio)
    return;

  ogg_release();

  /*adas_OGG_Stop_Stream();
     adas_OGG_Close_Stream(); */
}

//------------------------------------------------------------------------------------------------
// Plays sound
//------------------------------------------------------------------------------------------------
int ap_Play_Sound(int Type, char Relative, char bAmbient, float *p_Pos,
  int Wave_Index, EAX_DATA * p_eax, AUDIO_DATA * p_ad)
{
  char text[256];
  ADAS_SOUND_SOURCE_DATA ssd;
  int source;
  void *p_callback = 0;

  if (!karmin_aktivni)
    return -1;

  if (!p_ad->bAudio)
    return -1;

  if (bAmbient) {
    if (p_ad->Ambient_Gain < 0.05f)
      return -1;
    ssd.Gain = p_ad->Ambient_Gain;
  }
  else {
    if (p_ad->Sound_Gain < 0.05f)
      return -1;
    ssd.Gain = p_ad->Sound_Gain;
  }

  ssd.Owner = -1;
  ssd.Pitch = 1.0f;
  ssd.Pos[0] = p_Pos[0];
  ssd.Pos[1] = p_Pos[1];
  ssd.Pos[2] = p_Pos[2];
  ssd.Type = Type;
  ssd.Velocity[0] = 0.0f;
  ssd.Velocity[1] = 0.0f;
  ssd.Velocity[2] = 0.0f;
  ssd.Wave_Index = Wave_Index;
  ssd.Source_Relative = Relative;
  ssd.Bounding_Object.Type = UNDEFINED_VALUE;

  if (p_eax) {
    ssd.Obstruction = p_eax->Obstruction;
    ssd.ObstructionLF = p_eax->ObstructionLF;
    ssd.out_air_absorbtion = p_eax->out_air_absorbtion;
    ssd.out_room = p_eax->out_room;
    ssd.out_room_rolloff = p_eax->out_room_rolloff;
    ssd.out_roomHF = p_eax->out_roomHF;
    ssd.wall_occlusion = p_eax->wall_occlusion;
    ssd.wall_occlusion_room = p_eax->wall_occlusion_room;
    ssd.wall_occlusionLF = p_eax->wall_occlusionLF;
  }
  else {
    ssd.Obstruction = UNDEFINED_VALUE;
    ssd.ObstructionLF = (float) UNDEFINED_VALUE;
    ssd.out_air_absorbtion = (float) UNDEFINED_VALUE;
    ssd.out_room = UNDEFINED_VALUE;
    ssd.out_room_rolloff = (float) UNDEFINED_VALUE;
    ssd.out_roomHF = UNDEFINED_VALUE;
    ssd.wall_occlusion = UNDEFINED_VALUE;
    ssd.wall_occlusion_room = (float) UNDEFINED_VALUE;
    ssd.wall_occlusionLF = (float) UNDEFINED_VALUE;
  }

  adas_Reset_Last_Error();
  source = adas_Create_Source(&ssd, &p_callback);
  if (adas_Get_Last_Error(text, 256)) {
    //MessageBox(p_ad->hWnd,text,"Error",MB_OK);
    kprintf(1, text);
  }
  else if ((source != -1) && !Relative) {
    // je to takovy cudny, ze zvuky s relativni pozici se nepocitaji, ale
    // zatim to nevadi. Uvidime, jestli to tak pujde dal. Jestli, musim
    // pridat priznak
    p_ad->p_Source[source].Source = source;
    p_ad->p_Source[source].flag = 1;
    *(char **) p_callback = &p_ad->p_Source[source].flag;
  }
  else if (p_callback)
    *(char **) p_callback = NULL;

  return source;
}

//------------------------------------------------------------------------------------------------
// translates real position of object to logical(square map) posiiton
//------------------------------------------------------------------------------------------------
void ap_Translate_Position(float *p_real, int *p_log)
{
  p_log[0] = (int) floor(p_real[0] / 2.0f);
  p_log[2] = (int) floor(p_real[1] / 2.0f);
  p_log[1] = (int) floor(p_real[2] / 2.0f);
}

//------------------------------------------------------------------------------------------------
// makes normal vertor
//------------------------------------------------------------------------------------------------
void ap_Normal_Vector(float *v)
{
  float size =
    (float) sqrt((v[0]) * (v[0]) + (v[1]) * (v[1]) + (v[2]) * (v[2]));

  if (size == 0)
    return;

  v[0] /= size;
  v[1] /= size;
  v[2] /= size;
}

//------------------------------------------------------------------------------------------------
// count environment
//------------------------------------------------------------------------------------------------
void ap_Count_Environment(AUDIO_DATA * p_ad, void *p_Level)
{
  int i;
  float s_pos[3], l_pos[3], v[3], size;
  int sl_pos[3], lp[3], material;
  char end, t_end;
  long m_pos;
  LEVELINFO *p_L = (LEVELINFO *) p_Level;

  for (i = 0; i < p_ad->Max_Sources; i++) {
    if (p_ad->p_Source[i].Source != -1) {
      if (!p_ad->p_Source[i].flag) {
        p_ad->p_Source[i].Source = -1;
      }
      else {
        adas_Get_Source_Position(p_ad->p_Source[i].Source, s_pos);
        adas_Get_Listener_Position(l_pos);

        v[0] = l_pos[0] - s_pos[0];
        v[1] = l_pos[1] - s_pos[1];
        v[2] = l_pos[2] - s_pos[2];

        // jestlize je vzdalenist vetsi nez 15, nema smysl pokracovat,
        // protoze okluze se tejne vzhledem ke vzdalenosti neprojevi
        size = (float) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        if (size > 15)
          continue;

        ap_Translate_Position(s_pos, sl_pos);
        if (sl_pos[0] > (p_L->Size[0] - 1) ||
          sl_pos[0] < 0 ||
          sl_pos[1] > (p_L->Size[1] - 1) ||
          sl_pos[1] < 0 || sl_pos[2] > (p_L->Size[2] - 1) || sl_pos[2] < 0)
          continue;

        ap_Normal_Vector(v);
        lp[0] = sl_pos[0];
        lp[1] = sl_pos[1];
        lp[2] = sl_pos[2];
        t_end = 0;
        while (!t_end) {

          end = 0;
          //posun o kosticku
          while (!end) {
            s_pos[0] += 0.1f * v[0];
            s_pos[1] += 0.1f * v[1];
            s_pos[2] += 0.1f * v[2];
            ap_Translate_Position(s_pos, sl_pos);
            if (lp[0] != sl_pos[0] ||
              lp[1] != sl_pos[1] || lp[2] != sl_pos[2])
              end = 1;
          }

          lp[0] = sl_pos[0];
          lp[1] = sl_pos[1];
          lp[2] = sl_pos[2];
          if (sl_pos[0] > (p_L->Size[0] - 1) ||
            sl_pos[0] < 0 ||
            sl_pos[1] > (p_L->Size[1] - 1) ||
            sl_pos[1] < 0 ||
            sl_pos[2] > (p_L->Size[2] - 1) || sl_pos[2] < 0) {
            /*
            adas_Set_Source_Obstruction(PARTICULAR_SOUND_SOURCE,
              UNDEFINED_VALUE, p_ad->p_Source[i].Source,
              EAXBUFFER_DEFAULTOBSTRUCTION);
            adas_Set_Source_ObstructionLF(PARTICULAR_SOUND_SOURCE,
              UNDEFINED_VALUE, p_ad->p_Source[i].Source,
              EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO);
            */
            t_end = 1;
            break;
          }

          m_pos = sl_pos[0] + sl_pos[1] * p_L->Size[0] +
            sl_pos[2] * p_L->Size[0] * p_L->Size[1];

          if (p_L->Level[m_pos])
            break;
        }

        if (!t_end) {
          if (p_L->Level[m_pos]) {
            material = p_L->Level[m_pos]->p_Object->Material - 1;
            if (material > 0) {
              material--;
            /*
              adas_Set_Source_Obstruction(PARTICULAR_SOUND_SOURCE,
                UNDEFINED_VALUE, p_ad->p_Source[i].Source,
                p_ad->p_Material[material].Obstruction);
              adas_Set_Source_ObstructionLF(PARTICULAR_SOUND_SOURCE,
                UNDEFINED_VALUE, p_ad->p_Source[i].Source,
                p_ad->p_Material[material].ObstructionLF);
            */
            }
          }
          else {
          /*
            adas_Set_Source_Obstruction(PARTICULAR_SOUND_SOURCE,
              UNDEFINED_VALUE, p_ad->p_Source[i].Source,
              EAXBUFFER_DEFAULTOBSTRUCTION);
            adas_Set_Source_ObstructionLF(PARTICULAR_SOUND_SOURCE,
              UNDEFINED_VALUE, p_ad->p_Source[i].Source,
              EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO);
          */
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------------------------------------
// loades material list
//------------------------------------------------------------------------------------------------
int ap_Load_Material_List(char *p_File_Name, AUDIO_DATA * p_ad)
{
  FILE *file = 0;
  FILE *Material_file = 0;
  char text[30], error[256];
  int i;

  if (p_ad->p_Material)
    return 0;

  achdir(pSndArchive, p_ad->Sound_Dir);

  file = aopen(pSndArchive, p_File_Name, "r");
  if (!file) {
    //MessageBox(p_ad->hWnd,"Material list file not found","Error", MB_OK);
    MyMessageBox(NULL, "##error_title", "##material_list_error", "");
    kprintf(1, "Material list file not found");
    return 0;
  }

  agets(text, 30, file);
  p_ad->Size_of_Material_List = atoi(text);

  p_ad->p_Material =
    (MATERIAL_LIST_ITEM *) malloc((p_ad->Size_of_Material_List) *
    sizeof(MATERIAL_LIST_ITEM));
  if (!p_ad->p_Material) {
    //MessageBox(p_ad->hWnd,"Unable to allocate memory for material list","Error",MB_OK);
    kprintf(1, "Unable to allocate memory for material list");
    MyMessageBox(NULL, "##error_title", "##material_list_error", "");
    return 0;
  }

  for (i = 0; i < p_ad->Size_of_Material_List; i++) {
    agets(text, 30, file);
    newline_cut(text);
    Material_file = aopen(pSndArchive, text, "rb");
    if (!Material_file) {
      sprintf(error, "%s not found", text);
      //MessageBox(p_ad->hWnd,error,"Error", MB_OK);
      kprintf(1, error);
    }
    else {
      aread(&p_ad->p_Material[i], sizeof(MATERIAL_LIST_ITEM), 1,
        Material_file);
      aclose(Material_file);
    }
  }

  aclose(file);
  return 1;
}

//------------------------------------------------------------------------------------------------
// Releases materail list
//------------------------------------------------------------------------------------------------
void ap_Release_Material_List(AUDIO_DATA * p_ad)
{
  free((void *) p_ad->p_Material);
  p_ad->p_Material = 0;
  p_ad->Size_of_Material_List = 0;
}

//------------------------------------------------------------------------------------------------
// load environment
//------------------------------------------------------------------------------------------------
int ap_Load_Environment(char *p_Env_Name, void *p_Level, AUDIO_DATA * p_ad)
{
  FILE *file;
  LEVELINFO *p_L = (LEVELINFO *) p_Level;

  achdir(pSndArchive, p_ad->Sound_Dir);
  file = aopen(pSndArchive, p_Env_Name, "rb");
  if (file) {
    aread(&p_L->Environment, sizeof(ENVIRONMENT), 1, file);
    aclose(file);
    return 1;
  }
  else {    
    kprintf(1, "%s not found", p_Env_Name);
    return 0;
  }
}
