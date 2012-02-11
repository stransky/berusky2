#include "ogg_io.h"

   FILE*           oggFile;
   OggVorbis_File  oggStream;
   vorbis_info*    vorbisInfo;
   vorbis_comment* vorbisComment;

   ALuint buffers[3];
   ALuint source;
   ALenum format;

   unsigned long threadid;
   void			 *thread;
   char			 ogg_stop;
   
   extern char	 bDevice;

char ogg_check();
void ogg_empty();
char ogg_stream(ALuint buffer);

int ogg_proc( void *lpParameter );

void ogg_gain(float gain)
{
	if(!bDevice) return;
	if(!thread) return;

	alSourcef (source, AL_GAIN, gain);
}

int ogg_open(char *file, float gain)
{
    int result;
    
	if(!bDevice) return 0;

    if(!(oggFile = fopen(file, "rb")))
        return 0;

    if((result = ov_open(oggFile, &oggStream, NULL, 0)) < 0)
    {
        fclose(oggFile);
        
        return 0;
    }

    vorbisInfo = ov_info(&oggStream, -1);
    vorbisComment = ov_comment(&oggStream, -1);

    if(vorbisInfo->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
        
        
    alGenBuffers(3, buffers);
    ogg_check();
    alGenSources(1, &source);
    ogg_check();
    
    alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (source, AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (source, AL_SOURCE_RELATIVE, AL_TRUE      );
	alSourcef (source, AL_GAIN,			   gain			);

	return 1;
}

void ogg_release()
{
#ifdef WINDOWS
  unsigned long exit_code = STILL_ACTIVE;
  
  if(!bDevice) return;
  if(!thread) return;
  
  ogg_stop = 1;
  
  while(exit_code == STILL_ACTIVE)
    GetExitCodeThread(thread, &exit_code);
  
  CloseHandle(thread);
  
  thread = 0;
  threadid = 0;
  
  alSourceStop(source);
  ogg_empty();
  alDeleteSources(1, &source);
  ogg_check();
  alDeleteBuffers(3, buffers);
  ogg_check();
  
  ov_clear(&oggStream);
  
  fclose(oggFile);
#endif  
}




char ogg_playback()
{
 	if(!bDevice) return 0;

#ifdef WINDOWS
	if(ogg_playing())
        return 1;
        
    if(!ogg_stream(buffers[0]))
        return 0;
        
    if(!ogg_stream(buffers[1]))
        return 0;
    
    if(!ogg_stream(buffers[2]))
        return 0;

	alSourceQueueBuffers(source, 3, buffers);
	ogg_stop = 0;

	thread = CreateThread( NULL, 0, ogg_proc, NULL, 0, &(threadid));
	
	if(!thread)
		return 0;

	SetThreadPriority(thread,THREAD_PRIORITY_HIGHEST);
	//SetThreadPriority(thread,THREAD_PRIORITY_ABOVE_NORMAL);
#endif
    return 1;
}




char ogg_playing()
{
	ALenum state;
    
 	if(!bDevice) return 0;
	if(!thread) return 0;

    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    return (state == AL_PLAYING);
}




char ogg_update()
{
    int processed;
    char active = 1;

 	if(!bDevice) return 0;
	if(!thread) return 0;

    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    while(processed--)
    {
        ALuint buffer;
        
        alSourceUnqueueBuffers(source, 1, &buffer);
        ogg_check();

        active = ogg_stream(buffer);

        alSourceQueueBuffers(source, 1, &buffer);
        ogg_check();
    }

    return active;
}




char ogg_stream(ALuint buffer)
{
    char pcm[BUFFER_SIZE];
    int  size = 0;
    int  section;
    int  result;

	while(size < BUFFER_SIZE)
    {
        result = ov_read(&oggStream, pcm + size, BUFFER_SIZE - size, 0, 2, 1, &section);
    
        if(result > 0)
            size += result;
        else
            if(result < 0)
                return 0;
            else
                break;
    }
    
    if(size == 0)
        return 0;
        
    alBufferData(buffer, format, pcm, size, vorbisInfo->rate);
    ogg_check();
    
    return 1;
}




void ogg_empty()
{
    int queued;
    
 	if(!bDevice) return;
	if(!thread) return;

    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
    
    while(queued--)
    {
        ALuint buffer;
    
        alSourceUnqueueBuffers(source, 1, &buffer);
        ogg_check();
    }
}

char ogg_check()
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
		return 1;

	return 0;
}

int ogg_proc( void *lpParameter )
{
	alSourcePlay(source);

    while(ogg_update())
    {
	   if(ogg_stop)
	   {
		   alSourceStop(source);
		   ExitThread(0);
		   return 0;
	   }

       if(!ogg_playing())
	   {
		    alSourceStop(source);
			
			if(ogg_update())
				alSourcePlay(source);
			else
				break;
	   }
	
	   Sleep(4);
    }

	//ogg_release();

	ExitThread(0);

	thread = 0;
	threadid = 0;

  alSourceStop(source);
  ogg_empty();
  alDeleteSources(1, &source);
  ogg_check();
  alDeleteBuffers(3, buffers);
  ogg_check();

  ov_clear(&oggStream);

	fclose(oggFile);

	return 0;
}
