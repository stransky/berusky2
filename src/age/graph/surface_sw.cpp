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

// Constants and types
#include "age.h"

void surface_sw::create(tpos width, tpos height, SURFACE_FORMAT format)
{ 
  clear();
    
  p_pixels = (tcolor *)mmalloc(sizeof(tcolor)*width*height);
  this->width = width;
  this->height = height;
}

void surface_sw::create(SURFACE *p_surf, SURFACE_FORMAT format)
{ 
  if(p_surf->pixel_format_get() != PIXEL_TCOLOR)
    return;
  
  if(width != p_surf->width_get() || 
     height != p_surf->height_get()) 
  {
    clear();
    width = p_surf->width_get();
    height = p_surf->height_get();
    p_pixels = (tcolor *)mmalloc(sizeof(tcolor)*width*height);
  }
  
  int y;
  for(y = 0; y < height; y++) {
    tcolor *p_src = (tcolor *)p_surf->pixels_get(0,y);
    tcolor *p_dest = (tcolor *)pixels_get(0,y);
    memcpy(p_dest,p_src,sizeof(tcolor)*width);
  }
}

bool surface_sw::load(const char *p_file, SURFACE_FORMAT format)
{    
  SURFACE_SDL surf;
  
  if(!surf.load(p_file,SURFACE_TEXTURE))
    return(FALSE);
  
  create(surf.width_get(), surf.height_get());  
  tcolor *p_src = (tcolor *)surf.pixels_get();
  tcolor *p_dest = p_pixels;

  assert(p_src && p_dest);
  
  int y;
  for(y = 0; y < height; y++) {
    memcpy(p_dest,p_src,sizeof(tcolor)*width);
    p_dest += width;
    p_src += surf.pitch_get();
  }
  
  return(TRUE);
}

bool surface_sw::load_alpha(const char *p_file)
{
  return(FALSE);
}

bool surface_sw::save(const char *p_file, IMAGE_FORMAT format)
{ 
  SURFACE_SDL tmp(width,height);
  if(tmp.pixels_set(p_pixels,width,height)) {
    return(tmp.save(p_file));
  }
  return(FALSE);
}

void surface_sw::copy(class surface_sw *p_src)
{
  clear();
  
  // Source surface_sw is valid - copy pixels
  if(p_src->p_pixels) {
    create(p_src->width_get(), p_src->height_get());
    assert(p_pixels);
    memcpy(p_pixels, p_src->pixels_get(), sizeof(tcolor)*width*height);
  } else {
    *this = *p_src;
  }
}

void surface_sw::move(class surface_sw *p_src)
{
  clear();
  
  *this = *p_src;
  
  p_src->p_pixels = NULL;
}

void surface_sw::clear(void)
{
  assert(ref_get() == 0);

  ffree(p_pixels);
  width = 0;
  height = 0;
}

surface_sw::surface_sw(PIXEL_FORMAT format)
: surface(SW_SURFACE, format),
  p_pixels(NULL),
  width(0),
  height(0)
{
}

surface_sw::surface_sw(char *p_file, PIXEL_FORMAT format)
: surface(SW_SURFACE, format),
  p_pixels(NULL),
  width(0),
  height(0)
{
  load(p_file);
}

surface_sw::surface_sw(tpos width, tpos height, PIXEL_FORMAT format)
: surface(SW_SURFACE, format),
  p_pixels(NULL),
  width(0),
  height(0)
{
  create(width,height);
}

surface_sw::~surface_sw(void)
{
  clear();
}

void surface_sw::fill(tcolor color)
{ 
  if(loaded()) {
    memset(p_pixels, color, sizeof(color)*width*height);
  }
}

void surface_sw::fill(tpos sx, tpos sy, tpos dx, tpos dy, tcolor color)
{ 
  if(loaded()) {
    int y;
    for(y = 0; y < dy; y++) {
      void *p_pixels = pixels_get(sx,sy+y);
      memset(p_pixels, color, sizeof(color)*dx);
    }
  }
}

// blit whole source surtace to destination surface_sw
void surface_sw::blit(class surface_sw *p_dst, tpos tx, tpos ty)
{
  blit(0, 0, width, height, p_dst, tx, ty);
}

// blit part of source surface_sw to destination surface_sw
void surface_sw::blit(tpos sx, tpos sy, tpos dx, tpos dy, class surface_sw *p_dst, tpos tx, tpos ty)
{
  if(!loaded() || !p_dst->loaded())
    return;
    
  int y;
  for(y = 0; y < dy; y++) {
    void *p_pixels_src = pixels_get(sx,sy+y);
    void *p_pixels_dst = p_dst->pixels_get(tx,ty+y);
    memcpy(p_pixels_dst, p_pixels_src, sizeof(tcolor)*dx);
  }
}

void surface_sw::scale(int nx, int ny)
{
  // unimplemented
  assert(0);
}

void surface_sw::pixel_set_complex(int x, int y, RGBB color)
{
  RGBB target = pixel_get(x,y);
  target = blend(op,(RGBF)target,(RGBF)color);
  pixel_set_simple(x,y,color);
}

#define TEST_AND_SET(x,y,cx,cy)       \
{                                     \
  if(pixel_valid(x,y)) {              \
    pixels++;                         \
    if(pixel_get(x,y) == color) {     \
      VECT2DI tmp = VECT2DI(x,y)-VECT2DI(cx,cy);  \
      float act = tmp.size();         \
      if(act < distance_best) {       \
        distance_best = act;          \
        px = x; py = y;               \
      }                               \
    }                                 \
  }                                   \
}

// distance = current distance from given point
// returns if any pixel has been hit (so we can continue to search)
bool surface_sw::pixel_search_square(int x, int y,
                                     tcolor color, int distance,
                                     int &px, int &py,
                                     float &distance_best)
{  
  int pixels = 0;
  if(distance == 0) {
    if(pixel_valid(x,y)) {
      pixels++;
      if(pixel_get(x,y) == color) {
        distance_best = 0;
        px = x; py = y;          
      }
    }  
  }
  else {
    int tx, ty;
    // search rows
    for(tx = x-distance; tx <= x+distance; tx++) {
      ty = y-distance;
      TEST_AND_SET(tx,ty,x,y);
      ty = y+distance;
      TEST_AND_SET(tx,ty,x,y);
    }
    // search columns
    for(ty = y-distance-1; ty <= y+distance-1; ty++) {
      tx = x-distance;
      TEST_AND_SET(tx,ty,x,y);
      tx = x+distance;
      TEST_AND_SET(tx,ty,x,y);
    }
  }
  return(pixels);
}

// max_dist is maximal pixel distance
bool surface_sw::pixel_search(int x, int y, 
                              tcolor color, int range,
                              int &px, int &py, 
                              float &distance_best)
{
  distance_best = FLT_MAX;
  
  for(int distance = 0; distance <= range; distance++) {
    if(!pixel_search_square(x, y, color, distance, px, py, distance_best))
      break;
    // optimalization - sqrt(8) < 3
    if(distance < 3 && distance_best != FLT_MAX) {
      break;
    }    
  }
  
  if(distance_best != FLT_MAX && distance_best <= range) {
    distance_best = distance_best / range;
    return(TRUE);
  }
  
  return(FALSE);
}
