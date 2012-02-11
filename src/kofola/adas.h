#ifndef _ADAS_H_
#define _ADAS_H_

//------------------------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------------------------
#include "adas_types.h"
#include "ogg_stream.h"
#include "ogg_io.h"
#include <AL/alut.h>
#include <AL/al.h>
#include <AL/alc.h>

#ifdef __cplusplus
extern "C"
{
#endif

  //Init AnakreoN Digital Sound System:
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
  void adas_Init(ADAS_INIT_DATA * p_adas_data);

  //Release AnakreoN Digital Sound System
  void adas_Exit(void);

  // Set distance model
  int adas_Set_Distance_Model(long Distance_Model);

  // Set distance model
  int adas_Set_Doppler_Factor(float Doppler_Factor);

  // Set distance model
  int adas_Set_Doppler_Velocity(float Doppler_Velocity);

  // load sound data & init memory
  unsigned long adas_Load_First(char *p_Index_File, char *p_File_Name);
  unsigned long adas_Load_FirstMemory(char *p_Index_File, void *p_File,
    char *p_File_Name);

  // load sound data
  unsigned long adas_Load_Next(char *p_File_Name);
  unsigned long adas_Load_NextMemory(void *p_File, char *p_File_Name);

  // releases loaded sound data
  void adas_Release_Loaded_Data(void);

  // Set Listener Gain
  int adas_Set_Listener_Gain(float v);

  // Set Listener Position
  int adas_Set_Listener_Position(float *v);

  // Set Listener Velocity
  int adas_Set_Listener_Velocity(float *v);

  // Set Listener Orientation
  int adas_Set_Listener_Orientation(float *v);

  // Get Listener Gain
  int adas_Get_Listener_Gain(float *v);

  // Get Listener Position
  int adas_Get_Listener_Position(float *v);

  // Get Listener Velocity
  int adas_Get_Listener_Velocity(float *v);

  // Get Listener Orientation
  int adas_Get_Listener_Orientation(float *v);

  // Create Sound Source
  int adas_Create_Source(ADAS_SOUND_SOURCE_DATA * p_ssd, void **p_callback);

  // queue a sound to source
  int adas_Queue_Sound(int Source, int Wave_Index);

  // unqueue a sound to source
  void adas_Unqueue_Sound(int Source, int Sounds);

  // Returns load source of queued buffers in sound source
  void adas_Get_Load_Source(int Source, int *p_Load, int Size_of_Load);

  // Release sound source
  void adas_Release_Source(int Owner, int Type, int Index);

  // Pause sound source
  void adas_Pause_Source(int Owner, int Type, int Index);

  // Resume sound source
  void adas_Resume_Source(int Owner, int Type, int Index);

  // cheks, if is particular owher playing selected sound data
  int adas_Is_Sound_Processed(int Wave_Index, int Owner);

  // sel all sources volume (gain)
  void adas_Set_All_Source_Volume(float fValue);

  // set all listeners parametrs
  //int adas_Set_All_Listener_Properties(LPEAXLISTENERPROPERTIES lpData);

  // set listeners room
  int adas_Set_Listener_Room(long lValue);

  // set listeners roomHF
  int adas_Set_Listener_RoomHF(long lValue);

  // set listeners room rolloff
  int adas_Set_Listener_Room_Rolloff(float fValue);

  // set listeners room decay time
  int adas_Set_Listener_Decay_Time(float fValue);

  // set listeners room decay HFRatio
  int adas_Set_Listener_Decay_HFRatio(float fValue);

  // set listeners reflections
  int adas_Set_Listener_Reflections(long lValue);

  // set listeners reflections delay
  int adas_Set_Listener_Reflections_Delay(float fValue);

  // set listeners reverb
  int adas_Set_Listener_Reverb(long lValue);

  // set listeners reverb delay
  int adas_Set_Listener_Reverb_Delay(float fValue);

  // set listeners environment
  int adas_Set_Listener_Environment(unsigned long dwValue);

  // set listeners environment size
  int adas_Set_Listener_Environment_Size(float fValue);

  // set listeners environment diffusion
  int adas_Set_Listener_Environment_Diffusion(float fValue);

  // set listeners air absoprtion
  int adas_Set_Listener_Air_Absorption(float fValue);

  // set listeners flags
  int adas_Set_Listener_Flags(unsigned long dwValue);

  // set listeners scale delay time
  int adas_Set_Listener_Scale_Decay_Time(int bValue);

  // set listeners scale reflections
  int adas_Set_Listener_Scale_Reflections(int bValue);

  // set listeners scale reflections delay
  int adas_Set_Listener_Scale_Reflections_Delay(int bValue);

  // set listeners scale reverb
  int adas_Set_Listener_Scale_Reverb(int bValue);

  // set listeners scale reverb delay
  int adas_Set_Listener_Scale_Reverb_Delay(int bValue);

  // set listeners clip decayHF
  int adas_Set_Listener_Clip_DecayHF(int bValue);

  // get all listener properties
  //int adas_Get_All_Listener_Properties(LPEAXLISTENERPROPERTIES lpData);

  // get listener decay time
  int adas_Get_Listener_Decay_Time(float *pfValue);

  // get listener Reflections
  int adas_Get_Listener_Reflections(long *plValue);

  // get listener Reflections Delay
  int adas_Get_Listener_Reflections_Delay(float *pfValue);

  // get listener reverb
  int adas_Get_Listener_Reverb(long *plValue);

  // get listener reverb delay
  int adas_Get_Listener_Reverb_Delay(float *pfValue);

  // set listener rolloff
  int adas_Set_Listener_Rolloff(float fValue);

  // set source properties
  //void adas_Set_Source_All_Properties(int Owner, int Type, int Index,
    //LPEAXBUFFERPROPERTIES lpData);

  // Set cube position
  void adas_Set_Bounding_Object_Position(int Owner, int Type, int Index,
    float *v);

  // Set sound sources position
  void adas_Set_Source_Position(int Owner, int Type, int Index, float *v);

  // Set sound sources pitch
  void adas_Set_Source_Pitch(int Owner, int Type, int Index, float v);

  // Set sound sources gain
  void adas_Set_Source_Gain(int Owner, int Type, int Index, float v);

  // Set sound sources min. gain
  void adas_Set_Source_Min_Gain(int Owner, int Type, int Index, float v);

  // Set sound sources max. gain
  void adas_Set_Source_Max_Gain(int Owner, int Type, int Index, float v);

  // Set sound sources velocity
  void adas_Set_Source_Velocity(int Owner, int Type, int Index, float *v);


  // set source EAX properties
  void adas_Set_Source_lEAXproperty(int Owner, int Type, int Index,
    long lValue, ALuint Property);
  void adas_Set_Source_fEAXproperty(int Owner, int Type, int Index,
    float fValue, ALuint Property);
  void adas_Set_Source_dwEAXproperty(int Owner, int Type, int Index,
    unsigned long dwValue, ALuint Property);

  // set source direct
#define adas_Set_Source_Direct(Owner, Type, Index, lValue)			adas_Set_Source_lEAXproperty(Owner, Type, Index, lValue, DSPROPERTY_EAXBUFFER_DIRECT)

  // set source directHF
#define	adas_Set_Source_DirectHF(Owner, Type, Index, lValue)		adas_Set_Source_lEAXproperty(Owner, Type, Index, lValue, DSPROPERTY_EAXBUFFER_DIRECTHF)

  // set source room
#define	adas_Set_Source_Room(Owner, Type, Index, lValue)			adas_Set_Source_lEAXproperty(Owner, Type, Index, lValue, DSPROPERTY_EAXBUFFER_ROOM)

  // set source roomHF
#define adas_Set_Source_RoomHF(Owner, Type, Index, lValue)			adas_Set_Source_lEAXproperty(Owner, Type, Index, lValue, DSPROPERTY_EAXBUFFER_ROOMHF)

  // set source rolloff
#define	adas_Set_Source_Rolloff(Owner, Type, Index, fValue)			adas_Set_Source_fEAXproperty(Owner, Type, Index, fValue, DSPROPERTY_EAXBUFFER_ROOMROLLOFFFACTOR)

  // set source outsideHF volume
#define	adas_Set_Source_Outside(Owner, Type, Index, lValue)			adas_Set_Source_lEAXproperty(Owner, Type, Index, lValue, DSPROPERTY_EAXBUFFER_OUTSIDEVOLUMEHF)

  // set source absorption
#define	adas_Set_Source_Absorption(Owner, Type, Index, fValue)		adas_Set_Source_fEAXproperty(Owner, Type, Index, fValue, DSPROPERTY_EAXBUFFER_AIRABSORPTIONFACTOR)

  // set source flags
#define	adas_Set_Source_Flags(Owner, Type, Index, dwValue)			adas_Set_Source_dwEAXproperty(Owner, Type, Index, dwValue, DSPROPERTY_EAXBUFFER_FLAGS)

  // set source obstruction
#define adas_Set_Source_Obstruction(Owner, Type, Index, lValue)		adas_Set_Source_lEAXproperty(Owner, Type, Index, lValue, DSPROPERTY_EAXBUFFER_OBSTRUCTION)

  // set source obstructionLF
#define adas_Set_Source_ObstructionLF(Owner, Type, Index, fValue)	adas_Set_Source_fEAXproperty(Owner, Type, Index, fValue, DSPROPERTY_EAXBUFFER_OBSTRUCTIONLFRATIO)

  // set source occlusion
#define adas_Set_Source_Occlusion(Owner, Type, Index, lValue)		adas_Set_Source_lEAXproperty(Owner, Type, Index, lValue, DSPROPERTY_EAXBUFFER_OCCLUSION)

  // set source occlusionLF
#define adas_Set_Source_OcclusionLF(Owner, Type, Index, fValue)		adas_Set_Source_fEAXproperty(Owner, Type, Index, fValue, DSPROPERTY_EAXBUFFER_OCCLUSIONLFRATIO)

  // set source occlusion room
#define adas_Set_Source_Occlusion_Room(Owner, Type, Index, fValue)	adas_Set_Source_fEAXproperty(Owner, Type, Index, fValue, DSPROPERTY_EAXBUFFER_OCCLUSIONROOMRATIO)


  // set source OPENAL properties
  void adas_Set_Source_fOPENALproperty(int Owner, int Type, int Index,
    float fValue, ALenum Property);
  void adas_Set_Source_vOPENALproperty(int Owner, int Type, int Index,
    float *vValue, ALenum Property);
  void adas_Set_Source_dwOPENALproperty(int Owner, int Type, int Index,
    unsigned long dwValue, ALenum Property);

  // set source min distance
#define	adas_Set_Source_Min_Distance(Owner, Type, Index, fValue)		adas_Set_Source_fOPENALproperty (Owner, Type, Index, fValue,  AL_MIN_DISTANCE);

  // set source max distance
#define	adas_Set_Source_Max_Distance(Owner, Type, Index, fValue)		adas_Set_Source_fOPENALproperty (Owner, Type, Index, fValue,  AL_MAX_DISTANCE);

  // set source reference distance
#define	adas_Set_Source_Reference_Distance(Owner, Type, Index, fValue)	adas_Set_Source_fOPENALproperty (Owner, Type, Index, fValue,  AL_REFERENCE_DISTANCE);

  // set source orientation
#define	adas_Set_Source_Cone_Orientation(Owner, Type, Index, vValue)	adas_Set_Source_vOPENALproperty (Owner, Type, Index, *vValue,  AL_DIRECTION);

  // set source inside angle
#define adas_Set_Source_Cone_Inside_Angle(Owner, Type, Index, dwValue)	adas_Set_Source_dwOPENALproperty (Owner, Type, Index, dwValue,  AL_CONE_INNER_ANGLE);

  // set source cone outside angle
#define adas_Set_Source_Cone_Outside_Angle(Owner, Type, Index, dwValue) adas_Set_Source_dwOPENALproperty (Owner, Type, Index, dwValue,  AL_CONE_OUTER_ANGLE);

  // set source cone outside volume
  void adas_Set_Source_Cone_Outside_Volume(int Owner, int Type, int Index,
    long lValue);

  // get source position
  int adas_Get_Source_Position(int Index, float *fValue);

  // get sound of sound sources
  int adas_Query_Sources(void);

  // Get error message
  int adas_Get_Last_Error(char *p_Text, int Size);

  // Get warning message
  int adas_Get_Last_Warning(char *p_Text, int Size);

  // reset error message
  void adas_Reset_Last_Error(void);

  //returns version of Anakreon Digital Audio System
  void adas_Get_Version(int *piHi, int *piLow);

  // get current device context
  ALCcontext *adas_Get_Context(void);

  // get current device
  ALCdevice *adas_Get_Device(void);

#ifdef __cplusplus
}
#endif

#endif
