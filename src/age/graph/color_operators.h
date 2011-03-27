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

#ifndef __COLOR_OPERATORS_H__
#define __COLOR_OPERATORS_H__

/*
 * Unary operators
 */
#define OP_UNARY_RGB_1(type,type_scalar)  bool operator<=(type src) { return(r <= src.r && g <= src.g && b <= src.b); }
#define OP_UNARY_RGB_2(type,type_scalar)  bool operator>=(type src) { return(r >= src.r && g >= src.g && b >= src.b); }
#define OP_UNARY_RGB_3(type,type_scalar)  bool operator<(type src)  { return(r <  src.r && g <  src.g && b <  src.b); }
#define OP_UNARY_RGB_4(type,type_scalar)  bool operator>(type src)  { return(r >  src.r && g >  src.g && b >  src.b); }
#define OP_UNARY_RGB_5(type,type_scalar)  bool operator==(type src) { return(r == src.r && g == src.g && b == src.b); }
#define OP_UNARY_RGB_6(type,type_scalar)  bool operator!=(type src) { return(r != src.r || g != src.g || b != src.b); }

#define OP_UNARY_RGB_7(type,type_scalar)  void operator+=(type src) { r += src.r; g += src.g; b += src.b; norm_color(); }
#define OP_UNARY_RGB_8(type,type_scalar)  void operator-=(type src) { r -= src.r; g -= src.g; b -= src.b; norm_color(); }
#define OP_UNARY_RGB_9(type,type_scalar)  void operator*=(type src) { r *= src.r; g *= src.g; b *= src.b; norm_color(); }
#define OP_UNARY_RGB_10(type,type_scalar) void operator/=(type src) { r /= src.r; g /= src.g; b /= src.b; norm_color(); }

#define OP_UNARY_RGB_11(type,type_scalar) void operator+=(type_scalar src) { r += src; g += src; b += src; norm_color(); }
#define OP_UNARY_RGB_12(type,type_scalar) void operator-=(type_scalar src) { r -= src; g -= src; b -= src; norm_color(); }
#define OP_UNARY_RGB_13(type,type_scalar) void operator*=(type_scalar src) { r *= src; g *= src; b *= src; norm_color(); }
#define OP_UNARY_RGB_14(type,type_scalar) void operator/=(type_scalar src) { r /= src; g /= src; b /= src; norm_color(); }

#define OP_UNARY_RGB_15(type,type_scalar) type operator+(type src) { type tmp; tmp.r=r+src.r;tmp.g=g+src.g;tmp.b=b+src.b; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGB_16(type,type_scalar) type operator-(type src) { type tmp; tmp.r=r-src.r;tmp.g=g-src.g;tmp.b=b-src.b; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGB_17(type,type_scalar) type operator*(type src) { type tmp; tmp.r=r*src.r;tmp.g=g*src.g;tmp.b=b*src.b; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGB_18(type,type_scalar) type operator/(type src) { type tmp; tmp.r=r/src.r;tmp.g=g/src.g;tmp.b=b/src.b; tmp.norm_color(); return(tmp); }

#define OP_UNARY_RGB_19(type,type_scalar) type operator+(type_scalar src) { type tmp; tmp.r=r+src;tmp.g=g+src;tmp.b=b+src; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGB_20(type,type_scalar) type operator-(type_scalar src) { type tmp; tmp.r=r-src;tmp.g=g-src;tmp.b=b-src; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGB_21(type,type_scalar) type operator*(type_scalar src) { type tmp; tmp.r=r*src;tmp.g=g*src;tmp.b=b*src; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGB_22(type,type_scalar) type operator/(type_scalar src) { type tmp; tmp.r=r/src;tmp.g=g/src;tmp.b=b/src; tmp.norm_color(); return(tmp); }

#define OP_UNARY_RGB_23(type,type_scalar) void set(void) { r = g = b = 0; };
#define OP_UNARY_RGB_24(type,type_scalar) void set(type_scalar src) { r = g = b = src; };
#define OP_UNARY_RGB_25(type,type_scalar) void set(type_scalar r_, type_scalar g_, type_scalar b_) { r = r_; g = g_; b = b_; };

#define OP_UNARY_RGB_26(type,type_scalar) void norm_max(type_scalar src) { if(r > src) r = src;      if(g > src) g = src;      if(b > src) b = src; };
#define OP_UNARY_RGB_27(type,type_scalar) void norm_max(type src)        { if(r > src.r) r = src.r;  if(g > src.g) g = src.g;  if(b > src.b) b = src.b; };
#define OP_UNARY_RGB_28(type,type_scalar) void norm_min(type_scalar src) { if(r < src) r = src;      if(g < src) g = src;      if(b < src) b = src; };
#define OP_UNARY_RGB_29(type,type_scalar) void norm_min(type src)        { if(r < src.r) r = src.r;  if(g < src.g) g = src.g;  if(b < src.b) b = src.b; };

#define OP_UNARY_RGB_30(type,type_scalar) void norm_color(type_scalar src_max, type_scalar src_min)  { norm_max(src_max); norm_min(src_min); };
#define OP_UNARY_RGB_31(type,type_scalar) void norm_color(type        src_max, type        src_min)  { norm_max(src_max); norm_min(src_min); };

  
#define OP_UNARY_RGB(type,type_scalar) \
  OP_UNARY_RGB_1(type,type_scalar)     \
  OP_UNARY_RGB_2(type,type_scalar)     \
  OP_UNARY_RGB_3(type,type_scalar)     \
  OP_UNARY_RGB_4(type,type_scalar)     \
  OP_UNARY_RGB_5(type,type_scalar)     \
  OP_UNARY_RGB_6(type,type_scalar)     \
  OP_UNARY_RGB_7(type,type_scalar)     \
  OP_UNARY_RGB_8(type,type_scalar)     \
  OP_UNARY_RGB_9(type,type_scalar)     \
  OP_UNARY_RGB_10(type,type_scalar)    \
  OP_UNARY_RGB_11(type,type_scalar)    \
  OP_UNARY_RGB_12(type,type_scalar)    \
  OP_UNARY_RGB_13(type,type_scalar)    \
  OP_UNARY_RGB_14(type,type_scalar)    \
  OP_UNARY_RGB_15(type,type_scalar)    \
  OP_UNARY_RGB_16(type,type_scalar)    \
  OP_UNARY_RGB_17(type,type_scalar)    \
  OP_UNARY_RGB_18(type,type_scalar)    \
  OP_UNARY_RGB_19(type,type_scalar)    \
  OP_UNARY_RGB_20(type,type_scalar)    \
  OP_UNARY_RGB_21(type,type_scalar)    \
  OP_UNARY_RGB_22(type,type_scalar)    \
  OP_UNARY_RGB_23(type,type_scalar)    \
  OP_UNARY_RGB_24(type,type_scalar)    \
  OP_UNARY_RGB_25(type,type_scalar)    \
  OP_UNARY_RGB_26(type,type_scalar)    \
  OP_UNARY_RGB_27(type,type_scalar)    \
  OP_UNARY_RGB_28(type,type_scalar)    \
  OP_UNARY_RGB_29(type,type_scalar)    \
  OP_UNARY_RGB_30(type,type_scalar)    \
  OP_UNARY_RGB_31(type,type_scalar)    \

/*
 * Binary operators
 */
#define OP_BINARY_RGB_1(type,type_scalar)  bool operator<=(type src1, type src2) { return(src1.r <= src2.r && src1.g <= src2.g && src1.b <= src2.b); }
#define OP_BINARY_RGB_2(type,type_scalar)  bool operator>=(type src1, type src2) { return(src1.r >= src2.r && src1.g >= src2.g && src1.b >= src2.b); }
#define OP_BINARY_RGB_3(type,type_scalar)  bool operator< (type src1, type src2) { return(src1.r <  src2.r && src1.g <  src2.g && src1.b <  src2.b); }
#define OP_BINARY_RGB_4(type,type_scalar)  bool operator> (type src1, type src2) { return(src1.r >  src2.r && src1.g >  src2.g && src1.b >  src2.b); }
#define OP_BINARY_RGB_5(type,type_scalar)  bool operator==(type src1, type src2) { return(src1.r == src2.r && src1.g == src2.g && src1.b == src2.b); }
#define OP_BINARY_RGB_6(type,type_scalar)  bool operator!=(type src1, type src2) { return(src1.r != src2.r || src1.g != src2.g || src1.b != src2.b); }
#define OP_BINARY_RGB_7(type,type_scalar)  type operator+(type src1, type src2) { type tmp; tmp.r=src1.r+src2.r; tmp.g=src1.g+src2.g; tmp.b=src1.b+src2.b; tmp.norm_color(); return(tmp); }
#define OP_BINARY_RGB_8(type,type_scalar)  type operator-(type src1, type src2) { type tmp; tmp.r=src1.r-src2.r; tmp.g=src1.g-src2.g; tmp.b=src1.b-src2.b; tmp.norm_color(); return(tmp); }
#define OP_BINARY_RGB_9(type,type_scalar)  type operator*(type src1, type src2) { type tmp; tmp.r=src1.r*src2.r; tmp.g=src1.g*src2.g; tmp.b=src1.b*src2.b; tmp.norm_color(); return(tmp); }
#define OP_BINARY_RGB_10(type,type_scalar) type operator/(type src1, type src2) { type tmp; tmp.r=src1.r/src2.r; tmp.g=src1.g/src2.g; tmp.b=src1.b/src2.b; tmp.norm_color(); return(tmp); }
 
#define OP_BINARY_RGB(type,type_scalar) \
  OP_BINARY_RGB_1(type,type_scalar)     \
  OP_BINARY_RGB_2(type,type_scalar)     \
  OP_BINARY_RGB_3(type,type_scalar)     \
  OP_BINARY_RGB_4(type,type_scalar)     \
  OP_BINARY_RGB_5(type,type_scalar)     \
  OP_BINARY_RGB_6(type,type_scalar)     \
  OP_BINARY_RGB_7(type,type_scalar)     \
  OP_BINARY_RGB_8(type,type_scalar)     \
  OP_BINARY_RGB_9(type,type_scalar)     \
  OP_BINARY_RGB_10(type,type_scalar)    \

/*
 * Unary operators
 */
#define OP_UNARY_RGBA_1(type,type_scalar)  bool operator<=(type src) { return(r <= src.r && g <= src.g && b <= src.b && a <= src.a); }
#define OP_UNARY_RGBA_2(type,type_scalar)  bool operator>=(type src) { return(r >= src.r && g >= src.g && b >= src.b && a >= src.a); }
#define OP_UNARY_RGBA_3(type,type_scalar)  bool operator<(type src)  { return(r <  src.r && g <  src.g && b <  src.b && a <  src.a); }
#define OP_UNARY_RGBA_4(type,type_scalar)  bool operator>(type src)  { return(r >  src.r && g >  src.g && b >  src.b && a >  src.a); }
#define OP_UNARY_RGBA_5(type,type_scalar)  bool operator==(type src) { return(r == src.r && g == src.g && b == src.b && a == src.a); }
#define OP_UNARY_RGBA_6(type,type_scalar)  bool operator!=(type src) { return(r != src.r && g != src.g && b != src.b && a != src.a); }

#define OP_UNARY_RGBA_7(type,type_scalar)  void operator+=(type src) { r += src.r; g += src.g; b += src.b; a += src.a; norm_color(); }
#define OP_UNARY_RGBA_8(type,type_scalar)  void operator-=(type src) { r -= src.r; g -= src.g; b -= src.b; a -= src.a; norm_color(); }
#define OP_UNARY_RGBA_9(type,type_scalar)  void operator*=(type src) { r *= src.r; g *= src.g; b *= src.b; a *= src.a; norm_color(); }
#define OP_UNARY_RGBA_10(type,type_scalar) void operator/=(type src) { r /= src.r; g /= src.g; b /= src.b; a /= src.a; norm_color(); }

#define OP_UNARY_RGBA_11(type,type_scalar) void operator+=(type_scalar src) { r += src; g += src; b += src; a += src; norm_color(); }
#define OP_UNARY_RGBA_12(type,type_scalar) void operator-=(type_scalar src) { r -= src; g -= src; b -= src; a -= src; norm_color(); }
#define OP_UNARY_RGBA_13(type,type_scalar) void operator*=(type_scalar src) { r *= src; g *= src; b *= src; a *= src; norm_color(); }
#define OP_UNARY_RGBA_14(type,type_scalar) void operator/=(type_scalar src) { r /= src; g /= src; b /= src; a /= src; norm_color(); }

#define OP_UNARY_RGBA_15(type,type_scalar) type operator+(type src) { type tmp; tmp.r=r+src.r;tmp.g=g+src.g;tmp.b=b+src.b;tmp.a=a+src.a; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGBA_16(type,type_scalar) type operator-(type src) { type tmp; tmp.r=r-src.r;tmp.g=g-src.g;tmp.b=b-src.b;tmp.a=a-src.a; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGBA_17(type,type_scalar) type operator*(type src) { type tmp; tmp.r=r*src.r;tmp.g=g*src.g;tmp.b=b*src.b;tmp.a=a*src.a; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGBA_18(type,type_scalar) type operator/(type src) { type tmp; tmp.r=r/src.r;tmp.g=g/src.g;tmp.b=b/src.b;tmp.a=a/src.a; tmp.norm_color(); return(tmp); }

#define OP_UNARY_RGBA_19(type,type_scalar) type operator+(type_scalar src) { type tmp; tmp.r=r+src;tmp.g=g+src;tmp.b=b+src;tmp.a=a+src; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGBA_20(type,type_scalar) type operator-(type_scalar src) { type tmp; tmp.r=r-src;tmp.g=g-src;tmp.b=b-src;tmp.a=a-src; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGBA_21(type,type_scalar) type operator*(type_scalar src) { type tmp; tmp.r=r*src;tmp.g=g*src;tmp.b=b*src;tmp.a=a*src; tmp.norm_color(); return(tmp); }
#define OP_UNARY_RGBA_22(type,type_scalar) type operator/(type_scalar src) { type tmp; tmp.r=r/src;tmp.g=g/src;tmp.b=b/src;tmp.a=a/src; tmp.norm_color(); return(tmp); }
  
#define OP_UNARY_RGBA_23(type,type_scalar) void set(void) { r = g = b = a = 0; };
#define OP_UNARY_RGBA_24(type,type_scalar) void set(type_scalar src) { r = g = b = a = src; };
#define OP_UNARY_RGBA_25(type,type_scalar) void set(type_scalar r_, type_scalar g_, type_scalar b_, type_scalar a_) { r = r_; g = g_; b = b_; a = a_;};

#define OP_UNARY_RGBA_26(type,type_scalar) void norm_max(type_scalar src) { if(r > src) r = src;      if(g > src) g = src;      if(b > src) b = src;     if(a > src) a = src; };
#define OP_UNARY_RGBA_27(type,type_scalar) void norm_max(type src)        { if(r > src.r) r = src.r;  if(g > src.g) g = src.g;  if(b > src.b) b = src.b; if(a > src.a) a = src.a; };
#define OP_UNARY_RGBA_28(type,type_scalar) void norm_min(type_scalar src) { if(r < src) r = src;      if(g < src) g = src;      if(b < src) b = src;     if(a < src) a = src; };
#define OP_UNARY_RGBA_29(type,type_scalar) void norm_min(type src)        { if(r < src.r) r = src.r;  if(g < src.g) g = src.g;  if(b < src.b) b = src.b; if(a < src.a) a = src.a; };
  
#define OP_UNARY_RGBA_30(type,type_scalar) void norm_color(type_scalar src_max, type_scalar src_min)  { norm_max(src_max); norm_min(src_min); };
#define OP_UNARY_RGBA_31(type,type_scalar) void norm_color(type        src_max, type        src_min)  { norm_max(src_max); norm_min(src_min); };

#define OP_UNARY_RGBA(type,type_scalar) \
  OP_UNARY_RGBA_1(type,type_scalar)     \
  OP_UNARY_RGBA_2(type,type_scalar)     \
  OP_UNARY_RGBA_3(type,type_scalar)     \
  OP_UNARY_RGBA_4(type,type_scalar)     \
  OP_UNARY_RGBA_5(type,type_scalar)     \
  OP_UNARY_RGBA_6(type,type_scalar)     \
  OP_UNARY_RGBA_7(type,type_scalar)     \
  OP_UNARY_RGBA_8(type,type_scalar)     \
  OP_UNARY_RGBA_9(type,type_scalar)     \
  OP_UNARY_RGBA_10(type,type_scalar)    \
  OP_UNARY_RGBA_11(type,type_scalar)    \
  OP_UNARY_RGBA_12(type,type_scalar)    \
  OP_UNARY_RGBA_13(type,type_scalar)    \
  OP_UNARY_RGBA_14(type,type_scalar)    \
  OP_UNARY_RGBA_15(type,type_scalar)    \
  OP_UNARY_RGBA_16(type,type_scalar)    \
  OP_UNARY_RGBA_17(type,type_scalar)    \
  OP_UNARY_RGBA_18(type,type_scalar)    \
  OP_UNARY_RGBA_19(type,type_scalar)    \
  OP_UNARY_RGBA_20(type,type_scalar)    \
  OP_UNARY_RGBA_21(type,type_scalar)    \
  OP_UNARY_RGBA_22(type,type_scalar)    \
  OP_UNARY_RGBA_23(type,type_scalar)    \
  OP_UNARY_RGBA_24(type,type_scalar)    \
  OP_UNARY_RGBA_25(type,type_scalar)    \
  OP_UNARY_RGBA_26(type,type_scalar)    \
  OP_UNARY_RGBA_27(type,type_scalar)    \
  OP_UNARY_RGBA_28(type,type_scalar)    \
  OP_UNARY_RGBA_29(type,type_scalar)    \
  OP_UNARY_RGBA_30(type,type_scalar)    \
  OP_UNARY_RGBA_31(type,type_scalar)    \


/*
 * Unary operators
 */
#define OP_UNARY_HSV_5(type,type_scalar)  bool operator==(type src) { return(h == src.h && s == src.s && v == src.v); }
#define OP_UNARY_HSV_6(type,type_scalar)  bool operator!=(type src) { return(h != src.h || s != src.s || v != src.v); }

#define OP_UNARY_HSV_23(type,type_scalar) void set(void) { h = s = v = 0; };
#define OP_UNARY_HSV_24(type,type_scalar) void set(type_scalar src) { h = s = v = src; };
#define OP_UNARY_HSV_25(type,type_scalar) void set(type_scalar h_, type_scalar s_, type_scalar v_) { h = h_; s = s_; v = v_; };

#define OP_UNARY_HSV(type,type_scalar) \
  OP_UNARY_HSV_5(type,type_scalar)     \
  OP_UNARY_HSV_6(type,type_scalar)     \
  OP_UNARY_HSV_23(type,type_scalar)    \
  OP_UNARY_HSV_24(type,type_scalar)    \
  OP_UNARY_HSV_25(type,type_scalar)    \


/*
 * Binary operators
 */
#define OP_BINARY_RGBA_1(type,type_scalar)  bool operator<=(type src1, type src2) { return(src1.r <= src2.r && src1.g <= src2.g && src1.b <= src2.b && src1.a <= src2.a); }
#define OP_BINARY_RGBA_2(type,type_scalar)  bool operator>=(type src1, type src2) { return(src1.r >= src2.r && src1.g >= src2.g && src1.b >= src2.b && src1.a >= src2.a); }
#define OP_BINARY_RGBA_3(type,type_scalar)  bool operator< (type src1, type src2) { return(src1.r <  src2.r && src1.g <  src2.g && src1.b <  src2.b && src1.a <  src2.a); }
#define OP_BINARY_RGBA_4(type,type_scalar)  bool operator> (type src1, type src2) { return(src1.r >  src2.r && src1.g >  src2.g && src1.b >  src2.b && src1.a >  src2.a); }
#define OP_BINARY_RGBA_5(type,type_scalar)  bool operator==(type src1, type src2) { return(src1.r == src2.r && src1.g == src2.g && src1.b == src2.b && src1.a == src2.a); }
#define OP_BINARY_RGBA_6(type,type_scalar)  bool operator!=(type src1, type src2) { return(src1.r != src2.r || src1.g != src2.g || src1.b != src2.b || src1.a != src2.a); }
#define OP_BINARY_RGBA_7(type,type_scalar)  type operator+(type src1, type src2) { type tmp; tmp.r=src1.r+src2.r; tmp.g=src1.g+src2.g; tmp.b=src1.b+src2.b; tmp.a=src1.a+src2.a; tmp.norm_color(); return(tmp); }
#define OP_BINARY_RGBA_8(type,type_scalar)  type operator-(type src1, type src2) { type tmp; tmp.r=src1.r-src2.r; tmp.g=src1.g-src2.g; tmp.b=src1.b-src2.b; tmp.a=src1.a-src2.a; tmp.norm_color(); return(tmp); }
#define OP_BINARY_RGBA_9(type,type_scalar)  type operator*(type src1, type src2) { type tmp; tmp.r=src1.r*src2.r; tmp.g=src1.g*src2.g; tmp.b=src1.b*src2.b; tmp.a=src1.a*src2.a; tmp.norm_color(); return(tmp); }
#define OP_BINARY_RGBA_10(type,type_scalar) type operator/(type src1, type src2) { type tmp; tmp.r=src1.r/src2.r; tmp.g=src1.g/src2.g; tmp.b=src1.b/src2.b; tmp.a=src1.a/src2.a; tmp.norm_color(); return(tmp); }

#define OP_BINARY_RGBA(type,type_scalar) \
  OP_BINARY_RGBA_1(type,type_scalar)     \
  OP_BINARY_RGBA_2(type,type_scalar)     \
  OP_BINARY_RGBA_3(type,type_scalar)     \
  OP_BINARY_RGBA_4(type,type_scalar)     \
  OP_BINARY_RGBA_5(type,type_scalar)     \
  OP_BINARY_RGBA_6(type,type_scalar)     \
  OP_BINARY_RGBA_7(type,type_scalar)     \
  OP_BINARY_RGBA_8(type,type_scalar)     \
  OP_BINARY_RGBA_9(type,type_scalar)     \
  OP_BINARY_RGBA_10(type,type_scalar)    \

/*
  Color operation type
*/

/*
 * Describes color/other element blending operation
 */
typedef struct blend_operation
{
  ARITMETIC_OPERATOR  op;
  bool                blend;
  float               blend_factor;
  
public:  
  
  blend_operation(ARITMETIC_OPERATOR func = OPERATION_SET)
  {
    op = func;
    blend = FALSE;
    blend_factor = FLOAT_UNDEFINED;
  }
  
} BLEND_OPERATION;

/*
 * Blending operators
 */
#define OP_OPERATION(type, type_scalar)                                     \
                                                                            \
inline type blend(BLEND_OPERATION op, type dest, type src)                  \
{                                                                           \
  if(op.blend) {                                                            \
    float alpha_src = (op.blend_factor != FLOAT_UNDEFINED) ? op.blend_factor : src.alpha_get(); \
    float alpha_dest = 1.0f - alpha_src;                                    \
                                                                            \
    dest *= alpha_dest;                                                     \
    src *= alpha_src;                                                       \
  }                                                                         \
                                                                            \
  switch(op.op) {                                                           \
    case OPERATION_SET:                                                     \
      return(src);                                                          \
    case OPERATION_ADD:                                                     \
      return(dest+src);                                                     \
    case OPERATION_SUB:                                                     \
      return(dest-src);                                                     \
    case OPERATION_MODULATE:                                                \
      return(dest*src);                                                     \
    case OPERATION_MODULATE2X:                                              \
      return(dest*src*2.0f);                                                \
  }                                                                         \
  return(dest);                                                             \
}                                                                           \
                                                                            \
inline type interpolate(type src1, type src2, float t)                      \
{                                                                           \
  float t2 = 1.0f - t;                                                      \
  return(src1*t + src2*t2);                                                 \
}                                                                           \
inline type interpolate_cos(type src1, type src2, float t)                  \
{                                                                           \
  float f = (1.0f - cosf(t*PI))*0.5f;                                       \
  float f2 = 1.0f - f;                                                      \
  return(src1*f + src2*f2);                                                 \
}

inline float blend(BLEND_OPERATION op, float dest, float src)
{
  if(op.blend && op.blend_factor != FLOAT_UNDEFINED) {
    float alpha_src = op.blend_factor;
    float alpha_dest = 1.0f - alpha_src;

    dest *= alpha_dest;
    src *= alpha_src;
  }
  
  switch(op.op) {
    case OPERATION_SET:
      return(src);
    case OPERATION_ADD:
      return(dest+src);
    case OPERATION_SUB:
      return(dest-src);
    case OPERATION_MODULATE:
      return(dest*src);
    case OPERATION_MODULATE2X:
      return(dest*(src*2.0f));
  }
  return(dest);
}

#endif //__COLOR_OPERATORS_H__
