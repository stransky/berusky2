/*
  color.h
*/
#ifndef __COLOR_H
#define __COLOR_H

inline dword int_rgb(int r, int g, int b)
{
  dword barva;  

  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  barva  =  r;
  barva |=  g<<8;
  barva |=  b<<16;

  return(barva);
}

inline dword byte_rgb(byte r, byte g, byte b)
{
  dword barva;  
  barva  =  r;
  barva |=  g<<8;
  barva |=  b<<16;
  return(barva);
}

inline dword rgb_byte(dword barva, byte *p_r, byte *p_g, byte *p_b)
{    
  *p_r = (byte)((barva)&0xff);
  *p_g = (byte)((barva>>8)&0xff);
  *p_b = (byte)((barva>>16)&0xff);
  return(barva);
}

inline dword rgb_int(dword barva, int *p_r, int *p_g, int *p_b)
{    
  *p_r = (byte)((barva)&0xff);
  *p_g = (byte)((barva>>8)&0xff);
  *p_b = (byte)((barva>>16)&0xff);
  return(barva);
}

inline dword byte_a(dword barva, int a)
{  
  return(barva|(a<<24));
}

inline byte a_byte(dword barva)
{  
  return((byte)(barva>>24));
}

inline dword float_rgba(float *p_barva)
{
  dword barva;  
  barva  =   (int)(p_barva[0]*255.0f);
  barva |=  ((int)(p_barva[1]*255.0f)<<8);
  barva |=  ((int)(p_barva[2]*255.0f)<<16);
  barva |=  ((int)(p_barva[3]*255.0f)<<24);
  return(barva);
}

inline dword float_rgb(float *p_barva)
{
  dword barva;  
  barva  =   (int)(p_barva[0]*255.0f);
  barva |=  ((int)(p_barva[1]*255.0f)<<8);
  barva |=  ((int)(p_barva[2]*255.0f)<<16);
  return(barva);
}

inline dword float_a(float alfa)
{
  return((int)(alfa*255.0f));
}

inline float * rgba_float(dword barva, float *p_barva)
{  
  p_barva[0] = ((float)(barva&0xff))/255.0f;
  p_barva[1] = ((float)((barva&0xff00)>>8))/255.0f;
  p_barva[2] = ((float)((barva&0xff0000)>>16))/255.0f;
  p_barva[3] = ((float)((barva&0xff000000)>>24))/255.0f;
  return(p_barva);
}

inline float * rgb_float(dword barva, float *p_barva)
{  
  p_barva[0] = ((float)(barva&0xff))/255.0f;
  p_barva[1] = ((float)((barva&0xff00)>>8))/255.0f;
  p_barva[2] = ((float)((barva&0xff0000)>>16))/255.0f;
  return(p_barva);
}

inline float a_float(dword barva)
{  
  return(((float)(barva&0xff))/255.0f);
}

// Prace s barvama
inline void add_color(dword *p_baze, dword add)
{ // BGR
  dword rs,gs,bs,as;
  dword rd,gd,bd,ad;

  rd = ((*p_baze)&0xff);
  gd = ((*p_baze)&0xff00)>>8;
  bd = ((*p_baze)&0xff0000)>>16;
  ad = ((*p_baze)&0xff000000)>>24;

  rs = ((add)&0xff);
  gs = ((add)&0xff00)>>8;
  bs = ((add)&0xff0000)>>16;
  as = ((add)&0xff000000)>>24;

  rd += rs;
  gd += gs;
  bd += bs;
  ad += as;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;
  if(ad > MAX_BYTE) ad = MAX_BYTE;

  *p_baze  = rd;
  *p_baze |= (gd<<8);
  *p_baze |= (bd<<16);
  *p_baze |= (ad<<24);
}

inline void add_color_f(float *p_zdroj, float r, float g, float b)
{ // BGR
  p_zdroj[0] += r;
  p_zdroj[1] += g;
  p_zdroj[2] += b;  
   
  if(p_zdroj[0] > 1.0f) p_zdroj[0] = 1.0f;
  if(p_zdroj[1] > 1.0f) p_zdroj[1] = 1.0f;
  if(p_zdroj[2] > 1.0f) p_zdroj[2] = 1.0f;   
}

inline void add_color_fa(float *p_zdroj, float r, float g, float b, float a)
{ // BGR
  p_zdroj[0] += r;
  p_zdroj[1] += g;
  p_zdroj[2] += b;  
  p_zdroj[3] += a;  
   
  if(p_zdroj[0] > 1.0f) p_zdroj[0] = 1.0f;
  if(p_zdroj[1] > 1.0f) p_zdroj[1] = 1.0f;
  if(p_zdroj[2] > 1.0f) p_zdroj[2] = 1.0f;   
  if(p_zdroj[3] > 1.0f) p_zdroj[3] = 1.0f;
}

// r,g,b,a
inline void add_color_p(float *p_zdroj, float *p_add)
{ // BGR
  p_zdroj[0] += p_add[0];
  p_zdroj[1] += p_add[1];
  p_zdroj[2] += p_add[2];  
  
  if(p_zdroj[0] > 1.0f) p_zdroj[0] = 1.0f;
  if(p_zdroj[1] > 1.0f) p_zdroj[1] = 1.0f;
  if(p_zdroj[2] > 1.0f) p_zdroj[2] = 1.0f;  
}

inline void add_color_pa(float *p_zdroj, float *p_add)
{ // BGR
  p_zdroj[0] += p_add[0];
  p_zdroj[1] += p_add[1];
  p_zdroj[2] += p_add[2];  
  p_zdroj[3] += p_add[3];  
  
  if(p_zdroj[0] > 1.0f) p_zdroj[0] = 1.0f;
  if(p_zdroj[1] > 1.0f) p_zdroj[1] = 1.0f;
  if(p_zdroj[2] > 1.0f) p_zdroj[2] = 1.0f;  
  if(p_zdroj[3] > 1.0f) p_zdroj[3] = 1.0f;
}

inline void sub_color(dword *p_baze, dword add)
{ // BGR
  int rs,gs,bs,as;
  int rd,gd,bd,ad;

  rd = ((*p_baze)&0xff);
  gd = ((*p_baze)&0xff00)>>8;
  bd = ((*p_baze)&0xff0000)>>16;
  ad = ((*p_baze)&0xff000000)>>24;

  rs = ((add)&0xff);
  gs = ((add)&0xff00)>>8;
  bs = ((add)&0xff0000)>>16;
  as = ((add)&0xff000000)>>24;

  rd -= rs;
  gd -= gs;
  bd -= bs;
  ad -= as;
  
  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;
  if(ad < 0) ad = 0;

  *p_baze  = rd;
  *p_baze |= (gd<<8);
  *p_baze |= (bd<<16);
  *p_baze |= (ad<<24);
}

inline void sub_color_f(float *p_zdroj, float r, float g, float b)
{ // BGR
  p_zdroj[0] -= r;
  p_zdroj[1] -= g;
  p_zdroj[2] -= b;
  
  if(p_zdroj[0] < 0.0f) p_zdroj[0] = 0.0f;
  if(p_zdroj[1] < 0.0f) p_zdroj[1] = 0.0f;
  if(p_zdroj[2] < 0.0f) p_zdroj[2] = 0.0f;  
}

inline void sub_color_fa(float *p_zdroj, float r, float g, float b, float a)
{ // BGR
  p_zdroj[0] -= r;
  p_zdroj[1] -= g;
  p_zdroj[2] -= b;
  p_zdroj[3] -= a;
  
  if(p_zdroj[0] < 0.0f) p_zdroj[0] = 0.0f;
  if(p_zdroj[1] < 0.0f) p_zdroj[1] = 0.0f;
  if(p_zdroj[2] < 0.0f) p_zdroj[2] = 0.0f;  
  if(p_zdroj[3] < 0.0f) p_zdroj[3] = 0.0f;  
}

inline void sub_color_p(float *p_zdroj, float *p_sub)
{ // BGR
  p_zdroj[0] -= p_sub[0];
  p_zdroj[1] -= p_sub[1];
  p_zdroj[2] -= p_sub[2];  
  
  if(p_zdroj[0] < 0.0f) p_zdroj[0] = 0.0f;
  if(p_zdroj[1] < 0.0f) p_zdroj[1] = 0.0f;
  if(p_zdroj[2] < 0.0f) p_zdroj[2] = 0.0f;  
}

inline void sub_color_pa(float *p_zdroj, float *p_sub)
{ // BGR
  p_zdroj[0] -= p_sub[0];
  p_zdroj[1] -= p_sub[1];
  p_zdroj[2] -= p_sub[2];  
  p_zdroj[3] -= p_sub[3];  
  
  if(p_zdroj[0] < 0.0f) p_zdroj[0] = 0.0f;
  if(p_zdroj[1] < 0.0f) p_zdroj[1] = 0.0f;
  if(p_zdroj[2] < 0.0f) p_zdroj[2] = 0.0f;  
  if(p_zdroj[3] < 0.0f) p_zdroj[3] = 0.0f;  
}

inline void add_mult_color_dword(float *p_baze, dword barva, float scale)
{ // BGR  
  float d[4];

  rgba_float(barva,d);

  p_baze[0] += d[0]*scale;
  p_baze[1] += d[1]*scale;
  p_baze[2] += d[2]*scale;  
  p_baze[3] += d[3]*scale;  
  
  if(p_baze[0] > 1.0f) p_baze[0] = 1.0f;
  if(p_baze[1] > 1.0f) p_baze[1] = 1.0f;
  if(p_baze[2] > 1.0f) p_baze[2] = 1.0f;
  if(p_baze[3] > 1.0f) p_baze[3] = 1.0f;
}

inline void mult_mult_color_dword(float *p_baze, dword barva, float scale)
{ // BGR  
  float d[4];

  rgba_float(barva,d);

  p_baze[0] *= d[0]*scale;
  p_baze[1] *= d[1]*scale;
  p_baze[2] *= d[2]*scale;
  p_baze[3] *= d[3]*scale;
  
  if(p_baze[0] > 1.0f) p_baze[0] = 1.0f;
  if(p_baze[1] > 1.0f) p_baze[1] = 1.0f;
  if(p_baze[2] > 1.0f) p_baze[2] = 1.0f;
  if(p_baze[3] > 1.0f) p_baze[3] = 1.0f;
}

inline void add_mult_color(dword *p_baze, dword add, float scale)
{ // BGR
  float rs,gs,bs,as;
  float rd,gd,bd,ad;

  rd = (float)((*p_baze)&0xff);
  gd = (float)(((*p_baze)&0xff00)>>8);
  bd = (float)(((*p_baze)&0xff0000)>>16);
  ad = (float)(((*p_baze)&0xff000000)>>24);

  rs = (float) (add&0xff);
  gs = (float)((add&0xff00)>>8);
  bs = (float)((add&0xff0000)>>16);
  as = (float)((add&0xff000000)>>24);

  rd += rs*scale;
  gd += gs*scale;
  bd += bs*scale;
  ad += as*scale;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;
  if(ad > MAX_BYTE) ad = MAX_BYTE;

  *p_baze  = (byte)rd;
  *p_baze |= ((byte)gd<<8);
  *p_baze |= ((byte)bd<<16);
  *p_baze |= ((byte)ad<<24);
}

inline void add_mult_color_f(float *p_baze, float *p_add, float scale)
{ // BGR
  p_baze[0] += p_add[0]*scale;
  p_baze[1] += p_add[1]*scale;
  p_baze[2] += p_add[2]*scale;
  p_baze[3] += p_add[3]*scale;
  
  if(p_baze[0] > 1.0f) p_baze[0] = 1.0f;
  if(p_baze[1] > 1.0f) p_baze[1] = 1.0f;
  if(p_baze[2] > 1.0f) p_baze[2] = 1.0f;
  if(p_baze[3] > 1.0f) p_baze[3] = 1.0f;
}

inline void sub_mult_color(dword *p_baze, dword *p_add, float scale)
{ // BGR
  float rs,gs,bs,as;
  float rd,gd,bd,ad;

  rd = (float)((*p_baze)&0xff);
  gd = (float)(((*p_baze)&0xff00)>>8);
  bd = (float)(((*p_baze)&0xff0000)>>16);
  ad = (float)(((*p_baze)&0xff000000)>>24);

  rs = (float)((*p_add)&0xff);
  gs = (float)(((*p_add)&0xff00)>>8);
  bs = (float)(((*p_add)&0xff0000)>>16);
  as = (float)(((*p_add)&0xff000000)>>24);

  rd -= rs*scale;
  gd -= gs*scale;
  bd -= bs*scale;
  ad -= as*scale;
  
  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;
  if(ad < 0) ad = 0;

  *p_baze  = (byte)rd;
  *p_baze |= ((byte)gd<<8);
  *p_baze |= ((byte)bd<<16);
  *p_baze |= ((byte)ad<<24);
}

inline void sub_mult_color_f(float *p_baze, float *p_add, float scale)
{ // BGR
  p_baze[0] -= p_add[0]*scale;
  p_baze[1] -= p_add[1]*scale;
  p_baze[2] -= p_add[2]*scale;
  p_baze[3] -= p_add[3]*scale;
  
  if(p_baze[0] < 0.0f) p_baze[0] = 0.0f;
  if(p_baze[1] < 0.0f) p_baze[1] = 0.0f;
  if(p_baze[2] < 0.0f) p_baze[2] = 0.0f;
  if(p_baze[3] < 0.0f) p_baze[3] = 0.0f;
}

inline void add_color_rgb_dword(dword *p_baze, dword r, dword g, dword b)
{ // BGR
  dword rd,gd,bd;

  rd = ((*p_baze)&0xff);
  gd = ((*p_baze)&0xff00)>>8;
  bd = ((*p_baze)&0xff0000)>>16;

  rd += r;
  gd += g;
  bd += b;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;

  *p_baze  = rd;
  *p_baze |= (gd<<8);
  *p_baze |= (bd<<16);
}

inline void add_color_rgb(float *p_baze, dword r, dword g, dword b)
{ // BGR
  p_baze[0] += r/MAX_BYTE;
  p_baze[1] += g/MAX_BYTE;
  p_baze[2] += b/MAX_BYTE;
  
  if(p_baze[0] > 1.0f) p_baze[0] = 1.0f;
  if(p_baze[1] > 1.0f) p_baze[1] = 1.0f;
  if(p_baze[2] > 1.0f) p_baze[2] = 1.0f;
}

inline void add_color_rgb_byte(byte *p_baze, byte ar, byte ag, byte ab)
{ // BGR
  int r,g,b;

  r = p_baze[0];
  g = p_baze[1];
  b = p_baze[2];
  
  r += ar;
  g += ag;
  b += ab;

  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  p_baze[0] = (byte)r;
  p_baze[1] = (byte)g;
  p_baze[2] = (byte)b;
}

inline void sub_color_rgb(float *p_baze, dword r, dword g, dword b)
{ // BGR
  p_baze[0] -= r/MAX_BYTE;
  p_baze[1] -= g/MAX_BYTE;
  p_baze[2] -= b/MAX_BYTE;
  
  if(p_baze[0] < 0.0f) p_baze[0] = 0.0f;
  if(p_baze[1] < 0.0f) p_baze[1] = 0.0f;
  if(p_baze[2] < 0.0f) p_baze[2] = 01.0f;
}

inline void sub_color_rgb_dword(dword *p_baze, dword r, dword g, dword b)
{ // BGR
  int rd,gd,bd;

  rd = ((*p_baze)&0xff);
  gd = ((*p_baze)&0xff00)>>8;
  bd = ((*p_baze)&0xff0000)>>16;

  rd -= r;
  gd -= g;
  bd -= b;
  
  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  *p_baze  = rd;
  *p_baze |= (gd<<8);
  *p_baze |= (bd<<16);
}

inline void add_mult_color_rgba(dword *p_baze, int r, int g, int b, float scale)
{ // BGR
  float rd,gd,bd,ad;

  rd = (float)((*p_baze)&0xff);
  gd = (float)(((*p_baze)&0xff00)>>8);
  bd = (float)(((*p_baze)&0xff0000)>>16);
  ad = (float)(((*p_baze)&0xff000000)>>24);

  rd += r*scale;
  gd += g*scale;
  bd += b*scale;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;

  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  *p_baze  =  (byte)rd;
  *p_baze |= ((byte)gd<<8);
  *p_baze |= ((byte)bd<<16);
  *p_baze |= ((byte)ad<<24);
}

inline void add_mult_color_rgb_int(byte *p_baze, int r, int g, int b, float scale)
{ // BGR
  float rd,gd,bd;

  rd = (float)p_baze[0];
  gd = (float)p_baze[1];
  bd = (float)p_baze[2];

  rd += r*scale;
  gd += g*scale;
  bd += b*scale;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;

  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  p_baze[0] = (byte)rd;
  p_baze[1] = (byte)gd;
  p_baze[2] = (byte)bd;
}

inline void sub_mult_color_rgb(dword *p_baze, dword r, dword g, dword b, float scale)
{ // BGR
  float rd,gd,bd;

  rd = (float)((*p_baze)&0xff);
  gd = (float)(((*p_baze)&0xff00)>>8);
  bd = (float)(((*p_baze)&0xff0000)>>16);

  rd -= r*scale;
  gd -= g*scale;
  bd -= b*scale;
  
  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  *p_baze  = (byte)rd;
  *p_baze |= ((byte)gd<<8);
  *p_baze |= ((byte)bd<<16);
}


inline void get_add_color_rgb(dword baze, dword *p_r, dword *p_g, dword *p_b)
{ // BGR
  *p_r += ((baze)&0xff);
  *p_g += ((baze)&0xff00)>>8;
  *p_b += ((baze)&0xff0000)>>16;
}

inline void mult_color_rgb_max(dword *p_baze, dword r, dword g, dword b, float scale)
{ // BGR
  r = (dword)(int)(r*scale);
  g = (dword)(int)(g*scale);
  b = (dword)(int)(b*scale);
  
  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  *p_baze  =  (byte)r;
  *p_baze |= ((byte)g<<8);
  *p_baze |= ((byte)b<<16);
}


#endif
