#ifndef __OGG_STREAM_
#define __OGG_STREAM_

#ifdef __cplusplus
extern "C" {
#endif

void adas_OGG_Init(void);

// open OGG stream
int adas_OGG_Open_Stream(char *p_File_Name);

// close OGG stream
void adas_OGG_Close_Stream(void);

// Set OGG Setup Stream
int adas_OGG_Setup_Stream(void *p_Flag);

// play OGG stream
int adas_OGG_Play_Stream(void);

// set position of OGG stream
void adas_OGG_Set_Position(float *v);

// set Source Relative
void adas_OGG_Set_Source_Relative(char bValue);

// set velocity of OGG stream
void adas_OGG_Set_Velocity(float *v);

// set gain of OGG stream
void adas_OGG_Set_Gain(float v);

// set pitch of OGG stream
void adas_OGG_Set_Pitch(float v);

// stop OGG stream
void adas_OGG_Stop_Stream(void);

// seek OGG stream
void adas_OGG_Seek_Stream(double dValue);

// check if is OGG stream playing
int adas_Is_OGG_Stream_Playing(void);

// Set Ogg decompresion Thread priority
void adas_OGG_Set_Priority(int Priority);

// Get Ogg decompresion Thread priority
int adas_OGG_Get_Priority(void);

// Analyze OGG stream
int	adas_OGG_Analyze_Stream(char *p_ogg_file, char *p_size_file, char *p_cicles_file);

#ifdef __cplusplus
}
#endif

#endif
