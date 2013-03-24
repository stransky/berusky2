#ifndef __3D_GRAPHIC_
#define __3D_GRAPHIC_

typedef struct
{
  int x;
  int tx;
  int y;
  int ty;
  EDIT_TEXT text;
} HINT_TEXTURE;

typedef struct
{
  EDIT_TEXT_KONFIG konf;
  char bVLoaded;
  char bSLoaded;
  int x;
  int y;
  int tx;
  int ty;
  char *data;
} _3D_TEXTURE;

typedef struct
{
  int count;
  int last;
  char bm_dir[MAX_PATH];
  EDIT_TEXT *p_texture;
  _3D_TEXTURE *p_sysramtexture;
  int bMenuVRAMLoad;
} _3D_DATA;

typedef struct
{
  int idx;
  int idy;
  int iaddx;
  int iaddy;
} _3D_CURSOR;

void _3d_Begin_Draw(void);
void _3d_End_Draw(void);
void _3d_Set_Mask(void);
void _3d_Set_Smooth(void);

        // Draws box in 3d device
        // int i, float {left, top, right, bottom}
void _3d_Draw_Box(int i, float *vfPoint);

        // init 3d
int _3d_Init(void);

        // release 3d
void _3d_Release(void);

        // release texture
void _3d_Release_Texture(int Index);

        // load texture
int _3d_Load_Texture(char *p_File_Name, int Index, char bVideoRAM,
  char bSeek);

        // load list of textures
int _3d_Load_List(char *p_File_Name);

void _3d_Load_From_Sys_To_Video(int iStart);

void _3d_Release_From_Video(int iStart);

void _3d_Gen_Hints(HINT_TEXTURE * bTexture, int tsize);
void _3d_Release_Hints(HINT_TEXTURE * bTexture, int tsize);

#endif
