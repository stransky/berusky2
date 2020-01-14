//------------------------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------------------------
#include <AL/al.h>
#include "ogg_stream.h"
#include <vorbis/vorbisfile.h>
#include "compat_mini.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>

//------------------------------------------------------------------------------------------------
// libraries
//------------------------------------------------------------------------------------------------
/*
#ifdef _DEBUG
#pragma comment(lib,"lib\\ogg_d.lib")
#pragma comment(lib,"lib\\vorbis_d.lib")
#pragma comment(lib,"lib\\vorbisfile_d.lib")
#endif

#ifndef _DEBUG
#pragma comment(lib,"lib\\ogg.lib")
#pragma comment(lib,"lib\\vorbis.lib")
#pragma comment(lib,"lib\\vorbisfile.lib")
#endif
*/

//------------------------------------------------------------------------------------------------
// types definitions
//------------------------------------------------------------------------------------------------
typedef struct
{	
	OggVorbis_File			vf;					// pointer to ogg vorbis file structure
	vorbis_info				*vi;				// pointer to ogg vorbis file info structure
	FILE					*file;				// pointer to ogg file
	char					Name[256];			// name of file
	int						Current_Bitstream;	// currnt bit stream
	char                    Data[131072];		// decompresed data
	char					bEof;				// End Of File
	ALsizei					Size;				// size of buffer (Data[])
	ALuint					Frequece;			// frequence
	ALenum					Format;				// format (16, 8, stereo, mono)
	ALuint					Buffer[3];			// pointers of buffer
	ALuint					Source;				// sound source
	float					Gain;				// gain (volume)
	float					Pitch;				// pitch
	float					Position[3];		// position of source
	float					Velocity[3];		// velocity
	THREAD_ID     ThreadID;			// Decompression/Play Thread ID
	THREAD_HANDLE Thread;			  // pointer to D/P Thread
	int						Shot_down;			// shot down thred signal
	char					bSetup;				// Stream was Set
	void					*Setup_Flag;		// Flag to set, when setuping is done
	THREAD_HANDLE  Setup_Thread;		// pointer to Setup Thread
	THREAD_ID      Setup_ThreadID;		// Setup Thread ID
}	ADAS_OGG_STRUCTURE;

//------------------------------------------------------------------------------------------------
// global declarations
//------------------------------------------------------------------------------------------------
static	ADAS_OGG_STRUCTURE	ogg;					// OGG structure
extern char					bDevice;
//static FILE	*d_file;

//------------------------------------------------------------------------------------------------
// function declarations
//------------------------------------------------------------------------------------------------
void * adas_OGG_Proc( void *lpParameter );
void * adas_OGG_Setup_Thread( void *lpParameter );
long adas_OGG_Read_Stream(ADAS_OGG_STRUCTURE *p_ogg);
void adas_Set_Last_Error(char *p_Text);
void adas_Set_Last_Warning(char *p_Text);

void adas_OGG_Init(void)
{
	memset(&ogg, 0, sizeof(ADAS_OGG_STRUCTURE));
	ogg.bEof = 2;
}

//----------------------------------------------------------------------------------------------
// Open OGG stream
//----------------------------------------------------------------------------------------------
int adas_OGG_Open_Stream(char *p_File_Name)
{
	int i,result;

	if(!bDevice) return 0;

	strcpy(ogg.Name, p_File_Name);

	ogg.file = fopen(ogg.Name,"rb");

	if (!ogg.file)
	{
		adas_Set_Last_Error("File not found");
		return 0;
	}

	result = ov_open(ogg.file,&ogg.vf,NULL,0);
	if (result < 0)
	{
		fclose(ogg.file);
		switch(result)
		{
		case OV_EREAD:
			adas_Set_Last_Error("A read from media returned an error.");
			break;
		case OV_ENOTVORBIS:
			adas_Set_Last_Error("Bitstream is not Vorbis data.");
			break;
		case OV_EVERSION:
			adas_Set_Last_Error("Vorbis version mismatch.");
			break;
		case OV_EBADHEADER:
			adas_Set_Last_Error("Invalid Vorbis bitstream header.");
			break;
		case OV_EFAULT:
			adas_Set_Last_Error("Internal logic fault; indicates a bug or heap/stack corruption.");
			break;
		}
		return 0;
	}

	ogg.vi = ov_info(&ogg.vf,-1);
	ogg.bEof = 0;

	alGetError();
	alGenBuffers(3,ogg.Buffer);
	if(alGetError() != AL_NO_ERROR) 
		{
			adas_Set_Last_Error("Unable to create buffer");
			ov_clear(&ogg.vf);
			return 0;
		}

	for(i=0;i<3;i++)
		if (!alIsBuffer(ogg.Buffer[i]))
		{
			adas_Set_Last_Error("Unable to create buffer");
			ov_clear(&ogg.vf);
			return 0;
		}

	alGetError();
	alGenSources(1,&ogg.Source);
	if(alGetError() != AL_NO_ERROR) 
		{
			adas_Set_Last_Error("Unable to create source");
			ov_clear(&ogg.vf);
			alDeleteBuffers(2,ogg.Buffer);
			return 0;
		}
		if (!alIsSource(ogg.Source))
		{
			adas_Set_Last_Error("Unable to create source");
			ov_clear(&ogg.vf);
			alDeleteBuffers(2,ogg.Buffer);
			return 0;
		}

	ogg.bSetup = 0;
	ogg.Thread = 0;
	ogg.Setup_Thread = 0;
	ogg.Setup_Flag = NULL;

	return 1;
}

//----------------------------------------------------------------------------------------------
//  Close OGG stream
//----------------------------------------------------------------------------------------------
void adas_OGG_Close_Stream(void)
{
	if(!bDevice) return;

	alDeleteBuffers(3,ogg.Buffer);
	alDeleteSources(1,&ogg.Source);
	ov_clear(&ogg.vf);
}

//----------------------------------------------------------------------------------------------
//  Read OGG stream
//  adas_OGG_Read_Stream:	critical read
//  adas_OGG_Read_StreamA:	read optimized for segment play time
//  adas_OGG_Read_StreamB:	read and analize stream (NOT FOR PLAYING!!!)
//----------------------------------------------------------------------------------------------
long adas_OGG_Read_Stream(ADAS_OGG_STRUCTURE *p_ogg)
{
	long lReturn;
	long lTotal = 0;

	if(!bDevice) return 0;

	while (lTotal < sizeof(p_ogg->Data))
	{
		lReturn = ov_read(&p_ogg->vf, &p_ogg->Data[lTotal], sizeof(p_ogg->Data)-lTotal,
						  0, 2, 1, &p_ogg->Current_Bitstream);
		
		switch(lReturn)
		{
			case 0:
				memset((void *)&p_ogg->Data[lTotal], 0, sizeof(p_ogg->Data)-lTotal);
				p_ogg->bEof++;
				return lTotal;
				break;
			case OV_HOLE:
				adas_Set_Last_Warning("There was an interruption in the data. (one of: garbage \
									   between\npages, loss of sync followed by recapture, or a \
									   corrupt page).");
				break;
			case OV_EBADLINK:
				adas_Set_Last_Warning("an invalid stream section was supplied to libvorbisfile, \
									   or the requested link is corrupt.");
				break;
			default:
				lTotal += lReturn;
				break;
		}

		if(ogg.Shot_down)
			return lTotal;
	}

return lTotal;
}	

long adas_OGG_Read_StreamA(ADAS_OGG_STRUCTURE *p_ogg, int Critical_Time)
{
	long		  lReturn;
	long		  lTotal = 0;
	long		  lWait, lLostTime = 0;
	int  		  dwStart, dwFinish, dwEplased;

	if(!bDevice) return 0;

	while (lTotal < sizeof(p_ogg->Data))
	{
		dwStart = timeGetTime();

		lReturn = ov_read(&p_ogg->vf, &p_ogg->Data[lTotal], sizeof(p_ogg->Data)-lTotal,
						  0, 2, 1, &p_ogg->Current_Bitstream);
		
		switch(lReturn)
		{
			case 0:
				memset((void *)&p_ogg->Data[lTotal], 0, sizeof(p_ogg->Data)-lTotal);
				p_ogg->bEof++;
				return lTotal;
				break;
			case OV_HOLE:
				adas_Set_Last_Warning("There was an interruption in the data. (one of: garbage \
									   between\npages, loss of sync followed by recapture, or a \
									   corrupt page).");
				break;
			case OV_EBADLINK:
				adas_Set_Last_Warning("an invalid stream section was supplied to libvorbisfile, \
									   or the requested link is corrupt.");
				break;
			default:
				lTotal += lReturn;
				break;
		}

		if(ogg.Shot_down)
			return lTotal;

		dwFinish = timeGetTime();
		dwEplased = dwFinish - dwStart;
		lWait = Critical_Time - dwEplased + lLostTime;

		if(lWait > 0)
		{
			Sleep(lWait);
			lLostTime = 0;
		}
		else
			lLostTime = lWait;
	}

	return lTotal;
}	

long adas_OGG_Read_StreamB(ADAS_OGG_STRUCTURE *p_ogg, FILE	*p_size_file, FILE	*p_cycles_file)
{
	long lReturn;
	long lTotal = 0;
	int  iCounter = 0;

	while (lTotal < sizeof(p_ogg->Data))
	{
		lReturn = ov_read(&p_ogg->vf, &p_ogg->Data[lTotal], sizeof(p_ogg->Data)-lTotal,
						  0, 2, 1, &p_ogg->Current_Bitstream);
		iCounter++;

		if(p_size_file) {
			fprintf(p_size_file,"%ld\n",lReturn);
    }

		switch(lReturn)
		{
			case 0:
				memset((void *)&p_ogg->Data[lTotal], 0, sizeof(p_ogg->Data)-lTotal);
				p_ogg->bEof++;
				return lTotal;
				break;
			case OV_HOLE:
				adas_Set_Last_Warning("There was an interruption in the data. (one of: garbage \
									   between\npages, loss of sync followed by recapture, or a \
									   corrupt page).");
				break;
			case OV_EBADLINK:
				adas_Set_Last_Warning("an invalid stream section was supplied to libvorbisfile, \
									   or the requested link is corrupt.");
				break;
			default:
				lTotal += lReturn;
				break;
		}
	}

	if(p_cycles_file)
		fprintf(p_cycles_file,"%d\n",iCounter);

	return lTotal;
}	

//------------------------------------------------------------------------------------------------
// set position of OGG stream
//------------------------------------------------------------------------------------------------
void adas_OGG_Set_Position(float *v)
{
	if(!bDevice) return;

	alSourcefv(ogg.Source,AL_POSITION,v);
	memcpy(ogg.Position,v,3*sizeof(float));
}

//------------------------------------------------------------------------------------------------
// set Source Relative
//------------------------------------------------------------------------------------------------
void adas_OGG_Set_Source_Relative(char bValue)
{
	if(!bDevice) return;
	
	alSourcei(ogg.Source,AL_SOURCE_RELATIVE, bValue);
}

//------------------------------------------------------------------------------------------------
// set velocity of OGG stream
//------------------------------------------------------------------------------------------------
void adas_OGG_Set_Velocity(float *v)
{
	if(!bDevice) return;

	alSourcefv(ogg.Source,AL_VELOCITY,v);
	memcpy(ogg.Velocity,v,3*sizeof(float));
}

//------------------------------------------------------------------------------------------------
// set gain of OGG stream
//------------------------------------------------------------------------------------------------
void adas_OGG_Set_Gain(float v)
{
	if(!bDevice) return;

	alSourcef(ogg.Source,AL_GAIN,v);
	ogg.Gain = v;
}

//------------------------------------------------------------------------------------------------
// set pitch of OGG stream
//------------------------------------------------------------------------------------------------
void adas_OGG_Set_Pitch(float v)
{
	if(!bDevice) return;

	alSourcef(ogg.Source,AL_PITCH,v);
	ogg.Pitch = v;
}

//------------------------------------------------------------------------------------------------
// play OGG stream
//------------------------------------------------------------------------------------------------
int adas_OGG_Play_Stream(void)
{
	if(!bDevice) return 0;

	if (ogg.Thread)
		return 0;

	alSourcefv(ogg.Source,AL_POSITION,ogg.Position);
	alSourcefv(ogg.Source,AL_VELOCITY,ogg.Velocity);
	alSourcef(ogg.Source,AL_GAIN,ogg.Gain);
	alSourcef(ogg.Source,AL_PITCH,ogg.Pitch);
	alSourcei(ogg.Source,AL_LOOPING,0);

	ogg.Format = AL_FORMAT_STEREO16;
	ogg.Size = sizeof(ogg.Data);
	ogg.Frequece = ogg.vi->rate;
	ogg.Shot_down = 0;

	ogg.Thread = CreateThread( NULL, 0, adas_OGG_Proc, (void *)&ogg, 0, &(ogg.ThreadID));
	if (ogg.Thread)
		return 1;

return 0;
}

//----------------------------------------------------------------------------------------------
// Set Ogg decompresion Thread priority
//----------------------------------------------------------------------------------------------
void adas_OGG_Set_Priority(int Priority)
{
	if (!ogg.Thread) return;

	if(Priority > 3)
		Priority = 3;

	switch ( Priority )
	{
	case 3:
		SetThreadPriority(ogg.Thread,THREAD_PRIORITY_TIME_CRITICAL);
		break;
	case 2:
		SetThreadPriority(ogg.Thread,THREAD_PRIORITY_HIGHEST);
		break;
	case 1:
		SetThreadPriority(ogg.Thread,THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	case 0:
		SetThreadPriority(ogg.Thread,THREAD_PRIORITY_NORMAL);
		break;
	case -1:
		SetThreadPriority(ogg.Thread,THREAD_PRIORITY_BELOW_NORMAL);
		break;
	case -2:
		SetThreadPriority(ogg.Thread,THREAD_PRIORITY_LOWEST);
		break;
	case -3:
		SetThreadPriority(ogg.Thread,THREAD_PRIORITY_IDLE);
		break;
	}
}

//----------------------------------------------------------------------------------------------
// Get Ogg decompresion Thread priority
//----------------------------------------------------------------------------------------------
int adas_OGG_Get_Priority(void)
{
	return GetThreadPriority(ogg.Thread);
}


//----------------------------------------------------------------------------------------------
// Set OGG Setup Stream
//----------------------------------------------------------------------------------------------
int adas_OGG_Setup_Stream(void *p_Flag)
{
	if(!bDevice) return 0;

	if ((ogg.Thread) || (ogg.Setup_Thread))
		return 0;

	alSourcefv(ogg.Source,AL_POSITION,ogg.Position);
	alSourcefv(ogg.Source,AL_VELOCITY,ogg.Velocity);
	alSourcef(ogg.Source,AL_GAIN,ogg.Gain);
	alSourcef(ogg.Source,AL_PITCH,ogg.Pitch);
	alSourcei(ogg.Source,AL_LOOPING,0);

	ogg.Format = AL_FORMAT_STEREO16;
	ogg.Size = sizeof(ogg.Data);
	ogg.Frequece = ogg.vi->rate;
	ogg.Shot_down = 0;

	ogg.Setup_Flag = 0;

	if (p_Flag)
		ogg.Setup_Flag = p_Flag;

	ogg.Setup_Thread = CreateThread( NULL, 0, adas_OGG_Setup_Thread, (void *)&ogg, 0, &(ogg.Setup_ThreadID));
	if (ogg.Setup_Thread)
	{
		//ogg.bSetup = 1;
		return 1;
	}

return 0;
}

//----------------------------------------------------------------------------------------------
// OGG setup thread
//----------------------------------------------------------------------------------------------
void * adas_OGG_Setup_Thread( void *lpParameter )
{
	ADAS_OGG_STRUCTURE *p_ogg = (ADAS_OGG_STRUCTURE *) lpParameter;
	int i;

	if(!p_ogg->bSetup)
		for(i=0;i<3;i++)
		{
			adas_OGG_Read_Stream(p_ogg);
			alBufferData(p_ogg->Buffer[i], p_ogg->Format, &p_ogg->Data, p_ogg->Size, p_ogg->Frequece);
			if(p_ogg->Shot_down)
			{
				p_ogg->bEof = 2;
				ExitThread(0);
			}
		}

	*(int *)p_ogg->Setup_Flag = 1;
	p_ogg->bSetup = 1;
	return 0;
}


//----------------------------------------------------------------------------------------------
// ogg decompression and playing proc
//----------------------------------------------------------------------------------------------
void * adas_OGG_Proc( void *lpParameter )
{
	ADAS_OGG_STRUCTURE *p_ogg = (ADAS_OGG_STRUCTURE *) lpParameter;
	char buffer = 0;
	int processed = 0;
	int state,i;
	FILE	*file;//, *d_file;
	char	text[256];
	int		cycles, time;
	int   dwStart, dwFinish, dwEplased;
	long  losttime = 0;
	
	strcpy(text, ogg.Name);
	text[(strlen(ogg.Name)-3)] = '\0'; //TODO - newline?
	strcat(text,"cyc");

	file = fopen(text,"r");
	if(!file)
	{
		adas_Set_Last_Error("file not found.");
		p_ogg->bEof = 2;
		ExitThread(0);
	}

	if(!p_ogg->bSetup) {
		for(i=0;i<3;i++)
		{
			if (fgets(text,256, file) == NULL) {
				adas_Set_Last_Error("Cannot read file");
				p_ogg->bEof = 2;
				fclose(file);
				ExitThread(0);
			}
			cycles = atoi(text);
			time = (int)floor(750.0f / (float)cycles);
			adas_OGG_Read_StreamA(p_ogg, time);
			alBufferData(p_ogg->Buffer[i], p_ogg->Format, &p_ogg->Data, p_ogg->Size, p_ogg->Frequece);
			if(p_ogg->Shot_down)
			{
				p_ogg->bEof = 2;
				fclose(file);
				ExitThread(0);
			}
		}
	}
	else {
		for(i=0;i<3;i++) {
			if (fgets(text,256, file) == NULL) {
				adas_Set_Last_Error("Cannot read file");
				p_ogg->bEof = 2;
				fclose(file);
				ExitThread(0);
			}
		}
	}

	alSourceQueueBuffers(p_ogg->Source,3,p_ogg->Buffer);
	alSourcePlay(p_ogg->Source);
	p_ogg->bEof = 0;

    while(!p_ogg->Shot_down) 
    {
		alGetSourcei(p_ogg->Source, AL_BUFFERS_PROCESSED, &processed);
		if(processed > 0) 
		{
			if (!p_ogg->bEof)
			{
				alSourceUnqueueBuffers(p_ogg->Source,1,&p_ogg->Buffer[(int)buffer]);
				
				if (fgets(text,256, file) == NULL) {
					adas_Set_Last_Error("Cannot read file");
					p_ogg->bEof = 2;
					fclose(file);
					ExitThread(0);
				}
				cycles = atoi(text);

				dwStart = timeGetTime();
				if(losttime > 100)
				{
//					fprintf(d_file,"critical decopression - lost time = %d\n",losttime);
					adas_OGG_Read_Stream(p_ogg);	
				}
				else
				{
					time = (int)floor(750.0f / (float)(cycles+6));
//					fprintf(d_file,"normal decopression - lost time = %d\n",losttime);
					adas_OGG_Read_StreamA(p_ogg, time);
				}

				alBufferData(p_ogg->Buffer[(int)buffer],
					     p_ogg->Format, &p_ogg->Data, p_ogg->Size, p_ogg->Frequece);
				alSourceQueueBuffers(p_ogg->Source,1,&p_ogg->Buffer[(int)buffer]);
				
				dwFinish = timeGetTime();
				dwEplased = dwFinish - dwStart;
				losttime += (dwEplased - 750);
				
				if(losttime < 0)
					losttime = 0;
				
			}
			else
			{
				alGetSourcei(p_ogg->Source, AL_SOURCE_STATE, &state);
				if (state == AL_STOPPED)
				{
					alSourceStop(p_ogg->Source);
					alSourceUnqueueBuffers(p_ogg->Source,3,p_ogg->Buffer);
					p_ogg->bEof++;
					fclose(file);
//					fclose(d_file);
					ExitThread(0);
				}
			}
			
			buffer ++;
			if (buffer > 2) buffer = 0;
			
			processed = 0;
		}
		
		Sleep(10);
	}

	alSourceStop(p_ogg->Source);
	alSourceUnqueueBuffers(p_ogg->Source,3,p_ogg->Buffer);
	ogg.bEof = 2;

	fclose(file);
//	fclose(d_file);
	return 0;
}

//------------------------------------------------------------------------------------------------
// stop OGG stream
//------------------------------------------------------------------------------------------------
void adas_OGG_Stop_Stream(void)
{
	dword Exit_Code = STILL_ACTIVE;

	if(!bDevice) return;
	if(!ogg.Thread) return;

	ogg.Shot_down = 1;

	while(Exit_Code == STILL_ACTIVE)
		GetExitCodeThread(ogg.Thread, &Exit_Code);                 

	CloseHandle(ogg.Thread);
	ogg.Thread = 0;
	ogg.ThreadID = 0;
}

//------------------------------------------------------------------------------------------------
// seek OGG stream
//------------------------------------------------------------------------------------------------
void adas_OGG_Seek_Stream(double dValue)
{
	if(!bDevice) return;

	ov_time_seek(&ogg.vf, dValue);
}

//------------------------------------------------------------------------------------------------
// check if is OGG stream playing
//------------------------------------------------------------------------------------------------
int adas_Is_OGG_Stream_Playing(void)
{
	if(!bDevice) return 0;

	if ((ogg.bEof >= 0) && (ogg.bEof < 2)) return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------------------------
// Analyze OGG stream
//------------------------------------------------------------------------------------------------
int	adas_OGG_Analyze_Stream(char *p_ogg_file, char *p_size_file, char *p_cycles_file)
{
	int result;
	FILE *pf_size_file, *pf_cycles_file;

	// open stream
	ogg.file = fopen(p_ogg_file,"rb");

	if (!ogg.file)
	{
		adas_Set_Last_Error("File not found");
		return 0;
	}

	pf_size_file = fopen(p_size_file,"w");
	if(!pf_size_file)
	{
		adas_Set_Last_Error("Unable to create file");
		fclose(ogg.file);
		return 0;
	}

	pf_cycles_file = fopen(p_cycles_file,"w");
	if(!pf_cycles_file)
	{
		adas_Set_Last_Error("Unable to create file");
		fclose(ogg.file);
		fclose(pf_size_file);
		return 0;
	}

	result = ov_open(ogg.file,&ogg.vf,NULL,0);
	if (result < 0)
	{
		fclose(ogg.file);
		switch(result)
		{
		case OV_EREAD:
			adas_Set_Last_Error("A read from media returned an error.");
			break;
		case OV_ENOTVORBIS:
			adas_Set_Last_Error("Bitstream is not Vorbis data.");
			break;
		case OV_EVERSION:
			adas_Set_Last_Error("Vorbis version mismatch.");
			break;
		case OV_EBADHEADER:
			adas_Set_Last_Error("Invalid Vorbis bitstream header.");
			break;
		case OV_EFAULT:
			adas_Set_Last_Error("Internal logic fault; indicates a bug or heap/stack corruption.");
			break;
		}
		return 0;
	}

	ogg.vi = ov_info(&ogg.vf,-1);
	ogg.bEof = 0;

	// analyze stream
	while(!ogg.bEof)
		adas_OGG_Read_StreamB(&ogg, pf_size_file, pf_cycles_file);

	fprintf(pf_cycles_file,"_EOF_\n");

	// close stream
	fclose(ogg.file);
	fclose(pf_size_file);
	fclose(pf_cycles_file);

	return 1;
}
