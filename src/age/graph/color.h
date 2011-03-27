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
  color classes
*/

#ifndef __COLOR_H__
#define __COLOR_H__

#include "color_operators.h"

class rgbf;
class rgbb;
class rgbaf;
class rgbab;

typedef class rgbf RGBF;
typedef class rgbb RGBB;
typedef class rgbaf RGBAF;
typedef class rgbab RGBAB;

#define COLOR_TO_FLOAT(color) ((color)/MAX_BYTE_F)
#define COLOR_TO_BYTE(color)  ((byte)((color)*MAX_BYTE_F))

// Simple clases for parser
#ifdef __cplusplus
typedef struct rgbaf_simple {

  float r,g,b,a;
  
} RGBAF_SIMPLE;
#endif

typedef class rgbf {

public:
  
  float r,g,b;

public:
  
  OP_UNARY_RGB(class rgbf, float);
  
public:

  float alpha_get(void)
  {
    return(0.0f);
  }
  
public:

  rgbf(void) {};
  
public:
    
  rgbf(byte color)
  {
    r = g = b = color/MAX_BYTE_F;
  }

  rgbf(int color)
  {
    r = g = b = (byte)(color/MAX_BYTE_F);
  }
  
  rgbf(float color)
  {
    r = g = b = color;
  }
  
  rgbf(tcolor color)
  {
    rgb_float(color,&r,&g,&b);
  }
  
public:
  /*
  rgbf(byte r_, byte g_, byte b_)
  {
    r = r_/MAX_BYTE_F;
    g = g_/MAX_BYTE_F;
    b = b_/MAX_BYTE_F;
  }
  
  rgbf(int r_, int g_, int b_)
  {
    r = r_/MAX_BYTE_F;
    g = g_/MAX_BYTE_F;
    b = b_/MAX_BYTE_F;
  }
  */
  rgbf(float r_,float g_,float b_)
  {
    r = r_, g = g_, b = b_;
  }

public:
  
  rgbf(RGBB src);
  rgbf(RGBB *src);
  
public:

  operator tcolor(void)
  {    
    norm_color();
    return(float_rgb(r,g,b));
  }  

public:
  
  void norm_up(void)
  {
    #define MAX_UP 1.0f
    if(r > MAX_UP) r = MAX_UP;
    if(g > MAX_UP) g = MAX_UP;
    if(b > MAX_UP) b = MAX_UP;    
  }
  
  void norm_down(void)
  {
    #define MAX_DOWN 0.0f
    if(r < MAX_DOWN) r = MAX_DOWN;
    if(g < MAX_DOWN) g = MAX_DOWN;
    if(b < MAX_DOWN) b = MAX_DOWN;    
  }
  
  void norm_color(void)
  {
    norm_up();
    norm_down();
  }
  
} RGBF;

OP_OPERATION(RGBF, float);


typedef class rgbb {

public:

  byte r,g,b;

public:

  OP_UNARY_RGB(class rgbb, byte);  
  
public:

  float alpha_get(void)
  {
    return(0.0f);
  }
  
public:

  rgbb(void) {};
    
public:
  
  rgbb(byte color)
  {
    r = g = b = color;
  }

  rgbb(int color)
  {
    r = g = b = (byte)color;
  }

  rgbb(float color)
  {
    r = g = b = (byte)(color*MAX_BYTE_F);
  }
  
  rgbb(tcolor color)
  {
    rgb_byte(color,&r,&g,&b);
  }  
    
public:
  /*
  rgbb(byte r_, byte g_, byte b_)
  {
    r = r_;
    g = g_;
    b = b_;
  }
  
  rgbb(int r_, int g_, int b_)
  {
    r = (byte)r_;
    g = (byte)g_;
    b = (byte)b_;
  }
  */
  rgbb(float r_, float g_, float b_)
  {
    r = (byte)(r_*MAX_BYTE_F);
    g = (byte)(g_*MAX_BYTE_F);
    b = (byte)(b_*MAX_BYTE_F);
  }
  
public:
  
  rgbb(RGBF src)
  {
    r = (byte)(src.r*MAX_BYTE_F);
    g = (byte)(src.g*MAX_BYTE_F);
    b = (byte)(src.b*MAX_BYTE_F);
  }  
  rgbb(RGBF *src)
  {
    r = (byte)(src->r*MAX_BYTE_F);
    g = (byte)(src->g*MAX_BYTE_F);
    b = (byte)(src->b*MAX_BYTE_F);
  }  
  
public:
  
  operator tcolor(void)
  {    
    return(byte_rgb(r,g,b));
  }
  
  void norm_color(void)
  {
  }
  
} RGBB;

typedef class rgbaf {
  
public:
  
  float r,g,b,a;

public:
  
  OP_UNARY_RGBA(class rgbaf, float);
    
public:

  float alpha_get(void)
  {
    return(a);
  }
  
public:

  rgbaf(void) {};
  
public:
    
  rgbaf(byte color)
  {
    r = g = b = a = color/MAX_BYTE_F;
  }

  rgbaf(int color)
  {
    r = g = b = a = (byte)(color/MAX_BYTE_F);
  }

  rgbaf(float color)
  {
    r = g = b = a = color;
  }
  
  rgbaf(tcolor color)
  {
    rgba_float(color,&r,&g,&b,&a);
  }
  
  rgbaf(RGBAF_SIMPLE color)
  {
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
  }
  
  operator RGBAF_SIMPLE(void)
  {    
    RGBAF_SIMPLE tmp;
    tmp.r = r;
    tmp.g = g;
    tmp.b = b;
    tmp.a = a;
    return(tmp);
  }
  
public:
/*    
  rgbaf(byte r_, byte g_, byte b_, byte a_)
  {
    r = r_/MAX_BYTE_F;
    g = g_/MAX_BYTE_F;
    b = b_/MAX_BYTE_F;
    a = a_/MAX_BYTE_F;
  }

  rgbaf(int r_, int g_, int b_, int a_)
  {
    r = r_/MAX_BYTE_F;
    g = g_/MAX_BYTE_F;
    b = b_/MAX_BYTE_F;
    a = a_/MAX_BYTE_F;
  }
*/  
  rgbaf(float r_,float g_,float b_,float a_)
  {
    r = r_, g = g_, b = b_, a = a_;
  }

public:
  
  rgbaf(RGBAB src);
  rgbaf(RGBAB *src);
  
public:

  operator tcolor(void)
  {    
    norm_color();
    return(float_rgba(r,g,b,a));
  }
   
public:
  
  float intensity(void)
  {  
    return((r+g+b)/3.0f);
  }
  
public:
  
  void norm_up(void)
  {
    #define MAX_UP 1.0f
    if(r > MAX_UP) r = MAX_UP;
    if(g > MAX_UP) g = MAX_UP;
    if(b > MAX_UP) b = MAX_UP;
    if(a > MAX_UP) a = MAX_UP;
  }
  
  void norm_down(void)
  {
    #define MAX_DOWN 0.0f
    if(r < MAX_DOWN) r = MAX_DOWN;
    if(g < MAX_DOWN) g = MAX_DOWN;
    if(b < MAX_DOWN) b = MAX_DOWN;  
    if(a < MAX_DOWN) a = MAX_DOWN;
  }

  void norm_color(void)
  {
    norm_up();
    norm_down();
  }
  
} RGBAF;

OP_OPERATION(RGBAF, float);

typedef class rgbab {

public:

  byte r,g,b,a;

public:
  
  OP_UNARY_RGBA(class rgbab, byte);
    
public:

  float alpha_get(void)
  {
    return(COLOR_TO_FLOAT(a));
  }
  
public:

  rgbab(void) {};
    
public:
  
  rgbab(byte color)
  {
    r = g = b = a = color;
  }

  rgbab(int color)
  {
    r = g = b = a = (byte)color;
  }

  rgbab(float color)
  {
    r = g = b = a = (byte)(color*MAX_BYTE_F);
  }
  
  rgbab(tcolor color)
  {
    rgba_byte(color,&r,&g,&b,&a);
  }  
    
public:
  
  rgbab(byte r_, byte g_, byte b_, byte a_)
  {
    r = r_;
    g = g_;
    b = b_;
    a = a_;
  }
    
  rgbab(int r_, int g_, int b_, int a_)
  {
    r = (byte)r_;
    g = (byte)g_;
    b = (byte)b_;
    a = (byte)a_;
  }
  
  rgbab(float r_, float g_, float b_, float a_)
  {
    r = (byte)(r_*MAX_BYTE_F);
    g = (byte)(g_*MAX_BYTE_F);
    b = (byte)(b_*MAX_BYTE_F);
    a = (byte)(a_*MAX_BYTE_F);
  }
  
public:
  
  rgbab(RGBAF src)
  {
    r = (byte)(src.r*MAX_BYTE_F);
    g = (byte)(src.g*MAX_BYTE_F);
    b = (byte)(src.b*MAX_BYTE_F);
    a = (byte)(src.a*MAX_BYTE_F);
  }
  
  rgbab(RGBAF *src)
  {
    r = (byte)(src->r*MAX_BYTE_F);
    g = (byte)(src->g*MAX_BYTE_F);
    b = (byte)(src->b*MAX_BYTE_F);
    a = (byte)(src->a*MAX_BYTE_F);
  }  
  
public:
  
  operator tcolor(void)
  {    
    return(byte_rgba(r,g,b,a));
  }
  
  void norm_color(void)
  {
  }
  
} RGBAB;

/*
 *  HSVF color model
 */
typedef class hsvf {
  
public:
  
  float h,  // <0,1> - color
        s,  // <0,1> - saturation
        v;  // <0,1> - value

public:  
  
  OP_UNARY_HSV(class hsvf, float);
    
public:
  
  void norm_color(void)
  {
    // TODO
  }

public:

  hsvf(void) {};

} HSVF;

#endif //__COLOR_H__
