//------------------------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------------------------
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "3d_math.h"
#include "adas.h"
#include "compat_mini.h"

//------------------------------------------------------------------------------------------------
// libraries
//------------------------------------------------------------------------------------------------
/*
#pragma comment(lib,"lib\\ALut.lib")
#pragma comment(lib,"lib\\OpenAL32.lib")
#pragma comment(lib,"lib\\eax.lib")
#pragma comment(lib,"lib\\eaxguid.lib")
#pragma comment(lib,"winmm.lib")
*/

//------------------------------------------------------------------------------------------------
// constants declarations
//------------------------------------------------------------------------------------------------
#define SIZEOFSOUNDDATA		1000    // Max. number of Sound Data structures (max. wav
                                                                                        // files loaded)
#define MAXQUEUED			16      // Max. of queued buffers to sound source

#define ADAS_VERSION_LOW	1       // low version of AnakreoN Digital Audio System
#define ADAS_VERSION_HI		1       // hi version of AnakreoN Digital Audio System

//------------------------------------------------------------------------------------------------
// types definitions
//------------------------------------------------------------------------------------------------
typedef struct {
  THREAD_ID ThreadID;           // Manager Thread ID
  THREAD_HANDLE Thread;         // pointer to Manager Thread
  int Shot_down;                // shot down signal
} ADAS_MANAGER;

typedef struct {
  int Owner;                    // owner of sound source
  int Type;                     // type of sound source
  ALuint Buffer[MAXQUEUED];     // pointer do buffer
  ALuint Source;                // poinet to OpenAL source
  int Wave_Index[MAXQUEUED];    // wave index, which is played
  int Lock;                     // security lock (deallocation is not 
  // allowed)
  void *p_callback;             // callback flag is set when source is 
  // destroyed
  char Buffer_Pointer;          // pointer to free buffer in line
  int Buffer_Source[MAXQUEUED]; // RAM                          = Buffer is from RAM 
  // HDD                          = Buffer is from HDD
  // UNDEFINED_VALUE      = Undefined source
  BOUNDING_OBJECT Bounding_Object;      // Bounding Object for creating large 
  // sound dources
} ADAS_SOUND_SOURCE;


typedef struct {
  ALsizei Size;                 // size fo data
  ALuint Frequece;              // frequence
  ALenum Format;                // format (8, 16, stereo, mono)
  ALvoid *Data;                 // pointer to data
  ALuint Buffer;                // pointer to buffer
  int Wave_Index;               // index of wave in data
} ADAS_SOUND_DATA;

typedef struct {
  char Name[30];                // name of wave
  int Index;                    // uniqe index
} ADAS_WAVEFILEDESC;

//------------------------------------------------------------------------------------------------
// global declarations
//------------------------------------------------------------------------------------------------
typedef void *EAXSet;
typedef void *EAXGet;

static ADAS_MANAGER Manager;    // Manager for sound sources
static ADAS_INIT_DATA ADAS_data;        // general data: sound dir, app. pointer,...
static ADAS_SOUND_SOURCE *SoundSource;  // Pointer to structure SoundSource
static ALCcontext *p_Context = 0;       // Pointer to device kontext
static ALCdevice *p_Device = 0; // Pointer to device
static ALuint p_DummySource = 0;        // EAX Dummy Buffer
static EAXSet p_EAXSet = 0;     // Pointer to EAXSet function
static EAXGet p_EAXGet = 0;     // Pointer to EAXGet functionstatic ALboolean                   bEAXExtPresent;                 // EAX device presentation
static ADAS_WAVEFILEDESC *WaveFile;     // Pointer to structure which holds wave indexes
static ADAS_SOUND_DATA *SoundData;      // Pointer to structure SoundData
static int Size_of_Indexes;     // size of WaveFile
static int Size_of_Sound_Data;  // size of Sound Data
char Last_Error[256];           // text of last error
int bLast_Error;                // last error set
char Last_Warning[256];         // text of last warning
int bLast_Warning;              // last warning set
char bDevice = 0;               // was device found?

//------------------------------------------------------------------------------------------------
// function declarations
//------------------------------------------------------------------------------------------------
int adas_ManagerProc(void *lpParameter);
int adas_Translate_Wave(char *p_Wave_Name);
char *adas_Translate_Index(int Wave_Index);
int adas_Get_Free_Source(void);
int adas_Find_Sound(int Wave_Index);
int adas_Load_Wave(ADAS_SOUND_SOURCE * p_ss);
void adas_Release(ADAS_SOUND_SOURCE * p_ss);
int adas_Run_Manager(void);
void adas_Shot_Down_Manager(void);
void adas_Set_Source_Propertiesl(ADAS_SOUND_SOURCE * p_ss, ALuint uiValue,
                                 long lValue);
void adas_Set_Source_Propertiesf(ADAS_SOUND_SOURCE * p_ss, ALuint uiValue,
                                 float fValue);
void adas_Set_Source_Propertiesdw(ADAS_SOUND_SOURCE * p_ss, ALuint uiValue,
                                  unsigned long dwValue);
void adas_Update_Positions(float *v);

//------------------------------------------------------------------------------------------------
// Get error message
//------------------------------------------------------------------------------------------------
int adas_Get_Last_Error(char *p_Text, int Size)
{
  if (bLast_Error) {
    if (Size > 256)
      Size = 256;
    strncpy(p_Text, Last_Error, Size);
    return 1;
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
// Get warning message
//------------------------------------------------------------------------------------------------
int adas_Get_Last_Warning(char *p_Text, int Size)
{
  if (bLast_Warning) {
    if (Size > 256)
      Size = 256;
    strncpy(p_Text, Last_Warning, Size);
    return 1;
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
// reset error & warning message
//------------------------------------------------------------------------------------------------
void adas_Reset_Last_Error(void)
{
  strcpy(Last_Error, "");
  bLast_Error = 0;
  strcpy(Last_Warning, "");
  bLast_Warning = 0;
}


//------------------------------------------------------------------------------------------------
// Set error message
//------------------------------------------------------------------------------------------------
void adas_Set_Last_Error(char *p_Text)
{
  fprintf(stderr,"adas_Set_Last_Error(): %s\n",p_Text);
  strcpy(Last_Error, p_Text);
  bLast_Error = 1;
  assert(0);
}

//------------------------------------------------------------------------------------------------
// Set warning message
//------------------------------------------------------------------------------------------------
void adas_Set_Last_Warning(char *p_Text)
{
  fprintf(stderr,"adas_Set_Last_Error(): %s\n",p_Text);
  strcpy(Last_Warning, p_Text);
  bLast_Warning = 1;
}

//------------------------------------------------------------------------------------------------
// init's AnakreoN Digital Audio System:
//       Following is a brief description of the 3 different implementations 
//       that can be created.
//
//      DirectSound3D
//
//               Uses Direct Sound 3D to render the Open AL audio.  This implementation will
//               benefit from Direct Sound 3D accelerators like the SB Live !  Applications must
//               use this implementation if they wish to use EAX extensions.
//
//      DirectSound
//
//        This implimentation uses software mixing with output to a 
//        DSound stereo buffer. This implimentation offers the best
//        comprimise of speed and latency for all cards. However, EAX
//        extensions will not be available.
//
//      Default
//
//        This implimentation utilizes software mixing with output to 
//        mmsystem. It offers the best solution across Win9x and NT 
//        platforms. It does however offer the highest latency.
//------------------------------------------------------------------------------------------------
void adas_Init(ADAS_INIT_DATA * p_adas_data)
{
  int i, j;
  float v_3[] = { 0.0, 0.0, 0.0 };
  float v_6[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
  ADAS_SOUND_SOURCE *pSource;

  memcpy(&ADAS_data, p_adas_data, sizeof(ADAS_INIT_DATA));

  p_Device = alcOpenDevice((ALCubyte *) p_adas_data->Implementation);
  if (p_Device) {
    bDevice = 1;
    p_Context = alcCreateContext(p_Device, NULL);

    if (p_Context) {
      //Set active context
      if (alcMakeContextCurrent(p_Context)) {
        int bEAXExtPresent = alIsExtensionPresent((ALubyte *) "EAX2.0");

        if (!bEAXExtPresent) {
          adas_Set_Last_Warning("No EAX2.0 Extension found");
        }
        else {
          p_EAXSet = (EAXSet) alGetProcAddress((ALubyte *) "EAXSet");
          p_EAXGet = (EAXGet) alGetProcAddress((ALubyte *) "EAXGet");

          if (!p_EAXSet || !p_EAXGet) {
            adas_Set_Last_Error("Cannot find EAXSet and/or EAXGet function");
            bEAXExtPresent = 0;
          }

          if (bEAXExtPresent) {
            // The DirectSound3D implementation of Open AL will hold on to any EAX parameters
            // IF no Open AL Sources have been generated.  Because we may wish to update the 3D
            // graphs of the EAX Listener parameters before the user loads a sample, we generate
            // a dummy source here.  (Under normal circumstances there is no reason to do this)
            alGetError();
            alGenSources(1, &p_DummySource);
            if (alGetError() != AL_NO_ERROR) {
              adas_Set_Last_Error("Unable to create source");
              return;
            }
            if (!alIsSource(p_DummySource)) {
              adas_Set_Last_Error("Unable to create source");
              return;
            }
          }
        }

        SoundSource =
          (ADAS_SOUND_SOURCE *) mmalloc(ADAS_data.Channels *
                                        sizeof(ADAS_SOUND_SOURCE));
        if (!SoundSource) {
          adas_Set_Last_Error("Out of Memory");
          return;
        }

        for (i = 0; i < ADAS_data.Channels; i++) {
          pSource = &SoundSource[i];
          pSource->Source = 0;

          for (j = 0; j < MAXQUEUED; j++) {
            pSource->Buffer[j] = 0;
            pSource->Wave_Index[j] = UNDEFINED_VALUE;
            pSource->Buffer_Source[j] = UNDEFINED_VALUE;
          }

          pSource->Owner = UNDEFINED_VALUE;
          pSource->Type = UNDEFINED_VALUE;
          pSource->Buffer_Pointer = 0;
        }

        adas_Run_Manager();

        adas_Set_Listener_Position(v_3);
        adas_Set_Listener_Velocity(v_3);
        adas_Set_Listener_Orientation(v_6);

        if(!alutInitWithoutContext(NULL,NULL)) {
          adas_Set_Last_Error("Unable to init ALUT.");
        
          alcDestroyContext(p_Context);
          alcCloseDevice(p_Device);
          p_Context = NULL;
          p_Device = NULL;        
        }
            
        // succesfull exit
        return;
      }
      else {
        adas_Set_Last_Error("Unable to set active context.");
        bDevice = 1;
      }

      alcDestroyContext(p_Context);
      p_Context = NULL;
    }
    else {
      adas_Set_Last_Error("Unable to create context.");
      bDevice = 1;
    }

    alcCloseDevice(p_Device);
    p_Device = NULL;
  }
  else
    adas_Set_Last_Error("Unable to open device.");
}

//------------------------------------------------------------------------------------------------
// releases AnakreoN Digital Audio System
//------------------------------------------------------------------------------------------------
void adas_Exit(void)
{
  if (!bDevice)
    return;

  adas_Shot_Down_Manager();

  if (p_DummySource)
    alDeleteSources(1, &p_DummySource);

  alutExit();
  
  alcDestroyContext(p_Context);
  p_Context = 0;
  
  alcCloseDevice(p_Device);
  p_Device = 0;

  free((void *)SoundSource);
  SoundSource = NULL;
}

//------------------------------------------------------------------------------------------------
// load sound data & init memory
//------------------------------------------------------------------------------------------------
unsigned long adas_Load_First(char *p_Index_File, char *p_File_Name)
{
  FILE *file = NULL;
  char data[100];
  unsigned long Return;
  int i;
  ALboolean loop = 0;
  ADAS_SOUND_DATA *pSound;

  if (!bDevice)
    return 0;

  if ((!p_File_Name) || (!p_Index_File))
    return 0;

  SoundData =
    (ADAS_SOUND_DATA *) malloc(SIZEOFSOUNDDATA * sizeof(ADAS_SOUND_DATA));
  if (!SoundData) {
    adas_Set_Last_Error("Out of memory");
    return 0;
  }

  file = fopen(p_Index_File, "r");
  if (!file) {
    adas_Set_Last_Error("Index file not found");
    return 0;
  }

  fgets(data, 100, file);
  Size_of_Indexes = atoi(data);
  WaveFile = (ADAS_WAVEFILEDESC *) malloc(Size_of_Indexes * sizeof(ADAS_WAVEFILEDESC));
  if (!WaveFile) {
    adas_Set_Last_Error("Out of memory");
    fclose(file);
    file = NULL;
    return 0;
  }

  for (i = 0; i < Size_of_Indexes; i++) {
    fgets(data, 100, file);
    newline_cut(data);
    strcpy(WaveFile[i].Name, data);
    fgets(data, 100, file);
    WaveFile[i].Index = atoi(data);
  }
  fclose(file);
  file = NULL;

  file = fopen(p_File_Name, "r");
  if (file) {
    GetFileSize(file, &Return);
    fclose(file);
    Size_of_Sound_Data = 0;
    pSound = &SoundData[Size_of_Sound_Data];

    alGetError();
    alGenBuffers(1, &pSound->Buffer);
    if (alGetError() != AL_NO_ERROR) {
      adas_Set_Last_Error("Unable to create buffer");
      return 0;
    }

    if (!alIsBuffer(pSound->Buffer)) {
      adas_Set_Last_Error("Unable to create buffer");
      return 0;
    }

    alutLoadWAVFile(p_File_Name, &pSound->Format, &pSound->Data,
                    &pSound->Size, &pSound->Frequece, &loop);

    alBufferData(pSound->Buffer, pSound->Format, pSound->Data, pSound->Size,
                 pSound->Frequece);

    alutUnloadWAV(pSound->Format, pSound->Data, pSound->Size,
                  pSound->Frequece);

    pSound->Wave_Index = adas_Translate_Wave(p_File_Name);
    Size_of_Sound_Data++;

    return Return;
  }
  else {
    adas_Set_Last_Error("File not found");
    return 0;
  }
}

unsigned long adas_Load_FirstMemory(char *p_Index_File, void *p_File,
                                    long File_Size, char *p_File_Name)
{
  FILE *file = NULL;
  char data[100];
  int i;
  ALboolean loop = 0;
  ADAS_SOUND_DATA *pSound;

  if (!bDevice)
    return 0;

  if ((!p_File) || (!p_Index_File) || (!p_File_Name))
    return 0;

  SoundData =
    (ADAS_SOUND_DATA *) mmalloc(SIZEOFSOUNDDATA * sizeof(ADAS_SOUND_DATA));
  if (!SoundData) {
    adas_Set_Last_Error("Out of memory");
    return 0;
  }

  file = fopen(p_Index_File, "r");
  if (!file) {
    adas_Set_Last_Error("Index file not found");
    return 0;
  }


  fgets(data, 100, file);
  Size_of_Indexes = atoi(data);
  WaveFile = (ADAS_WAVEFILEDESC *) mmalloc(Size_of_Indexes *
                                  sizeof(ADAS_WAVEFILEDESC));
  if (!WaveFile) {
    adas_Set_Last_Error("Out of memory");
    fclose(file);
    file = NULL;
    return 0;
  }

  for (i = 0; i < Size_of_Indexes; i++) {
    fgets(data, 100, file);
    newline_cut(data);
    strcpy(WaveFile[i].Name, data);
    fgets(data, 100, file);
    WaveFile[i].Index = atoi(data);
  }
  fclose(file);
  file = NULL;

  Size_of_Sound_Data = 0;
  pSound = &SoundData[Size_of_Sound_Data];

  alGetError();
  alGenBuffers(1, &pSound->Buffer);
  if (alGetError() != AL_NO_ERROR) {
    adas_Set_Last_Error("Unable to create buffer");
    return 0;
  }

  if (!alIsBuffer(pSound->Buffer)) {
    adas_Set_Last_Error("Unable to create buffer");
    return 0;
  }

  if (adasLoadWAVMemory(p_File, File_Size, &pSound->Format, &pSound->Data, &pSound->Size, &pSound->Frequece, &loop)) {
    alBufferData(pSound->Buffer, pSound->Format, pSound->Data, pSound->Size,
                 pSound->Frequece);
    alutUnloadWAV(pSound->Format, pSound->Data, pSound->Size,
                  pSound->Frequece);
    pSound->Wave_Index = adas_Translate_Wave(p_File_Name);
    Size_of_Sound_Data++;
  }
  else {
    adas_Set_Last_Error("Unable to convert audio data (adasLoadWAVMemory)");
    return 0;
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
// load sound data
//------------------------------------------------------------------------------------------------
unsigned long adas_Load_Next(char *p_File_Name)
{
  FILE *file = NULL;
  unsigned long Return;
  ALboolean loop = 0;
  ADAS_SOUND_DATA *pSound;

  if (!bDevice)
    return 0;
  if ((Size_of_Sound_Data > SIZEOFSOUNDDATA) || (!p_File_Name))
    return 0;

  chdir(ADAS_data.Sound_Dir);
  file = fopen(p_File_Name, "r");
  if (file) {
    GetFileSize(file, &Return);
    fclose(file);

    pSound = &SoundData[Size_of_Sound_Data];

    alGetError();
    alGenBuffers(1, &pSound->Buffer);
    if (alGetError() != AL_NO_ERROR) {
      adas_Set_Last_Error("Unable to create buffer");
      return 0;
    }
    if (!alIsBuffer(pSound->Buffer)) {
      adas_Set_Last_Error("Unable to create buffer");
      return 0;
    }

    alutLoadWAVFile(p_File_Name, &pSound->Format, &pSound->Data,
                    &pSound->Size, &pSound->Frequece, &loop);

    alBufferData(pSound->Buffer, pSound->Format, pSound->Data, pSound->Size,
                 pSound->Frequece);

    alutUnloadWAV(pSound->Format, pSound->Data, pSound->Size,
                  pSound->Frequece);

    pSound->Wave_Index = adas_Translate_Wave(p_File_Name);
    Size_of_Sound_Data++;
    return Return;
  }
  else {
    adas_Set_Last_Error("File not found");
    return 0;
  }
}

unsigned long adas_Load_NextMemory(void *p_File, long File_Size,
                                   char *p_File_Name)
{
  FILE *file = NULL;
  ALboolean loop = 0;
  ADAS_SOUND_DATA *pSound;

  if (!bDevice)
    return 0;
  if ((Size_of_Sound_Data > SIZEOFSOUNDDATA) || (!p_File_Name) || (!p_File))
    return 0;


  pSound = &SoundData[Size_of_Sound_Data];

  alGetError();
  alGenBuffers(1, &pSound->Buffer);
  if (alGetError() != AL_NO_ERROR) {
    adas_Set_Last_Error("Unable to create buffer");
    return 0;
  }

  if (!alIsBuffer(pSound->Buffer)) {
    adas_Set_Last_Error("Unable to create buffer");
    return 0;
  }

  if (adasLoadWAVMemory(p_File, File_Size, &pSound->Format, &pSound->Data, &pSound->Size, &pSound->Frequece, &loop)) {
    alBufferData(pSound->Buffer, pSound->Format, pSound->Data, pSound->Size,
                 pSound->Frequece);
    alutUnloadWAV(pSound->Format, pSound->Data, pSound->Size,
                  pSound->Frequece);
    pSound->Wave_Index = adas_Translate_Wave(p_File_Name);
    Size_of_Sound_Data++;
  }
  else {
    adas_Set_Last_Error("Unable to convert audio data (adasLoadWAVMemory)");
    return 0;
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
// releases loaded sound data
//------------------------------------------------------------------------------------------------
void adas_Release_Loaded_Data(void)
{
  int i;

  if (!bDevice)
    return;

  for (i = 0; i < Size_of_Sound_Data; i++)
    alDeleteBuffers(1, &SoundData[i].Buffer);

  assert(WaveFile);
  
  if (WaveFile) {
    free((void *)WaveFile);
    WaveFile = NULL;
  }

  if (SoundData) {
    free((void *)SoundData);
    SoundData = NULL;
  }
}

//------------------------------------------------------------------------------------------------
// Set distance model
//------------------------------------------------------------------------------------------------
int adas_Set_Distance_Model(long Distance_Model)
{
  if (!bDevice)
    return 0;

  alGetError();
  alDistanceModel(Distance_Model);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;

  return 0;
}

//------------------------------------------------------------------------------------------------
// Set distance model
//------------------------------------------------------------------------------------------------
int adas_Set_Doppler_Factor(float Doppler_Factor)
{
  if (!bDevice)
    return 0;

  alGetError();
  alDopplerFactor(Doppler_Factor);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Set distance model
//------------------------------------------------------------------------------------------------
int adas_Set_Doppler_Velocity(float Doppler_Velocity)
{
  if (!bDevice)
    return 0;

  alGetError();
  alDopplerVelocity(Doppler_Velocity);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Set Listener Position
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Position(float *v)
{
  if (!bDevice)
    return 0;

  v[0] *= ADAS_data.Scale_Factor;
  v[1] *= ADAS_data.Scale_Factor;
  v[2] *= ADAS_data.Scale_Factor;
  alGetError();
  alListenerfv(AL_POSITION, v);
  if (alGetError() == AL_NO_ERROR) {
    adas_Update_Positions(v);
    return 1;
  }
  else
    return 0;

  return 0;
}

//------------------------------------------------------------------------------------------------
// Set Listener Gain
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Gain(float v)
{
  if (!bDevice)
    return 0;

  alGetError();
  alListenerf(AL_GAIN, v);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Set Listener Velocity
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Velocity(float *v)
{
  if (!bDevice)
    return 0;

  alGetError();
  alListenerfv(AL_VELOCITY, v);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Set Listener Orientation
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Orientation(float *v)
{
  if (!bDevice)
    return 0;

  alGetError();
  alListenerfv(AL_ORIENTATION, v);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Get Listener Gain
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Gain(float *v)
{
  if (!bDevice)
    return 0;

  alGetError();
  alGetListenerf(AL_GAIN, v);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Get Listener Position
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Position(float *v)
{
  if (!bDevice)
    return 0;

  alGetError();
  alGetListenerfv(AL_POSITION, v);
  v[0] *= 1.0f / ADAS_data.Scale_Factor;
  v[1] *= 1.0f / ADAS_data.Scale_Factor;
  v[2] *= 1.0f / ADAS_data.Scale_Factor;
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Get Listener Velocity
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Velocity(float *v)
{
  if (!bDevice)
    return 0;

  alGetError();
  alGetListenerfv(AL_VELOCITY, v);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Get Listener Orientation
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Orientation(float *v)
{
  if (!bDevice)
    return 0;

  alGetError();
  alGetListenerfv(AL_ORIENTATION, v);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  else
    return 1;
}

//------------------------------------------------------------------------------------------------
// Find free sound source
//------------------------------------------------------------------------------------------------
int adas_Get_Free_Source(void)
{
  int i;

  if (!bDevice)
    return -1;

  for (i = 0; i < ADAS_data.Channels; i++)
    if (!SoundSource[i].Source)
      return i;
  return -1;
}

//------------------------------------------------------------------------------------------------
// Find sound data in memory
//------------------------------------------------------------------------------------------------
int adas_Find_Sound(int Wave_Index)
{
  int i;

  if (!bDevice)
    return -1;

  for (i = 0; i < Size_of_Sound_Data; i++)
    if (SoundData[i].Wave_Index == Wave_Index)
      return i;
  return -1;
}

//------------------------------------------------------------------------------------------------
// Translate wave name to index
//------------------------------------------------------------------------------------------------
int adas_Translate_Wave(char *p_Wave_Name)
{
  int i;

  if (!bDevice)
    return -1;

  for (i = 0; i < Size_of_Indexes; i++)
    if (!strcasecmp(WaveFile[i].Name, p_Wave_Name))
      return WaveFile[i].Index;
  return -1;
}

//------------------------------------------------------------------------------------------------
// Translate wave index to name
//------------------------------------------------------------------------------------------------
char *adas_Translate_Index(int Wave_Index)
{
  int i;

  if (!bDevice)
    return NULL;

  for (i = 0; i < Size_of_Indexes; i++)
    if (WaveFile[i].Index == Wave_Index)
      return WaveFile[i].Name;
  return NULL;
}

//------------------------------------------------------------------------------------------------
// Load wave
//------------------------------------------------------------------------------------------------
int adas_Load_Wave(ADAS_SOUND_SOURCE * p_ss)
{
  char text[100];
  char *name = NULL;
  ALsizei Size;
  ALuint Frequece;
  ALenum Format;
  ALvoid *Data;
  ALboolean loop;

  if (!bDevice)
    return 0;

  alGetError();
  alGenBuffers(1, &p_ss->Buffer[p_ss->Buffer_Pointer]);
  if (alGetError() != AL_NO_ERROR) {
    adas_Set_Last_Error("Unable to create buffer");
    return 0;
  }
  if (!alIsBuffer(p_ss->Buffer[p_ss->Buffer_Pointer])) {
    adas_Set_Last_Error("Unable to create buffer");
    return 0;
  }
  name = adas_Translate_Index(p_ss->Wave_Index[p_ss->Buffer_Pointer]);

  if (!name)
    return 0;

  strcpy(text, name);
  chdir(ADAS_data.Sound_Dir);
  alutLoadWAVFile(text, &Format, &Data, &Size, &Frequece, &loop);
  alBufferData(p_ss->Buffer[p_ss->Buffer_Pointer], Format, Data, Size,
               Frequece);
  alutUnloadWAV(Format, Data, Size, Frequece);
  return 1;
}

//------------------------------------------------------------------------------------------------
// Create Sound Source
//------------------------------------------------------------------------------------------------
int adas_Create_Source(ADAS_SOUND_SOURCE_DATA * p_ssd, void **p_callback)
{
  int Src_Index, Mem_Index;
  ADAS_SOUND_SOURCE *p_ss;

  if (!bDevice)
    return -1;

  Src_Index = adas_Get_Free_Source();
  if (Src_Index == -1)
    return -1;

  p_ss = &SoundSource[Src_Index];
  Mem_Index = adas_Find_Sound(p_ssd->Wave_Index);

  p_ss->Buffer_Pointer = 0;
  p_ss->Wave_Index[0] = p_ssd->Wave_Index;
  p_ss->Owner = p_ssd->Owner;
  p_ss->Type = p_ssd->Type;

  if (p_ssd->Bounding_Object.Type == BOUNDING_BOX) {
    int i;

    memcpy(&p_ss->Bounding_Object, &p_ssd->Bounding_Object,
           sizeof(BOUNDING_OBJECT));

    for (i = 0; i < 3; i++) {
      p_ss->Bounding_Object.LBF_point[i] *= ADAS_data.Scale_Factor;
      p_ss->Bounding_Object.RTB_point[i] *= ADAS_data.Scale_Factor;
    }
  }

  if (p_ssd->Bounding_Object.Type == BOUNDING_SPHERE)
    p_ss->Bounding_Object.Radius =
      p_ssd->Bounding_Object.Radius * ADAS_data.Scale_Factor;

  if (Mem_Index == -1) {
/*		p_ss->Lock = 1;

		if (!adas_Load_Wave(p_ss))
			return -1;

		alGetError();
		alGenSources(1,&p_ss->Source);
		if(alGetError() != AL_NO_ERROR) 
		{
			adas_Set_Last_Error("Unable to create source");
			alDeleteBuffers(1,&p_ss->Buffer[0]);
			return -1;
		}

		if (!alIsSource(p_ss->Source))
		{
			adas_Set_Last_Error("Unable to create source");
			alDeleteBuffers(1,&p_ss->Buffer[0]);
			return -1;
		}

		p_ss->Buffer_Source[p_ss->Buffer_Pointer] = HDD;

		alSourcei(p_ss->Source,AL_BUFFER,p_ss->Buffer[0]);*/    
    return -1;
  }
  else {
    p_ss->Lock = 1;

    alGetError();
    alGenSources(1, &p_ss->Source);
    if (alGetError() != AL_NO_ERROR) {
      adas_Set_Last_Error("Unable to create source");
      return -1;
    }
    if (!alIsSource(p_ss->Source)) {
      adas_Set_Last_Error("Unable to create source");
      return -1;
    }

    p_ss->Buffer_Source[p_ss->Buffer_Pointer] = RAM;

    p_ss->Buffer[0] = SoundData[Mem_Index].Buffer;
    alSourcei(p_ss->Source, AL_BUFFER, p_ss->Buffer[0]);
  }

  p_ssd->Pos[0] *= ADAS_data.Scale_Factor;
  p_ssd->Pos[1] *= ADAS_data.Scale_Factor;
  p_ssd->Pos[2] *= ADAS_data.Scale_Factor;

  alSourcef(p_ss->Source, AL_PITCH, p_ssd->Pitch);
  alSourcef(p_ss->Source, AL_GAIN, p_ssd->Gain);
  alSourcei(p_ss->Source, AL_SOURCE_RELATIVE, p_ssd->Source_Relative);
  alSourcefv(p_ss->Source, AL_POSITION, p_ssd->Pos);
  alSourcefv(p_ss->Source, AL_VELOCITY, p_ssd->Velocity);

  if (p_ss->Type == 2)
    alSourcei(p_ss->Source, AL_LOOPING, 1);
  else
    alSourcei(p_ss->Source, AL_LOOPING, 0);
/*
	if(p_ssd->Obstruction != UNDEFINED_VALUE)
		adas_Set_Source_Obstruction(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->Obstruction);
	if(p_ssd->ObstructionLF != UNDEFINED_VALUE)
		adas_Set_Source_ObstructionLF(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->ObstructionLF);
	if(p_ssd->wall_occlusion != UNDEFINED_VALUE)
		adas_Set_Source_Occlusion(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->wall_occlusion);
	if(p_ssd->wall_occlusionLF != UNDEFINED_VALUE)
		adas_Set_Source_OcclusionLF(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->wall_occlusionLF);
	if(p_ssd->wall_occlusion_room != UNDEFINED_VALUE)
		adas_Set_Source_Occlusion_Room(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->wall_occlusion_room);
	if(p_ssd->out_room != UNDEFINED_VALUE)
		adas_Set_Source_Room(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->out_room);
	if(p_ssd->out_roomHF != UNDEFINED_VALUE)
		adas_Set_Source_RoomHF(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->out_roomHF);
	if(p_ssd->out_room_rolloff != UNDEFINED_VALUE)
		adas_Set_Source_Rolloff(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->out_room_rolloff);
	if(p_ssd->out_air_absorbtion != UNDEFINED_VALUE)
		adas_Set_Source_Absorption(PARTICULAR_SOUND_SOURCE,UNDEFINED_VALUE,Src_Index,p_ssd->out_air_absorbtion);
*/
  alSourcePlay(p_ss->Source);

  *p_callback = &SoundSource[Src_Index].p_callback;
  p_ss->Lock = 0;
  p_ss->Buffer_Pointer++;

  return Src_Index;
}

//------------------------------------------------------------------------------------------------
// Release sound sources
//------------------------------------------------------------------------------------------------
void adas_Release(ADAS_SOUND_SOURCE * p_ss)
{
  int i;

  if (!bDevice)
    return;

  p_ss->Lock = 2;
  alSourceStop(p_ss->Source);
  alDeleteSources(1, &p_ss->Source);


  for (i = 0; i < p_ss->Buffer_Pointer; i++)
    if (p_ss->Buffer[i] && (p_ss->Buffer_Source[i] != RAM))
      alDeleteBuffers(1, &p_ss->Buffer[i]);

  p_ss->Source = 0;

  for (i = 0; i < p_ss->Buffer_Pointer; i++) {
    p_ss->Buffer[i] = 0;
    p_ss->Buffer_Source[i] = UNDEFINED_VALUE;
    p_ss->Wave_Index[i] = UNDEFINED_VALUE;
  }

  p_ss->Owner = UNDEFINED_VALUE;
  p_ss->Type = UNDEFINED_VALUE;

  /*if (p_ss->p_callback)
   *(int *)p_ss->p_callback = 0;*/
}

//------------------------------------------------------------------------------------------------
// Release sound sources
//------------------------------------------------------------------------------------------------
void adas_Release_Source(int Owner, int Type, int Index)
{
  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    adas_Release(&SoundSource[Index]);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner)
        if (Type == ALL_TYPES) {
          if (SoundSource[i].Type != SCREAM_TYPE)
            adas_Release(&SoundSource[i]);
        }
        else if (SoundSource[i].Type == Type)
          adas_Release(&SoundSource[i]);
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        adas_Release(&SoundSource[i]);
  }
}

//------------------------------------------------------------------------------------------------
// Set cube position
//------------------------------------------------------------------------------------------------
void adas_Set_Bounding_Object_PositionA(ADAS_SOUND_SOURCE * p_source,
                                        float *v)
{
  float size[3];
  int i;

  if (p_source->Bounding_Object.Type == BOUNDING_SPHERE)
    for (i = 0; i < 3; i++)
      p_source->Bounding_Object.LBF_point[i] = v[i];
  else {
    for (i = 0; i < 3; i++)
      size[i] =
        (p_source->Bounding_Object.RTB_point[i] -
         p_source->Bounding_Object.LBF_point[i]) / 2.0f;

    for (i = 0; i < 3; i++) {
      p_source->Bounding_Object.LBF_point[i] = v[i] - size[i];
      p_source->Bounding_Object.RTB_point[i] = v[i] + size[i];
    }
  }
}

void adas_Set_Bounding_Object_Position(int Owner, int Type, int Index,
                                       float *v)
{
  if (!bDevice)
    return;

  v[0] *= ADAS_data.Scale_Factor;
  v[1] *= ADAS_data.Scale_Factor;
  v[2] *= ADAS_data.Scale_Factor;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    adas_Set_Bounding_Object_PositionA(&SoundSource[Index], v);


  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES)
          adas_Set_Bounding_Object_PositionA(&SoundSource[i], v);
        else if (SoundSource[i].Type == Type)
          adas_Set_Bounding_Object_PositionA(&SoundSource[i], v);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        adas_Set_Bounding_Object_PositionA(&SoundSource[i], v);
  }
}

//------------------------------------------------------------------------------------------------
// Set sound sources position
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Position(int Owner, int Type, int Index, float *v)
{
  if (!bDevice)
    return;

  v[0] *= ADAS_data.Scale_Factor;
  v[1] *= ADAS_data.Scale_Factor;
  v[2] *= ADAS_data.Scale_Factor;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcefv(SoundSource[Index].Source, AL_POSITION, v);


  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcefv(SoundSource[i].Source, AL_POSITION, v);
        else if (SoundSource[i].Type == Type)
          alSourcefv(SoundSource[i].Source, AL_POSITION, v);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcefv(SoundSource[i].Source, AL_POSITION, v);
  }
}


void adas_Update_Cube_Pos(ADAS_SOUND_SOURCE * p_s, int index, float *v)
{
  int j;
  float pos[3];

  for (j = 0; j < 3; j++) {
    if (v[j] < p_s->Bounding_Object.LBF_point[j])
      pos[j] = p_s->Bounding_Object.LBF_point[j];
    else if (v[j] > p_s->Bounding_Object.RTB_point[j])
      pos[j] = p_s->Bounding_Object.RTB_point[j];
    else
      pos[j] = v[j];
  }

  adas_Set_Source_Position(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, index,
                           pos);
}

void adas_Update_Sphere_Pos(ADAS_SOUND_SOURCE * p_s, int index, float *v)
{
  float Direction[3], Distance, SourcePos[3];

  adas_Get_Source_Position(index, SourcePos);

  Sub_Vector(v, SourcePos, Direction);

  Distance = Vector_Length(Direction);

  if (Distance <= p_s->Bounding_Object.Radius) {
    adas_Set_Source_Position(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, index,
                             v);
  }

  Normalize_Vector(Direction);
  Multiple_Vector(Direction, p_s->Bounding_Object.Radius);

  Add_Vector(SourcePos, Direction, SourcePos);
  adas_Set_Source_Position(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, index,
                           SourcePos);
}

//------------------------------------------------------------------------------------------------
// Update position of floating sound sources 
//------------------------------------------------------------------------------------------------
void adas_Update_Positions(float *v)
{
  int i;
  ADAS_SOUND_SOURCE *p_s;


  for (i = 0; i < ADAS_data.Channels; i++) {
    p_s = &SoundSource[i];
    if (p_s->Lock != 2)
      switch (p_s->Bounding_Object.Type) {
        case BOUNDING_BOX:
          adas_Update_Cube_Pos(p_s, i, v);
          break;
        case BOUNDING_SPHERE:
          adas_Update_Sphere_Pos(p_s, i, v);
          break;
      }
  }
}

//------------------------------------------------------------------------------------------------
// Set sound sources pitch
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Pitch(int Owner, int Type, int Index, float v)
{
  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcef(SoundSource[Index].Source, AL_PITCH, v);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcef(SoundSource[i].Source, AL_PITCH, v);
        else if (SoundSource[i].Type == Type)
          alSourcef(SoundSource[i].Source, AL_PITCH, v);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcef(SoundSource[i].Source, AL_PITCH, v);
  }
}

//------------------------------------------------------------------------------------------------
// Set sound sources gain
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Gain(int Owner, int Type, int Index, float v)
{
  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcef(SoundSource[Index].Source, AL_GAIN, v);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcef(SoundSource[i].Source, AL_GAIN, v);
        else if (SoundSource[i].Type == Type)
          alSourcef(SoundSource[i].Source, AL_GAIN, v);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcef(SoundSource[i].Source, AL_GAIN, v);
  }
}

//------------------------------------------------------------------------------------------------
// Set sound sources min. gain
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Min_Gain(int Owner, int Type, int Index, float v)
{
  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcef(SoundSource[Index].Source, AL_MIN_GAIN, v);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcef(SoundSource[i].Source, AL_MIN_GAIN, v);
        else if (SoundSource[i].Type == Type)
          alSourcef(SoundSource[i].Source, AL_MIN_GAIN, v);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcef(SoundSource[i].Source, AL_MIN_GAIN, v);
  }
}

//------------------------------------------------------------------------------------------------
// Set sound sources max. gain
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Max_Gain(int Owner, int Type, int Index, float v)
{
  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcef(SoundSource[Index].Source, AL_MAX_GAIN, v);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcef(SoundSource[i].Source, AL_MAX_GAIN, v);
        else if (SoundSource[i].Type == Type)
          alSourcef(SoundSource[i].Source, AL_MAX_GAIN, v);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcef(SoundSource[i].Source, AL_MAX_GAIN, v);
  }
}

//------------------------------------------------------------------------------------------------
// Set sound sources velocity
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Velocity(int Owner, int Type, int Index, float *v)
{
  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcefv(SoundSource[Index].Source, AL_VELOCITY, v);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcefv(SoundSource[i].Source, AL_VELOCITY, v);
        else if (SoundSource[i].Type == Type)
          alSourcefv(SoundSource[i].Source, AL_VELOCITY, v);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcefv(SoundSource[i].Source, AL_VELOCITY, v);
  }
}

//------------------------------------------------------------------------------------------------
// Pause sound source
//------------------------------------------------------------------------------------------------
void adas_Pause_Source(int Owner, int Type, int Index)
{
  if (!bDevice)
    return;

  if ((Owner == PARTICULAR_SOUND_SOURCE) && SoundSource[Index].Source)
    alSourcePause(SoundSource[Index].Source);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if ((Type == ALL_TYPES) && SoundSource[i].Source)
          alSourcePause(SoundSource[i].Source);
        else
          if ((SoundSource[i].Type == Type)
              && (SoundSource[i].Source != UNDEFINED_VALUE))
          alSourcePause(SoundSource[i].Source);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcePause(SoundSource[i].Source);
  }
}

//------------------------------------------------------------------------------------------------
// Pause sound source
//------------------------------------------------------------------------------------------------
void adas_Resume_Source(int Owner, int Type, int Index)
{
  if (!bDevice)
    return;

  if ((Owner == PARTICULAR_SOUND_SOURCE) && SoundSource[Index].Source)
    alSourcePlay(SoundSource[Index].Source);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if ((Type == ALL_TYPES) && SoundSource[i].Source)
          alSourcePlay(SoundSource[i].Source);
        else if ((SoundSource[i].Type == Type) && SoundSource[i].Source)
          alSourcePlay(SoundSource[i].Source);
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        alSourcePlay(SoundSource[i].Source);
  }
}

//------------------------------------------------------------------------------------------------
// cheks, if is particular owher playing selected sound data
//------------------------------------------------------------------------------------------------
int adas_Is_Sound_Processed(int Wave_Index, int Owner)
{
  int i;

  if (!bDevice)
    return 0;

  for (i = 0; i < ADAS_data.Channels; i++)
    if ((SoundSource[i].Owner == Owner)
        && (SoundSource[i].Wave_Index[0] == Wave_Index))
      return 1;

  return 0;
}

//------------------------------------------------------------------------------------------------
// sel all sources volume (gain)
//------------------------------------------------------------------------------------------------
void adas_Set_All_Source_Volume(float fValue)
{
  int i;

  if (!bDevice)
    return;
  if (fValue < 0)
    fValue = 0.001f;
  if (fValue > 1)
    fValue = 1.0f;

  for (i = 0; i < ADAS_data.Channels; i++)
    if (SoundSource[i].Source)
      alSourcef(SoundSource[i].Source, AL_GAIN, fValue);
}

//------------------------------------------------------------------------------------------------
// run sound source manager
//------------------------------------------------------------------------------------------------
int adas_Run_Manager(void)
{
  if (!bDevice)
    return 0;
  Manager.Shot_down = 0;
  Manager.Thread = CreateThread(NULL, 16384, adas_ManagerProc,
                                (void *)&Manager, 0, &(Manager.ThreadID));
  if (Manager.Thread)
    return 1;
  else
    return 0;
}

//----------------------------------------------------------------------------------------------
// sound source manager thread
//----------------------------------------------------------------------------------------------
int adas_ManagerProc(void *lpParameter)
{
  ADAS_MANAGER *p_m = (ADAS_MANAGER *) lpParameter;
  int i, status;

  if (!bDevice)
    return 0;

  while (!p_m->Shot_down) {
    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source) {
        alGetSourcei(SoundSource[i].Source, AL_SOURCE_STATE, &status);
        if ((status == AL_STOPPED) && !SoundSource[i].Lock)
          adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, i);
      }

    Sleep(250);
  }
  return 0;
}

//------------------------------------------------------------------------------------------------
// run sound source manager
//------------------------------------------------------------------------------------------------
void adas_Shot_Down_Manager(void)
{
  unsigned long Exit_Code;

  if (!bDevice)
    return;
// TODO
#ifdef WIDOWS
  Manager.Shot_down = 1;
  Sleep(500);
  GetExitCodeThread(Manager.Thread, &Exit_Code);
  if ((Exit_Code != STILL_ACTIVE)) {
    CloseHandle(Manager.Thread);
    Manager.Thread = 0;
    Manager.ThreadID = 0;
  }
  else {
    TerminateThread(Manager.Thread, 1);
    CloseHandle(Manager.Thread);
    Manager.Thread = 0;
    Manager.ThreadID = 0;
  }
#endif
}

#ifdef WIDOWS
//------------------------------------------------------------------------------------------------
// set all listeners parametrs
//------------------------------------------------------------------------------------------------
int adas_Set_All_Listener_Properties(LPEAXLISTENERPROPERTIES lpData)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ALLPARAMETERS,
                    0, lpData, sizeof(EAXLISTENERPROPERTIES));
  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners room
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Room(long lValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ROOM, 0, &lValue, sizeof(long));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners roomHF
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_RoomHF(long lValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ROOMHF, 0, &lValue, sizeof(long));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners room rolloff
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Room_Rolloff(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR,
                    0, &fValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners room decay time
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Decay_Time(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_DECAYTIME,
                    0, &fValue, sizeof(float));

  return 0;
}


//------------------------------------------------------------------------------------------------
// set listeners room decay HFRatio
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Decay_HFRatio(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_DECAYHFRATIO,
                    0, &fValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners reflections
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Reflections(long lValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REFLECTIONS,
                    0, &lValue, sizeof(long));

  return 0;
}


//------------------------------------------------------------------------------------------------
// set listeners reflections delay
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Reflections_Delay(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY,
                    0, &fValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners reverb
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Reverb(LONG lValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REVERB, 0, &lValue, sizeof(long));

  return 0;
}


//------------------------------------------------------------------------------------------------
// set listeners reverb delay
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Reverb_Delay(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REVERBDELAY,
                    0, &fValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners environment
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Environment(unsigned long dwValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ENVIRONMENT,
                    0, &dwValue, sizeof(unsigned long));

  return 0;
}


//------------------------------------------------------------------------------------------------
// set listeners environment size
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Environment_Size(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE,
                    0, &fValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners environment diffusion
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Environment_Diffusion(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,
                    0, &fValue, sizeof(float));

  return 0;
}


//------------------------------------------------------------------------------------------------
// set listeners air absoprtion
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Air_Absorption(float fValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF,
                    0, &fValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners flags
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Flags(unsigned long dwValue)
{
  if (p_EAXSet)
    return p_EAXSet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_FLAGS,
                    0, &dwValue, sizeof(unsigned long));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners scale delay time
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Scale_Decay_Time(int bValue)
{
  if (p_EAXGet) {
    unsigned long dwFlags;

    p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
             DSPROPERTY_EAXLISTENER_FLAGS,
             0, &dwFlags, sizeof(unsigned long));

    dwFlags &= (0xFFFFFFFF ^ EAXLISTENERFLAGS_DECAYTIMESCALE);
    if (bValue)
      dwFlags |= EAXLISTENERFLAGS_DECAYTIMESCALE;

    return adas_Set_Listener_Flags(dwFlags);
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners scale reflections
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Scale_Reflections(int bValue)
{
  if (p_EAXGet) {
    unsigned long dwFlags;

    p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
             DSPROPERTY_EAXLISTENER_FLAGS,
             0, &dwFlags, sizeof(unsigned long));

    dwFlags &= (0xFFFFFFFF ^ EAXLISTENERFLAGS_REFLECTIONSSCALE);
    if (bValue)
      dwFlags |= EAXLISTENERFLAGS_REFLECTIONSSCALE;

    return adas_Set_Listener_Flags(dwFlags);
  }


  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners scale reflections delay
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Scale_Reflections_Delay(int bValue)
{
  if (p_EAXGet) {
    unsigned long dwFlags;

    p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
             DSPROPERTY_EAXLISTENER_FLAGS,
             0, &dwFlags, sizeof(unsigned long));

    dwFlags &= (0xFFFFFFFF ^ EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE);
    if (bValue)
      dwFlags |= EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE;

    return adas_Set_Listener_Flags(dwFlags);
  }


  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners scale reverb
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Scale_Reverb(int bValue)
{
  if (p_EAXGet) {
    unsigned long dwFlags;

    p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
             DSPROPERTY_EAXLISTENER_FLAGS,
             0, &dwFlags, sizeof(unsigned long));

    dwFlags &= (0xFFFFFFFF ^ EAXLISTENERFLAGS_REVERBSCALE);
    if (bValue)
      dwFlags |= EAXLISTENERFLAGS_REVERBSCALE;

    return adas_Set_Listener_Flags(dwFlags);
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners scale reverb delay
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Scale_Reverb_Delay(int bValue)
{
  if (p_EAXGet) {
    unsigned long dwFlags;

    p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
             DSPROPERTY_EAXLISTENER_FLAGS,
             0, &dwFlags, sizeof(unsigned long));

    dwFlags &= (0xFFFFFFFF ^ EAXLISTENERFLAGS_REVERBDELAYSCALE);
    if (bValue)
      dwFlags |= EAXLISTENERFLAGS_REVERBDELAYSCALE;

    return adas_Set_Listener_Flags(dwFlags);
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listeners clip decayHF
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Clip_DecayHF(int bValue)
{
  if (p_EAXGet) {
    unsigned long dwFlags;

    p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
             DSPROPERTY_EAXLISTENER_FLAGS,
             0, &dwFlags, sizeof(unsigned long));

    dwFlags &= (0xFFFFFFFF ^ EAXLISTENERFLAGS_DECAYHFLIMIT);
    if (bValue)
      dwFlags |= EAXLISTENERFLAGS_DECAYHFLIMIT;

    return adas_Set_Listener_Flags(dwFlags);
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
// get all listener properties
//------------------------------------------------------------------------------------------------
int adas_Get_All_Listener_Properties(LPEAXLISTENERPROPERTIES lpData)
{
  if (p_EAXGet)
    return p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_ALLPARAMETERS,
                    0, lpData, sizeof(EAXLISTENERPROPERTIES));

  return 0;
}

//------------------------------------------------------------------------------------------------
// get listener decay time
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Decay_Time(float *pfValue)
{
  if (p_EAXGet)
    return p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_DECAYTIME,
                    0, pfValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// get listener Reflections
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Reflections(long *plValue)
{
  if (p_EAXGet)
    return p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REFLECTIONS,
                    0, plValue, sizeof(long));

  return 0;
}

//------------------------------------------------------------------------------------------------
// get listener Reflections Delay
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Reflections_Delay(float *pfValue)
{
  if (p_EAXGet)
    return p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY,
                    0, pfValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// get listener reverb
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Reverb(long *plValue)
{
  if (p_EAXGet)
    return p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REVERB, 0, plValue, sizeof(long));

  return 0;
}

//------------------------------------------------------------------------------------------------
// get listener reverb delay
//------------------------------------------------------------------------------------------------
int adas_Get_Listener_Reverb_Delay(float *pfValue)
{
  if (p_EAXGet)
    return p_EAXGet(&DSPROPSETID_EAX_ListenerProperties,
                    DSPROPERTY_EAXLISTENER_REVERBDELAY,
                    0, pfValue, sizeof(float));

  return 0;
}

//------------------------------------------------------------------------------------------------
// set listener rolloff
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Rolloff(float fValue)
{
  if (!bDevice)
    return 0;

  alListenerf(AL_ROLLOFF_FACTOR, fValue);

  return 0;
}

//------------------------------------------------------------------------------------------------
// set all source properties
//------------------------------------------------------------------------------------------------
void adas_Set_Source_All_Properties(int Owner, int Type, int Index,
                                    LPEAXBUFFERPROPERTIES lpData)
{
  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    if (p_EAXSet)
      p_EAXSet(&DSPROPSETID_EAX_BufferProperties,
               DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
               SoundSource[Index].Source,
               lpData, sizeof(EAXBUFFERPROPERTIES));

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Owner == Owner) {
        if (Type == ALL_TYPES) {
          if (p_EAXSet)
            p_EAXSet(&DSPROPSETID_EAX_BufferProperties,
                     DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
                     SoundSource[i].Source,
                     lpData, sizeof(EAXBUFFERPROPERTIES));
        }
        else if (SoundSource[i].Type == Type) {
          if (p_EAXSet)
            p_EAXSet(&DSPROPSETID_EAX_BufferProperties,
                     DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
                     SoundSource[i].Source,
                     lpData, sizeof(EAXBUFFERPROPERTIES));
        }
      }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++)
      if (SoundSource[i].Source)
        if (p_EAXSet)
          p_EAXSet(&DSPROPSETID_EAX_BufferProperties,
                   DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
                   SoundSource[i].Source,
                   lpData, sizeof(EAXBUFFERPROPERTIES));

  }
}

//------------------------------------------------------------------------------------------------
// set source properties
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Propertiesl(ADAS_SOUND_SOURCE * p_ss, ALuint uiValue,
                                 long lValue)
{
  if (p_EAXSet)
    p_EAXSet(&DSPROPSETID_EAX_BufferProperties,
             uiValue, p_ss->Source, &lValue, sizeof(LONG));
}

void adas_Set_Source_Propertiesf(ADAS_SOUND_SOURCE * p_ss, ALuint uiValue,
                                 float fValue)
{
  if (p_EAXSet)
    p_EAXSet(&DSPROPSETID_EAX_BufferProperties,
             uiValue, p_ss->Source, &fValue, sizeof(long));
}

void adas_Set_Source_Propertiesdw(ADAS_SOUND_SOURCE * p_ss, ALuint uiValue,
                                  unsigned long dwValue)
{
  if (p_EAXSet)
    p_EAXSet(&DSPROPSETID_EAX_BufferProperties,
             uiValue, p_ss->Source, &dwValue, sizeof(long));
}

//------------------------------------------------------------------------------------------------
// set source properties
//------------------------------------------------------------------------------------------------
void adas_Set_Source_lEAXproperty(int Owner, int Type, int Index, long lValue,
                                  ALuint Property)
{
  ADAS_SOUND_SOURCE *p_Source;

  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    adas_Set_Source_Propertiesl(&SoundSource[Index], Property, lValue);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Owner == Owner) {
        if (Type == ALL_TYPES)
          adas_Set_Source_Propertiesl(p_Source, Property, lValue);
        else if (p_Source->Type == Type)
          adas_Set_Source_Propertiesl(p_Source, Property, lValue);
      }
    }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Source)
        adas_Set_Source_Propertiesl(p_Source, Property, lValue);
    }
  }
}

void adas_Set_Source_fEAXproperty(int Owner, int Type, int Index,
                                  float fValue, ALuint Property)
{
  ADAS_SOUND_SOURCE *p_Source;

  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    adas_Set_Source_Propertiesf(&SoundSource[Index], Property, fValue);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Owner == Owner) {
        if (Type == ALL_TYPES)
          adas_Set_Source_Propertiesf(p_Source, Property, fValue);
        else if (p_Source->Type == Type)
          adas_Set_Source_Propertiesf(p_Source, Property, fValue);
      }
    }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Source)
        adas_Set_Source_Propertiesf(p_Source, Property, fValue);
    }
  }
}

void adas_Set_Source_dwEAXproperty(int Owner, int Type, int Index,
                                   unsigned long dwValue, ALuint Property)
{
  ADAS_SOUND_SOURCE *p_Source;

  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    adas_Set_Source_Propertiesdw(&SoundSource[Index], Property, dwValue);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Owner == Owner) {
        if (Type == ALL_TYPES)
          adas_Set_Source_Propertiesdw(p_Source, Property, dwValue);
        else if (p_Source->Type == Type)
          adas_Set_Source_Propertiesdw(p_Source, Property, dwValue);
      }
    }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Source)
        adas_Set_Source_Propertiesdw(p_Source, Property, dwValue);
    }
  }
}
#endif

#ifdef LINUX
//------------------------------------------------------------------------------------------------
// set listeners environment
//------------------------------------------------------------------------------------------------
int adas_Set_Listener_Environment(unsigned long dwValue)
{
  return 0;
}
#endif

//------------------------------------------------------------------------------------------------
// set source OPENAL properties
//------------------------------------------------------------------------------------------------
void adas_Set_Source_fOPENALproperty(int Owner, int Type, int Index,
                                     float fValue, ALenum Property)
{
  ADAS_SOUND_SOURCE *p_Source;

  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcef(SoundSource[Index].Source, Property, fValue);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcef(SoundSource[Index].Source, Property, fValue);
        else if (p_Source->Type == Type)
          alSourcef(p_Source->Source, Property, fValue);
      }
    }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Source)
        alSourcef(p_Source->Source, Property, fValue);
    }
  }
}

void adas_Set_Source_vOPENALproperty(int Owner, int Type, int Index,
                                     float *vValue, ALenum Property)
{
  ADAS_SOUND_SOURCE *p_Source;

  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcefv(SoundSource[Index].Source, Property, vValue);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcefv(p_Source->Source, Property, vValue);
        else if (p_Source->Type == Type)
          alSourcefv(p_Source->Source, Property, vValue);
      }
    }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Source)
        alSourcefv(p_Source->Source, Property, vValue);
    }
  }
}

void adas_Set_Source_dwOPENALproperty(int Owner, int Type, int Index,
                                      unsigned long dwValue, ALenum Property)
{
  ADAS_SOUND_SOURCE *p_Source;

  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcei(SoundSource[Index].Source, Property, dwValue);

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcei(p_Source->Source, Property, dwValue);
        else if (p_Source->Type == Type)
          alSourcei(p_Source->Source, Property, dwValue);
      }
    }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Source)
        alSourcei(p_Source->Source, Property, dwValue);
    }
  }
}

//------------------------------------------------------------------------------------------------
// set source cone outside volume
//------------------------------------------------------------------------------------------------
void adas_Set_Source_Cone_Outside_Volume(int Owner, int Type, int Index,
                                         long lValue)
{
  ADAS_SOUND_SOURCE *p_Source;

  if (!bDevice)
    return;

  if (Owner == PARTICULAR_SOUND_SOURCE)
    alSourcef(SoundSource[Index].Source, AL_CONE_OUTER_GAIN,
              (float)pow(10, (float)lValue / 2000.0f));

  if (Owner < PARTICULAR_SOUND_SOURCE) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Owner == Owner) {
        if (Type == ALL_TYPES)
          alSourcef(p_Source->Source, AL_CONE_OUTER_GAIN,
                    (float)pow(10, (float)lValue / 2000.0f));
        else if (p_Source->Type == Type)
          alSourcef(p_Source->Source, AL_CONE_OUTER_GAIN,
                    (float)pow(10, (float)lValue / 2000.0f));
      }
    }
  }

  if (Owner == ALL_SOUND_SOURCES) {
    int i;

    for (i = 0; i < ADAS_data.Channels; i++) {
      p_Source = &SoundSource[i];

      if (p_Source->Source)
        alSourcef(p_Source->Source, AL_CONE_OUTER_GAIN,
                  (float)pow(10, (float)lValue / 2000.0f));
    }
  }
}

//------------------------------------------------------------------------------------------------
// get sound of sound sources
//------------------------------------------------------------------------------------------------
int adas_Query_Sources(void)
{
  int i, r = 0;

  if (!bDevice)
    return 0;

  for (i = 0; i < ADAS_data.Channels; i++)
    if (SoundSource[i].Source)
      r++;

  return r;
}

//------------------------------------------------------------------------------------------------
// get source position
//------------------------------------------------------------------------------------------------
int adas_Get_Source_Position(int Index, float *fValue)
{
  if (!bDevice)
    return 0;

  alGetError();
  alGetSourcefv(SoundSource[Index].Source, AL_POSITION, fValue);
  fValue[0] *= 1.0f / ADAS_data.Scale_Factor;
  fValue[1] *= 1.0f / ADAS_data.Scale_Factor;
  fValue[2] *= 1.0f / ADAS_data.Scale_Factor;
  if (alGetError() != AL_NO_ERROR)
    return 1;
  else
    return 0;
}

//------------------------------------------------------------------------------------------------
// queue a sound to source
//------------------------------------------------------------------------------------------------
int adas_Queue_Sound(int Source, int Wave_Index)
{
  int Mem_Index;
  ADAS_SOUND_SOURCE *p_ss;

  if (!bDevice)
    return 0;

  if (!SoundSource[Source].Source)
    return 0;

  p_ss = &SoundSource[Source];

  if (p_ss->Buffer_Pointer > MAXQUEUED)
    return 0;

  Mem_Index = adas_Find_Sound(Wave_Index);

  p_ss->Wave_Index[p_ss->Buffer_Pointer] = Wave_Index;

  if (Mem_Index == -1) {
    p_ss->Lock = 1;

    if (!adas_Load_Wave(p_ss))
      return 0;

    alSourceQueueBuffers(p_ss->Source, 1,
                         &p_ss->Buffer[p_ss->Buffer_Pointer]);

    p_ss->Buffer_Source[p_ss->Buffer_Pointer] = HDD;
    p_ss->Lock = 0;
  }
  else {
    p_ss->Lock = 1;

    p_ss->Buffer[p_ss->Buffer_Pointer] = SoundData[Mem_Index].Buffer;
    alSourceQueueBuffers(p_ss->Source, 1,
                         &p_ss->Buffer[p_ss->Buffer_Pointer]);

    p_ss->Buffer_Source[p_ss->Buffer_Pointer] = RAM;
    p_ss->Lock = 0;
  }

  p_ss->Buffer_Pointer++;

  return 1;
}

//------------------------------------------------------------------------------------------------
// unqueue a sound to source
//------------------------------------------------------------------------------------------------
void adas_Unqueue_Sound(int Source, int Sounds)
{
  int i;
  ADAS_SOUND_SOURCE *p_ss;

  p_ss = &SoundSource[Source];

  for (i = 0; i < Sounds; i++) {
    alSourceUnqueueBuffers(p_ss->Source, 1, &p_ss->Buffer[i]);

    if ((p_ss->Buffer_Source[i] != RAM)
        && (p_ss->Buffer_Source[i] != UNDEFINED_VALUE))
      alDeleteBuffers(1, &p_ss->Buffer[i]);

    p_ss->Buffer_Source[i] = UNDEFINED_VALUE;
    p_ss->Buffer[i] = 0;
    p_ss->Wave_Index[i] = UNDEFINED_VALUE;

  }

  memcpy((void *)&p_ss->Buffer_Source[0],
         (void *)&p_ss->Buffer_Source[Sounds],
         (p_ss->Buffer_Pointer - Sounds) * sizeof(int));

  memcpy((void *)&p_ss->Buffer[0],
         (void *)&p_ss->Buffer[Sounds],
         (p_ss->Buffer_Pointer - Sounds) * sizeof(ALuint));

  memcpy((void *)&p_ss->Wave_Index[0],
         (void *)&p_ss->Wave_Index[Sounds],
         (p_ss->Buffer_Pointer - Sounds) * sizeof(ALuint));

  p_ss->Buffer_Pointer -= Sounds;
}

//------------------------------------------------------------------------------------------------
// Returns load source of queued buffers in sound source
//------------------------------------------------------------------------------------------------
void adas_Get_Load_Source(int Source, int *p_Load, int Size_of_Load)
{
  int i;
  ADAS_SOUND_SOURCE *p_ss;

  p_ss = &SoundSource[Source];

  for (i = 0; i < Size_of_Load; i++)
    if (i < MAXQUEUED) {
      p_Load[i] = p_ss->Buffer_Source[i];
    }
    else
      break;

}

//------------------------------------------------------------------------------------------------
// returns version of Anakreon Digital Audio System
//------------------------------------------------------------------------------------------------
void adas_Get_Version(int *piHi, int *piLow)
{
  *piHi = ADAS_VERSION_HI;
  *piLow = ADAS_VERSION_LOW;
}

//------------------------------------------------------------------------------------------------
// get current device context
//------------------------------------------------------------------------------------------------
ALCcontext *adas_Get_Context(void)
{
  return p_Context;
}

//------------------------------------------------------------------------------------------------
// get current device
//------------------------------------------------------------------------------------------------
ALCdevice *adas_Get_Device(void)
{
  return p_Device;
}

int
adasLoadWAVMemory(ALbyte * buffer, ALsizei buffer_length, ALenum * format,
                  void **data, ALsizei * size, ALuint * frequency,
                  ALboolean * loop)
{
  ALfloat freq;
  *data = alutLoadMemoryFromFileImage(buffer, buffer_length, format, size, &freq);
  if (!(*data)) {
    fprintf(stderr, "ADAS: adasLoadWAVMemory(): %s\n",
            alutGetErrorString(alutGetError()));
    assert(0);
    return (FALSE);
  }
  *frequency = (ALuint)freq;
  *loop = AL_FALSE;
  return (*data);
}
