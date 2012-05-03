//------------------------------------------------------------------------------------------------
// version 0.0.1
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "Apak.h"
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"

extern char cFontFile[5][64];

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
  apuInt ulsize;

  char Text[1024];
  char Caption[1024];
  char *buffer;
  FILE *file;

  APAK_HANDLE *hArchive = NULL;

  if (!ctagtitle || !ctagtext)
    return;

  getcwd(odir, MAX_PATH);

  GetPrivateProfileString("game", "bitmap_dir", "c:\\", dir, MAX_PATH, ini_file);
  working_file_translate(dir, 256);
  chdir(dir);
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
  kprintf(TRUE, Text);

  aclose(file);
  apakclose(hArchive);
  chdir((odir));
}

int translation_table[255];

char * Key2String(int k, char *text)
{
#ifdef LINUX
  static int translation_table_init = TRUE;
  if(translation_table_init) {
    translation_table_init = FALSE;
    
    translation_table[K_ESC] = 1; // ESC
  
    translation_table[K_1] = 2; // 1
    translation_table[K_2] = 3; // 2
    translation_table[K_3] = 4; // 3
    translation_table[K_4] = 5; // 4
    translation_table[K_5] = 6; // 5
    translation_table[K_6] = 7; // 6
    translation_table[K_7] = 8; // 7
    translation_table[K_8] = 9; // 8
    translation_table[K_9] = 10; // 9
    translation_table[K_0] = 11; // 0
  
    translation_table[K_MINUS] = 12; // -
    translation_table[K_PLUS] = 13; // +
  
    .. and more
  
  }
  k = translation_table[k];
#endif

  sprintf(text, "##control_k_%d", k);
  return(text);
}
