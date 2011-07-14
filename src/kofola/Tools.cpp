//------------------------------------------------------------------------------------------------
// version 0.0.1
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "Apak.h"
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"

#define KEY			5419579899328476981
#define CIPHER0		8321075519164711180
#define CIPHER1		2091360229723956080
#define CIPHER2		649765738391233904
#define CIPHER3		656119274643003910
#define CIPHER4		5419580143046849816
#define FILECIPHER0	2961124561019426902
#define FILECIPHER1	9008328511069505368

extern char cFontFile[5][64];

//------------------------------------------------------------------------------------------------
// Get CPU speed
//------------------------------------------------------------------------------------------------
/*
double _ui64tod(unsigned __int64 ui64Value)
{
	char	string[9], *stopstring;

	sprintf(string,"%d", ui64Value);
	return strtod(string, &stopstring);
}

float tools_Base_Priority(unsigned __int64 CPU_Speed)
{
	float  cpu;
	double result;

	cpu = (float)(_ui64tod(CPU_Speed) / 1000000000.f);

	result = ((((cpu - 0.5f)*(cpu - 0.8f) * (cpu - 1.0f))/(-0.024f)) * 3) +
			 ((((cpu - 0.4f)*(cpu - 0.8f) * (cpu - 1.0f))/ (0.015f)) * 2) +
			 ((((cpu - 0.4f)*(cpu - 0.5f) * (cpu - 1.0f))/(-0.024f)) * 1) +
			 ((((cpu - 0.4f)*(cpu - 0.5f) * (cpu - 0.8f))/ (0.06f))  * 0);

	if (result < 0) result = 0;
	if (result > 3) result = 3;

	return (float)result;
}
*/

void tools_Parse_Command_Line(char *pCommnad, char *pLevel, char *pDemo,
  char *demo)
{
  int i;

  for (i = 0; i < (int) strlen(pCommnad); i++)
    if (pCommnad[i] == ' ')
      break;

  if (i > 0) {
    strncpy(pLevel, pCommnad, i);
    pLevel[i] = '\0';
  }

  if (i < (int) strlen(pCommnad)) {
    if (!strncmp((const char *) &pCommnad[i + 1], "np:", 3))
      *demo = 1;
    else if (!strncmp((const char *) &pCommnad[i + 1], "fp:", 3))
      *demo = 2;
    else
      *demo = 0;

    strcpy(pDemo, (const char *) &pCommnad[i + 4]);
  }
  else {
    *demo = 0;
    sprintf(pDemo, "[%s]_[%s]_[%s].dem", pLevel, __DATE__, __TIME__);

    for (i = 0; i < (int) strlen(pDemo); i++) {
      if (pDemo[i] == 32)
        pDemo[i] = '_';

      if (pDemo[i] == ':')
        pDemo[i] = '_';
    }

    //strcpy(pDemo, "Demo.dem");
  }
}

/*
void _ui64toc(__int64 a, __int64 b, __int64 *r)
{
	*r = a^b;
}


char *_ui64towc(__int64 i64, char *cText)
{
	int i;
	unsigned __int64 ui64ID[5];
	FILE	*file;
	char	buffer[256], buffer1[256];

	chdir((cText));

	ZeroMemory(buffer, 256);
	ZeroMemory(ui64ID, 5 * sizeof(__int64));

	_ui64toc(FILECIPHER0, KEY, &ui64ID[0]);
	memcpy((void *) buffer, (void *)&ui64ID[0], sizeof(__int64));

	_ui64toc(FILECIPHER1, KEY, &ui64ID[0]);
	memcpy((void *) &buffer[8], (void *)&ui64ID[0], sizeof(__int64));

	file = fopen(buffer,"rb");

	if(!file)
		return NULL;

	fseek(file, 153656, SEEK_SET);

	fread(ui64ID, sizeof(__int64), 5, file);

	for(i=0;i<5;i++)
		_ui64toc(ui64ID[i], KEY, &ui64ID[i]);

	memcpy((void *) buffer, (void *)ui64ID, 5 * sizeof(__int64));

	fclose(file);

	_ui64toc(CIPHER0, KEY, &ui64ID[0]);
	memcpy((void *) &buffer1, (void *)&ui64ID[0], sizeof(__int64));
	_ui64toc(CIPHER1, KEY, &ui64ID[0]);
	memcpy((void *) &buffer1[24], (void *)&ui64ID[0], sizeof(__int64));
	_ui64toc(CIPHER2, KEY, &ui64ID[0]);
	memcpy((void *) &buffer1[16], (void *)&ui64ID[0], sizeof(__int64));
	_ui64toc(CIPHER3, KEY, &ui64ID[0]);
	memcpy((void *) &buffer1[8], (void *)&ui64ID[0], sizeof(__int64));
	_ui64toc(CIPHER4, KEY, &ui64ID[0]);
	memcpy((void *) &buffer1[32], (void *)&ui64ID[0], sizeof(__int64));

	if(!strcmp(buffer, buffer1))
		return cText;
	else
		return NULL;
}
*/
void GetText(char *Buffer, char *mask, char *text)
{
  char *wcChar = strstr(Buffer, mask);
  char *wcEnter = NULL;

  if (!wcChar)
    return;

  wcChar += 2 + strlen(mask);

  wcEnter = strstr(wcChar, "\n");

  if (!wcEnter)
    return;

  wcEnter--;

  strncpy(text, wcChar, wcEnter - wcChar);
}

void MyMessageBox(HWND hWnd, char *ctagtitle, char *ctagtext, char *addtext)
{
  char odir[MAX_PATH + 1];
  char dir[MAX_PATH + 1];
  int error;
  unsigned long ulsize;

  char Text[1024];
  char Caption[1024];
  char *buffer;
  FILE *file;

  APAK_HANDLE *hArchive = NULL;

  if (!ctagtitle || !ctagtext)
    return;

  getcwd(odir, MAX_PATH);

  GetPrivateProfileString("game", "bitmap_dir", "c:\\", dir, MAX_PATH,
    ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  apak_dir_correction(dir);
  hArchive = apakopen(cFontFile[2], dir, &error);

  if (!hArchive) {
    chdir((odir));
    return;
  }
  else
    hArchive->pActualNode = hArchive->pRootNode->pNextNode;

  file = aopen(hArchive, "messages.txt", "rb");

  if (!file) {
    apakclose(hArchive);
    chdir((odir));
  }

  agetbuffer(file, (char **) &buffer, &ulsize);

  memset(Text, 0, 1024 * sizeof(char));
  memset(Caption, 0, 1024 * sizeof(char));

  GetText(buffer, ctagtext, Text);
  GetText(buffer, ctagtitle, Caption);

  strcat(Caption, addtext);

  //MessageBox(hWnd, (LPCTSTR) Text, (LPCTSTR) Caption, MB_OK);

  aclose(file);
  apakclose(hArchive);
  chdir((odir));
}
