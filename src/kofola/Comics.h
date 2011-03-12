#ifndef __COMICS_
#define __COMICS_

	void cmcs_Start_Comics(char *cFile, HWND hWnd, AUDIO_DATA *p_ad, char bMusic);

	void cmcs_Play_Intro(char *cFile, HWND hWnd, AUDIO_DATA *p_ad);

	void cmcs_Play_Video(char *pFile, HWND hWnd, long dwVideoTime,  AUDIO_DATA *p_ad);

	void cmcs_Start_Picture(int Index, long time, HWND hWnd, AUDIO_DATA *p_ad, char bMusic);

	void cmcs_Game_Down(HWND hWnd);
	void cmcs_Game_Up(HWND hWnd);

#endif