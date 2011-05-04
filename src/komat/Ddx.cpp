/*
  Interface pro GL
*/
#include <stdarg.h>
#include "3d_all.h"

HW_KONFIG  hwconf;
TXT_KONFIG txconf;

#define FORMATU  24

int  cti_texture_format(int format, char *p_string)
{
  int i;
  int formaty_cisla[FORMATU] = {GL_RGB5,
                 GL_RGB5_A1,
                 GL_RGBA4,
                 GL_LUMINANCE8,
                 GL_LUMINANCE6_ALPHA2,
                 GL_LUMINANCE4_ALPHA4,
                 GL_ALPHA8,
                 GL_RGB5,
                 GL_RGB8,
                 GL_RGBA8,
                 GL_RGBA8,
                 GL_LUMINANCE8,
                 GL_LUMINANCE8_ALPHA8,
                 GL_LUMINANCE8_ALPHA8,
                 GL_ALPHA8,
                 GL_RGB8,
                 GL_COMPRESSED_RGB_ARB,
                 GL_COMPRESSED_RGBA_ARB,
                 GL_COMPRESSED_RGBA_ARB,
                 GL_COMPRESSED_LUMINANCE_ARB,
                 GL_COMPRESSED_LUMINANCE_ALPHA_ARB,
                 GL_COMPRESSED_LUMINANCE_ALPHA_ARB,
                 GL_COMPRESSED_ALPHA_ARB,
                 GL_RGB5};
  char formaty_stringy[FORMATU][50] = {"GL_RGB5",
                      "GL_RGB5_A1",
                      "GL_RGBA4",
                      "GL_LUMINANCE8",
                      "GL_LUMINANCE6_ALPHA2",
                      "GL_LUMINANCE4_ALPHA4",
                      "GL_ALPHA8",
                      "GL_RGB5",
                      "GL_RGB8",
                      "GL_RGBA8",
                      "GL_RGBA8",
                      "GL_LUMINANCE8",
                      "GL_LUMINANCE8_ALPHA8",
                      "GL_LUMINANCE8_ALPHA8",
                      "GL_ALPHA8",
                      "GL_RGB8",
                      "GL_COMPRESSED_RGB_ARB",
                      "GL_COMPRESSED_RGBA_ARB",
                      "GL_COMPRESSED_RGBA_ARB",
                      "GL_COMPRESSED_LUMINANCE_ARB",
                      "GL_COMPRESSED_LUMINANCE_ALPHA_ARB",
                      "GL_COMPRESSED_LUMINANCE_ALPHA_ARB",
                      "GL_COMPRESSED_ALPHA_ARB",
                      "GL_RGB5"};

  for(i = 0; i < FORMATU; i++) {
    if(formaty_cisla[i] == format) {
      strcpy(p_string,formaty_stringy[i]);
      return(TRUE);
    }
  }
  return(FALSE);
}

int preloz_texture_format(int *p_format, char *p_string)
{
  int i;
  char format_string[FORMAT_POCET][50] = {
       "FORMAT_RGB",
       "FORMAT_RGBA1",
       "FORMAT_RGBA4",
       "FORMAT_LUM",
       "FORMAT_LUM_ALFA1",
       "FORMAT_LUM_ALFA4",
       "FORMAT_ALFA",
       "FORMAT_LIGHTMAPY"};

  strupr(p_string);
  for(i = 0; i < FORMAT_POCET; i++) {
    if(!strcmp(format_string[i],p_string)) {
      *p_format = i;
      return(TRUE);
    }
  }
  *p_format = 0;
  return(FALSE);
}

/* Nahravaci retezec textur:

  1. Load do raw
  2. Orezani na velikost graf karty
  3. Vyrobeni mip-mappingu
  4. Prevedeni do textur
*/

void nastav_konfig(HW_KONFIG *p_hwconf, TXT_KONFIG *p_txt)
{
  int max;

  /* Konfigurace - dithering
  */
  if(p_hwconf->ditering) {  
    glEnable(GL_DITHER);
  } else {    
    glDisable(GL_DITHER);
  } 
  
  kprintf(1,"Text Dithering: %s", p_hwconf->ditering ? "ON" : "OFF");

  /* Povoleni Z-bufferu
  */
  glEnable(GL_DEPTH_TEST);

  /* Alfa-test
  */
  glstav_alfa_test = TRUE;
  alfa_test_off();
  glAlphaFunc(GL_GREATER,0.5f);

  /* Nastaveni kvality
  */
  p_txt->text_perspective = TRUE;
  
  kprintf(1,"OpenGL Render quality: %s",p_txt->text_kvalita ? "Nicest" : "Fastest");
  glHint(GL_POLYGON_SMOOTH_HINT,(p_txt->text_kvalita) ? GL_NICEST : GL_FASTEST);
  
  kprintf(1,"Fog type: %s",p_hwconf->typ_mlhy ? "Pixel-Fog" : "Vertex-Fog");
  glHint(GL_FOG_HINT ,(p_hwconf->typ_mlhy) ? GL_NICEST : GL_FASTEST);

  /* Maximalni rozmery textur
  */
  glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
  kprintf(1,"Text maximal size %d pixels",max);
  
  if(max < p_txt->text_max_x) {
    p_txt->text_max_x = max;
  }
  
  if(max < p_txt->text_max_y) {
    p_txt->text_max_y = max;
  }

  if(p_txt->text_komprese) {
    if(gl_ext::extlist_text_compresion) {
      // Komprese je podporovana a povolena
      p_txt->text_bpp = TEXT_DTX;
    }
  }

  if(p_txt->text_bpp != TEXT_DTX) {
    if(p_txt->text_bpp_plocha) {
      kprintf(1,"Text automatic texture depth");      
      p_txt->text_bpp = p_hwconf->bpp;
    }
    kprintf(1,"Text depth %d bpp",p_txt->text_bpp);
  } else {
    kprintf(1,"Text DTX compress on",p_txt->text_bpp);
  }

  /* Konfigurace formatu textur
  */
  if(p_txt->text_bpp == TEXT_16BPP) {
    p_txt->format1d[FORMAT_RGB]       = p_txt->format2d[FORMAT_RGB]       = GL_RGB5;
    p_txt->format1d[FORMAT_RGBA1]     = p_txt->format2d[FORMAT_RGBA1]     = GL_RGB5_A1;
    p_txt->format1d[FORMAT_RGBA4]     = p_txt->format2d[FORMAT_RGBA4]     = GL_RGBA4;
    p_txt->format1d[FORMAT_LUM]       = p_txt->format2d[FORMAT_LUM]       = GL_LUMINANCE8;
    p_txt->format1d[FORMAT_LUM_ALFA1] = p_txt->format2d[FORMAT_LUM_ALFA1] = GL_LUMINANCE6_ALPHA2;
    p_txt->format1d[FORMAT_LUM_ALFA4] = p_txt->format2d[FORMAT_LUM_ALFA4] = GL_LUMINANCE4_ALPHA4;
    p_txt->format1d[FORMAT_ALFA]      = p_txt->format2d[FORMAT_ALFA]      = GL_ALPHA8;
    p_txt->format1d[FORMAT_LIGHTMAPY] = p_txt->format2d[FORMAT_LIGHTMAPY] = GL_RGB5;

  } else if(p_txt->text_bpp == TEXT_32BPP) {
    p_txt->format1d[FORMAT_RGB]       = p_txt->format2d[FORMAT_RGB]       = GL_RGB8;
    p_txt->format1d[FORMAT_RGBA1]     = p_txt->format2d[FORMAT_RGBA1]     = GL_RGBA8;
    p_txt->format1d[FORMAT_RGBA4]     = p_txt->format2d[FORMAT_RGBA4]     = GL_RGBA8;
    p_txt->format1d[FORMAT_LUM]       = p_txt->format2d[FORMAT_LUM]       = GL_LUMINANCE8;
    p_txt->format1d[FORMAT_LUM_ALFA1] = p_txt->format2d[FORMAT_LUM_ALFA1] = GL_LUMINANCE8_ALPHA8;
    p_txt->format1d[FORMAT_LUM_ALFA4] = p_txt->format2d[FORMAT_LUM_ALFA4] = GL_LUMINANCE8_ALPHA8;
    p_txt->format1d[FORMAT_ALFA]      = p_txt->format2d[FORMAT_ALFA]      = GL_ALPHA8;
    p_txt->format1d[FORMAT_LIGHTMAPY] = p_txt->format2d[FORMAT_LIGHTMAPY] = GL_RGB5;

  } else if(p_txt->text_bpp == TEXT_DTX) {
    
    p_txt->format1d[FORMAT_RGB] = GL_RGB5;
    p_txt->format1d[FORMAT_RGBA1] = GL_RGB5_A1;
    p_txt->format1d[FORMAT_RGBA4] = GL_RGBA4;
    p_txt->format1d[FORMAT_LUM] = GL_LUMINANCE8;
    p_txt->format1d[FORMAT_LUM_ALFA1] = GL_LUMINANCE6_ALPHA2;
    p_txt->format1d[FORMAT_LUM_ALFA4] = GL_LUMINANCE4_ALPHA4;
    p_txt->format1d[FORMAT_ALFA] = GL_ALPHA8;
    p_txt->format1d[FORMAT_LIGHTMAPY] = GL_RGB5;

    p_txt->format2d[FORMAT_RGB] = GL_COMPRESSED_RGB_ARB;
    p_txt->format2d[FORMAT_RGBA1] = GL_COMPRESSED_RGBA_ARB;
    p_txt->format2d[FORMAT_RGBA4] = GL_COMPRESSED_RGBA_ARB;
    p_txt->format2d[FORMAT_LUM] = GL_COMPRESSED_LUMINANCE_ARB;
    p_txt->format2d[FORMAT_LUM_ALFA1] = GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
    p_txt->format2d[FORMAT_LUM_ALFA4] = GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
    p_txt->format2d[FORMAT_ALFA] = GL_COMPRESSED_ALPHA_ARB;
    p_txt->format2d[FORMAT_LIGHTMAPY] = GL_RGB5;
  }

  if(p_txt->text_bpp != TEXT_32BPP &&
     p_txt->text_bpp != TEXT_16BPP &&
     p_txt->text_bpp != TEXT_DTX) {
    kprintf(1,"Bad texture depth %d, switch to 16bpp",p_txt->text_bpp);
    p_txt->text_bpp = TEXT_16BPP;
  }
 
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);
}

/* load hw konfigu
*/
int nahraj_device_config(char *p_file, char *p_sekce, HW_KONFIG *p_conf)
{
 p_conf->xres = GetPrivateProfileInt(p_sekce,"xres",800,p_file);
 p_conf->yres = GetPrivateProfileInt(p_sekce,"yres",600,p_file);
 p_conf->bpp = GetPrivateProfileInt(p_sekce,"bpp",16,p_file);
 p_conf->fullscreen = GetPrivateProfileInt(p_sekce,"fullscreen",1,p_file);
 
 p_conf->frekvence = GetPrivateProfileInt(p_sekce,"freq",0,p_file);
 p_conf->safe_frekvence = GetPrivateProfileInt(p_sekce,"safe_freq",0,p_file); 
 p_conf->ditering = GetPrivateProfileInt(p_sekce,"ditering",0,p_file);
 p_conf->typ_mlhy = GetPrivateProfileInt(p_sekce,"typ_mlhy",0,p_file);
 p_conf->bump_mapping = GetPrivateProfileInt(p_sekce,"bump_mapping",0,p_file);
 p_conf->bump_mapping_typ = AUTO_BUMP_DOT3;
 p_conf->vertex_arrays = GetPrivateProfileInt(p_sekce,"vertex_arrays",0,p_file); 

 return(TRUE);
}

int nahraj_universal_device_config(char *p_file, char *p_sekce, HW_KONFIG *p_conf)
{ 
 p_conf->pn_triangles = GetPrivateProfileInt(p_sekce,"pn_triangles",0,p_file);
 p_conf->pn_triangles_detail = GetPrivateProfileInt(p_sekce,"pn_triangles_tesell",0,p_file);
 return(TRUE);
}

/* load konfigu textur
*/
int nahraj_texture_config(char *p_file, TXT_KONFIG *p_txt)
{
 char pom[200];
 int  i;
 
 p_txt->text_kvalita =     GetPrivateProfileInt(TXT_SEKCE,"text_kvalita",0,p_file); 
 p_txt->text_komprese =    GetPrivateProfileInt(TXT_SEKCE,"text_komprese",0,p_file);
 p_txt->text_bpp =         GetPrivateProfileInt(TXT_SEKCE,"text_bpp",16,p_file);
 p_txt->text_mip_mapping = GetPrivateProfileInt(TXT_SEKCE,"text_mip_mapping",1,p_file);
 p_txt->text_mip_filtr =   GetPrivateProfileInt(TXT_SEKCE,"text_mip_filtr",1,p_file);
 p_txt->text_perspective = GetPrivateProfileInt(TXT_SEKCE,"text_perspective",1,p_file);
 p_txt->text_max_x =       GetPrivateProfileInt(TXT_SEKCE,"text_max_x",256,p_file);
 p_txt->text_max_y =       GetPrivateProfileInt(TXT_SEKCE,"text_max_y",256,p_file);
 p_txt->text_bpp_plocha =  GetPrivateProfileInt(TXT_SEKCE,"text_bpp_plocha",0,p_file);
 p_txt->text_anis =        GetPrivateProfileInt(TXT_SEKCE,"text_ans",0,p_file);
 p_txt->text_anis_level = (float)GetPrivateProfileInt(TXT_SEKCE,"text_ans_stupen",1,p_file);
 p_txt->text_wrap_x = 1;
 p_txt->text_wrap_y = 1;
 GetPrivateProfileString(TXT_SEKCE,"text_ostrost","0.0",pom,200,p_file);
 p_txt->text_ostrost = (float)atof(pom);
 
 for(i = 0; i < TEXT_TRID; i++) {
   sprintf(pom,"text_detail%d",i);
   p_txt->text_detail[i] = GetPrivateProfileInt(TXT_SEKCE,pom,0,p_file);
   sprintf(pom,"text_bump%d",i);
   p_txt->text_bump[i] = GetPrivateProfileInt(TXT_SEKCE,pom,0,p_file);
 }  

 return(1);
}

void anisotropic_filtr_init(TXT_KONFIG *p_txt)
{
  float max_anis,as,akt_anis;
  int   i,imax;

  if(p_txt->text_anis && gl_ext::extlist_text_filter_anisotropic) {
    
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anis);
    
    akt_anis = p_txt->text_anis_level;
    if(akt_anis > 5)
      akt_anis = 5;
    
    imax = (int)akt_anis-1;
    for(as = 2, i = 0; i < imax; i++, as *= 2);

    if(as > max_anis)
      as = max_anis;

    p_txt->text_anis_level = as/max_anis;
    kprintf(TRUE,"Text Max Anisotropy %.1f, akt %.1f",max_anis,p_txt->text_anis_level);
  } else {
    p_txt->text_anis = FALSE;
  }
}

int grf_start(char *p_file)
{
 nahraj_texture_config(p_file,&txconf);

 if(!gl_init(&hwconf)) {
   kprintf(1,"Unable initialize OpenGL!");
   return(FALSE);
 }

 /* Nahozeni stavu OpenGL
 */
 nastav_konfig(&hwconf,&txconf);
 anisotropic_filtr_init(&txconf);
 
 BuildFont();
 return(TRUE);
}

int grf_stop(void)
{
 KillFont();
 return(1);
}

int rozeber_switch(long shit)
{
/*
  switch(shit) {
    case DISP_CHANGE_SUCCESSFUL:
      assert(0);
      break;
    case DISP_CHANGE_RESTART:
      assert(0);
      break;
    case DISP_CHANGE_BADFLAGS:
      assert(0);
      break;
    case DISP_CHANGE_BADPARAM:
      assert(0);
      break;
    case DISP_CHANGE_FAILED:
      assert(0);
      break;
    case DISP_CHANGE_BADMODE:
      assert(0);
      break;
    case DISP_CHANGE_NOTUPDATED: 
      assert(0);
      break;
  }
*/
  return(TRUE);
}

int cti_max_frekvenci_test(int xPels, int yPels, int Bits, int freq)
{
/*
	long l;
	DEVMODE	devmode;
	ZeroMemory(&devmode, sizeof(DEVMODE));

	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmBitsPerPel = Bits;
	devmode.dmPelsWidth  = xPels;
	devmode.dmPelsHeight = yPels;
	devmode.dmDisplayFrequency = freq;
	devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

	l = ChangeDisplaySettings(&devmode, CDS_TEST);

	kprintf(1, "Grafic_Test %dx%dx%d %dHz ...", xPels, yPels, Bits, freq);

	switch(l)
	{
	case DISP_CHANGE_SUCCESSFUL:
		kprintf(1, "DISP_CHANGE_SUCCESSFUL");
		break;
	case DISP_CHANGE_RESTART:
		kprintf(1, "DISP_CHANGE_RESTART");
		break;
	case DISP_CHANGE_BADFLAGS:
		kprintf(1, "DISP_CHANGE_BADFLAGS");
		break;
	case DISP_CHANGE_BADPARAM:
		kprintf(1, "DISP_CHANGE_BADPARAM");
		break;
	case DISP_CHANGE_FAILED:
		kprintf(1, "DISP_CHANGE_FAILED");
		break;
	case DISP_CHANGE_BADMODE:
		kprintf(1, "DISP_CHANGE_BADMODE");
		break;
	case DISP_CHANGE_NOTUPDATED:
		kprintf(1, "DISP_CHANGE_NOTUPDATED");
		break;
	}

  if(l != DISP_CHANGE_SUCCESSFUL) {
		return(0);
  }
  else {
    kprintf(TRUE,"Test %d OK",freq);
		return(1);
  }
  */
}

int cti_max_frekvenci(int xPels, int yPels, int bpp)
{
/*
	DEVMODE dmSettings;
  int     i;
  dword   max = 0;

	memset(&dmSettings,0,sizeof(dmSettings));	
	dmSettings.dmSize = sizeof(DEVMODE);
	
  kprintf(TRUE,"Test frekvence pro %dx%d,%dbpp",xPels,yPels,bpp);

	for(i = 0; EnumDisplaySettings(NULL,i,&dmSettings); i++) {
		if(dmSettings.dmPelsWidth == (unsigned)xPels && dmSettings.dmPelsHeight == (unsigned)yPels && dmSettings.dmBitsPerPel == (unsigned)bpp) {
      if(cti_max_frekvenci_test(xPels, yPels, bpp, dmSettings.dmDisplayFrequency)) {
				if(max < dmSettings.dmDisplayFrequency)
				  max = dmSettings.dmDisplayFrequency;
      }
		}
		memset(&dmSettings,0,sizeof(dmSettings));
		dmSettings.dmSize = sizeof(DEVMODE);
	}

  if(max > MAX_FREKVENCE)
    max = MAX_FREKVENCE;

  kprintf(TRUE,"Max frekvence %d",max);
	return(max);
  */
}


int grf_prehod_mod_hra(HW_KONFIG *p_conf)
{
/*
  LONG vysl;

  if(p_conf->fullscreen) {    
    DEVMODE dmScreenSettings;					// Device Mode

    kprintf(1,"Prepinam do %dx%d %dbpp...",p_conf->xres,p_conf->yres,p_conf->bpp);    
    
    memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
    dmScreenSettings.dmPelsWidth	= p_conf->xres;			// Selected Screen Width
    dmScreenSettings.dmPelsHeight	= p_conf->yres;			// Selected Screen Height
    dmScreenSettings.dmBitsPerPel	= p_conf->bpp;				// Selected Bits Per Pixel
    dmScreenSettings.dmFields= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
    dmScreenSettings.dmDisplayFrequency = cti_max_frekvenci(p_conf->xres, p_conf->yres, p_conf->bpp);
    
    if(dmScreenSettings.dmDisplayFrequency && p_conf->safe_frekvence)
       dmScreenSettings.dmDisplayFrequency = SAFE_FREKVENCE;

    if(dmScreenSettings.dmDisplayFrequency > 0) {
      dmScreenSettings.dmFields |= DM_DISPLAYFREQUENCY;
      kprintf(1,"Freqvence %d...",dmScreenSettings.dmDisplayFrequency);
    }
    
    vysl = ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
    if(vysl != DISP_CHANGE_SUCCESSFUL) {
      rozeber_switch(vysl);
      kprintf(1,"Full-screen is not availabile!");
      return(FALSE);
    } else {
      kprintf(1,"Switch to full-screen %dx%d %dbpp freqv %d ok",p_conf->xres,p_conf->yres,p_conf->bpp,p_conf->frekvence);
    }    
  }
  ShowCursor(FALSE);
  */
  return(TRUE);
}

#define MENU_XRES 1024
#define MENU_YRES 768
#define MENU_BPP  16

int grf_prehod_mod_menu(void)
{
/*
  LONG vysl;
  
  DEVMODE dmScreenSettings;					// Device Mode
  memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
  dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
  dmScreenSettings.dmPelsWidth	= MENU_XRES;			// Selected Screen Width
  dmScreenSettings.dmPelsHeight	= MENU_YRES;			// Selected Screen Height
  dmScreenSettings.dmBitsPerPel	= MENU_BPP;				// Selected Bits Per Pixel
  dmScreenSettings.dmFields= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

  dmScreenSettings.dmDisplayFrequency = cti_max_frekvenci(MENU_XRES, MENU_YRES, MENU_BPP);
  if(dmScreenSettings.dmDisplayFrequency && hwconf.safe_frekvence)
    dmScreenSettings.dmDisplayFrequency = SAFE_FREKVENCE;

  if(dmScreenSettings.dmDisplayFrequency > 0) {
    dmScreenSettings.dmFields |= DM_DISPLAYFREQUENCY;
  }

  vysl = ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
  if(vysl != DISP_CHANGE_SUCCESSFUL) {
    rozeber_switch(vysl);
    kprintf(1,"Full-screen is not availabile!");
    return(FALSE);
  } else {
    kprintf(1,"Switch to MENU ok");
  }
  
  ShowCursor(FALSE);
  */
  return(TRUE);
}

int grf_prehod_mod_zpet(HW_KONFIG *p_conf)
{
  if(p_conf->fullscreen) {
    //ChangeDisplaySettings(NULL,0);
    ShowCursor(TRUE);
  }
  return(TRUE);
}

int gl_init(HW_KONFIG *p_conf)
{
/*
  PIXELFORMATDESCRIPTOR pfd;
  GLuint	PixelFormat;		// Holds The Results After Searching For A Match
  RECT    rec;

  memset(&pfd,0,sizeof(pfd));
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  
  pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER|PFD_SWAP_EXCHANGE;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = p_conf->bpp;
  pfd.cDepthBits = p_conf->bpp;
  pfd.iLayerType = PFD_MAIN_PLANE;


  if(!p_conf->fullscreen) {
    // Nacteni rozmeru z okna
    GetWindowRect(hwnd,&rec);
    p_conf->xres = rec.right - rec.left;
    p_conf->yres = rec.bottom - rec.top;
  }
    
  pfd.cColorBits = p_conf->bpp;

  kprintf(1,"gl_Get hDC...");
  if(!(p_conf->hDC = GetDC(hwnd))) {
    return(FALSE);
  }
  
  kprintf(1,"gl_ChoosePixelFormat...");
  if(!(PixelFormat = ChoosePixelFormat(p_conf->hDC,&pfd))) {
    return(FALSE);
  }
  
  p_conf->pfd = pfd;

  kprintf(1,"gl_SetPixelFormat...");
  if(!SetPixelFormat(p_conf->hDC,PixelFormat,&pfd)) {
    return(FALSE);
  }
  
  kprintf(1,"wglCreateContext...");
  if(!(p_conf->hRC = wglCreateContext(p_conf->hDC))) {
    return(FALSE);
  }
  
  kprintf(1,"wglMakeCurrent...");
  if(!wglMakeCurrent(p_conf->hDC,p_conf->hRC)) {
    return(FALSE);
  }
*/
  return(TRUE);
}


#define KONZOLE_POZADI  0xffffffff
#define KONZOLE_TEXT    0x0

void ddw_surf(int x, int y, char *fmt,...)
{
 char text[1000];	
 va_list	ap;								  		  // Pointer To List Of Arguments

 if(fmt == NULL)									// If There's No Text
   return;									   		  // Do Nothing

 //glColor3f(0.0f,0.0f,0.0f);
 glColor3f(1.0f,1.0f,1.0f);
 glRasterPos2i(x*SIRKA_FONTU,y*VYSKA_FONTU);

 va_start(ap, fmt);								// Parses The String For Variables
 vsprintf(text, fmt, ap);					// And Converts Symbols To Actual Numbers
 va_end(ap);											  // Results Are Stored In Text
  
 glPushAttrib(GL_LIST_BIT);				// Pushes The Display List Bits
 glListBase(hwconf.font_baze-32); // Sets The Base Character to 32
 glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
 glPopAttrib();										// Pops The Display List Bits 
}

void ddw_surf_xy(int x, int y, char *fmt,...)
{
 char text[1000];	
 va_list	ap;								  		  // Pointer To List Of Arguments

 if(fmt == NULL)									// If There's No Text
   return;									   		  // Do Nothing

 //glColor3f(0.0f,0.0f,0.0f);
 glColor3f(1.0f,1.0f,1.0f);
 glRasterPos2i(x,y);

 va_start(ap, fmt);								// Parses The String For Variables
 vsprintf(text, fmt, ap);					// And Converts Symbols To Actual Numbers
 va_end(ap);											  // Results Are Stored In Text
  
 glPushAttrib(GL_LIST_BIT);				// Pushes The Display List Bits
 glListBase(hwconf.font_baze-32); // Sets The Base Character to 32
 glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
 glPopAttrib();										// Pops The Display List Bits 
}

int ddwqueto(char *p_text,...)
{
 char    text[500];
 va_list argumenty;

 va_start(argumenty,p_text);
 vsprintf(text,p_text,argumenty);
 va_end(argumenty);

// return(MessageBox(hwnd,text,"DdwQueto:",MB_ICONASTERISK|MB_YESNO|MB_SYSTEMMODAL) == IDYES);
}

int ddwquetot(char *p_title, char *p_text,...)
{
 char    text[500];
 va_list argumenty;

 va_start(argumenty,p_text);
 vsprintf(text,p_text,argumenty);
 va_end(argumenty);

// return(MessageBox(hwnd,text,p_title,MB_ICONASTERISK|MB_YESNO|MB_SYSTEMMODAL) == IDYES);
}

void gl_texture_scan(void)
{
  dword i,num,max = 0;

  num = 0;
  max--;
  for(i = 0; i < max; i++)
    if(glIsTexture(i))
      num++;

  kprintf(TRUE,"Textur %d...",num);
}

/*
  tiskni chybu
*/
void tiskni_chybu(int line, char *p_file, char *p_text,...)
{
  char    text[2000];
  char    text1[2000];
  va_list argumenty;

  sprintf(text1,"Chyba na line %d file %s\n",line,p_file); 

  va_start(argumenty,p_text);
  vsprintf(text,p_text,argumenty);
  va_end(argumenty);
  
  strcat(text1,text);
  fprintf(stderr,text1);

  assert(0);
  exit(0);
}

void tiskni_chybu_bez_exit(int line, char *p_file, char *p_text,...)
{
  char    text[2000];
  char    text1[2000];
  va_list argumenty;

  sprintf(text1,"Chyba na line %d file %s\n",line,p_file); 

  va_start(argumenty,p_text);
  vsprintf(text,p_text,argumenty);
  va_end(argumenty);
  
  strcat(text1,text);  
  fprintf(stderr,text1);
}

void mfree(void **p_mem)
{
  if(*p_mem) {
    free((*p_mem));
    *p_mem = NULL;
  }
}

void rozeber_chybu(void)
{
  
}


/* Taken from NeHe tutorials:
*/
/*
 * This code was created by Jeff Molofee '99 
 * (ported to Linux/SDL by Ti Leggett '01)
 *
 * If you've found this code useful, please let me know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 * 
 * or for port-specific comments, questions, bugreports etc. 
 * email to leggett@eecs.tulane.edu
 */
void BuildFont(void)		// Build Our Bitmap Font
{
  Display *dpy;          /* Our current X display */
  XFontStruct *fontInfo; /* Our font info */
  
  /* Sotrage for 96 characters */
  hwconf.font_baze = glGenLists( 96 );
  
  /* Get our current display long enough to get the fonts */
  dpy = XOpenDisplay( NULL );
  
  /* Get the font information */
  fontInfo = XLoadQueryFont( dpy, "-adobe-helvetica-medium-r-normal--18-*-*-*-p-*-iso8859-1" );  
  
  /* If the above font didn't exist try one that should */
  if ( fontInfo == NULL )
  {
      fontInfo = XLoadQueryFont( dpy, "fixed" );
      /* If that font doesn't exist, something is wrong */
      if ( fontInfo == NULL )
      {
          fprintf( stderr, "no X font available?\n" );
          return;
      }
  }
  
  /* generate the list */
  glXUseXFont( fontInfo->fid, 32, 96, hwconf.font_baze);
  
  /* Recover some memory */
  XFreeFont( dpy, fontInfo );
  
  /* close the display now that we're done with it */
  XCloseDisplay( dpy );
}

void KillFont(void)									// Delete The Font
{
	glDeleteLists(hwconf.font_baze, 96);	// Delete All 96 Characters
}

void glChyba(void)
{
  dword chyba = glGetError();
  switch(chyba) {
     case 0:
       break;
    case GL_INVALID_ENUM:
      kprintf(1,"GL_INVALID_ENUM");
      assert(0);
      break;
    case GL_INVALID_VALUE:
      kprintf(1,"GL_INVALID_VALUE");
      assert(0);
      break;
    case GL_INVALID_OPERATION:
      kprintf(1,"GL_INVALID_OPERATION");
      assert(0);
      break;
    case GL_STACK_OVERFLOW:
      kprintf(1,"GL_STACK_OVERFLOW");
      assert(0);
      break;
    case GL_STACK_UNDERFLOW:
      kprintf(1,"GL_STACK_UNDERFLOW");
      assert(0);
      break;
    case GL_OUT_OF_MEMORY:
      kprintf(1,"GL_OUT_OF_MEMORY");
      assert(0);
      break;
    default:
      assert(0);
      break;
  }
}
