#ifndef __AMBIENT_SOUNDS_
#define __AMBIENT_SOUNDS_
	
#include "audio_plug-in.h"

	// starts environment
	void as_Start(int Env_ID,AUDIO_DATA *p_ad, void *p_Level);

	// starts environment
	void as_Do_Ambient_Sounds(int Env_ID,AUDIO_DATA *p_ad, void *p_Level);

#endif