#ifndef __AC3_STREAM_
#define __AC3_STREAM_

//------------------------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif
  // init
  int adas_AC3_Init(void);

  // Release
  void adas_AC3_Release(void);

  // enum AC3 devices
  int adas_AC3_Enum_Devices(void);

  // get AC3 device caps
  void adas_AC3_Get_Device_Caps(int Device, EAXAC3DEVICEINFO * p_Device_Info);

  // open AC3 device
  int adas_AC3_Open_Device(int Device);

  // Close AC3 device
  int adas_AC3_Close_Device(int Device);

  // Gets file info
  int adas_AC3_Query_File(char *p_File_Name, AC3FILEINFO * p_File_Info);

  // open AC3 stream
  int adas_AC3_Open_Stream(int Device, char *p_File_Name, int AC3_Source,
    AC3STREAM * p_AC3_Stream, LPAC3CALLBACK AC3_Call_Back);

  // close AC3 stream
  int adas_AC3_Close_Stream(AC3STREAM AC3_Stream);

  // play AC3 stream
  int adas_AC3_Play_Stream(AC3STREAM AC3_Stream, BOOL Loop);

  // setup AC3 stream:
  // Fuction will perfort exactly same operations as AC3_Play_Stream method  - exept 
  // for actually starting to play data.
  int adas_AC3_Setup_Stream(AC3STREAM AC3_Stream);

  // stop AC3 stream
  int adas_AC3_Stop_Stream(AC3STREAM AC3_Stream);

  // pause AC3 stream
  int adas_AC3_Pause_Stream(AC3STREAM AC3_Stream);

  // resume AC3 stream
  int adas_AC3_Resume_Stream(AC3STREAM AC3_Stream);

  // set AC3 file position
  int adas_AC3_Set_File_Position(AC3STREAM AC3_Stream, int Pos_Format,
    int iValue);

  // get AC3 file position
  int adas_AC3_Get_File_Position(AC3STREAM AC3_Stream, int Pos_Format,
    int *iValue);

  // set AC3 playback mode
  int adas_AC3_Playback_Mode(int Device, unsigned int Mode);

#ifdef __cplusplus
}
#endif

#endif
