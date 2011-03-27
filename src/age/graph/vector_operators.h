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

#ifndef __VECTOR_OPERATORS_H__
#define __VECTOR_OPERATORS_H__

/*--------------------------------------------------------------------------
   Vector operators
  --------------------------------------------------------------------------
*/

#define OPERATORS3D(type,type_scalar)       \
                                            \
void set(type_scalar value = 0)             \
{                                           \
  x = value;                                \
  y = value;                                \
  z = value;                                \
}                                           \
void set(type_scalar vx, type_scalar vy, type_scalar vz) \
{                                           \
  x = vx;                                   \
  y = vy;                                   \
  z = vz;                                   \
}                                           \
void set(type & src)                        \
{                                           \
  *this = src;                              \
}                                           \
void set(type *p_src)                       \
{                                           \
  *this = *p_src;                           \
}                                           \
void operator+=(type & src)                 \
{                                           \
  x += src.x;                               \
  y += src.y;                               \
  z += src.z;                               \
}                                           \
void operator+=(type_scalar src)            \
{                                           \
  x += src;                                 \
  y += src;                                 \
  z += src;                                 \
}                                           \
void operator-=(type & src)                 \
{                                           \
  x -= src.x;                               \
  y -= src.y;                               \
  z -= src.z;                               \
}                                           \
void operator-=(type_scalar src)            \
{                                           \
  x -= src;                                 \
  y -= src;                                 \
  z -= src;                                 \
}                                           \
void operator*=(type & src)                 \
{                                           \
  x *= src.x;                               \
  y *= src.y;                               \
  z *= src.z;                               \
}                                           \
void operator*=(type_scalar src)            \
{                                           \
  x *= src;                                 \
  y *= src;                                 \
  z *= src;                                 \
}                                           \
void operator/=(type & src)                 \
{                                           \
  x /= src.x;                               \
  y /= src.y;                               \
  z /= src.z;                               \
}                                           \
type operator/(type & src)                  \
{                                           \
  type tmp;                                 \
  tmp.x = x/src.x;                          \
  tmp.y = y/src.y;                          \
  tmp.z = z/src.z;                          \
  return(tmp);                              \
}                                           \
void operator/=(type_scalar src)            \
{                                           \
  x /= src;                                 \
  y /= src;                                 \
  z /= src;                                 \
}                                           \
type operator/(type_scalar src)             \
{                                           \
  type tmp;                                 \
  tmp.x = x/src;                            \
  tmp.y = y/src;                            \
  tmp.z = z/src;                            \
  return(tmp);                              \
}                                           \
bool operator==(type & src)                 \
{                                           \
  return(x == src.x && y == src.y && z == src.z); \
}                                           \
type_scalar dot_product(type & src)         \
{                                           \
  return(x*src.x+y*src.y+z*src.z);          \
}                                           \
float size(void)                            \
{                                           \
  float dis = x*x+y*y+z*z;                  \
  if(dis > 0)                               \
    return(sqrtf(dis));                     \
  else                                      \
    return(0);                              \
}                                           \
float norm(type_scalar new_size = 1)        \
{                                           \
  float vel2,vel = size();                  \
  if(vel == 0)                              \
    return(0);                              \
  else {                                    \
    vel2 = new_size/vel;                    \
    x = (type_scalar)(x * vel2);            \
    y = (type_scalar)(y * vel2);            \
    z = (type_scalar)(z * vel2);            \
    return(vel);                            \
  }                                         \
}                                           \
void invert(void)                           \
{                                           \
  x = -x;                                   \
  y = -y;                                   \
  z = -z;                                   \
}                                           \
void vect_mult(type & v)                    \
{                                           \
  type c;                                   \
  c.x = y*v.z - z*v.y;                      \
  c.y = z*v.x - x*v.z;                      \
  c.z = x*v.y - y*v.x;                      \
  *this = c;                                \
}                                           \
bool zero(void)                             \
{                                           \
  return(x*x+y*y+z*z == 0);                 \
}                                           \
void rotate_x(float angle)                  \
{                                           \
  float tz = (float)z;                      \
  float ty = (float)y;                      \
                                            \
  float cosa = cosf(angle);                 \
  float sina = sinf(angle);                 \
                                            \
  z = (type_scalar)(tz*cosa - ty*sina);     \
  y = (type_scalar)(tz*sina + ty*cosa);     \
}                                           \
void rotate_y(float angle)                  \
{                                           \
  float tx = (float)x;                      \
  float tz = (float)z;                      \
                                            \
  float cosa = cosf(angle);                 \
  float sina = sinf(angle);                 \
                                            \
  x = (type_scalar)(tx*cosa - tz*sina);     \
  z = (type_scalar)(tx*sina + tz*cosa);     \
}                                           \
void rotate_z(float angle)                  \
{                                           \
  float tx = (float)x;                      \
  float ty = (float)y;                      \
                                            \
  float cosa = cosf(angle);                 \
  float sina = sinf(angle);                 \
                                            \
  x = (type_scalar)(tx*cosa - ty*sina);     \
  y = (type_scalar)(tx*sina + ty*cosa);     \
}                                           \

#define METHODS3D(type,type_scalar)         \
                                            \
inline type_scalar vect_dot_product(type & src1, type & src2) \
{                                                             \
  return(src1.x*src2.x+src1.y*src2.y+src1.z*src2.z);          \
}                                                             \
inline type_scalar vect_dot_product(type *src1, type *src2)   \
{                                                             \
  return(src1->x*src2->x+src1->y*src2->y+src1->z*src2->z);    \
}                                                             \
inline float vect_size(type_scalar x, type_scalar y, type_scalar z) \
{                                           \
  float dis = (x)*(x)+(y)*(y)+(z)*(z);      \
  if(dis > 0)                               \
    return(sqrtf(dis));                     \
  else                                      \
    return(0);                              \
}                                           \
inline float vect_norm(type_scalar *p_x, type_scalar *p_y, type_scalar *p_z)  \
{                                                                             \
  float vel2,vel = vect_size(*p_x,*p_y,*p_z);                                 \
  if(vel == 0)                                                                \
    return(0.0f);                                                             \
  else {                                                                      \
    vel2 = 1.0f/vel;                                                          \
    *p_x = (type_scalar)((*p_x)*vel2);                                        \
    *p_y = (type_scalar)((*p_y)*vel2);                                        \
    *p_z = (type_scalar)((*p_z)*vel2);                                        \
    return(vel);                                                              \
  }                                                                           \
}                                                                             \
inline type vect_mult(type &u, type &v)   \
{                                         \
  type c;                                 \
  c.x = u.y*v.z - u.z*v.y;                \
  c.y = u.z*v.x - u.x*v.z;                \
  c.z = u.x*v.y - u.y*v.x;                \
  return(c);                              \
}                                         \
inline type * vect_mult(type *u, type *v, type *c) \
{                                         \
  c->x = u->y*v->z - u->z*v->y;           \
  c->y = u->z*v->x - u->x*v->z;           \
  c->z = u->x*v->y - u->y*v->x;           \
  return(c);                              \
}                                         \
inline float point_dist(type a, type b)                                   \
{                                                                         \
  float v = (b.x-a.x)*(b.x-a.x)+(b.y-a.y)*(b.y-a.y)+(b.z-a.z)*(b.z-a.z);  \
  return((v > 0.0f) ? (float)sqrtf(v) : v);                               \
}                                                                         \
inline float point_dist(type *a, type *b)                                 \
{                                                                         \
  float v = (b->x-a->x)*(b->x-a->x)+(b->y-a->y)*(b->y-a->y)+(b->z-a->z)*(b->z-a->z);  \
  return((v > 0.0f) ? (float)sqrtf(v) : v);                               \
}                                                                         \
inline type interpolate(type src1, type src2, float i)                    \
{                                                                         \
  type tmp;                                                               \
  float i2 = 1.0f-i;                                                      \
  tmp.x = (type_scalar)(src1.x*i+src2.x*i2);                              \
  tmp.y = (type_scalar)(src1.y*i+src2.y*i2);                              \
  tmp.z = (type_scalar)(src1.z*i+src2.z*i2);                              \
  return(tmp);                                                            \
}                                                                         \
inline type operator+(type & src1, type & src2)    \
{                                           \
  type tmp;                                 \
  tmp.x = src1.x+src2.x;                    \
  tmp.y = src1.y+src2.y;                    \
  tmp.z = src1.z+src2.z;                    \
  return(tmp);                              \
}                                           \
inline type operator+(type & src1, type_scalar src2)\
{                                           \
  type tmp;                                 \
  tmp.x = src1.x+src2;                      \
  tmp.y = src1.y+src2;                      \
  tmp.z = src1.z+src2;                      \
  return(tmp);                              \
}                                           \
inline type operator-(type & src1, type & src2)    \
{                                           \
  type tmp;                                 \
  tmp.x = src1.x-src2.x;                    \
  tmp.y = src1.y-src2.y;                    \
  tmp.z = src1.z-src2.z;                    \
  return(tmp);                              \
}                                           \
inline type operator-(type & src1, type_scalar src2)\
{                                           \
  type tmp;                                 \
  tmp.x = src1.x-src2;                      \
  tmp.y = src1.y-src2;                      \
  tmp.z = src1.z-src2;                      \
  return(tmp);                              \
}                                           \
inline type operator*(type & src1, type & src2)    \
{                                           \
  type tmp;                                 \
  tmp.x = src1.x*src2.x;                    \
  tmp.y = src1.y*src2.y;                    \
  tmp.z = src1.z*src2.z;                    \
  return(tmp);                              \
}                                           \
inline type operator*(type & src1, type_scalar src2)\
{                                           \
  type tmp;                                 \
  tmp.x = src1.x*src2;                      \
  tmp.y = src1.y*src2;                      \
  tmp.z = src1.z*src2;                      \
  return(tmp);                              \
}                                           \


#define OPERATORS2D(type,type_scalar)       \
                                            \
void set(type_scalar value = 0)             \
{                                           \
  x = value;                                \
  y = value;                                \
}                                           \
void set(type_scalar vx, type_scalar vy)    \
{                                           \
  x = vx;                                   \
  y = vy;                                   \
}                                           \
void set(type & src)                        \
{                                           \
  *this = src;                              \
}                                           \
void set(type *p_src)                       \
{                                           \
  *this = *p_src;                           \
}                                           \
void operator+=(type & src)                 \
{                                           \
  x += src.x;                               \
  y += src.y;                               \
}                                           \
void operator+=(type_scalar src)            \
{                                           \
  x += src;                                 \
  y += src;                                 \
}                                           \
void operator-=(type & src)                 \
{                                           \
  x -= src.x;                               \
  y -= src.y;                               \
}                                           \
void operator-=(type_scalar src)            \
{                                           \
  x -= src;                                 \
  y -= src;                                 \
}                                           \
type operator-(void)                        \
{                                           \
  type tmp;                                 \
  tmp.x = -x;                               \
  tmp.y = -y;                               \
  return(tmp);                              \
}                                           \
void operator*=(type & src)                 \
{                                           \
  x *= src.x;                               \
  y *= src.y;                               \
}                                           \
void operator*=(type_scalar src)            \
{                                           \
  x *= src;                                 \
  y *= src;                                 \
}                                           \
void operator/=(type & src)                 \
{                                           \
  x /= src.x;                               \
  y /= src.y;                               \
}                                           \
type operator/(type & src)                  \
{                                           \
  type tmp;                                 \
  tmp.x = x/src.x;                          \
  tmp.y = y/src.y;                          \
  return(tmp);                              \
}                                           \
void operator/=(type_scalar src)            \
{                                           \
  x /= src;                                 \
  y /= src;                                 \
}                                           \
type operator/(type_scalar src)             \
{                                           \
  type tmp;                                 \
  tmp.x = x/src;                            \
  tmp.y = y/src;                            \
  return(tmp);                              \
}                                           \
bool operator==(type & src)                 \
{                                           \
  return(x == src.x && y == src.y);         \
}                                           \
type_scalar dot_product(type & src)         \
{                                           \
  return(x*src.x+y*src.y);                  \
}                                           \
float size(void)                            \
{                                           \
  float dis = x*x+y*y;                      \
  if(dis > 0.0f)                            \
    return(sqrtf(dis));                     \
  else                                      \
    return(0.0f);                           \
}                                           \
float norm(type_scalar new_size = 1)        \
{                                           \
  float vel2, vel = size();                 \
  if(vel == 0.0f)                           \
    return(0.0f);                           \
  else {                                    \
    vel2 = (float)new_size/vel;             \
    x = (type_scalar)(x * vel2);            \
    y = (type_scalar)(y * vel2);            \
    return(vel);                            \
  }                                         \
}                                           \
void invert(void)                           \
{                                           \
  x = -x;                                   \
  y = -y;                                   \
}                                           \
void swap(void)                             \
{                                           \
  type_scalar t = x;                        \
  x = y;                                    \
  y = t;                                    \
}                                           \
type_scalar vect_mult(type & v)             \
{                                           \
  return(x*v.y - v.x*y);                    \
}                                           \
void rotate(float angle)                    \
{                                           \
  float tx = (float)x;                      \
  float ty = (float)y;                      \
                                            \
  float cosa = cosf(angle);                 \
  float sina = sinf(angle);                 \
                                            \
  x = (type_scalar)(tx*cosa - ty*sina);     \
  y = (type_scalar)(tx*sina + ty*cosa);     \
}                                           \
bool zero(void)                             \
{                                           \
  return(x*x+y*y == 0);                     \
}                                           \

#define METHODS2D(type,type_scalar)                                       \
                                                                          \
inline type_scalar vect_dot_product(type & src1, type & src2)             \
{                                                                         \
  return(src1.x*src2.x+src1.y*src2.y);                                    \
}                                                                         \
inline type_scalar vect_dot_product(type *src1, type *src2)               \
{                                                                         \
  return(src1->x*src2->x+src1->y*src2->y);                                \
}                                                                         \
inline float vect_size(type_scalar x, type_scalar y)                      \
{                                                                         \
  float dis = (x)*(x)+(y)*(y);                                            \
  if(dis > 0)                                                             \
    return(sqrtf(dis));                                                   \
  else                                                                    \
    return(0);                                                            \
}                                                                         \
inline float vect_norm(type_scalar *p_x, type_scalar *p_y)                \
{                                                                         \
  float vel2,vel = vect_size(*p_x,*p_y);                                  \
  if(vel == 0)                                                            \
    return(0.0f);                                                         \
  else {                                                                  \
    vel2 = 1.0f/vel;                                                      \
    *p_x = (type_scalar)((*p_x)*vel2);                                    \
    *p_y = (type_scalar)((*p_y)*vel2);                                    \
    return(vel);                                                          \
  }                                                                       \
}                                                                         \
inline type_scalar vect_mult(type &u, type &v)                            \
{                                                                         \
  return(u.x*v.y - v.x*u.y);                                              \
}                                                                         \
inline type_scalar vect_mult(type *u, type *v)                            \
{                                                                         \
  return(u->x*v->y - v->x*u->y);                                          \
}                                                                         \
inline float point_dist(type a, type b)                                   \
{                                                                         \
  float v = (b.x-a.x)*(b.x-a.x)+(b.y-a.y)*(b.y-a.y);                      \
  return((v > 0.0f) ? (float)sqrtf(v) : v);                               \
}                                                                         \
inline float point_dist(type *a, type *b)                                 \
{                                                                         \
  float v = (b->x-a->x)*(b->x-a->x)+(b->y-a->y)*(b->y-a->y);              \
  return((v > 0.0f) ? (float)sqrtf(v) : v);                               \
}                                                                         \
inline type operator+(type src1, type src2)                               \
{                                                                         \
  type tmp;                                                               \
  tmp.x = src1.x+src2.x;                                                  \
  tmp.y = src1.y+src2.y;                                                  \
  return(tmp);                                                            \
}                                                                         \
inline type operator+(type src1, type_scalar src2)                        \
{                                                                         \
  type tmp;                                                               \
  tmp.x = src1.x+src2;                                                    \
  tmp.y = src1.y+src2;                                                    \
  return(tmp);                                                            \
}                                                                         \
inline type operator-(type src1, type src2)                               \
{                                                                         \
  type tmp;                                                               \
  tmp.x = src1.x-src2.x;                                                  \
  tmp.y = src1.y-src2.y;                                                  \
  return(tmp);                                                            \
}                                                                         \
inline type operator*(type src1, type src2)                               \
{                                                                         \
  type tmp;                                                               \
  tmp.x = src1.x*src2.x;                                                  \
  tmp.y = src1.y*src2.y;                                                  \
  return(tmp);                                                            \
}                                                                         \
inline type operator*(type src1, type_scalar src2)                        \
{                                                                         \
  type tmp;                                                               \
  tmp.x = src1.x*src2;                                                    \
  tmp.y = src1.y*src2;                                                    \
  return(tmp);                                                            \
}                                                                         \
inline type interpolate(type src1, type src2, float i)                    \
{                                                                         \
  type tmp;                                                               \
  float i2 = 1.0f-i;                                                      \
  tmp.x = (type_scalar)(src1.x*i+src2.x*i2);                              \
  tmp.y = (type_scalar)(src1.y*i+src2.y*i2);                              \
  return(tmp);                                                            \
}                                                                         \
inline type interpolate(type src1, type src2, float i1, float i2)         \
{                                                                         \
  type tmp;                                                               \
  float i11 = 1.0f-i1;                                                    \
  float i22 = 1.0f-i2;                                                    \
  tmp.x = (type_scalar)(src1.x*i1+src2.x*i11);                            \
  tmp.y = (type_scalar)(src1.y*i2+src2.y*i22);                            \
  return(tmp);                                                            \
}                                                                         \
inline float line_dist(type a, type b, type p)                            \
{                                                                         \
  float lenght = point_dist(a,b);                                         \
  float t = ((b.x - a.x)*(p.x-a.x)+(b.y - a.y)*(p.y-a.y))/(lenght*lenght); \
  return(t);                                                              \
}                                                                         \
inline bool line_dist(type a, type b, type p, float &t, float &dist)      \
{                                                                         \
  float lenght = point_dist(a,b);                                         \
  t = ((b.x - a.x)*(p.x - a.x)+(b.y - a.y)*(p.y - a.y))/(lenght*lenght);  \
  dist = fabs((b.x - a.x)*(p.y-a.y)-(b.y - a.y)*(p.x-a.x))/lenght;        \
  return(t >= 0 || t <= 1.0f);                                            \
}                                                                         \
inline bool line_dist(type a, type b, type p, type &in, float &t, float &dist) \
{                                                                         \
  float lenght = point_dist(a,b);                                         \
  t = ((b.x - a.x)*(p.x - a.x)+(b.y - a.y)*(p.y - a.y))/(lenght*lenght);  \
  dist = fabs((b.x - a.x)*(p.y-a.y)-(b.y - a.y)*(p.x-a.x))/lenght;        \
  in.x = (b.x - a.x)*t;                                                   \
  in.y = (b.y - a.y)*t;                                                   \
  return(t >= 0 || t <= 1.0f);                                            \
}

/*--------------------------------------------------------------------------
   Other functions
  --------------------------------------------------------------------------
*/

#define MATH_HELPERS(type_scalar)                                           \
                                                                            \
inline type_scalar interpolate(type_scalar v1, type_scalar v2, float t)     \
{                                                                           \
  return(v1*(1-t) + v2*t);                                                  \
}                                                                           \
inline type_scalar interpolate_cos(type_scalar v1, type_scalar v2, float t) \
{                                                                           \
	float f = (1.0f - cosf(t*PI))*0.5f;	                                      \
  return(v1*(1-f) + v2*f);                                                  \
}

#endif //__VECTOR_OPERATORS_H__
