/*
  Header s texturam
*/

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#define DEF_MIP_LEVEL 9
 
/* 
** RGB Image Structure 
*/ 
typedef struct _AUX_RGBImageRec { 
    GLint sizeX, sizeY; 
    unsigned char *data; 
} AUX_RGBImageRec; 


// ABGR poradi slozek
typedef struct _RBITMAP {
    int    x, y;
    dword *data;
    float *data_float;
} RBITMAP;

typedef RBITMAP  bitmapa;

#define  TEXT_GEN_DOT3  0x0001 //textura se prevede na dot3
#define  TEXT_GEN_DUDV  0x0002 //textura se prevede na dudv
#define  TEXT_FLOYD     0x0004 //aplikovat floyda
#define  TEXT_GEN       (TEXT_GEN_DOT3|TEXT_GEN_DUDV)
#define  TEXT2_POUZITA  0x0001
#define  TEXT2_FLARE    0x0002

typedef struct _EDIT_TEXT_OLD {

  char     jmeno[MAX_JMENO]; //jmeno textury
  bitmapa *p_bmp;     // pointer na bitmapu textury
  GLuint   text;      // jmeno textury
  GLenum   typ;       // typ textury (1D/2D)
  byte     load;      // = 1 text je nahrana, 0 - neni nahrana
  int      flag;      // flag - TEXT_GEN_DOT3/....
  int      flag2;     // flag - pouzita/flare...
  GLint    format;    // format texury
  int      trida;     // trida textury
  int      mip;       // mip_mapping
  int      mip_filtr; // filtr aplikovany na mip-mapy
  int      std_filtr; // std filtr
  GLint    wrap_x;    // wrap/clamp
  GLint    wrap_y;
  int      bump;
  int      alfa_stage;// alfa-stage z textury
  int      no_cull; // bez culingu
  int      floyd;
  int      floyd_transp;
  
} EDIT_TEXT, EDIT_TEXT_OLD;

// Konfigurace textury pro nahrani z disku (format&pod.)
typedef struct _EDIT_TEXT_KONFIG {

  char     bitmapa[200]; // bitmapa
  char     alfamap[200]; // alfamapa textury
  GLenum   typ;      // typ textury (1D/2D/3D/4D/5D/6D)
  GLint    format;   // format texury
  GLint    wrap_x;   // wrap/clamp
  GLint    wrap_y;
  int      mip;      // mip_mapping
  int      mip_filtr;// filtr aplikovany na mip-mapy
  int      alfa;     // alfa kanal ?
  int      std_filtr;
  int      bump;
  int      alfa_stage;// alfa-stage z textury
  int      no_cull; // bez culingu
  int      floyd;
  int      floyd_transp;

} EDIT_TEXT_KONFIG;

#define KONCOVKA_TEXT_INFO  ".btx"
#define MASKA_TEXT_INFO    "*.btx"

#define KONCOVKA_ALFA      "_a"


#define TEXT_16BPP           16
#define TEXT_32BPP           32
#define TEXT_DTX             128

void      txt_lightmap_konfig(EDIT_TEXT_KONFIG *p_konf);
bitmapa * txt_bmp2textura(bitmapa *p_bmp, EDIT_TEXT_OLD *p_text, EDIT_TEXT_KONFIG *p_konf, int _2d_text);
int       txt_reload_lightmap(EDIT_TEXT_OLD *p_text, bitmapa *p_bmp);

int  txt_nahraj_texturu_do_vram(EDIT_TEXT_OLD *p_text, EDIT_TEXT_KONFIG *p_konf);
int  txt_zrus_texturu_z_vram(EDIT_TEXT_OLD *p_text);
int  txt_nahraj_lightmapu_z_bmp(char *p_file, KFILE *f, EDIT_TEXT_OLD *p_text, int save);

int  txt_nahraj_texturu_z_func(APAK_HANDLE *pHandle, char *p_file, EDIT_TEXT_OLD *p_text, int save, int load, EDIT_TEXT_KONFIG *p_konf, AUX_RGBImageRec * (*p_load)(APAK_HANDLE *pAHandle, char *p_file));
int  txt_nahraj_texturu_z_dds(APAK_HANDLE *pHandle, char *p_file, EDIT_TEXT_OLD *p_text, int save);
int  txt_nahraj_texturu_z_lib(APAK_HANDLE *pHandle, char *p_file, EDIT_TEXT_OLD *p_text, int save);
int  txt_nahraj_texturu_z_tga(APAK_HANDLE *pHandle, char *p_file, EDIT_TEXT_OLD *p_text);
int  txt_nahraj_texturu_dot3(APAK_HANDLE *pHandle, char *p_file, EDIT_TEXT_OLD *p_text, int save);
int  txt_to_dot3(EDIT_TEXT_OLD *p_text, int save);
AUX_RGBImageRec * txt_lib_to_aux(APAK_HANDLE *pHandle, char *p_file);

int  txt_uvolni_texturu(EDIT_TEXT_OLD *p_text);
int  txt_zrus_texturu(EDIT_TEXT_OLD *p_text);
int  txt_zrus_texturu_ram(EDIT_TEXT_OLD *p_text);
AUX_RGBImageRec * nahraj_aux(APAK_HANDLE *pAHandle, char *p_file);
AUX_RGBImageRec * nahraj_aux_file(FILE *f);
bitmapa * txt_bmp2dot3(bitmapa *p_bmp);
void txt_default_konfig(char *p_file, EDIT_TEXT_KONFIG *p_konf, char *p_koncovka);
int  txt_uloz_btx(char *p_file, int typ, int wrap_x, int wrap_y);
int  txt_kopiruj(EDIT_TEXT_OLD *p_dest, EDIT_TEXT_OLD *p_src);

int  txt_vyrob_2D_texturu(int x, int y, int filtr, int format);
int  txt_zrus_2D_texturu(int *p_text);

int  txt_zrus_lightmapu(EDIT_TEXT_OLD *p_text);

byte * file_read(APAK_HANDLE *pHandle, char *p_file, int *p_read);

bitmapa * bmp_vyrob(int x, int y);
bitmapa * bmp_nahraj(char *p_file, int flip);
bitmapa * bmp_nahraj_pack(char *p_file, APAK_HANDLE *pAHandle);
int       bmp_uloz(char *p_file, bitmapa *p_bmp);
int       bmp_uloz_pack(FFILE f, bitmapa *p_bmp);
void      bmp_zrus(bitmapa **p_bmp);
bitmapa * bmp_kopituj(bitmapa *p_bmp);
inline void      bmp_cti_rozmery(bitmapa *p_bmp, int *p_x, int *p_y);
inline int       bmp_velikost(bitmapa *p_bmp);


typedef struct _bitmapa_pixel {
  byte r,g,b,a;
} bitmapa_pixel;
/*
inline dword barva_to_bmp(dword barva)
{
  return((barva << 8)|(barva >> 24));
}
*/
inline dword bmp_getpixel(bitmapa *p_bmp, int x, int y)
{
 return(p_bmp->data[p_bmp->x*y+x]);
}

inline bitmapa_pixel bmp_getpixel_pix(bitmapa *p_bmp, int x, int y)
{
 bitmapa_pixel *p_pix = (bitmapa_pixel *)p_bmp->data;
 return(p_pix[p_bmp->x*y+x]);
}

inline void bmp_putpixel(bitmapa *p_bmp, int x, int y, dword barva)
{
 p_bmp->data[p_bmp->x*y+x] = barva;
}

inline void bmp_putpixel_pix(bitmapa *p_bmp, int x, int y, bitmapa_pixel barva)
{
 bitmapa_pixel *p_pix = (bitmapa_pixel *)p_bmp->data;
 p_pix[p_bmp->x*y+x] = barva;
}

inline void bmp_cti_rozmery(bitmapa *p_bmp, int *p_x, int *p_y)
{
  if(p_bmp) {
    *p_x = p_bmp->x;
    *p_y = p_bmp->y;
  } else {
    *p_x = 0;
    *p_y = 0;
  }
}

// kopiruje bitmapu do druhe vetsi na zadane souradnice
// src->dest
inline void bmp_kopiruj_rec(bitmapa *p_src, bitmapa *p_desc, int xp, int yp)
{  
  int   x,y;

  for(x = 0; x < p_src->x; x++) {
    for(y = 0; y < p_src->y; y++) {
       bmp_putpixel(p_desc,x+xp,y+yp,bmp_getpixel(p_src,x,y));
    }
  }
}

// src->dest
inline void bmp_vyber_rec(bitmapa *p_src, bitmapa *p_desc, int xp, int yp)
{  
  int   x,y;

  for(x = 0; x < p_desc->x; x++) {
    for(y = 0; y < p_desc->y; y++) {
       bmp_putpixel(p_desc,x,y,bmp_getpixel(p_src,x+xp,y+yp));
    }
  }
}

// kopie src->desc
inline void bmp_kopiruj(bitmapa *p_src, bitmapa *p_desc)
{  
  p_desc->x = p_src->x;
  p_desc->y = p_src->y;
  memcpy(p_desc->data,p_src->data,sizeof(dword)*p_src->x*p_src->y);  
}

inline void bmp_smaz(bitmapa *p_text, dword barva)
{   
  int i,n = p_text->x*p_text->y; 
  dword *p_col = p_text->data;
  for(i = 0; i < n; i++, p_col++)
    *p_col = barva;      
}

inline byte PackFloatInByte(float in)
{
   return (byte) ((in+1.0f) / 2.0f * 255.0f);
}

inline int bmp_velikost(bitmapa *p_bmp)
{
  return(p_bmp->x*p_bmp->y*4);
}

void   bmp_to_16bit(bitmapa *p_bmp, byte maska, int pruh);

/*
#define  TEXT_MENU   0
#define  TEXT_SYSTEM 1
#define  TEXT_PRVKY  2
#define  TEXT_SCENA  3
#define  TEXT_LIGHT  4
#define  TEXT_TRID   5
*/

void txt_trida(int trida);

#endif
