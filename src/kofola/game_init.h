#ifndef _GAMEINIT_
#define _GAMEINIT_

#include <stdio.h>
#include "audio_plug-in.h"

void gi_Init_Sound_Engine(AUDIO_DATA *p_ad);
void gi_Release_Sound_Engine(void);

//int gi_EnumDisplaySettings(DEVMODE *pdevmode);
void gi_Set_Win_Version(void);

#endif
