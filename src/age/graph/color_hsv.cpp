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

#include "age.h"

/*
 * RGB <-> HSV translations
 * some code is taken from gtk2 (gtkhsv.c file)
 */

/* Utility functions */
#define INTENSITY(r, g, b) ((r) * 299f + (g) * 0.587f + (b) * 0.114f)

/* Converts from HSV to RGB */
void hsv_to_rgb(float hue, float saturation, float value,
                float &r, float &g, float &b)
{
  float f, p, q, t;

  if(saturation == 0) {
    r = value;
    g = value;
    b = value; /* heh */
  }
  else {
    hue *= 6;
    
    if(hue == 6)
      hue = 0;
  
    f = hue - (int) hue;
    p = value * (1 - saturation);
    q = value * (1 - saturation * f);
    t = value * (1 - saturation * (1.0 - f));

    switch ((int) hue)  {
    case 0:
      r = value;
      g = t;
      b = p;
      break;
    
    case 1:
      r = q;
      g = value;
      b = p;
      break;
    
    case 2:
      r = p;
      g = value;
      b = t;
      break;
    
    case 3:
      r = p;
      g = q;
      b = value;
      break;
    
    case 4:
      r = t;
      g = p;
      b = value;
      break;
    
    case 5:
      r = value;
      g = p;
      b = q;
      break;
    }
  }
}

/* Converts from RGB to HSV */
void rgb_to_hsv(float red, float green, float blue,
                float &h, float &s, float &v)
{  
  h = 0;

  float max = MAX3(red,green,blue);
  float min = MIN3(red,green,blue);
  
  v = max;

  if(max != 0.0) {
    s = (max - min) / max;
  } else {
    s = 0;
  }

  if(s == 0) {
    h = 0;
  } else {
    float delta = max - min;
    
    if(red == max)
      h = (green - blue) / delta;
    else if(green == max)
      h = 2 + (blue - red) / delta;
    else if(blue == max)
      h = 4 + (red - green) / delta;
    
    h /= 6;
    
    if(h < 0)
      h += 1;
    else if(h > 1)
      h -= 1;
  }
}
