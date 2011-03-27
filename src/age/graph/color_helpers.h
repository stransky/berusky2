/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * AGE (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
  Color helpers
*/

#ifndef __COLOR_HELPERS_H__
#define __COLOR_HELPERS_H__

inline tcolor rb_switch(tcolor color)
{
  byte r = (color&0xff);
  byte b = (color&0xff0000)>>16;
  
  tcolor tmp = color&0xff00ff00;
  
  tmp |= (r<<16);
  tmp |= b;
  
  return(tmp);
}

inline tcolor int_rgb(int r, int g, int b)
{
  tcolor color = 0xff000000;

  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  color |=  r;
  color |=  g<<8;
  color |=  b<<16;

  return(color);
}

inline tcolor byte_rgb(byte r, byte g, byte b)
{
  tcolor color = 0xff000000;
  color |=  r;
  color |=  g<<8;
  color |=  b<<16;
  return(color);
}

inline tcolor byte_rgba(byte r, byte g, byte b, byte a)
{
  tcolor color;  
  color  =  r;
  color |=  g<<8;
  color |=  b<<16;
  color |=  a<<24;
  return(color);
}

inline tcolor rgb_byte(tcolor color, byte *p_r, byte *p_g, byte *p_b)
{
  *p_r = (byte)((color)&0xff);
  *p_g = (byte)((color>>8)&0xff);
  *p_b = (byte)((color>>16)&0xff);
  return(color);
}

inline tcolor rgba_byte(tcolor color, byte *p_r, byte *p_g, byte *p_b, byte *p_a)
{    
  *p_r = (byte)((color)&0xff);
  *p_g = (byte)((color>>8)&0xff);
  *p_b = (byte)((color>>16)&0xff);
  *p_a = (byte)((color>>24)&0xff);
  return(color);
}

inline tcolor rgb_int(tcolor color, int *p_r, int *p_g, int *p_b)
{    
  *p_r = (byte)((color)&0xff);
  *p_g = (byte)((color>>8)&0xff);
  *p_b = (byte)((color>>16)&0xff);
  return(color);
}

inline tcolor byte_a(tcolor color, int a)
{  
  return(color|(a<<24));
}

inline byte a_byte(tcolor color)
{  
  return((byte)(color>>24));
}

inline tcolor float_rgba(float *p_color)
{
  tcolor color;  
  color  =   (tcolor)(p_color[0]*255.0f);
  color |=  ((tcolor)(p_color[1]*255.0f)<<8);
  color |=  ((tcolor)(p_color[2]*255.0f)<<16);
  color |=  ((tcolor)(p_color[3]*255.0f)<<24);
  return(color);
}

inline tcolor float_rgba(float r, float g, float b, float a)
{
  tcolor color;  
  color  =   (tcolor)(r*255.0f);
  color |=  ((tcolor)(g*255.0f)<<8);
  color |=  ((tcolor)(b*255.0f)<<16);
  color |=  ((tcolor)(a*255.0f)<<24);
  return(color);
}

inline tcolor float_rgb(float *p_color)
{
  tcolor color;  
  color  =   (tcolor)(p_color[0]*255.0f);
  color |=  ((tcolor)(p_color[1]*255.0f)<<8);
  color |=  ((tcolor)(p_color[2]*255.0f)<<16);
  return(color);
}

inline tcolor float_rgb(float r, float g, float b)
{
  tcolor color;  
  color  =   (tcolor)(r*255.0f);
  color |=  ((tcolor)(g*255.0f)<<8);
  color |=  ((tcolor)(b*255.0f)<<16);
  return(color);
}

inline tcolor float_a(float alfa)
{
  return((tcolor)(alfa*255.0f));
}

inline float * rgba_float(tcolor color, float *p_color)
{  
  p_color[0] = ((float)(color&0xff))/255.0f;
  p_color[1] = ((float)((color&0xff00)>>8))/255.0f;
  p_color[2] = ((float)((color&0xff0000)>>16))/255.0f;
  p_color[3] = ((float)((color&0xff000000)>>24))/255.0f;
  return(p_color);
}

inline void rgba_float(tcolor color, float *p_r, float *p_g, float *p_b, float *p_a)
{  
  *p_r = ((float)(color&0xff))/255.0f;
  *p_g = ((float)((color&0xff00)>>8))/255.0f;
  *p_b = ((float)((color&0xff0000)>>16))/255.0f;
  *p_a = ((float)((color&0xff000000)>>24))/255.0f;
}

inline float * rgb_float(tcolor color, float *p_color)
{  
  p_color[0] = ((float)(color&0xff))/255.0f;
  p_color[1] = ((float)((color&0xff00)>>8))/255.0f;
  p_color[2] = ((float)((color&0xff0000)>>16))/255.0f;
  return(p_color);
}

inline void rgb_float(tcolor color, float *p_r, float *p_g, float *p_b)
{  
  *p_r = ((float)(color&0xff))/255.0f;
  *p_g = ((float)((color&0xff00)>>8))/255.0f;
  *p_b = ((float)((color&0xff0000)>>16))/255.0f;
}

inline float a_float(tcolor color)
{  
  return(((float)(color&0xff))/255.0f);
}

inline float rgb_intensity(tcolor color)
{  
  return((((float)(color&0xff))/255.0f) +
         (((float)((color&0xff00)>>8))/255.0f) +
         (((float)((color&0xff0000)>>16))/255.0f)) / 3.0f;
}

// Prace s colorma
inline void add_color(tcolor *p_base, tcolor add)
{ // BGR
  tcolor rs,gs,bs,as;
  tcolor rd,gd,bd,ad;

  rd = ((*p_base)&0xff);
  gd = ((*p_base)&0xff00)>>8;
  bd = ((*p_base)&0xff0000)>>16;
  ad = ((*p_base)&0xff000000)>>24;

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

  *p_base  = rd;
  *p_base |= (gd<<8);
  *p_base |= (bd<<16);
  *p_base |= (ad<<24);
}

inline void add_color_f(float *p_base, float r, float g, float b)
{ // BGR
  p_base[0] += r;
  p_base[1] += g;
  p_base[2] += b;  
   
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;   
}

inline void add_color_fa(float *p_base, float r, float g, float b, float a)
{ // BGR
  p_base[0] += r;
  p_base[1] += g;
  p_base[2] += b;  
  p_base[3] += a;  
   
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;   
  if(p_base[3] > 1.0f) p_base[3] = 1.0f;
}

// r,g,b,a
inline void add_color_p(float *p_base, float *p_add)
{ // BGR
  p_base[0] += p_add[0];
  p_base[1] += p_add[1];
  p_base[2] += p_add[2];  
  
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;  
}

inline void add_color_pa(float *p_base, float *p_add)
{ // BGR
  p_base[0] += p_add[0];
  p_base[1] += p_add[1];
  p_base[2] += p_add[2];  
  p_base[3] += p_add[3];  
  
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;  
  if(p_base[3] > 1.0f) p_base[3] = 1.0f;
}

inline void sub_color(tcolor *p_base, tcolor add)
{ // BGR
  int rs,gs,bs,as;
  int rd,gd,bd,ad;

  rd = ((*p_base)&0xff);
  gd = ((*p_base)&0xff00)>>8;
  bd = ((*p_base)&0xff0000)>>16;
  ad = ((*p_base)&0xff000000)>>24;

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

  *p_base  = rd;
  *p_base |= (gd<<8);
  *p_base |= (bd<<16);
  *p_base |= (ad<<24);
}

inline void sub_color_f(float *p_base, float r, float g, float b)
{ // BGR
  p_base[0] -= r;
  p_base[1] -= g;
  p_base[2] -= b;
  
  if(p_base[0] < 0.0f) p_base[0] = 0.0f;
  if(p_base[1] < 0.0f) p_base[1] = 0.0f;
  if(p_base[2] < 0.0f) p_base[2] = 0.0f;  
}

inline void sub_color_fa(float *p_base, float r, float g, float b, float a)
{ // BGR
  p_base[0] -= r;
  p_base[1] -= g;
  p_base[2] -= b;
  p_base[3] -= a;
  
  if(p_base[0] < 0.0f) p_base[0] = 0.0f;
  if(p_base[1] < 0.0f) p_base[1] = 0.0f;
  if(p_base[2] < 0.0f) p_base[2] = 0.0f;  
  if(p_base[3] < 0.0f) p_base[3] = 0.0f;  
}

inline void sub_color_p(float *p_base, float *p_sub)
{ // BGR
  p_base[0] -= p_sub[0];
  p_base[1] -= p_sub[1];
  p_base[2] -= p_sub[2];  
  
  if(p_base[0] < 0.0f) p_base[0] = 0.0f;
  if(p_base[1] < 0.0f) p_base[1] = 0.0f;
  if(p_base[2] < 0.0f) p_base[2] = 0.0f;  
}

inline void sub_color_pa(float *p_base, float *p_sub)
{ // BGR
  p_base[0] -= p_sub[0];
  p_base[1] -= p_sub[1];
  p_base[2] -= p_sub[2];  
  p_base[3] -= p_sub[3];  
  
  if(p_base[0] < 0.0f) p_base[0] = 0.0f;
  if(p_base[1] < 0.0f) p_base[1] = 0.0f;
  if(p_base[2] < 0.0f) p_base[2] = 0.0f;  
  if(p_base[3] < 0.0f) p_base[3] = 0.0f;  
}

inline void add_mult_color_tcolor(float *p_base, tcolor color, float scale)
{ // BGR  
  float d[4];

  rgba_float(color,d);

  p_base[0] += d[0]*scale;
  p_base[1] += d[1]*scale;
  p_base[2] += d[2]*scale;  
  p_base[3] += d[3]*scale;  
  
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;
  if(p_base[3] > 1.0f) p_base[3] = 1.0f;
}

inline void mult_mult_color_tcolor(float *p_base, tcolor color, float scale)
{ // BGR  
  float d[4];

  rgba_float(color,d);

  p_base[0] *= d[0]*scale;
  p_base[1] *= d[1]*scale;
  p_base[2] *= d[2]*scale;
  p_base[3] *= d[3]*scale;
  
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;
  if(p_base[3] > 1.0f) p_base[3] = 1.0f;
}

inline void add_mult_color(tcolor *p_base, tcolor add, float scale)
{ // BGR
  float rs,gs,bs,as;
  float rd,gd,bd,ad;

  rd = (float)((*p_base)&0xff);
  gd = (float)(((*p_base)&0xff00)>>8);
  bd = (float)(((*p_base)&0xff0000)>>16);
  ad = (float)(((*p_base)&0xff000000)>>24);

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

  *p_base  =  (byte)rd;
  *p_base |= ((byte)gd<<8);
  *p_base |= ((byte)bd<<16);
  *p_base |= ((byte)ad<<24);
}

inline void add_mult_color_f(float *p_base, float *p_add, float scale)
{ // BGR
  p_base[0] += p_add[0]*scale;
  p_base[1] += p_add[1]*scale;
  p_base[2] += p_add[2]*scale;
  p_base[3] += p_add[3]*scale;
  
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;
  if(p_base[3] > 1.0f) p_base[3] = 1.0f;
}

inline void sub_mult_color(tcolor *p_base, tcolor *p_add, float scale)
{ // BGR
  float rs,gs,bs,as;
  float rd,gd,bd,ad;

  rd = (float)((*p_base)&0xff);
  gd = (float)(((*p_base)&0xff00)>>8);
  bd = (float)(((*p_base)&0xff0000)>>16);
  ad = (float)(((*p_base)&0xff000000)>>24);

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

  *p_base  =  (byte)rd;
  *p_base |= ((byte)gd<<8);
  *p_base |= ((byte)bd<<16);
  *p_base |= ((byte)ad<<24);
}

inline void sub_mult_color_f(float *p_base, float *p_add, float scale)
{ // BGR
  p_base[0] -= p_add[0]*scale;
  p_base[1] -= p_add[1]*scale;
  p_base[2] -= p_add[2]*scale;
  p_base[3] -= p_add[3]*scale;
  
  if(p_base[0] < 0.0f) p_base[0] = 0.0f;
  if(p_base[1] < 0.0f) p_base[1] = 0.0f;
  if(p_base[2] < 0.0f) p_base[2] = 0.0f;
  if(p_base[3] < 0.0f) p_base[3] = 0.0f;
}

inline void add_color_rgb_tcolor(tcolor *p_base, dword r, dword g, dword b)
{ // BGR
  tcolor rd,gd,bd;

  rd = ((*p_base)&0xff);
  gd = ((*p_base)&0xff00)>>8;
  bd = ((*p_base)&0xff0000)>>16;

  rd += r;
  gd += g;
  bd += b;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;

  *p_base  = rd;
  *p_base |= (gd<<8);
  *p_base |= (bd<<16);
}

inline void add_color_rgb(float *p_base, dword r, dword g, dword b)
{ // BGR
  p_base[0] += r/MAX_BYTE;
  p_base[1] += g/MAX_BYTE;
  p_base[2] += b/MAX_BYTE;
  
  if(p_base[0] > 1.0f) p_base[0] = 1.0f;
  if(p_base[1] > 1.0f) p_base[1] = 1.0f;
  if(p_base[2] > 1.0f) p_base[2] = 1.0f;
}

inline void add_color_rgb_byte(byte *p_base, byte ar, byte ag, byte ab)
{ // BGR
  int r,g,b;

  r = p_base[0];
  g = p_base[1];
  b = p_base[2];
  
  r += ar;
  g += ag;
  b += ab;

  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  p_base[0] = (byte)r;
  p_base[1] = (byte)g;
  p_base[2] = (byte)b;
}

inline void sub_color_rgb(float *p_base, dword r, dword g, dword b)
{ // BGR
  p_base[0] -= r/MAX_BYTE;
  p_base[1] -= g/MAX_BYTE;
  p_base[2] -= b/MAX_BYTE;
  
  if(p_base[0] < 0.0f) p_base[0] = 0.0f;
  if(p_base[1] < 0.0f) p_base[1] = 0.0f;
  if(p_base[2] < 0.0f) p_base[2] = 0.0f;
}

inline void sub_color_rgb_tcolor(tcolor *p_base, dword r, dword g, dword b)
{ // BGR
  int rd,gd,bd;

  rd = ((*p_base)&0xff);
  gd = ((*p_base)&0xff00)>>8;
  bd = ((*p_base)&0xff0000)>>16;

  rd -= r;
  gd -= g;
  bd -= b;
  
  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  *p_base  = rd;
  *p_base |= (gd<<8);
  *p_base |= (bd<<16);
}

inline void add_mult_color_rgba(tcolor *p_base, int r, int g, int b, float scale)
{ // BGR
  float rd,gd,bd,ad;

  rd = (float)((*p_base)&0xff);
  gd = (float)(((*p_base)&0xff00)>>8);
  bd = (float)(((*p_base)&0xff0000)>>16);
  ad = (float)(((*p_base)&0xff000000)>>24);

  rd += r*scale;
  gd += g*scale;
  bd += b*scale;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;

  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  *p_base  =  (byte)rd;
  *p_base |= ((byte)gd<<8);
  *p_base |= ((byte)bd<<16);
  *p_base |= ((byte)ad<<24);
}

inline void add_mult_color_rgb_int(byte *p_base, int r, int g, int b, float scale)
{ // BGR
  float rd,gd,bd;

  rd = (float)p_base[0];
  gd = (float)p_base[1];
  bd = (float)p_base[2];

  rd += r*scale;
  gd += g*scale;
  bd += b*scale;
  
  if(rd > MAX_BYTE) rd = MAX_BYTE;
  if(gd > MAX_BYTE) gd = MAX_BYTE;
  if(bd > MAX_BYTE) bd = MAX_BYTE;

  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  p_base[0] = (byte)rd;
  p_base[1] = (byte)gd;
  p_base[2] = (byte)bd;
}

inline void sub_mult_color_rgb(dword *p_base, dword r, dword g, dword b, float scale)
{ // BGR
  float rd,gd,bd;

  rd = (float)((*p_base)&0xff);
  gd = (float)(((*p_base)&0xff00)>>8);
  bd = (float)(((*p_base)&0xff0000)>>16);

  rd -= r*scale;
  gd -= g*scale;
  bd -= b*scale;
  
  if(rd < 0) rd = 0;
  if(gd < 0) gd = 0;
  if(bd < 0) bd = 0;

  *p_base  = (byte)rd;
  *p_base |= ((byte)gd<<8);
  *p_base |= ((byte)bd<<16);
}

inline void get_add_color_rgb(dword base, dword *p_r, dword *p_g, dword *p_b)
{ // BGR
  *p_r += ((base)&0xff);
  *p_g += ((base)&0xff00)>>8;
  *p_b += ((base)&0xff0000)>>16;
}

inline void mult_color_rgb_max(dword *p_base, dword r, dword g, dword b, float scale)
{ // BGR
  r = (dword)(r*scale);
  g = (dword)(g*scale);
  b = (dword)(b*scale);
  
  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  *p_base  =  (byte)r;
  *p_base |= ((byte)g<<8);
  *p_base |= ((byte)b<<16);
}

inline float color_intensity(float r, float g, float b)
{
  return(0.299f*r + 0.587f*g + 0.114f*b);
}

inline float color_intensity(int r, int g, int b)
{
  return(color_intensity(r/255.f, g/255.f, b/255.f));
}

#endif // __COLOR_HELPERS_H__
