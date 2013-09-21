#ifndef _AUDIOPLUGIN_
#define _AUDIOPLUGIN_

#ifdef __cplusplus
extern "C"
{
#endif                          // __cplusplus

  typedef struct
  {
    char Song_Name[30];
  } PLAY_LIST_ITEM;

  typedef struct
  {
    long Obstruction;
    float ObstructionLF;
  } MATERIAL_LIST_ITEM;

  typedef struct
  {
    int Source;
    char flag;
  } _SOURCE;

  typedef struct
  {
    long Obstruction;
    float ObstructionLF;
    long wall_occlusion;
    float wall_occlusionLF;
    float wall_occlusion_room;
    long out_room;
    long out_roomHF;
    float out_room_rolloff;
    float out_air_absorbtion;
  } EAX_DATA;

  typedef struct
  {
    char bAudio;
    float Music_Gain;
    float Sound_Gain;
    float Ambient_Gain;
    char Music_Dir[MAX_FILENAME];
    char Sound_Dir[MAX_FILENAME];
    PLAY_LIST_ITEM *p_Play_List;
    MATERIAL_LIST_ITEM *p_Material;
    int Size_of_Play_List;
    int Size_of_Material_List;
    int Max_Sources;
    _SOURCE *p_Source;
    EAX_DATA Eax;
    //HWND hWnd;
  } AUDIO_DATA;

  // Init
  void ap_Init(AUDIO_DATA * p_ad);

  // Release
  void ap_Release(AUDIO_DATA * p_ad);

  // loades play list
  int ap_Load_Play_List(char *p_File_Name, AUDIO_DATA * p_ad);

  // Releases play list
  void ap_Release_Play_List(AUDIO_DATA * p_ad);

  // playes ogg song
  int ap_Play_Song(int Index, char Random, AUDIO_DATA * p_ad);
  int ap_Setup_and_Play_Song(int Index, char Random, AUDIO_DATA * p_ad);

  // stops ogg song
  void ap_Stop_Song(AUDIO_DATA * p_ad);

  // Plays sound
  int ap_Play_Sound(int Type, char Relative, char bAmbient, float *p_Pos,
    int Wave_Index, EAX_DATA * p_eax, AUDIO_DATA * p_ad);

  // Releases materail list
  void ap_Release_Material_List(AUDIO_DATA * p_ad);

  // loades material list
  int ap_Load_Material_List(char *p_File_Name, AUDIO_DATA * p_ad);

  // count environment
  void ap_Count_Environment(AUDIO_DATA * p_ad, void *p_Level);

  // load environment
  int ap_Load_Environment(char *p_Env_Name, void *p_Level, AUDIO_DATA * p_ad);

  int ap_Load_Sound_List(AUDIO_DATA * p_ad, char *cFile, int iStart);

#ifdef __cplusplus
}
#endif                          // __cplusplus

#endif
