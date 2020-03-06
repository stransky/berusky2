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
    /* This allows 18 chars for abbreviated month name. Should be more
       than enough in any locale. */
    char timestamp[40];
    time_t date = time(NULL);
    struct tm *unpacked_date = localtime(&date);
    size_t ret = strftime(timestamp, sizeof(timestamp),
			  "[%b_%d_%Y]_[%H_%M_%S]", unpacked_date);
    assert(ret);
    *demo = 0;
    sprintf(pDemo, "[%s]_%s.dem", pLevel, timestamp);

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
  char odir[MAX_FILENAME + 1];
  char dir[MAX_FILENAME + 1];
  int error;
  apuInt ulsize;

  char Text[1024];
  char Caption[1024];
  char *buffer;
  FILE *file;

  APAK_HANDLE *hArchive = NULL;

  if (!ctagtitle || !ctagtext)
    return;

  if (getcwd(odir, MAX_FILENAME) == NULL)
    return;

  strcpy(dir, BITMAP_DIR);
  if (chdir(dir))
    return;
  hArchive = apakopen(cFontFile[2], dir, &error);

  if (!hArchive) {
    /* GCC warns when we don't check the return value of chdir(). For
       some reason, casting to (void) doesn't work. */
    if (chdir(odir))
      return;
    return;
  }
  else
    hArchive->pActualNode = hArchive->pRootNode->pNextNode;

  file = aopen(hArchive, "messages.txt", "rb");

  if (!file) {
    apakclose(&hArchive);
    if (chdir(odir))
      return;
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
  apakclose(&hArchive);
  /* GCC warns when we don't check the return value of chdir(). For
     some reason, casting to (void) doesn't work. */
  if (chdir(odir))
    return;
}

static int translation_table[KEYNUM];

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
      
    translation_table[K_BKSP] = 14; // BACKSPACE
    translation_table[K_TAB] = 15; // TAB
  
    translation_table[K_Q] = 16;
    translation_table[K_W] = 17;
    translation_table[K_E] = 18;
    translation_table[K_R] = 19;
    translation_table[K_T] = 20;
    translation_table[K_Y] = 21;
    translation_table[K_U] = 22;
    translation_table[K_I] = 23;
    translation_table[K_O] = 24;
    translation_table[K_P] = 25;
    
    translation_table[K_BRACKET_L] = 26; // [
    translation_table[K_BRACKET_R] = 27; // ]
  
    translation_table[K_ENTER] = 28;
    translation_table[K_CTRL] = 29;
    translation_table[SDLK_LCTRL] = 29;
    
    translation_table[K_A] = 30;
    translation_table[K_S] = 31;
    translation_table[K_D] = 32;
    translation_table[K_F] = 33;
    translation_table[K_G] = 34;
    translation_table[K_H] = 35;
    translation_table[K_J] = 36;
    translation_table[K_K] = 37;
    translation_table[K_L] = 38;
    
    translation_table[K_SEMICOL] = 39;
    translation_table[K_QUOTE] = 40;
    //translation_table[K_TILDA] = 41;
    translation_table[SDLK_LSHIFT] = 42;
    translation_table[SDLK_BACKSLASH] = 43;
    
    translation_table[K_Z] = 44;
    translation_table[K_X] = 45;
    translation_table[K_C] = 46;
    translation_table[K_V] = 47;
    translation_table[K_B] = 48;
    translation_table[K_N] = 49;
    translation_table[K_M] = 50;
    
    translation_table[SDLK_COMMA] = 51;
    translation_table[SDLK_PERIOD] = 52;
    translation_table[SDLK_SLASH] = 53;
    translation_table[SDLK_RSHIFT] = 54;    
    translation_table[SDLK_PRINT] = 55;
    translation_table[SDLK_LALT] = 56;    
    translation_table[SDLK_SPACE] = 57;
    translation_table[SDLK_CAPSLOCK] = 58;
    
    translation_table[K_F1] = 59;
    translation_table[K_F2] = 60;
    translation_table[K_F3] = 61;
    translation_table[K_F4] = 62;
    translation_table[K_F5] = 63;
    translation_table[K_F6] = 64;
    translation_table[K_F7] = 65;
    translation_table[K_F8] = 66;
    translation_table[K_F9] = 67;
    translation_table[K_F10] = 68;
    translation_table[K_F11] = 87;
    translation_table[K_F12] = 88;
    
    translation_table[SDLK_NUMLOCK] = 69;
    translation_table[SDLK_SCROLLOCK] = 70;
    translation_table[SDLK_HOME] = 71;
    translation_table[SDLK_UP] = 72;
    translation_table[SDLK_PAGEUP] = 73;
    translation_table[SDLK_KP_MINUS] = 74;
    translation_table[SDLK_LEFT] = 75;
    translation_table[SDLK_KP5] = 76;
    translation_table[SDLK_RIGHT] = 77;
    translation_table[SDLK_KP_PLUS] = 78;
    translation_table[SDLK_END] = 79;
    translation_table[SDLK_DOWN] = 80;
    translation_table[SDLK_PAGEDOWN] = 81;
    translation_table[SDLK_INSERT] = 82;
    translation_table[SDLK_DELETE] = 83;
  }
  k = translation_table[k];
#endif

  sprintf(text, "##control_k_%d", k);
  return(text);
}
