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
 * Basic Ferguson splines implementation
 */

#ifndef __FERGUSON_H__
#define __FERGUSON_H__

typedef enum {

  CLASS_IN  = 1,      // a: incoming case
  CLASS_OUT = -1      // b: outgoing case
  
} FERGUSON_KEY_CLASS,
  BEZIER_KEY_CLASS;

template<class vect> vect ferguson_tangent(vect p0, vect p1, vect p2)
{
  vect g1 = p1 - p0;
  vect g2 = p2 - p1;
  vect g3 = g2 - g1;
  return(g1 + g3*0.5f);
}

template<class vect> vect ferguson_tangent(vect p0, vect p1, vect p2,
                                           FERGUSON_KEY_CLASS key_class,
                                           float bias,
                                           float continuity,
                                           float tension)
{
  vect g1 = (p1 - p0)*(1.0f+bias);
  vect g2 = (p2 - p1)*(1.0f-bias);
  vect g3 = g2 - g1;
  return(g1 + g3*(0.5f + key_class*0.5f*continuity))*(1.0f-tension);
}

template<class vect> vect ferguson_interpolate(vect v0, vect p0,
                                               vect v1, vect p1,
                                               float t)
{
  if(t < 0) {
    return(p0);
  } else if(t > 1) {
    return(p1);
  } else {  
    float t2 = t*t,
          t3 = t2*t;
    
    float F1 = 2*t3-3*t2+1;
    float F2 = t3-2*t2+t;
    float F3 = -2*t3+3*t2;
    float F4 = t3-t2;
    
    return(p0*F1 + v0*F2 + p1*F3 + v1*F4);
  }
}

#endif // __FERGUSON_H__
