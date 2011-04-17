#ifndef __COMICS_
#define __COMICS_

	void cmcs_Start_Comics(char *cFile, AUDIO_DATA *p_ad, char bMusic);

	void cmcs_Play_Intro(char *cFile, AUDIO_DATA *p_ad);

	void cmcs_Play_Video(char *pFile, long dwVideoTime,  AUDIO_DATA *p_ad);

	void cmcs_Start_Picture(int Index, long time, AUDIO_DATA *p_ad, char bMusic);

	void cmcs_Game_Down();
	void cmcs_Game_Up();

#endif
