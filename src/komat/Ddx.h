/*
  Headr fajl
*/
#ifndef __DDX_H
#define __DDX_H

typedef struct _HW_KONFIG {

  int    fullscreen;
  int    frekvence;
  int    safe_frekvence;
  
  int    xres;   // rozliseni obrazovky
  int    yres; 
  int    xstart; // start
  int    ystart;
  int    bpp;
  int    ditering;
  int    typ_mlhy;
  int    pn_triangles;
  int    pn_triangles_detail;
  int    bump_mapping;
  int    bump_mapping_typ;
  int    vertex_arrays;

  // TODO
  //HDC    hDC;
  //HGLRC  hRC;
  byte **p_ext;
  int    extnum;
  int    perspective;
  GLuint font_baze;

  // TODO
  //PIXELFORMATDESCRIPTOR pfd;

} HW_KONFIG;

#define  FORMAT_POCET       8

#define  FORMAT_RGB         0  // format pro standartni rgb texturu
#define  FORMAT_RGBA1       1  // format pro rgb texturu s jednoduchou alfou
#define  FORMAT_RGBA4       2  // format pro rgb texturu s jemnou alfou
#define  FORMAT_LUM         3  // format pro svetlou/tmavou texturu 
#define  FORMAT_LUM_ALFA1   4  // format pro svetlou/tmavou texturu + alfa
#define  FORMAT_LUM_ALFA4   5  // format pro svetlou/tmavou texturu + alfa
#define  FORMAT_ALFA        6  // format pouze s alfa slozkou textury
#define  FORMAT_LIGHTMAPY   7  // format pro standartni rgb texturu

#define  TEXT_MENU   0
#define  TEXT_SYSTEM 1
#define  TEXT_PRVKY  2
#define  TEXT_SCENA  3
#define  TEXT_LIGHT  4
#define  TEXT_TRID   5

// Konfigurace textur
typedef struct _TXT_KONFIG {

  int   text_kvalita;
  int   text_komprese;
  int   text_mip_mapping;
  int   text_mip_filtr;
  int   text_filtr;
  int   text_detail[TEXT_TRID]; 
  int   text_bump[TEXT_TRID];
  int   text_trida;
  int   text_bpp;
  int   text_perspective;
  int   text_max_x;
  int   text_max_y;
  int   text_bpp_plocha;
  int   text_wrap_x;
  int   text_wrap_y;
  int   text_anis;
  float text_anis_level;
  float text_ostrost;
  int   format1d[FORMAT_POCET];      // format pro texturu
  int   format2d[FORMAT_POCET];      // format pro texturu

} TXT_KONFIG;

extern   HW_KONFIG          hwconf;
extern   TXT_KONFIG         txconf;

/*
#define  XRES              (hwconf.xres)
#define  YRES              (hwconf.yres)
*/

#define  TXT_HI_RES_LOW_X  128
#define  TXT_HI_RES_LOW_Y  128

#define  TXT_SEKCE        "textury"


int gl_init(HW_KONFIG *p_conf);
int gl_stop(HW_KONFIG *p_conf);
int grf_prehod_mod_hra(HW_KONFIG *p_conf);
int grf_prehod_mod_zpet(HW_KONFIG *p_conf);
int grf_prehod_mod_menu(void);
int grf_start(byte *p_file, byte *p_sekce, int extension);
int grf_stop();


void nastav_konfig(HW_KONFIG  *p_hwconf, TXT_KONFIG *p_txt);
int  nahraj_device_config(byte *p_file, byte *p_sekce, HW_KONFIG *p_conf);
int  nahraj_universal_device_config(byte *p_file, byte *p_sekce, HW_KONFIG *p_conf);
int  nahraj_texture_config(byte *p_file, TXT_KONFIG *p_txt);
void ddw_surf(int x, int y, byte *fmt,...);
void ddw_surf_xy(int x, int y, byte *fmt,...);
int  input_surf(int x, int y, byte *p_text, int max);
int  input_surf_ddw(int x, int y, byte *p_text, void *p_1, void *p_2, void *p_3, void *p_4, void *p_5, void *p_6, void *p_7, void *p_8, void *p_9);
int  ddwqueto(byte *p_text,...);
int  ddwquetot(byte *p_title, byte *p_text,...);
void tiskni_chybu(int line, char *p_file, char *p_text,...);
void tiskni_chybu_bez_exit(int line, char *p_file, char *p_text,...);
void rozeber_chybu(void);
int  cti_texture_format(int format, byte *p_string);
int  preloz_texture_format(int *p_format, byte *p_string);

void BuildFont(void);
void KillFont(void);
void glPrint(const char *fmt, ...);
void glChyba(void);

#define printfs   ddw_surf
#define printfsx  ddw_surf_xy

void pn_triangles_init(HW_KONFIG *p_hwconf);
void anisotropic_filtr_init(TXT_KONFIG *p_txt);

#define VYSKA_FONTU 15
#define SIRKA_FONTU 12

void mfree(void **p_mem);

void gl_texture_scan(void);

inline void flip(void)
{
  // TODO
  //SwapBuffers(hwconf.hDC);
}

inline int fgets_korekce(byte *p_kor)
{
  int delka = strlen((const char *)p_kor);
  if(p_kor[delka-1] == '\n') {
    p_kor[delka-1] = 0;
    return(delka-1);
  } else {
    return(delka);
  }
}

#define MAX_FREKVENCE  85
#define SAFE_FREKVENCE 60

#endif
