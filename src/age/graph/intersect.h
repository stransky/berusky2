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
 
#ifndef __INTERSECT_H__
#define __INTERSECT_H__
 
/*
 * Basic intersection routines
 * 
 * Functions names:
 *
 * intersect_(object)_(object)
 *
 * objects are ordered by number of their points
 *
 * objects:
 *
 *  - ray       - two points (origin+direction)
 *  - line      - two points
 *  - triangle
 *  - rectangle
 */

/*
 * Line/Ray and point p intersection
 */
bool intersect_ray_point(VECT *p_a, VECT *p_b, VECT *p_p, VECT *p_I, float *p_ti = NULL);
bool intersect_ray_point(VECT2DI *p_a, VECT2DI *p_b, VECT2DI *p_p, VECT2DI *p_I, float *p_ti = NULL);
bool intersect_line_point(VECT *p_a, VECT *p_b, VECT *p_p, VECT *p_I, float *p_ti = NULL);
bool intersect_line_point(VECT2DI *p_a, VECT2DI *p_b, VECT2DI *p_p, VECT2DI *p_I, float *p_ti = NULL);

/*
 * Line/Ray and Line/Ray intersection
 */
bool intersect_ray_ray(VECT2DI *p_a1, VECT2DI *p_b1, VECT2DI *p_a2, VECT2DI *p_b2, VECT2DI *p_p = NULL, float *p_t1 = NULL, float *p_t2 = NULL);
bool intersect_ray_line(VECT2DI *p_a1, VECT2DI *p_b1, VECT2DI *p_a2, VECT2DI *p_b2, VECT2DI *p_p = NULL, float *p_t = NULL);
bool intersect_line_line(VECT2DI *p_a1, VECT2DI *p_b1, VECT2DI *p_a2, VECT2DI *p_b2, VECT2DI *p_p = NULL);

/*
 * Triangle intersection
 */
bool intersect_triangle_point(VECT2DI *p_v0, VECT2DI *p_v1, VECT2DI *p_v2, VECT2DI *p_p);
bool intersect_triangle_ray(VECT2DI *p_p0, VECT2DI *p_p1, VECT2DI *p_p2, VECT2DI *p_a, VECT2DI *p_b, float *p_tn = NULL);
bool intersect_triangle_line(VECT2DI *p_v0, VECT2DI *p_v1, VECT2DI *p_v2, VECT2DI *p_a, VECT2DI *p_b, float *p_t1 = NULL, float *p_t2 = NULL);

bool intersect_triangle_ray_cull(VECT *p_v0, VECT *p_v1, VECT *p_v2, VECT *p_orig, VECT *p_dir, float *p_t = NULL, float *p_u = NULL, float *p_v = NULL);
bool intersect_triangle_ray_ncull(VECT *p_v0, VECT *p_v1, VECT *p_v2, VECT *p_orig, VECT *p_dir, float *p_t = NULL, float *p_u = NULL, float *p_v = NULL);
bool intersect_triangle_line_cull(VECT *p_v0, VECT *p_v1, VECT *p_v2, VECT *p_orig, VECT *p_dir, float *p_t = NULL, float *p_u = NULL, float *p_v = NULL);
bool intersect_triangle_line_ncull(VECT *p_v0, VECT *p_v1, VECT *p_v2, VECT *p_orig, VECT *p_dir, float *p_t = NULL, float *p_u = NULL, float *p_v = NULL);

#define intersect_triangle_line intersect_triangle_line_cull

/*
 * Rect and Point/Line/Ray intersection
 */

typedef enum {

  INTERSECT_NONE = 0,
  INTERSECT_CROSS,
  INTERSECT_INSIDE
  
} INTERSECTION_TYPE;

inline int intersect_rect_point(RECT2DI *p_r, VECT2DI *p_p)
{
  return((p_p->x >= p_r->x && p_p->x <= p_r->x+p_r->dx && 
         p_p->y >= p_r->y && p_p->y <= p_r->y+p_r->dy) ? 1 : 0);
}

INTERSECTION_TYPE intersect_rect_ray(RECT2DI *p_r, VECT2DI *p_a, VECT2DI *p_b, float *p_tn = NULL, float *p_tf = NULL);
INTERSECTION_TYPE intersect_rect_line(RECT2DI *p_r, VECT2DI *p_a, VECT2DI *p_b, float *p_tn = NULL, float *p_tf = NULL);
INTERSECTION_TYPE intersect_rect_triangle(RECT2DI *p_r, VECT2DI *p_v0, VECT2DI *p_v1, VECT2DI *p_v2);
INTERSECTION_TYPE intersect_rect_rect(RECT2DI *p_r, RECT2DI *p_r2);

#endif // __INTERSECT_H__
